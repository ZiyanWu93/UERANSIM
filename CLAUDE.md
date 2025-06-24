# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

UERANSIM is an open-source 5G UE (User Equipment) and RAN (gNodeB) simulator implementing 3GPP Release 15 specifications. The project simulates 5G mobile phones and base stations for testing 5G Core Networks.

Key characteristics:
- Written in C++ (C++17) and C (C11)
- Actor-based architecture with event-driven design
- Radio interface simulated over UDP
- Control and User planes are fully functional
- No longer actively maintained by original author, but accepts community contributions

## Build Commands

```bash
# Standard build (Release mode)
make build

# Clean build artifacts
make clean

# Manual CMake build (Debug mode)
cmake -DCMAKE_BUILD_TYPE=Debug -G "CodeBlocks - Unix Makefiles" . -B cmake-build-debug
cmake --build cmake-build-debug --target all

# The build produces these executables in the build/ directory:
# - nr-gnb: gNodeB (5G base station) simulator
# - nr-ue: User Equipment (5G device) simulator  
# - nr-cli: Command-line interface tool
# - runtime: NFLambda runtime executable
# - libdevbnd.so: Network binding library
# - nr-binder: Network binding tool
# - ipc_echo_demo: NFLambda IPC echo server demo
# - ipc_client_demo: IPC client demo application
```

## Running the Simulator

```bash
# Run gNodeB with configuration
./build/nr-gnb -c config/free5gc-gnb.yaml

# Run UE with configuration
./build/nr-ue -c config/free5gc-ue.yaml

# Use CLI tool to interact with running instances
./build/nr-cli <gnb-id/ue-id> --exec <command>

# Run NFLambda IPC demos
./build/ipc_echo_demo                    # Start IPC server
./build/ipc_client_demo "Hello World"    # Send test message
```

Configuration files are in YAML format and located in `config/` directory with presets for free5gc and open5gs.

## Architecture Overview

### Directory Structure
- `src/asn/`: ASN.1 encoding/decoding for NGAP and RRC protocols
- `src/gnb/`: gNodeB implementation with RRC, NGAP, GTP layers
- `src/ue/`: UE implementation with NAS, RRC layers
- `src/lib/`: Common libraries (app, crypt, nas, rrc, sctp)
- `src/utils/`: Utility functions and common data structures
- `src/nflambda/`: NFLambda - event-driven runtime framework (with 5G core as an application)

### NFLambda Architecture (Event-Driven Runtime Framework)
The project is developing a new high-performance 5G core based on:
- Actor framework with mailbox-based message passing
- Event-driven architecture for cache-aware processing
- Memory pool allocation for predictable performance
- Zero-copy message passing between actors
- Unified context matching for efficient state management
- Built-in IPC system for external application integration

### Key Design Patterns
- Command pattern for actor messages
- Factory pattern for protocol message creation
- Observer pattern for event handling
- Memory pool pattern for allocation

## Development Guidelines

### Code Style
- Use existing code conventions in each module
- C++17 features are available
- Strict compilation with `-Wall -Wextra -pedantic`
- Follow existing naming patterns (camelCase for methods, snake_case for files)

### Adding New Features
1. Check existing implementations in relevant directories
2. Follow the actor model for nflambda components
3. Use memory pools for frequent allocations
4. Implement proper error handling with result types
5. Add logging using existing log infrastructure

### Protocol Implementation
- ASN.1 definitions are in `src/asn/` 
- Use asn1c generated code for encoding/decoding
- Follow 3GPP specifications (Release 15)
- Message flows should align with standard procedures

### Testing
Unit tests exist for core components:
```bash
# Build includes test executables
make build

# Run specific tests (from build directory)
./memory_test
./mailbox_test  
./event_system_test
```

## Important Notes

- The project uses pthread for threading - ensure thread safety
- Memory management is critical - use provided memory pools in nflambda
- Configuration files use YAML format with specific schemas
- UDP is used for simulating radio interface
- SCTP is used for NGAP communication with 5G Core
- The project is dual-licensed (GPL-3.0 and commercial)

## Common Tasks

### Adding a new protocol message
1. Define ASN.1 in appropriate directory under `src/asn/`
2. Generate C code using asn1c
3. Create wrapper classes following existing patterns
4. Add encoding/decoding logic

### Implementing a new actor (nflambda)
1. Create actor class inheriting from base actor
2. Define message types in actor's namespace
3. Implement message handlers
4. Register with actor system
5. Use mailbox for inter-actor communication

### Adding IPC to NFLambda applications
1. Initialize IPC event source with `ipc_event_source_init()`
2. Register IPC event source: `register_event_source("ipc", ipc_event_source_poll)`
3. Register handler for `EVENT_IPC_MESSAGE_RECEIVED`
4. Use `ipc_send_response()` to reply to messages
5. See `src/nflambda/app/ipc_echo/` for complete example

### Debugging protocol issues
1. Enable detailed logging in configuration
2. Use Wireshark with provided UDP/SCTP dissectors
3. Check message encoding/decoding with ASN.1 tools
4. Verify against 3GPP specifications