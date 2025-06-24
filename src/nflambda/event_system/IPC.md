# NFLambda IPC System

## Overview

The NFLambda IPC (Inter-Process Communication) system provides a built-in mechanism for external applications to communicate with NFLambda-based applications. It implements a Unix domain socket server that integrates seamlessly with NFLambda's event-driven architecture.

## Architecture

### Components

The IPC system consists of two main components:

1. **IPC Event Source** (`ipc_event_source.h/c`) - Server-side integration with NFLambda
2. **IPC Client Library** (`ipc_client.h/c`) - Client-side library for external applications

### Integration with NFLambda

```
External App → Unix Socket → IPC Event Source → NFLambda Event System
                                    ↓
External App ← Unix Socket ← IPC Response ← Event Handler
```

### Event Flow

1. External application connects to Unix domain socket
2. Client sends message through IPC client library
3. IPC event source polls socket (non-blocking)
4. When message arrives, generates `EVENT_IPC_MESSAGE_RECEIVED`
5. NFLambda application handles event through registered handler
6. Handler can send response using `ipc_send_response()`
7. Response is delivered back to client

## Protocol Specification

### Message Format

All IPC messages use a simple binary protocol:

```c
typedef struct {
    uint32_t length;      // Message length (excluding header)
    uint8_t data[2044];   // Payload data (max 2044 bytes)
} IpcMessage;
```

- **Total message size**: 2048 bytes (2KB)
- **Header size**: 4 bytes (length field)
- **Maximum payload**: 2044 bytes
- **Byte order**: Host byte order (local communication only)

### Connection Model

- **Transport**: Unix domain sockets (`SOCK_STREAM`)
- **Path**: `/tmp/nflambda_ipc.sock` (configurable)
- **Concurrency**: Single client at a time
- **Reliability**: TCP-like semantics (reliable, ordered delivery)

## Server-Side API (NFLambda Applications)

### Initialization

```c
#include "event_system/ipc_event_source.h"

// Initialize IPC server
int result = ipc_event_source_init(NULL);  // Use default socket path
if (result < 0) {
    // Handle initialization error
}

// Register with runtime
register_event_source("ipc", ipc_event_source_poll);
```

### Event Handling

```c
#include "event_system/event.h"

EVENT_HANDLER(handle_ipc_message) {
    const char* message = EVENT_PAYLOAD;
    size_t length = strlen(message);
    
    // Process the message
    printf("Received: %s\n", message);
    
    // Send response
    if (ipc_send_response(message, length) < 0) {
        // Handle send error
    }
}

// Register event handler
void register_handlers(void) {
    register_event_handler(EVENT_IPC_MESSAGE_RECEIVED, handle_ipc_message);
}
```

### Cleanup

```c
// Clean up IPC resources
ipc_event_source_cleanup();
```

### Server-Side API Reference

| Function | Description | Returns |
|----------|-------------|---------|
| `ipc_event_source_init(path)` | Initialize IPC server | 0 on success, -1 on error |
| `ipc_event_source_poll()` | Poll for IPC events (non-blocking) | void |
| `ipc_send_response(data, len)` | Send response to connected client | bytes sent or -1 |
| `ipc_is_client_connected()` | Check if client is connected | true/false |
| `ipc_event_source_cleanup()` | Clean up IPC resources | void |

## Client-Side API (External Applications)

### Basic Usage

```c
#include "nflambda/event_system/ipc_client.h"

// Connect to NFLambda
int fd = ipc_client_connect(NULL);  // Use default socket path
if (fd < 0) {
    // Handle connection error
}

// Send message and receive response
char response[2048];
size_t response_len = sizeof(response);
int result = ipc_client_send_recv(fd, "Hello", 5, response, &response_len);
if (result < 0) {
    // Handle communication error
}

printf("Response: %.*s\n", (int)response_len, response);

// Close connection
ipc_client_close(fd);
```

### Client-Side API Reference

| Function | Description | Returns |
|----------|-------------|---------|
| `ipc_client_connect(path)` | Connect to NFLambda IPC server | Socket FD or -1 |
| `ipc_client_send_recv(fd, send_data, send_len, recv_data, recv_len)` | Send message and receive response | 0 on success, -1 on error |
| `ipc_client_close(fd)` | Close IPC connection | void |

