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
Messages are processed through the standard NFLambda event system using the actor model:
```c
EVENT_HANDLER(handle_ipc_message) {
    const char* message = (const char*)EVENT_PAYLOAD;
    // Following actor model - trigger response event instead of direct I/O
    trigger_event(EVENT_IPC_SEND_RESPONSE, message);
}
```

The runtime handles the actual IPC response through a dedicated handler:
```c
EVENT_HANDLER(handle_ipc_send_response) {
    const char* message = (const char*)EVENT_PAYLOAD;
    ipc_send_response(message, strlen(message));
}
```

### Binary IPC Support
NFLambda IPC also supports binary payloads for protocol messages:
```c
EVENT_HANDLER(handle_binary_ipc_message) {
    const uint8_t* data = EVENT_PAYLOAD;
    size_t len = EVENT_PAYLOAD_SIZE;
    // Process binary protocol data
    trigger_event_binary(EVENT_IPC_SEND_BINARY_RESPONSE, data, len);
}
```

This design maintains the event-driven actor model where actors never perform I/O directly.

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

## Integration Patterns

### With Other NFLambda Apps
The IPC echo pattern can be extended for real applications:

```c
// Integration with 5G Core
EVENT_HANDLER(handle_nas_ipc_request) {
    // Unpack NAS protocol message
    nas_message_t* nas = unpack_nas_message(EVENT_PAYLOAD);
    
    // Process with AMF handlers
    trigger_event(EVENT_PROCESS_NAS_MESSAGE, nas);
}
```

## Learning Path

Recommended progression after this demo:
1. **[IPC System Documentation](../../event_system/IPC.md)** - Technical IPC details
2. **[Binary Event System](../../event_system/)** - Advanced payload handling
3. **[NFLambda 5G Core](../nflambda_5gcore/)** - Production IPC usage
4. **[API Reference](../../docs/api-reference.md#ipc-apis)** - Complete IPC API
5. Build your own IPC-enabled applications

## See Also

- **[IPC System](../../event_system/IPC.md)** - Technical details and API reference
- **[Event System](../../event_system/)** - Event handling framework
- **[NFLambda 5G Core](../nflambda_5gcore/)** - Production IPC application
- **[Binary Event System](../../event_system/)** - Advanced payload handling
- **[NFLambda Overview](../../README.md)** - System architecture concepts
- **[Ping-Pong Demo](../ping_pong/)** - Basic actor communication patterns