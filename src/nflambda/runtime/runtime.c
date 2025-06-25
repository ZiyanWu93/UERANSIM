#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  // For usleep
#include <time.h>
#include "runtime.h"
#include "../event_system/event.h"
#include "../event_system/event_pool.h"
#include "../mailbox/mailbox.h"
#include "../memory/memory_utils.h"

volatile sig_atomic_t keep_running = 1;

// Global mailbox for event queue
static Mailbox event_mailbox;

// Routing table: index = event number
static event_handler_t routing_table[MAX_EVENTS] = {0};

// Event source management
static EventSource event_sources[MAX_EVENT_SOURCES];
static int num_event_sources = 0;

// Exit condition management
static ExitCondition exit_conditions[MAX_EXIT_CONDITIONS];
static int num_exit_conditions = 0;

// Handler registrar management
static handler_registrar_fn handler_registrars[MAX_HANDLER_REGISTRARS];
static int num_handler_registrars = 0;

// Error handling
static error_handler_fn custom_error_handler = NULL;

// Runtime statistics
static RuntimeStats runtime_stats = {0};

// Default error handler
static void default_error_handler(int event_id, const char* error)
{
    fprintf(stderr, "Runtime error - Event ID: %d, Error: %s\n", event_id, error);
}

// Built-in exit condition for signal handling
static bool signal_exit_condition(void)
{
    return keep_running == 0;
}

void handle_sigint(int sig)
{
    keep_running = 0;
    printf("Caught SIGINT, exiting...\n");
}

void register_handlers()
{
    // Call all registered handler registrars
    for (int i = 0; i < num_handler_registrars; i++) {
        if (handler_registrars[i]) {
            handler_registrars[i]();
        }
    }
}


void listen_to_events()
{
    // Poll all enabled event sources
    for (int i = 0; i < num_event_sources; i++) {
        if (event_sources[i].enabled && event_sources[i].poll_fn) {
            event_sources[i].poll_fn();
        }
    }
}


void trigger_event(int event_id, const void* payload, int length)
{
    if (event_id < 0 || event_id >= MAX_EVENTS) {
        runtime_stats.handler_errors++;
        error_handler_fn error_fn = custom_error_handler ? custom_error_handler : default_error_handler;
        error_fn(event_id, "Invalid event ID");
        return;
    }
    
    if (routing_table[event_id] == NULL) {
        runtime_stats.handler_errors++;
        error_handler_fn error_fn = custom_error_handler ? custom_error_handler : default_error_handler;
        error_fn(event_id, "No handler registered for event");
        return;
    }
    
    // Create event data
    EventNf event_data;
    event_data.event_id = event_id;
    
    // Copy binary payload with bounds checking
    if (length > MAX_EVENT_PAYLOAD_SIZE) {
        length = MAX_EVENT_PAYLOAD_SIZE;
    }
    if (payload && length > 0) {
        memcpy(event_data.input_payload, payload, length);
    }
    event_data.input_payload_length = length;
    
    // Put event in mailbox
    if (mailbox_put(&event_mailbox, &event_data)) {
        // Success - event queued
    } else {
        runtime_stats.events_dropped++;
        error_handler_fn error_fn = custom_error_handler ? custom_error_handler : default_error_handler;
        error_fn(event_id, "Mailbox full - event dropped");
    }
}


void process_events()
{
    // Update current queue depth statistic
    uint32_t queue_depth = mailbox_count(&event_mailbox);
    runtime_stats.current_queue_depth = queue_depth;
    if (queue_depth > runtime_stats.max_queue_depth) {
        runtime_stats.max_queue_depth = queue_depth;
    }
    
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
            // Execute handler with error protection
            routing_table[event->event_id]();
            runtime_stats.events_processed++;
        } else {
            // Handle invalid event ID
            runtime_stats.handler_errors++;
            error_handler_fn error_fn = custom_error_handler ? custom_error_handler : default_error_handler;
            error_fn(event->event_id, "No handler registered for event");
        }
        
        // Return event to mailbox's pool
        mailbox_return_event(&event_mailbox, event);
        event_nf_ptr = NULL;
    }
}


// Check all registered exit conditions
static bool check_exit_conditions(void)
{
    for (int i = 0; i < num_exit_conditions; i++) {
        if (exit_conditions[i].check_fn && exit_conditions[i].check_fn()) {
            printf("Exit condition '%s' triggered\n", exit_conditions[i].name);
            return true;
        }
    }
    return false;
}

void runtime()
{
    printf("Initializing runtime...\n");
    
    // Register built-in exit condition for signal handling
    register_exit_condition("signal", signal_exit_condition);
    
    // Call all handler registrars
    register_handlers();
    
    // Initialize subsystems in correct order
    // 1. Memory system first (foundation for everything)
    if (!memory_system_init()) {
        fprintf(stderr, "Failed to initialize memory system\n");
        return;
    }
    
    // 2. Event pool (now uses memory system)
    initialize_event_pool();
    
    // 3. Mailbox (also uses memory system)
    mailbox_init(&event_mailbox);
    
    printf("Runtime initialized successfully\n");
    printf("Registered %d event sources, %d exit conditions\n", 
           num_event_sources, num_exit_conditions);
    
    // Main event loop
    while (!check_exit_conditions())
    {
        listen_to_events();
        process_events();
        
        // Small sleep to prevent CPU spinning when idle
        if (mailbox_is_empty(&event_mailbox)) {
            usleep(1000);  // 1ms sleep
        }
    }
    
    // Cleanup
    mailbox_cleanup(&event_mailbox);
    memory_system_cleanup();  // Clean up memory system last
    
    printf("Runtime statistics:\n");
    printf("  Events processed: %lu\n", runtime_stats.events_processed);
    printf("  Events dropped: %lu\n", runtime_stats.events_dropped);
    printf("  Handler errors: %lu\n", runtime_stats.handler_errors);
    printf("  Max queue depth: %u\n", runtime_stats.max_queue_depth);
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

// API Implementation

int register_event_source(const char* name, event_source_fn fn)
{
    if (num_event_sources >= MAX_EVENT_SOURCES) {
        return -1;
    }
    
    event_sources[num_event_sources].name = name;
    event_sources[num_event_sources].poll_fn = fn;
    event_sources[num_event_sources].enabled = true;
    num_event_sources++;
    
    return 0;
}

int register_exit_condition(const char* name, exit_condition_fn fn)
{
    if (num_exit_conditions >= MAX_EXIT_CONDITIONS) {
        return -1;
    }
    
    exit_conditions[num_exit_conditions].name = name;
    exit_conditions[num_exit_conditions].check_fn = fn;
    num_exit_conditions++;
    
    return 0;
}

int register_handler_registrar(handler_registrar_fn fn)
{
    if (num_handler_registrars >= MAX_HANDLER_REGISTRARS) {
        return -1;
    }
    
    handler_registrars[num_handler_registrars] = fn;
    num_handler_registrars++;
    
    return 0;
}

void set_error_handler(error_handler_fn fn)
{
    custom_error_handler = fn;
}

RuntimeStats* get_runtime_stats(void)
{
    return &runtime_stats;
}

int set_event_source_enabled(const char* name, bool enabled)
{
    for (int i = 0; i < num_event_sources; i++) {
        if (strcmp(event_sources[i].name, name) == 0) {
            event_sources[i].enabled = enabled;
            return 0;
        }
    }
    return -1;
}

