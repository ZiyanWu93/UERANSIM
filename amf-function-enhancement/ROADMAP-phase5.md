# ROADMAP-phase5.md Implementation Plan

## Parse Tree Analysis and Field Derivation

### Input Message Parse Tree: PDU Session Establishment Request
```json
{
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS)
  "nas_5gs.security_header_type": "2",     // Integrity protected and ciphered
  "nas_5gs.msg_auth_code": "0xba0292cd",   // Message authentication code
  "nas_5gs.seq_no": "2",                   // Sequence number
  "nas_5gs.mm.message_type": "0x67",       // UL NAS Transport
  "Payload container type": {
    "nas_5gs.mm.pld_cont_type": "1"        // N1 SM information
  },
  "Payload container": {
    "gsm_a.len": "21",
    "Plain NAS 5GS Message": {
      "nas_5gs.epd": "46",                 // 5GSM (0x2E)
      "nas_5gs.pdu_session_id": "1",       // Session identifier
      "nas_5gs.proc_trans_id": "1",        // Procedure transaction ID
      "nas_5gs.sm.message_type": "0xc1",   // PDU Session Establishment Request
      "Integrity protection maximum data rate": {
        "nas_5gs.sm.int_prot_max_data_rate_ul": "255",  // Full rate
        "nas_5gs.sm.int_prot_max_data_rate_dl": "255"   // Full rate
      },
      "PDU session type": {
        "nas_5gs.sm.pdu_session_type": "1"  // IPv4
      },
      "SSC mode": {
        "nas_5gs.sm.sc_mode": "1"           // SSC mode 1
      },
      "5GSM capability": {
        "nas_5gs.sm.tpmic": "0",            // No TPMI capability
        "nas_5gs.sm.atsss_st": "0",         // No ATSSS support
        "nas_5gs.sm.ept_s1": "0",           // No Ethernet PDU support
        "nas_5gs.sm.mh6_pdu": "0",          // No multi-homed IPv6
        "nas_5gs.sm.rqos": "0"              // No reflective QoS
      },
      "Extended protocol configuration options": {
        "gsm_a.gm.sm.pco_pid": "0x000d",   // DNS Server IPv4 request
        "gsm_a.gm.sm.pco.length": "0x00"   // Empty request
      }
    }
  },
  "PDU session identity 2": {
    "nas_5gs.pdu_session_id": "1"          // Repeated session ID
  },
  "Request type": {
    "nas_5gs.mm.req_type": "1"             // Initial request
  },
  "S-NSSAI": {
    "nas_5gs.mm.sst": "1"                  // eMBB slice type
  },
  "DNN": {
    "nas_5gs.cmn.dnn": "internet"          // Data network name
  }
}
```

