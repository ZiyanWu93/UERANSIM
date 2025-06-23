#include "event_pool.h"
#include <stdio.h>
#include <stdlib.h>

// Global event_nf pointer needed to satisfy external references
EventNf* event_nf_ptr = NULL;
EventNf event_nf;

int main()
{
    printf("=== Event Pool Test ===\n");
    
    // Initialize the event pool
    initialize_event_pool();
    
    // Allocate some events
    EventNf* events[10];
    printf("\n--- Allocating Events ---\n");
    for (int i = 0; i < 5; i++) {
        events[i] = allocate_event();
        if (events[i] == NULL) {
            printf("Failed to allocate event %d\n", i);
            exit(1);
        }
        
        // Set some test data
        events[i]->event_id = i;
        sprintf(events[i]->input_payload, "Test input payload %d", i);
    }
    
    // Check pool usage
    printf("\nEvent pool usage: %d\n", get_event_pool_usage());
    
    // Return some events to the pool
    printf("\n--- Returning Events ---\n");
    return_event_to_pool(events[1]);
    return_event_to_pool(events[3]);
    
    // Check pool usage
    printf("\nEvent pool usage: %d\n", get_event_pool_usage());
    
    // Allocate more events
    printf("\n--- Allocating More Events ---\n");
    events[1] = allocate_event();
    events[3] = allocate_event();
    
    // Return all events to the pool
    printf("\n--- Returning All Events ---\n");
    for (int i = 0; i < 5; i++) {
        return_event_to_pool(events[i]);
    }
    
    // Check pool usage
    printf("\nEvent pool usage: %d\n", get_event_pool_usage());
    
    // Try to return an event not from the pool
    printf("\n--- Invalid Return Test ---\n");
    EventNf invalid_event;
    return_event_to_pool(&invalid_event);
    
    printf("\n=== Test Complete ===\n");
    return 0;
}
