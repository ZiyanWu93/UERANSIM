# ROADMAP-phase2.md Implementation Plan

## Parse Tree Analysis and Field Derivation

### Input Message Parse Tree: Authentication Response
```json
{
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS)
  "nas_5gs.security_header_type": "0",     // Plain NAS message (no security yet)
  "nas_5gs.mm.message_type": "0x57",       // Authentication Response
  "Authentication response parameter": {
    "nas_eps.emm.elem_id": "0x2d",         // Element identifier
    "gsm_a.len": "16",                     // RES* length
    "nas_eps.emm.res": "ef:27:70:c6:9e:73:82:aa:38:e8:13:4f:60:22:34:e1" // RES* value
  }
}
```

### Output Message Parse Tree: Security Mode Command
```json
{
  "_ngap": {
    "per.octet_string_length": "21",        // Total NAS PDU length → See "Protocol Encoding Fields" section
    "ngap.NAS_PDU": "7e:03:13:bf:99:5a:00:7e:00:5d:00:00:02:00:00:22:04:80:01:00:e1:36:01:01"  // Complete hex encoding → See "Complete Message Encoding" section
  },
  "Security protected NAS 5GS message": {
    "nas_5gs.epd": "126",                  // Extended Protocol Discriminator (5GS) → See "Static Fields" section
    "nas_5gs.security_header_type": "3",   // Integrity protected with new security context → See "Security Header Type" section
    "nas_5gs.msg_auth_code": "0x13bf995a", // MAC (4 bytes) → See "MAC Calculation" section
    "nas_5gs.seq_no": "0"                  // Sequence number (first protected message) → See "Sequence Number" section
  },
  "Plain NAS 5GS Message": {
    "nas_5gs.epd": "126",                  // Extended Protocol Discriminator (5GS) → See "Static Fields" section
    "nas_5gs.security_header_type": "0",   // Plain header for inner message → See "Static Fields" section
    "nas_5gs.spare_half_octet": "0",       // Spare bits (4 bits) → See "Spare Bit Handling" section
    "nas_5gs.mm.message_type": "0x5d",     // Security Mode Command → See "Static Fields" section
    "NAS security algorithms": {
      "nas_5gs.mm.type_of_ciphering": "0",  // 5G-EA0 (null encryption) → See "Selected Algorithms" section
      "nas_5gs.mm.spare_bits": "0",         // Spare bits (4 bits) → See "Spare Bit Handling" section
      "nas_5gs.mm.nas_sec_algo_enc": "0",   // 5G-EA0 (null encryption) → See "Selected Algorithms" section
      "nas_5gs.mm.type_of_integrity": "2",  // 5G-IA2 (128-bit AES integrity) → See "Selected Algorithms" section
      "nas_5gs.mm.nas_sec_algo_ip": "2"     // 5G-IA2 (128-bit AES integrity) → See "Selected Algorithms" section
    },
    "NAS key set identifier - ngKSI": {
      "nas_5gs.mm.tsc": "0",               // Type of Security Context (native) → See "ngKSI" section
      "nas_5gs.mm.spare_bits": "0",        // Spare bits (4 bits) → See "Spare Bit Handling" section
      "nas_5gs.mm.nas_key_set_id": "0"     // Key Set Identifier → See "ngKSI" section
    },
    "UE security capability": {             // Replayed from registration request → See "UE Security Capabilities" section
      "gsm_a.len": "4",                    // Length of security capability → See "Length Calculations" section
      "nas_5gs.mm.5g_ea0": "1",           // EA0 supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ea1": "0",       // EA1 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ea2": "0",       // EA2 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ea3": "0",       // EA3 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ea4": "0",           // EA4 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ea5": "0",           // EA5 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ea6": "0",           // EA6 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ea7": "0",           // EA7 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ia0": "1",           // IA0 supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ia1": "1",       // IA1 supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ia2": "1",       // IA2 supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_128_ia3": "1",       // IA3 supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ia4": "0",           // IA4 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ia5": "0",           // IA5 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ia6": "0",           // IA6 not supported → See "Complete UE Security Capability Encoding" section
      "nas_5gs.mm.5g_ia7": "0",           // IA7 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eea0": "1",            // EEA0 supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eea1": "0",         // EEA1 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eea2": "0",         // EEA2 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eea3": "0",         // EEA3 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eea4": "0",            // EEA4 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eea5": "0",            // EEA5 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eea6": "0",            // EEA6 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eea7": "0",            // EEA7 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eia0": "1",            // EIA0 supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eia1": "1",         // EIA1 supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eia2": "1",         // EIA2 supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.128eia3": "1",         // EIA3 supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eia4": "0",            // EIA4 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eia5": "0",            // EIA5 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eia6": "0",            // EIA6 not supported → See "Complete UE Security Capability Encoding" section
      "nas_eps.emm.eia7": "0"             // EIA7 not supported → See "Complete UE Security Capability Encoding" section
    },
    "IMEISV request": {
      "nas_eps.emm.elem_id": "0x0e",       // Element identifier → See "Information Element Identifiers" section
      "nas_5gs.mm.imeisv_req_spare": "0",  // Spare bits (5 bits) → See "Spare Bit Handling" section
      "nas_eps.emm.imeisv_req": "1"        // Request IMEISV from UE → See "Additional Security Parameters" section
    },
    "Additional 5G security information": {
      "nas_eps.emm.elem_id": "0x36",       // Element identifier → See "Information Element Identifiers" section
      "gsm_a.len": "1",                    // Length of element → See "Length Calculations" section
      "nas_5gs.mm.rinmr": "1",             // Retransmission indication → See "Additional Security Parameters" section
      "nas_5gs.mm.hdp": "0",               // No horizontal derivation → See "Additional Security Parameters" section
      "nas_5gs.mm.spare_bits6": "0"        // Spare bits (6 bits) → See "Spare Bit Handling" section
    }
  }
}
```

