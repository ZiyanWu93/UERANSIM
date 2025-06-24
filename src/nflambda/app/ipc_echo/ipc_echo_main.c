/*
 * IPC Echo Demo Application
 * 
 * This application demonstrates the IPC event source by echoing
 * messages received from external clients back to them.
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"
#include "../../event_system/ipc_event_source.h"

// Handler for IPC messages
EVENT_HANDLER(handle_ipc_message)
{
    const char* message = EVENT_PAYLOAD;
    int message_len = strlen(message);
    
    printf("IPC Echo: Received message: '%s' (%d bytes)\n", message, message_len);
    
    // Echo the message back
    if (ipc_send_response(message, message_len) < 0) {
        fprintf(stderr, "IPC Echo: Failed to send response\n");
    } else {
        printf("IPC Echo: Sent response\n");
    }
}

// Register IPC event handlers
static void register_ipc_handlers(void)
{
    printf("Registering IPC event handlers...\n");
    register_event_handler(EVENT_IPC_MESSAGE_RECEIVED, handle_ipc_message);
}

int main(int argc, char* argv[])
{
    printf("=====================================\n");
    printf("     IPC Echo Demo Application       \n");
    printf("=====================================\n");
    printf("Demonstrates IPC event source in NFLambda\n\n");
    
    // Step 1: Initialize IPC event source
    printf("1. Initializing IPC event source...\n");
    if (ipc_event_source_init(NULL) < 0) {
        fprintf(stderr, "Failed to initialize IPC event source\n");
        return 1;
    }
    
    // Step 2: Register IPC event source with runtime
    printf("2. Registering IPC event source...\n");
    register_event_source("ipc", ipc_event_source_poll);
    
    // Step 3: Register handler registrar
    printf("3. Registering handler registrar...\n");
    register_handler_registrar(register_ipc_handlers);
    
    // Step 4: Setup signal handler
    printf("4. Setting up signal handler (Ctrl+C to exit)...\n");
    signal(SIGINT, handle_sigint);
    
    // Step 5: Run the runtime
    printf("5. Starting runtime...\n");
    printf("\nWaiting for IPC connections on %s\n", IPC_DEFAULT_SOCKET_PATH);
    printf("Press Ctrl+C to exit\n");
    printf("---------------------------------\n\n");
    
    runtime();
    
    // Step 6: Cleanup
    printf("\n---------------------------------\n");
    printf("Cleaning up...\n");
    ipc_event_source_cleanup();
    
    RuntimeStats* stats = get_runtime_stats();
    printf("\n=== Demo Complete ===\n");
    printf("Statistics:\n");
    printf("  Events processed: %lu\n", stats->events_processed);
    printf("  Events dropped: %lu\n", stats->events_dropped);
    printf("  Handler errors: %lu\n", stats->handler_errors);
    
    return 0;
}