### Output Message Parse Tree: PDU Session Establishment Accept
```json
{
  "per.octet_string_length": "86",         // Total NGAP NAS-PDU length → See "Protocol Encoding Fields" section
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS) → See "Protocol Constants" section
  "nas_5gs.security_header_type": "2",     // Integrity protected and ciphered → See "Security-Related Fields" section
  "nas_5gs.msg_auth_code": "0xfbd62d81",   // New message authentication code → See "Security-Related Fields" section
  "nas_5gs.seq_no": "3",                   // Incremented sequence number → See "Security-Related Fields" section
  "ngap.NAS_PDU": "7e02fbd62d8103682e010147...d2", // Complete hex encoding → See "Protocol Encoding Fields" section
  "nas_5gs.mm.message_type": "0x68",       // DL NAS Transport → See "Protocol Constants" section
  "Payload container type": {
    "nas_5gs.mm.elem_id": "0x01",          // Payload container type IE → See "Information Element Identifiers" section
    "nas_5gs.mm.pld_cont_type": "1"        // N1 SM information → See "Protocol Constants" section
  },
  "Payload container": {
    "nas_5gs.mm.elem_id": "0x7b",          // Payload container IE → See "Information Element Identifiers" section
    "gsm_a.len": "71",                     // Container length → See "Length Calculations" section
    "Plain NAS 5GS Message": {
      "nas_5gs.epd": "46",                 // 5GSM (0x2E) → See "Protocol Constants" section
      "nas_5gs.pdu_session_id": "1",       // Same session ID → See "Derived Fields from PDU Session Request" section
      "nas_5gs.proc_trans_id": "1",        // Same PTI → See "Derived Fields from PDU Session Request" section
      "nas_5gs.sm.message_type": "0xc2",   // PDU Session Establishment Accept → See "Protocol Constants" section
      "nas_5gs.sm.sel_sc_mode": "1",       // Selected SSC mode 1 → See "Derived Fields from PDU Session Request" section
      "PDU session type": {
        "nas_5gs.sm.elem_id": "0x59",      // PDU session type IE → See "Information Element Identifiers" section
        "nas_5gs.sm.pdu_session_type": "1"  // IPv4 confirmed → See "Derived Fields from PDU Session Request" section
      },
      "QoS rules": {
        "nas_5gs.sm.elem_id": "0x7a",      // QoS rules IE → See "Information Element Identifiers" section
        "gsm_a.len": "9",                   // Total QoS rules length → See "QoS Rule Creation" section
        "QoS rule 1": {
          "nas_5gs.sm.qos_rule_id": "1",    // Default rule ID → See "QoS Rule Creation" section
          "nas_5gs.sm.length": "6",         // Rule content length → See "QoS Rule Packet Filter Encoding" section
          "nas_5gs.sm.rop": "1",            // Create new QoS rule → See "QoS Rule Creation" section
          "nas_5gs.sm.dqr": "1",            // Default QoS rule → See "QoS Rule Creation" section
          "nas_5gs.sm.nof_pkt_filters": "1", // Number of filters → See "QoS Rule Packet Filter Encoding" section
          "Packet filter 1": {
            "nas_5gs.sm.pkt_flt_dir": "3",  // Bidirectional → See "QoS Rule Packet Filter Encoding" section
            "nas_5gs.sm.pkt_flt_id": "1",   // Filter ID → See "QoS Rule Packet Filter Encoding" section
            "nas_5gs.sm.pf_type": "1",      // Match all packets → See "QoS Rule Packet Filter Encoding" section
            "nas_5gs.sm.pf_length": "1"     // Filter content length → See "QoS Rule Packet Filter Encoding" section
          },
          "nas_5gs.sm.qos_rule_precedence": "255",  // Lowest priority → See "QoS Rule Creation" section
          "nas_5gs.sm.qfi": "1"             // QoS flow identifier → See "QoS Rule Creation" section
        }
      },
      "Session-AMBR": {
        "nas_5gs.sm.elem_id": "0x2a",      // Session-AMBR IE → See "Information Element Identifiers" section
        "gsm_a.len": "6",                   // IE content length → See "Length Calculations" section
        "nas_5gs.sm.unit_for_session_ambr_dl": "3",  // Mbps → See "SMF-Generated Fields" section
        "nas_5gs.sm.session_ambr_dl": "62500",       // 62.5 Mbps → See "Session AMBR Calculation" section
        "nas_5gs.sm.unit_for_session_ambr_ul": "3",  // Mbps → See "SMF-Generated Fields" section
        "nas_5gs.sm.session_ambr_ul": "62500"        // 62.5 Mbps → See "Session AMBR Calculation" section
      },
      "PDU address": {
        "nas_5gs.sm.elem_id": "0x29",      // PDU address IE → See "Information Element Identifiers" section
        "gsm_a.len": "5",                   // IE content length → See "Length Calculations" section
        "nas_5gs.sm.pdu_ses_type": "1",    // IPv4 → See "Derived Fields from PDU Session Request" section
        "nas_5gs.sm.pdu_addr_inf_ipv4": "10.45.0.2",  // Allocated IP → See "IP Address Allocation Algorithm" section
        "nas_5gs.sm.si6lla": "0"           // IPv6 link-local not set → See "PDU Address Encoding" section
      },
      "S-NSSAI": {
        "nas_5gs.sm.elem_id": "0x22",      // S-NSSAI IE → See "Information Element Identifiers" section
        "gsm_a.len": "1",                   // IE content length → See "Length Calculations" section
        "nas_5gs.mm.sst": "1"              // Confirmed slice → See "Derived Fields from PDU Session Request" section
      },
      "QoS flow descriptions": {
        "nas_5gs.sm.elem_id": "0x79",      // QoS flow descriptions IE → See "Information Element Identifiers" section
        "gsm_a.len": "5",                   // IE content length → See "Length Calculations" section
        "QoS flow description 1": {
          "nas_5gs.sm.qfi": "1",           // Default flow → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.op_code": "1",       // Create new QoS flow → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.e": "1",             // Parameters list present → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.nof_params": "1",    // One parameter → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.5qi": "9",           // Default 5QI → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.param_id": "1",      // 5QI parameter → See "QoS Flow Parameter Encoding" section
          "nas_5gs.sm.param_len": "1"      // Parameter length → See "QoS Flow Parameter Encoding" section
        }
      },
      "Extended protocol configuration options": {
        "nas_5gs.sm.elem_id": "0x7b",      // Extended PCO IE → See "Information Element Identifiers" section
        "gsm_a.len": "15",                  // IE content length → See "Extended PCO Structure" section
        "nas_5gs.sm.ext": "1",             // Extension bit → See "Extended PCO Structure" section
        "nas_5gs.sm.config_protocol": "0", // PPP protocol → See "Extended PCO Structure" section
        "gsm_a.gm.sm.pco_pid": "0x000d",  // DNS Server IPv4 response → See "Extended PCO Structure" section
        "gsm_a.gm.sm.pco.length": "0x04",  // Container length → See "Extended PCO Structure" section
        "gsm_a.gm.sm.pco.dns.ipv4": "8.8.4.4"  // DNS server address → See "SMF-Generated Fields" section
      },
      "DNN": {
        "nas_5gs.sm.elem_id": "0x25",      // DNN IE → See "Information Element Identifiers" section
        "gsm_a.len": "9",                   // IE content length → See "DNN Label Encoding" section
        "nas_5gs.cmn.dnn": "internet"      // Echoed DNN → See "DNN Label Encoding" section
      }
    }
  },
  "PDU session identity 2": {
    "nas_5gs.mm.elem_id": "0x12",          // PDU session ID IE → See "Information Element Identifiers" section
    "nas_5gs.pdu_session_id": "1"         // Repeated session ID → See "Derived Fields from PDU Session Request" section
  }
}
```