## Field Derivation Logic

### Static Fields (Direct Assignment)
1. **EPD (126)**: Always 0x7E for 5GS messages
2. **Inner Security Header Type (0)**: Plain for the encapsulated message
3. **Message Type (0x5d)**: Security Mode Command constant
4. **TSC (0)**: Native security context (not mapped from EPS)

### Protocol Encoding Fields

#### PER Octet String Length
- **Field**: `per.octet_string_length = "21"`
- **Calculation**: Total NAS PDU bytes including security header
  - Security header: 7 bytes (EPD + security type + MAC + seq)
  - Plain message: 14 bytes
  - Total: 21 bytes
- **Encoding**: ASN.1 PER length encoding for NGAP container

### Information Element Identifiers

#### IMEISV Request Element ID
- **Field**: `nas_eps.emm.elem_id = "0x0e"`
- **Value**: 0x0E (14 decimal)
- **Standard**: 3GPP TS 24.301 Table 9.9.1.1
- **Type**: Type 1 IEI (single octet)

#### Additional 5G Security Information Element ID
- **Field**: `nas_eps.emm.elem_id = "0x36"`
- **Value**: 0x36 (54 decimal)
- **Standard**: 3GPP TS 24.501 Table 9.11.1.1
- **Type**: Type 4 IEI (TLV format)

### Length Calculations

#### UE Security Capability Length
- **Field**: `gsm_a.len = "4"`
- **Calculation**: 
  - 5G algorithms: 2 bytes (16 bits)
  - EPS algorithms: 2 bytes (16 bits)
  - Total: 4 bytes
- **Format**: Length octet for Type 4 IE

#### Additional 5G Security Information Length
- **Field**: `gsm_a.len = "1"`
- **Content**: Single octet containing RINMR, HDP, and spare bits
- **Format**: Length octet for Type 4 IE

### Spare Bit Handling

#### Message Level Spare Bits
- **Field**: `nas_5gs.spare_half_octet = "0"`
- **Location**: Upper 4 bits of security header type octet
- **Requirement**: Must be set to 0 by sender, ignored by receiver

#### Algorithm Selection Spare Bits
- **Field**: `nas_5gs.mm.spare_bits = "0"`
- **Location**: Upper 4 bits of each algorithm octet
- **Purpose**: Reserved for future algorithm types

#### ngKSI Spare Bits
- **Field**: `nas_5gs.mm.spare_bits = "0"`
- **Location**: Bits 4-7 of ngKSI octet
- **Purpose**: Reserved for future use

#### IMEISV Request Spare Bits
- **Field**: `nas_5gs.mm.imeisv_req_spare = "0"`
- **Location**: Bits 4-8 of IMEISV request octet
- **Format**: 5 spare bits before the 3-bit IMEISV request value

