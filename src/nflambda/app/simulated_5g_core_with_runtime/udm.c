#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// UDM (Unified Data Management) Dispatcher
void udm_dispatcher(EventNf* event)
{
    printf("[UDM Dispatcher] Received event ID: %d\n", event->event_id);
    printf("[UDM Dispatcher] Message length: %d bytes\n", event->input_payload_length);
    
    // Read message and classify (prototype only)
    if (event->input_payload_length > 0) {
        printf("[UDM Dispatcher] Would route subscriber data related message\n");
    }
}

// Phase 1: Authentication vector generation
void udm_generate_authentication_vector(void)
{
    printf("[UDM] Generating authentication vector\n");
}

// Phase 3: Subscriber data retrieval
void udm_retrieve_subscription_data(void)
{
    printf("[UDM] Retrieving subscription data\n");
}

void udm_update_subscriber_status(void)
{
    printf("[UDM] Updating subscriber status\n");
}

// Service Function Chain Handler for Authentication Request

EVENT_HANDLER(udm_set_autn)
{
    printf("[UDM] Setting AUTN value in authentication request\n");
    
    // Set AUTN IEI at byte 24
    EVENT_PAYLOAD[24] = 0x20;  // AUTN IEI
    
    // Set AUTN length at byte 25
    EVENT_PAYLOAD[25] = 0x10;  // AUTN length (16 bytes)
    
    // Set AUTN value at bytes 26-41 (16 bytes)
    uint8_t autn_value[16] = {
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00,
        0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19
    };
    memcpy(&EVENT_PAYLOAD[26], autn_value, 16);
    
    // Trigger AMF to send the complete authentication request
    trigger_event(EVENT_AMF_SEND_AUTH_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}