## Field Derivation Logic

### Protocol Constants
1. **EPD (126)**: Always 0x7E for 5GS messages (3GPP TS 24.501)
2. **5GSM EPD (46)**: Always 0x2E for 5GSM messages
3. **Message Type (0x68)**: DL NAS Transport constant
4. **5GSM Message Type (0xC2)**: PDU Session Establishment Accept
5. **Payload Container Type (1)**: N1 SM information constant

### Protocol Encoding Fields
1. **per.octet_string_length (86)**: Total length of NGAP NAS-PDU
   - Calculated as: Security header (6) + Message content (80)
   - Security header: Type (1) + MAC (4) + Seq (1) = 6 bytes
   - Message content: All fields after security header

2. **ngap.NAS_PDU**: Complete hex encoding of NAS message
   - Starts with 0x7e (EPD) followed by security header
   - Contains all encoded IEs in binary format
   - Used by NGAP layer for transport to RAN

### Information Element Identifiers
1. **0x01**: Payload container type (MM context)
2. **0x7b**: Payload container (MM context)
3. **0x12**: PDU session identity 2 (MM context)
4. **0x59**: PDU session type (SM context)
5. **0x7a**: QoS rules (SM context)
6. **0x2a**: Session-AMBR (SM context)
7. **0x29**: PDU address (SM context)
8. **0x22**: S-NSSAI (SM context)
9. **0x79**: QoS flow descriptions (SM context)
10. **0x7b**: Extended protocol configuration options (SM context)
11. **0x25**: DNN (SM context)