## Performance Characteristics

### Latency
- **Round-trip time**: Sub-2ms for small messages
- **Event dispatch**: ~100ns overhead
- **Socket overhead**: ~1ms (Unix domain sockets)

### Throughput
- **Message rate**: Limited by client connection model (single client)
- **Bandwidth**: Up to ~1GB/s for large messages (Unix socket theoretical max)
- **CPU usage**: Minimal (non-blocking I/O, event-driven)

### Scalability
- **Memory**: Fixed overhead (~8KB for buffers)
- **File descriptors**: 2 per IPC instance (server + client)
- **Connections**: 1 concurrent client (by design)

## Configuration

### Socket Path
The default socket path is `/tmp/nflambda_ipc.sock`. You can customize it:

```c
// Server side
ipc_event_source_init("/custom/path/to/socket");

// Client side  
ipc_client_connect("/custom/path/to/socket");
```

### Buffer Sizes
The message buffer size is fixed at 2KB. For larger messages, implement chunking at the application level.

### Connection Timeout
Clients should implement timeouts for robust error handling:

```c
// Set socket timeout (example using raw socket APIs)
struct timeval timeout = {.tv_sec = 5, .tv_usec = 0};
setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
```

## Error Handling

### Common Error Conditions

| Error | Cause | Solution |
|-------|-------|----------|
| Connection refused | NFLambda not running | Start NFLambda application |
| Permission denied | Socket path not writable | Check directory permissions |
| Address in use | Socket file exists | Remove stale socket file |
| Message too large | Payload > 2044 bytes | Split message or reduce size |
| Client disconnected | Network error | Reconnect from client |

### Error Codes

- **-1**: General error (check `errno` for details)
- **0**: Success
- **Positive**: Number of bytes sent/received

## Security Considerations

### Access Control
- Unix domain sockets inherit filesystem permissions
- Default socket created with user-only access (0600)
- Consider using dedicated directory with restricted permissions

### Input Validation
- Always validate message lengths
- Sanitize input data in event handlers
- Implement application-level message validation

### DoS Protection
- Single client limitation provides basic DoS protection
- Consider rate limiting in application logic
- Monitor for abnormal message patterns

## Integration Examples

### Simple Echo Server
See [IPC Echo Demo](../app/ipc_echo/) for complete example.

### Protocol Gateway
```c
EVENT_HANDLER(handle_protocol_message) {
    // Parse incoming protocol message
    ProtocolMessage msg;
    if (parse_message(EVENT_PAYLOAD, &msg) < 0) {
        ipc_send_response("ERROR: Invalid message", 21);
        return;
    }
    
    // Process and respond
    ProtocolResponse response = process_message(&msg);
    char response_buffer[2048];
    int len = serialize_response(&response, response_buffer);
    ipc_send_response(response_buffer, len);
}
```

### Asynchronous Processing
```c
EVENT_HANDLER(handle_async_request) {
    // Store client context for later response
    store_client_context(EVENT_PAYLOAD);
    
    // Trigger async processing
    trigger_event(EVENT_ASYNC_PROCESS, EVENT_PAYLOAD);
    
    // Response will be sent when processing completes
}
```

## Troubleshooting

### Debug Mode
Enable debug logging to trace IPC operations:

```bash
export IPC_DEBUG=1
./build/your_app
```

### Socket Issues
```bash
# Check if socket exists
ls -la /tmp/nflambda_ipc.sock

# Remove stale socket
rm -f /tmp/nflambda_ipc.sock

# Check socket permissions
stat /tmp/nflambda_ipc.sock
```

### Connection Testing
Use `socat` to test IPC connectivity:

```bash
# Send test message
echo "test" | socat - UNIX-CONNECT:/tmp/nflambda_ipc.sock
```

## Future Enhancements

Potential improvements to the IPC system:
- Multiple concurrent client support
- Named message types and routing
- Message priority and queuing
- Authentication and encryption
- Remote IPC over TCP sockets
- Shared memory transport option

## See Also

- **[Event System Documentation](README.md)** - Core event handling
- **[IPC Echo Demo](../app/ipc_echo/)** - Complete working example
- **[API Reference](../docs/api-reference.md)** - All API documentation
- **[Architecture Guide](../docs/architecture.md)** - System design principles