# Runtime Module

## Overview

The Runtime module is the central orchestrator of the Core5G system, implementing an event-driven execution environment that manages the lifecycle of network function actors, event processing, and system resources. It provides a single-threaded, cooperative multitasking framework designed for predictable performance and efficient resource utilization.

## Architecture

### Design Philosophy

The runtime follows these key principles:
- **Event-Driven**: All processing is triggered by events, ensuring reactive and efficient execution
- **Single-Threaded**: Eliminates concurrency complexities while maintaining high performance
- **Cooperative**: Long operations are broken into smaller event-driven steps
- **Deterministic**: Predictable execution order and timing behavior
- **Extensible**: Easy to add new event types and handlers

### Core Components

#### 1. Event Loop
The heart of the runtime, implementing a continuous processing cycle:
```c
while (!check_exit_conditions()) {
    listen_to_events();    // Check for external events
    process_events();      // Process all queued events
}
```

#### 2. Event Routing System
- **Routing Table**: Static array of function pointers indexed by event ID
- **O(1) Dispatch**: Direct array lookup for handler execution
- **Dynamic Registration**: Handlers can be registered at runtime

#### 3. Mailbox Integration
- Interfaces with the mailbox system for event queuing
- Manages event lifecycle from creation to disposal
- Ensures proper memory management through event pools

## API Reference

### Public Functions

#### `void runtime(void)`
Main entry point for the runtime system.
- Initializes all subsystems
- Runs the event loop
- Handles cleanup on exit

#### `void trigger_event(int event_id, const char* payload)`
Enqueues a new event for processing.
- **event_id**: Numeric identifier for the event type
- **payload**: Hex-encoded NAS PDU data
- Creates an EventNf structure and adds it to the mailbox

#### `int register_event_handler(int event_id, event_handler_t handler)`
Registers a handler function for a specific event type.
- **event_id**: Event type to handle (must be < MAX_EVENTS)
- **handler**: Function pointer to the handler
- Returns: 0 on success, -1 on error

#### `event_handler_t* get_routing_table(void)`
Returns pointer to the routing table for direct access.
- Used by modules that need bulk registration
- Allows inspection of registered handlers

#### `Mailbox* get_event_mailbox(void)`
Returns pointer to the global event mailbox.
- Enables direct mailbox operations
- Used for advanced event management

### Handler Functions

#### `void register_handlers(void)`
Called during initialization to register all event handlers.
- Currently empty - handlers register themselves
- Can be extended for static handler registration

#### `void listen_to_events(void)`
Abstraction point for external event sources.
- Monitors network interfaces
- Checks timers
- Polls user input
- Converts external data to events via `trigger_event()`

#### `void process_events(void)`
Processes all events in the mailbox queue.
- Pops events from mailbox
- Dispatches to appropriate handlers
- Returns events to memory pool

## Event Processing Flow

### 1. Event Creation
External sources create events through `listen_to_events()`:
```
[External Source] → [listen_to_events()] → [trigger_event()] → [EventNf Creation]
```

### 2. Event Queuing
Events are queued in the mailbox for processing:
```
[EventNf] → [mailbox_put()] → [Circular Buffer] → [Waiting for dispatch]
```

### 3. Event Dispatch
The runtime dispatches events to registered handlers:
```
[mailbox_pop()] → [Routing Table Lookup] → [Handler Execution] → [Event Return]
```

### 4. Handler Chaining
Handlers can trigger additional events:
```
[Handler A] → [trigger_event()] → [Handler B] → [trigger_event()] → [Handler C]
```

## Exit Conditions

The runtime supports flexible termination through user-defined exit condition functions:

### Exit Condition Function Signature
```c
bool exit_condition_function(void) {
    // Check condition
    return true;   // Request exit
    return false;  // Continue running
}
```

### Common Exit Conditions
- **Signal Handlers**: SIGINT/SIGTERM for graceful shutdown
- **Resource Limits**: Memory or event pool exhaustion
- **Error States**: Critical failures requiring termination
- **Time Limits**: Maximum runtime duration
- **User Commands**: Interactive shutdown requests

