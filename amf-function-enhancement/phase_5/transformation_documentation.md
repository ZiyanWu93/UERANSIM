# Phase 5: PDU Session Establishment Request → PDU Session Establishment Accept Transformation

## Overview
This document details how open5gs AMF and SMF transform a PDU Session Establishment Request into a PDU Session Establishment Accept message through a complex multi-network function flow.

## Input Message: PDU Session Establishment Request
- **Message Type**: 0x67 (UL NAS Transport containing PDU Session Establishment Request)
- **Security**: Integrity protected and ciphered
- **Key Fields**:
  - PDU session ID: 1
  - Session type: IPv4 (type 1)
  - SSC mode: 1
  - DNN: "internet"
  - S-NSSAI: SST=1
  - Extended protocol configuration options

## Output Message: PDU Session Establishment Accept
- **Message Type**: 0x68 (DL NAS Transport containing PDU Session Establishment Accept)
- **Security**: Integrity protected and ciphered
- **Key Fields**:
  - PDU session ID: 1
  - Selected session type: IPv4
  - QoS rules: Default rule with QFI=1
  - Session AMBR: Uplink/Downlink rates
  - PDU address: Allocated IPv4 address
  - S-NSSAI: SST=1
  - QoS flow descriptions: Default QoS flow

## Network Functions Involved

### 1. AMF (Access and Mobility Management Function)
- Receives PDU session establishment request
- Validates S-NSSAI and DNN
- Selects appropriate SMF
- Creates SM context with SMF
- Forwards session accept in DL NAS transport

### 2. SMF (Session Management Function)
- Creates session context
- Allocates IP address
- Creates QoS flows and rules
- Builds PDU session establishment accept
- Coordinates with UPF for user plane setup

### 3. UPF (User Plane Function)
- Sets up user plane tunnels
- Applies QoS policies
- Handles packet forwarding

### 4. NRF (Network Repository Function)
- Provides SMF discovery
- Maintains network function profiles

## Detailed Function Call Flow

### Step 1: PDU Session Request Reception (AMF)
**File**: `src/amf/gmm-handler.c`
**Function**: `gmm_handle_ul_nas_transport()` (lines 1214-1376)

```c
// Extract payload container from UL NAS transport
payload_container_type = &ul_nas_transport->payload_container_type;
payload_container = &ul_nas_transport->payload_container;

// Validate container type
if (payload_container_type->type != OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION) {
    ogs_error("Invalid payload container type");
    return OGS_ERROR;
}

// Extract PDU session ID
pdu_session_id = &ul_nas_transport->pdu_session_id;
if (*pdu_session_id == OGS_NAS_PDU_SESSION_IDENTITY_UNASSIGNED) {
    ogs_error("PDU session identity unassigned");
    return OGS_ERROR;
}

// Parse GSM header from payload
gsm_header = (ogs_nas_5gs_gsm_header_t *)payload_container->buffer;
if (gsm_header->message_type == OGS_NAS_5GS_PDU_SESSION_ESTABLISHMENT_REQUEST) {
    // Create or find session
    sess = amf_sess_find_by_psi(amf_ue, *pdu_session_id);
    if (!sess) {
        sess = amf_sess_add(amf_ue, *pdu_session_id);
    }
}
```

### Step 2: Session Validation and S-NSSAI/DNN Checking
**File**: `src/amf/gmm-handler.c` (lines 1286-1377)

```c
// Extract S-NSSAI and DNN from PDU session request
requested_nssai = &pdu_session_establishment_request->s_nssai;
dnn = &pdu_session_establishment_request->dnn;

// Validate against UE's allowed slices
for (i = 0; i < amf_ue->num_of_slice; i++) {
    if (ogs_nas_s_nssai_is_equal(&amf_ue->slice[i].s_nssai, requested_nssai)) {
        // Check if DNN is allowed for this slice
        for (j = 0; j < amf_ue->slice[i].num_of_dnn; j++) {
            if (ogs_strcasecmp(amf_ue->slice[i].dnn[j], dnn->value) == 0) {
                slice_matched = true;
                break;
            }
        }
    }
}

if (!slice_matched) {
    ogs_error("Requested slice/DNN not allowed");
    // Send rejection with appropriate cause
}
```

### Step 3: SMF Selection and Discovery
**File**: `src/amf/gmm-handler.c` (lines 1390-1444)

