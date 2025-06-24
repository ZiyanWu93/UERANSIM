# Core5G

High-performance 5G core network implementation using actor-based architecture for UERANSIM.

## Quick Start

```bash
# Build everything
make build

# Run example applications
./build/ping_pong_demo  # Simple actor demonstration
./build/amf_demo       # 5G AMF simulation
```

## Architecture

Core5G implements an event-driven, single-threaded architecture optimized for:
- **Zero-copy message passing** between components
- **Predictable performance** with pre-allocated memory pools  
- **Cache-efficient processing** with optimized data structures
- **Modular design** allowing flexible deployment

### Key Components

| Component | Purpose | Details |
|-----------|---------|---------|
| **Runtime** | Event loop and dispatch | [runtime/](runtime/) |
| **Event System** | NAS message definitions | [event_system/](event_system/) |
| **Mailbox** | Inter-actor communication | [mailbox/](mailbox/) |
| **Memory** | Pool-based allocation | [memory/](memory/) |

### Applications

Example implementations demonstrating the framework:
- **[ping_pong](app/ping_pong/)** - Basic actor communication pattern
- **[amf](app/amf/)** - Full 5G AMF with NAS message processing

## Documentation

- **[Architecture Guide](docs/architecture.md)** - System design and principles
- **[API Reference](docs/api-reference.md)** - Component APIs
- **[Integration Guide](docs/integration.md)** - Building applications with Core5G
- **[Examples](docs/examples.md)** - Code examples and patterns

## Directory Structure

```
core5g/
├── runtime/          # Event processing framework
├── event_system/     # Event definitions and utilities  
├── mailbox/         # Message queue implementation
├── memory/          # Memory management
├── app/            # Example applications
│   ├── ping_pong/  # Actor demo
│   └── amf/        # 5G AMF implementation
├── docs/           # Detailed documentation
├── development_log/ # Design decisions
└── messages_doc/   # NAS message specifications
```

## Development

See [CLAUDE.md](../../CLAUDE.md) for AI assistant guidance and [development_log/](development_log/) for design decisions.