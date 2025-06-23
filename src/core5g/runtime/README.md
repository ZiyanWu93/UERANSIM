# Runtime Module

*Part of [Core5G](../overview.md) > Runtime Module*

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

### Core Functions

#### `void runtime(void)`
Main entry point for the runtime system.
- Registers built-in exit conditions (signal handler)
- Calls all handler registrars
- Initializes event pool and mailbox
- Runs the event loop
- Handles cleanup and prints statistics on exit

#### `void trigger_event(int event_id, const char* payload)`
Enqueues a new event for processing.
- **event_id**: Numeric identifier for the event type
- **payload**: Hex-encoded NAS PDU data
- Validates event ID and handler registration
- Creates an EventNf structure and adds it to the mailbox
- Tracks dropped events if mailbox is full

#### `int register_event_handler(int event_id, event_handler_t handler)`
Registers a handler function for a specific event type.
- **event_id**: Event type to handle (must be < MAX_EVENTS)
- **handler**: Function pointer to the handler (use EVENT_HANDLER macro to define)
- Returns: 0 on success, -1 on error

Note: Handlers should be defined using the `EVENT_HANDLER(name)` macro from event.h, which provides the correct function signature and allows access to EVENT_ID and EVENT_PAYLOAD macros.

### Event Source Management

#### `int register_event_source(const char* name, event_source_fn fn)`
Registers an event source that will be polled during listen_to_events.
- **name**: Descriptive name for the event source
- **fn**: Function to call for polling events
- Returns: 0 on success, -1 if maximum sources reached (MAX_EVENT_SOURCES)

#### `int set_event_source_enabled(const char* name, bool enabled)`
Enable or disable an event source by name.
- **name**: Name of the event source
- **enabled**: true to enable, false to disable
- Returns: 0 on success, -1 if source not found

### Exit Condition Management

#### `int register_exit_condition(const char* name, exit_condition_fn fn)`
Registers an exit condition that will be checked in the main loop.
- **name**: Descriptive name for the exit condition
- **fn**: Function that returns true when runtime should exit
- Returns: 0 on success, -1 if maximum conditions reached (MAX_EXIT_CONDITIONS)
- Built-in "signal" condition is registered automatically

### Handler Registration

#### `int register_handler_registrar(handler_registrar_fn fn)`
Registers a handler registrar function to be called during initialization.
- **fn**: Function that registers event handlers
- Returns: 0 on success, -1 if maximum registrars reached (MAX_HANDLER_REGISTRARS)
- Allows modules to register handlers without coupling to runtime

### Error Handling

#### `void set_error_handler(error_handler_fn fn)`
Sets a custom error handler for runtime errors.
- **fn**: Error handler function (NULL to use default)
- Default handler prints to stderr

### Monitoring

#### `RuntimeStats* get_runtime_stats(void)`
Returns pointer to runtime statistics structure.
- **events_processed**: Total events successfully processed
- **events_dropped**: Events dropped due to full mailbox
- **handler_errors**: Invalid events or missing handlers
- **max_queue_depth**: Maximum queue depth observed
- **current_queue_depth**: Current number of queued events

### Utility Functions

#### `event_handler_t* get_routing_table(void)`
Returns pointer to the routing table for direct access.
- Used by modules that need bulk registration
- Allows inspection of registered handlers

#### `Mailbox* get_event_mailbox(void)`
Returns pointer to the global event mailbox.
- Enables direct mailbox operations
- Used for advanced event management

### Internal Functions

#### `void register_handlers(void)`
Called during initialization to execute all handler registrars.
- Iterates through registered handler registrar functions
- Each registrar is responsible for calling register_event_handler()

#### `void listen_to_events(void)`
Polls all enabled event sources.
- Iterates through registered event sources
- Calls poll function for each enabled source
- Sources are responsible for calling trigger_event()

#### `void process_events(void)`
Processes all events in the mailbox queue.
- Updates queue depth statistics
- Pops events from mailbox
- Dispatches to handlers via routing table
- Handles errors for invalid events
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

The runtime supports flexible termination through registered exit condition functions. The main loop continues until any exit condition returns true.

### Exit Condition Function Signature
```c
typedef bool (*exit_condition_fn)(void);

bool my_exit_condition(void) {
    // Check condition
    return true;   // Request exit
    return false;  // Continue running
}
```

### Built-in Exit Conditions
- **Signal Handler**: Automatically registered as "signal" condition
  - Responds to SIGINT (Ctrl+C)
  - Sets keep_running flag to 0

### Registering Exit Conditions
```c
// Register a timeout condition
bool timeout_condition(void) {
    static time_t start = 0;
    if (start == 0) start = time(NULL);
    return (time(NULL) - start) > 3600;  // Exit after 1 hour
}

register_exit_condition("timeout", timeout_condition);
```

