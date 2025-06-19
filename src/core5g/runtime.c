#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "event.h"
#include "event_system/event_pool.h"

volatile sig_atomic_t keep_running = 1;

void handle_sigint(int sig)
{
    keep_running = 0;
    printf("Caught SIGINT, exiting...\n");
}

// Example event handlers - they don't interact with event_nf directly
EVENT_HANDLER(handle_event_0) { 
    printf("Handling event 0\n"); 
    printf("Input payload: %s\n", EVENT_PAYLOAD);
    strcpy(EVENT_OUTPUT_PAYLOAD, "Event 0 processed");
}

EVENT_HANDLER(handle_event_1) { 
    printf("Handling event 1\n"); 
    printf("Input payload: %s\n", EVENT_PAYLOAD);
    strcpy(EVENT_OUTPUT_PAYLOAD, "Event 1 processed");
}

// Routing table: index = event number
#define MAX_EVENTS 10
event_handler_t routing_table[MAX_EVENTS] = {0};

void register_handlers()
{
    routing_table[0] = handle_event_0;
    routing_table[1] = handle_event_1;
}


void listen_to_events()
{
    printf("Listening to events...\n");
}

void exit_condition()
{
    printf("Exit condition met, exiting...\n");
}

void trigger_event(int event_id, const char* payload)
{
    if (event_id >= 0 && event_id < MAX_EVENTS && routing_table[event_id] != NULL) {
        // Acquire a new event from the event pool
        EventNf* event = allocate_event();
        if (event == NULL) {
            printf("Failed to allocate event for ID %d\n", event_id);
            return;
        }
        
        // Set the global event pointer to point to our newly allocated event
        event_nf_ptr = event;
        
        // Populate the event with the provided payload
        event_nf_ptr->event_id = event_id;
        strncpy(event_nf_ptr->input_payload, payload, MAX_NAS_HEX_LEN - 1);
        event_nf_ptr->input_payload[MAX_NAS_HEX_LEN - 1] = '\0'; // Ensure null-termination
        
        printf("Event %d triggered with payload: %s\n", event_id, payload);
    } else {
        // raise error
        printf("Invalid event ID %d or no handler registered\n", event_id);
        exit(1);
    }
}

void process_events()
{
    printf("Processing events...\n");
    
    // After processing, return event to the pool
    if (event_nf_ptr != NULL) {
        return_event_to_pool(event_nf_ptr);
        event_nf_ptr = NULL; // Reset global pointer
    }
}


void runtime()
{
    register_handlers();  // Set up routing table
    initialize_event_pool(); // Initialize the event pool
    
    while (keep_running)
    {
        exit_condition();
        listen_to_events();
        process_events();
    }
    printf("Exiting runtime gracefully...\n");
}

int main()
{
    signal(SIGINT, handle_sigint);
    runtime();
    return 0;
}