#### Additional Security Information Spare Bits
- **Field**: `nas_5gs.mm.spare_bits6 = "0"`
- **Location**: Bits 3-8 of additional security info octet
- **Purpose**: Reserved for future security parameters

### Complete UE Security Capability Encoding

#### 5G Algorithm Support (2 bytes)
**Byte 1 - Encryption Algorithms**:
- Bit 8: 5G-EA0 = 1 (null encryption supported)
- Bit 7: 5G-EA1 = 0 (128-bit SNOW 3G not supported)
- Bit 6: 5G-EA2 = 0 (128-bit AES not supported)
- Bit 5: 5G-EA3 = 0 (128-bit ZUC not supported)
- Bit 4: 5G-EA4 = 0 (reserved)
- Bit 3: 5G-EA5 = 0 (reserved)
- Bit 2: 5G-EA6 = 0 (reserved)
- Bit 1: 5G-EA7 = 0 (reserved)
- **Hex**: 0x80

**Byte 2 - Integrity Algorithms**:
- Bit 8: 5G-IA0 = 1 (null integrity supported)
- Bit 7: 5G-IA1 = 1 (128-bit SNOW 3G supported)
- Bit 6: 5G-IA2 = 1 (128-bit AES supported)
- Bit 5: 5G-IA3 = 1 (128-bit ZUC supported)
- Bit 4: 5G-IA4 = 0 (reserved)
- Bit 3: 5G-IA5 = 0 (reserved)
- Bit 2: 5G-IA6 = 0 (reserved)
- Bit 1: 5G-IA7 = 0 (reserved)
- **Hex**: 0xF0

#### EPS Algorithm Support (2 bytes)
**Byte 3 - EPS Encryption Algorithms**:
- Bit 8: EEA0 = 1 (null encryption supported)
- Bit 7: 128-EEA1 = 0 (SNOW 3G not supported)
- Bit 6: 128-EEA2 = 0 (AES not supported)
- Bit 5: 128-EEA3 = 0 (ZUC not supported)
- Bit 4: EEA4 = 0 (reserved)
- Bit 3: EEA5 = 0 (reserved)
- Bit 2: EEA6 = 0 (reserved)
- Bit 1: EEA7 = 0 (reserved)
- **Hex**: 0x80

**Byte 4 - EPS Integrity Algorithms**:
- Bit 8: EIA0 = 1 (null integrity supported)
- Bit 7: 128-EIA1 = 1 (SNOW 3G supported)
- Bit 6: 128-EIA2 = 1 (AES supported)
- Bit 5: 128-EIA3 = 1 (ZUC supported)
- Bit 4: EIA4 = 0 (reserved)
- Bit 3: EIA5 = 0 (reserved)
- Bit 2: EIA6 = 0 (reserved)
- Bit 1: EIA7 = 0 (reserved)
- **Hex**: 0xF0

**Complete UE Security Capability**: `0x80 0xF0 0x80 0xF0`

### Complete Message Encoding

#### NGAP NAS-PDU Container
- **Field**: `ngap.NAS_PDU`
- **Value**: `7e:03:13:bf:99:5a:00:7e:00:5d:00:00:02:00:00:22:04:80:f0:80:f0:0e:01:36:01:01`
- **Breakdown**:
  - `7e`: EPD (5GS)
  - `03`: Security header type (integrity protected with new context)
  - `13:bf:99:5a`: MAC
  - `00`: Sequence number
  - `7e:00`: Inner EPD and security header type
  - `5d`: Message type (Security Mode Command)
  - `00:00`: Selected algorithms (EA0, IA2)
  - `02`: ngKSI (TSC=0, KSI=0)
  - `00:00`: Spare octets
  - `22`: UE security capability IEI
  - `04`: Length
  - `80:f0:80:f0`: Capability bits
  - `0e:01`: IMEISV request
  - `36:01:01`: Additional 5G security info

### Container Structures

#### Security Protected Container
- **Structure**: Security header (7 bytes) + Plain NAS message (variable)
- **Security Header**:
  - Octet 1: EPD (0x7E)
  - Octet 2: Security header type (0x03)
  - Octets 3-6: MAC (4 bytes)
  - Octet 7: Sequence number (1 byte)
- **Purpose**: Provides integrity protection for the encapsulated message

