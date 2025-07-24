#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "upf.h"
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// UPF (User Plane Function) Dispatcher
EVENT_HANDLER(upf_dispatcher)
{
    printf("[UPF Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[UPF Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[UPF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Check for service chain request type
    uint8_t request_type = EVENT_PAYLOAD[0];
    
    if (request_type == REQ_TYPE_ALLOCATE_IP) {
        // IP allocation request from SMF
        printf("[UPF Dispatcher] Received IP allocation request from SMF\n");
        trigger_event(EVENT_UPF_ALLOCATE_IP, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    printf("[UPF Dispatcher] Unknown request type: 0x%02x\n", request_type);
}

// Allocate IP address
EVENT_HANDLER(upf_allocate_ip)
{
    printf("[UPF] Allocating IP address for PDU session\n");
    
    // Add PDU address at bytes 37-43
    EVENT_PAYLOAD[37] = 0x29; // PDU address IE
    EVENT_PAYLOAD[38] = 0x05; // Length
    EVENT_PAYLOAD[39] = 0x01; // PDU session type (IPv4)
    EVENT_PAYLOAD[40] = 0x0a; // IP address: 10.45.0.2
    EVENT_PAYLOAD[41] = 0x2d; // 45
    EVENT_PAYLOAD[42] = 0x00; // 0
    EVENT_PAYLOAD[43] = 0x02; // 2
    
    // Set response type and send back to SMF
    EVENT_PAYLOAD[0] = RESP_TYPE_IP_ALLOCATED;
    event_nf_ptr->input_payload_length = 44;
    
    printf("[UPF] IP address allocated (10.45.0.2), sending to SMF\n");
    trigger_event(EVENT_TO_SMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