### Length Calculations for Complex IEs
1. **Payload Container (71 bytes)**:
   - 5GSM header: 3 bytes (EPD + PSI + PTI)
   - Message type: 1 byte
   - Selected SSC mode: 1 byte
   - All IEs with TLV encoding: 66 bytes total

2. **QoS Rules (9 bytes)**:
   - Rule ID: 1 byte
   - Length: 2 bytes (0x0006)
   - Rule content: 6 bytes (operation, filters, precedence, QFI)

3. **Session-AMBR (6 bytes)**:
   - DL unit: 1 byte
   - DL value: 2 bytes
   - UL unit: 1 byte
   - UL value: 2 bytes

4. **PDU Address (5 bytes)**:
   - Type + SI6LLA: 1 byte
   - IPv4 address: 4 bytes

5. **QoS Flow Descriptions (5 bytes)**:
   - QFI: 1 byte
   - Operation code + E bit: 1 byte
   - Number of parameters: 1 byte
   - Parameter (5QI): 2 bytes (ID + length + value)

6. **Extended PCO (15 bytes)**:
   - Extension + config protocol: 1 byte
   - Container structure: 14 bytes

7. **DNN (9 bytes)**:
   - Label encoding of "internet": 8 bytes
   - Length byte: 1 byte

### Derived Fields from PDU Session Request
1. **PDU Session ID**: Copied from request (1)
2. **Procedure Transaction ID**: Copied from request (1)
3. **Selected SSC Mode**: Validated and echoed (1)
4. **Selected PDU Session Type**: Validated IPv4 request → IPv4
5. **S-NSSAI**: Validated against allowed slices → SST=1
6. **DNN**: Used for SMF selection, echoed in response

### SMF-Generated Fields
1. **PDU Address (10.45.0.2)**: Allocated from SMF IP pool
   - Pool selection based on DNN and S-NSSAI
   - Sequential or random allocation strategy
   - Address tracking in session context

2. **DNS Server (8.8.4.4)**: From SMF configuration
   - Primary DNS for the selected DNN
   - Can be overridden by policy

### QoS Rule Creation
1. **Rule ID**: 1 (first rule, auto-assigned)
2. **Length**: 6 bytes of rule content
3. **Rule Operation (ROP)**: 1 (create new QoS rule)
4. **DQR bit**: 1 (marks as default QoS rule)
5. **Number of packet filters**: 1
6. **Precedence**: 255 (lowest priority for default rule)
7. **QFI**: 1 (links to default QoS flow)

### QoS Rule Packet Filter Encoding
1. **Packet Filter Direction**: 3 (bidirectional)
   - Encoded in 2 bits: 11b = bidirectional
2. **Packet Filter ID**: 1 (first filter)
   - Encoded in 6 bits: 000001b
3. **Packet Filter Length**: 1 byte
   - Only contains component type for match-all
4. **Component Type**: 1 (match all packets)
   - Special value indicating no specific matching

### QoS Flow Parameter Encoding
1. **QFI**: 1 (default flow identifier, 6 bits)
2. **Operation Code**: 1 (create new QoS flow description)
3. **E bit**: 1 (parameters list is present)
4. **Number of Parameters**: 1 (only 5QI)
5. **Parameter ID**: 1 (5QI parameter)
6. **Parameter Length**: 1 byte
7. **5QI Value**: 9 (default internet QoS)

### Session AMBR Calculation
1. **Input**: Subscription data AMBR
   - Subscribed UL: 100 Mbps
   - Subscribed DL: 100 Mbps

2. **Processing**:
   - Apply slice-specific limits
   - Apply DNN-specific limits
   - Apply session policy

3. **Output**: Session AMBR
   - Session UL: 62.5 Mbps (policy limited)
   - Session DL: 62.5 Mbps (policy limited)
   - Encoding: value in kbps (62500)
   - Unit: 3 (indicates Mbps)

