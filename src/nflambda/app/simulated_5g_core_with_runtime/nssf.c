#include <stdio.h>
#include <stdint.h>
#include "end_to_end_events.h"
#include "../../event_system/event.h"

// NSSF (Network Slice Selection Function) Dispatcher
void nssf_dispatcher(EventNf* event)
{
    printf("[NSSF Dispatcher] Received event ID: %d\n", event->event_id);
    printf("[NSSF Dispatcher] Message length: %d bytes\n", event->input_payload_length);
    
    // Read message and classify (prototype only)
    if (event->input_payload_length > 0) {
        printf("[NSSF Dispatcher] Would route network slice selection message\n");
    }
}

