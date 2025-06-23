#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For usleep
#include "runtime.h"
#include "../event_system/event.h"
#include "../event_system/event_pool.h"
#include "../mailbox/mailbox.h"
#include "../actor/amf_actor.h"

volatile sig_atomic_t keep_running = 1;
// Global mailbox for event queue
static Mailbox event_mailbox;

void handle_sigint(int sig)
{
    keep_running = 0;
    printf("Caught SIGINT, exiting...\n");
}

// Routing table: index = event number
static event_handler_t routing_table[MAX_EVENTS] = {0};
void register_handlers()
{
}


void listen_to_events()
{
    // Check if there are events in the mailbox
    if (!mailbox_is_empty(&event_mailbox)) {
        printf("Found %d events in mailbox\n", mailbox_count(&event_mailbox));
    }
}


void trigger_event(int event_id, const char* payload)
{
    if (event_id >= 0 && event_id < MAX_EVENTS && routing_table[event_id] != NULL) {
        // Create event data
        EventNf event_data;
        event_data.event_id = event_id;
        strncpy(event_data.input_payload, payload, MAX_NAS_HEX_LEN - 1);
        event_data.input_payload[MAX_NAS_HEX_LEN - 1] = '\0';
        event_data.input_payload_length = strlen(payload);
        
        // Put event in mailbox
        if (mailbox_put(&event_mailbox, &event_data)) {
            printf("Event %d enqueued to mailbox\n", event_id);
        } else {
            printf("Failed to enqueue event %d - mailbox full\n", event_id);
        }
    } else {
        printf("Invalid event ID %d or no handler registered\n", event_id);
    }
}


void process_events()
{
    while (!mailbox_is_empty(&event_mailbox)) {
        // Pop event from mailbox
        EventNf* event = mailbox_pop(&event_mailbox);
        if (event == NULL) {
            break;
        }
        
        // Set global event pointer
        event_nf_ptr = event;
        
        // Dispatch to handler through routing table
        if (event->event_id >= 0 && event->event_id < MAX_EVENTS && routing_table[event->event_id] != NULL) {
            routing_table[event->event_id]();
        }
        
        // Return event to mailbox's pool
        mailbox_return_event(&event_mailbox, event);
        event_nf_ptr = NULL;
    }
}


void runtime()
{
    printf("Initializing runtime...\n");
    
    register_handlers();  // Set up routing table
    initialize_event_pool(); // Initialize the event pool
    mailbox_init(&event_mailbox); // Initialize the mailbox
    
    printf("Runtime initialized successfully\n");
    
    while (keep_running)
    {
        listen_to_events();
        process_events();
    }
    
    // Cleanup
    mailbox_cleanup(&event_mailbox);
    printf("Exiting runtime gracefully...\n");
}

event_handler_t* get_routing_table(void)
{
    return routing_table;
}

int register_event_handler(int event_id, event_handler_t handler)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        return -1;
    }
    routing_table[event_id] = handler;
    return 0;
}

Mailbox* get_event_mailbox(void)
{
    return &event_mailbox;
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_sigint);
    runtime();
    return 0;
}
