#ifndef CORE5G_RUNTIME_H
#define CORE5G_RUNTIME_H

#include <signal.h>
#include "event_system/event.h"
#include "mailbox/mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of events that can be registered in the routing table */
#define MAX_EVENTS 200

/* Global variable to control runtime execution */
extern volatile sig_atomic_t keep_running;

/**
 * Signal handler for SIGINT (Ctrl+C)
 * Sets keep_running to 0 to gracefully terminate the runtime
 * 
 * @param sig Signal number (should be SIGINT)
 */
void handle_sigint(int sig);

/**
 * Register event handlers in the routing table
 * This function should be called during initialization to set up
 * the mapping between event IDs and their corresponding handler functions
 */
void register_handlers(void);

/**
 * Check for pending events in the mailbox
 * Logs the number of events found if any are present
 */
void listen_to_events(void);

/**
 * Trigger a new event by adding it to the mailbox
 * 
 * @param event_id The ID of the event to trigger (must be < MAX_EVENTS)
 * @param payload The hex-encoded NAS PDU payload for the event
 */
void trigger_event(int event_id, const char* payload);

/**
 * Process all pending events in the mailbox
 * Pops events from the mailbox, dispatches them to registered handlers,
 * and returns them to the memory pool
 */
void process_events(void);

/**
 * Main runtime loop
 * Initializes the event system, registers handlers, and runs the event
 * processing loop until keep_running is set to 0
 */
void runtime(void);

/**
 * Get the routing table for external registration
 * This allows other modules to register their event handlers
 * 
 * @return Pointer to the routing table array
 */
event_handler_t* get_routing_table(void);

/**
 * Register a single event handler
 * 
 * @param event_id The event ID to register (must be < MAX_EVENTS)
 * @param handler The handler function to call for this event
 * @return 0 on success, -1 on error (invalid event_id)
 */
int register_event_handler(int event_id, event_handler_t handler);

/**
 * Get the event mailbox instance
 * This allows other modules to interact with the mailbox if needed
 * 
 * @return Pointer to the global event mailbox
 */
Mailbox* get_event_mailbox(void);

#ifdef __cplusplus
}
#endif

#endif /* CORE5G_RUNTIME_H */