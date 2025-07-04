# ROADMAP-phase1.md Implementation Plan

## Parse Tree Analysis and Field Derivation

### Input Message Parse Tree: Registration Request
```json
{
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS)
  "nas_5gs.security_header_type": "0",     // Plain NAS message
  "nas_5gs.mm.message_type": "0x41",       // Registration Request
  "5GS registration type": {
    "nas_5gs.mm.for": "1",                 // Follow-on Request flag
    "nas_5gs.mm.5gs_reg_type": "1"         // Initial registration
  },
  "NAS key set identifier": {
    "nas_5gs.mm.tsc.h1": "0",             // Type of Security Context
    "nas_5gs.mm.nas_key_set_id.h1": "7"   // Key Set Identifier
  },
  "5GS mobile identity": {
    "nas_5gs.mm.type_id": "1",            // SUCI type
    "e212.mcc": "999",                     // Mobile Country Code
    "e212.mnc": "70",                      // Mobile Network Code  
    "nas_5gs.mm.suci.msin": "0000000001", // Mobile Station ID
    "nas_5gs.mm.suci.scheme_id": "0"       // Null protection scheme
  },
  "UE security capability": {
    "nas_5gs.mm.5g_ea0": "1",             // 5G-EA0 supported
    "nas_5gs.mm.ia0": "1",                // 5G-IA0 supported
    "nas_5gs.mm.5g_128_ia1": "1",         // 5G-IA1 supported
    "nas_5gs.mm.5g_128_ia2": "1",         // 5G-IA2 supported
    "nas_5gs.mm.5g_128_ia3": "1"          // 5G-IA3 supported
  }
}
```

### Output Message Parse Tree: Authentication Request
```json
{
  "per.octet_string_length": "42",         // Total NAS PDU length → See "Protocol Encoding Fields" section
  "ngap.NAS_PDU": "7e:00:56:00:02:00:00:21:5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69:20:10:40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19", // Complete encoded NAS PDU → See "Container Structures" section
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS) → See "Static Fields" section
  "nas_5gs.spare_half_octet": "0",         // Reserved bits → See "Static Fields" section
  "nas_5gs.security_header_type": "0",     // Plain NAS message → See "Static Fields" section
  "nas_5gs.mm.message_type": "0x56",       // Authentication Request → See "Static Fields" section
  "NAS key set identifier - ngKSI": {
    "nas_5gs.mm.tsc": "0",                // Type of Security Context → See "Derived Fields from Registration Request" section
    "nas_5gs.mm.nas_key_set_id": "0"      // New Key Set Identifier → See "Derived Fields from Registration Request" section
  },
  "ABBA": {
    "gsm_a.len": "2",                      // ABBA length → See "Length Calculations" section
    "nas_5gs.mm.abba_contents": "00:00"   // Anti-Bidding down Between Architectures → See "Static Fields" section
  },
  "Authentication Parameter RAND": {
    "gsm_a.dtap.elem_id": "0x21",         // RAND element identifier → See "Information Element Identifiers" section
    "gsm_a.dtap.rand": "5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69" // → See "Generated Fields (from UDM/AUSF)" section
  },
  "Authentication Parameter AUTN": {
    "gsm_a.dtap.elem_id": "0x20",         // AUTN element identifier → See "Information Element Identifiers" section
    "gsm_a.len": "16",                     // AUTN length → See "Length Calculations" section
    "gsm_a.dtap.autn": "40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19", // Complete AUTN → See "Container Structures" section
    "gsm_a.dtap.autn.sqn_xor_ak": "40:62:96:99:30:82",  // SQN ⊕ AK → See "Generated Fields (from UDM/AUSF)" section
    "gsm_a.dtap.autn.amf": "80:00",                      // AMF field → See "Generated Fields (from UDM/AUSF)" section
    "gsm_a.dtap.autn.mac": "30:b7:62:45:5c:89:0b:19"    // MAC → See "Generated Fields (from UDM/AUSF)" section
  }
}
```

## Field Derivation Logic

### Static Fields (Direct Assignment)
1. **EPD (126)**: Always 0x7E for 5GS messages
   - Function: `amf_build_authentication_request()`
2. **Spare Half Octet (0)**: Reserved bits, always 0
   - Function: `amf_build_authentication_request()`
3. **Security Header Type (0)**: Plain message (no security context yet)
   - Function: `amf_build_authentication_request()`