```c
// Create discovery options for SMF selection
discovery_option = ogs_sbi_discovery_option_new();
ogs_sbi_discovery_option_add_snssais(discovery_option, &sess->s_nssai);
ogs_sbi_discovery_option_set_dnn(discovery_option, sess->dnn);
ogs_sbi_discovery_option_set_tai(discovery_option, &amf_ue->nr_tai);

// Discover and select SMF
r = amf_sess_sbi_discover_and_send(
    OGS_SBI_SERVICE_TYPE_NSMF_PDUSESSION,
    discovery_option,
    amf_nsmf_pdusession_build_create_sm_context,
    ran_ue, sess, AMF_CREATE_SM_CONTEXT_NO_STATE, NULL);

if (r != OGS_OK) {
    // Fallback to NSSF if no SMF found
    ogs_error("SMF selection failed, trying NSSF");
}
```

### Step 4: SM Context Creation with SMF
**File**: `src/amf/nsmf-build.c`
**Function**: `amf_nsmf_pdusession_build_create_sm_context()`

```c
// Build SmContextCreateData
SmContextCreateData = ogs_calloc(1, sizeof(*SmContextCreateData));

// Set basic UE information
SmContextCreateData->supi = ogs_strdup(amf_ue->supi);
SmContextCreateData->pei = ogs_strdup(amf_ue->pei);
SmContextCreateData->serving_nf_id = ogs_strdup(ogs_sbi_self()->nf_instance_id);

// Set serving network information
serving_network = &SmContextCreateData->serving_network;
ogs_nas_to_plmn_id(&serving_network->plmn_id, &amf_ue->current.tai.plmn_id);

// Set S-NSSAI
SmContextCreateData->s_nssai = ogs_calloc(1, sizeof(ogs_sbi_s_nssai_t));
ogs_nas_to_sbi_s_nssai(SmContextCreateData->s_nssai, &sess->s_nssai);

// Set DNN
SmContextCreateData->dnn = ogs_strdup(sess->dnn);

// Attach N1 SM message (PDU session establishment request)
SmContextCreateData->n1_sm_msg = ogs_calloc(1, sizeof(ogs_sbi_n1_sm_msg_t));
SmContextCreateData->n1_sm_msg->content_id = ogs_strdup("n1SmMsg");
```

### Step 5: SMF Session Processing
**File**: `src/smf/nsmf-handler.c`
**Function**: `smf_nsmf_handle_create_sm_context()` (lines 26-200)

```c
// Parse create SM context request
SmContextCreateData = request->SmContextCreateData;

// Validate mandatory parameters
if (!SmContextCreateData->serving_network ||
    !SmContextCreateData->s_nssai ||
    !SmContextCreateData->ue_location) {
    ogs_error("Missing mandatory parameters");
    return OGS_ERROR;
}

// Create UE and session context
ue = smf_ue_find_by_supi(SmContextCreateData->supi);
if (!ue) {
    ue = smf_ue_add_by_supi(SmContextCreateData->supi);
}

sess = smf_sess_add_by_psi(ue, SmContextCreateData->pdu_session_id);
```

### Step 6: IP Address Allocation and QoS Setup (SMF)
**File**: `src/smf/context.c` & `src/smf/gsm-build.c`

```c
// IP address allocation (context.c)
sess->ue_ip.ipv4 = 1;  // Configure for IPv4
ogs_pool_alloc(&smf_self()->ue_pool, &sess->ue_ip.addr);
sess->paa.session_type = OGS_PDU_SESSION_TYPE_IPV4;
sess->paa.ipv4 = true;
sess->paa.both.addr = sess->ue_ip.addr->addr[0];

// Default QoS flow creation
qos_flow = smf_qos_flow_add(sess);
qos_flow->qfi = 1;  // Default QFI
qos_flow->qos.index = OGS_QOS_INDEX_1;  // Default 5QI
```

### Step 7: PDU Session Establishment Accept Building (SMF)
**File**: `src/smf/gsm-build.c`
**Function**: `gsm_build_pdu_session_establishment_accept()`

```c
// Set message type
message.gsm.h.message_type = OGS_NAS_5GS_PDU_SESSION_ESTABLISHMENT_ACCEPT;

// Set selected PDU session type
pdu_session_establishment_accept->selected_pdu_session_type.type = 
    sess->paa.session_type;

// Build QoS rules
qos_rule[0].identifier = qos_flow->qfi;
qos_rule[0].code = OGS_NAS_QOS_CODE_CREATE_NEW_QOS_RULE;
qos_rule[0].DQR_bit = 1;  // Default QoS rule
qos_rule[0].precedence = 255;  // Lowest precedence
qos_rule[0].length = build_qos_rule_packet_filter_list(qos_rule[0].flow);

// Set session AMBR
session_ambr->uplink = ogs_htobe16(sess->session.ambr.uplink / 1000);  // kbps
session_ambr->downlink = ogs_htobe16(sess->session.ambr.downlink / 1000);

// Set PDU address
if (sess->paa.ipv4) {
    pdu_address->length = OGS_NAS_PDU_ADDRESS_IPV4_LEN;
    pdu_address->type = OGS_PDU_SESSION_TYPE_IPV4;
    pdu_address->addr = sess->paa.both.addr;
}

// Set S-NSSAI
s_nssai->length = ogs_nas_build_s_nssai(s_nssai->buffer, &sess->s_nssai);

// Build QoS flow descriptions
qos_flow_description[0].identifier = qos_flow->qfi;
qos_flow_description[0].code = OGS_NAS_CREATE_NEW_QOS_FLOW_DESCRIPTION;
qos_flow_description[0].E_bit = 1;  // Extension bit
qos_flow_description[0].num_of_parameter = 2;
qos_flow_description[0].param[0].identifier = OGS_NAS_QOS_FLOW_PARAM_5QI;
qos_flow_description[0].param[0].value = qos_flow->qos.index;
```

