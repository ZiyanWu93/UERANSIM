/*
 * NFLambda 5G Core Application
 * 
 * This application implements a simplified 5G Core (AMF) that communicates
 * with UERANSIM via IPC, following the event-driven actor model.
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "core_5g_actor.h"
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"
#include "../../event_system/ipc_event_source.h"

/* Application name and version */
#define APP_NAME "NFLambda 5G Core"
#define APP_VERSION "1.0.0"

/* IPC socket path for 5G Core */
#define CORE_5G_IPC_SOCKET "/tmp/nflambda_5gcore.sock"

/* Register all event handlers */
static void register_all_handlers(void)
{
    printf("Registering all event handlers...\n");
    
    /* Register Core 5G handlers (includes AMF and IPC handlers) */
    core_5g_register_handlers();
}

/* Exit condition check */
static bool check_exit_condition(void)
{
    return core_5g_is_stopped();
}

int main(int argc, char* argv[])
{
    printf("=====================================\n");
    printf("     %s v%s\n", APP_NAME, APP_VERSION);
    printf("=====================================\n");
    printf("Event-driven 5G Core AMF with IPC support\n\n");
    
    /* Step 1: Initialize Core 5G actor */
    printf("1. Initializing Core 5G actor...\n");
    core_5g_init();
    
    /* Step 2: Initialize IPC event source */
    printf("2. Initializing IPC event source...\n");
    if (ipc_event_source_init(CORE_5G_IPC_SOCKET) < 0) {
        fprintf(stderr, "Failed to initialize IPC event source\n");
        return 1;
    }
    
    /* Step 3: Register IPC event source with runtime */
    printf("3. Registering IPC event source...\n");
    register_event_source("ipc", ipc_event_source_poll);
    
    /* Step 4: Register handler registrar */
    printf("4. Registering handler registrar...\n");
    register_handler_registrar(register_all_handlers);
    
    /* Step 5: Register exit condition */
    printf("5. Registering exit condition...\n");
    register_exit_condition("core_5g_stopped", check_exit_condition);
    
    /* Step 6: Setup signal handler */
    printf("6. Setting up signal handler (Ctrl+C to exit)...\n");
    signal(SIGINT, handle_sigint);
    
    /* Step 7: Run the runtime */
    printf("7. Starting runtime...\n");
    printf("\nWaiting for NAS messages on %s\n", CORE_5G_IPC_SOCKET);
    printf("Press Ctrl+C to exit\n");
    printf("---------------------------------\n\n");
    
    runtime();
    
    /* Step 8: Cleanup */
    printf("\n---------------------------------\n");
    printf("Cleaning up...\n");
    ipc_event_source_cleanup();
    
    RuntimeStats* stats = get_runtime_stats();
    printf("\n=== %s Statistics ===\n", APP_NAME);
    printf("Events processed: %lu\n", stats->events_processed);
    printf("Events dropped: %lu\n", stats->events_dropped);
    printf("Handler errors: %lu\n", stats->handler_errors);
    
    return 0;
}