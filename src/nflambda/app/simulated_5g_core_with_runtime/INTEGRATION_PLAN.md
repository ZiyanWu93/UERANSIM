# Service Function Chain Implementation Plan

## Overview
Transform the current monolithic message handlers into service function chains where each network function is responsible for specific message fields. The output messages remain identical, but the construction is distributed across multiple handlers.

## Implementation Strategy
- Split each hardcoded message array into field-specific handlers
- Each handler modifies EVENT_PAYLOAD directly by setting its designated fields
- Maintain the exact same byte sequences as the current implementation

## Message 1: Registration Request → Authentication Request

The service function chain for generating an authentication request has been implemented using a fully event-driven architecture that demonstrates proper separation of concerns between network functions.

When the AMF receives a registration request, the `amf_handle_registration_request` event handler processes it by building the initial portion of the authentication request message, including the EPD header, message type, ngKSI field, and ABBA information element. Rather than constructing the entire message locally, the AMF sends a high-level "Generate Authentication Data" request to the AUSF through the EVENT_TO_AUSF event, marking the beginning of the service function chain.

The AUSF dispatcher receives this request and, instead of directly calling internal functions, triggers an internal event EVENT_AUSF_PROCESS_AUTH_REQ. This allows the runtime to schedule the `ausf_process_auth_request` handler, which adds the RAND (Random challenge) value to the message. The AUSF then forwards a "Get Authentication Vectors" request to the UDM through EVENT_TO_UDM, continuing the chain.

Similarly, the UDM dispatcher triggers EVENT_UDM_GEN_AUTH_VECTORS, allowing the runtime to schedule the `udm_gen_auth_vectors` handler. This handler adds the AUTN (Authentication Token) value, completing the authentication vector generation. The UDM sends an "Authentication Vectors Response" back to the AUSF through EVENT_TO_AUSF.

The AUSF dispatcher receives this response and triggers EVENT_AUSF_COMPLETE_AUTH_DATA, which causes the `ausf_complete_auth_data` handler to forward the complete authentication data back to the AMF through EVENT_TO_AMF. Finally, the AMF dispatcher triggers EVENT_AMF_FINALIZE_AUTH_REQUEST, and the `amf_finalize_auth_request` handler removes the service chain request type byte and sends the complete 42-byte authentication request message to the UE.

This implementation fully utilizes the NFLambda event-driven runtime for scheduling all operations. Each network function maintains its autonomy by processing requests through its dispatcher and internal events, while the runtime handles the scheduling and execution of all event handlers. The approach ensures loose coupling between network functions and allows for better scalability, testability, and adherence to 5G service-based architecture principles.

## Message 2: Authentication Response → Security Mode Command

**Current Implementation**: Event-driven service function chain across AMF, AUSF, and UDM network functions.

The implementation follows the same event-driven architecture as Message 1. When the AMF receives an Authentication Response, the `amf_dispatcher` triggers EVENT_AMF_AUTH_RESPONSE, which schedules the `amf_handle_authentication_response` handler. This handler builds the initial security mode command structure (outer header, sequence number, inner header, and message type) and sends a REQ_TYPE_PREPARE_SEC_MODE request to the AUSF through EVENT_TO_AUSF.

The AUSF dispatcher receives this request and triggers EVENT_AUSF_PROCESS_SEC_MODE, causing the `ausf_process_sec_mode` handler to add the MAC value and NAS algorithms selection. The AUSF then forwards a REQ_TYPE_GET_UE_SEC_CAP request to the UDM through EVENT_TO_UDM. The UDM dispatcher triggers EVENT_UDM_PROVIDE_SEC_CAP, and the `udm_provide_sec_cap` handler adds the ngKSI field and UE security capabilities before sending a RESP_TYPE_UE_SEC_CAP response back to the AUSF.

The AUSF dispatcher receives this response and triggers EVENT_AUSF_COMPLETE_SEC_MODE, which causes the `ausf_complete_sec_mode` handler to add the IMEISV request and additional security information. The complete security mode data is then forwarded to the AMF through EVENT_TO_AMF with a RESP_TYPE_SEC_MODE_DATA response type. Finally, the AMF dispatcher triggers EVENT_AMF_FINALIZE_SEC_MODE, and the `amf_finalize_sec_mode` handler removes the request type byte and sends the complete 21-byte Security Mode Command to the UE.

This implementation demonstrates the same principles as Message 1: proper separation of concerns, event-driven processing, and network function autonomy while maintaining the exact message format required by the 5G standards.

## Message 3: Security Mode Complete → Registration Accept

**Current Implementation**: Event-driven service function chain across AMF and UDM network functions.

The implementation follows a simplified event-driven architecture compared to Messages 1 and 2, demonstrating that not all messages require involvement from all network functions. When the AMF receives a Security Mode Complete, the `amf_dispatcher` triggers EVENT_AMF_SECURITY_MODE_COMPLETE, which schedules the `amf_handle_security_mode_complete` handler. This handler builds the majority of the Registration Accept message, including the outer header, MAC, sequence number, inner header, message type, registration result, GUTI IE, and TAI list.

The AMF then sends a REQ_TYPE_PREPARE_REG_ACCEPT request to the UDM through EVENT_TO_UDM. The UDM dispatcher receives this request and triggers EVENT_UDM_PROVIDE_SUBSCRIPTION, causing the `udm_provide_subscription` handler to add the allowed NSSAI (Network Slice Selection Assistance Information) based on the subscriber's profile. The UDM sends a RESP_TYPE_SUBSCRIPTION_DATA response back to the AMF.

