# NFLambda 5G Core

Event-driven 5G Core Network Function implementation using NFLambda runtime, designed to integrate with UERANSIM via IPC.

## Overview

This directory contains the NFLambda-based 5G Core implementation that replaces direct function calls between UERANSIM and AMF with event-driven IPC communication. The goal is to demonstrate how 5G network functions can be built using the NFLambda framework.

## Current Status

### ✅ Milestone 1: NAS Message Protocol (COMPLETED)

The NAS IPC protocol has been fully implemented and tested:

- **Protocol Specification**: See [PROTOCOL.md](PROTOCOL.md)
- **Implementation**: `nas_ipc_protocol.h/c`
- **Unit Tests**: `nas_ipc_protocol_test.c` (13/13 tests passing)
- **Documentation**: Complete protocol documentation with examples

### 🚧 Upcoming Milestones

- **Milestone 2**: Core 5G Actor Implementation
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

## Files

- `nas_ipc_protocol.h` - Protocol definitions and API
- `nas_ipc_protocol.c` - Protocol implementation
- `nas_ipc_protocol_test.c` - Unit tests
- `PROTOCOL.md` - Detailed protocol specification
- `ROADMAP.md` - Development roadmap
- `CMakeLists.txt` - Build configuration

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

## Next Steps

1. Implement the Core 5G Actor (Milestone 2)
2. Port AMF handlers from existing code
3. Integrate IPC event source
4. Modify UERANSIM to use IPC client

## Contributing

Follow the existing code style and ensure all tests pass before submitting changes. See the main UERANSIM contributing guidelines.