#ifndef MAILBOX_H
#define MAILBOX_H

#include "../event_system/event.h"
#include "../memory/allocator.h"

// Define mailbox capacity - adjust as needed
#define MAILBOX_MAX_SIZE 32

// Mailbox structure - circular buffer implementation with memory management
typedef struct {
    EventNf* events[MAILBOX_MAX_SIZE];  // Array of pointers to EventNf
    int head;                           // Index for dequeuing events
    int tail;                           // Index for enqueuing events
    int count;                          // Number of events currently in mailbox
    memory_allocator_t* allocator;      // Memory allocator for EventNf objects
} Mailbox;

// Initialize a mailbox with a dedicated memory pool for EventNf objects
void mailbox_init(Mailbox* mailbox);

// Check if mailbox is empty
int mailbox_is_empty(Mailbox* mailbox);

// Check if mailbox is full
int mailbox_is_full(Mailbox* mailbox);

// Add an event to the mailbox, allocates from internal pool
// The event data is copied into a newly allocated object
// Returns 1 if successful, 0 if mailbox is full
int mailbox_put(Mailbox* mailbox, const EventNf* event_data);

// Get an event pointer from the mailbox
// Returns pointer to the event or NULL if mailbox is empty
// The caller is responsible for returning the event to the pool using mailbox_return_event
EventNf* mailbox_pop(Mailbox* mailbox);

// Return an event to the mailbox's memory pool
// Returns 1 if successful, 0 if the event doesn't belong to this mailbox's pool
int mailbox_return_event(Mailbox* mailbox, EventNf* event);

// Get number of events in the mailbox
int mailbox_count(Mailbox* mailbox);

// Clean up mailbox resources (memory pool)
void mailbox_cleanup(Mailbox* mailbox);

#endif // MAILBOX_H
