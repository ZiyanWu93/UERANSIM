# Implementation Differences: nr-gnb vs nr-gnb-nflambda vs nr-gnb-direct

## Overview

UERANSIM provides three different implementations of the gNodeB (5G base station) simulator, each designed for different use cases and testing scenarios:

1. **nr-gnb** - Original implementation using SCTP/NGAP to communicate with external 5G Core
2. **nr-gnb-nflambda** - IPC-based implementation communicating with NFLambda 5G Core via Unix domain socket
3. **nr-gnb-direct** - Direct function call implementation with embedded AMF handlers

## Architecture Comparison

### 1. Original nr-gnb (Standard Mode)

```
┌─────────────┐     SCTP/NGAP      ┌─────────────┐
│   nr-gnb    │◄──────────────────►│  External   │
│             │                     │  5G Core    │
└─────────────┘                     │ (free5gc,   │
                                    │  open5gs)   │
                                    └─────────────┘
```

- Uses standard 3GPP protocols (SCTP for transport, NGAP for signaling)
- Communicates with external 5G Core implementations
- Full protocol stack implementation
- Production-like behavior

### 2. nr-gnb-nflambda (IPC Mode)

```
┌─────────────┐     Unix Socket     ┌─────────────┐
│nr-gnb-      │◄──────────────────►│  NFLambda   │
│nflambda     │      (IPC)          │  5G Core    │
└─────────────┘                     └─────────────┘
```

- Bypasses SCTP layer entirely
- Uses Unix domain socket for IPC communication
- Messages sent as serialized NAS IPC protocol
- Requires NFLambda 5G Core runtime to be running

### 3. nr-gnb-direct (Direct Call Mode)

```
┌─────────────────────────────────┐
│        nr-gnb-direct            │
│ ┌─────────────┐ ┌─────────────┐ │
│ │   gNodeB    │ │  Embedded   │ │
│ │   Logic     │ │ AMF Handlers│ │
│ └─────────────┘ └─────────────┘ │
└─────────────────────────────────┘
```

- No external communication
- Direct function calls to embedded AMF handlers
- Self-contained testing environment
- Fastest execution for development/testing

## Preprocessor Flags

The implementation differences are controlled by two preprocessor flags:

### USE_NFLAMBDA
- Enables NFLambda mode (bypasses SCTP)
- Used by both nr-gnb-nflambda and nr-gnb-direct

### USE_DIRECT_CALLS
- Enables direct function calls instead of IPC
- Only used by nr-gnb-direct (together with USE_NFLAMBDA)

## Build System Configuration

From `src/gnb/CMakeLists.txt`:

```cmake
# Original gnb library (without NFLambda)
add_library(gnb ${HDR_FILES} ${SRC_FILES})
target_compile_options(gnb PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter)

# NFLambda gnb library (with USE_NFLAMBDA defined)
add_library(gnb-nflambda ${HDR_FILES} ${SRC_FILES})
target_compile_options(gnb-nflambda PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -DUSE_NFLAMBDA)

# Direct calls gnb library (with USE_NFLAMBDA and USE_DIRECT_CALLS defined)
add_library(gnb-direct ${HDR_FILES} ${SRC_FILES})
target_compile_options(gnb-direct PRIVATE -Wall -Wextra -pedantic -Wno-unused-parameter -DUSE_NFLAMBDA -DUSE_DIRECT_CALLS)
```

## Key Implementation Differences

### 1. SCTP Task Behavior

In `src/gnb/sctp/task.cpp`:

```cpp
void SctpTask::onLoop()
{
#ifdef USE_NFLAMBDA
    // NFLambda: Bypass SCTP processing completely
    return;
#endif
    // Original SCTP processing code...
}

void SctpTask::receiveSctpConnectionSetupRequest(...)
{
#ifdef USE_NFLAMBDA
    // NFLambda: Skip SCTP connection setup
    m_logger->info("NFLambda mode: Skipping SCTP connection setup");
    return;
#endif
    // Original SCTP connection setup...
}
```

### 2. NGAP Task Initialization

In `src/gnb/ngap/task.cpp`:

```cpp
void NgapTask::onLoop()
{
#ifdef USE_NFLAMBDA
    // NFLambda: Automatically trigger simulated AMF connection on startup
    if (init == 0)
    {
        receiveNgSetupResponse_base_case();
        init = 1;
    }
#endif
    // Continue with normal message processing...
}
```

### 3. NAS Message Handling

In `src/gnb/ngap/nas.cpp`, the key difference is in message delivery:

```cpp
void NgapTask::handleInitialNasTransport(int ueId, const OctetString &nasPdu, ...)
{
    // ... common processing ...
    
#ifdef USE_NFLAMBDA
    // Store NAS PDU for later use
    copyOctetString(m_regReqInitNasPdu, nasPdu);
    
    #ifdef USE_DIRECT_CALLS
        deliverDownlinkNasRefactored();  // Direct function calls
    #else
        deliverDownlinkNasViaIpc();      // IPC communication
    #endif
#else
    // Original: Send via NGAP/SCTP
    sendNgapUeAssociated(ueId, pdu);
#endif
}
```

