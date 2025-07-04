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
   - Function: `amf_build_security_mode_command()` → Sets EPD in outer security header
2. **Inner Security Header Type (0)**: Plain for the encapsulated message
   - Function: `amf_build_security_mode_command()` → Sets inner security header type to plain
3. **Message Type (0x5d)**: Security Mode Command constant
   - Function: `amf_build_security_mode_command()` → Sets message type for Security Mode Command
4. **TSC (0)**: Native security context (not mapped from EPS)
   - Function: `amf_build_security_mode_command()` → Sets TSC in ngKSI field

### Protocol Encoding Fields

#### PER Octet String Length
- **Field**: `per.octet_string_length = "21"`
- **Calculation**: Total NAS PDU bytes including security header
  - Security header: 7 bytes (EPD + security type + MAC + seq)
  - Plain message: 14 bytes
  - Total: 21 bytes
- **Encoding**: ASN.1 PER length encoding for NGAP container
- Function: `amf_send_security_mode_command()` → Calculates total PDU length for NGAP encoding

### Information Element Identifiers

#### IMEISV Request Element ID
- **Field**: `nas_eps.emm.elem_id = "0x0e"`
- **Value**: 0x0E (14 decimal)
- **Standard**: 3GPP TS 24.301 Table 9.9.1.1
- **Type**: Type 1 IEI (single octet)
- Function: `amf_build_security_mode_command()` → Sets IMEISV request IEI

#### Additional 5G Security Information Element ID
- **Field**: `nas_eps.emm.elem_id = "0x36"`
- **Value**: 0x36 (54 decimal)
- **Standard**: 3GPP TS 24.501 Table 9.11.1.1
- **Type**: Type 4 IEI (TLV format)
- Function: `amf_build_security_mode_command()` → Sets additional security info IEI

### Length Calculations

#### UE Security Capability Length
- **Field**: `gsm_a.len = "4"`
- **Calculation**: 
  - 5G algorithms: 2 bytes (16 bits)
  - EPS algorithms: 2 bytes (16 bits)
  - Total: 4 bytes
- **Format**: Length octet for Type 4 IE
- Function: `amf_build_security_mode_command()` → Calculates UE security capability length

#### Additional 5G Security Information Length
- **Field**: `gsm_a.len = "1"`
- **Content**: Single octet containing RINMR, HDP, and spare bits
- **Format**: Length octet for Type 4 IE
- Function: `amf_build_security_mode_command()` → Sets additional security info length

### Spare Bit Handling

#### Message Level Spare Bits
- **Field**: `nas_5gs.spare_half_octet = "0"`
- **Location**: Upper 4 bits of security header type octet
- **Requirement**: Must be set to 0 by sender, ignored by receiver
- Function: `amf_build_security_mode_command()` → Clears spare bits in message header

#### Algorithm Selection Spare Bits
- **Field**: `nas_5gs.mm.spare_bits = "0"`
- **Location**: Upper 4 bits of each algorithm octet
- **Purpose**: Reserved for future algorithm types
- Function: `amf_build_security_mode_command()` → Clears spare bits in algorithm fields

#### ngKSI Spare Bits
- **Field**: `nas_5gs.mm.spare_bits = "0"`
- **Location**: Bits 4-7 of ngKSI octet
- **Purpose**: Reserved for future use
- Function: `amf_build_security_mode_command()` → Clears spare bits in ngKSI field

#### IMEISV Request Spare Bits
- **Field**: `nas_5gs.mm.imeisv_req_spare = "0"`
- **Location**: Bits 4-8 of IMEISV request octet
- **Format**: 5 spare bits before the 3-bit IMEISV request value
- Function: `amf_build_security_mode_command()` → Clears spare bits in IMEISV request

#### Additional Security Information Spare Bits
- **Field**: `nas_5gs.mm.spare_bits6 = "0"`
- **Location**: Bits 3-8 of additional security info octet
- **Purpose**: Reserved for future security parameters
- Function: `amf_build_security_mode_command()` → Clears spare bits in additional security info

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
- Function: `amf_build_security_mode_command()` → Copies UE encryption capabilities from registration request

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
- Function: `amf_build_security_mode_command()` → Copies UE integrity capabilities from registration request

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
- Function: `amf_build_security_mode_command()` → Copies EPS encryption capabilities from registration request

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
- Function: `amf_build_security_mode_command()` → Copies EPS integrity capabilities from registration request

**Complete UE Security Capability**: `0x80 0xF0 0x80 0xF0`
- Function: `amf_build_security_mode_command()` → Assembles complete 4-byte UE security capability

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
- Function: `amf_send_security_mode_command()` → Encodes complete NAS PDU for NGAP transport

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
- Function: `amf_build_security_mode_command()` → Sets security header type for new context

