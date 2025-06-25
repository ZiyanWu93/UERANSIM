# NFLambda 5G Core Integration Roadmap

## Overview

This roadmap outlines the integration of NFLambda-based 5G Core (AMF) with UERANSIM using IPC. The goal is to replace direct function calls with event-driven IPC communication for NAS message processing.

## Current State

- **UERANSIM**: Directly calls AMF functions from `src/nflambda/app/5gcore/amf.c`
- **NFLambda**: Has working AMF event handlers but only in simulated demo
- **IPC System**: Ready with 2KB message capacity
- **Scope**: NAS message processing only (no NGAP layer yet)

## Target Architecture

```
UERANSIM (gNB/UE)          NFLambda 5G Core Runtime
       │                            │
   NAS PDU                    IPC Event Source
       │                            │
  IPC Client ←────IPC────→   core_5g_actor
                                    │
                              AMF Event Handlers
                                    │
                            NAS Response via IPC
```

## Milestones

### Milestone 1: NAS Message Protocol (3 days) ✅ COMPLETED

**Goal**: Define how NAS messages are transported over IPC

**Tasks**:
- [x] Define NAS message encoding in IpcMessage.data[2044]
- [x] Create protocol specification document
- [x] Design correlation scheme (using Transaction ID, no UE ID yet)
- [x] Define message type enumeration

**Deliverables**:
- ✅ Protocol specification in PROTOCOL.md
- ✅ Header file with message format definitions (nas_ipc_protocol.h)
- ✅ Implementation of packing/unpacking functions (nas_ipc_protocol.c)
- ✅ Comprehensive unit tests (nas_ipc_protocol_test.c)
- ✅ CMakeLists.txt for building

**Completion Date**: 2025-06-24

### Milestone 2: Core 5G Actor Implementation (1 week) ✅ COMPLETED

**Goal**: Create the NFLambda 5G Core application with IPC support

**Tasks**:
- [x] Create `core_5g_actor.c` with IPC event handling
- [x] Port AMF handlers from `src/nflambda/app/5gcore/amf.c`
- [x] Implement NAS message unpacking and event triggering
- [x] Single UE state management (simplified from multi-UE)
- [x] Create main.c for NFLambda app initialization
- [x] Write CMakeLists.txt for building
- [x] Create test client for verification
- [x] Fix binary data handling in event system

**Deliverables**:
- ✅ Working NFLambda 5G Core application (`nflambda_5gcore`)
- ✅ Single UE state management system
- ✅ Event-driven IPC response handling
- ✅ Test client (`nflambda_5gcore_test_client`)
- ✅ Binary event payload support throughout the system

**Key Design Decisions**:
- Simplified to single UE support (no multi-UE complexity)
- Event-driven IPC responses following actor model
- Direct use of EVENT_IPC_SEND_RESPONSE for responses
- Binary event payloads (uint8_t arrays) instead of strings
- Proper length tracking with input_payload_length field

**Technical Improvements**:
- Enhanced event system to support binary payloads up to 2048 bytes
- Updated trigger_event() to accept binary data with length
- Modified IPC event source to pass full binary messages
- Removed hex encoding workarounds in favor of native binary handling

**Completion Date**: 2025-06-25

**How to Run**:
```bash
# Build
make build

# Start server
./build/nflambda_5gcore

# Test (in another terminal)
./build/nflambda_5gcore_test_client
```

### Milestone 3: UERANSIM IPC Integration (1 week)

**Goal**: Modify UERANSIM to use IPC client instead of direct calls

**Tasks**:
- [ ] Identify all AMF function call points in UERANSIM
- [ ] Replace direct calls with IPC client send/recv
- [ ] Implement NAS message packing for IPC
- [ ] Handle response unpacking and correlation
- [ ] Add configuration option for IPC vs direct mode
- [ ] Ensure backward compatibility

**Deliverables**:
- Modified UERANSIM with IPC support
- Configuration documentation

### Milestone 4: End-to-End Testing (3 days)

**Goal**: Validate the complete integration

**Tasks**:
- [ ] Test UE registration flow via IPC
- [ ] Verify Authentication Request/Response
- [ ] Test Security Mode Command/Complete
- [ ] Validate Registration Accept/Complete
- [ ] Test PDU Session Establishment
- [ ] Performance benchmarking
- [ ] Multi-UE stress testing

**Deliverables**:
- Test results documentation
- Performance metrics

### Milestone 5: Documentation & Examples (2 days)

**Goal**: Complete documentation for users and developers

**Tasks**:
- [ ] Write comprehensive README.md
- [ ] Create integration guide
- [ ] Document configuration options
- [ ] Provide troubleshooting guide
- [ ] Create example scenarios
- [ ] Update main UERANSIM documentation

**Deliverables**:
- Complete documentation package
- Example configurations

