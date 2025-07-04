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
   - Function: `amf_build_dl_nas_transport()` → Sets EPD in NAS header
2. **5GSM EPD (46)**: Always 0x2E for 5GSM messages
   - Function: `smf_build_pdu_session_accept()` → Sets 5GSM EPD in PDU session message
3. **Message Type (0x68)**: DL NAS Transport constant
   - Function: `amf_build_dl_nas_transport()` → Sets message type for downlink transport
4. **5GSM Message Type (0xC2)**: PDU Session Establishment Accept
   - Function: `smf_build_pdu_session_accept()` → Sets PDU session accept message type
5. **Payload Container Type (1)**: N1 SM information constant
   - Function: `amf_build_dl_nas_transport()` → Sets container type for SM information

### Protocol Encoding Fields
1. **per.octet_string_length (86)**: Total length of NGAP NAS-PDU
   - Calculated as: Security header (6) + Message content (80)
   - Security header: Type (1) + MAC (4) + Seq (1) = 6 bytes
   - Message content: All fields after security header
   - Function: `amf_send_dl_nas_transport()` → Calculates total length for NGAP encoding

2. **ngap.NAS_PDU**: Complete hex encoding of NAS message
   - Starts with 0x7e (EPD) followed by security header
   - Contains all encoded IEs in binary format
   - Used by NGAP layer for transport to RAN
   - Function: `amf_send_dl_nas_transport()` → Encodes complete NAS PDU for NGAP transport

### Information Element Identifiers
1. **0x01**: Payload container type (MM context)
   - Function: `amf_build_dl_nas_transport()` → Sets IE identifier for container type
2. **0x7b**: Payload container (MM context)
   - Function: `amf_build_dl_nas_transport()` → Sets IE identifier for payload container
3. **0x12**: PDU session identity 2 (MM context)
   - Function: `amf_build_dl_nas_transport()` → Sets IE identifier for PDU session ID
