#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// AUSF (Authentication Server Function) Dispatcher
EVENT_HANDLER(ausf_dispatcher)
{
    printf("[AUSF Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[AUSF Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[AUSF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Check request type
    uint8_t request_type = EVENT_PAYLOAD[0];
    
    switch(request_type) {
        case REQ_TYPE_GEN_AUTH_DATA:
            printf("[AUSF Dispatcher] Generate Authentication Data request from AMF\n");
            // Trigger internal event for processing
            trigger_event(EVENT_AUSF_PROCESS_AUTH_REQ, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case REQ_TYPE_PREPARE_SEC_MODE:
            printf("[AUSF Dispatcher] Prepare Security Mode request from AMF\n");
            // Trigger internal event for security mode processing
            trigger_event(EVENT_AUSF_PROCESS_SEC_MODE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case RESP_TYPE_AUTH_VECTORS:
            printf("[AUSF Dispatcher] Authentication Vectors response from UDM\n");
            // Trigger internal event for completion
            trigger_event(EVENT_AUSF_COMPLETE_AUTH_DATA, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        case RESP_TYPE_UE_SEC_CAP:
            printf("[AUSF Dispatcher] UE Security Capabilities response from UDM\n");
            // Trigger internal event for security mode completion
            trigger_event(EVENT_AUSF_COMPLETE_SEC_MODE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            break;
            
        default:
            printf("[AUSF Dispatcher] Unknown request type: 0x%02x\n", request_type);
            break;
    }
}


// Handle authentication data request from AMF
EVENT_HANDLER(ausf_process_auth_request)
{
    printf("[AUSF] Processing authentication data request\n");
    
    // Set RAND IEI at byte 8 (accounting for request type byte)
    EVENT_PAYLOAD[8] = 0x21;  // RAND IEI
    
    // Set RAND value at bytes 9-24 (16 bytes)
    uint8_t rand_value[16] = {
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf,
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69
    };
    memcpy(&EVENT_PAYLOAD[9], rand_value, 16);
    
    // Change request type for UDM
    EVENT_PAYLOAD[0] = REQ_TYPE_GET_AUTH_VECTORS;
    
    printf("[AUSF] Added RAND, requesting authentication vectors from UDM\n");
    
    // Request authentication vectors from UDM
    trigger_event(EVENT_TO_UDM, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle authentication vectors response from UDM
EVENT_HANDLER(ausf_complete_auth_data)
{
    printf("[AUSF] Received authentication vectors from UDM\n");
    
    // Change request type for AMF response
    EVENT_PAYLOAD[0] = RESP_TYPE_AUTH_DATA;
    
    printf("[AUSF] Sending authentication data response to AMF\n");
    
    // Send response back to AMF
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle security mode request from AMF
EVENT_HANDLER(ausf_process_sec_mode)
{
    printf("[AUSF] Processing security mode request\n");
    
    // Set MAC at bytes 3-6
    uint8_t mac_value[] = {0x13, 0xbf, 0x99, 0x5a};
    memcpy(&EVENT_PAYLOAD[3], mac_value, 4);
    
    // Set NAS algorithms at byte 11
    EVENT_PAYLOAD[11] = 0x02;
    
    // Change request type for UDM
    EVENT_PAYLOAD[0] = REQ_TYPE_GET_UE_SEC_CAP;
    
    // Update length
    event_nf_ptr->input_payload_length = 12;
    
    printf("[AUSF] Added MAC and NAS algorithms, requesting UE security capabilities from UDM\n");
    
    // Request UE security capabilities from UDM
    trigger_event(EVENT_TO_UDM, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Handle security mode completion
EVENT_HANDLER(ausf_complete_sec_mode)
{
    printf("[AUSF] Completing security mode data\n");
    
    // Add IMEISV request at byte 18
    EVENT_PAYLOAD[18] = 0xe1;
    
    // Add additional security info at bytes 19-21
    EVENT_PAYLOAD[19] = 0x36;
    EVENT_PAYLOAD[20] = 0x01;
    EVENT_PAYLOAD[21] = 0x02;
    
    // Change request type for AMF response
    EVENT_PAYLOAD[0] = RESP_TYPE_SEC_MODE_DATA;
    
    // Update final length
    event_nf_ptr->input_payload_length = 22;
    
    printf("[AUSF] Sending security mode data response to AMF\n");
    
    // Send response back to AMF
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}