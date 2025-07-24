#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// UDM (Unified Data Management) Dispatcher
EVENT_HANDLER(udm_dispatcher)
{
    printf("[UDM Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[UDM Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[UDM Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Check request type
    uint8_t request_type = EVENT_PAYLOAD[0];
    
    switch(request_type) {
        case REQ_TYPE_GET_AUTH_VECTORS:
            printf("[UDM Dispatcher] Get Authentication Vectors request from AUSF\n");
            // Trigger internal event for processing
            trigger_event(EVENT_UDM_GEN_AUTH_VECTORS, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case REQ_TYPE_GET_UE_SEC_CAP:
            printf("[UDM Dispatcher] Get UE Security Capabilities request from AUSF\n");
            // Trigger internal event for providing security capabilities
            trigger_event(EVENT_UDM_PROVIDE_SEC_CAP, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case REQ_TYPE_PREPARE_REG_ACCEPT:
            printf("[UDM Dispatcher] Prepare Registration Accept request from AMF\n");
            // Trigger internal event for providing subscription data
            trigger_event(EVENT_UDM_PROVIDE_SUBSCRIPTION, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case REQ_TYPE_GET_DNN_INFO:
            printf("[UDM Dispatcher] Get DNN information request from SMF\n");
            // Trigger internal event for providing DNN
            trigger_event(EVENT_UDM_PROVIDE_DNN, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        default:
            printf("[UDM Dispatcher] Unknown request type: 0x%02x\n", request_type);
            break;
    }
}


// Handle authentication vectors request from AUSF
EVENT_HANDLER(udm_gen_auth_vectors)
{
    printf("[UDM] Generating authentication vectors\n");
    
    // Set AUTN IEI at byte 25 (accounting for request type byte)
    EVENT_PAYLOAD[25] = 0x20;  // AUTN IEI
    
    // Set AUTN length at byte 26
    EVENT_PAYLOAD[26] = 0x10;  // AUTN length (16 bytes)
    
    // Set AUTN value at bytes 27-42 (16 bytes)
    uint8_t autn_value[16] = {
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00,
        0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19
    };
    memcpy(&EVENT_PAYLOAD[27], autn_value, 16);
    
    // Change request type for AUSF response
    EVENT_PAYLOAD[0] = RESP_TYPE_AUTH_VECTORS;
    
    printf("[UDM] Generated authentication vectors, sending response to AUSF\n");
    
    // Send response back to AUSF
    trigger_event(EVENT_TO_AUSF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle UE security capabilities request from AUSF
EVENT_HANDLER(udm_provide_sec_cap)
{
    printf("[UDM] Providing UE security capabilities\n");
    
    // Set ngKSI field at byte 12
    EVENT_PAYLOAD[12] = 0x00;
    
    // Set UE security capability length at byte 13
    EVENT_PAYLOAD[13] = 0x04;
    
    // Set UE security capability value at bytes 14-17
    uint8_t ue_sec_cap[] = {0x80, 0xf0, 0x80, 0xf0};
    memcpy(&EVENT_PAYLOAD[14], ue_sec_cap, 4);
    
    // Change request type for AUSF response
    EVENT_PAYLOAD[0] = RESP_TYPE_UE_SEC_CAP;
    
    // Update length
    event_nf_ptr->input_payload_length = 18;
    
    printf("[UDM] Provided UE security capabilities, sending response to AUSF\n");
    
    // Send response back to AUSF
    trigger_event(EVENT_TO_AUSF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle subscription data request from AMF
EVENT_HANDLER(udm_provide_subscription)
{
    printf("[UDM] Providing subscription data\n");
    
    // Add allowed NSSAI at bytes 36-39
    EVENT_PAYLOAD[36] = 0x15;  // NSSAI IE
    EVENT_PAYLOAD[37] = 0x02;  // Length
    EVENT_PAYLOAD[38] = 0x01;  // S-NSSAI SST
    EVENT_PAYLOAD[39] = 0x01;  // S-NSSAI SD
    
    // Change request type for AMF response
    EVENT_PAYLOAD[0] = RESP_TYPE_SUBSCRIPTION_DATA;
    
    // Update length
    event_nf_ptr->input_payload_length = 40;
    
    printf("[UDM] Provided allowed NSSAI, sending response to AMF\n");
    
    // Send response back to AMF
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle DNN information request from SMF
EVENT_HANDLER(udm_provide_dnn)
{
    printf("[UDM] Providing DNN information for PDU session\n");
    
    // Add S-NSSAI at bytes 44-46
    EVENT_PAYLOAD[44] = 0x22; // S-NSSAI IE
    EVENT_PAYLOAD[45] = 0x01; // Length
    EVENT_PAYLOAD[46] = 0x01; // S-NSSAI value
    
    // Add DNN at bytes 74-83
    EVENT_PAYLOAD[74] = 0x25; // DNN IE
    EVENT_PAYLOAD[75] = 0x09; // Length
    EVENT_PAYLOAD[76] = 0x08; // DNN value: "internet"
    EVENT_PAYLOAD[77] = 0x69; // 'i'
    EVENT_PAYLOAD[78] = 0x6e; // 'n'
    EVENT_PAYLOAD[79] = 0x74; // 't'
    EVENT_PAYLOAD[80] = 0x65; // 'e'
    EVENT_PAYLOAD[81] = 0x72; // 'r'
    EVENT_PAYLOAD[82] = 0x6e; // 'n'
    EVENT_PAYLOAD[83] = 0x65; // 'e'
    EVENT_PAYLOAD[84] = 0x74; // 't'
    
    // Change request type for SMF response
    EVENT_PAYLOAD[0] = RESP_TYPE_DNN_INFO;
    
    // Update length
    event_nf_ptr->input_payload_length = 85;
    
    printf("[UDM] Provided DNN and S-NSSAI, sending response to SMF\n");
    
    // Send response back to SMF
    trigger_event(EVENT_TO_SMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}