#### Plain NAS Message Container
- **Structure**: Header (2 bytes) + Message content (variable)
- **Header**:
  - Octet 1: EPD (0x7E)
  - Octet 2: Security header type (0x00) with spare bits
- **Content**: Message type followed by mandatory and optional IEs

#### Information Element Containers
- **Type 1 IEs**: Single octet with IEI in upper 4 bits, value in lower 4 bits
- **Type 3 IEs**: IEI (1 byte) + fixed length content
- **Type 4 IEs**: IEI (1 byte) + length (1 byte) + variable content
- **Type 6 IEs**: IEI (1 byte) + length (2 bytes) + variable content

### Derived Fields from Authentication Flow

#### 1. **Security Header Type (3)**
- Value: Integrity protected with new security context
- Reason: First message after authentication establishes new security context
- Triggers MAC calculation and security header addition

#### 2. **Sequence Number (0)**
- Value: 0 for first protected message
- Storage: Initialize downlink counter in UE context
- Purpose: Replay protection mechanism

#### 3. **MAC (0x13bf995a)**
- Calculation: AES-CMAC with KNAS-int over entire message
- Inputs:
  - Key: KNAS-int (derived from KAMF)
  - Count: 0 (first message)
  - Bearer: 0x01 (3GPP access)
  - Direction: Downlink (0x01)
  - Message: Complete NAS PDU
- Output: First 32 bits of AES-CMAC result

#### 4. **Selected Algorithms**
- **Encryption (0)**: 5G-EA0 (null encryption)
  - Selected based on AMF configuration and UE capabilities
  - UE supports: EA0=1, EA1=0, EA2=0, EA3=0
  - AMF selects EA0 as it's the only supported algorithm
- **Integrity (2)**: 5G-IA2 (128-bit AES)
  - UE supports: IA0=1, IA1=1, IA2=1, IA3=1
  - AMF priority order selects IA2 (AES) over IA1 (SNOW3G)

#### 5. **ngKSI (0)**
- Reset from previous value (7) to new value (0)
- Indicates new security context after authentication
- Will be used in all subsequent protected messages

### Replayed Fields from Registration Request

#### 1. **UE Security Capabilities**
- Exact copy from registration request to prevent tampering
- Includes all encryption and integrity algorithm support flags
- Must match stored values or security mode reject
- Complete 32-bit encoding replayed verbatim

#### 2. **Additional Security Parameters**
- **RINMR (1)**: Retransmission of initial NAS message requested
- **HDP (0)**: No horizontal key derivation performed
- **IMEISV Request (1)**: UE should include IMEISV in response

## Implementation Function Chain

### Phase 2: NFLambda AMF Function Implementation

```
nflambda_amf_handle_authentication_response()
    ↓
verify_res_star()
    ↓
ausf_confirm_authentication()
    ↓
receive_kseaf_and_supi()
    ↓
derive_security_keys()
    ↓
select_security_algorithms()
    ↓
build_security_mode_command()
    ↓
calculate_mac()
    ↓
send_security_mode_command()
```

### MAC Calculation

#### Input Construction for AES-CMAC
- **COUNT**: 0x00000000 (4 bytes, first downlink message)
- **BEARER**: 0x01 (1 byte, 3GPP access)
- **DIRECTION**: 0x01 (1 byte, downlink)
- **MESSAGE**: Plain NAS message starting after sequence number
- **Algorithm**: AES-CMAC-128 (for 5G-IA2)
- **Key**: KNAS-int (16 bytes derived from KAMF)
- **Output**: Full 128-bit MAC, truncated to first 32 bits = `0x13bf995a`

### Detailed Function Specifications

#### 1. `nflambda_amf_handle_authentication_response()`
**Input**: Authentication Response NAS PDU
**Output**: Trigger security establishment flow
**Logic**:
- Parse NAS PDU using ASN.1 decoder
- Extract RES* parameter (16 bytes)
- Validate message format and length
- Store RES* in UE context for verification

#### 2. `verify_res_star()`
**Input**: Received RES*, stored authentication data
**Output**: Verification result
**Logic**:
- Calculate HXRES* from received RES*:
  ```c
  HXRES* = SHA-256(RAND || RES*)
  ```
- Compare with stored HXRES* from authentication vector
- On mismatch: Send authentication reject
- On match: Proceed to AUSF confirmation