4. **0x59**: PDU session type (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for PDU session type
5. **0x7a**: QoS rules (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for QoS rules
6. **0x2a**: Session-AMBR (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for session AMBR
7. **0x29**: PDU address (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for PDU address
8. **0x22**: S-NSSAI (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for S-NSSAI
9. **0x79**: QoS flow descriptions (SM context)
   - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for QoS flows
10. **0x7b**: Extended protocol configuration options (SM context)
    - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for extended PCO
11. **0x25**: DNN (SM context)
    - Function: `smf_build_pdu_session_accept()` → Sets IE identifier for DNN

### Length Calculations for Complex IEs
1. **Payload Container (71 bytes)**:
   - 5GSM header: 3 bytes (EPD + PSI + PTI)
   - Message type: 1 byte
   - Selected SSC mode: 1 byte
   - All IEs with TLV encoding: 66 bytes total
   - Function: `amf_build_dl_nas_transport()` → Calculates total payload container length

2. **QoS Rules (9 bytes)**:
   - Rule ID: 1 byte
   - Length: 2 bytes (0x0006)
   - Rule content: 6 bytes (operation, filters, precedence, QFI)
   - Function: `smf_create_default_qos_flows()` → Calculates QoS rule encoding length

3. **Session-AMBR (6 bytes)**:
   - DL unit: 1 byte
   - DL value: 2 bytes
   - UL unit: 1 byte
   - UL value: 2 bytes
   - Function: `smf_build_pdu_session_accept()` → Encodes session AMBR with length

4. **PDU Address (5 bytes)**:
   - Type + SI6LLA: 1 byte
   - IPv4 address: 4 bytes
   - Function: `smf_build_pdu_session_accept()` → Encodes PDU address with length

5. **QoS Flow Descriptions (5 bytes)**:
   - QFI: 1 byte
   - Operation code + E bit: 1 byte
   - Number of parameters: 1 byte
   - Parameter (5QI): 2 bytes (ID + length + value)
   - Function: `smf_create_default_qos_flows()` → Calculates QoS flow encoding length

6. **Extended PCO (15 bytes)**:
   - Extension + config protocol: 1 byte
   - Container structure: 14 bytes
   - Function: `smf_build_pdu_session_accept()` → Builds extended PCO response

7. **DNN (9 bytes)**:
   - Label encoding of "internet": 8 bytes
   - Length byte: 1 byte
   - Function: `smf_build_pdu_session_accept()` → Encodes DNN label with length

### Derived Fields from PDU Session Request
1. **PDU Session ID**: Copied from request (1)
   - Function: `amf_extract_pdu_session_request()` → Extracts from UL NAS transport
2. **Procedure Transaction ID**: Copied from request (1)
   - Function: `amf_extract_pdu_session_request()` → Extracts PTI from 5GSM header
3. **Selected SSC Mode**: Validated and echoed (1)
   - Function: `smf_nflambda_smf_handle_create_sm_context()` → Validates requested SSC mode
4. **Selected PDU Session Type**: Validated IPv4 request → IPv4
   - Function: `smf_nflambda_smf_handle_create_sm_context()` → Validates PDU type support
5. **S-NSSAI**: Validated against allowed slices → SST=1
   - Function: `amf_validate_snssai_and_dnn()` → Validates against subscription
6. **DNN**: Used for SMF selection, echoed in response
   - Function: `amf_validate_snssai_and_dnn()` → Validates DNN for slice

### SMF-Generated Fields
1. **PDU Address (10.45.0.2)**: Allocated from SMF IP pool
   - Pool selection based on DNN and S-NSSAI
   - Sequential or random allocation strategy
   - Address tracking in session context
   - Function: `smf_allocate_ip_address()` → Allocates IP from configured pool

2. **DNS Server (8.8.4.4)**: From SMF configuration
   - Primary DNS for the selected DNN
   - Can be overridden by policy
   - Function: `smf_build_pdu_session_accept()` → Retrieves DNS from SMF config

### QoS Rule Creation
1. **Rule ID**: 1 (first rule, auto-assigned)
   - Function: `smf_create_default_qos_flows()` → Assigns rule ID for default rule
2. **Length**: 6 bytes of rule content
   - Function: `smf_create_default_qos_flows()` → Calculates rule content length
3. **Rule Operation (ROP)**: 1 (create new QoS rule)
   - Function: `smf_create_default_qos_flows()` → Sets operation code for rule creation
4. **DQR bit**: 1 (marks as default QoS rule)
   - Function: `smf_create_default_qos_flows()` → Sets default QoS rule indicator
5. **Number of packet filters**: 1
   - Function: `smf_create_default_qos_flows()` → Creates match-all packet filter
6. **Precedence**: 255 (lowest priority for default rule)
   - Function: `smf_create_default_qos_flows()` → Sets lowest precedence for default
7. **QFI**: 1 (links to default QoS flow)
   - Function: `smf_create_default_qos_flows()` → Links rule to default QoS flow

### QoS Rule Packet Filter Encoding
1. **Packet Filter Direction**: 3 (bidirectional)
   - Encoded in 2 bits: 11b = bidirectional
   - Function: `smf_create_default_qos_flows()` → Sets bidirectional filter for default
2. **Packet Filter ID**: 1 (first filter)
   - Encoded in 6 bits: 000001b
   - Function: `smf_create_default_qos_flows()` → Assigns filter ID within rule
3. **Packet Filter Length**: 1 byte
   - Only contains component type for match-all
   - Function: `smf_create_default_qos_flows()` → Calculates filter content length
4. **Component Type**: 1 (match all packets)
   - Special value indicating no specific matching
   - Function: `smf_create_default_qos_flows()` → Sets match-all filter type

### QoS Flow Parameter Encoding
1. **QFI**: 1 (default flow identifier, 6 bits)
   - Function: `smf_create_default_qos_flows()` → Assigns QFI for default flow
2. **Operation Code**: 1 (create new QoS flow description)
   - Function: `smf_create_default_qos_flows()` → Sets create operation for flow
3. **E bit**: 1 (parameters list is present)
   - Function: `smf_create_default_qos_flows()` → Sets E bit for parameter presence
4. **Number of Parameters**: 1 (only 5QI)
   - Function: `smf_create_default_qos_flows()` → Counts QoS flow parameters
5. **Parameter ID**: 1 (5QI parameter)
   - Function: `smf_create_default_qos_flows()` → Sets parameter type as 5QI
6. **Parameter Length**: 1 byte
   - Function: `smf_create_default_qos_flows()` → Calculates 5QI parameter length
7. **5QI Value**: 9 (default internet QoS)
   - Function: `smf_create_default_qos_flows()` → Sets 5QI=9 for best effort

### Session AMBR Calculation
1. **Input**: Subscription data AMBR
   - Subscribed UL: 100 Mbps
   - Subscribed DL: 100 Mbps
   - Function: `smf_handle_create_sm_context()` → Retrieves from subscription

2. **Processing**:
   - Apply slice-specific limits
   - Apply DNN-specific limits
   - Apply session policy
   - Function: `smf_handle_create_sm_context()` → Applies policy limits

3. **Output**: Session AMBR
   - Session UL: 62.5 Mbps (policy limited)
   - Session DL: 62.5 Mbps (policy limited)
   - Encoding: value in kbps (62500)
   - Unit: 3 (indicates Mbps)
   - Function: `smf_build_pdu_session_accept()` → Encodes AMBR values with units

### Extended PCO Structure
1. **Extension bit**: 1 (always set for extended PCO)
   - Function: `smf_build_pdu_session_accept()` → Sets extension bit for extended PCO
2. **Configuration Protocol**: 0 (PPP for compatibility)
   - Function: `smf_build_pdu_session_accept()` → Sets protocol type for compatibility
3. **Container Structure**:
   - Protocol ID: 0x000D (DNS Server IPv4 Address Request)
   - Length: 0x04 (4 bytes for IPv4 address)
   - Content: IPv4 address (8.8.4.4)
   - Function: `smf_build_pdu_session_accept()` → Builds DNS container response

### DNN Label Encoding
1. **Input**: "internet" (8 characters)
   - Function: `amf_extract_pdu_session_request()` → Extracts from request
2. **Encoding**: DNS label format
   - Length byte: 8
   - ASCII bytes: "internet"
   - Total: 9 bytes (1 + 8)
   - Function: `smf_build_pdu_session_accept()` → Encodes DNN in DNS label format
3. **No compression or null terminator**
   - Function: `smf_build_pdu_session_accept()` → Ensures proper label encoding

### PDU Address Encoding
1. **PDU Session Type**: 1 (IPv4)
   - Encoded in 3 bits: 001b
   - Function: `smf_build_pdu_session_accept()` → Encodes PDU type in address field
2. **SI6LLA bit**: 0 (no IPv6 link-local address)
   - Only relevant for IPv6 PDU types
   - Function: `smf_build_pdu_session_accept()` → Clears SI6LLA for IPv4
3. **IPv4 Address**: 10.45.0.2
   - 4 bytes in network byte order
   - From SMF IP pool allocation
   - Function: `smf_allocate_ip_address()` → Returns allocated IP address

### Security-Related Fields
1. **Security Header Type**: 2 (integrity protected and ciphered)
   - Function: `amf_build_dl_nas_transport()` → Sets security header type based on context
2. **Message Authentication Code**: 0xfbd62d81
   - Calculated using NAS integrity algorithm
   - Based on COUNT, direction, and message content
   - Function: `amf_build_dl_nas_transport()` → Calculates MAC using NAS security context
3. **Sequence Number**: 3
   - Incremented from request (2 → 3)
   - Part of NAS COUNT for replay protection
   - Function: `amf_build_dl_nas_transport()` → Increments and sets sequence number

## Implementation Function Chain

### Phase 5: NFLambda AMF/SMF Function Implementation

```
amf_handle_ul_nas_transport()
    ↓
amf_extract_pdu_session_request()
    ↓
amf_validate_snssai_and_dnn()
    ↓
amf_discover_and_select_smf()
    ↓
amf_create_sm_context_request()
    ↓
[SMF Processing]
smf_handle_create_sm_context()
    ↓
smf_allocate_ip_address()
    ↓
smf_create_default_qos_flows()
    ↓
smf_build_pdu_session_accept()
    ↓
[AMF Processing]
amf_build_dl_nas_transport()
    ↓
amf_send_dl_nas_transport()
```

### Detailed Function Specifications

#### 1. `amf_handle_ul_nas_transport()`
**Input**: UL NAS Transport binary structure
**Output**: Trigger SMF interaction
**Logic**:
- Apply decryption and verify integrity using established NAS security
- Extract payload container type field - verify equals 0x01 (N1 SM information)
- Extract PDU session ID from request (value: 5)
- Extract the embedded PDU session establishment request buffer
- Create or find session context for PDU session ID

#### 2. `amf_extract_pdu_session_request()`
**Input**: Payload container from UL NAS transport
**Output**: PDU session request structure
**Logic**:
- Extract 5GSM header fields: EPD (0x2E), PSI (5), PTI (0)
- Extract PDU session type field (value: 1 for IPv4)
- Extract SSC mode field if present (default: mode 1)
- Extract integrity protection max data rate fields
- Extract 5GSM capability bitmap if present
- Extract extended protocol configuration options if IEI 0x7B present

#### 3. `amf_validate_snssai_and_dnn()`
**Input**: S-NSSAI, DNN from request, UE subscription
**Output**: Validation result with selected slice
**Logic**:
- Check S-NSSAI against UE's allowed slices
- Validate DNN for the requested slice
- Apply local policy (roaming restrictions)
- Check concurrent session limits
- Return selected S-NSSAI or rejection cause

#### 4. `amf_discover_and_select_smf()`
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

#### 5. `amf_create_sm_context_request()`
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

#### 6. `smf_handle_create_sm_context()`
**Input**: SM context create request from AMF
**Output**: SM context and PDU session accept
**Logic**:
- Validate request parameters
- Create UE/session context
- Authorize session based on subscription
- Trigger UP session establishment
- Select UPF based on topology
- Build response with session accept

#### 7. `smf_allocate_ip_address()`
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

#### 8. `smf_create_default_qos_flows()`
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

#### 9. `smf_build_pdu_session_accept()`
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

#### 10. `amf_build_dl_nas_transport()`
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

#### 11. `amf_send_dl_nas_transport()`
**Input**: DL NAS Transport message
**Output**: Message sent to RAN
**Logic**:
- Get RAN UE NGAP context
- Build NGAP DL NAS transport
- Include N2 SM information if needed
- Send via SCTP to gNB
- Update session state

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

## Binary Message Structures

### PDU Session Establishment Request Message Structure

```c
#pragma pack(1)  // Ensure no padding between fields

// PDU Session Establishment Request structure matching the binary layout
// Hex: 7e:02:ba:02:92:cd:02:7e:00:67:01:00:15:2e:01:01:c1:ff:ff:91:a1:28:01:00:7b:00:07:80:00:0a:00:00:0d:00:12:01:81:22:01:01:25:09:08:69:6e:74:65:72:6e:65:74
typedef struct {
    // Security header (6 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header_type;       // 0x02 - Integrity protected and ciphered
    uint8_t message_auth_code[4];       // 0xBA0292CD - Message authentication code
    uint8_t sequence_number;            // 0x02 - Sequence number
    
    // Plain NAS 5GS Message header (after decryption)
    uint8_t plain_epd;                  // 0x7E - Extended Protocol Discriminator
    uint8_t plain_security_header;      // 0x00 - No security
    uint8_t message_type;               // 0x67 - UL NAS Transport
    
    // Payload container type IE
    uint8_t pld_cont_type_iei;          // 0x01 - Payload container type IEI
    union {
        uint8_t pld_cont_type;          // 0x00 - bits 7-4: spare, bits 3-0: type
        struct {
            uint8_t type:4;             // bits 3-0: Container type (1 = N1 SM)
            uint8_t spare:4;            // bits 7-4: Spare
        } bits;
    } payload_container_type;
    
    // Payload container IE
    uint8_t pld_cont_iei;               // 0x15 - Payload container IEI (NAS 5 0x01 0x05)
    uint8_t pld_cont_length;            // 0x15 - Length (21 bytes)
    
    // Embedded PDU Session Establishment Request
    struct {
        uint8_t epd;                    // 0x2E - 5GSM EPD
        uint8_t pdu_session_id;         // 0x01 - PDU session identity
        uint8_t pti;                    // 0x01 - Procedure transaction identity
        uint8_t message_type;           // 0xC1 - PDU Session Establishment Request
        
        // Integrity protection maximum data rate
        uint8_t int_prot_max_data_rate[2];  // 0xFF:FF - UL and DL rates
        
        // PDU session type (Type 1 half octet IE)
        union {
            uint8_t pdu_session_type_spare; // 0x91
            struct {
                uint8_t pdu_session_type:3; // bits 2-0: Type (1 = IPv4)
                uint8_t spare:1;            // bit 3: Spare
                uint8_t spare_half:4;       // bits 7-4: Spare half octet
            } bits;
        } pdu_session_type;
        
        // SSC mode (Type 1 half octet IE)
        union {
            uint8_t ssc_mode_spare;         // 0xA1
            struct {
                uint8_t ssc_mode:3;         // bits 2-0: SSC mode (1)
                uint8_t spare:1;            // bit 3: Spare
                uint8_t spare_half:4;       // bits 7-4: Spare half octet
            } bits;
        } ssc_mode;
        
        // 5GSM capability IE
        uint8_t _5gsm_cap_iei;          // 0x28 - 5GSM capability IEI
        uint8_t _5gsm_cap_length;       // 0x01 - Length
        union {
            uint8_t capability;         // 0x00
            struct {
                uint8_t rqos:1;         // bit 0: Reflective QoS (0)
                uint8_t mh6_pdu:1;      // bit 1: Multi-homed IPv6 PDU (0)
                uint8_t ept_s1:1;       // bit 2: Ethernet PDU session (0)
                uint8_t atsss_st:2;     // bits 4-3: ATSSS support (0)
                uint8_t tpmic:1;        // bit 5: TPMI capability (0)
                uint8_t spare:2;        // bits 7-6: Spare
            } bits;
        } _5gsm_capability;
        
        // Extended protocol configuration options IE
        uint8_t ext_pco_iei;            // 0x7B - Extended PCO IEI
        uint8_t ext_pco_length;         // 0x00 - Length (0 initially)
        uint8_t ext_pco_content[7];     // 0x07:80:00:0A:00:00:0D:00 - Extension + containers
    } pdu_session_request;
    
    // PDU session identity 2 IE
    uint8_t pdu_session_id2_iei;       // 0x12 - PDU session ID IEI
    uint8_t pdu_session_id2;           // 0x01 - PDU session identity
    
    // Request type IE
    uint8_t request_type_iei;          // 0x81 - Request type IEI (NAS 5 0x08 0x01)
    union {
        uint8_t request_type;           // 0x01
        struct {
            uint8_t type:3;             // bits 2-0: Request type (1 = initial)
            uint8_t spare:5;            // bits 7-3: Spare
        } bits;
    } req_type;
    
    // S-NSSAI IE
    uint8_t snssai_iei;                // 0x22 - S-NSSAI IEI
    uint8_t snssai_length;             // 0x01 - Length
    uint8_t sst;                       // 0x01 - Slice/Service Type
    
    // DNN IE
    uint8_t dnn_iei;                   // 0x25 - DNN IEI
    uint8_t dnn_length;                // 0x09 - Length
    uint8_t dnn[9];                    // 0x08:69:6E:74:65:72:6E:65:74 - "internet"
} pdu_session_establishment_request_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(pdu_session_establishment_request_t) == 50, "PDU Session Establishment Request structure size mismatch");
```

### PDU Session Establishment Accept Message Structure

```c
// PDU Session Establishment Accept structure matching the binary layout
// Hex: 7e:02:fb:d6:2d:81:03:7e:00:68:01:00:47:2e:01:01:c2:11:00:09:01:00:06:31:31:01:01:ff:01:06:03:f4:24:03:f4:24:29:05:01:0a:2d:00:02:22:01:01:79:00:06:01:20:41:01:01:09:7b:00:0f:80:00:0d:04:08:08:08:08:00:0d:04:08:08:04:04:25:09:08:69:6e:74:65:72:6e:65:74:12:01
typedef struct {
    // Security header (7 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header_type;       // 0x02 - Integrity protected and ciphered
    uint8_t message_auth_code[4];       // 0xFBD62D81 - Message authentication code
    uint8_t sequence_number;            // 0x03 - Sequence number
    
    // Plain NAS 5GS Message header (after decryption)
    uint8_t plain_epd;                  // 0x7E - Extended Protocol Discriminator
    uint8_t plain_security_header;      // 0x00 - No security
    uint8_t message_type;               // 0x68 - DL NAS Transport
    
    // Payload container type IE
    uint8_t pld_cont_type_iei;          // 0x01 - Payload container type IEI
    union {
        uint8_t pld_cont_type;          // 0x00 - bits 7-4: spare, bits 3-0: type
        struct {
            uint8_t type:4;             // bits 3-0: Container type (1 = N1 SM)
            uint8_t spare:4;            // bits 7-4: Spare
        } bits;
    } payload_container_type;
    
    // Payload container IE
    uint8_t pld_cont_iei;               // 0x47 - Payload container IEI (NAS 5 0x04 0x07)
    uint8_t pld_cont_length;            // 0x47 - Length (71 bytes)
    
    // Embedded PDU Session Establishment Accept
    struct {
        uint8_t epd;                    // 0x2E - 5GSM EPD
        uint8_t pdu_session_id;         // 0x01 - PDU session identity
        uint8_t pti;                    // 0x01 - Procedure transaction identity
        uint8_t message_type;           // 0xC2 - PDU Session Establishment Accept
        
        // Selected PDU session type and SSC mode (Type 1 half octet)
        union {
            uint8_t type_and_ssc;       // 0x11
            struct {
                uint8_t ssc_mode:3;     // bits 2-0: Selected SSC mode (1)
                uint8_t spare:1;        // bit 3: Spare
                uint8_t pdu_type:3;     // bits 6-4: PDU session type (1 = IPv4)
                uint8_t spare2:1;       // bit 7: Spare
            } bits;
        } selected_type_ssc;
        
        // QoS rules IE
        uint8_t qos_rules_iei;          // 0x00 - QoS rules IEI (actually part of length)
        uint8_t qos_rules_length;       // 0x09 - Length
        struct {
            uint8_t qos_rule_id;        // 0x01 - QoS rule identifier
            uint16_t length;            // 0x0006 - Rule length (big-endian)
            union {
                uint8_t rule_oper_code; // 0x31
                struct {
                    uint8_t rop:3;      // bits 2-0: Rule operation (1 = create)
                    uint8_t dqr:1;      // bit 3: Default QoS rule (1)
                    uint8_t num_filters:4; // bits 7-4: Number of packet filters (1)
                } bits;
            } operation;
            // Packet filter 1
            union {
                uint8_t filter_header;  // 0x31
                struct {
                    uint8_t filter_id:6;  // bits 5-0: Filter identifier (1)
                    uint8_t direction:2;  // bits 7-6: Direction (3 = bidirectional)
                } bits;
            } pkt_filter;
            uint8_t filter_length;      // 0x01 - Filter content length
            uint8_t component_type;     // 0x01 - Match all packets
            uint8_t precedence;         // 0xFF - Rule precedence (255)
            uint8_t qfi;               // 0x01 - QoS flow identifier
        } qos_rule;
        
        // Session-AMBR IE
        uint8_t session_ambr_iei;       // 0x06 - Session-AMBR IEI (actually 0x2A)
        uint8_t session_ambr_length;    // 0x06 - Length
        uint8_t dl_unit;               // 0x03 - DL unit (3 = Mbps)
        uint16_t dl_ambr;              // 0xF424 - DL AMBR (62500 in big-endian)
        uint8_t ul_unit;               // 0x03 - UL unit (3 = Mbps)
        uint16_t ul_ambr;              // 0xF424 - UL AMBR (62500 in big-endian)
        
        // PDU address IE
        uint8_t pdu_address_iei;        // 0x29 - PDU address IEI
        uint8_t pdu_address_length;     // 0x05 - Length
        union {
            uint8_t type_field;         // 0x01
            struct {
                uint8_t pdu_type:3;     // bits 2-0: PDU session type (1 = IPv4)
                uint8_t spare:4;        // bits 6-3: Spare
                uint8_t si6lla:1;       // bit 7: IPv6 link-local (0)
            } bits;
        } pdu_addr_type;
        uint32_t ipv4_address;          // 0x0A2D0002 - 10.45.0.2 (big-endian)
        
        // S-NSSAI IE
        uint8_t snssai_iei;            // 0x22 - S-NSSAI IEI
        uint8_t snssai_length;         // 0x01 - Length
        uint8_t sst;                   // 0x01 - Slice/Service Type
        
        // QoS flow descriptions IE
        uint8_t qos_flow_desc_iei;     // 0x79 - QoS flow descriptions IEI
        uint8_t qos_flow_desc_length;  // 0x00 - Length (actually 0x05)
        uint8_t qos_flow_desc_len2;    // 0x06 - Actual length byte
        struct {
            union {
                uint8_t qfi_oper;       // 0x01
                struct {
                    uint8_t qfi:6;      // bits 5-0: QoS flow identifier (1)
                    uint8_t spare:2;    // bits 7-6: Spare
                } bits;
            } qfi;
            union {
                uint8_t oper_params;    // 0x20
                struct {
                    uint8_t op_code:3;  // bits 2-0: Operation code (1 = create)
                    uint8_t spare:4;    // bits 6-3: Spare
                    uint8_t e:1;        // bit 7: Parameters present (1)
                } bits;
            } operation;
            uint8_t num_params;         // 0x41 - E bit + number of params
            uint8_t param_id;           // 0x01 - Parameter ID (5QI)
            uint8_t param_length;       // 0x01 - Parameter length
            uint8_t _5qi;              // 0x09 - 5QI value (9)
        } qos_flow;
        
        // Extended protocol configuration options IE
        uint8_t ext_pco_iei;           // 0x7B - Extended PCO IEI
        uint8_t ext_pco_length;        // 0x00 - Length (actually 0x0F)
        uint8_t ext_pco_len2;          // 0x0F - Actual length byte
        uint8_t ext_config_prot;       // 0x80 - Extension (1) + config protocol (0)
        // DNS Server IPv4 Address container
        uint16_t dns_container_id;     // 0x000D - Protocol ID (big-endian)
        uint8_t dns_length;            // 0x04 - Container length
        uint32_t dns_ipv4;             // 0x08080808 - 8.8.8.8 (big-endian)
        // Additional DNS Server
        uint16_t dns2_container_id;    // 0x000D - Protocol ID (big-endian)
        uint8_t dns2_length;           // 0x04 - Container length
        uint32_t dns2_ipv4;            // 0x08080404 - 8.8.4.4 (big-endian)
        
        // DNN IE
        uint8_t dnn_iei;               // 0x25 - DNN IEI
        uint8_t dnn_length;            // 0x09 - Length
        uint8_t dnn[9];                // 0x08:69:6E:74:65:72:6E:65:74 - "internet"
    } pdu_session_accept;
    
    // PDU session identity 2 IE
    uint8_t pdu_session_id2_iei;      // 0x12 - PDU session ID IEI
    uint8_t pdu_session_id2;          // 0x01 - PDU session identity
} pdu_session_establishment_accept_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(pdu_session_establishment_accept_t) == 93, "PDU Session Establishment Accept structure size mismatch");
```


