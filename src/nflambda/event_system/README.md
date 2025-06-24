# Event System

Core message processing infrastructure for NAS events and inter-actor communication.

## Overview

The Event System provides:
- **Event definitions** for NAS messages and control signals
- **IPC event source** for external application communication
- **Memory pool** for zero-allocation event handling
- **Utilities** for debugging and logging

## Event Structure

```c
typedef struct EventNf {
    int event_id;                          // Event type
    char input_payload[MAX_NAS_HEX_LEN];   // Hex-encoded payload
    int input_payload_length;              // Binary length
} EventNf;
```

## Event Categories

### NAS Events
- **Registration**: Request, Accept, Complete, Reject
- **Authentication**: Request, Response, Failure, Reject
- **Security Mode**: Command, Complete, Reject
- **PDU Session**: Establishment, Modification, Release
- **Configuration**: Update Command, Complete

### Control Events
- Internal coordination (Context Setup, Bearer Management)
- Application-specific (Start, Stop, Timer)

### IPC Events
- **MESSAGE_RECEIVED**: External application sent message via IPC

See `event.h` for complete event ID definitions.

## Key APIs

### Pool Management
```c
void initialize_event_pool(void)         // Init after memory system
EventNf* allocate_event(void)           // Get event from pool
void return_event_to_pool(EventNf* evt) // Return to pool
int get_event_pool_usage(void)          // Monitor usage
```

### Utilities
```c
const char* event_name(int event_id)    // Human-readable name
void event_dump(const EventNf* event)   // Debug output
void print_nas_pdu(const char* hex)     // Pretty-print PDU
```

## Usage Pattern

```c
// Allocate
EventNf* event = allocate_event();
if (!event) return ERROR_NO_MEMORY;

// Configure
event->event_id = EVENT_NAS_REGISTRATION_REQUEST;
strcpy(event->input_payload, "7e004179000d01...");

// Use (e.g., send via mailbox)
mailbox_put(mbox, event);

// Handler accesses via macros
EVENT_HANDLER(my_handler) {
    process(EVENT_PAYLOAD);  // Access payload
    int id = EVENT_ID;       // Access event ID
}
```

## Memory Management

- Uses NFLambda Memory System's pool allocator
- Fixed pool size (configurable via `EVENT_POOL_SIZE`)
- Zero-copy throughout the system
- O(1) allocation/deallocation

## Dependencies

Requires Memory System initialization:
```c
memory_system_init();      // First
initialize_event_pool();   // Then
```

## Configuration

In `event.h`:
- `MAX_NAS_HEX_LEN`: 1000 (max payload size)
- `EVENT_POOL_SIZE`: 32 (concurrent events)

## Examples

See implementations in:
- [AMF Actor](../app/amf/fiveg_core_actor.c) - NAS message handling
- [IPC Echo Demo](../app/ipc_echo/) - IPC event handling
- [Event Tests](event_pool_test.c) - Unit tests

## IPC Integration

For Inter-Process Communication, see:
- **[IPC System Documentation](IPC.md)** - Complete IPC guide
- **[IPC Echo Demo](../app/ipc_echo/)** - Working example

## API Details

For complete documentation, see [API Reference](../docs/api-reference.md).