#### 3. `ausf_confirm_authentication()`
**Input**: RES*, authentication context ID
**Output**: HTTP request to AUSF
**Logic**:
- Build ConfirmationData with RES*
- Set endpoint: `/ue-authentications/{authCtxId}/5g-aka-confirmation`
- Include serving network name
- Send HTTP PUT request to AUSF

#### 4. `receive_kseaf_and_supi()`
**Input**: AUSF confirmation response
**Output**: KSEAF and authenticated SUPI
**Logic**:
- Extract KSEAF (256 bits) from response
- Extract authenticated SUPI
- Store both in UE context
- Update UE state to authenticated

#### 5. `derive_security_keys()`
**Input**: KSEAF, SUPI, ABBA
**Output**: KAMF, KNAS-int, KNAS-enc
**Logic**:
```c
// Step 1: Derive KAMF from KSEAF
KAMF = KDF(KSEAF, 0x6D, SUPI || ABBA_len || ABBA)
// 0x6D = FC value for KAMF derivation

// Step 2: Derive NAS integrity key
KNAS-int = KDF(KAMF, 0x69, algorithm_type || algorithm_id)
// 0x69 = FC for NAS integrity
// algorithm_type = 0x02 (integrity)
// algorithm_id = 0x02 (5G-IA2)

// Step 3: Derive NAS encryption key  
KNAS-enc = KDF(KAMF, 0x69, algorithm_type || algorithm_id)
// algorithm_type = 0x01 (encryption)
// algorithm_id = 0x00 (5G-EA0)
```

#### 6. `select_security_algorithms()`
**Input**: UE security capabilities, AMF configuration
**Output**: Selected encryption and integrity algorithms
**Logic**:
- Parse UE capabilities from stored registration request
- Check AMF algorithm priority order
- For integrity: Select first matching (IA2 in this case)
- For encryption: Select first matching (EA0 in this case)
- Validate at least one algorithm selected for each type

#### 7. `build_security_mode_command()`
**Input**: Selected algorithms, UE capabilities, security context
**Output**: Security Mode Command NAS PDU
**Logic**:
- Set message type = 0x5d
- Set selected algorithms (EA0=0, IA2=2)
- Set ngKSI (TSC=0, KSI=0)
- Copy UE security capabilities exactly
- Set IMEISV request = 1
- Set additional security info (RINMR=1, HDP=0)
- Build complete NAS message structure

#### 8. `calculate_mac()`
**Input**: Complete NAS message, security context
**Output**: 4-byte MAC
**Logic**:
```c
// Prepare MAC input
count = dl_count (0 for first message)
bearer = 0x01 (3GPP access)
direction = 0x01 (downlink)

// Calculate full MAC
full_mac = AES_CMAC(KNAS-int, count || bearer || direction || message)

// Truncate to 32 bits
mac = full_mac[0..3]
```

#### 9. `send_security_mode_command()`
**Input**: Protected NAS PDU with MAC
**Output**: Message sent to RAN
**Logic**:
- Add security header (type=3)
- Add MAC (4 bytes)
- Add sequence number (1 byte)
- Get RAN UE context
- Send via NGAP interface
- Start T3560 timer for response
- Increment downlink count

## NFLambda Integration Points

### Event-Driven Architecture
```
EVENT_AUTH_RESPONSE → amf_actor
    ↓
EVENT_RES_VERIFICATION → crypto_actor
    ↓
EVENT_AUSF_CONFIRM → ausf_client_actor
    ↓
EVENT_KEY_DERIVATION → crypto_actor
    ↓
EVENT_ALGO_SELECTION → security_actor
    ↓
EVENT_SMC_BUILD → nas_builder_actor
    ↓
EVENT_MAC_CALCULATION → crypto_actor
    ↓
EVENT_NAS_SEND → ngap_sender_actor
```

### Memory Pool Usage
- **Security Context Pool**: Store KSEAF, KAMF, NAS keys
- **Message Pool**: Security mode command buffers
- **Crypto Pool**: Temporary buffers for MAC calculation
- **Algorithm Pool**: Selected algorithm storage

