# NFLambda 5G Core

Event-driven 5G Core Network Function implementation using NFLambda runtime, designed to integrate with UERANSIM via IPC.

## Overview

This directory contains the NFLambda-based 5G Core implementation that replaces direct function calls between UERANSIM and AMF with event-driven IPC communication. The goal is to demonstrate how 5G network functions can be built using the NFLambda framework.

## Quick Start

```bash
# Build the project
make build

# Terminal 1: Start 5G Core server
./build/nflambda_5gcore

# Terminal 2: Run test client
./build/nflambda_5gcore_test_client

# Stop server with Ctrl+C
```

## Current Status

### ✅ Milestone 1: NAS Message Protocol (COMPLETED)

The NAS IPC protocol has been fully implemented and tested:

- **Protocol Specification**: See [PROTOCOL.md](PROTOCOL.md)
- **Implementation**: `nas_ipc_protocol.h/c`
- **Unit Tests**: `nas_ipc_protocol_test.c` (13/13 tests passing)
- **Documentation**: Complete protocol documentation with examples

### ✅ Milestone 2: Core 5G Actor Implementation (COMPLETED)

The NFLambda 5G Core actor has been implemented:

- **Core Actor**: `core_5g_actor.h/c` - Event-driven IPC message handling
- **AMF Handlers**: `amf_handlers.h/c` - NAS message processors
- **UE State**: `ue_state.h/c` - Single UE state management
- **Main App**: `main.c` - Runtime initialization
- **Test Client**: `test_client.c` - IPC testing tool

### 🚧 Upcoming Milestones

- **Milestone 3**: UERANSIM IPC Integration  
- **Milestone 4**: End-to-End Testing
- **Milestone 5**: Documentation & Examples

See [ROADMAP.md](ROADMAP.md) for detailed planning.

## Building

The project is built as part of the UERANSIM build system:

```bash
# From UERANSIM root directory
make build

# Run unit tests
./build/nas_ipc_protocol_test
```

The build produces these executables in the `build/` directory:
- `nflambda_5gcore` - The 5G Core AMF server
- `nflambda_5gcore_test_client` - Test client for verification
- `nas_ipc_protocol_test` - Protocol unit tests

## Running the NFLambda 5G Core

### 1. Start the 5G Core Server

```bash
# From UERANSIM root directory
./build/nflambda_5gcore
```

You should see:
```
=====================================
     NFLambda 5G Core v1.0.0
=====================================
Event-driven 5G Core AMF with IPC support

1. Initializing Core 5G actor...
2. Initializing IPC event source...
3. Registering IPC event source...
4. Registering handler registrar...
5. Registering exit condition...
6. Setting up signal handler (Ctrl+C to exit)...
7. Starting runtime...

Waiting for NAS messages on /tmp/nflambda_5gcore.sock
Press Ctrl+C to exit
---------------------------------
```

The server is now listening on `/tmp/nflambda_5gcore.sock` for IPC connections.

### 2. Test with the Test Client

In another terminal:

```bash
# From UERANSIM root directory
./build/nflambda_5gcore_test_client
```

Expected output:
```
NFLambda 5G Core Test Client
============================

Connecting to NFLambda 5G Core...
Connected successfully

Sending Registration Request (trans_id=1001)...
Received response (56 bytes)

Response details:
  Message type: DOWNLINK
  Event type: Registration Request
  Transaction ID: 1001
  NAS PDU length: 44
  NAS PDU: 7e005600020000215ca0df8c9bb8dbcf3c2a7dd448da13692010406296993082800030b762455c890b19

Closing connection...
Test complete!
```

### 3. Stop the Server

Press `Ctrl+C` in the server terminal to gracefully shutdown:

```
^C
Signal received, stopping runtime...

---------------------------------
Cleaning up...

=== NFLambda 5G Core Statistics ===
Events processed: 3
Events dropped: 0
Handler errors: 0
```

## Understanding the Output

### Server Side

When processing messages, the server logs:
```
[Core 5G] Received IPC message
[Core 5G] Unpacked NAS message: type=UPLINK, event=Registration Request, trans_id=1001, nas_len=25
[Core 5G] Triggering registration request event
[AMF] Processing Registration Request
[AMF] Input NAS PDU: 7e004179000d0199f907...
[AMF] Generated Auth Request: 7e005600020000215ca0df8c...
[AMF] Triggering IPC NAS response event
```