4. **Message Type (0x56)**: Authentication Request constant
   - Function: `amf_build_authentication_request()`
5. **ABBA Contents (00:00)**: Default anti-bidding down parameter
   - Function: `amf_build_authentication_request()`

### Derived Fields from Registration Request
1. **NAS KSI TSC (0)**: Reset from registration request value (new security context)
   - Function: `amf_build_authentication_request()` → sets to 0
2. **NAS KSI ID (0)**: Reset from 7 → 0 (new key set identifier)
   - Function: `amf_build_authentication_request()` → sets to 0
3. **SUCI Extraction**: MCC=999, MNC=70, MSIN=0000000001 → Used for AUSF lookup
   - Function: `amf_extract_suci_from_mobile_identity()`

### Generated Fields (from UDM/AUSF)
1. **RAND (16 bytes)**: Random challenge from authentication vector
   - Value: `5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69`
   - Function: `udm_generate_authentication_vector()` → generates RAND
2. **AUTN Components**:
   - SQN ⊕ AK (6 bytes): `40:62:96:99:30:82` - Sequence number XOR anonymity key
     - Function: `udm_generate_authentication_vector()` → calculates SQN⊕AK
   - AMF (2 bytes): `80:00` - Authentication Management Field
     - Function: `udm_generate_authentication_vector()` → sets AMF bits
   - MAC (8 bytes): `30:b7:62:45:5c:89:0b:19` - Message Authentication Code
     - Function: `udm_generate_authentication_vector()` → calculates MAC = f1(K, SQN||RAND||AMF)

### Protocol Encoding Fields
1. **PER Octet String Length (42)**: Total length of the NAS PDU in octets
   - Calculation: 1 (EPD) + 1 (Security header) + 1 (Message type) + 1 (ngKSI) + 3 (ABBA with length) + 1 (RAND IE ID) + 16 (RAND) + 1 (AUTN IE ID) + 1 (AUTN length) + 16 (AUTN) = 42 bytes
   - Function: `amf_build_authentication_request()` → calculated during message assembly

### Information Element Identifiers
1. **RAND Element ID (0x21)**: Fixed identifier for Authentication Parameter RAND IE
   - Function: `amf_build_authentication_request()` → sets IE identifier
2. **AUTN Element ID (0x20)**: Fixed identifier for Authentication Parameter AUTN IE
   - Function: `amf_build_authentication_request()` → sets IE identifier
   - These IDs are defined in 3GPP TS 24.501 Table 9.11.3.16.1

### Length Calculations
1. **ABBA Length (2)**: Length of ABBA contents field
   - Fixed at 2 bytes for this implementation (1 byte length + 1 byte content)
   - Function: `amf_build_authentication_request()` → sets length field
2. **AUTN Length (16)**: Fixed length of AUTN parameter
   - Always 16 bytes per 3GPP TS 33.501
   - Function: `amf_build_authentication_request()` → sets length field

### Container Structures
1. **NGAP NAS PDU**: Complete encoded NAS message as hex string
   - Value: `7e:00:56:00:02:00:00:21:5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69:20:10:40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19`
   - This is the complete Authentication Request message encoded for transmission
   - Function: `amf_build_authentication_request()` → assembles complete NAS PDU
2. **AUTN Complete Field**: Concatenated AUTN before decomposition
   - Value: `40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19`
   - Structure: SQN⊕AK || AMF || MAC (6 + 2 + 8 = 16 bytes)
   - Function: `udm_generate_authentication_vector()` → creates concatenated AUTN

## Implementation Function Chain

### Phase 1: NFLambda AMF Function Implementation

```
amf_handle_registration_request()
    ↓
amf_extract_suci_from_mobile_identity()
    ↓ 
amf_validate_registration_request()
    ↓
ausf_authenticate_request()
    ↓
udm_generate_authentication_vector()
    ↓
amf_build_authentication_request()
    ↓
amf_send_authentication_request()
```

### Detailed Function Specifications

#### 1. `amf_handle_registration_request()`
**Input**: Registration Request binary structure
**Output**: Trigger authentication flow
**Logic**:
- Extract registration type from input structure (nas_5gs.mm.5gs_reg_type)
- Extract NAS key set identifier (nas_5gs.mm.nas_key_set_id.h1)
- Extract UE security capabilities bitmap
- Store extracted values in UE context

