# Phase 3: Security Mode Complete → Registration Accept Transformation

## Overview
This document details how open5gs AMF transforms a Security Mode Complete message into a Registration Accept message.

## Input Message: Security Mode Complete
- **Message Type**: 0x5e (Security Mode Complete)
- **Security**: Integrity protected and ciphered (header type 4)
- **Key Fields**:
  - MAC: 0x22e4ee19
  - Sequence number: 0
  - IMEISV: 4370816125816151
  - NAS message container: Contains replayed registration request

## Output Message: Registration Accept
- **Message Type**: 0x42 (Registration Accept)
- **Security**: Integrity protected and ciphered (header type 2)
- **Key Fields**:
  - MAC: 0x7239674c
  - Sequence number: 1
  - 5G-GUTI: MCC=999, MNC=70, AMF Region=2, AMF Set=1, AMF Pointer=0, 5G-TMSI=0xC0000727
  - TAI list: Type 2, containing MCC=999, MNC=70, TAC=1
  - Allowed NSSAI: SST=1
  - T3512 timer: 18 minutes

## Network Functions Involved

### 1. AMF (Access and Mobility Management Function)
- Validates security mode complete
- Allocates GUTI for the UE
- Determines serving TAI list
- Configures allowed network slices
- Builds and sends registration accept

### 2. UDM (Unified Data Management)
- Provides subscription data
- Returns allowed network slices
- Provides mobility restrictions

### 3. NSSF (Network Slice Selection Function)
- Validates requested network slices
- Returns allowed S-NSSAIs based on subscription

## Detailed Function Call Flow

### Step 1: Security Mode Complete Reception
**File**: `src/amf/gmm-handler.c`
**Function**: `gmm_handle_security_mode_complete()` (lines 1045-1135)

```c
// Verify NAS message container is present (required by TS33.501)
if (!security_mode_complete->presencemask & 
    OGS_NAS_5GS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_PRESENT) {
    ogs_error("No NAS Message Container in Security mode complete");
    return OGS_ERROR;
}

// Extract IMEISV if present
if (security_mode_complete->presencemask & 
    OGS_NAS_5GS_SECURITY_MODE_COMPLETE_IMEISV_PRESENT) {
    mobile_identity_imeisv = &security_mode_complete->imeisv;
    
    // Convert to BCD format
    imeisv_bcd_to_buffer(&mobile_identity_imeisv->imeisv,
        amf_ue->imeisv, &amf_ue->imeisv_len);
    
    // Create masked IMEISV (last 2 digits masked)
    memset(amf_ue->masked_imeisv, 0xff, OGS_MAX_IMEISV_LEN);
    amf_ue->masked_imeisv_len = amf_ue->imeisv_len;
    memcpy(amf_ue->masked_imeisv, amf_ue->imeisv, amf_ue->imeisv_len - 1);
}

// Process the NAS message container (contains registration request)
nas_message_container = &security_mode_complete->nas_message_container;
// This verifies the original registration request wasn't tampered
```

### Step 2: GUTI Allocation
**File**: `src/amf/context.c`
**Function**: `amf_ue_new_guti()` (lines 1479-1518)

```c
// Allocate new GUTI
amf_ue_new_guti(amf_ue) {
    // Set PLMN ID from assigned GUAMI
    ogs_nas_from_plmn_id(&amf_ue->next.guti.nas_plmn_id,
        &amf_ue->guami->plmn_id);
    
    // Copy AMF ID components
    memcpy(&amf_ue->next.guti.amf_id, 
        &amf_ue->guami->amf_id, sizeof(ogs_amf_id_t));
    
    // Allocate new M-TMSI (5G-TMSI)
    amf_ue->next.m_tmsi = amf_m_tmsi_alloc();
    
    // Build complete GUTI
    amf_ue->next.guti.m_tmsi = *(amf_ue->next.m_tmsi);
}

// M-TMSI allocation (context.c lines 398-421)
amf_m_tmsi_alloc() {
    // Find free M-TMSI from pool
    ogs_pool_alloc(&amf_m_tmsi_pool, &m_tmsi);
    *m_tmsi = ogs_htobe32(index + 1);  // Non-zero M-TMSI
    
    // Hash table entry for quick lookup
    m_tmsi->value = *m_tmsi;
    ogs_hash_set(self.m_tmsi_hash, &m_tmsi->value, sizeof(m_tmsi->value), m_tmsi);
}
```

**GUTI Structure**:
```
5G-GUTI = GUAMI + 5G-TMSI
GUAMI = PLMN ID + AMF ID
AMF ID = AMF Region ID (8 bits) + AMF Set ID (10 bits) + AMF Pointer (6 bits)
```

### Step 3: TAI List Determination
**File**: `src/amf/context.c`
**Function**: `amf_find_served_tai()` (lines 2520-2568)

```c
// Find which TAI list contains the UE's current TAI
served_tai_index = amf_find_served_tai(&amf_ue->nr_tai);

// Search through configured TAI lists
for (i = 0; i < self.num_of_served_tai; i++) {
    // Type 0: TAIs belonging to one PLMN
    if (self.served_tai[i].list0.tai[0].type == OGS_TAI0_TYPE) {
        for (j = 0; j < self.served_tai[i].list0.num; j++) {
            if (memcmp(&self.served_tai[i].list0.tai[j], 
                nr_tai, sizeof(ogs_5gs_tai_t)) == 0) {
                return i;  // Found matching TAI
            }
        }
    }
    // Similar checks for Type 1 and Type 2 TAI lists...
}
```

