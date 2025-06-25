#include "event_pool.h"
#include "../memory/memory_utils.h"
#include <stdio.h>
#include <string.h>

// Global event pool allocator
static memory_allocator_t* event_allocator = NULL;
static int allocated_count = 0;

void initialize_event_pool(void)
{
    // Get an allocator for EventNf objects from the memory system
    event_allocator = memory_get_allocator(sizeof(EventNf), EVENT_POOL_SIZE);
    
    if (event_allocator == NULL) {
        printf("ERROR: Failed to get memory allocator for event pool\n");
        return;
    }
    
    allocated_count = 0;
    printf("Event pool initialized with %d slots using memory system\n", EVENT_POOL_SIZE);
}

EventNf* allocate_event(void)
{
    if (event_allocator == NULL) {
        printf("ERROR: Event pool not initialized\n");
        return NULL;
    }
    
    // Allocate from the memory pool
    EventNf* event = (EventNf*)event_allocator->allocate(event_allocator->impl);
    
    if (event == NULL) {
        printf("ERROR: Event pool is full! Cannot allocate new event.\n");
        return NULL;
    }
    
    // Clear the event structure
    event->event_id = -1;
    memset(event->input_payload, 0, MAX_EVENT_PAYLOAD_SIZE);
    event->input_payload_length = 0;
    
    allocated_count++;
    printf("Allocated event from pool, %d/%d in use\n", 
           allocated_count, EVENT_POOL_SIZE);
    
    return event;
}

void return_event_to_pool(EventNf* event)
{
    if (event_allocator == NULL || event == NULL) {
        printf("WARNING: Invalid event pool or event pointer\n");
        return;
    }
    
    // Check if this event belongs to our pool
    if (!event_allocator->owns(event_allocator->impl, event)) {
        printf("WARNING: Attempted to return an event not from the pool\n");
        return;
    }
    
    // Return to the memory pool
    event_allocator->deallocate(event_allocator->impl, event);
    
    allocated_count--;
    printf("Returned event to pool, %d/%d in use\n", 
           allocated_count, EVENT_POOL_SIZE);
}

int get_event_pool_usage(void)
{
    return allocated_count;
}