## Memory Management

### Event Lifecycle
1. **Allocation**: Events allocated from mailbox's memory pool
2. **Processing**: Events passed by reference (zero-copy)
3. **Return**: Events returned to pool after processing
4. **Reuse**: Returned events available for new allocations

### Memory Guarantees
- **Bounded Usage**: Fixed-size event pool prevents unbounded growth
- **No Dynamic Allocation**: All memory pre-allocated at startup
- **Predictable Performance**: No GC pauses or allocation delays

## Error Handling

### Handler Errors
- Handlers should not throw exceptions or crash
- Errors logged but don't stop event processing
- Failed handlers don't affect subsequent events

### Mailbox Overflow
- Full mailbox drops new events (backpressure)
- Logged for monitoring and debugging
- System continues processing existing events

### Invalid Events
- Events with unregistered IDs are ignored
- Invalid event IDs logged for debugging
- No system disruption from bad events

## Performance Characteristics

### Latency
- **Event Dispatch**: Sub-microsecond overhead
- **Handler Execution**: Depends on handler complexity
- **End-to-End**: Typically under 1ms for simple handlers

### Throughput
- **Event Rate**: Limited by slowest handler
- **Typical**: 100K+ events/second for simple handlers
- **Bottleneck**: Usually handler processing, not runtime

### Resource Usage
- **Memory**: Fixed allocation, typically < 1MB
- **CPU**: Single core, 100% during processing
- **Cache**: Optimized for L1/L2 cache efficiency

## Integration Guidelines

### Adding New Event Types
1. Define event ID in `event.h`
2. Implement handler function
3. Register handler in initialization
4. Trigger events as needed

### Handler Best Practices
- **Keep handlers short**: < 1ms execution time
- **Don't block**: No I/O or long computations
- **Use event chaining**: Break complex operations into steps
- **Handle errors gracefully**: Log and continue

### Testing Handlers
- Unit test handlers independently
- Use mock events for testing
- Verify handler registration
- Test error conditions

## Configuration

### Compile-Time Settings
- `MAX_EVENTS`: Maximum number of event types (default: 200)
- Mailbox size configured in mailbox module
- Memory pool size set during initialization

### Runtime Configuration
- Handlers registered dynamically
- Exit conditions user-defined
- Event sources pluggable

## Debugging

### Logging
- Event dispatch logged with event IDs
- Handler registration tracked
- Errors and warnings for diagnostics

### Monitoring
- `mailbox_count()`: Check queue depth
- Event processing rate measurable
- Handler execution time trackable

### Common Issues
1. **Handler not called**: Check registration and event ID
2. **Events dropped**: Mailbox full, add backpressure
3. **Performance degradation**: Profile slow handlers
4. **Memory exhaustion**: Check for event leaks

## Example Usage

### Basic Handler Registration
```c
// Define handler
void handle_registration_request(void) {
    // Process EVENT_PAYLOAD
    char* nas_pdu = EVENT_PAYLOAD;
    // Generate response
    strcpy(EVENT_PAYLOAD, "response_pdu_hex");
}

// Register during initialization
void register_my_handlers(void) {
    register_event_handler(EVENT_NAS_REGISTRATION_REQUEST, 
                          handle_registration_request);
}
```

### Triggering Events
```c
// From external source
void on_nas_message_received(const char* hex_pdu) {
    trigger_event(EVENT_NAS_REGISTRATION_REQUEST, hex_pdu);
}
```

### Custom Exit Condition
```c
bool check_timeout(void) {
    static time_t start_time = 0;
    if (start_time == 0) start_time = time(NULL);
    return (time(NULL) - start_time) > 3600; // Exit after 1 hour
}
```

## Future Enhancements

### Planned Features
- Multi-threaded event processing
- Priority-based event scheduling  
- Event filtering and routing rules
- Performance profiling integration
- Dynamic handler hot-swapping

### Optimization Opportunities
- SIMD for event matching
- Lock-free data structures
- NUMA-aware memory allocation
- Hardware event acceleration