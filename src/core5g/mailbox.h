#ifndef MAILBOX_H
#define MAILBOX_H

#include "event.h"

// Define mailbox capacity - adjust as needed
#define MAILBOX_MAX_SIZE 32

// Mailbox structure - circular buffer implementation
typedef struct {
    EventNf* events[MAILBOX_MAX_SIZE];  // Array of pointers to EventNf
    int head;                           // Index for dequeuing events
    int tail;                           // Index for enqueuing events
    int count;                          // Number of events currently in mailbox
} Mailbox;

// Initialize a mailbox
void mailbox_init(Mailbox* mailbox);

// Check if mailbox is empty
int mailbox_is_empty(Mailbox* mailbox);

// Check if mailbox is full
int mailbox_is_full(Mailbox* mailbox);

// Add an event pointer to the mailbox, returns 1 if successful, 0 if mailbox is full
int mailbox_put(Mailbox* mailbox, EventNf* event);

// Get an event pointer from the mailbox, returns 1 if successful, 0 if mailbox is empty
// The caller is responsible for the memory management of the returned pointer
EventNf* mailbox_pop(Mailbox* mailbox);

// Get number of events in the mailbox
int mailbox_count(Mailbox* mailbox);

#endif // MAILBOX_H
