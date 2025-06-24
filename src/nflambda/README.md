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

Applications built on NFLambda:
- **[ping_pong](app/ping_pong/)** - Basic actor communication pattern
- **[ipc_echo](app/ipc_echo/)** - Inter-process communication demo
- **[simulated_5g_core_with_runtime](app/simulated_5g_core_with_runtime/)** - 5G AMF protocol implementation
- **[5gcore_without_runtime](app/5gcore_without_runtime/)** - Full 5G core components

## Documentation

- **[Architecture Guide](docs/architecture.md)** - System design and principles
- **[API Reference](docs/api-reference.md)** - Component APIs
- **[Integration Guide](docs/integration.md)** - Building applications with NFLambda
- **[Examples](docs/examples.md)** - Code examples and patterns

## Directory Structure

```
nflambda/
├── runtime/          # Event processing framework
├── event_system/     # Event definitions and utilities  
├── mailbox/         # Message queue implementation
├── memory/          # Memory management
├── app/            # Example applications
│   ├── ping_pong/  # Actor demo
│   ├── ipc_echo/   # IPC communication demo
│   ├── amf/        # 5G AMF demo
│   └── 5gcore/     # Full 5G core implementation
├── docs/           # Detailed documentation
├── development_log/ # Design decisions
└── messages_doc/   # Protocol specifications
```

## Building Applications

NFLambda applications follow the actor pattern:

```c
// Define event handler
EVENT_HANDLER(my_handler) {
    process_event(EVENT_PAYLOAD);
    trigger_event(RESPONSE_EVENT, response);
}

// Register handlers
void my_actor_register_handlers(void) {
    register_event_handler(MY_EVENT, my_handler);
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
- **Network protocols** - 5G, SDN controllers, protocol gateways
- **IPC servers** - High-performance inter-process communication
- **High-frequency trading** - Low-latency order processing
- **IoT gateways** - Event-driven sensor data processing
- **Real-time analytics** - Stream processing with predictable latency

## Development

See [CLAUDE.md](../../CLAUDE.md) for AI assistant guidance and [development_log/](development_log/) for design decisions.