# Mailbox System

*Part of [Core5G](../overview.md) > Mailbox System*

## Overview

The Mailbox system provides a high-performance, thread-safe message queue for inter-actor communication in Core5G. It implements a circular buffer with integrated memory management, enabling zero-copy message passing between system components. The mailbox serves as the central communication hub for event-driven processing.

## Architecture

### Design Principles

- **Zero-Copy Operations**: Messages passed by reference, not copied
- **Bounded Queue**: Fixed-size circular buffer prevents unbounded growth
- **Integrated Memory**: Built-in allocator eliminates external dependencies
- **Cache-Friendly**: Optimized for CPU cache efficiency
- **Thread-Safe**: Lock-based synchronization for multi-threaded use

### Core Components

1. **Circular Buffer**: 32-slot ring buffer for event pointers
2. **Memory Allocator**: Integrated pool allocator for EventNf objects
3. **Synchronization**: Mutex and condition variables for thread safety
4. **Statistics**: Built-in counters for monitoring

### Data Structure

```c
typedef struct {
    EventNf** queue;              // Circular buffer of event pointers
    size_t capacity;              // Queue capacity (32)
    size_t head;                  // Write position
    size_t tail;                  // Read position
    size_t count;                 // Current number of items
    
    PoolAllocator* allocator;     // Integrated memory pool
    
    pthread_mutex_t mutex;        // Thread synchronization
    pthread_cond_t not_empty;     // Condition for consumers
    pthread_cond_t not_full;      // Condition for producers
} Mailbox;
```

## API Reference

### Initialization and Cleanup

#### `Mailbox* mailbox_create(size_t queue_size, size_t event_pool_size)`
Creates a new mailbox with specified capacities.
- **queue_size**: Number of slots in circular buffer (typically 32)
- **event_pool_size**: Number of pre-allocated EventNf objects
- **Returns**: Pointer to Mailbox or NULL on failure

Example:
```c
Mailbox* mbox = mailbox_create(32, 256);
if (!mbox) {
    log_error("Failed to create mailbox");
    return -1;
}
```

#### `void mailbox_destroy(Mailbox* mailbox)`
Destroys a mailbox and frees all resources.
- Frees any remaining events in queue
- Destroys the internal allocator
- Releases all memory

### Event Operations

#### `EventNf* mailbox_alloc(Mailbox* mailbox)`
Allocates an event from the mailbox's memory pool.
- **Returns**: Pointer to EventNf or NULL if pool exhausted
- Zero-initialized for safety
- O(1) allocation time

#### `void mailbox_free(Mailbox* mailbox, EventNf* event)`
Returns an event to the mailbox's memory pool.
- Automatically clears event data
- O(1) deallocation time

### Queue Operations

#### `bool mailbox_put(Mailbox* mailbox, EventNf* event)`
Adds an event to the mailbox queue (non-blocking).
- **Returns**: true on success, false if queue full
- Does not block if queue is full
- Thread-safe

Example:
```c
EventNf* event = mailbox_alloc(mbox);
event->event_id = EVENT_NAS_REGISTRATION_REQUEST;
strcpy(event->payload, nas_pdu);

if (!mailbox_put(mbox, event)) {
    log_warning("Mailbox full, dropping event");
    mailbox_free(mbox, event);
}
```

#### `EventNf* mailbox_get(Mailbox* mailbox)`
Removes and returns an event from the queue (blocking).
- **Returns**: Event pointer or NULL on error
- Blocks until an event is available
- Thread-safe

Example:
```c
EventNf* event = mailbox_get(mbox);
if (event) {
    process_event(event);
    mailbox_free(mbox, event);
}
```

#### `EventNf* mailbox_try_get(Mailbox* mailbox)`
Attempts to get an event without blocking.
- **Returns**: Event pointer or NULL if queue empty
- Non-blocking variant of mailbox_get
- Thread-safe

### Queue Status

#### `size_t mailbox_count(const Mailbox* mailbox)`
Returns the current number of events in the queue.
- Useful for monitoring queue depth
- Can trigger flow control decisions

#### `bool mailbox_empty(const Mailbox* mailbox)`
Checks if the mailbox queue is empty.
- **Returns**: true if empty, false otherwise

