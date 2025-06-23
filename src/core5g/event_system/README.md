# Event System

*Part of [Core5G](../overview.md) > Event System*

## Overview

The Event System is the core message processing infrastructure of Core5G, providing event definitions, memory management, and utilities for handling NAS (Non-Access Stratum) messages. It implements a type-safe, efficient event handling mechanism with pre-allocated memory pools for predictable performance.

## Architecture

### Components

1. **Event Definitions** (`event.h`)
   - Event type enumeration
   - Event structure definition
   - Event ID constants

2. **Event Pool** (`event_pool.h/c`)
   - Memory pool for EventNf allocations
   - Fixed-size allocation strategy
   - Zero-initialization guarantees

3. **Event Utilities** (`event_utils.h`)
   - Debugging and logging helpers
   - Event inspection functions
   - Payload formatting utilities

### Design Principles

- **Type Safety**: Strongly typed event IDs prevent routing errors
- **Memory Efficiency**: Pre-allocated pools eliminate allocation overhead
- **Zero-Copy**: Events passed by reference through the system
- **Simplicity**: Minimal API surface for easy integration

## Event Structure

### EventNf Definition

```c
#define MAX_NAS_HEX_LEN 1000    // Maximum hex string length

typedef struct EventNf {
    int event_id;                          // Event type identifier
    char input_payload[MAX_NAS_HEX_LEN];   // Event payload (hex-encoded NAS PDU)
    int input_payload_length;              // Actual binary length of input_payload
} EventNf;
```

The structure fields:
- **event_id**: Identifies the event type (e.g., EVENT_NAS_REGISTRATION_REQUEST)
- **input_payload**: Contains hex-encoded NAS PDU or application-specific data
- **input_payload_length**: Tracks the actual binary length of the payload

Note: The payload is used for both input and output - handlers modify it in-place.

### Event Types

```c
// Registration events
#define EVENT_NAS_REGISTRATION_REQUEST      1
#define EVENT_NAS_REGISTRATION_ACCEPT       2
#define EVENT_NAS_REGISTRATION_COMPLETE     3
#define EVENT_NAS_REGISTRATION_REJECT       4

// Authentication events  
#define EVENT_NAS_AUTHENTICATION_REQUEST    5
#define EVENT_NAS_AUTHENTICATION_RESPONSE   6
#define EVENT_NAS_AUTHENTICATION_FAILURE    7
#define EVENT_NAS_AUTHENTICATION_REJECT     8

// Security mode events
#define EVENT_NAS_SECURITY_MODE_COMMAND     9
#define EVENT_NAS_SECURITY_MODE_COMPLETE   10
#define EVENT_NAS_SECURITY_MODE_REJECT     11

// Session management events
#define EVENT_NAS_PDU_SESSION_ESTABLISHMENT_REQUEST  12
#define EVENT_NAS_PDU_SESSION_ESTABLISHMENT_ACCEPT   13
#define EVENT_NAS_PDU_SESSION_ESTABLISHMENT_REJECT   14
#define EVENT_NAS_PDU_SESSION_MODIFICATION_REQUEST   15
#define EVENT_NAS_PDU_SESSION_MODIFICATION_COMPLETE  16
#define EVENT_NAS_PDU_SESSION_MODIFICATION_REJECT    17
#define EVENT_NAS_PDU_SESSION_RELEASE_REQUEST        18
#define EVENT_NAS_PDU_SESSION_RELEASE_COMPLETE       19

// Configuration events
#define EVENT_NAS_CONFIGURATION_UPDATE_COMMAND    20
#define EVENT_NAS_CONFIGURATION_UPDATE_COMPLETE   21

// Internal events
#define EVENT_CONTEXT_SETUP_REQUEST     50
#define EVENT_CONTEXT_RELEASE_REQUEST   51
#define EVENT_BEARER_SETUP_REQUEST      52
#define EVENT_BEARER_MODIFY_REQUEST     53
#define EVENT_BEARER_RELEASE_REQUEST    54
```

## API Reference

### Event Pool Management

#### `void event_pool_init(void)`
Initializes the global event pool.
- Must be called before any event operations
- Allocates memory for EVENT_POOL_SIZE events
- Zero-initializes all event structures

#### `EventNf* event_pool_allocate(void)`
Allocates an event from the pool.
- Returns: Pointer to EventNf or NULL if pool exhausted
- Guaranteed zero-initialized
- O(1) allocation time

#### `void event_pool_free(EventNf* event)`
Returns an event to the pool.
- Parameters: event - The event to free
- Clears event data before returning to pool
- O(1) deallocation time

#### `size_t event_pool_available(void)`
Returns the number of available events in the pool.
- Useful for monitoring pool usage
- Can trigger backpressure mechanisms

#### `void event_pool_destroy(void)`
Destroys the event pool and frees resources.
- Called during system shutdown
- Ensures clean resource deallocation

### Event Utilities

#### `const char* event_name(int event_id)`
Returns human-readable name for an event ID.
```c
printf("Processing event: %s\n", event_name(EVENT_NAS_REGISTRATION_REQUEST));
// Output: "Processing event: NAS_REGISTRATION_REQUEST"
```

#### `void event_dump(const EventNf* event)`
Dumps event contents for debugging.
```c
EventNf* evt = event_pool_allocate();
evt->event_id = EVENT_NAS_AUTHENTICATION_REQUEST;
strcpy(evt->payload, "7e0056020000");
event_dump(evt);
// Output:
// Event ID: 5 (NAS_AUTHENTICATION_REQUEST)
// Payload (6 bytes): 7e0056020000
```

