#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "smf.h"
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// SMF (Session Management Function) Dispatcher  
EVENT_HANDLER(smf_dispatcher)
{
    printf("[SMF Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[SMF Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[SMF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Check for service chain request/response type
    uint8_t request_type = EVENT_PAYLOAD[0];
    
    if (request_type == REQ_TYPE_CREATE_PDU_SESSION) {
        // PDU session creation request from AMF
        printf("[SMF Dispatcher] Received PDU session creation request from AMF\n");
        trigger_event(EVENT_SMF_PROCESS_PDU_REQ, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    if (request_type == RESP_TYPE_IP_ALLOCATED) {
        // IP allocation response from UPF
        printf("[SMF Dispatcher] Received IP allocation response from UPF\n");
        trigger_event(EVENT_SMF_REQUEST_POLICY, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    if (request_type == RESP_TYPE_PCC_RULES) {
        // PCC rules response from PCF
        printf("[SMF Dispatcher] Received PCC rules response from PCF\n");
        trigger_event(EVENT_SMF_REQUEST_DNN, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    if (request_type == RESP_TYPE_DNN_INFO) {
        // DNN information response from UDM
        printf("[SMF Dispatcher] Received DNN information response from UDM\n");
        trigger_event(EVENT_SMF_COMPLETE_PDU, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    printf("[SMF Dispatcher] Unknown request type: 0x%02x\n", request_type);
}

// Process PDU session request
EVENT_HANDLER(smf_process_pdu_request)
{
    printf("[SMF] Processing PDU session creation request\n");
    
    // Add DL NAS transport header at bytes 10-13
    EVENT_PAYLOAD[10] = 0x68;  // DL NAS transport message type
    EVENT_PAYLOAD[11] = 0x01; // Spare/Payload type
    EVENT_PAYLOAD[12] = 0x00; // Payload container length (high)
    EVENT_PAYLOAD[13] = 0x47; // Payload container length (low)
    
    // Add SM headers at bytes 14-18
    EVENT_PAYLOAD[14] = 0x2e; // SM EPD
    EVENT_PAYLOAD[15] = 0x01; // PDU session ID
    EVENT_PAYLOAD[16] = 0x01; // PTI
    EVENT_PAYLOAD[17] = 0xc2; // PDU session accept message type
    EVENT_PAYLOAD[18] = 0x11; // SSC mode (bits 7-5) and PDU session type (bits 3-0)
    
    // Add QoS rules at bytes 19-29
    EVENT_PAYLOAD[19] = 0x00; // QoS rules IE
    EVENT_PAYLOAD[20] = 0x09; // QoS rules length
    EVENT_PAYLOAD[21] = 0x01; // QoS rule ID
    EVENT_PAYLOAD[22] = 0x00; // Rule length (high)
    EVENT_PAYLOAD[23] = 0x06; // Rule length (low)
    EVENT_PAYLOAD[24] = 0x31; // Rule operation code and DQR bit
    EVENT_PAYLOAD[25] = 0x31; // Number of packet filters
    EVENT_PAYLOAD[26] = 0x01; // Packet filter direction
    EVENT_PAYLOAD[27] = 0x01; // Packet filter ID
    EVENT_PAYLOAD[28] = 0xff; // Packet filter content length
    EVENT_PAYLOAD[29] = 0x01; // Packet filter content

    // Add Session-AMBR at bytes 30-36
    EVENT_PAYLOAD[30] = 0x06; // Session-AMBR IE
    EVENT_PAYLOAD[31] = 0x03; // Length for DL
    EVENT_PAYLOAD[32] = 0xf4; // DL AMBR unit and value
    EVENT_PAYLOAD[33] = 0x24; // DL AMBR value continued
    EVENT_PAYLOAD[34] = 0x03; // Length for UL  
    EVENT_PAYLOAD[35] = 0xf4; // UL AMBR unit and value
    EVENT_PAYLOAD[36] = 0x24; // UL AMBR value continued
    
    // Request IP allocation from UPF
    EVENT_PAYLOAD[0] = REQ_TYPE_ALLOCATE_IP;
    event_nf_ptr->input_payload_length = 37;
    
    printf("[SMF] Prepared PDU session headers, requesting UPF to allocate IP address\n");
    trigger_event(EVENT_TO_UPF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Request policy from PCF
EVENT_HANDLER(smf_request_policy)
{
    printf("[SMF] Requesting PCC rules from PCF\n");
    
    // Change request type
    EVENT_PAYLOAD[0] = REQ_TYPE_GET_PCC_RULES;
    
    // Forward to PCF
    trigger_event(EVENT_TO_PCF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Request DNN from UDM
EVENT_HANDLER(smf_request_dnn)
{
    printf("[SMF] Requesting DNN information from UDM\n");
    
    // Change request type
    EVENT_PAYLOAD[0] = REQ_TYPE_GET_DNN_INFO;
    
    // Forward to UDM
    trigger_event(EVENT_TO_UDM, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Complete PDU session
EVENT_HANDLER(smf_complete_pdu_session)
{
    printf("[SMF] Completing PDU session establishment\n");
    
    // Add QoS flow descriptions at bytes 47-55
    EVENT_PAYLOAD[47] = 0x79; // QoS flow descriptions IE
    EVENT_PAYLOAD[48] = 0x00; // Length (high)
    EVENT_PAYLOAD[49] = 0x06; // Length (low)
    EVENT_PAYLOAD[50] = 0x01; // QFI
    EVENT_PAYLOAD[51] = 0x20; // Operation code
    EVENT_PAYLOAD[52] = 0x41; // E bit and number of parameters
    EVENT_PAYLOAD[53] = 0x01; // 5QI
    EVENT_PAYLOAD[54] = 0x01; // Parameter length
    EVENT_PAYLOAD[55] = 0x09; // Parameter content
    
    // Add 5GSM cause at bytes 85-86
    EVENT_PAYLOAD[85] = 0x12; // 5GSM cause IE
    EVENT_PAYLOAD[86] = 0x01; // Cause value (SMF requested)
    
    // Set response type and send to AMF
    EVENT_PAYLOAD[0] = RESP_TYPE_PDU_SESSION_CREATED;
    event_nf_ptr->input_payload_length = 87;
    
    printf("[SMF] PDU session establishment complete, sending to AMF\n");
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