Finally, the AMF dispatcher receives this response and triggers EVENT_AMF_FINALIZE_REG_ACCEPT. The `amf_finalize_reg_accept` handler adds the network features support and T3512 timer value, removes the request type byte, and sends the complete 46-byte Registration Accept message to the UE.

This implementation shows that service function chains can be flexible - while Messages 1 and 2 involved three network functions (AMF, AUSF, UDM), Message 3 only requires AMF and UDM collaboration, as the subscription data is the only external information needed from another network function.

## Message 4: Registration Complete → Configuration Update

**Current Implementation**: Event-driven service function chain across AMF and PCF network functions.

The implementation follows a simplified event-driven architecture similar to Message 3, demonstrating that different messages require different levels of network function collaboration. When the AMF receives a Registration Complete, the `amf_dispatcher` triggers EVENT_AMF_REGISTRATION_COMPLETE, which schedules the `amf_handle_registration_complete` handler. This handler builds the initial configuration update structure, including the outer header, MAC, sequence number, inner header, and message type.

The AMF then sends a REQ_TYPE_GET_CONFIG_DATA request to the PCF through EVENT_TO_PCF. The PCF dispatcher receives this request and triggers EVENT_PCF_PROVIDE_CONFIG, causing the `pcf_provide_config` handler to add the network configuration data. Specifically, the PCF provides the full network name ("Open5GS" in UCS2 encoding) and short network name ("Next" in UCS2 encoding) based on the operator's network configuration. The PCF sends a RESP_TYPE_CONFIG_DATA response back to the AMF.

Finally, the AMF dispatcher receives this response and triggers EVENT_AMF_FINALIZE_CONFIG_UPDATE. The `amf_finalize_config_update` handler adds the timezone information, universal time and timezone, and daylight saving time settings. It then removes the request type byte and sends the complete 51-byte Configuration Update Command to the UE.

This implementation shows that service function chains can involve different combinations of network functions based on the specific requirements. While registration and authentication involve AMF-AUSF-UDM collaboration, configuration updates require AMF-PCF collaboration, as the PCF is responsible for policy and configuration data in 5G networks.

## Message 5: PDU Session Request → PDU Session Accept

**Current Implementation**: Event-driven service function chain across AMF, SMF, UPF, PCF, and UDM network functions.

This implementation represents the most complex service function chain in the system, involving five network functions collaborating to establish a PDU session. When the AMF receives a PDU Session Request, the `amf_dispatcher` triggers EVENT_AMF_PDU_SESSION_REQUEST, which schedules the `amf_handle_pdu_session_request` handler. This handler builds the initial PDU session structure with outer headers, MAC, and sequence number, then forwards the request to SMF.

The service chain follows this flow: AMF → SMF → UPF → SMF → PCF → SMF → UDM → SMF → AMF. Each network function contributes specific components:

1. **AMF to SMF**: The AMF sends a REQ_TYPE_CREATE_PDU_SESSION request. The SMF dispatcher triggers EVENT_SMF_PROCESS_PDU_REQ, and the handler adds DL NAS transport headers, SM headers, PDU session type, QoS rules, and Session-AMBR.

2. **SMF to UPF**: The SMF sends a REQ_TYPE_ALLOCATE_IP request. The UPF dispatcher triggers EVENT_UPF_ALLOCATE_IP, and the handler allocates an IP address (10.45.0.2) for the PDU session.

3. **SMF to PCF**: The SMF sends a REQ_TYPE_GET_PCC_RULES request. The PCF dispatcher triggers EVENT_PCF_PROVIDE_PCC, and the handler adds extended protocol configuration options including DNS server addresses (8.8.8.8 and 8.8.4.4).

4. **SMF to UDM**: The SMF sends a REQ_TYPE_GET_DNN_INFO request. The UDM dispatcher triggers EVENT_UDM_PROVIDE_DNN, and the handler adds S-NSSAI and DNN ("internet") information based on the subscriber profile.

5. **SMF to AMF**: Finally, the SMF completes the PDU session by adding QoS flow descriptions and 5GSM cause, then sends a RESP_TYPE_PDU_SESSION_CREATED response back to AMF.

The AMF dispatcher receives this response and triggers EVENT_AMF_FINALIZE_PDU_ACCEPT. The handler removes the request type byte and sends the complete 86-byte PDU Session Accept message to the UE. This implementation demonstrates how complex 5G procedures can be decomposed into manageable, distributed operations across multiple network functions, each handling its specific domain of responsibility.

## Implementation Details

Each handler will:
1. Access EVENT_PAYLOAD directly
2. Write its specific fields at the correct offset
3. Not modify fields outside its responsibility
4. Maintain the exact byte values from the original arrays

Example implementation pattern:
```c
// In amf.c
void amf_set_auth_ngksi(void) {
    EVENT_PAYLOAD[3] = 0x00;  // ngKSI field
}

// In ausf.c
void ausf_set_rand(void) {
    EVENT_PAYLOAD[7] = 0x21;  // RAND IEI
    // Copy RAND value
    uint8_t rand[] = {0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 
                      0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69};
    memcpy(&EVENT_PAYLOAD[8], rand, 16);
}

// In udm.c
void udm_set_autn(void) {
    EVENT_PAYLOAD[24] = 0x20;  // AUTN IEI
    EVENT_PAYLOAD[25] = 0x10;  // AUTN length
    // Copy AUTN value
    uint8_t autn[] = {0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00,
                      0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19};
    memcpy(&EVENT_PAYLOAD[26], autn, 16);
}
```

## Benefits
- Demonstrates service function chaining architecture
- Shows how different NFs contribute to message construction
- Maintains exact compatibility with current implementation
- Provides clear separation of concerns between network functions
- Easy to extend or modify individual field handlers