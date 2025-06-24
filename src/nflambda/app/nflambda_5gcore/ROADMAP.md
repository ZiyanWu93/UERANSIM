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

### Milestone 1: NAS Message Protocol (3 days)

**Goal**: Define how NAS messages are transported over IPC

**Tasks**:
- [ ] Define NAS message encoding in IpcMessage.data[2044]
- [ ] Create protocol specification document
- [ ] Design UE identification and correlation scheme
- [ ] Define message type enumeration

**Deliverables**:
- Protocol specification in this ROADMAP
- Header file with message format definitions

### Milestone 2: Core 5G Actor Implementation (1 week)

**Goal**: Create the NFLambda 5G Core application with IPC support

**Tasks**:
- [ ] Create `core_5g_actor.c` with IPC event handling
- [ ] Port AMF handlers from `src/nflambda/app/5gcore/amf.c`
- [ ] Implement NAS message unpacking and event triggering
- [ ] Add UE context management for multiple UEs
- [ ] Create main.c for NFLambda app initialization
- [ ] Write CMakeLists.txt for building

**Deliverables**:
- Working NFLambda 5G Core application
- UE state management system

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
┌────────────┬────────────┬─────────────┬──────────────┐
│ msg_type   │   ue_id    │  nas_len    │   nas_pdu    │
│ (1 byte)   │ (4 bytes)  │  (2 bytes)  │  (N bytes)   │
└────────────┴────────────┴─────────────┴──────────────┘
     0             1             5             7

msg_type: 0x01 = NAS_UPLINK, 0x02 = NAS_DOWNLINK
ue_id: RAN-UE-NGAP-ID for correlation
nas_len: Length of NAS PDU
nas_pdu: Actual NAS message (up to 2037 bytes)
```

### Helper Functions

```c
// Pack NAS message into IpcMessage (no extra memory)
void pack_nas_message(IpcMessage* msg, uint8_t type, 
                     uint32_t ue_id, const uint8_t* nas_pdu, 
                     uint16_t nas_len);

// Unpack NAS message from IpcMessage
void unpack_nas_message(const IpcMessage* msg, uint8_t* type,
                       uint32_t* ue_id, uint8_t** nas_pdu, 
                       uint16_t* nas_len);
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