#### 2. `amf_extract_suci_from_mobile_identity()`  
**Input**: Mobile Identity IE from registration request
**Output**: SUCI structure
**Logic**:
- Verify mobile identity type field equals 1 (SUCI)
- Extract MCC (999), MNC (70), MSIN (0000000001) from binary fields
- Check protection scheme field equals 0 (null scheme)
- Format as SUCI string for AUSF interface

#### 3. `amf_validate_registration_request()`
**Input**: Registration request components  
**Output**: Validation result
**Logic**:
- Check cleartext IEs only (per TS 33.501)
- Validate SUCI format
- Check registration type
- Validate UE security capabilities

#### 4. `ausf_authenticate_request()`
**Input**: SUCI, serving network name
**Output**: Authentication vectors from AUSF
**Logic**:
- Build AuthenticationInfo request
- Set supiOrSuci field
- Set servingNetworkName: "5G:mnc070.mcc999.3gppnetwork.org"
- Send HTTP POST to AUSF

#### 5. `udm_generate_authentication_vector()`
**Input**: SUCI, serving network
**Output**: 5G AKA authentication data
**Logic**:
- Decrypt SUCI to SUPI (if encrypted)
- Retrieve subscriber key K
- Generate RAND (16 random bytes)
- Calculate AUTN using Milenage:
  - Calculate XRES*, CK', IK', AK
  - Calculate MAC = f1(K, SQN||RAND||AMF)
  - Calculate AUTN = SQN⊕AK || AMF || MAC
- Calculate HXRES* = SHA-256(RAND || XRES*)

#### 6. `amf_build_authentication_request()`
**Input**: Authentication vectors (RAND, AUTN)
**Output**: Authentication Request NAS PDU
**Logic**:
- Set message type = 0x56
- Set ngKSI.tsc = 0, ngKSI.ksi = 0  
- Set ABBA = 0x0000
- Copy RAND (16 bytes)
- Copy AUTN (16 bytes)
- Encode as plain NAS message

#### 7. `amf_send_authentication_request()`
**Input**: Authentication Request NAS PDU
**Output**: Message sent to RAN
**Logic**:
- Get RAN UE context
- Send via NGAP interface
- Start T3560 timer for response timeout

## Detailed Field Calculations

### Authentication Parameter RAND
- **Source**: Generated by UDM using cryptographically secure random number generator
- **Size**: 128 bits (16 bytes)
- **Example Value**: `5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69`
- **Purpose**: Challenge for mutual authentication

### Authentication Parameter AUTN Structure
Total size: 128 bits (16 bytes)

#### SQN ⊕ AK (48 bits / 6 bytes)
- **Value**: `40:62:96:99:30:82`
- **Calculation**: SQN XOR AK
  - SQN: Sequence number from HSS/UDM (48 bits)
  - AK: Anonymity Key = f5(K, RAND) using Milenage
  - Result hides the sequence number for privacy

#### AMF (16 bits / 2 bytes)  
- **Value**: `80:00`
- **Purpose**: Authentication Management Field
- **Bit allocation**:
  - Bit 0: Separation bit (set to 0 for 5G AKA)
  - Bits 1-7: Reserved for operator use
  - Bits 8-15: AMF field for algorithm identification

#### MAC (64 bits / 8 bytes)
- **Value**: `30:b7:62:45:5c:89:0b:19`  
- **Calculation**: MAC = f1(K, SQN || RAND || AMF)
- **Purpose**: Message authentication code for AUTN integrity

### NAS Key Set Identifier Transformation
- **Input KSI**: 7 (from registration request)
- **Output KSI**: 0 (reset for new authentication)
- **Reason**: New security context establishment requires fresh key set

### ABBA Parameter
- **Value**: `00:00` (2 bytes)
- **Purpose**: Anti-Bidding down Between Architectures
- **Function**: Prevents downgrade attacks from 5G to 4G
- **Format**: Length (1 byte) + Contents (variable, here 1 byte = 0x00)

## Open5GS Code Reference Mapping

### SUCI Extraction
**File**: `open5gs/src/amf/gmm-handler.c:146-184`
```c
switch (mobile_identity->h.type) {
case OGS_NAS_5GS_MOBILE_IDENTITY_SUCI:
    amf_ue_set_suci(amf_ue, mobile_identity);
    ogs_nas_5gs_suci_from_mobile_identity(mobile_identity);
```