### 4. IPC Implementation (nr-gnb-nflambda)

```cpp
void NgapTask::deliverDownlinkNasViaIpc()
{
    // Initialize IPC connection if needed
    if (ipc_fd < 0) {
        const char* socket_path = "/tmp/nflambda_5gcore.sock";
        ipc_fd = ipc_client_connect(socket_path);
        // ...
    }
    
    // Map times counter to NAS IPC event type
    uint16_t event_type = 0;
    switch (times) {
        case 1: event_type = NAS_IPC_EVT_REG_REQUEST; break;
        case 2: event_type = NAS_IPC_EVT_AUTH_RESPONSE; break;
        case 3: event_type = NAS_IPC_EVT_SEC_MODE_COMPLETE; break;
        // ...
    }
    
    // Send message via IPC
    nas_ipc_send_message(ipc_fd, event_type, nas_pdu_data, nas_pdu_length, 
                         &transaction_id, response_buffer, sizeof(response_buffer));
}
```

### 5. Direct Call Implementation (nr-gnb-direct)

```cpp
void NgapTask::deliverDownlinkNasRefactored()
{
    // Prepare event structure for direct function call
    struct Event event;
    event.type = EVENT_GENERATE_AUTH_REQ;  // or other event types
    
    // Copy NAS PDU to event payload
    memcpy(event.payload, stored_nas_pdu, nas_pdu_length);
    
    // Direct function call to AMF handler
    generate_auth_req(&event);  // Defined in 5gcore_without_runtime
    
    // Process response directly
    deliverDownlinkNas(ueId, response_from_event);
}
```

## Message Flow Sequences

### Original nr-gnb
```
UE ──► nr-gnb ──SCTP──► 5G Core AMF
         │                    │
         └────SCTP/NGAP──────┘
```

### nr-gnb-nflambda
```
UE ──► nr-gnb-nflambda ──IPC──► NFLambda 5G Core
         │                             │
         └──Unix Socket Protocol──────┘
```

### nr-gnb-direct
```
UE ──► nr-gnb-direct
         │
         └─► Embedded AMF handlers (direct calls)
```

## Use Cases and Selection Guide

### Use nr-gnb when:
- Testing with real 5G Core implementations (free5gc, open5gs)
- Validating protocol compliance
- Performance testing with production-like setup
- Integration testing with external systems

### Use nr-gnb-nflambda when:
- Testing NFLambda event-driven architecture
- Developing NFLambda-based network functions
- Evaluating IPC-based communication patterns
- Running isolated 5G Core tests

### Use nr-gnb-direct when:
- Rapid development and testing
- Unit testing NAS message flows
- Debugging message processing logic
- Educational purposes (understanding 5G flows)
- CI/CD pipelines (no external dependencies)

## Performance Characteristics

### Latency
- **nr-gnb**: Highest (SCTP + network stack)
- **nr-gnb-nflambda**: Medium (IPC overhead)
- **nr-gnb-direct**: Lowest (direct function calls)

### Resource Usage
- **nr-gnb**: Full protocol stack overhead
- **nr-gnb-nflambda**: Reduced (no SCTP threads)
- **nr-gnb-direct**: Minimal (single process)

### Scalability
- **nr-gnb**: Limited by SCTP associations
- **nr-gnb-nflambda**: Limited by IPC mechanism
- **nr-gnb-direct**: Limited by single process model

## Configuration Requirements

### nr-gnb
- Requires external 5G Core running
- SCTP endpoints must be configured
- Network connectivity needed

### nr-gnb-nflambda
- Requires NFLambda 5G Core running
- Unix socket path must match
- No network configuration needed

### nr-gnb-direct
- No external dependencies
- Self-contained configuration
- Hardcoded test parameters

## Debugging and Development

### Logging Differences
All modes use the same logging framework, but:
- **nr-gnb**: Includes SCTP connection logs
- **nr-gnb-nflambda**: Includes IPC communication logs
- **nr-gnb-direct**: Simplified logs (no transport layer)

### Testing Approach
- **nr-gnb**: End-to-end testing with real core
- **nr-gnb-nflambda**: IPC protocol testing
- **nr-gnb-direct**: Fast iteration on NAS logic

## Future Enhancements

### Planned Improvements
1. Dynamic mode switching via configuration
2. Unified testing framework across all modes
3. Performance benchmarking suite
4. Multi-UE support in direct mode

### Integration Plans
- Service function chaining in direct mode
- Event replay capabilities
- Protocol fuzzing support
- Automated test generation

## Conclusion

The three implementations provide flexibility for different testing and development scenarios:
- **nr-gnb** for production-like testing
- **nr-gnb-nflambda** for NFLambda integration
- **nr-gnb-direct** for rapid development

Choose the appropriate mode based on your specific requirements for realism, performance, and ease of development.