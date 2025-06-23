#ifndef EVENT_POOL_H
#define EVENT_POOL_H

#include "event.h"
#include <stdbool.h>

// Event pool configuration
#define EVENT_POOL_SIZE 32

// Initialize the event pool
void initialize_event_pool(void);

// Allocate an event from the pool
EventNf* allocate_event(void);

// Return an event to the pool
void return_event_to_pool(EventNf* event);

// Get the number of events currently in use
int get_event_pool_usage(void);

#endif // EVENT_POOL_H
