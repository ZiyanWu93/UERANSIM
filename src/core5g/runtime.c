#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "event.h"

volatile sig_atomic_t keep_running = 1;

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
#define MAX_EVENTS 10
event_handler_t routing_table[MAX_EVENTS] = {0};

void register_handlers()
{
    routing_table[0] = handle_event_0;
    routing_table[1] = handle_event_1;
}


void listen_to_events()
{
    printf("Listening to events...\n");
}

void exit_condition()
{
    printf("Exit condition met, exiting...\n");
}

void process_events()
{
    printf("Processing events...\n");
}


void runtime()
{
    register_handlers();  // Set up routing table
    
    while (keep_running)
    {
        exit_condition();
        listen_to_events();
        process_events();
    }
    printf("Exiting runtime gracefully...\n");
}

int main()
{
    signal(SIGINT, handle_sigint);
    runtime();
    return 0;
}
