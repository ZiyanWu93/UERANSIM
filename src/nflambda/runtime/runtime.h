#ifndef CORE5G_RUNTIME_H
#define CORE5G_RUNTIME_H

#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include "../event_system/event.h"
#include "../mailbox/mailbox.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum number of events that can be registered in the routing table */
#define MAX_EVENTS 200

/* Maximum number of event sources that can be registered */
#define MAX_EVENT_SOURCES 32

/* Maximum number of exit conditions that can be registered */
#define MAX_EXIT_CONDITIONS 16

/* Maximum number of handler registrars that can be registered */
#define MAX_HANDLER_REGISTRARS 32

/* Global variable to control runtime execution */
extern volatile sig_atomic_t keep_running;

/* Type definitions for runtime framework */
typedef void (*event_source_fn)(void);
typedef bool (*exit_condition_fn)(void);
typedef void (*handler_registrar_fn)(void);
typedef void (*error_handler_fn)(int event_id, const char* error);

/* Event source structure */
typedef struct {
    const char* name;
    event_source_fn poll_fn;
    bool enabled;
} EventSource;

/* Exit condition structure */
typedef struct {
    const char* name;
    exit_condition_fn check_fn;
} ExitCondition;

/* Runtime statistics structure */
typedef struct {
    uint64_t events_processed;
    uint64_t events_dropped;
    uint64_t handler_errors;
    uint32_t max_queue_depth;
    uint32_t current_queue_depth;
} RuntimeStats;

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

/**
 * Register an event source that will be polled during listen_to_events
 * 
 * @param name Descriptive name for the event source
 * @param fn Function to call for polling events
 * @return 0 on success, -1 if maximum sources reached
 */
int register_event_source(const char* name, event_source_fn fn);

/**
 * Register an exit condition that will be checked in the main loop
 * 
 * @param name Descriptive name for the exit condition
 * @param fn Function that returns true when runtime should exit
 * @return 0 on success, -1 if maximum conditions reached
 */
int register_exit_condition(const char* name, exit_condition_fn fn);

/**
 * Register a handler registrar function to be called during initialization
 * This allows modules to register their event handlers without coupling
 * 
 * @param fn Function that registers event handlers
 * @return 0 on success, -1 if maximum registrars reached
 */
int register_handler_registrar(handler_registrar_fn fn);

/**
 * Set a custom error handler for runtime errors
 * 
 * @param fn Error handler function (NULL to use default)
 */
void set_error_handler(error_handler_fn fn);

/**
 * Get runtime statistics
 * 
 * @return Pointer to runtime statistics structure
 */
RuntimeStats* get_runtime_stats(void);

/**
 * Enable or disable an event source by name
 * 
 * @param name Name of the event source
 * @param enabled true to enable, false to disable
 * @return 0 on success, -1 if source not found
 */
int set_event_source_enabled(const char* name, bool enabled);

#ifdef __cplusplus
}
#endif

#endif /* CORE5G_RUNTIME_H */