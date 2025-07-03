# NFLambda AMF Function Enhancement Roadmap

## Project Overview

This roadmap outlines the plan to enhance the NFLambda AMF implementation by replacing hardcoded NAS message outputs with actual processing chains that mirror the Open5GS implementation while maintaining the event-driven architecture.

**Note**: The function chains outlined below are planned implementations and are subject to changes as the project progresses and requirements are refined.

## Project Phases

### Phase 1: Authentication Request

**Message**: `generate_auth_req`
**Trigger**: Registration Request received
**Result**: Authentication Request sent

**Objectives**:
- Implement SUCI to SUPI conversion
- Implement authentication vector generation (RAND, AUTN)
- Implement Milenage algorithms
- Store authentication context for subsequent messages

**Planned Function Chain**:
```
amf_handle_registration_request()
    ↓
amf_extract_suci()
    ↓
udm_decrypt_suci_to_supi()
    ↓
udm_get_authentication_subscription_data()
    ↓
ausf_initiate_authentication()
    ↓
udm_generate_authentication_vector()
    ↓
ausf_process_authentication_vector()
    ↓
amf_store_authentication_context()
    ↓
amf_build_authentication_request()
```

### Phase 2: Security Mode Command

**Message**: `generate_security_cmd`
**Trigger**: Authentication Response received
**Result**: Security Mode Command sent

**Objectives**:
- Implement RES* verification
- Implement key derivation (KAUSF → KAMF → NAS keys)
- Implement security algorithm selection
- Implement MAC calculation for message integrity

**Planned Function Chain**:
```
amf_handle_authentication_response()
    ↓
amf_extract_res_star()
    ↓
ausf_verify_authentication_response()
    ↓
amf_derive_security_keys()
    ↓
amf_select_security_algorithms()
    ↓
amf_create_security_context()
    ↓
amf_build_security_mode_command()
```

### Phase 3: Registration Accept

**Message**: `generate_registration_accept`
**Trigger**: Security Mode Complete received
**Result**: Registration Accept sent

**Objectives**:
- Implement GUTI allocation
- Implement network slice selection
- Implement TAI list management
- Apply security protection to messages

**Planned Function Chain**:
```
amf_handle_security_mode_complete()
    ↓
amf_verify_security_protection()
    ↓
amf_allocate_guti()
    ↓
udm_get_subscriber_data()
    ↓
nssf_select_network_slice()
    ↓
amf_determine_registration_area()
    ↓
amf_set_mobility_restrictions()
    ↓
amf_build_registration_accept()
```

### Phase 4: Configuration Update Command

**Message**: `generate_configuration_update`
**Trigger**: Registration Complete received
**Result**: Configuration Update Command sent

**Objectives**:
- Implement network parameter encoding
- Implement UTF-16 conversion for network names
- Implement time zone and time encoding
- Apply security protection with sequence management

**Planned Function Chain**:
```
amf_handle_registration_complete()
    ↓
amf_verify_registration_status()
    ↓
pcf_get_ue_policy()
    ↓
amf_prepare_network_parameters()
    ↓
amf_encode_configuration_data()
    ↓
amf_build_configuration_update_command()
```

### Phase 5: PDU Session Establishment Accept

**Message**: `generate_pdu_session_establishment`
**Trigger**: PDU Session Establishment Request received
**Result**: PDU Session Establishment Accept sent

**Objectives**:
- Implement session resource allocation
- Implement QoS flow creation
- Implement IP address allocation
- Implement nested message structure (DL NAS Transport)

**Planned Function Chain**:
```
amf_handle_pdu_session_request()
    ↓
amf_select_smf()
    ↓
smf_create_session_context()
    ↓
smf_select_upf()
    ↓
smf_allocate_session_resources()
    ↓
pcf_get_session_policy()
    ↓
smf_create_qos_flows()
    ↓
smf_build_session_establishment_accept()
    ↓
amf_build_dl_nas_transport()
```