### Step 8: DL NAS Transport Building (AMF)
**File**: `src/amf/gmm-build.c`
**Function**: `gmm_build_dl_nas_transport()` (lines 627-692)

```c
// Set message type and security headers
message.h.security_header_type = 
    OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED;
message.gmm.h.message_type = OGS_NAS_5GS_DL_NAS_TRANSPORT;

// Set payload container type
dl_nas_transport->payload_container_type.type = 
    OGS_NAS_PAYLOAD_CONTAINER_N1_SM_INFORMATION;

// Set payload container (contains PDU session establishment accept)
dl_nas_transport->payload_container.length = n1smbuf->len;
dl_nas_transport->payload_container.buffer = n1smbuf->data;

// Set PDU session ID
if (sess) {
    dl_nas_transport->presencemask |= 
        OGS_NAS_5GS_DL_NAS_TRANSPORT_PDU_SESSION_ID_PRESENT;
    dl_nas_transport->pdu_session_id.value = sess->psi;
}
```

## Field Transformations Summary

| Field | PDU Session Request | PDU Session Accept | Transformation Logic |
|-------|-------------------|-------------------|---------------------|
| Message Type | 0xC1 (PS Est Req) | 0xC2 (PS Est Accept) | Changed to accept |
| Session Type | IPv4 requested | IPv4 selected | Validated and confirmed |
| DNN | "internet" | Not included | Used for SMF selection |
| S-NSSAI | SST=1 | SST=1 | Validated and echoed |
| PDU Address | Not present | Allocated IPv4 | From SMF IP pool |
| QoS Rules | Not present | Default rule QFI=1 | Created by SMF |
| Session AMBR | Not present | UL/DL rates | From subscription data |
| QoS Flows | Not present | Default flow QFI=1 | Created by SMF |

## Key Data Structures

### 1. Session Context (AMF)
```c
typedef struct amf_sess_s {
    uint8_t psi;                    // PDU session identity
    char *dnn;                      // Data network name
    ogs_s_nssai_t s_nssai;         // Slice information
    ogs_nas_paa_t paa;             // PDU address
    bool n1_released;              // N1 release status
    bool n2_released;              // N2 release status
} amf_sess_t;
```

### 2. Session Context (SMF)
```c
typedef struct smf_sess_s {
    uint8_t psi;                    // PDU session identity
    ogs_nas_paa_t paa;             // Allocated IP address
    ogs_session_t session;          // Session parameters
    ogs_list_t qos_flow_list;      // QoS flows
    smf_ue_ip_t ue_ip;             // UE IP allocation
} smf_sess_t;
```

### 3. QoS Flow Structure
```c
typedef struct smf_qos_flow_s {
    uint8_t qfi;                    // QoS flow identifier
    ogs_qos_t qos;                 // QoS parameters (5QI, etc.)
    bool ul_teid_allocation;        // Uplink TEID status
    bool dl_teid_allocation;        // Downlink TEID status
} smf_qos_flow_t;
```

## Key Algorithms and Calculations

### 1. SMF Selection Algorithm
```
1. Parse S-NSSAI and DNN from request
2. Query NRF with discovery filter:
   - Service type: NSMF_PDUSESSION
   - S-NSSAI filter
   - DNN filter
   - TAI filter
3. Select SMF from response based on load/policy
4. Fallback to NSSF if no SMF found
```

### 2. IP Address Allocation
```
1. Determine session type (IPv4/IPv6/IPv4v6)
2. Select appropriate IP pool based on DNN/slice
3. Allocate address from pool
4. Update UE context with allocated address
```

### 3. QoS Rule Creation
```
Default QoS Rule:
- QRI: 1 (auto-assigned)
- Precedence: 255 (lowest)
- QFI: 1 (default flow)
- Packet filters: Match all packets
```

## Security and Protocol Considerations
1. N1 SM messages are protected by NAS security
2. SMF validates UE authorization for requested resources
3. QoS enforcement prevents resource abuse
4. Session contexts synchronized between AMF and SMF
5. User plane security handled by UPF