### Extended PCO Structure
1. **Extension bit**: 1 (always set for extended PCO)
2. **Configuration Protocol**: 0 (PPP for compatibility)
3. **Container Structure**:
   - Protocol ID: 0x000D (DNS Server IPv4 Address Request)
   - Length: 0x04 (4 bytes for IPv4 address)
   - Content: IPv4 address (8.8.4.4)

### DNN Label Encoding
1. **Input**: "internet" (8 characters)
2. **Encoding**: DNS label format
   - Length byte: 8
   - ASCII bytes: "internet"
   - Total: 9 bytes (1 + 8)
3. **No compression or null terminator**

### PDU Address Encoding
1. **PDU Session Type**: 1 (IPv4)
   - Encoded in 3 bits: 001b
2. **SI6LLA bit**: 0 (no IPv6 link-local address)
   - Only relevant for IPv6 PDU types
3. **IPv4 Address**: 10.45.0.2
   - 4 bytes in network byte order
   - From SMF IP pool allocation

### Security-Related Fields
1. **Security Header Type**: 2 (integrity protected and ciphered)
2. **Message Authentication Code**: 0xfbd62d81
   - Calculated using NAS integrity algorithm
   - Based on COUNT, direction, and message content
3. **Sequence Number**: 3
   - Incremented from request (2 → 3)
   - Part of NAS COUNT for replay protection

## Implementation Function Chain

### Phase 5: NFLambda AMF/SMF Function Implementation

```
nflambda_amf_handle_ul_nas_transport()
    ↓
extract_pdu_session_request()
    ↓
validate_snssai_and_dnn()
    ↓
discover_and_select_smf()
    ↓
create_sm_context_request()
    ↓
[SMF Processing]
nflambda_smf_handle_create_sm_context()
    ↓
allocate_ip_address()
    ↓
create_default_qos_flows()
    ↓
build_pdu_session_accept()
    ↓
[AMF Processing]
build_dl_nas_transport()
    ↓
send_dl_nas_transport()
```

### Detailed Function Specifications

#### 1. `nflambda_amf_handle_ul_nas_transport()`
**Input**: UL NAS Transport containing PDU Session Request
**Output**: Trigger SMF interaction
**Logic**:
- Decrypt and verify integrity of NAS message
- Extract payload container type (must be N1 SM)
- Extract PDU session ID
- Parse embedded PDU session establishment request
- Create or find session context

#### 2. `extract_pdu_session_request()`
**Input**: Payload container from UL NAS transport
**Output**: Parsed PDU session request structure
**Logic**:
- Parse 5GSM header (EPD, PSI, PTI)
- Extract PDU session type (IPv4/IPv6/IPv4v6)
- Extract SSC mode preference
- Extract integrity protection data rates
- Parse 5GSM capabilities
- Extract extended PCO if present

#### 3. `validate_snssai_and_dnn()`
**Input**: S-NSSAI, DNN from request, UE subscription
**Output**: Validation result with selected slice
**Logic**:
- Check S-NSSAI against UE's allowed slices
- Validate DNN for the requested slice
- Apply local policy (roaming restrictions)
- Check concurrent session limits
- Return selected S-NSSAI or rejection cause

#### 4. `discover_and_select_smf()`
**Input**: S-NSSAI, DNN, TAI, PLMN
**Output**: Selected SMF instance
**Logic**:
- Build NRF discovery request:
  - Service type: NSMF_PDUSESSION
  - S-NSSAI filter
  - DNN filter
  - TAI/PLMN filter
- Query NRF for available SMFs
- Apply selection criteria:
  - Load balancing
  - Locality preference
  - Capacity constraints
- Cache SMF selection for session

#### 5. `create_sm_context_request()`
**Input**: Session info, N1 SM message, UE context
**Output**: HTTP request to SMF
**Logic**:
- Build SmContextCreateData:
  - SUPI from UE context
  - PDU session ID
  - S-NSSAI and DNN
  - UE location (TAI, CGI)
  - AN type (3GPP access)
  - Request type (initial)
