#include "mailbox.h"
#include "../memory/memory_utils.h"
#include <string.h> // For memcpy
#include <stdio.h> // For NULL

// Initialize a mailbox with memory pool
void mailbox_init(Mailbox* mailbox) {
    if (mailbox) {
        mailbox->head = 0;
        mailbox->tail = 0;
        mailbox->count = 0;
        
        // Initialize all pointers to NULL
        for (int i = 0; i < MAILBOX_MAX_SIZE; i++) {
            mailbox->events[i] = NULL;
        }
        
        // Create dedicated memory pool for EventNf objects with capacity matching MAILBOX_MAX_SIZE
        mailbox->allocator = memory_get_allocator(sizeof(EventNf), MAILBOX_MAX_SIZE);
    }
}

// Check if mailbox is empty
int mailbox_is_empty(Mailbox* mailbox) {
    return mailbox ? (mailbox->count == 0) : 1;
}

// Check if mailbox is full
int mailbox_is_full(Mailbox* mailbox) {
    return mailbox ? (mailbox->count == MAILBOX_MAX_SIZE) : 1;
}

// Add an event to the mailbox, allocates from internal pool
// The event data is copied into a newly allocated object
// Returns 1 if successful, 0 if mailbox is full
int mailbox_put(Mailbox* mailbox, const EventNf* event_data) {
    if (!mailbox || !event_data || mailbox_is_full(mailbox) || !mailbox->allocator) {
        return 0;  // Fail if mailbox is NULL, event_data is NULL, mailbox is full, or no allocator
    }

    // Allocate a new EventNf object from the memory pool
    EventNf* new_event = (EventNf*)mailbox->allocator->allocate(mailbox->allocator->impl);
    if (!new_event) {
        return 0;  // Memory allocation failed
    }
    
    // Copy event data to the newly allocated object
    memcpy(new_event, event_data, sizeof(EventNf));
    
    // Store the event pointer at the tail position
    mailbox->events[mailbox->tail] = new_event;
    
    // Update tail and count
    mailbox->tail = (mailbox->tail + 1) % MAILBOX_MAX_SIZE;
    mailbox->count++;
    
    return 1;  // Success
}

// Get an event pointer from the mailbox, returns the event pointer or NULL if mailbox is empty
// The caller is responsible for returning the event to the pool using mailbox_return_event
EventNf* mailbox_pop(Mailbox* mailbox) {
    if (!mailbox || mailbox_is_empty(mailbox)) {
        return NULL;  // Fail if mailbox is NULL or mailbox is empty
    }

    // Get the event pointer from the head position
    EventNf* event = mailbox->events[mailbox->head];
    
    // Clear the pointer in the mailbox
    mailbox->events[mailbox->head] = NULL;
    
    // Update head and count
    mailbox->head = (mailbox->head + 1) % MAILBOX_MAX_SIZE;
    mailbox->count--;
    
    return event;  // Return the event pointer with ownership transferred to caller
}

// Return an event to the mailbox's memory pool
// Returns 1 if successful, 0 if the event doesn't belong to this mailbox's pool
int mailbox_return_event(Mailbox* mailbox, EventNf* event) {
    if (!mailbox || !event || !mailbox->allocator) {
        return 0;  // Invalid parameters
    }
    
    // Check if the event belongs to this mailbox's pool
    if (!mailbox->allocator->owns(mailbox->allocator->impl, event)) {
        return 0;  // Event doesn't belong to this mailbox's pool
    }
    
    // Return the event to the memory pool
    mailbox->allocator->deallocate(mailbox->allocator->impl, event);
    return 1;  // Success
}

// Clean up mailbox resources (memory pool)
void mailbox_cleanup(Mailbox* mailbox) {
    if (!mailbox) {
        return;
    }
    
    // Release all events still in the mailbox back to the pool
    for (int i = 0; i < MAILBOX_MAX_SIZE; i++) {
        if (mailbox->events[i]) {
            mailbox->allocator->deallocate(mailbox->allocator->impl, mailbox->events[i]);
            mailbox->events[i] = NULL;
        }
    }
    
    // Reset mailbox state
    mailbox->head = 0;
    mailbox->tail = 0;
    mailbox->count = 0;
    
    // We don't free the allocator itself as it's managed by the memory system
    mailbox->allocator = NULL;
}

// Get number of events in the mailbox
int mailbox_count(Mailbox* mailbox) {
    return mailbox ? mailbox->count : 0;
}