### Actor Message Definitions
```c
typedef struct {
    uint32_t ue_id;
    uint8_t res_star[16];
    size_t res_len;
} auth_response_msg_t;

typedef struct {
    uint32_t ue_id;
    char* auth_ctx_id;
    uint8_t res_star[16];
} ausf_confirm_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t kseaf[32];
    char* supi;
} kseaf_response_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t kamf[32];
    uint8_t knas_int[16];
    uint8_t knas_enc[16];
} derived_keys_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t selected_enc_algo;
    uint8_t selected_int_algo;
} algo_selection_msg_t;
```

### Error Handling
- RES* verification failure → Authentication reject (cause: MAC failure)
- AUSF unreachable → Retry with exponential backoff
- Key derivation failure → Authentication reject
- No matching algorithms → Security mode reject
- Timer T3560 expiry → Retransmit or release

### Security State Transitions
```
State: UNAUTHENTICATED
    ↓ (Authentication Request)
State: AUTHENTICATING
    ↓ (Authentication Response + RES* verified)
State: AUTHENTICATED
    ↓ (Security Mode Command sent)
State: SECURITY_MODE_INITIATED
    ↓ (Security Mode Complete)
State: SECURITY_ESTABLISHED
```

## Detailed Field Calculations

### RES* Verification and HXRES* Calculation
- **Input RES***: `ef:27:70:c6:9e:73:82:aa:38:e8:13:4f:60:22:34:e1`
- **Stored RAND**: `5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69` (from Phase 1)
- **Calculation**:
  ```
  HXRES* = SHA-256(RAND || RES*)
         = SHA-256(5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69 || 
                   ef:27:70:c6:9e:73:82:aa:38:e8:13:4f:60:22:34:e1)
  ```
- **Purpose**: Privacy protection - HXRES* stored instead of RES*

### Security Key Hierarchy and Derivation

#### KAUSF → KSEAF (in AUSF)
- **Input**: KAUSF from authentication
- **Output**: KSEAF (256 bits)
- **Function**: KDF with serving network name

#### KSEAF → KAMF (in AMF)
- **Input**: 
  - KSEAF: 32 bytes from AUSF
  - SUPI: "imsi-999700000000001"
  - ABBA: 00:00 (from authentication request)
- **Calculation**:
  ```
  KAMF = KDF(KSEAF, 0x6D, SUPI || length(ABBA) || ABBA)
  ```
- **Output**: KAMF (256 bits)

#### KAMF → NAS Keys
- **KNAS-int** (Integrity):
  ```
  KNAS-int = KDF(KAMF, 0x69, 0x02 || 0x02)
  // 0x02 = integrity algorithm type
  // 0x02 = 5G-IA2 algorithm ID
  ```
- **KNAS-enc** (Encryption):
  ```
  KNAS-enc = KDF(KAMF, 0x69, 0x01 || 0x00)
  // 0x01 = encryption algorithm type
  // 0x00 = 5G-EA0 algorithm ID
  ```

### MAC Calculation Details
- **Algorithm**: AES-CMAC (for 5G-IA2)
- **Key**: KNAS-int (128 bits)
- **Input Construction**:
  ```
  Input = COUNT[0] || COUNT[1] || COUNT[2] || COUNT[3] ||
          BEARER || DIRECTION || 
          MESSAGE[0] || MESSAGE[1] || ... || MESSAGE[n-1]
  
  Where:
  - COUNT = 0x00000000 (first message)
  - BEARER = 0x01 (3GPP access)
  - DIRECTION = 0x01 (downlink)
  - MESSAGE = Complete NAS PDU before security header
  ```
- **Output**: First 32 bits = `0x13bf995a`

### Algorithm Selection Logic
From UE capabilities and AMF configuration:

**Integrity Algorithm Selection**:
- UE supports: IA0=1, IA1=1, IA2=1, IA3=1
- AMF priority: [IA2, IA1, IA3, IA0]
- Selected: IA2 (128-bit AES)

**Encryption Algorithm Selection**:
- UE supports: EA0=1, EA1=0, EA2=0, EA3=0
- AMF priority: [EA2, EA1, EA3, EA0]
- Selected: EA0 (null encryption) - only supported algorithm

### Replayed UE Security Capabilities
Exact copy from registration request to prevent modification attacks:
- 5G algorithms: EA0=1, IA0=1, IA1=1, IA2=1, IA3=1
- EPS algorithms: EEA0=1, EIA0=1, EIA1=1, EIA2=1, EIA3=1
- Total: 4 bytes (32 bits of capability flags)

