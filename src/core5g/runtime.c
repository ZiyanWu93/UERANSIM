#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For usleep
#include "event_system/event.h"
#include "event_system/event_pool.h"
#include "mailbox/mailbox.h"
#include "actor/amf_actor.h"

volatile sig_atomic_t keep_running = 1;

// Global mailbox for event queue
static Mailbox event_mailbox;

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
#define MAX_EVENTS 200  // Increased to accommodate NAS events (100+)
event_handler_t routing_table[MAX_EVENTS] = {0};

// Hardcoded NAS PDUs for testing (from amf.c comments)
static const char* NAS_PDU_REGISTRATION_REQUEST = "7e004179000d0199f9070000000000000000102e0480f080f0";
static const char* NAS_PDU_AUTH_RESPONSE = "7e00572d10ef2770c69e7382aa38e8134f602234e1";
static const char* NAS_PDU_SECURITY_MODE_COMPLETE = "7e0422e4ee19007e005e7700094573806121856151f17100237e004179000d0199f9070000000000000000101001002e0480f080f02f020101530100";
static const char* NAS_PDU_REGISTRATION_COMPLETE = "7e02469d6a8b017e0043";
static const char* NAS_PDU_PDU_SESSION_REQUEST = "7e02ba0292cd027e00670100152e0101c1ffff91a12801007b000780000a00000d00120181220101250908696e7465726e6574";

// Current step in the NAS flow simulation
static int nas_flow_step = 0;
static int should_simulate_nas = 0;

void register_handlers()
{
    // Register test handlers
    routing_table[0] = handle_event_0;
    routing_table[1] = handle_event_1;
    
    // Register AMF actor handlers for NAS events
    routing_table[EVENT_NAS_REGISTRATION_REQUEST] = amf_process_registration_request;
    routing_table[EVENT_NAS_AUTH_RESPONSE] = amf_handle_auth_response;
    routing_table[EVENT_NAS_SECURITY_MODE_COMPLETE] = amf_handle_security_mode_complete;
    routing_table[EVENT_NAS_REGISTRATION_COMPLETE] = amf_handle_configuration_update_trigger;
    routing_table[EVENT_NAS_PDU_SESSION_REQUEST] = amf_handle_pdu_session_establishment_request;
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
        event_data.output_payload[0] = '\0';
        
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

// Simulate uplink NAS message arriving from UE
void simulate_uplink_nas_event(int event_id, const char* nas_pdu)
{
    printf("\n=== Simulating Uplink NAS Event ===\n");
    printf("Event Type: ");
    switch(event_id) {
        case EVENT_NAS_REGISTRATION_REQUEST:
            printf("Registration Request\n");
            break;
        case EVENT_NAS_AUTH_RESPONSE:
            printf("Authentication Response\n");
            break;
        case EVENT_NAS_SECURITY_MODE_COMPLETE:
            printf("Security Mode Complete\n");
            break;
        case EVENT_NAS_REGISTRATION_COMPLETE:
            printf("Registration Complete\n");
            break;
        case EVENT_NAS_PDU_SESSION_REQUEST:
            printf("PDU Session Establishment Request\n");
            break;
        default:
            printf("Unknown (ID: %d)\n", event_id);
    }
    
    trigger_event(event_id, nas_pdu);
}

// Simulate complete NAS registration and PDU session flow
void simulate_nas_flow()
{
    printf("\n=== Starting NAS Flow Simulation ===\n");
    
    // Trigger initial registration request
    simulate_uplink_nas_event(EVENT_NAS_REGISTRATION_REQUEST, NAS_PDU_REGISTRATION_REQUEST);
    nas_flow_step = 1;
}

void process_events()
{
    // Process all events in the mailbox
    if (!mailbox_is_empty(&event_mailbox)) {
        printf("Processing events from mailbox...\n");
    }
    while (!mailbox_is_empty(&event_mailbox)) {
        // Pop event from mailbox
        EventNf* event = mailbox_pop(&event_mailbox);
        if (event == NULL) {
            break;
        }
        
        // Set global event pointer
        event_nf_ptr = event;
        
        printf("Processing event ID: %d\n", event->event_id);
        
        // Dispatch to handler
        if (event->event_id >= 0 && event->event_id < MAX_EVENTS && routing_table[event->event_id] != NULL) {
            routing_table[event->event_id]();
            
            // Log output if present
            if (strlen(EVENT_OUTPUT_PAYLOAD) > 0) {
                printf("Downlink NAS PDU: %s\n", EVENT_OUTPUT_PAYLOAD);
            }
        } else {
            printf("No handler registered for event ID %d\n", event->event_id);
        }
        
        // Return event to mailbox's pool
        mailbox_return_event(&event_mailbox, event);
        event_nf_ptr = NULL;
        
        // Trigger next event in NAS flow if we're simulating
        if (nas_flow_step > 0) {
            usleep(100000); // 100ms delay between events
            
            switch(nas_flow_step) {
                case 1: // After registration request, expect auth response
                    simulate_uplink_nas_event(EVENT_NAS_AUTH_RESPONSE, NAS_PDU_AUTH_RESPONSE);
                    nas_flow_step = 2;
                    break;
                case 2: // After auth response, expect security mode complete
                    simulate_uplink_nas_event(EVENT_NAS_SECURITY_MODE_COMPLETE, NAS_PDU_SECURITY_MODE_COMPLETE);
                    nas_flow_step = 3;
                    break;
                case 3: // After security mode complete, expect registration complete
                    simulate_uplink_nas_event(EVENT_NAS_REGISTRATION_COMPLETE, NAS_PDU_REGISTRATION_COMPLETE);
                    nas_flow_step = 4;
                    break;
                case 4: // After registration complete, expect PDU session request
                    simulate_uplink_nas_event(EVENT_NAS_PDU_SESSION_REQUEST, NAS_PDU_PDU_SESSION_REQUEST);
                    nas_flow_step = 5;
                    break;
                case 5: // Flow complete
                    printf("\n=== NAS Flow Simulation Complete ===\n");
                    nas_flow_step = 0;
                    // Exit after simulation completes
                    if (should_simulate_nas) {
                        keep_running = 0;
                    }
                    break;
            }
        }
    }
}


void runtime()
{
    printf("Initializing runtime...\n");
    
    register_handlers();  // Set up routing table
    initialize_event_pool(); // Initialize the event pool
    mailbox_init(&event_mailbox); // Initialize the mailbox
    
    printf("Runtime initialized successfully\n");
    
    // Start NAS simulation if requested
    if (should_simulate_nas) {
        usleep(500000); // 500ms delay before starting
        simulate_nas_flow();
    }
    
    while (keep_running)
    {
        listen_to_events();
        process_events();
        
        // Small delay to prevent busy waiting
        usleep(10000); // 10ms
    }
    
    // Cleanup
    mailbox_cleanup(&event_mailbox);
    printf("Exiting runtime gracefully...\n");
}

int main(int argc, char* argv[])
{
    signal(SIGINT, handle_sigint);
    
    // Check for simulation flag
    if (argc > 1 && strcmp(argv[1], "--simulate-nas") == 0) {
        printf("NAS flow simulation will start after runtime initialization\n");
        should_simulate_nas = 1;
    }
    
    runtime();
    return 0;
}
