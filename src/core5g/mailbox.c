#include "mailbox.h"
#include <string.h> // For memcpy
#include <stdio.h> // For NULL

// Initialize a mailbox
void mailbox_init(Mailbox* mailbox) {
    if (mailbox) {
        mailbox->head = 0;
        mailbox->tail = 0;
        mailbox->count = 0;
        
        // Initialize all pointers to NULL
        for (int i = 0; i < MAILBOX_MAX_SIZE; i++) {
            mailbox->events[i] = NULL;
        }
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

// Add an event pointer to the mailbox, returns 1 if successful, 0 if mailbox is full
int mailbox_put(Mailbox* mailbox, EventNf* event) {
    if (!mailbox || !event || mailbox_is_full(mailbox)) {
        return 0;  // Fail if mailbox is NULL, event is NULL, or mailbox is full
    }

    // Store the event pointer at the tail position
    mailbox->events[mailbox->tail] = event;
    
    // Update tail and count
    mailbox->tail = (mailbox->tail + 1) % MAILBOX_MAX_SIZE;
    mailbox->count++;
    
    return 1;  // Success
}

// Get an event pointer from the mailbox, returns the event pointer or NULL if mailbox is empty
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
    
    return event;  // Return the event pointer
}

// Get number of events in the mailbox
int mailbox_count(Mailbox* mailbox) {
    return mailbox ? mailbox->count : 0;
}
