# NFLambda Applications

This directory contains example applications built on the NFLambda event-driven runtime framework. Each application demonstrates different aspects of NFLambda's capabilities, from basic actor communication to full 5G network function implementations.

## Application Overview

| Application | Type | Complexity | Key Features |
|-------------|------|------------|--------------|
| **[ping_pong](ping_pong/)** | Demo | Basic | Actor communication, event handling, runtime lifecycle |
| **[ipc_echo](ipc_echo/)** | Demo | Intermediate | IPC integration, external communication, client/server |
| **[simulated_5g_core_with_runtime](simulated_5g_core_with_runtime/)** | Demo | Advanced | 5G protocol simulation, complete NAS flows |
| **[nflambda_5gcore](nflambda_5gcore/)** | Production | Advanced | Full 5G AMF with IPC, UERANSIM integration |
| **[5gcore_without_runtime](5gcore_without_runtime/)** | Library | Intermediate | Standalone NAS generators, reusable components |

## Learning Path

Follow this recommended progression to understand NFLambda concepts:

### 1. Start Simple: [ping_pong](ping_pong/)
- **Learn**: Basic actor pattern, event handlers, runtime initialization
- **Key Concepts**: Event triggering, handler registration, exit conditions
- **Next Step**: Understand how actors communicate without direct function calls

### 2. External Communication: [ipc_echo](ipc_echo/)
- **Learn**: IPC event sources, Unix domain sockets, client/server patterns
- **Key Concepts**: Non-blocking I/O, event-driven IPC, external integration
- **Next Step**: See how NFLambda interacts with external processes

### 3. Protocol Implementation: [simulated_5g_core_with_runtime](simulated_5g_core_with_runtime/)
- **Learn**: Complex event flows, state management, protocol handling
- **Key Concepts**: NAS message flows, UE state transitions, AMF functionality
- **Next Step**: Understand real network protocol implementation patterns

### 4. Production System: [nflambda_5gcore](nflambda_5gcore/)
- **Learn**: Complete 5G Core AMF, IPC protocol design, testing strategies
- **Key Concepts**: Binary protocols, production architecture, UERANSIM integration
- **Next Step**: Build production-ready network functions

### 5. Reusable Components: [5gcore_without_runtime](5gcore_without_runtime/)
- **Learn**: Library design, standalone components, modular architecture
- **Key Concepts**: Event handler libraries, NAS message generation, integration patterns
- **Next Step**: Create your own reusable NFLambda components

## Common Patterns

### Actor Definition
All applications follow the actor pattern:
```c
// Define actor's event handlers
EVENT_HANDLER(my_handler) {
    // Process event
    trigger_event(RESPONSE_EVENT, data);
}

// Register handlers
void my_actor_register_handlers(void) {
    register_event_handler(MY_EVENT, my_handler);
}
```

### Binary Event Support
NFLambda supports both string and binary payloads:
```c
// String events (simple messages)
trigger_event(EVENT_TYPE, "Hello");

// Binary events (protocol messages)
uint8_t nas_pdu[] = {0x7e, 0x00, 0x41, ...};
trigger_event_binary(EVENT_TYPE, nas_pdu, sizeof(nas_pdu));
```

### IPC Integration
Applications can communicate with external processes:
```c
// Server side
ipc_event_source_init(NULL);
register_event_source("ipc", ipc_event_source_poll);

// Client side
int fd = ipc_client_connect("/tmp/nflambda.sock");
ipc_client_send_recv(fd, request, req_len, response, &resp_len);
```

## Building Applications

All applications are built together with NFLambda:

```bash
# From UERANSIM root directory
make build

# Executables are placed in build/
ls build/*demo build/nflambda_5gcore*
```

## Creating Your Own Application

To create a new NFLambda application:

1. **Create directory**: `mkdir my_app`
2. **Add CMakeLists.txt**:
   ```cmake
   add_executable(my_app
       main.c
       my_actor.c
   )
   target_link_libraries(my_app
       runtime
       event_system
       memory
   )
   ```
3. **Implement actors** following the patterns above
4. **Add to parent CMakeLists.txt**: `add_subdirectory(my_app)`
5. **Build and test** your application

## Architecture Considerations

### Memory Management
- Use NFLambda's memory pools for predictable performance
- Avoid dynamic allocation in event handlers
- Pre-allocate buffers for protocol messages

### Event Design
- Keep event handlers short and non-blocking
- Use event chaining for complex workflows
- Maintain clear event boundaries

### State Management
- Store actor state in static or actor-specific structures
- Use event payloads for message passing only
- Implement proper cleanup in exit handlers

## Testing Applications

Each application includes different testing approaches:
- **Unit tests**: See `nflambda_5gcore/nas_ipc_protocol_test.c`
- **Integration tests**: Use test clients like `nflambda_5gcore/test_client.c`
- **Manual testing**: Interactive demos with console input

## Documentation

Each application has its own README with:
- Detailed architecture explanation
- Building and running instructions
- Code examples and API reference
- Integration guidance

## See Also

- **[NFLambda Overview](../README.md)** - Framework architecture and concepts
- **[Runtime Documentation](../runtime/)** - Event loop and dispatch system
- **[Event System](../event_system/)** - Event handling framework
- **[Memory Management](../memory/)** - Pool allocators and memory safety
- **[API Reference](../docs/api-reference.md)** - Complete API documentation