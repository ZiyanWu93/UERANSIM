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
| **Runtime** | Event loop and dispatch | [runtime/README.md](runtime/README.md) |
| **Event System** | Event infrastructure | [event_system/README.md](event_system/README.md) |
| **Mailbox** | Inter-actor communication | [mailbox/README.md](mailbox/README.md) |
| **Memory** | Pool-based allocation | [memory/README.md](memory/README.md) |

### Example Applications

NFLambda includes five example applications demonstrating different architectural patterns and use cases. See **[Application Overview](app/README.md)** for:
- Detailed descriptions of each application
- Recommended learning path from basic to advanced
- Common patterns and code examples
- How to create your own NFLambda applications

## Documentation

### Getting Started
- **[Application Overview](app/README.md)** - Example applications and learning paths
- **[Quick Start](#quick-start)** - Build and run NFLambda applications

### Technical Documentation
- **[Architecture Guide](docs/architecture.md)** - System design and principles
- **[API Reference](docs/api-reference.md)** - Component APIs
- **[Integration Guide](docs/integration.md)** - Building applications with NFLambda
- **[Examples](docs/examples.md)** - Code examples and patterns

### Component Documentation
- **[Runtime](runtime/README.md)** - Event loop and dispatch system
- **[Event System](event_system/README.md)** - Event definitions and handlers
- **[Memory](memory/README.md)** - Pool-based memory management
- **[Mailbox](mailbox/README.md)** - Inter-actor message queues

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

See the **[Application Overview](app/README.md)** for detailed examples and patterns for building NFLambda applications.

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
2. **Explore applications**: Read the **[Application Overview](app/README.md)** for a guided learning path
3. **Understand concepts**: Review the [Architecture Guide](docs/architecture.md)
4. **Build your own**: Use the patterns from example applications

## Development

See [CLAUDE.md](../../CLAUDE.md) for AI assistant guidance and [development_log/](development_log/) for design decisions.