- Attach N1 SM message as binary
- Set AMF callback URI

#### 6. `nflambda_smf_handle_create_sm_context()`
**Input**: SM context create request from AMF
**Output**: SM context and PDU session accept
**Logic**:
- Validate request parameters
- Create UE/session context
- Authorize session based on subscription
- Trigger UP session establishment
- Select UPF based on topology
- Build response with session accept

#### 7. `allocate_ip_address()`
**Input**: Session type, DNN, S-NSSAI
**Output**: Allocated IP address
**Logic**:
- Select IP pool based on:
  - DNN configuration
  - S-NSSAI assignment
  - UPF allocation
- Allocate address from pool:
  - Check for static assignment
  - Use DHCP if configured
  - Default to local pool
- Update session context
- Configure UPF with IP rules

#### 8. `create_default_qos_flows()`
**Input**: Session context, subscription data
**Output**: QoS flows and rules
**Logic**:
- Create default QoS flow:
  - QFI = 1
  - 5QI = 9 (default internet)
  - No GBR parameters
- Create default QoS rule:
  - Rule ID = 1
  - Link to QFI = 1
  - Match all filter
  - Precedence = 255
- Set session AMBR from subscription

#### 9. `build_pdu_session_accept()`
**Input**: Session context with allocated resources
**Output**: PDU Session Establishment Accept message
**Logic**:
- Set message type = 0xC2
- Copy PDU session ID and PTI
- Set selected SSC mode
- Set selected PDU session type
- Encode authorized QoS rules
- Set session AMBR values
- Encode PDU address
- Add S-NSSAI
- Encode QoS flow descriptions
- Build extended PCO response
- Add DNN

#### 10. `build_dl_nas_transport()`
**Input**: N1 SM message, session context
**Output**: DL NAS Transport message
**Logic**:
- Set message type = 0x68
- Set payload container type = N1 SM
- Attach PDU session accept as payload
- Set PDU session ID
- Apply NAS security:
  - Calculate MAC
  - Encrypt if required
  - Increment sequence number

#### 11. `send_dl_nas_transport()`
**Input**: DL NAS Transport message
**Output**: Message sent to RAN
**Logic**:
- Get RAN UE NGAP context
- Build NGAP DL NAS transport
- Include N2 SM information if needed
- Send via SCTP to gNB
- Update session state

## NFLambda Integration Points

### Event-Driven Architecture
```
EVENT_UL_NAS_TRANSPORT → amf_nas_actor
    ↓
EVENT_SMF_DISCOVERY → nrf_client_actor
    ↓
EVENT_CREATE_SM_CONTEXT → smf_client_actor
    ↓
EVENT_ALLOCATE_IP → ip_pool_actor
    ↓
EVENT_QOS_SETUP → qos_manager_actor
    ↓
EVENT_BUILD_SESSION_ACCEPT → sm_builder_actor
    ↓
EVENT_DL_NAS_TRANSPORT → ngap_sender_actor
```

### Memory Pool Usage
- **Session Context Pool**: PDU session state
- **IP Address Pool**: Available IPv4/IPv6 addresses
- **QoS Rule Pool**: Dynamic QoS rule allocation
- **Message Pool**: NAS/NGAP message buffers

### Actor Message Definitions
```c
typedef struct {
    uint32_t ue_id;
    uint8_t psi;
    uint8_t* n1_sm_msg;
    size_t n1_sm_len;
    ogs_s_nssai_t s_nssai;
    char* dnn;
} pdu_session_request_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t psi;
    char* smf_uri;
    ogs_sbi_discovery_option_t* discovery_option;
} smf_selection_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t psi;
    uint32_t ipv4_addr;
    uint8_t ipv6_addr[16];
    bool ipv4_allocated;
    bool ipv6_allocated;
} ip_allocation_msg_t;

typedef struct {
    uint32_t ue_id;
    uint8_t psi;
    uint8_t qfi;
    uint8_t _5qi;
    uint32_t session_ambr_ul;
    uint32_t session_ambr_dl;
} qos_setup_msg_t;
```

