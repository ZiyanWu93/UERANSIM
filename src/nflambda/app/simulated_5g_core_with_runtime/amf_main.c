#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include "amf_common.h"
#include "fiveg_core_actor.h"
#include "ueransim_actor.h"
#include "../../runtime/runtime.h"

// Event structures for runtime
EventNf event_nf;
EventNf* event_nf_ptr = &event_nf;

// The handle_sigint function is provided by the runtime library

// Error handler
static void error_handler(int event_id, const char* message)
{
    fprintf(stderr, "[Main] Error for event %d: %s\n", event_id, message);
}

// Exit condition - check if 5G Core has stopped
static bool should_exit(void)
{
    return fiveg_core_stopped();
}

// Handler registrar for 5G Core actor
static void fiveg_core_handler_registrar(void)
{
    fiveg_core_register_handlers();
}

// Handler registrar for UERANSIM actor
static void ueransim_handler_registrar(void)
{
    ueransim_register_handlers();
}

int main(int argc, char* argv[])
{
    printf("=== AMF Actor-based Simulator ===\n");
    printf("Simulating 5G registration and PDU session establishment flow\n\n");
    
    // Initialize actors
    fiveg_core_init();
    ueransim_init();
    
    // Register handler registrars
    register_handler_registrar(fiveg_core_handler_registrar);
    register_handler_registrar(ueransim_handler_registrar);
    
    // Register event sources
    register_event_source("ueransim_startup", ueransim_startup_event_source);
    
    // Register exit condition
    register_exit_condition("fiveg_core_stopped", should_exit);
    
    // Set error handler
    set_error_handler(error_handler);
    
    // Setup signal handlers
    signal(SIGINT, handle_sigint);
    signal(SIGTERM, handle_sigint);
    
    printf("[Main] Starting runtime...\n\n");
    
    // Run the runtime
    runtime();
    
    // Print statistics
    RuntimeStats* stats = get_runtime_stats();
    printf("\n=== Runtime Statistics ===\n");
    printf("Events processed: %lu\n", stats->events_processed);
    printf("Events dropped: %lu\n", stats->events_dropped);
    printf("Handler errors: %lu\n", stats->handler_errors);
    printf("Max queue depth: %u\n", stats->max_queue_depth);
    
    printf("\n[Main] AMF simulation completed\n");
    
    return 0;
}