#### `void print_nas_pdu(const char* hex_pdu)`
Pretty-prints a NAS PDU in hex format.
```c
print_nas_pdu("7e004179000d01");
// Output: NAS PDU: 7e 00 41 79 00 0d 01
```

## Usage Patterns

### Basic Event Creation

```c
// Allocate event
EventNf* event = event_pool_allocate();
if (!event) {
    log_error("Event pool exhausted");
    return;
}

// Set event type
event->event_id = EVENT_NAS_REGISTRATION_REQUEST;

// Set payload (hex-encoded NAS PDU)
strcpy(event->payload, "7e004179000d0101f11111111100000001");

// Use event (e.g., put in mailbox)
mailbox_put(event_mailbox, event);
```

### Event Handler Access

Event handlers access the current event through the EVENT_PAYLOAD macro:
```c
void handle_registration_request(void) {
    // Input is in EVENT_PAYLOAD
    char* nas_pdu = EVENT_PAYLOAD;
    
    // Process the NAS PDU
    process_registration(nas_pdu);
    
    // Write response back to EVENT_PAYLOAD
    generate_registration_accept(EVENT_PAYLOAD);
}
```

### Event Chaining

Handlers can trigger follow-up events:
```c
void handle_registration_request(void) {
    // Process registration
    registration_context_t ctx;
    parse_registration_request(EVENT_PAYLOAD, &ctx);
    
    // Need authentication first
    if (needs_authentication(&ctx)) {
        // Trigger authentication flow
        trigger_event(EVENT_REQUEST_AUTHENTICATION, ctx.supi);
        return;
    }
    
    // Direct accept
    generate_registration_accept(EVENT_PAYLOAD);
}
```

## Memory Management

### Pool Configuration

```c
#define EVENT_POOL_SIZE 1024  // Number of pre-allocated events
```

Adjust based on:
- Expected concurrent events
- System memory constraints
- Performance requirements

### Memory Layout

```
Event Pool Memory Layout:
+----------------+
| Event Pool     |
| Metadata       |
+----------------+
| EventNf[0]     |
|   event_id     |
|   payload[2048]|
+----------------+
| EventNf[1]     |
|   event_id     |
|   payload[2048]|
+----------------+
| ...            |
+----------------+
| EventNf[1023]  |
|   event_id     |
|   payload[2048]|
+----------------+
| Free List      |
| (Stack-based)  |
+----------------+
```

### Best Practices

1. **Always check allocation success**
   ```c
   EventNf* event = event_pool_allocate();
   if (!event) {
       // Handle pool exhaustion
       return ERROR_NO_RESOURCES;
   }
   ```

2. **Free events promptly**
   ```c
   // After processing
   event_pool_free(event);
   ```

3. **Monitor pool usage**
   ```c
   if (event_pool_available() < EVENT_POOL_SIZE / 10) {
       log_warning("Event pool running low: %zu available", 
                   event_pool_available());
   }
   ```

## Error Handling

### Pool Exhaustion

When the event pool is exhausted:
1. `event_pool_allocate()` returns NULL
2. Caller must handle gracefully
3. Consider implementing backpressure
4. Log for monitoring/alerting

### Invalid Event IDs

Unknown event IDs are logged but don't crash the system:
```c
if (event->event_id < 0 || event->event_id >= MAX_EVENTS) {
    log_error("Invalid event ID: %d", event->event_id);
    event_pool_free(event);
    return;
}
```

## Testing

### Unit Tests

The event system includes comprehensive unit tests:

```bash
# Build and run tests
make event_system_test
./build/event_system_test
```

Test coverage includes:
- Pool initialization/destruction
- Allocation/deallocation cycles
- Pool exhaustion handling
- Event utilities
- Thread safety (if applicable)

### Integration Testing

Test event flow through the system:
```c
void test_event_flow(void) {
    // Initialize
    event_pool_init();
    
    // Create event
    EventNf* event = event_pool_allocate();
    event->event_id = EVENT_NAS_REGISTRATION_REQUEST;
    strcpy(event->payload, test_nas_pdu);
    
    // Process through system
    mailbox_put(event_mailbox, event);
    process_events();
    
    // Verify handling
    assert(registration_processed);
    
    // Cleanup
    event_pool_destroy();
}
```

## Performance Considerations

### Benchmarks

Typical performance metrics:
- Allocation: ~50ns per event
- Deallocation: ~30ns per event  
- Pool initialization: ~1ms for 1024 events
- Memory usage: ~2MB per 1024 events

### Optimization Tips

1. **Payload Size**: Consider reducing payload size if not fully utilized
2. **Pool Size**: Right-size based on concurrent event count
3. **CPU Cache**: Events are cache-aligned for performance
4. **Zero-Copy**: Pass events by reference, never copy payload

## Future Enhancements

### Planned Features
- Dynamic pool resizing
- Event priority levels
- Event payload compression
- Telemetry and metrics
- Event replay for testing
- Payload type safety

### Potential Optimizations
- NUMA-aware allocation
- Lock-free pool implementation
- Event batching
- Hardware acceleration for NAS parsing

## See Also

- [Core5G Overview](../overview.md) - High-level architecture and design principles
- [Runtime Module](../runtime/README.md) - Event processing and dispatch mechanisms
- [Actor Framework](../actor/README.md) - Network function implementations using events
- [Memory Management](../memory/README.md) - Understanding pool allocation strategies