### Error Handling
- Invalid S-NSSAI/DNN → PDU session reject (cause 27/33)
- SMF discovery failure → PDU session reject (cause 35)
- IP allocation failure → PDU session reject (cause 26)
- QoS setup failure → PDU session reject (cause 31)
- SM context creation failure → Retry or reject

### Inter-Actor Communication
```c
// AMF → NRF Discovery
typedef struct {
    msg_header_t header;
    ogs_sbi_service_type_e service_type;
    ogs_sbi_discovery_option_t* options;
    uint32_t requester_nf_instance_id;
} nrf_discovery_request_t;

// AMF → SMF Context Creation
typedef struct {
    msg_header_t header;
    ogs_sbi_sm_context_create_data_t* create_data;
    uint8_t* n1_sm_msg;
    size_t n1_sm_len;
} smf_context_create_request_t;

// SMF → AMF Context Response
typedef struct {
    msg_header_t header;
    uint32_t sm_context_ref;
    uint8_t* n1_sm_msg;
    size_t n1_sm_len;
    uint8_t* n2_sm_info;
    size_t n2_sm_len;
} smf_context_create_response_t;
```

## Detailed Field Calculations

### IP Address Allocation Algorithm
```
1. Pool Selection:
   - Map DNN to IP pool configuration
   - Apply S-NSSAI-specific pools if configured
   - Default to general pool

2. Address Assignment:
   - Check static assignment table
   - Try DHCP if configured
   - Allocate from local pool:
     - Sequential allocation
     - Random selection
     - Least-recently-used

3. Example Pool Configuration:
   - Pool: 10.45.0.0/24
   - Reserved: 10.45.0.0 - 10.45.0.1
   - Available: 10.45.0.2 - 10.45.0.254
   - Allocated: 10.45.0.2 (first available)
```

### QoS Rule Encoding
```
QoS Rule Structure (9 bytes):
- Rule ID: 1 byte (0x01)
- Length: 2 bytes (0x0006)
- Rule operation: 3 bits (001 = create)
- DQR: 1 bit (1 = default)
- Num filters: 4 bits (0001)
- Packet filter:
  - Direction: 2 bits (11 = bidirectional)
  - Filter ID: 6 bits (000001)
  - Length: 1 byte (0x01)
  - Component type: 1 byte (0x01 = match all)
- Precedence: 1 byte (0xFF)
- QFI: 1 byte (0x01)
```

### Session AMBR Calculation
```
Input: Subscription data AMBR
- Subscribed UL: 100 Mbps
- Subscribed DL: 100 Mbps

Processing:
- Apply slice-specific limits
- Apply DNN-specific limits
- Apply session policy

Output: Session AMBR
- Session UL: 62.5 Mbps (policy limited)
- Session DL: 62.5 Mbps (policy limited)
- Encoding: value/1000 in kbps units
```

### Extended PCO Processing
```
Request PCO:
- Container ID: 0x000D (DNS IPv4 request)
- Length: 0x00 (request indicator)

Response PCO:
- Container ID: 0x000D (DNS IPv4 address)
- Length: 0x04
- DNS Primary: 8.8.4.4 (from SMF config)
- DNS Secondary: 8.8.8.8 (if requested)
```

## Open5GS Code Reference Mapping

### UL NAS Transport Handler
**File**: `src/amf/gmm-handler.c:1214-1376`
```c
gmm_handle_ul_nas_transport() {
    // Extract container type and validate
    payload_container_type = &ul_nas_transport->payload_container_type;
    if (payload_container_type->type != OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION)
        return error;
    
    // Extract PDU session ID
    pdu_session_id = &ul_nas_transport->pdu_session_id;
    
    // Parse GSM header for message type
    gsm_header = (ogs_nas_5gs_gsm_header_t *)payload_container->buffer;
    if (gsm_header->message_type == OGS_NAS_5GS_PDU_SESSION_ESTABLISHMENT_REQUEST)
        // Process session establishment
}
```

