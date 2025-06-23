/*
 * Pong Actor Implementation
 */

#include <stdio.h>
#include <string.h>
#include "pong_actor.h"
#include "common.h"
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"

// Event handler for EVENT_PING_REQUEST
EVENT_HANDLER(handle_ping_request)
{
    char* payload = EVENT_PAYLOAD;
    printf("PONG: Received ping: \"%s\"\n", payload);
    
    // Create pong response
    char response[100];
    snprintf(response, sizeof(response), "pong_for_%s", payload);
    
    printf("PONG: Sending response: \"%s\"\n", response);
    trigger_event(EVENT_PONG_RESPONSE, response);
}

// Event handler for EVENT_STOP
EVENT_HANDLER(pong_handle_stop)
{
    printf("PONG: Received stop signal\n");
}

// Initialize the pong actor
void pong_init(void)
{
    // Pong actor has no state to initialize
}

// Register pong actor's event handlers
void pong_register_handlers(void)
{
    printf("  - Pong actor: Registering handlers\n");
    register_event_handler(EVENT_PING_REQUEST, handle_ping_request);
    register_event_handler(EVENT_STOP, pong_handle_stop);
}