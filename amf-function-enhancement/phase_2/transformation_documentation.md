# Phase 2: Authentication Response → Security Mode Command Transformation

## Overview
This document details how open5gs AMF transforms an Authentication Response message into a Security Mode Command message.

## Input Message: Authentication Response
- **Message Type**: 0x57 (Authentication Response)
- **Security**: Plain/unprotected NAS message
- **Key Fields**:
  - RES* (16 bytes): ef:27:70:c6:9e:73:82:aa:38:e8:13:4f:60:22:34:e1

## Output Message: Security Mode Command
- **Message Type**: 0x5d (Security Mode Command)
- **Security**: Integrity protected with new security context
- **Key Fields**:
  - Security header type: 3
  - MAC: 0x13bf995a (4 bytes)
  - Sequence number: 0
  - Selected algorithms: 5G-EA0 (null encryption), 5G-IA2 (128-bit AES)
  - ngKSI: 0

## Network Functions Involved

### 1. AMF (Access and Mobility Management Function)
- Receives and processes authentication response
- Verifies RES* against expected HXRES*
- Derives security keys (KAMF, NAS keys)
- Selects security algorithms
- Builds and sends security mode command

### 2. AUSF (Authentication Server Function)
- Confirms authentication result
- Provides KSEAF to AMF upon successful verification
- Returns authenticated SUPI

### 3. UDM (Unified Data Management)
- Provides authentication confirmation to AUSF
- Maintains authentication status

## Detailed Function Call Flow

### Step 1: Authentication Response Reception
**File**: `src/amf/gmm-handler.c`
**Function**: `gmm_handle_authentication_response()` (line 924)

```c
// Extract RES* from authentication response
authentication_response_parameter = &authentication_response->
    authentication_response_parameter;
res_star_len = authentication_response_parameter->length;
res_star = authentication_response_parameter->res_star;

// Calculate HXRES* from received RES*
ogs_kdf_hxres_star(
    amf_ue->rand,           // RAND from authentication request
    res_star, res_star_len, // Received RES*
    hxres_star              // Output: calculated HXRES*
);

// Compare with stored HXRES*
if (memcmp(hxres_star, amf_ue->hxres_star, OGS_MAX_RES_LEN) != 0) {
    // Authentication failure
    gmm_cause = OGS_5GMM_CAUSE_5GS_SERVICES_NOT_ALLOWED;
}
```

**Key Calculation**:
- HXRES* = SHA-256(RAND || RES*)
- This provides privacy by not transmitting RES* in clear

### Step 2: AUSF Authentication Confirmation
**File**: `src/amf/nausf-build.c`
**Function**: `amf_nausf_auth_build_authenticate_confirmation()`

```c
// Build confirmation request with RES*
ConfirmationData->res_star = ogs_sbi_s_nssai_build_from_string(
    (char *)amf_ue->xres_star, 
    amf_ue->xres_star_len
);

// Send to AUSF endpoint: /ue-authentications/{authCtxId}/5g-aka-confirmation
```

### Step 3: AUSF Response Processing
**File**: `src/amf/nausf-handler.c`
**Function**: `amf_nausf_auth_handle_authenticate_confirmation()` (lines 170-236)

```c
// Receive authenticated data
ConfirmationDataResponse = message->ConfirmationDataResponse;

// Extract KSEAF
kseaf = ConfirmationDataResponse->kseaf;
// Store KSEAF (32 bytes)
ogs_ascii_to_hex(kseaf, strlen(kseaf), amf_ue->kseaf, sizeof(amf_ue->kseaf));

// Extract authenticated SUPI
supi = ConfirmationDataResponse->supi;
amf_ue_set_supi(amf_ue, supi);
```

### Step 4: Security Key Derivation
**File**: `src/amf/nausf-handler.c` & `src/amf/gmm-build.c`

```c
// Derive KAMF from KSEAF (in nausf-handler.c, line 195)
ogs_kdf_kamf(
    amf_ue->kseaf,      // Input: KSEAF from AUSF
    amf_ue->supi,       // SUPI
    amf_ue->abba,       // ABBA parameter
    amf_ue->abba_len,   
    amf_ue->kamf        // Output: KAMF (32 bytes)
);

// Derive NAS keys from KAMF (in gmm-build.c, lines 477-480)
ogs_kdf_nas_5gs(
    OGS_KDF_NAS_INT_ALG,           // Algorithm type: integrity
    amf_ue->selected_int_algorithm, // Selected algorithm (5G-IA2)
    amf_ue->kamf,                  // Input: KAMF
    amf_ue->knas_int               // Output: KNAS-int (16 bytes)
);

ogs_kdf_nas_5gs(
    OGS_KDF_NAS_ENC_ALG,           // Algorithm type: encryption
    amf_ue->selected_enc_algorithm, // Selected algorithm (5G-EA0)
    amf_ue->kamf,                  // Input: KAMF
    amf_ue->knas_enc               // Output: KNAS-enc (16 bytes)
);
```

**Key Hierarchy**:
```
KAUSF (in AUSF)
    ↓
KSEAF (32 bytes) - Shared with AMF
    ↓
KAMF (32 bytes) - AMF specific key
    ↓
KNAS-int (16 bytes) - NAS integrity key
KNAS-enc (16 bytes) - NAS encryption key
```