### Authentication Vector Processing  
**File**: `open5gs/src/amf/nausf-handler.c:23-160`
```c
_5g_aka = response->authentication_vector->hxres_star_av->_5g_aka;
amf_ue->rand = _5g_aka->rand;
amf_ue->hxres_star = _5g_aka->hxres_star;
amf_ue->autn = _5g_aka->autn;
```

### Authentication Request Building
**File**: `open5gs/src/amf/gmm-build.c:354-385`
```c
message.gmm.h.message_type = OGS_NAS_5GS_AUTHENTICATION_REQUEST;
authentication_request->ngksi.tsc = amf_ue->nas.tsc;
authentication_request->ngksi.ksi = amf_ue->nas.ksi;
authentication_request->authentication_parameter_rand.rand = amf_ue->rand;
authentication_request->authentication_parameter_autn.autn = amf_ue->autn;
```

## Binary Message Structures

### Registration Request Message Structure

```c
#pragma pack(1)  // Ensure no padding between fields

// Registration Request structure matching the binary layout
// Hex: 7e:00:41:79:00:0d:01:99:f9:07:00:00:00:00:00:00:00:00:10:2e:04:80:f0:80:f0
typedef struct {
    // Header (3 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header;            // 0x00 - Security header (bits 7-4: spare, bits 3-0: type)
    uint8_t message_type;               // 0x41 - Registration Request
    
    // Registration type and ngKSI (1 byte)
    union {
        uint8_t reg_type_and_ksi;       // 0x79
        struct {
            uint8_t reg_type:3;         // bits 2-0: Registration type (1 = initial)
            uint8_t for_bit:1;          // bit 3: Follow-on request (1)
            uint8_t ksi:3;              // bits 6-4: Key set identifier (7)
            uint8_t tsc:1;              // bit 7: Type of security context (0)
        } bits;
    } reg_type_ksi;
    
    // Mobile Identity header (2 bytes)
    uint8_t spare_half_octet;           // 0x00 - Spare half octet
    uint8_t mobile_id_length;           // 0x0D - Length (13 bytes)
    
    // Mobile Identity - SUCI (1 byte header)
    union {
        uint8_t suci_header;            // 0x01
        struct {
            uint8_t type_id:3;          // bits 2-0: Identity type (1 = SUCI)
            uint8_t spare_b3:1;         // bit 3: Spare
            uint8_t supi_format:3;      // bits 6-4: SUPI format (0 = IMSI)
            uint8_t spare_b7:1;         // bit 7: Spare
        } bits;
    } suci_hdr;
    
    // PLMN (3 bytes in BCD format)
    union {
        uint8_t bytes[3];               // 0x99, 0xF9, 0x07
        struct {
            uint8_t mcc_digit2:4;       // MCC digit 2
            uint8_t mcc_digit1:4;       // MCC digit 1
            uint8_t mnc_digit3:4;       // MNC digit 3 (F = filler)
            uint8_t mcc_digit3:4;       // MCC digit 3
            uint8_t mnc_digit2:4;       // MNC digit 2
            uint8_t mnc_digit1:4;       // MNC digit 1
        } digits;
    } plmn;
    
    // Routing indicator (2 bytes)
    uint16_t routing_indicator;         // 0x0000
    
    // Protection scheme (1 byte)
    union {
        uint8_t prot_scheme_byte;       // 0x00
        struct {
            uint8_t prot_scheme_id:4;   // bits 3-0: Protection scheme (0 = null)
            uint8_t home_net_pki:4;     // bits 7-4: Home network public key ID
        } bits;
    } protection;
    
    // MSIN in BCD format (5 bytes)
    union {
        uint8_t bytes[5];               // 0x00:00:00:00:10
        struct {
            uint8_t digit2:4;
            uint8_t digit1:4;
            uint8_t digit4:4;
            uint8_t digit3:4;
            uint8_t digit6:4;
            uint8_t digit5:4;
            uint8_t digit8:4;
            uint8_t digit7:4;
            uint8_t digit10:4;
            uint8_t digit9:4;
        } digits;
    } msin;
    
    // UE Security Capability IE
    uint8_t ue_sec_cap_iei;             // 0x2E - UE security capability IEI
    uint8_t ue_sec_cap_length;          // 0x04 - Length (4 bytes)
    
    // Security algorithms (4 bytes)
    struct {
        union {
            uint8_t byte;               // 0x80
            struct {
                uint8_t ea7:1;          // bit 0: 5G-EA7
                uint8_t ea6:1;          // bit 1: 5G-EA6
                uint8_t ea5:1;          // bit 2: 5G-EA5
                uint8_t ea4:1;          // bit 3: 5G-EA4
                uint8_t ea3:1;          // bit 4: 5G-EA3
                uint8_t ea2:1;          // bit 5: 5G-EA2
                uint8_t ea1:1;          // bit 6: 5G-EA1
                uint8_t ea0:1;          // bit 7: 5G-EA0 (1 = supported)
            } bits;
        } _5g_ea;
        
        union {
            uint8_t byte;               // 0xF0
            struct {
                uint8_t ia7:1;          // bit 0: 5G-IA7
                uint8_t ia6:1;          // bit 1: 5G-IA6
                uint8_t ia5:1;          // bit 2: 5G-IA5
                uint8_t ia4:1;          // bit 3: 5G-IA4
                uint8_t ia3:1;          // bit 4: 5G-IA3 (1)
                uint8_t ia2:1;          // bit 5: 5G-IA2 (1)
                uint8_t ia1:1;          // bit 6: 5G-IA1 (1)
                uint8_t ia0:1;          // bit 7: 5G-IA0 (1)
            } bits;
        } _5g_ia;
        
        union {
            uint8_t byte;               // 0x80
            struct {
                uint8_t eea7:1;         // bit 0: EEA7
                uint8_t eea6:1;         // bit 1: EEA6
                uint8_t eea5:1;         // bit 2: EEA5
                uint8_t eea4:1;         // bit 3: EEA4
                uint8_t eea3:1;         // bit 4: EEA3
                uint8_t eea2:1;         // bit 5: EEA2
                uint8_t eea1:1;         // bit 6: EEA1
                uint8_t eea0:1;         // bit 7: EEA0 (1 = supported)
            } bits;
        } eps_ea;
        
        union {
            uint8_t byte;               // 0xF0
            struct {
                uint8_t eia7:1;         // bit 0: EIA7
                uint8_t eia6:1;         // bit 1: EIA6
                uint8_t eia5:1;         // bit 2: EIA5
                uint8_t eia4:1;         // bit 3: EIA4
                uint8_t eia3:1;         // bit 4: EIA3 (1)
                uint8_t eia2:1;         // bit 5: EIA2 (1)
                uint8_t eia1:1;         // bit 6: EIA1 (1)
                uint8_t eia0:1;         // bit 7: EIA0 (1)
            } bits;
        } eps_ia;
    } ue_security_capability;
} registration_request_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(registration_request_t) == 25, "Registration Request structure size mismatch");
```