## Technical Design

### NAS IPC Message Format

Using the existing IpcMessage structure (2KB total), we pack NAS data as follows:

```
IpcMessage.data[2044] layout:
┌────────────┬───────────┬─────────────┬────────────┬────────────┬───────────┬──────────────┐
│ msg_type   │  version  │  nas_len    │  trans_id  │ event_type │ reserved  │   nas_pdu    │
│ (1 byte)   │ (1 byte)  │  (2 bytes)  │ (4 bytes)  │ (2 bytes)  │ (2 bytes) │  (N bytes)   │
└────────────┴───────────┴─────────────┴────────────┴────────────┴───────────┴──────────────┘
     0            1            2             4            8           10           12

msg_type: 0x01 = NAS_UPLINK, 0x02 = NAS_DOWNLINK, 0xFF = ERROR
version: 0x01 = Protocol version 1
nas_len: Length of NAS PDU (network byte order)
trans_id: Transaction ID for correlation (network byte order)
event_type: Maps to EVENT_NAS_* constants (network byte order)
reserved: Set to 0 for future use
nas_pdu: Actual NAS message (up to 2032 bytes)
```

**Note**: UE identification is not included in Milestone 1 - using hard-coded values

### Helper Functions (Implemented in nas_ipc_protocol.h/c)

```c
// Pack NAS message into IpcMessage
int nas_ipc_pack_message(
    IpcMessage* ipc_msg,
    uint8_t msg_type,
    uint16_t event_type,
    uint32_t transaction_id,
    const uint8_t* nas_pdu,
    uint16_t nas_len
);

// Unpack NAS message from IpcMessage
int nas_ipc_unpack_message(
    const IpcMessage* ipc_msg,
    uint8_t* msg_type,
    uint16_t* event_type,
    uint32_t* transaction_id,
    const uint8_t** nas_pdu,
    uint16_t* nas_len
);

// Additional helper functions implemented:
int nas_ipc_validate_message(const IpcMessage* ipc_msg);
int nas_ipc_create_error_response(IpcMessage* ipc_msg, uint8_t error_code, uint32_t transaction_id);
uint16_t nas_ipc_times_to_event_type(int times);
const char* nas_ipc_msg_type_to_string(uint8_t msg_type);
const char* nas_ipc_event_type_to_string(uint16_t event_type);
const char* nas_ipc_error_to_string(uint8_t error_code);
```

### Event Flow Mapping

| UERANSIM Action | IPC Message | NFLambda Event |
|-----------------|-------------|----------------|
| Registration Request | NAS_UPLINK | EVENT_NAS_REGISTRATION_REQUEST |
| Auth Response | NAS_UPLINK | EVENT_NAS_AUTH_RESPONSE |
| Security Mode Complete | NAS_UPLINK | EVENT_NAS_SECURITY_MODE_COMPLETE |
| Registration Complete | NAS_UPLINK | EVENT_NAS_REGISTRATION_COMPLETE |
| PDU Session Request | NAS_UPLINK | EVENT_NAS_PDU_SESSION_REQUEST |

### Integration Points

#### UERANSIM Side
- Modify points where `amf_process_*` functions are called
- Use IPC client library: `ipc_client_connect()`, `ipc_client_send_recv()`
- Pack/unpack NAS messages

#### NFLambda Side
- IPC event source receives messages
- `core_5g_actor` unpacks and triggers appropriate events
- AMF handlers process events and generate responses
- Responses sent back via `ipc_send_response()`

## Success Criteria

1. **Functional Requirements**
   - Complete UE registration through IPC
   - All NAS procedures work correctly
   - No change in UE behavior

2. **Performance Requirements**
   - < 2ms additional latency per NAS message
   - Support 100+ concurrent UEs
   - No memory leaks or excessive CPU usage

3. **Quality Requirements**
   - Clean separation between UERANSIM and 5G Core
   - Maintainable and extensible code
   - Comprehensive error handling

## Project Structure

```
nflambda_5gcore/
├── ROADMAP.md              # This document
├── README.md               # User guide
├── core_5g_actor.c         # Main actor implementation
├── core_5g_actor.h         # Actor interface
├── nas_protocol.h          # NAS IPC protocol definitions
├── nas_handlers.c          # AMF NAS handlers
├── nas_handlers.h          # Handler declarations
├── ue_context.c            # UE state management
├── ue_context.h            # UE context definitions
├── main.c                  # App entry point
└── CMakeLists.txt          # Build configuration
```

## Next Steps

1. Review and approve this roadmap
2. Create initial project structure
3. Begin Milestone 1 implementation
4. Set up development environment

## Notes

- This integration maintains the existing AMF logic while adding IPC transport
- The design allows for future extension to NGAP and other protocols
- Performance is prioritized with zero-copy message handling where possible