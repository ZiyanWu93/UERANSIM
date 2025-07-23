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
        case SFC_TYPE_GET_AUTH_VECTORS_REQ:
            printf("[UDM Dispatcher] Get Authentication Vectors request from AUSF\n");
            // Trigger internal event for processing
            trigger_event(EVENT_UDM_GEN_AUTH_VECTORS, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
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
    EVENT_PAYLOAD[0] = SFC_TYPE_AUTH_VECTORS_RESP;
    
    printf("[UDM] Generated authentication vectors, sending response to AUSF\n");
    
    // Send response back to AUSF
    trigger_event(EVENT_TO_AUSF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}