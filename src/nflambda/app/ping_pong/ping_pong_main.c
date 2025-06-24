/*
 * Ping-Pong Demo Application
 * 
 * This application demonstrates the runtime framework by implementing
 * a simple ping-pong message exchange between two actors.
 */

#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <sys/select.h>
#include <sys/time.h>
#include "../../runtime/runtime.h"
#include "ping_actor.h"
#include "pong_actor.h"
#include "common.h"

// Event source: Triggers initial EVENT_START after a short delay
static void startup_event_source(void)
{
    static bool started = false;
    static time_t start_time = 0;
    
    if (!started) {
        if (start_time == 0) {
            start_time = time(NULL);
            return;
        }
        
        // Wait 1 second before starting
        if (time(NULL) - start_time >= 1) {
            printf("\n=== Starting ping-pong sequence ===\n\n");
            trigger_event(EVENT_START, "start");
            started = true;
        }
    }
}

// Event source: Check for user input (Enter key to stop)
static void console_event_source(void)
{
    static bool checked = false;
    
    if (!checked) {
        // Non-blocking check for input
        fd_set fds;
        struct timeval tv = {0, 0};
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        
        if (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0) {
            char buffer[10];
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                printf("\n=== User requested stop ===\n");
                trigger_event(EVENT_STOP, "user_request");
                checked = true;
            }
        }
    }
}

// Exit condition: Stop when EVENT_STOP has been processed
static bool stop_event_condition(void)
{
    return ping_is_stopped();
}

// Exit condition: Timeout after 30 seconds (safety)
static bool timeout_condition(void)
{
    static time_t start_time = 0;
    if (start_time == 0) {
        start_time = time(NULL);
    }
    return (time(NULL) - start_time) > 30;
}

// Custom error handler
static void demo_error_handler(int event_id, const char* error)
{
    fprintf(stderr, "[DEMO ERROR] Event %d: %s\n", event_id, error);
}

int main(int argc, char* argv[])
{
    printf("=================================\n");
    printf("   Ping-Pong Demo Application    \n");
    printf("=================================\n");
    printf("Demonstrating the NFLambda Runtime Framework\n\n");
    
    // Step 1: Initialize actors
    printf("1. Initializing actors...\n");
    ping_init();
    pong_init();
    
    // Step 2: Register handler registrars
    printf("2. Registering handler registrars...\n");
    register_handler_registrar(ping_register_handlers);
    register_handler_registrar(pong_register_handlers);
    
    // Step 3: Register event sources
    printf("3. Registering event sources...\n");
    register_event_source("startup", startup_event_source);
    register_event_source("console", console_event_source);
    
    // Step 4: Register exit conditions
    printf("4. Registering exit conditions...\n");
    register_exit_condition("demo_complete", stop_event_condition);
    register_exit_condition("timeout", timeout_condition);
    
    // Step 5: Set custom error handler
    printf("5. Setting custom error handler...\n");
    set_error_handler(demo_error_handler);
    
    // Step 6: Setup signal handler
    printf("6. Setting up signal handler (Ctrl+C to exit)...\n");
    signal(SIGINT, handle_sigint);
    
    // Step 7: Run the runtime
    printf("7. Starting runtime...\n");
    printf("\nPress Enter to stop the demo...\n");
    printf("---------------------------------\n");
    
    runtime();
    
    // Step 8: Print results
    printf("\n---------------------------------\n");
    RuntimeStats* stats = get_runtime_stats();
    printf("\n=== Demo Complete ===\n");
    printf("Statistics:\n");
    printf("  Events processed: %lu\n", stats->events_processed);
    printf("  Events dropped: %lu\n", stats->events_dropped);
    printf("  Handler errors: %lu\n", stats->handler_errors);
    printf("  Max queue depth: %u\n", stats->max_queue_depth);
    printf("\nThank you for trying the Ping-Pong demo!\n");
    
    return 0;
}