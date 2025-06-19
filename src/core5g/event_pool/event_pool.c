#include "event_pool.h"
#include <stdio.h>
#include <string.h>

// Global event pool
static EventPool event_pool = {0};

void initialize_event_pool(void)
{
    memset(&event_pool, 0, sizeof(EventPool));
    printf("Event pool initialized with %d slots\n", EVENT_POOL_SIZE);
}

EventNf* allocate_event(void)
{
    // Find first available event in the pool
    for (int i = 0; i < EVENT_POOL_SIZE; i++) {
        if (!event_pool.used[i]) {
            event_pool.used[i] = true;
            event_pool.count++;
            // Clear the event structure
            event_pool.events[i].event_id = -1;
            memset(event_pool.events[i].input_payload, 0, MAX_NAS_HEX_LEN);
            memset(event_pool.events[i].output_payload, 0, MAX_NAS_HEX_LEN);
            printf("Allocated event %d from pool, %d/%d in use\n", 
                   i, event_pool.count, EVENT_POOL_SIZE);
            return &event_pool.events[i];
        }
    }
    
    // No available events in the pool
    printf("ERROR: Event pool is full! Cannot allocate new event.\n");
    return NULL;
}

void return_event_to_pool(EventNf* event)
{
    // Find the event in the pool by its address
    for (int i = 0; i < EVENT_POOL_SIZE; i++) {
        if (&event_pool.events[i] == event) {
            if (event_pool.used[i]) {
                event_pool.used[i] = false;
                event_pool.count--;
                printf("Returned event %d to pool, %d/%d in use\n", 
                       i, event_pool.count, EVENT_POOL_SIZE);
                return;
            }
        }
    }
    
    printf("WARNING: Attempted to return an event not from the pool\n");
}

int get_event_pool_usage(void)
{
    return event_pool.count;
}