### SMF Selection
**File**: `src/amf/gmm-handler.c:1390-1444`
```c
// Build discovery options
discovery_option = ogs_sbi_discovery_option_new();
ogs_sbi_discovery_option_add_snssais(discovery_option, &sess->s_nssai);
ogs_sbi_discovery_option_set_dnn(discovery_option, sess->dnn);

// Discover SMF
amf_sess_sbi_discover_and_send(
    OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION,
    discovery_option,
    amf_nsmf_pdusession_build_create_sm_context);
```

### IP Address Allocation
**File**: `src/smf/context.c`
```c
// Allocate from pool
ogs_pool_alloc(&smf_self()->ue_pool, &sess->ue_ip.addr);
sess->paa.ipv4 = true;
sess->paa.both.addr = sess->ue_ip.addr->addr[0];
```

### PDU Session Accept Building
**File**: `src/smf/gsm-build.c`
```c
gsm_build_pdu_session_establishment_accept() {
    // Set message type
    message.gsm.h.message_type = OGS_NAS_5GS_PDU_SESSION_ESTABLISHMENT_ACCEPT;
    
    // Build QoS rules
    qos_rule[0].identifier = 1;
    qos_rule[0].code = OGS_NAS_QOS_CODE_CREATE_NEW_QOS_RULE;
    qos_rule[0].DQR_bit = 1;
    
    // Set PDU address
    pdu_address->type = OGS_PDU_SESSION_TYPE_IPV4;
    pdu_address->addr = sess->paa.both.addr;
}
```

### DL NAS Transport Building
**File**: `src/amf/gmm-build.c:627-692`
```c
gmm_build_dl_nas_transport() {
    message.gmm.h.message_type = OGS_NAS_5GS_DL_NAS_TRANSPORT;
    
    // Set payload container
    dl_nas_transport->payload_container_type.type = 
        OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION;
    dl_nas_transport->payload_container.buffer = n1smbuf->data;
    
    // Set PDU session ID
    dl_nas_transport->pdu_session_id.value = sess->psi;
}
```

## Implementation Priority

### Phase 5a: Core Message Processing
1. UL NAS transport parsing
2. PDU session request extraction
3. S-NSSAI/DNN validation
4. Basic session context creation

### Phase 5b: SMF Integration
1. NRF discovery client
2. SMF selection logic
3. SM context create/update API
4. Response processing

### Phase 5c: Resource Management
1. IP address pool implementation
2. QoS rule/flow creation
3. Session AMBR handling
4. PCO processing

### Phase 5d: Message Building
1. PDU session accept encoder
2. QoS rule serialization
3. DL NAS transport wrapper
4. Security context application

### Phase 5e: Integration and Testing
1. End-to-end PDU session flow
2. Multiple session handling
3. Error scenarios
4. Performance optimization

## Testing Strategy

### Unit Tests
1. **Message Parsing**: PDU session request decoder
2. **Validation Logic**: S-NSSAI/DNN checks
3. **IP Allocation**: Pool management
4. **QoS Building**: Rule/flow encoding
5. **PCO Processing**: Container handling

### Integration Tests
1. **AMF-SMF Interface**: SM context operations
2. **NRF Discovery**: SMF selection flow
3. **Session Flow**: Complete establishment
4. **Security**: NAS protection verification
5. **Multi-Session**: Concurrent PDU sessions

### Interoperability Tests
1. **UERANSIM**: PDU session with simulator
2. **Open5GS SMF**: Full core integration
3. **Commercial UE**: Real device testing
4. **Roaming**: Inter-PLMN sessions

### Performance Tests
1. **Session Rate**: Sessions per second
2. **IP Pool**: Allocation efficiency
3. **Message Size**: Encoding optimization
4. **Latency**: End-to-end delay

This roadmap provides a complete blueprint for implementing Phase 5 PDU session establishment in the NFLambda AMF/SMF while maintaining compatibility with 3GPP specifications and open5gs architecture.