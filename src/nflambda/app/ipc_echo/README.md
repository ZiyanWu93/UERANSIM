# IPC Echo Demo Application

## Overview

The IPC Echo demo demonstrates NFLambda's Inter-Process Communication (IPC) capabilities. It consists of two components:
- **IPC Echo Server** (`ipc_echo_demo`) - NFLambda application that echoes received messages
- **IPC Client Demo** (`ipc_client_demo`) - External client that sends test messages

This demo showcases how external applications can communicate with NFLambda using the built-in IPC event source.

## Purpose

This demo illustrates:
- How to integrate IPC event sources with NFLambda applications
- Event-driven IPC message handling
- Bidirectional communication between NFLambda and external processes
- Simple request-response patterns
- Runtime integration with IPC functionality

## Architecture

### Components

1. **IPC Echo Server** (`ipc_echo_main.c`)
   - Registers IPC event source with runtime
   - Handles `EVENT_IPC_MESSAGE_RECEIVED` events
   - Echoes received messages back to sender
   - Demonstrates NFLambda IPC integration

2. **IPC Client Demo** (`ipc_client_demo.c`)
   - External application using IPC client library
   - Connects to NFLambda IPC server
   - Sends messages and receives responses
   - Supports both single-message and interactive modes

### IPC Flow

```
[Client App] → Unix Socket → [IPC Event Source] → EVENT_IPC_MESSAGE_RECEIVED
                                                         ↓
[Client App] ← Unix Socket ← [IPC Response] ← [Echo Handler]
```

## Building

Both applications are built automatically with the NFLambda project:

```bash
make build
```

The executables `ipc_echo_demo` and `ipc_client_demo` will be copied to the `build/` directory.

## Running

### 1. Start the IPC Echo Server

```bash
./build/ipc_echo_demo
```

Expected output:
```
=====================================
     IPC Echo Demo Application       
=====================================
Demonstrates IPC event source in NFLambda

1. Initializing IPC event source...
2. Registering IPC event source...
3. Registering handler registrar...
4. Setting up signal handler (Ctrl+C to exit)...
5. Starting runtime...

Waiting for IPC connections on /tmp/nflambda_ipc.sock
Press Ctrl+C to exit
---------------------------------
```

### 2. Test with Client (New Terminal)

**Single Message Mode:**
```bash
./build/ipc_client_demo "Hello, NFLambda!"
```

**Interactive Mode:**
```bash
./build/ipc_client_demo
```

### Example Session

**Terminal 1 (Server):**
```bash
./build/ipc_echo_demo
# Server waits for connections
```

**Terminal 2 (Client):**
```bash
./build/ipc_client_demo "Test message"
```

**Client Output:**
```
IPC Client Demo
===============

Connecting to nflambda IPC server...
Connected successfully

Sending: 'Test message'
Received: 'Test message'
Round-trip time: 1.89 ms

Closing connection...
Goodbye!
```

## Key Concepts Demonstrated

### 1. IPC Event Source Integration
The demo shows how to add IPC functionality to any NFLambda application. See `ipc_echo_main.c` for the complete implementation.

### 2. Event-Driven IPC Handling
Messages are processed through the standard NFLambda event system using the actor model. The IPC system handles responses internally.

### 3. External Client Integration
External applications use the IPC client library for simple communication. See `ipc_client_demo.c` for example usage.

### 4. Non-blocking Server Design
The IPC event source uses non-blocking I/O integrated with NFLambda's event loop, ensuring high performance and responsiveness.

## Technical Details

### Protocol
- **Transport**: Unix domain sockets (reliable, local IPC)
- **Message Format**: 4-byte length header + payload (max 2KB)
- **Connection Model**: Single client at a time
- **Socket Path**: `/tmp/nflambda_ipc.sock`

### Performance
- Sub-2ms round-trip times for small messages
- Non-blocking server implementation
- Zero-copy message handling where possible
- Integrated with NFLambda's event-driven architecture

## Customization

You can extend this demo to:
- Handle different message types
- Implement request routing
- Add message validation
- Support multiple concurrent clients
- Integrate with other NFLambda applications

## Integration Patterns

### With Other NFLambda Apps
The IPC echo pattern can be extended for real applications. See the [NFLambda 5G Core](../nflambda_5gcore/) application for a production example of IPC integration with protocol handling.

## Learning Path

Recommended progression after this demo:
1. Study the source code in this directory for implementation details
2. **[NFLambda 5G Core](../nflambda_5gcore/)** - Production IPC usage
3. Build your own IPC-enabled applications

## See Also

- **[Event System](../../event_system/)** - Event handling framework
- **[NFLambda 5G Core](../nflambda_5gcore/)** - Production IPC application
- **[NFLambda Overview](../../README.md)** - System architecture concepts
- **[Ping-Pong Demo](../ping_pong/)** - Basic actor communication patterns