### Step 4: Network Slice Configuration
**File**: `src/amf/gmm-handler.c` & `src/amf/gmm-build.c`

```c
// Process requested NSSAI (from registration request)
// Already parsed and stored in amf_ue->requested_nssai

// Build allowed NSSAI in registration accept (gmm-build.c lines 107-123)
if (amf_ue->allowed_nssai.num_of_s_nssai) {
    registration_accept->presencemask |= 
        OGS_NAS_5GS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_PRESENT;
    
    allowed_nssai->length = ogs_nas_build_nssai(
        allowed_nssai->buffer,
        &amf_ue->allowed_nssai);
}

// Include rejected NSSAI if any
if (amf_ue->rejected_nssai.num_of_s_nssai) {
    registration_accept->presencemask |= 
        OGS_NAS_5GS_REGISTRATION_ACCEPT_REJECTED_NSSAI_PRESENT;
    
    ogs_nas_build_rejected_nssai(
        &registration_accept->rejected_nssai,
        &amf_ue->rejected_nssai);
}
```

### Step 5: Build Registration Accept
**File**: `src/amf/gmm-build.c`
**Function**: `gmm_build_registration_accept()` (lines 30-173)

```c
// Set message type and security headers
message.h.security_header_type = 
    OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED;
message.gmm.h.message_type = OGS_NAS_5GS_REGISTRATION_ACCEPT;

// Set registration result
registration_accept->registration_result.access_type = 
    OGS_5GS_REGISTRATION_TYPE_3GPP_ACCESS;

// Include 5G-GUTI if allocated
if (amf_ue->next.m_tmsi) {
    registration_accept->presencemask |= 
        OGS_NAS_5GS_REGISTRATION_ACCEPT_5G_GUTI_PRESENT;
    ogs_nas_5gs_nas_guti_to_mobility_identity_guti(
        &amf_ue->next.guti, &registration_accept->guti);
}

// Include TAI list (mandatory)
served_tai_index = amf_find_served_tai(&amf_ue->nr_tai);
ogs_nas_5gs_tai_list_build(&registration_accept->tai_list,
    &amf_self()->served_tai[served_tai_index]);

// Set T3512 timer (mandatory)
registration_accept->presencemask |= 
    OGS_NAS_5GS_REGISTRATION_ACCEPT_T3512_VALUE_PRESENT;
registration_accept->t3512_value.unit = 
    OGS_NAS_GRPS_TIMER_3_UNIT_MULTIPLES_OF_DECI_HOURS;
registration_accept->t3512_value.value = 18;  // 18 * 6 minutes = 108 minutes

// Set network feature support
network_feature_support->length = 2;
network_feature_support->ims_vops_3gpp = 1;  // VoPS supported
```

### Step 6: Security Protection and Transmission
**File**: `src/amf/nas-security.c`
**Function**: `nas_5gs_security_encode()`

```c
// Apply security protection
// - Increment DL count (sequence number = 1)
// - Calculate new MAC with sequence number 1
// - Apply encryption if enabled (EA0 = null in this case)

// MAC calculation for registration accept
ogs_nas_mac_calculate(
    amf_ue->selected_int_algorithm,  // 5G-IA2
    amf_ue->knas_int,               // NAS integrity key
    amf_ue->dl_count.i32,           // Downlink count = 1
    0x01,                           // Access type
    OGS_NAS_SECURITY_DOWNLINK_DIRECTION,
    &nas_message,
    mac                             // Output: 0x7239674c
);
```

## Field Transformations Summary

| Field | Security Mode Complete | Registration Accept | Transformation Logic |
|-------|----------------------|---------------------|---------------------|
| Message Type | 0x5e | 0x42 | Changed to registration accept |
| Security | Type 4 (int+enc) | Type 2 (int+enc) | Different header for established context |
| Sequence Number | 0 | 1 | Incremented for next message |
| IMEISV | Present | Not included | Stored in UE context |
| NAS Container | Registration request | Not included | Used for verification |
| 5G-GUTI | Not present | Allocated | New temporary identity |
| TAI List | Not present | Type 2 list | Based on current location |
| NSSAI | Not present | Allowed: SST=1 | Based on subscription |
| Timers | Not present | T3512=18 min | Periodic update timer |

## Key Algorithms and Calculations

### 1. GUTI Structure
```
5G-GUTI (80 bits total):
- PLMN ID: 24 bits (MCC + MNC)
- AMF ID: 24 bits
  - AMF Region ID: 8 bits (value: 2)
  - AMF Set ID: 10 bits (value: 1)
  - AMF Pointer: 6 bits (value: 0)
- 5G-TMSI: 32 bits (value: 0xC0000727)
```

### 2. TAI List Encoding
```
Type 2 TAI list (different PLMNs):
- List type: 2
- Number of elements: 1
- TAI[0]: PLMN(999,70) + TAC(1)
```

### 3. Timer Encoding (GPRS Timer 3)
```
T3512 value:
- Unit: 0 (multiples of 10 minutes)
- Value: 18
- Total: 180 minutes = 3 hours
```

### 4. NSSAI Encoding
```
Allowed NSSAI:
- Number of S-NSSAIs: 1
- S-NSSAI[0]:
  - Length: 1
  - SST: 1 (eMBB slice)
  - SD: not present
```

## Security Considerations
1. NAS message container ensures registration request integrity
2. GUTI provides temporary identity for privacy
3. TAI list defines mobility without re-registration
4. Network slicing ensures service isolation
5. Timers ensure periodic security updates