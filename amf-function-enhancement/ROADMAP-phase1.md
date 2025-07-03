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
2. **Spare Half Octet (0)**: Reserved bits, always 0
3. **Security Header Type (0)**: Plain message (no security context yet)
4. **Message Type (0x56)**: Authentication Request constant
5. **ABBA Contents (00:00)**: Default anti-bidding down parameter

### Derived Fields from Registration Request
1. **NAS KSI TSC (0)**: Reset from registration request value (new security context)
2. **NAS KSI ID (0)**: Reset from 7 → 0 (new key set identifier)
3. **SUCI Extraction**: MCC=999, MNC=70, MSIN=0000000001 → Used for AUSF lookup

### Generated Fields (from UDM/AUSF)
1. **RAND (16 bytes)**: Random challenge from authentication vector
   - Value: `5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69`
2. **AUTN Components**:
   - SQN ⊕ AK (6 bytes): `40:62:96:99:30:82` - Sequence number XOR anonymity key
   - AMF (2 bytes): `80:00` - Authentication Management Field  
   - MAC (8 bytes): `30:b7:62:45:5c:89:0b:19` - Message Authentication Code

### Protocol Encoding Fields
1. **PER Octet String Length (42)**: Total length of the NAS PDU in octets
   - Calculation: 1 (EPD) + 1 (Security header) + 1 (Message type) + 1 (ngKSI) + 3 (ABBA with length) + 1 (RAND IE ID) + 16 (RAND) + 1 (AUTN IE ID) + 1 (AUTN length) + 16 (AUTN) = 42 bytes

### Information Element Identifiers
1. **RAND Element ID (0x21)**: Fixed identifier for Authentication Parameter RAND IE
2. **AUTN Element ID (0x20)**: Fixed identifier for Authentication Parameter AUTN IE
   - These IDs are defined in 3GPP TS 24.501 Table 9.11.3.16.1

### Length Calculations
1. **ABBA Length (2)**: Length of ABBA contents field
   - Fixed at 2 bytes for this implementation (1 byte length + 1 byte content)
2. **AUTN Length (16)**: Fixed length of AUTN parameter
   - Always 16 bytes per 3GPP TS 33.501

### Container Structures
1. **NGAP NAS PDU**: Complete encoded NAS message as hex string
   - Value: `7e:00:56:00:02:00:00:21:5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69:20:10:40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19`
   - This is the complete Authentication Request message encoded for transmission
2. **AUTN Complete Field**: Concatenated AUTN before decomposition
   - Value: `40:62:96:99:30:82:80:00:30:b7:62:45:5c:89:0b:19`
   - Structure: SQN⊕AK || AMF || MAC (6 + 2 + 8 = 16 bytes)

## Implementation Function Chain

### Phase 1: NFLambda AMF Function Implementation

```
nflambda_amf_handle_registration_request()
    ↓
extract_suci_from_mobile_identity()
    ↓ 
validate_registration_request()
    ↓
ausf_authenticate_request()
    ↓
udm_generate_authentication_vector()
    ↓
build_authentication_request()
    ↓
send_authentication_request()
```

### Detailed Function Specifications

#### 1. `nflambda_amf_handle_registration_request()`
**Input**: Registration Request NAS PDU
**Output**: Trigger authentication flow
**Logic**:
- Parse NAS PDU using ASN.1 decoder
- Extract registration type and validate
- Extract UE security capabilities
- Store in UE context

#### 2. `extract_suci_from_mobile_identity()`  
**Input**: Mobile Identity IE from registration request
**Output**: SUCI structure
**Logic**:
- Parse mobile identity type (must be SUCI = 1)
- Extract MCC, MNC, MSIN
- Validate protection scheme
- Convert to string format for AUSF

#### 3. `validate_registration_request()`
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

#### 6. `build_authentication_request()`
**Input**: Authentication vectors (RAND, AUTN)
**Output**: Authentication Request NAS PDU
**Logic**:
- Set message type = 0x56
- Set ngKSI.tsc = 0, ngKSI.ksi = 0  
- Set ABBA = 0x0000
- Copy RAND (16 bytes)
- Copy AUTN (16 bytes)
- Encode as plain NAS message

#### 7. `send_authentication_request()`
**Input**: Authentication Request NAS PDU
**Output**: Message sent to RAN
**Logic**:
- Get RAN UE context
- Send via NGAP interface
- Start T3560 timer for response timeout

## NFLambda Integration Points

### Event-Driven Architecture
```
EVENT_REGISTRATION_REQUEST → amf_actor
    ↓
EVENT_AUSF_AUTH_REQUEST → ausf_client_actor  
    ↓
EVENT_UDM_AUTH_VECTOR → udm_client_actor
    ↓
EVENT_AUTH_REQUEST_BUILD → nas_builder_actor
    ↓  
EVENT_NAS_SEND → ngap_sender_actor
```

### Memory Pool Usage
- **UE Context Pool**: Store SUCI, security capabilities
- **Message Pool**: Authentication request buffers
- **Crypto Pool**: RAND, AUTN storage

### Actor Message Definitions
```c
typedef struct {
    uint32_t ue_id;
    uint8_t* nas_pdu;
    size_t nas_len;
} registration_request_msg_t;

typedef struct {
    uint32_t ue_id; 
    char* suci;
    char* serving_network;
} ausf_auth_request_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t rand[16];
    uint8_t autn[16]; 
    uint8_t hxres_star[16];
} auth_vector_msg_t;
```

### Error Handling
- Invalid SUCI format → Reject with cause
- AUSF unreachable → Retry with backoff
- Authentication vector failure → Authentication reject
- Timer expiry → Cleanup UE context

### Testing Strategy
1. **Unit Tests**: Individual function validation
2. **Integration Tests**: Full message flow
3. **Interoperability**: Against open5gs core
4. **Performance**: Message throughput metrics

This implementation maintains the event-driven architecture while replacing hardcoded outputs with proper 5G authentication procedures.

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

## Implementation Priority

### Phase 1a: Core Message Processing
1. NAS PDU parsing and validation
2. SUCI extraction and storage
3. Basic authentication request building

### Phase 1b: AUSF Integration  
1. HTTP client for AUSF communication
2. Authentication vector request/response
3. Error handling and retry logic

### Phase 1c: Cryptographic Functions
1. Milenage algorithm implementation  
2. AUTN generation and validation
3. HXRES* calculation

### Phase 1d: Integration and Testing
1. End-to-end message flow testing
2. Performance optimization
3. Interoperability validation

This roadmap provides a complete blueprint for implementing Phase 1 authentication request generation in the NFLambda AMF while maintaining compatibility with 3GPP specifications and open5gs architecture.