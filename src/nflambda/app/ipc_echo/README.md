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
Shows how to add IPC functionality to any NFLambda application:
```c
// Initialize IPC
ipc_event_source_init(NULL);

// Register with runtime
register_event_source("ipc", ipc_event_source_poll);

// Register event handlers
register_event_handler(EVENT_IPC_MESSAGE_RECEIVED, handle_ipc_message);
```

### 2. Event-Driven IPC Handling
Messages are processed through the standard NFLambda event system:
```c
EVENT_HANDLER(handle_ipc_message) {
    const char* message = EVENT_PAYLOAD;
    ipc_send_response(message, strlen(message));
}
```

### 3. External Client Integration
External applications use the IPC client library for simple communication:
```c
int fd = ipc_client_connect(NULL);
ipc_client_send_recv(fd, data, len, response, &response_len);
ipc_client_close(fd);
```

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

## Learning Path

After understanding this demo:
1. Study the [IPC System Documentation](../../event_system/IPC.md)
2. Review the [IPC API Reference](../../docs/api-reference.md#ipc-apis)
3. Examine other NFLambda applications for integration patterns
4. Build your own IPC-enabled applications

## See Also

- **[IPC System](../../event_system/IPC.md)** - Technical details and API reference
- **[Event System](../../event_system/)** - Event handling framework
- **[NFLambda Overview](../../README.md)** - System architecture  
- **[API Reference](../../docs/api-reference.md)** - Complete API documentation
- **[Ping-Pong Demo](../ping_pong/)** - Basic actor communication example