#### 2. **Sequence Number (0)**
- Value: 0 for first protected message
- Storage: Initialize downlink counter in UE context
- Purpose: Replay protection mechanism
- Function: `amf_send_security_mode_command()` → Initializes sequence number for first protected message

#### 3. **MAC (0x13bf995a)**
- Calculation: AES-CMAC with KNAS-int over entire message
- Inputs:
  - Key: KNAS-int (derived from KAMF)
  - Count: 0 (first message)
  - Bearer: 0x01 (3GPP access)
  - Direction: Downlink (0x01)
  - Message: Complete NAS PDU
- Output: First 32 bits of AES-CMAC result
- Function: `amf_calculate_mac()` → Computes MAC using AES-CMAC algorithm

#### 4. **Selected Algorithms**
- **Encryption (0)**: 5G-EA0 (null encryption)
  - Selected based on AMF configuration and UE capabilities
  - UE supports: EA0=1, EA1=0, EA2=0, EA3=0
  - AMF selects EA0 as it's the only supported algorithm
  - Function: `amf_select_security_algorithms()` → Selects encryption algorithm based on capabilities
- **Integrity (2)**: 5G-IA2 (128-bit AES)
  - UE supports: IA0=1, IA1=1, IA2=1, IA3=1
  - AMF priority order selects IA2 (AES) over IA1 (SNOW3G)
  - Function: `amf_select_security_algorithms()` → Selects integrity algorithm based on priorities

#### 5. **ngKSI (0)**
- Reset from previous value (7) to new value (0)
- Indicates new security context after authentication
- Will be used in all subsequent protected messages
- Function: `amf_derive_security_keys()` → Assigns new ngKSI value for fresh security context

### Replayed Fields from Registration Request

#### 1. **UE Security Capabilities**
- Exact copy from registration request to prevent tampering
- Includes all encryption and integrity algorithm support flags
- Must match stored values or security mode reject
- Complete 32-bit encoding replayed verbatim
- Function: `amf_build_security_mode_command()` → Replays stored UE capabilities from registration

#### 2. **Additional Security Parameters**
- **RINMR (1)**: Retransmission of initial NAS message requested
  - Function: `amf_build_security_mode_command()` → Sets RINMR flag for retransmission
- **HDP (0)**: No horizontal key derivation performed
  - Function: `amf_build_security_mode_command()` → Sets HDP flag to indicate no horizontal derivation
- **IMEISV Request (1)**: UE should include IMEISV in response
  - Function: `amf_build_security_mode_command()` → Sets IMEISV request flag

## Implementation Function Chain

### Phase 2: NFLambda AMF Function Implementation