### Step 5: Security Algorithm Selection
**File**: `src/amf/context.c`
**Functions**: `amf_selected_int_algorithm()` (line 2678), `amf_selected_enc_algorithm()` (line 2694)

```c
// Select integrity algorithm
for (i = 0; i < self->num_of_integrity_order; i++) {
    if (self->integrity_order[i] == OGS_NAS_SECURITY_ALGORITHMS_NIA0) {
        if (amf_ue->ue_security_capability.nr_ia0) {
            amf_ue->selected_int_algorithm = self->integrity_order[i];
            break;
        }
    } else if (self->integrity_order[i] == OGS_NAS_SECURITY_ALGORITHMS_128_NIA1) {
        if (amf_ue->ue_security_capability.nr_ia1) {
            amf_ue->selected_int_algorithm = self->integrity_order[i];
            break;
        }
    } else if (self->integrity_order[i] == OGS_NAS_SECURITY_ALGORITHMS_128_NIA2) {
        if (amf_ue->ue_security_capability.nr_ia2) {
            amf_ue->selected_int_algorithm = self->integrity_order[i];
            break;
        }
    }
    // Similar for NIA3...
}
```

**Algorithm Selection Logic**:
- AMF has configured priority order
- Checks UE capabilities from registration request
- Selects first matching algorithm
- In this case: 5G-IA2 (AES) for integrity, 5G-EA0 (null) for encryption

### Step 6: Build Security Mode Command
**File**: `src/amf/gmm-build.c`
**Function**: `gmm_build_security_mode_command()` (line 400)

```c
// Set message header
message.h.security_header_type = 
    OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_NEW_SECURITY_CONTEXT;
message.gmm.h.message_type = OGS_NAS_5GS_SECURITY_MODE_COMMAND;

// Set selected algorithms
security_mode_command->selected_nas_security_algorithms.type_of_integrity = 
    amf_ue->selected_int_algorithm;
security_mode_command->selected_nas_security_algorithms.type_of_ciphering = 
    amf_ue->selected_enc_algorithm;

// Set ngKSI
security_mode_command->ngksi.tsc = amf_ue->nas.tsc;
security_mode_command->ngksi.ksi = amf_ue->nas.ksi;

// Replay UE security capabilities
security_mode_command->replayed_ue_security_capabilities = 
    amf_ue->ue_security_capability;

// Set additional information
additional_security_information->rinmr = 1;  // Retransmission requested
additional_security_information->hdp = 0;    // No horizontal derivation
```

### Step 7: MAC Calculation
**File**: `src/amf/nas-security.c`
**Function**: `nas_5gs_security_encode()` → `ogs_nas_mac_calculate()`

```c
// MAC calculation inputs:
ogs_nas_mac_calculate(
    amf_ue->selected_int_algorithm,  // 5G-IA2 (AES)
    amf_ue->knas_int,               // 16-byte integrity key
    amf_ue->dl_count.i32,           // Downlink counter (0 for first message)
    0x01,                           // Access type (3GPP access)
    OGS_NAS_SECURITY_DOWNLINK_DIRECTION,
    &nas_message,                   // Message to protect
    mac                            // Output: 4-byte MAC
);

// MAC = Truncate(Algorithm(Key, Count, Bearer, Direction, Message), 32)
```

## Field Transformations Summary

| Field | Authentication Response | Security Mode Command | Transformation Logic |
|-------|------------------------|----------------------|---------------------|
| Message Type | 0x57 | 0x5d | Changed to security mode command |
| Security Header | 0 (plain) | 3 (integrity protected) | First protected message |
| RES* | 16 bytes | Not included | Used for authentication verification |
| MAC | Not present | 4 bytes (0x13bf995a) | Calculated using KNAS-int |
| Sequence Number | Not present | 0 | First protected message |
| Selected Algorithms | Not present | EA0, IA2 | Based on UE capabilities |
| ngKSI | Not present | 0 | New security context |

## Security State Transition
1. **Before**: No security context, all messages in plain text
2. **After**: Security context established with:
   - KAMF derived
   - NAS keys (KNAS-int, KNAS-enc) derived
   - Integrity protection active
   - Sequence numbers initialized
   - Ready for encrypted communication

## Key Algorithms Used

### 1. HXRES* Calculation
```
HXRES* = SHA-256(RAND || RES*)
```

### 2. KAMF Derivation
```
KAMF = KDF(KSEAF, "KAMF", SUPI, ABBA)
```

### 3. NAS Key Derivation
```
KNAS-int = KDF(KAMF, "NAS-int", algorithm_id)
KNAS-enc = KDF(KAMF, "NAS-enc", algorithm_id)
```

### 4. MAC Calculation (AES-CMAC for IA2)
```
MAC = AES-CMAC(KNAS-int, Count || Bearer || Direction || Message)[0..31]
```

## Security Considerations
1. RES* verification ensures UE possesses correct key
2. MAC protects against message tampering
3. Sequence numbers prevent replay attacks
4. Algorithm negotiation prevents downgrade attacks
5. Replayed UE capabilities prevent capability tampering