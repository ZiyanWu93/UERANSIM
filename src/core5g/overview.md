# Core5G Overview

## Introduction

Core5G is a new high-performance 5G core network implementation for UERANSIM, designed with an actor-based architecture for efficient, cache-aware processing. This implementation aims to provide a lightweight, locally-deployable 5G core that eliminates HTTP/NGAP overhead while maintaining full 5G functionality.

## Architecture

### Design Principles
- **Actor-based Architecture**: Event-driven design with mailbox-based message passing
- **Memory Efficiency**: Pre-allocated memory pools with zero-copy message passing
- **Cache-Aware Processing**: Optimized data structures for cache-conscious performance
- **Unified Context Matching**: Efficient state management across network functions
- **Modular Design**: Clean separation of concerns with well-defined interfaces

### Core Components

#### 1. Event System (`event_system/`)
The heart of the processing pipeline, handling NAS message events:
- **EventNf Structure**: Core event data structure (see [EventNf definition](event_system/README.md#eventnf-definition))
- **Event Types**: Registration, Authentication, Security Mode, PDU Session events
- **Event Pool**: Memory management for event objects
- **Utilities**: Helper functions for event debugging and logging

For detailed information about the event system, see [event_system/README.md](event_system/README.md).

#### 2. Mailbox System (`mailbox/`)
Inter-actor communication mechanism:
- **Circular Buffer**: 32-event capacity queue
- **Integrated Memory**: Built-in allocator for EventNf objects
- **Zero-Copy Design**: Events passed by reference, not copied

For detailed information about the mailbox system, see [mailbox/README.md](mailbox/README.md).

#### 3. Runtime (`runtime/`)
Generic event-driven framework providing infrastructure:
- **Event Loop**: Pluggable event sources and flexible exit conditions
- **Routing Table**: Dynamic handler registration without coupling
- **Event Dispatcher**: Routes events with error handling and statistics
- **Framework APIs**: Event sources, exit conditions, handler registrars

For detailed information about the runtime system, see [runtime/README.md](runtime/README.md).

#### 4. Memory Management (`memory/`)
Efficient memory allocation system:
- **Allocator Interface**: Generic allocator abstraction
- **Pool Allocator**: Fixed-size block allocation with O(1) performance
- **Memory Utilities**: Helper functions for memory operations

For detailed information about the memory management system, see [memory/README.md](memory/README.md).

#### 5. Network Function Actors (`actor/`)
Currently implemented:
- **AMF (Access and Mobility Management)**: Handles NAS message processing
  - Registration procedures
  - Authentication flows
  - Security mode establishment
  - PDU session management

Planned implementations:
- **SMF**: Session Management Function
- **UDM**: Unified Data Management
- **AUSF**: Authentication Server Function

For detailed information about the actor framework and network functions, see [actor/README.md](actor/README.md).

## Runtime Architecture

The runtime module provides a generic, decoupled event-driven framework that serves as infrastructure for the Core5G system. It implements a pluggable architecture where actors, event sources, and exit conditions can be registered without the runtime having any knowledge of their specific functionality.

Key features:
- **Pluggable Event Sources**: External sources poll for events and trigger them
- **Dynamic Handler Registration**: Actors register handlers through registrar functions
- **Flexible Exit Conditions**: Multiple conditions can trigger graceful shutdown
- **Built-in Monitoring**: Statistics and error handling without intrusion

For detailed information about the runtime module, including architecture, API reference, integration guidelines, and performance characteristics, see [runtime/README.md](runtime/README.md).


## Directory Structure
```
src/core5g/
├── actor/              # Network function actors
│   ├── amf_actor.*    # AMF implementation
│   ├── smf_actor.c    # SMF placeholder
│   ├── udm_actor.c    # UDM placeholder
│   ├── ausf_actor.c   # AUSF placeholder
│   └── README.md      # Actor framework documentation
├── event_system/       # Event handling infrastructure
│   ├── event.*        # Event definitions
│   ├── event_pool.*   # Event memory management
│   └── README.md      # Event system documentation
├── mailbox/           # Inter-actor communication
│   ├── mailbox.*      # Circular buffer queue
│   └── README.md      # Mailbox system documentation
├── memory/            # Memory management
│   ├── allocator.h    # Allocator interface
│   ├── pool_allocator.* # Pool allocation
│   ├── memory_utils.* # Memory utilities
│   └── README.md      # Memory management documentation
├── development_log/   # Design documents
├── messages_doc/      # NAS message documentation
├── runtime/           # Runtime module
│   ├── runtime.c     # Main event loop implementation
│   ├── runtime.h     # Runtime API and function declarations
│   └── README.md     # Detailed runtime documentation
├── amf.*             # AMF core implementation
├── event_utils.h     # Event utility functions for debugging
└── core5g.h         # Common definitions
```

## Key Features

### Performance Optimizations
- O(1) memory allocation via pools
- Zero-copy message passing
- Cache-aligned data structures
- Deterministic memory usage
- Predictable timing behavior

### Flexibility
- Modular network function deployment
- Local or remote function placement
- Configurable event routing
- Extensible handler registration

### Integration Points
- Designed to work with existing UERANSIM gNB/UE
- Replaces traditional HTTP-based 5G core
- Maintains 3GPP compliance
- Supports standard NAS procedures