## Open5GS Code Reference Mapping

### RES* Verification
**File**: `open5gs/src/amf/gmm-handler.c:924-970`
```c
authentication_response_parameter = &authentication_response->
    authentication_response_parameter;
res_star = authentication_response_parameter->res_star;

ogs_kdf_hxres_star(amf_ue->rand, res_star, res_star_len, hxres_star);

if (memcmp(hxres_star, amf_ue->hxres_star, OGS_MAX_RES_LEN) != 0) {
    gmm_cause = OGS_5GMM_CAUSE_5GS_SERVICES_NOT_ALLOWED;
}
```

### AUSF Confirmation
**File**: `open5gs/src/amf/nausf-build.c:162-196`
```c
ConfirmationData->res_star = ogs_sbi_s_nssai_build_from_string(
    (char *)amf_ue->xres_star, amf_ue->xres_star_len);

message.h.method = "PUT";
message.h.uri = amf_ue->confirmation_uri;
```

### Key Derivation Chain
**File**: `open5gs/src/amf/nausf-handler.c:195-236`
```c
// KSEAF → KAMF
ogs_kdf_kamf(amf_ue->kseaf, amf_ue->supi, 
             amf_ue->abba, amf_ue->abba_len, amf_ue->kamf);

// KAMF → NAS keys
ogs_kdf_nas_5gs(OGS_KDF_NAS_INT_ALG, 
                amf_ue->selected_int_algorithm,
                amf_ue->kamf, amf_ue->knas_int);

ogs_kdf_nas_5gs(OGS_KDF_NAS_ENC_ALG,
                amf_ue->selected_enc_algorithm,
                amf_ue->kamf, amf_ue->knas_enc);
```

### Security Mode Command Building
**File**: `open5gs/src/amf/gmm-build.c:400-510`
```c
message.h.security_header_type = 
    OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_NEW_SECURITY_CONTEXT;

security_mode_command->selected_nas_security_algorithms.type_of_integrity = 
    amf_ue->selected_int_algorithm;
security_mode_command->selected_nas_security_algorithms.type_of_ciphering = 
    amf_ue->selected_enc_algorithm;

// Replay UE capabilities
security_mode_command->replayed_ue_security_capabilities = 
    amf_ue->ue_security_capability;
```

### MAC Calculation
**File**: `open5gs/src/amf/nas-security.c`
```c
ogs_nas_mac_calculate(
    amf_ue->selected_int_algorithm,
    amf_ue->knas_int,
    amf_ue->dl_count.i32,
    0x01, // 3GPP access
    OGS_NAS_SECURITY_DOWNLINK_DIRECTION,
    pkbuf,
    mac);
```

## Implementation Priority

### Phase 2a: RES* Verification
1. HXRES* calculation function
2. RES* comparison logic
3. Authentication failure handling

### Phase 2b: AUSF Integration
1. Confirmation request builder
2. KSEAF extraction and storage
3. SUPI update in UE context

### Phase 2c: Key Derivation
1. KAMF derivation from KSEAF
2. NAS key derivation functions
3. Key storage in security context

### Phase 2d: Security Establishment
1. Algorithm selection based on capabilities
2. Security mode command construction
3. MAC calculation and protection
4. State transition management

### Phase 2e: Integration and Testing
1. End-to-end authentication to security flow
2. Key derivation verification
3. MAC calculation validation
4. Interoperability with UERANSIM

## Testing Strategy

### Unit Tests
1. **RES* Verification**: Test vectors for HXRES* calculation
2. **Key Derivation**: Known KSEAF → KAMF → NAS keys
3. **MAC Calculation**: Test vectors for AES-CMAC
4. **Algorithm Selection**: Various UE capability combinations

### Integration Tests
1. **Full Flow**: Registration → Authentication → Security Mode
2. **Error Cases**: Wrong RES*, AUSF timeout, no matching algorithms
3. **State Management**: Verify state transitions
4. **Timer Handling**: T3560 expiry scenarios

### Performance Tests
1. **Throughput**: Messages per second
2. **Latency**: Authentication to security establishment time
3. **Concurrency**: Multiple UEs simultaneously
4. **Memory**: Pool utilization under load

This roadmap provides a complete blueprint for implementing Phase 2 security establishment in the NFLambda AMF, transitioning from plain text to integrity-protected communication while maintaining full 3GPP compliance.