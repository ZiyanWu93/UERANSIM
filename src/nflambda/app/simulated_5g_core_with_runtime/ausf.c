#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// AUSF (Authentication Server Function) Dispatcher
void ausf_dispatcher(EventNf* event)
{
    printf("[AUSF Dispatcher] Received event ID: %d\n", event->event_id);
    printf("[AUSF Dispatcher] Message length: %d bytes\n", event->input_payload_length);
    
    // Read message and classify (prototype only)
    if (event->input_payload_length > 0) {
        printf("[AUSF Dispatcher] Would route authentication related message\n");
    }
}

// Phase 1: Authentication procedures
void ausf_authenticate_request(void)
{
    printf("[AUSF] Processing authentication request\n");
}

// Phase 2: Authentication confirmation
void ausf_confirm_authentication(void)
{
    printf("[AUSF] Confirming authentication\n");
}

void ausf_derive_kseaf(void)
{
    printf("[AUSF] Deriving KSEAF\n");
}

// Service Function Chain Handler for Authentication Request

EVENT_HANDLER(ausf_set_rand)
{
    printf("[AUSF] Setting RAND value in authentication request\n");
    
    // Set RAND IEI at byte 7
    EVENT_PAYLOAD[7] = 0x21;  // RAND IEI
    
    // Set RAND value at bytes 8-23 (16 bytes)
    uint8_t rand_value[16] = {
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf,
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69
    };
    memcpy(&EVENT_PAYLOAD[8], rand_value, 16);
    
    // Trigger UDM to set AUTN
    trigger_event(EVENT_UDM_SET_AUTN, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}