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
    const uint8_t* msg_data = (const uint8_t*)EVENT_PAYLOAD;
    int msg_len = event_nf_ptr->input_payload_length;
    
    printf("IPC Echo: Received message (%d bytes)\n", msg_len);
    
    // For text messages sent by ipc_client_demo, extract and print the actual message
    if (msg_len >= sizeof(uint32_t)) {
        // Skip the IPC message length header (first 4 bytes)
        uint32_t payload_len;
        memcpy(&payload_len, msg_data, sizeof(payload_len));
        
        if (payload_len > 0 && payload_len <= msg_len - sizeof(uint32_t)) {
            // Create null-terminated string for display
            char display_msg[256];
            int copy_len = payload_len < sizeof(display_msg) - 1 ? payload_len : sizeof(display_msg) - 1;
            memcpy(display_msg, msg_data + sizeof(uint32_t), copy_len);
            display_msg[copy_len] = '\0';
            
            printf("IPC Echo: Message content: '%s'\n", display_msg);
        }
    }
    
    // Trigger response event instead of directly sending
    printf("IPC Echo: Triggering response event\n");
    
    // For echo, we need to echo back the full IPC message including length header
    trigger_event(EVENT_IPC_SEND_RESPONSE, msg_data, msg_len);
}

// Register IPC event handlers
static void register_ipc_handlers(void)
{
    printf("Registering IPC event handlers...\n");
    register_event_handler(EVENT_IPC_MESSAGE_RECEIVED, handle_ipc_message);
    
    // Also register IPC internal handlers for response handling
    ipc_register_handlers();
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