#### `bool mailbox_full(const Mailbox* mailbox)`
Checks if the mailbox queue is full.
- **Returns**: true if full, false otherwise

## Usage Patterns

### Basic Producer-Consumer

```c
// Producer thread
void* producer(void* arg) {
    Mailbox* mbox = (Mailbox*)arg;
    
    while (running) {
        // Allocate event from mailbox pool
        EventNf* event = mailbox_alloc(mbox);
        if (!event) {
            log_error("Event pool exhausted");
            continue;
        }
        
        // Populate event
        event->event_id = get_next_event_type();
        generate_event_payload(event->payload);
        
        // Send to consumer
        if (!mailbox_put(mbox, event)) {
            log_warning("Queue full, dropping event");
            mailbox_free(mbox, event);
        }
    }
    return NULL;
}

// Consumer thread
void* consumer(void* arg) {
    Mailbox* mbox = (Mailbox*)arg;
    
    while (running) {
        // Get event (blocks if empty)
        EventNf* event = mailbox_get(mbox);
        if (!event) continue;
        
        // Process event
        handle_event(event);
        
        // Return to pool
        mailbox_free(mbox, event);
    }
    return NULL;
}
```

### Non-Blocking Consumer

```c
void process_available_events(Mailbox* mbox) {
    EventNf* event;
    
    // Process all available events without blocking
    while ((event = mailbox_try_get(mbox)) != NULL) {
        handle_event(event);
        mailbox_free(mbox, event);
    }
}
```

### Flow Control

```c
void producer_with_backpressure(Mailbox* mbox) {
    EventNf* event = mailbox_alloc(mbox);
    if (!event) {
        // Memory backpressure
        apply_memory_backpressure();
        return;
    }
    
    populate_event(event);
    
    if (!mailbox_put(mbox, event)) {
        // Queue backpressure
        apply_queue_backpressure();
        mailbox_free(mbox, event);
    }
}
```

## Memory Management

### Memory Layout

```
Mailbox Memory Structure:
+-------------------+
| Mailbox struct    |
| - queue metadata  |
| - synchronization |
+-------------------+
| Circular Buffer   |
| EventNf*[32]      |
+-------------------+
| Pool Allocator    |
| - metadata        |
| - free list       |
+-------------------+
| Event Pool        |
| EventNf[256]      |
| - 2KB per event   |
+-------------------+
```

### Memory Guarantees

1. **Bounded Memory**: Fixed allocation at creation time
2. **No Dynamic Allocation**: All memory pre-allocated
3. **Zero-Copy**: Events passed by pointer reference
4. **Cache Alignment**: Structures aligned for performance

### Best Practices

1. **Right-size the pools**
   ```c
   // For high-throughput scenarios
   Mailbox* high_perf = mailbox_create(64, 512);
   
   // For memory-constrained environments
   Mailbox* low_mem = mailbox_create(16, 64);
   ```

2. **Monitor pool usage**
   ```c
   void monitor_mailbox_health(Mailbox* mbox) {
       size_t queue_depth = mailbox_count(mbox);
       size_t queue_capacity = 32; // or mbox->capacity
       
       if (queue_depth > queue_capacity * 0.8) {
           log_warning("Mailbox queue high: %zu/%zu", 
                      queue_depth, queue_capacity);
       }
   }
   ```

3. **Handle allocation failures**
   ```c
   EventNf* event = mailbox_alloc(mbox);
   if (!event) {
       // Don't panic, apply backpressure
       stats.alloc_failures++;
       return RETRY_LATER;
   }
   ```

## Thread Safety

### Synchronization Mechanisms

- **Mutex**: Protects queue operations and counters
- **Condition Variables**: Enable blocking get operations
- **Memory Barriers**: Ensure visibility across threads

### Thread-Safe Operations

All public API functions are thread-safe:
- Multiple producers can call `mailbox_put` concurrently
- Multiple consumers can call `mailbox_get` concurrently
- Producers and consumers can operate simultaneously

### Deadlock Prevention

The implementation prevents deadlocks by:
1. Using a single mutex per mailbox
2. Never holding locks during callbacks
3. Proper condition variable usage
4. Timeout mechanisms (in future versions)

## Performance Characteristics