### Message Flow

1. Client sends Registration Request (UPLINK)
2. Core 5G actor unpacks the IPC message
3. AMF handler processes the NAS PDU
4. AMF generates Authentication Request
5. Response sent back via IPC (DOWNLINK)

## Troubleshooting

### Socket Already Exists

If you see "Address already in use":
```bash
rm -f /tmp/nflambda_5gcore.sock
```

### Connection Refused

Make sure the server is running before starting the client.

### Debug Mode

For more verbose output:
```bash
# Set debug environment variable (future feature)
export NFLAMBDA_DEBUG=1
./build/nflambda_5gcore
```

## Protocol Overview

The NAS IPC protocol enables communication between UERANSIM and NFLambda 5G Core:

- **Message Types**: UPLINK (0x01), DOWNLINK (0x02), ERROR (0xFF)
- **Max PDU Size**: 2032 bytes
- **Transport**: Unix domain socket via IPC
- **Correlation**: Transaction ID for request/response matching

Example message flow:
```
UERANSIM                    NFLambda 5G Core
    |                              |
    |--[Registration Request]----->|
    |<--[Auth Request]-------------|
    |--[Auth Response]------------>|
    |<--[Security Mode Command]----|
    |--[Security Mode Complete]--->|
    |<--[Registration Accept]------|
    |--[Registration Complete]---->|
```

## Key Features

### Binary Protocol Support
Supports both string and binary NAS message handling:

```c
// String-based NAS PDU (legacy compatibility)
char nas_string[] = "7e004179000d0199f907...";

// Binary NAS PDU (efficient processing)
uint8_t nas_binary[] = {0x7e, 0x00, 0x41, 0x79, ...};
```

### Event-Driven Architecture
Integrates seamlessly with NFLambda's event system:

```c
// IPC message triggers event
EVENT_HANDLER(handle_ipc_nas_message) {
    unpack_nas_ipc_message(EVENT_PAYLOAD);
    trigger_event(EVENT_PROCESS_REGISTRATION_REQUEST, nas_data);
}
```

## Files

- `nas_ipc_protocol.h/c` - Protocol definitions and implementation
- `core_5g_actor.h/c` - Main 5G Core actor with event handling
- `amf_handlers.h/c` - NAS message processing functions
- `ue_state.h/c` - UE context management
- `main.c` - Application entry point and runtime setup
- `test_client.c` - IPC testing utility
- `nas_ipc_protocol_test.c` - Protocol unit tests
- `PROTOCOL.md` - Detailed protocol specification
- `ROADMAP.md` - Development roadmap

## Testing

Run the unit tests to verify protocol implementation:

```bash
./nas_ipc_protocol_test
```

Expected output:
```
=== NAS IPC Protocol Unit Tests ===

Running pack_uplink_message... PASSED
Running unpack_message... PASSED
...
ALL TESTS PASSED!
```

## Integration Examples

### With UERANSIM
```c
// UERANSIM integration (future)
int fd = ipc_client_connect("/tmp/nflambda_5gcore.sock");
nas_ipc_send_uplink(fd, nas_pdu, length, transaction_id);
nas_ipc_receive_downlink(fd, response_buffer, &response_length);
```

### With Other NFLambda Apps
- **[5gcore_without_runtime](../5gcore_without_runtime/)** - Use as AMF handler library
- **[IPC Echo](../ipc_echo/)** - Adapt IPC patterns for NAS messages
- **[Simulated 5G Core](../simulated_5g_core_with_runtime/)** - Compare actor implementations

## Related Documentation

- **[5gcore_without_runtime](../5gcore_without_runtime/)** - Standalone AMF message generators
- **[Simulated 5G Core](../simulated_5g_core_with_runtime/)** - Actor-based 5G implementation
- **[IPC Echo Demo](../ipc_echo/)** - IPC integration patterns
- **[Event System](../../event_system/)** - NFLambda event handling framework
- **[Runtime Documentation](../../runtime/)** - NFLambda runtime architecture

## Contributing

Follow the existing code style and ensure all tests pass before submitting changes. See the main UERANSIM contributing guidelines.