```
amf_handle_authentication_response()
    ↓
amf_verify_res_star()
    ↓
ausf_confirm_authentication()
    ↓
ausf_derive_kseaf()
    ↓
amf_derive_security_keys()
    ↓
amf_select_security_algorithms()
    ↓
amf_build_security_mode_command()
    ↓
amf_calculate_mac()
    ↓
amf_send_security_mode_command()
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

#### 1. `amf_handle_authentication_response()`
**Input**: Authentication Response binary structure
**Output**: Trigger security establishment flow
**Logic**:
- Extract RES* parameter from nas_5gs.mm.auth_resp_par field (16 bytes)
- Verify message type equals 0x57 (Authentication Response)
- Check RES* length field matches expected 16 bytes
- Store RES* value in UE context for verification

#### 2. `amf_verify_res_star()`
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

#### 4. `ausf_derive_kseaf()`
**Input**: AUSF confirmation response
**Output**: KSEAF and authenticated SUPI
**Logic**:
- Extract KSEAF (256 bits) from response
- Extract authenticated SUPI
- Store both in UE context
- Update UE state to authenticated

#### 5. `amf_derive_security_keys()`
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

#### 6. `amf_select_security_algorithms()`
**Input**: UE security capabilities, AMF configuration
**Output**: Selected encryption and integrity algorithms
**Logic**:
- Parse UE capabilities from stored registration request
- Check AMF algorithm priority order
- For integrity: Select first matching (IA2 in this case)
- For encryption: Select first matching (EA0 in this case)
- Validate at least one algorithm selected for each type

#### 7. `amf_build_security_mode_command()`
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

#### 8. `amf_calculate_mac()`
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

#### 9. `amf_send_security_mode_command()`
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
- Function: `amf_verify_res_star()` → Calculates HXRES* and verifies against stored value

### Security Key Hierarchy and Derivation

#### KAUSF → KSEAF (in AUSF)
- **Input**: KAUSF from authentication
- **Output**: KSEAF (256 bits)
- **Function**: KDF with serving network name
- Function: `ausf_derive_kseaf()` → Derives KSEAF from KAUSF (AUSF function)

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
- Function: `amf_derive_security_keys()` → Derives KAMF from KSEAF

#### KAMF → NAS Keys
- **KNAS-int** (Integrity):
  ```
  KNAS-int = KDF(KAMF, 0x69, 0x02 || 0x02)
  // 0x02 = integrity algorithm type
  // 0x02 = 5G-IA2 algorithm ID
  ```
  - Function: `amf_derive_security_keys()` → Derives KNAS-int from KAMF
- **KNAS-enc** (Encryption):
  ```
  KNAS-enc = KDF(KAMF, 0x69, 0x01 || 0x00)
  // 0x01 = encryption algorithm type
  // 0x00 = 5G-EA0 algorithm ID
  ```
  - Function: `amf_derive_security_keys()` → Derives KNAS-enc from KAMF

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
- Function: `amf_calculate_mac()` → Constructs input and computes AES-CMAC

### Algorithm Selection Logic
From UE capabilities and AMF configuration:

**Integrity Algorithm Selection**:
- UE supports: IA0=1, IA1=1, IA2=1, IA3=1
- AMF priority: [IA2, IA1, IA3, IA0]
- Selected: IA2 (128-bit AES)
- Function: `amf_select_security_algorithms()` → Matches UE capabilities with AMF priorities

**Encryption Algorithm Selection**:
- UE supports: EA0=1, EA1=0, EA2=0, EA3=0
- AMF priority: [EA2, EA1, EA3, EA0]
- Selected: EA0 (null encryption) - only supported algorithm
- Function: `amf_select_security_algorithms()` → Selects only mutually supported algorithm

### Replayed UE Security Capabilities
Exact copy from registration request to prevent modification attacks:
- 5G algorithms: EA0=1, IA0=1, IA1=1, IA2=1, IA3=1
- EPS algorithms: EEA0=1, EIA0=1, EIA1=1, EIA2=1, EIA3=1
- Total: 4 bytes (32 bits of capability flags)
- Function: `amf_build_security_mode_command()` → Copies exact capabilities from stored registration

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

## Binary Message Structures

### Authentication Response Structure

```c
#pragma pack(1)
typedef struct {
    // NAS header
    uint8_t epd;                      // 0x7E (5GS)
    uint8_t security_header_type;     // 0x00 (plain NAS message)
    uint8_t message_type;             // 0x57 (Authentication Response)
    
    // Authentication response parameter (Type 4 IE)
    uint8_t elem_id;                  // 0x2D
    uint8_t length;                   // 0x10 (16 bytes)
    uint8_t res_star[16];             // RES* value
} authentication_response_t;

// Static assert to ensure correct size
_Static_assert(sizeof(authentication_response_t) == 21, "authentication_response_t size mismatch");

