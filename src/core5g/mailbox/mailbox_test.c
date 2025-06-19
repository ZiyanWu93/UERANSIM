#include "mailbox.h"
#include "../memory/memory_utils.h"
#include <stdio.h>
#include <assert.h>

// Test function to verify mailbox functionality with memory management
int main() {
    printf("Starting mailbox test with memory management integration...\n");
    
    // Initialize the memory system
    bool memory_init_success = memory_system_init();
    assert(memory_init_success && "Memory system initialization failed");
    printf("Memory system initialized successfully\n");
    
    // Create and initialize a mailbox
    Mailbox test_mailbox;
    mailbox_init(&test_mailbox);
    printf("Mailbox initialized with dedicated memory pool\n");
    
    // Test mailbox operations
    printf("Testing mailbox operations...\n");
    
    // Create test events
    EventNf event1 = { .event_id = 1, .input_payload = "Event 1 payload" };
    EventNf event2 = { .event_id = 2, .input_payload = "Event 2 payload" };
    EventNf event3 = { .event_id = 3, .input_payload = "Event 3 payload" };
    
    // Add events to mailbox
    int put_result1 = mailbox_put(&test_mailbox, &event1);
    int put_result2 = mailbox_put(&test_mailbox, &event2);
    int put_result3 = mailbox_put(&test_mailbox, &event3);
    
    assert(put_result1 && "Failed to add event1");
    assert(put_result2 && "Failed to add event2");
    assert(put_result3 && "Failed to add event3");
    
    printf("Added 3 events to the mailbox\n");
    printf("Current mailbox count: %d\n", mailbox_count(&test_mailbox));
    assert(mailbox_count(&test_mailbox) == 3 && "Mailbox count should be 3");
    
    // Pop events and verify content
    EventNf* popped_event1 = mailbox_pop(&test_mailbox);
    assert(popped_event1 && "Failed to pop event1");
    assert(popped_event1->event_id == 1 && "Incorrect event ID for event1");
    printf("Popped event1 with ID: %d and payload: %s\n", 
           popped_event1->event_id, popped_event1->input_payload);
    
    // Return event to the pool
    int return_result = mailbox_return_event(&test_mailbox, popped_event1);
    assert(return_result && "Failed to return event to pool");
    printf("Returned event1 to memory pool\n");
    
    // Pop more events
    EventNf* popped_event2 = mailbox_pop(&test_mailbox);
    EventNf* popped_event3 = mailbox_pop(&test_mailbox);
    
    assert(popped_event2 && popped_event2->event_id == 2 && "Event2 error");
    assert(popped_event3 && popped_event3->event_id == 3 && "Event3 error");
    
    printf("Popped event2 with ID: %d\n", popped_event2->event_id);
    printf("Popped event3 with ID: %d\n", popped_event3->event_id);
    printf("Current mailbox count: %d\n", mailbox_count(&test_mailbox));
    
    // Return remaining events to pool
    mailbox_return_event(&test_mailbox, popped_event2);
    mailbox_return_event(&test_mailbox, popped_event3);
    
    // Test cleanup
    mailbox_cleanup(&test_mailbox);
    printf("Mailbox cleaned up successfully\n");
    
    // Clean up memory system
    memory_system_cleanup();
    printf("Memory system cleaned up\n");
    
    printf("All mailbox tests passed successfully!\n");
    return 0;
}
