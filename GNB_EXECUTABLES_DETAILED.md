# UERANSIM gNodeB Executables: Technical Deep Dive

## Table of Contents
1. [Executive Summary](#executive-summary)
2. [Architecture Overview](#architecture-overview)
3. [Implementation Details](#implementation-details)
4. [Communication Mechanisms](#communication-mechanisms)
5. [Build System Configuration](#build-system-configuration)
6. [Code Flow Analysis](#code-flow-analysis)
7. [Use Cases and Selection Guide](#use-cases-and-selection-guide)
8. [Performance Considerations](#performance-considerations)
9. [Testing and Development](#testing-and-development)

## Executive Summary

UERANSIM provides three distinct gNodeB (5G base station) executables, each optimized for different deployment scenarios and testing requirements:

| Executable | Purpose | Communication | Use Case |
|------------|---------|---------------|----------|
| **nr-gnb** | Production | SCTP/NGAP | Real 5G Core testing |
| **nr-gnb-nflambda** | Event-driven | IPC/Unix Socket | NFLambda integration |
| **nr-gnb-direct** | Testing | Direct calls | Rapid development |

## Architecture Overview

### Traditional Architecture (nr-gnb) - Microservices
```
                     Traditional 5G Core (Microservices)
    ┌────────────────────────────────────────────────────────────┐
    │  ┌─────────┐ HTTP  ┌─────────┐ HTTP  ┌─────────┐           │
    │  │   UDM   │◄─────►│   UDR   │◄─────►│   PCF   │           │
    │  └────┬────┘       └─────────┘       └────┬────┘           │
    │       │ HTTP                               │ HTTP          │
    │  ┌────▼────┐      ┌─────────┐        ┌────▼────┐           │
    │  │  AUSF   │ HTTP │   NRF   │  HTTP  │   SMF   │           │
    │  └────┬────┘◄─────►─────────◄───────►└────┬────┘           │
    │       │                                    │               │
    │       │ HTTP                               │ HTTP/PFCP     │
    │  ┌────▼────────────────────────────────────▼────┐          │
    │  │                    AMF                       │          │
    │  │  (Access and Mobility Management Function)   │          │
    │  └──────────────────────┬───────────────────────┘          │
    └─────────────────────────┼──────────────────────────────────┘
                              │ SCTP/NGAP
                              ▼
                        ┌─────────────┐
                        │   nr-gnb    │
                        └──────┬──────┘
                               │ UDP/RRC
                               ▼
                        ┌─────────────┐
                        │    nr-ue    │
                        └─────────────┘

Note: Each 5G Core component runs as a separate process/container with HTTP-based service interfaces

### NFLambda Architecture (nr-gnb-nflambda) - Consolidated Event-Driven
```
                     NFLambda 5G Core (Consolidated)
    ┌────────────────────────────────────────────────────────────┐
    │                    Single Process/Runtime                  │
    │  ┌───────────────────────────────────────────────────┐     │
    │  │                   Event Loop                      │     │
    │  │  ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐     │     │
    │  │  │ AMF  │ │ SMF  │ │ UDM  │ │ AUSF │ │ PCF  │     │     │
    │  │  │Actor │ │Actor │ │Actor │ │Actor │ │Actor │     │     │
    │  │  └───┬──┘ └───┬──┘ └───┬──┘ └───┬──┘ └───┬──┘     │     │
    │  │      └─────────┴─────────┴─────────┴───────┘      │     │
    │  │                    Mailbox System                 │     │
    │  │                  (Zero-copy messaging)            │     │
    │  └────────────────────────┬──────────────────────────┘     │
    │                           │ IPC Handler                    │
    └───────────────────────────┼────────────────────────────────┘
                                │ Unix Socket
                                ▼
                        ┌──────────────┐
                        │nr-gnb-       │
                        │nflambda      │
                        └──────┬───────┘
                               │ UDP/RRC
                               ▼
                        ┌─────────────┐
                        │    nr-ue    │
                        └─────────────┘

Note: All 5G Core functions run as actors in a single event-driven process

### Direct Call Architecture (nr-gnb-direct) - Embedded Testing
```
┌─────────────────────────────────────────────────────┐
│              nr-gnb-direct (Single Process)         │
│                                                     │
│  ┌─────────────┐      Direct      ┌──────────────┐ │
│  │             │     Function     │              │ │
│  │ gNB Logic   │ ◄────Calls────► │ AMF Handlers │ │
│  │             │                  │  (embedded)  │ │
│  └──────┬──────┘                  └──────────────┘ │
│         │                                           │
│         │ Internal Events/Callbacks                 │
│         ▼                                           │
│  ┌─────────────────────────────────────────────┐   │
│  │        5gcore_without_runtime Library       │   │
│  │   • generate_auth_req()                     │   │
│  │   • generate_security_cmd()                 │   │
│  │   • generate_registration_accept()          │   │
│  │   • generate_pdu_session_establishment()    │   │
│  └─────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────┘
          │ UDP/RRC
          ▼
    ┌─────────────┐
    │    nr-ue    │
    └─────────────┘

Note: All processing happens within a single process for testing

## Architectural Comparison: Microservices vs Consolidated

### Traditional 5G Core (Microservices)

**Characteristics:**
- **Deployment**: Each network function (AMF, SMF, UDM, etc.) runs as a separate process or container
- **Communication**: HTTP/2 based service interfaces (typically REST or gRPC)
- **Service Discovery**: Via NRF (Network Repository Function)
- **Scaling**: Individual functions can be scaled independently
- **Complexity**: High operational complexity, orchestration required
- **Latency**: Inter-service communication adds latency (typically 1-10ms per hop)

**Advantages:**
- Industry standard (3GPP Service-Based Architecture)
- Independent scaling and deployment
- Fault isolation between services
- Technology diversity possible

**Challenges:**
- Network overhead between services
- Complex deployment and orchestration
- Distributed state management
- Higher resource consumption

### NFLambda 5G Core (Consolidated)

**Characteristics:**
- **Deployment**: All network functions run as actors in a single process
- **Communication**: Zero-copy message passing via mailboxes
- **Service Discovery**: Compile-time actor registration
- **Scaling**: Scale the entire core as a unit
- **Complexity**: Simplified deployment, single binary
- **Latency**: Sub-microsecond inter-actor communication

**Advantages:**
- Minimal communication overhead
- Cache-efficient processing
- Predictable performance
- Simplified deployment
- Lower resource usage

**Challenges:**
- Monolithic scaling
- Shared fault domain
- Less flexibility in technology choices
- Requires careful memory management

### Performance Impact

| Metric | Microservices | Consolidated |
|--------|---------------|--------------|
| Message Latency | 1-10ms | <1μs |
| Throughput | Network limited | Memory limited |
| CPU Efficiency | Lower (marshaling) | Higher (direct) |
| Memory Usage | Higher (duplicated) | Lower (shared) |
| Deployment | Complex | Simple |

## Implementation Details

### Preprocessor Directives

The behavior of each executable is controlled by compile-time flags:

| Executable | USE_NFLAMBDA | USE_DIRECT_CALLS |
|------------|--------------|------------------|
| nr-gnb | Not defined | Not defined |
| nr-gnb-nflambda | Defined | Not defined |
| nr-gnb-direct | Defined | Defined |

### Key Code Differences

#### 1. NGAP Task Initialization (src/gnb/ngap/task.cpp)

```cpp
#ifdef USE_NFLAMBDA
void NgapTask::onStart() {
    // Skip NGAP association setup
    m_ueCtx = nullptr;
    
    // Set AMF as connected (simulated)
    m_amfCtx->association.inStreams = 0;
    m_amfCtx->association.outStreams = 0;
    m_amfCtx->state = EAmfState::CONNECTED;
    
    logger->info("Simulated AMF connection established");
}
#else
void NgapTask::onStart() {
    // Traditional SCTP association setup
    performSctpConnection();
}
#endif
```

#### 2. NAS Message Handling (src/gnb/ngap/nas.cpp)

```cpp
#ifdef USE_NFLAMBDA
void NgapTask::handleInitialNasTransport(...) {
    // Store NAS PDU for correlation
    m_initialNasPdu = nasPdu.copy();
    m_ueCtx = asn::New<ASN_NGAP_UE_NGAP_IDs>();
    // ... setup UE context ...
    
    #ifdef USE_DIRECT_CALLS
        // Direct function call to AMF handler
        handleDirectAMFCall(nasPdu);
    #else
        // Send via IPC to NFLambda 5G Core
        sendViaIPC(nasPdu);
    #endif
}
#else
void NgapTask::handleInitialNasTransport(...) {
    // Build and send NGAP InitialUEMessage
    auto *pdu = asn::ngap::NewMessagePdu<InitialUEMessage>(...);
    sendNgapUeAssociated(ueId, pdu);
}
#endif
```

#### 3. SCTP Processing (src/gnb/sctp/task.cpp)

```cpp
void SctpTask::onLoop() {
#ifdef USE_NFLAMBDA
    // No SCTP processing needed
    return;
#else
    // Normal SCTP socket handling
    fd_set fdSet;
    // ... process SCTP events ...
#endif
}
```

### Member Variable Additions

When USE_NFLAMBDA is defined, additional storage is added:

```cpp
// src/gnb/ngap/task.hpp
class NgapTask : public NtsTask {
#ifdef USE_NFLAMBDA
    OctetString m_initialNasPdu;
    OctetString m_uplinkNasPdu;
    ASN_NGAP_UE_NGAP_IDs *m_ueCtx;
    #ifdef USE_DIRECT_CALLS
        DirectAMFHandler m_amfHandler;
    #else
        IPCClient m_ipcClient;
    #endif
#endif
    // ... rest of class ...
};
```

## Communication Mechanisms

### 1. Traditional SCTP/NGAP (nr-gnb)

- **Protocol**: Stream Control Transmission Protocol (SCTP)
- **Message Format**: ASN.1 encoded NGAP PDUs
- **Connection**: Persistent association with AMF
- **Reliability**: Built-in SCTP reliability features
- **Example Flow**:
  ```
  gNB → AMF: SCTP INIT
  AMF → gNB: SCTP INIT-ACK
  gNB → AMF: NGSetupRequest
  AMF → gNB: NGSetupResponse
  ```

### 2. IPC via Unix Socket (nr-gnb-nflambda)

- **Protocol**: Binary protocol over Unix domain socket
- **Message Format**: Custom header + NAS PDU
- **Connection**: `/tmp/nflambda_5gcore.sock`
- **Event Integration**: Polls IPC socket in event loop
- **Example Message Structure**:
  ```c
  struct IPCMessage {
      uint32_t message_type;  // e.g., NAS_UPLINK
      uint32_t payload_size;
      uint8_t payload[];      // NAS PDU
  };
  ```

### 3. Direct Function Calls (nr-gnb-direct)

- **Protocol**: In-process function invocation
- **Message Format**: Function parameters
- **Connection**: None (same process)
- **Handlers**: From `5gcore_without_runtime` library
- **Example Call Chain**:
  ```c
  handleInitialNasTransport()
    → generate_auth_req()
    → simulateAuthResponse()
    → generate_security_cmd()
    → simulateSecurityComplete()
    → generate_registration_accept()
  ```

## Build System Configuration

### CMakeLists.txt Structure

```cmake
# Three separate libraries with different compile flags
add_library(gnb ${SRC_FILES})
target_compile_options(gnb PRIVATE -Wall -Wextra -pedantic)

add_library(gnb-nflambda ${SRC_FILES})
target_compile_options(gnb-nflambda PRIVATE -DUSE_NFLAMBDA)

add_library(gnb-direct ${SRC_FILES})
target_compile_options(gnb-direct PRIVATE -DUSE_NFLAMBDA -DUSE_DIRECT_CALLS)

# Three executables linking different libraries
add_executable(nr-gnb src/gnb.cpp)
target_link_libraries(nr-gnb gnb)

add_executable(nr-gnb-nflambda src/gnb.cpp)
target_link_libraries(nr-gnb-nflambda gnb-nflambda)

add_executable(nr-gnb-direct src/gnb.cpp)
target_link_libraries(nr-gnb-direct gnb-direct)
```

### Library Dependencies

All three executables link against:
- `asn-ngap`: NGAP protocol ASN.1 definitions
- `asn-rrc`: RRC protocol ASN.1 definitions
- `common-lib`: Shared utilities
- `5gcore_without_runtime_lib`: NAS message generators
- `nflambda`: Core event system (even for traditional mode)

## Code Flow Analysis

### Registration Flow Comparison

#### Traditional (nr-gnb)
```
1. UE → gNB: RRC Setup Complete (with NAS Registration Request)
2. gNB → AMF: NGAP Initial UE Message
3. AMF → gNB: NGAP Downlink NAS Transport (Auth Request)
4. gNB → UE: RRC DL Information Transfer
5. ... (full NGAP message exchange) ...
```

#### Event-Driven (nr-gnb-nflambda)
```
1. UE → gNB: RRC Setup Complete (with NAS Registration Request)
2. gNB → IPC: Send NAS PDU to NFLambda
3. NFLambda: Process in event loop, generate response
4. IPC → gNB: Receive NAS response
5. gNB → UE: RRC DL Information Transfer
```

#### Direct Call (nr-gnb-direct)
```
1. UE → gNB: RRC Setup Complete (with NAS Registration Request)
2. gNB: Call generate_auth_req() directly
3. gNB: Simulate UE response internally
4. gNB: Call generate_security_cmd() directly
5. ... (all processing in same thread) ...
```

## Use Cases and Selection Guide

### When to Use nr-gnb (Traditional)

**Scenarios:**
- Production testing with real 5G Core (Open5GS, free5GC)
- Conformance testing requiring standard protocols
- Integration with commercial 5G equipment
- Network deployment validation

**Advantages:**
- Industry-standard protocols
- Interoperability with any 3GPP-compliant core
- Network-level debugging with Wireshark
- Realistic network conditions

**Limitations:**
- Requires external 5G Core
- Network configuration complexity
- Higher latency due to network stack

### When to Use nr-gnb-nflambda (Event-Driven)

**Scenarios:**
- High-performance testing scenarios
- Exploring event-driven architectures
- Integration with NFLambda-based network functions
- Research on novel 5G architectures

**Advantages:**
- Superior performance (no network stack overhead)
- Cache-efficient actor model
- Seamless NFLambda integration
- Predictable latency

**Limitations:**
- Requires NFLambda 5G Core
- Non-standard communication protocol
- Limited to local deployment

### When to Use nr-gnb-direct (Testing)

**Scenarios:**
- Unit testing NAS procedures
- Rapid prototyping of new features
- Debugging message flows
- Educational purposes

**Advantages:**
- Fastest execution (no IPC overhead)
- Simplified debugging (single process)
- No external dependencies
- Immediate feedback

**Limitations:**
- Not suitable for production
- Limited to predefined message flows
- No real network testing

## Performance Considerations

### Latency Comparison

| Mode | Message Round-Trip | Overhead Source |
|------|-------------------|-----------------|
| Traditional | 5-50ms | Network stack, SCTP processing |
| Event-Driven | 0.1-1ms | IPC, event dispatch |
| Direct Call | <0.01ms | Function call only |

### Throughput Characteristics

- **Traditional**: Limited by network bandwidth and SCTP flow control
- **Event-Driven**: 100K+ messages/second possible
- **Direct Call**: Memory bandwidth limited

### Resource Usage

- **Traditional**: Highest (SCTP buffers, network threads)
- **Event-Driven**: Moderate (IPC buffers, event system)
- **Direct Call**: Lowest (stack memory only)

## Testing and Development

### Development Workflow

#### For Protocol Development
1. Start with `nr-gnb-direct` for rapid iteration
2. Implement and test message flows
3. Migrate to `nr-gnb-nflambda` for IPC testing
4. Finally validate with `nr-gnb` against real core

#### For Performance Testing
1. Use `nr-gnb-nflambda` for baseline performance
2. Compare with `nr-gnb` for network overhead analysis
3. Use `nr-gnb-direct` for theoretical maximum

### Debugging Tips

#### Traditional Mode
```bash
# Capture SCTP/NGAP traffic
sudo tcpdump -i any -w ngap.pcap sctp
# Analyze with Wireshark
wireshark ngap.pcap
```

#### Event-Driven Mode
```bash
# Monitor IPC socket
socat - UNIX-CONNECT:/tmp/nflambda_5gcore.sock
# Check NFLambda logs
tail -f nflambda_5gcore.log
```

#### Direct Call Mode
```bash
# Use debugger for step-through
gdb ./build/nr-gnb-direct
(gdb) break generate_auth_req
(gdb) run -c config/free5gc-gnb.yaml
```

### Configuration Examples

All three modes use the same YAML configuration format:
```yaml
# config/free5gc-gnb.yaml
mcc: '999'
mnc: '70'
nci: '0x000000010'
idLength: 32
tac: 1
linkIp: 127.0.0.1
ngapIp: 127.0.0.1    # Used only by nr-gnb
gtpIp: 127.0.0.1
```

## Conclusion

The three gNodeB executables represent a progression from traditional network protocols to modern event-driven architectures:

1. **nr-gnb**: Industry-standard implementation for production use
2. **nr-gnb-nflambda**: Next-generation event-driven architecture
3. **nr-gnb-direct**: Streamlined testing and development tool

Each serves distinct purposes in the 5G development ecosystem, from standards compliance to performance optimization to rapid prototyping. Understanding their differences enables developers to choose the right tool for their specific requirements.