### Authentication Request Message Structure

```c
// Authentication Request structure matching the binary layout
// Hex: 7e:00:56:00:02:00:00:21:5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69:20:10:40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19
typedef struct {
    // Header (3 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header;            // 0x00 - Security header
    uint8_t message_type;               // 0x56 - Authentication Request
    
    // ngKSI (1 byte)
    union {
        uint8_t ngksi_spare;            // 0x00
        struct {
            uint8_t ksi:3;              // bits 2-0: Key set identifier (0)
            uint8_t tsc:1;              // bit 3: Type of security context (0)
            uint8_t spare:4;            // bits 7-4: Spare half octet
        } bits;
    } ngksi;
    
    // ABBA
    uint8_t abba_length;                // 0x02 - Length
    uint8_t abba[2];                    // 0x00:00 - ABBA contents
    
    // Authentication Parameter RAND
    uint8_t rand_iei;                   // 0x21 - RAND IEI
    uint8_t rand[16];                   // 16 bytes of RAND value
    
    // Authentication Parameter AUTN
    uint8_t autn_iei;                   // 0x20 - AUTN IEI
    uint8_t autn_length;                // 0x10 - Length (16)
    struct {
        uint8_t sqn_xor_ak[6];          // SQN XOR AK (6 bytes)
        uint8_t amf[2];                 // AMF field (2 bytes)
        uint8_t mac[8];                 // MAC (8 bytes)
    } autn;
} authentication_request_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(authentication_request_t) == 42, "Authentication Request structure size mismatch");
```