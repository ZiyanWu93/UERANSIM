# NFLambda

Event-driven runtime framework for network functions - like AWS Lambda but for high-performance network processing.

## Quick Start

```bash
# Build everything
make build

# Run example applications
./build/runtime         # Standalone runtime
./build/ping_pong_demo  # Simple actor demonstration
./build/simulated_5g_core_with_runtime_demo       # 5G AMF example
./build/ipc_echo_demo   # IPC communication demo
```

## What is NFLambda?

NFLambda is a generic, high-performance event-driven runtime framework designed for building network functions. It provides:
- **Actor-based architecture** for concurrent processing without threads
- **Zero-copy message passing** between components
- **Predictable performance** with pre-allocated memory pools
- **Modular design** for building complex network functions
- **Binary event system** supporting both string and binary payloads
- **IPC integration** for external application communication
- **Memory-safe operations** with overflow protection

## Architecture

NFLambda implements an event-driven, single-threaded architecture optimized for:
- **Low latency** - Sub-microsecond event dispatch
- **High throughput** - 100K+ events/second
- **Predictable behavior** - No garbage collection or dynamic allocation
- **Cache efficiency** - Optimized data structures and access patterns

### Core Components

| Component | Purpose | Details |
|-----------|---------|---------|
| **Runtime** | Event loop and dispatch | [runtime/](runtime/) |
| **Event System** | Event infrastructure | [event_system/](event_system/) |
| **Mailbox** | Inter-actor communication | [mailbox/](mailbox/) |
| **Memory** | Pool-based allocation | [memory/](memory/) |

### Example Applications

NFLambda includes five example applications demonstrating different architectural patterns and use cases. See **[Application Overview](app/)** for:
- Detailed descriptions of each application
- Recommended learning path from basic to advanced
- Common patterns and code examples
- How to create your own NFLambda applications

## Documentation

### Getting Started
- **[Application Overview](app/)** - Example applications and learning paths
- **[Quick Start](#quick-start)** - Build and run NFLambda applications

### Technical Documentation
- **[Architecture Guide](docs/architecture.md)** - System design and principles
- **[API Reference](docs/api-reference.md)** - Component APIs
- **[Integration Guide](docs/integration.md)** - Building applications with NFLambda
- **[Examples](docs/examples.md)** - Code examples and patterns

### Component Documentation
- **[Runtime](runtime/)** - Event loop and dispatch system
- **[Event System](event_system/)** - Event definitions and handlers
- **[Memory](memory/)** - Pool-based memory management
- **[Mailbox](mailbox/)** - Inter-actor message queues

## Directory Structure

```
nflambda/
├── runtime/          # Event processing framework
├── event_system/     # Event definitions and utilities  
├── mailbox/         # Message queue implementation
├── memory/          # Memory management
├── app/            # Example applications
│   ├── ping_pong/                      # Basic actor communication
│   ├── ipc_echo/                       # IPC integration demo
│   ├── nflambda_5gcore/               # Full 5G Core with IPC
│   ├── simulated_5g_core_with_runtime/ # Self-contained 5G demo
│   └── 5gcore_without_runtime/         # Standalone NAS library
├── docs/           # Detailed documentation
├── development_log/ # Design decisions
└── messages_doc/   # Protocol specifications
```

## Building Applications

NFLambda applications follow the actor pattern with support for both binary and string payloads:

```c
// Define event handler for string data
EVENT_HANDLER(my_string_handler) {
    const char* message = (const char*)EVENT_PAYLOAD;
    printf("Received: %s\n", message);
    trigger_event(RESPONSE_EVENT, "response");
}

// Define event handler for binary data
EVENT_HANDLER(my_binary_handler) {
    const uint8_t* data = EVENT_PAYLOAD;
    size_t len = EVENT_PAYLOAD_SIZE;
    // Process binary data
    trigger_event_binary(RESPONSE_EVENT, data, len);
}

// Register handlers
void my_actor_register_handlers(void) {
    register_event_handler(MY_STRING_EVENT, my_string_handler);
    register_event_handler(MY_BINARY_EVENT, my_binary_handler);
}

// Initialize and run
int main() {
    register_handler_registrar(my_actor_register_handlers);
    runtime();
    return 0;
}
```

## Use Cases

NFLambda is ideal for:
- **Network protocols** - 5G Core functions, SDN controllers, protocol gateways
- **IPC servers** - High-performance inter-process communication
- **High-frequency trading** - Low-latency order processing systems
- **IoT gateways** - Event-driven sensor data processing
- **Real-time analytics** - Stream processing with predictable latency
- **Protocol testing** - Network function validation and simulation
- **Embedded systems** - Resource-constrained network applications

## Getting Started

1. **Build the framework**: Follow the [Quick Start](#quick-start) instructions
2. **Explore applications**: Read the **[Application Overview](app/)** for a guided learning path
3. **Understand concepts**: Review the [Architecture Guide](docs/architecture.md)
4. **Build your own**: Use the patterns from example applications

## Development

See [CLAUDE.md](../../CLAUDE.md) for AI assistant guidance and [development_log/](development_log/) for design decisions.