### Common Exit Condition Patterns
- **Time Limits**: Maximum runtime duration
- **Event Counts**: Exit after processing N events
- **Resource Limits**: Memory or event pool exhaustion
- **Error Thresholds**: Too many consecutive errors
- **External Triggers**: File existence, network conditions

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
- `MAX_EVENT_SOURCES`: Maximum registered event sources (default: 32)
- `MAX_EXIT_CONDITIONS`: Maximum exit conditions (default: 16)
- `MAX_HANDLER_REGISTRARS`: Maximum handler registrars (default: 32)
- Mailbox size configured in mailbox module
- Memory pool size set during initialization

### Runtime Configuration
- Handlers registered dynamically via registrars
- Exit conditions registered at startup
- Event sources pluggable and can be enabled/disabled
- Error handlers can be customized
- Statistics available for monitoring

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

### Complete Actor Integration
```c
// actor_example.c

// Define handler using EVENT_HANDLER macro
EVENT_HANDLER(handle_my_event) {
    // Access event data
    char* payload = EVENT_PAYLOAD;
    int event_id = EVENT_ID;
    
    // Process event
    printf("Processing event %d with payload: %s\n", event_id, payload);
    
    // Modify payload for response
    strcpy(EVENT_PAYLOAD, "response_data");
    
    // Trigger follow-up event if needed
    trigger_event(NEXT_EVENT_ID, "follow_up_data");
}

// Handler registration function
void my_actor_register_handlers(void) {
    register_event_handler(MY_EVENT_ID, handle_my_event);
    register_event_handler(OTHER_EVENT_ID, handle_other_event);
}

// Initialize actor
void my_actor_init(void) {
    // Register our handler registrar
    register_handler_registrar(my_actor_register_handlers);
}
```

### Event Source Integration
```c
// event_source_example.c

// Poll function for external events
void network_event_source(void) {
    // Check for incoming data (non-blocking)
    if (has_network_data()) {
        char* data = read_network_data();
        int event_type = determine_event_type(data);
        trigger_event(event_type, data);
    }
}

// Timer event source
void timer_event_source(void) {
    static time_t last_tick = 0;
    time_t now = time(NULL);
    
    if (now - last_tick >= 10) {  // Every 10 seconds
        trigger_event(TIMER_TICK_EVENT, "tick");
        last_tick = now;
    }
}

// Register event sources
void init_event_sources(void) {
    register_event_source("network", network_event_source);
    register_event_source("timer", timer_event_source);
}
```

### Custom Exit Conditions
```c
// exit_conditions.c

// Exit after processing N events
bool event_count_exit(void) {
    RuntimeStats* stats = get_runtime_stats();
    return stats->events_processed >= 10000;
}

// Exit on error threshold
bool error_threshold_exit(void) {
    RuntimeStats* stats = get_runtime_stats();
    return stats->handler_errors > 100;
}

// Exit on file existence
bool file_trigger_exit(void) {
    return access("/tmp/stop_runtime", F_OK) == 0;
}

// Register exit conditions
void setup_exit_conditions(void) {
    register_exit_condition("event_count", event_count_exit);
    register_exit_condition("error_threshold", error_threshold_exit);
    register_exit_condition("file_trigger", file_trigger_exit);
}
```

### Main Application
```c
// main.c

int main(int argc, char* argv[]) {
    // Initialize actors
    amf_actor_init();
    smf_actor_init();
    
    // Initialize event sources
    init_event_sources();
    
    // Setup custom exit conditions
    setup_exit_conditions();
    
    // Optionally set custom error handler
    set_error_handler(my_error_handler);
    
    // Run the runtime
    runtime();
    
    return 0;
}
```

## Using Runtime as a Library

The runtime is available as both a standalone executable and a reusable library.

### Standalone Executable
The `runtime` executable provides a ready-to-use runtime environment with basic signal handling:
```bash
./build/runtime
```

### Library Usage
Applications can link against `runtime_lib` to create custom event-driven programs:

#### CMakeLists.txt
```cmake
add_executable(my_app
    main.c
    my_actors.c
)

target_link_libraries(my_app
    runtime_lib
    event_system
    mailbox
    memory
)
```

#### Custom main.c
```c
#include <signal.h>
#include "runtime/runtime.h"
#include "my_actors.h"

int main(int argc, char* argv[]) {
    // Custom initialization
    printf("My Application v1.0\n");
    
    // Register components
    register_handler_registrar(my_actor_register_handlers);
    register_event_source("my_source", my_event_source);
    register_exit_condition("my_exit", my_exit_condition);
    
    // Setup signal handling
    signal(SIGINT, handle_sigint);
    
    // Run the runtime
    runtime();
    
    // Custom cleanup
    printf("Application terminated\n");
    return 0;
}
```

### Build System Integration
The runtime provides:
- `runtime_lib`: Static library containing the framework
- `runtime`: Standalone executable for testing
- Headers automatically included via CMake

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

## See Also

- [Core5G Overview](../overview.md) - Overall system architecture
- [Event System](../event_system/README.md) - Event types and structures
- [Actor Framework](../actor/README.md) - Handler implementations
- [Mailbox System](../mailbox/README.md) - Event queue management
- [Memory Management](../memory/README.md) - Resource allocation strategies