### Benchmarks

Typical performance on modern x86_64:
- `mailbox_put`: ~100ns (uncontended)
- `mailbox_get`: ~120ns (uncontended)
- `mailbox_alloc`: ~80ns
- `mailbox_free`: ~60ns

### Optimization Strategies

1. **Minimize Lock Contention**
   - Keep critical sections small
   - Use try_get for polling scenarios
   - Consider multiple mailboxes for scaling

2. **Cache Optimization**
   - Events are cache-line aligned
   - Circular buffer fits in L1 cache
   - Hot path code is compact

3. **Batch Operations** (future enhancement)
   ```c
   // Potential batch API
   int mailbox_put_batch(Mailbox* mbox, EventNf** events, int count);
   int mailbox_get_batch(Mailbox* mbox, EventNf** events, int max_count);
   ```

## Error Handling

### Common Errors

1. **Allocation Failure**
   ```c
   EventNf* event = mailbox_alloc(mbox);
   if (!event) {
       // Pool exhausted - apply backpressure
       return ERROR_NO_MEMORY;
   }
   ```

2. **Queue Full**
   ```c
   if (!mailbox_put(mbox, event)) {
       // Queue full - handle overflow
       stats.queue_drops++;
       mailbox_free(mbox, event);
   }
   ```

3. **Invalid Parameters**
   ```c
   // All functions check for NULL pointers
   if (!mailbox || !event) {
       return ERROR_INVALID_PARAM;
   }
   ```

## Testing

### Unit Tests

Comprehensive test suite included:
```bash
make mailbox_test
./build/mailbox_test
```

Tests cover:
- Creation and destruction
- Single-threaded operations
- Multi-threaded scenarios
- Edge cases and error conditions
- Performance benchmarks

### Integration Testing

```c
void test_mailbox_integration(void) {
    Mailbox* mbox = mailbox_create(32, 256);
    
    // Start producer and consumer threads
    pthread_t producer_thread, consumer_thread;
    pthread_create(&producer_thread, NULL, producer, mbox);
    pthread_create(&consumer_thread, NULL, consumer, mbox);
    
    // Run test workload
    sleep(10);
    
    // Verify results
    assert(events_produced == events_consumed);
    
    // Cleanup
    mailbox_destroy(mbox);
}
```

## Debugging

### Debug Helpers

```c
// Dump mailbox state
void mailbox_dump_state(const Mailbox* mbox) {
    printf("Mailbox State:\n");
    printf("  Queue: %zu/%zu items\n", mbox->count, mbox->capacity);
    printf("  Head: %zu, Tail: %zu\n", mbox->head, mbox->tail);
    printf("  Pool: %zu available\n", pool_available(mbox->allocator));
}

// Trace event flow
void trace_event(const EventNf* event, const char* action) {
    printf("[%s] Event %d: %s\n", action, event->event_id, 
           event_name(event->event_id));
}
```

### Common Issues

1. **Memory Leaks**: Always free events after processing
2. **Deadlocks**: Check for proper mutex usage
3. **Queue Stalls**: Monitor for full/empty conditions
4. **Performance**: Profile lock contention

## Future Enhancements

### Planned Features

- **Priority Queues**: Multiple queues with priority levels
- **Timed Operations**: Timeouts for blocking operations
- **Batch APIs**: Process multiple events efficiently
- **Statistics API**: Detailed performance metrics
- **Dynamic Resizing**: Grow/shrink based on load
- **Lock-Free Option**: For extreme performance

### API Evolution

Future API additions under consideration:
```c
// Timed operations
EventNf* mailbox_get_timeout(Mailbox* mbox, int timeout_ms);

// Bulk operations
int mailbox_put_many(Mailbox* mbox, EventNf** events, int count);

// Statistics
mailbox_stats_t mailbox_get_stats(const Mailbox* mbox);

// Callbacks
void mailbox_set_full_callback(Mailbox* mbox, void (*cb)(void));
```

## See Also

- [Core5G Overview](../overview.md) - System architecture and design principles
- [Event System](../event_system/README.md) - EventNf structure and usage
- [Memory Management](../memory/README.md) - Pool allocator implementation details
- [Runtime Module](../runtime/README.md) - Event processing flow and integration