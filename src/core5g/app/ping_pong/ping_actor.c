/*
 * Ping Actor Implementation
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ping_actor.h"
#include "common.h"
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"

// State variables
static int ping_count = 0;
static bool stopped = false;

// Event handler for EVENT_START
EVENT_HANDLER(handle_start)
{
    printf("PING: Starting ping-pong sequence\n");
    ping_count = 0;
    stopped = false;
    
    // Send first ping
    char payload[100];
    snprintf(payload, sizeof(payload), "ping_%d", ++ping_count);
    printf("PING[%d]: Sending \"%s\"\n", ping_count, payload);
    trigger_event(EVENT_PING_REQUEST, payload);
}

// Event handler for EVENT_PONG_RESPONSE
EVENT_HANDLER(handle_pong_response)
{
    char* payload = EVENT_PAYLOAD;
    printf("PING[%d]: Received pong: \"%s\"\n", ping_count, payload);
    
    // Check if we should continue
    if (ping_count >= MAX_PING_COUNT) {
        printf("PING: Reached maximum count (%d), stopping\n", MAX_PING_COUNT);
        stopped = true;
        trigger_event(EVENT_STOP, "max_count_reached");
        return;
    }
    
    // Send next ping
    char next_payload[100];
    snprintf(next_payload, sizeof(next_payload), "ping_%d", ++ping_count);
    printf("PING[%d]: Sending \"%s\"\n", ping_count, next_payload);
    trigger_event(EVENT_PING_REQUEST, next_payload);
}

// Event handler for EVENT_STOP
EVENT_HANDLER(ping_handle_stop)
{
    printf("PING: Received stop signal\n");
    stopped = true;
}

// Initialize the ping actor
void ping_init(void)
{
    ping_count = 0;
    stopped = false;
}

// Register ping actor's event handlers
void ping_register_handlers(void)
{
    printf("  - Ping actor: Registering handlers\n");
    register_event_handler(EVENT_START, handle_start);
    register_event_handler(EVENT_PONG_RESPONSE, handle_pong_response);
    register_event_handler(EVENT_STOP, ping_handle_stop);
}

// Check if ping actor has stopped
bool ping_is_stopped(void)
{
    return stopped;
}