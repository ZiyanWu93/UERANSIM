# Core5G Integration Guide

## Building Applications with Core5G

### Project Structure
```
my_app/
├── CMakeLists.txt
├── my_actor.c
├── my_actor.h
└── main.c
```

### CMakeLists.txt
```cmake
add_executable(my_app
    main.c
    my_actor.c
)

target_link_libraries(my_app
    runtime_lib
    pthread
)
```

### Basic Application Template
```c
// main.c
#include "runtime/runtime.h"
#include "my_actor.h"

int main(int argc, char* argv[]) {
    // Initialize actors
    my_actor_init();
    
    // Register event sources
    register_event_source("startup", startup_source);
    
    // Register exit conditions
    register_exit_condition("complete", is_complete);
    
    // Setup signal handling
    signal(SIGINT, handle_sigint);
    
    // Run
    runtime();
    
    // Print statistics
    RuntimeStats* stats = get_runtime_stats();
    printf("Processed %lu events\n", stats->events_processed);
    
    return 0;
}
```

## NAS Message Processing

### Handling Uplink Messages
```c
EVENT_HANDLER(handle_registration_request) {
    // Parse input NAS PDU
    nas_registration_request_t req;
    parse_nas_pdu(EVENT_PAYLOAD, &req);
    
    // Process
    if (needs_authentication(&req)) {
        generate_auth_request(EVENT_PAYLOAD);
        trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD);
    } else {
        generate_registration_accept(EVENT_PAYLOAD);
        trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD);
    }
}
```

### State Management
```c
typedef struct {
    char supi[32];
    ue_state_t state;
    security_context_t security;
} ue_context_t;

static ue_context_t contexts[MAX_UE];

EVENT_HANDLER(handle_auth_response) {
    ue_context_t* ctx = find_context_by_tmsi(EVENT_PAYLOAD);
    ctx->state = UE_AUTHENTICATED;
    // Continue processing...
}
```

## Testing

### Unit Testing Actors
```c
void test_my_actor() {
    // Initialize
    memory_system_init();
    initialize_event_pool();
    
    // Register handler
    register_event_handler(MY_EVENT, my_handler);
    
    // Trigger event
    trigger_event(MY_EVENT, "test_data");
    
    // Process
    process_events();
    
    // Verify
    assert(expected_result);
}
```

### Integration Testing
See `app/ping_pong/` and `app/amf/` for complete examples.

## Performance Tuning

### Configuration
```c
// Adjust in compile time
#define MAX_EVENTS 200
#define EVENT_POOL_SIZE 32
#define MAILBOX_SIZE 32
```

### Monitoring
```c
void print_stats() {
    RuntimeStats* stats = get_runtime_stats();
    printf("Queue depth: %u/%u\n", 
           stats->current_queue_depth,
           stats->max_queue_depth);
}
```

## Best Practices

1. **Keep handlers short** - Break complex operations into multiple events
2. **Don't block** - Use event sources for I/O operations
3. **Handle errors gracefully** - Log and continue processing
4. **Monitor resources** - Check pool usage and queue depth
5. **Test thoroughly** - Unit test handlers independently

## Common Pitfalls

- **Handler not called**: Check event ID registration
- **Events dropped**: Mailbox full, add backpressure
- **Memory leaks**: Always return events to pool
- **Infinite loops**: Avoid recursive event triggering

## Examples

Complete working examples:
- [Ping-Pong Demo](../app/ping_pong/) - Basic actor communication
- [AMF Demo](../app/amf/) - Full 5G protocol implementation