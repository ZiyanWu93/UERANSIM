# Mailbox System

Thread-safe message queue for inter-actor communication with integrated memory management.

## Overview

The Mailbox provides:
- **Circular buffer** with 32-slot capacity
- **Zero-copy** message passing
- **Integrated memory** pool for events
- **Thread-safe** operations

## Architecture

```
┌─────────────┐
│  Producer   │──put()──┐
└─────────────┘         ▼
                  ┌─────────────┐
                  │   Queue     │ (32 slots)
                  │  ┌─┬─┬─┬─┐  │
                  │  │ │ │ │ │  │
                  │  └─┴─┴─┴─┘  │
                  └─────────────┘
┌─────────────┐         ▲
│  Consumer   │──pop()──┘
└─────────────┘
```

## Key APIs

### Lifecycle
```c
Mailbox* mailbox_create(size_t queue_size, size_t event_pool_size)
void mailbox_destroy(Mailbox* mailbox)
```

### Event Management
```c
EventNf* mailbox_alloc(Mailbox* mailbox)   // Allocate from pool
void mailbox_free(Mailbox* mailbox, EventNf* event)
```

### Queue Operations
```c
bool mailbox_put(Mailbox* mailbox, EventNf* event)  // Non-blocking
EventNf* mailbox_pop(Mailbox* mailbox)              // Blocking
size_t mailbox_count(Mailbox* mailbox)              // Current size
```

## Usage Pattern

```c
// Create mailbox
Mailbox* mbox = mailbox_create(32, 256);

// Producer
EventNf* event = mailbox_alloc(mbox);
event->event_id = MY_EVENT;
strcpy(event->input_payload, data);

if (!mailbox_put(mbox, event)) {
    // Handle full queue
    mailbox_free(mbox, event);
}

// Consumer
EventNf* evt = mailbox_pop(mbox);  // Blocks if empty
process_event(evt);
mailbox_free(mbox, evt);
```

## Thread Safety

- All operations are mutex-protected
- `pop()` blocks on empty queue
- `put()` returns false on full queue (non-blocking)

## Performance

- **Enqueue/Dequeue**: O(1) operations
- **Memory**: Pre-allocated, no dynamic allocation
- **Contention**: Minimal with single producer/consumer

## Configuration

Default settings:
- Queue capacity: 32 events
- Integrated pool size: Specified at creation

## Examples

Used throughout NFLambda:
- [Runtime](../runtime/runtime.c) - Main event queue
- [Tests](mailbox_test.c) - Unit tests

## API Details

For complete documentation, see [API Reference](../docs/api-reference.md).