// Field offsets for direct access
_Static_assert(offsetof(authentication_response_t, epd) == 0, "epd offset mismatch");
_Static_assert(offsetof(authentication_response_t, security_header_type) == 1, "security_header_type offset mismatch");
_Static_assert(offsetof(authentication_response_t, message_type) == 2, "message_type offset mismatch");
_Static_assert(offsetof(authentication_response_t, elem_id) == 3, "elem_id offset mismatch");
_Static_assert(offsetof(authentication_response_t, length) == 4, "length offset mismatch");
_Static_assert(offsetof(authentication_response_t, res_star) == 5, "res_star offset mismatch");
```

### Security Mode Command Structure

```c
#pragma pack(1)
typedef struct {
    // Security header
    uint8_t epd;                      // 0x7E (5GS)
    uint8_t security_header_type;     // 0x03 (integrity protected with new security context)
    uint8_t mac[4];                   // Message Authentication Code
    uint8_t sequence_number;          // 0x00 (first protected message)
    
    // Plain NAS message container
    uint8_t inner_epd;                // 0x7E (5GS)
    uint8_t inner_security_header;    // 0x00 (plain message, includes spare half octet)
    uint8_t message_type;             // 0x5D (Security Mode Command)
    
    // Selected NAS security algorithms (Type 3)
    struct {
        uint8_t type_of_ciphering : 4;  // 0x0 (5G-EA0)
        uint8_t spare1 : 4;             // 0x0
        uint8_t type_of_integrity : 4;  // 0x2 (5G-IA2)
        uint8_t spare2 : 4;             // 0x0
    } selected_algorithms;
    
    // NAS key set identifier (Type 3)
    struct {
        uint8_t nas_key_set_id : 3;     // 0x0
        uint8_t tsc : 1;                // 0x0 (native security context)
        uint8_t spare : 4;              // 0x0
    } ngksi;
    
    // Replayed UE security capabilities (Type 4)
    uint8_t ue_sec_cap_iei;            // 0x00 (no IEI for mandatory IE)
    uint8_t ue_sec_cap_length;         // 0x00 (no length for mandatory IE)
    uint8_t ue_sec_cap_iei_actual;     // 0x22 (actual IEI)
    uint8_t ue_sec_cap_length_actual;  // 0x04
    struct {
        // 5G encryption algorithms
        uint8_t ea7 : 1;               // 0
        uint8_t ea6 : 1;               // 0
        uint8_t ea5 : 1;               // 0
        uint8_t ea4 : 1;               // 0
        uint8_t ea3 : 1;               // 0
        uint8_t ea2 : 1;               // 0
        uint8_t ea1 : 1;               // 0
        uint8_t ea0 : 1;               // 1
        
        // 5G integrity algorithms
        uint8_t ia7 : 1;               // 0
        uint8_t ia6 : 1;               // 0
        uint8_t ia5 : 1;               // 0
        uint8_t ia4 : 1;               // 0
        uint8_t ia3 : 1;               // 1
        uint8_t ia2 : 1;               // 1
        uint8_t ia1 : 1;               // 1
        uint8_t ia0 : 1;               // 1
        
        // EPS encryption algorithms
        uint8_t eea7 : 1;              // 0
        uint8_t eea6 : 1;              // 0
        uint8_t eea5 : 1;              // 0
        uint8_t eea4 : 1;              // 0
        uint8_t eea3 : 1;              // 0
        uint8_t eea2 : 1;              // 0
        uint8_t eea1 : 1;              // 0
        uint8_t eea0 : 1;              // 1
        
        // EPS integrity algorithms
        uint8_t eia7 : 1;              // 0
        uint8_t eia6 : 1;              // 0
        uint8_t eia5 : 1;              // 0
        uint8_t eia4 : 1;              // 0
        uint8_t eia3 : 1;              // 1
        uint8_t eia2 : 1;              // 1
        uint8_t eia1 : 1;              // 1
        uint8_t eia0 : 1;              // 1
    } ue_security_capability;
    
    // IMEISV request (Type 1)
    struct {
        uint8_t imeisv_request : 3;     // 0x1
        uint8_t spare : 1;              // 0x0
        uint8_t iei : 4;                // 0xE
    } imeisv_request;
    
    // Additional 5G security information (Type 4)
    uint8_t additional_sec_info_iei;    // 0x36
    uint8_t additional_sec_info_length; // 0x01
    struct {
        uint8_t spare : 6;              // 0x00
        uint8_t hdp : 1;                // 0x0 (no horizontal derivation)
        uint8_t rinmr : 1;              // 0x1 (retransmission requested)
    } additional_security_info;
    
} security_mode_command_t;

// Static assert to ensure correct size
_Static_assert(sizeof(security_mode_command_t) == 21, "security_mode_command_t size mismatch");

// Field offsets for direct access
_Static_assert(offsetof(security_mode_command_t, epd) == 0, "epd offset mismatch");
_Static_assert(offsetof(security_mode_command_t, security_header_type) == 1, "security_header_type offset mismatch");
_Static_assert(offsetof(security_mode_command_t, mac) == 2, "mac offset mismatch");
_Static_assert(offsetof(security_mode_command_t, sequence_number) == 6, "sequence_number offset mismatch");
_Static_assert(offsetof(security_mode_command_t, inner_epd) == 7, "inner_epd offset mismatch");
_Static_assert(offsetof(security_mode_command_t, message_type) == 9, "message_type offset mismatch");
_Static_assert(offsetof(security_mode_command_t, selected_algorithms) == 10, "selected_algorithms offset mismatch");
_Static_assert(offsetof(security_mode_command_t, ngksi) == 12, "ngksi offset mismatch");
_Static_assert(offsetof(security_mode_command_t, ue_security_capability) == 17, "ue_security_capability offset mismatch");
_Static_assert(offsetof(security_mode_command_t, imeisv_request) == 21, "imeisv_request offset mismatch");
_Static_assert(offsetof(security_mode_command_t, additional_sec_info_iei) == 22, "additional_sec_info_iei offset mismatch");
_Static_assert(offsetof(security_mode_command_t, additional_security_info) == 24, "additional_security_info offset mismatch");
```