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

### 2. External Communication: [ipc_echo](ipc_echo/)
- **Learn**: IPC event sources, Unix domain sockets, client/server patterns
- **Key Concepts**: Non-blocking I/O, event-driven IPC, external integration

### 3. Protocol Implementation: [simulated_5g_core_with_runtime](simulated_5g_core_with_runtime/)
- **Learn**: Complex event flows, state management, protocol handling
- **Key Concepts**: NAS message flows, UE state transitions, AMF functionality

### 4. 5G Core Prototype on NFLambda: [nflambda_5gcore](nflambda_5gcore/)
- **Learn**: Complete 5G Core AMF, IPC protocol design, testing strategies
- **Key Concepts**: Binary protocols, production architecture, UERANSIM integration

### 5. Reusable Components: [5gcore_without_runtime](5gcore_without_runtime/)
- **Learn**: Library design, standalone components, modular architecture
- **Key Concepts**: Event handler libraries, NAS message generation, integration patterns

## Common Patterns

For code examples and implementation patterns, see the individual application directories listed above. Each application demonstrates different aspects of NFLambda's capabilities.

## Building Applications

All applications are built together with NFLambda:

```bash
# From UERANSIM root directory
make build

# Executables are placed in build/
ls build/*demo build/nflambda_5gcore*
```

## Creating Your Own Application

For guidance on creating new NFLambda applications, refer to the existing examples above and their individual README files.

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