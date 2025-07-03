# NFLambda NAS Changes (Phase 3)

This document details the specific changes related to NAS (Non-Access Stratum) message handling between gNB and AMF for the NFLambda version.

## Overview of NAS Flow Changes

The NFLambda version replaces the standard SCTP/NGAP-based NAS transport with a direct IPC mechanism to communicate with the NFLambda 5G Core. Instead of encapsulating NAS messages in NGAP protocol and sending them over SCTP, the system now:

1. Sends NAS PDUs directly via Unix domain socket IPC
2. Bypasses NGAP protocol encapsulation entirely
3. Uses a synchronous request/response pattern
4. Maintains state by storing uplink NAS PDUs for correlation

## Detailed Code Changes

### 1. NFLambda-Specific Includes
**File:** `src/gnb/ngap/nas.cpp`
```cpp
// Lines 29-33: NFLambda-specific headers
#include "nflambda/nflambda.h"
#include "nflambda/event_system/event.h"
#include "amf.h"
#include "nflambda/event_system/ipc_client.h"
#include "nflambda/app/nflambda_5gcore/nas_ipc_protocol.h"
```

### 2. Initial NAS Transport Handling
**File:** `src/gnb/ngap/nas.cpp`
```cpp
// Lines 163-179: Original NGAP encapsulation bypassed
void NgapTask::handleInitialNasTransport(...) {
    // ... create NGAP InitialUEMessage ...
    
    // Original: Send via NGAP/SCTP
    // sendNgapUeAssociated(ueId, pdu);  // <-- COMMENTED OUT
    
    // NFLambda: Store PDU and trigger IPC delivery
    usleep(300000);  // 0.3 second delay
    deliverDownlinkNasViaIpc();  // <-- NEW IPC METHOD
}
```

### 3. Downlink NAS Delivery Disabled
**File:** `src/gnb/ngap/nas.cpp`
```cpp
// Lines 190-193: Original implementation replaced with empty function
void NgapTask::deliverDownlinkNas(int ueId, OctetString &&nasPdu)
{
    return;  // NFLambda: Disabled
}

// Lines 181-187: Original implementation (commented out)
// void NgapTask::deliverDownlinkNas(int ueId, OctetString &&nasPdu)
// {
//     auto w = std::make_unique<NmGnbNgapToRrc>(NmGnbNgapToRrc::NAS_DELIVERY);
//     w->ueId = ueId;
//     w->pdu = std::move(nasPdu);
//     m_base->rrcTask->push(std::move(w));  // Forward to RRC
// }
```

### 4. IPC-Based NAS Delivery
**File:** `src/gnb/ngap/nas.cpp`
```cpp
// Lines 301-460: New IPC implementation
void NgapTask::deliverDownlinkNasViaIpc()
{
    constexpr int hardcoded_ueId = 1;
    static int ipc_fd = -1;
    static uint32_t transaction_id = 1000;
    
    // Connect to NFLambda 5G Core via Unix socket
    if (ipc_fd < 0) {
        const char* socket_path = "/tmp/nflambda_5gcore.sock";
        ipc_fd = ipc_client_connect(socket_path);
        // ...
    }
    
    // Map message sequence to NAS event types
    switch (times) {
        case 1: event_type = NAS_IPC_EVT_REG_REQUEST; break;
        case 2: event_type = NAS_IPC_EVT_AUTH_RESPONSE; break;
        case 3: event_type = NAS_IPC_EVT_SEC_MODE_COMP; break;
        case 4: event_type = NAS_IPC_EVT_REG_COMPLETE; break;
        case 5: event_type = NAS_IPC_EVT_PDU_SESSION; break;
    }
    
    // Send uplink NAS PDU and receive downlink response
    nas_ipc_pack_message(&request, NAS_IPC_MSG_UPLINK, event_type, ...);
    ipc_client_send_recv(ipc_fd, request.data, request.length, ...);
    nas_ipc_unpack_message(&resp_msg, ...);
    
    // Forward response to RRC
    m_base->rrcTask->push(std::move(w));
}
```

### 5. Uplink NAS Transport Modified
**File:** `src/gnb/ngap/nas.cpp`
```cpp
// Lines 462-518: Store uplink PDUs and trigger IPC
void NgapTask::handleUplinkNasTransport(int ueId, const OctetString &nasPdu)
{
    // ... create NGAP UplinkNASTransport ...
    
    // Original: Send via NGAP
    // sendNgapUeAssociated(ueId, pdu);  // <-- COMMENTED OUT
    
    // NFLambda: Store PDU based on message sequence
    switch (times) {
        case 2: copyOctetString(m_authRespUplinkNasPdu, nasPdu); break;
        case 3: copyOctetString(m_secModeUplinkNasPdu, nasPdu); break;
        case 4: copyOctetString(m_regCmpUplinkNasPdu, nasPdu); break;
        case 5: copyOctetString(m_pduReqUplinkNasPdu, nasPdu); break;
    }
    
    // Trigger next IPC exchange
    if (times <= guard) {
        usleep(500000);  // 0.5 second delay
        deliverDownlinkNasViaIpc();
    }
}
```

### 6. State Management Variables
**File:** `src/gnb/ngap/task.hpp` (inferred from usage)
```cpp
// NFLambda: Store uplink NAS PDUs for correlation
OctetString m_initialUplinkNasPdu;    // Registration Request
OctetString m_authRespUplinkNasPdu;   // Authentication Response  
OctetString m_secModeUplinkNasPdu;    // Security Mode Complete
OctetString m_regCmpUplinkNasPdu;     // Registration Complete
OctetString m_pduReqUplinkNasPdu;     // PDU Session Request
```

## NAS Message Flow Sequence

The NFLambda implementation enforces a fixed sequence of NAS messages:

1. **Initial Registration** (times=1)
   - UE → gNB: Registration Request
   - gNB → IPC: NAS_IPC_EVT_REG_REQUEST
   - IPC → gNB: Authentication Request
   - gNB → UE: Authentication Request

2. **Authentication** (times=2)
   - UE → gNB: Authentication Response
   - gNB → IPC: NAS_IPC_EVT_AUTH_RESPONSE
   - IPC → gNB: Security Mode Command
   - gNB → UE: Security Mode Command

3. **Security Mode** (times=3)
   - UE → gNB: Security Mode Complete
   - gNB → IPC: NAS_IPC_EVT_SEC_MODE_COMP
   - IPC → gNB: Registration Accept
   - gNB → UE: Registration Accept

4. **Registration Complete** (times=4)
   - UE → gNB: Registration Complete
   - gNB → IPC: NAS_IPC_EVT_REG_COMPLETE
   - IPC → gNB: Configuration Update Command
   - gNB → UE: Configuration Update Command

5. **PDU Session** (times=5)
   - UE → gNB: PDU Session Establishment Request
   - gNB → IPC: NAS_IPC_EVT_PDU_SESSION
   - IPC → gNB: PDU Session Establishment Accept
   - gNB → UE: PDU Session Establishment Accept

## IPC Communication Details

### Connection Parameters
- **Socket Path:** `/tmp/nflambda_5gcore.sock`
- **Protocol:** Unix domain socket
- **Pattern:** Synchronous request/response
- **Message Format:** Binary protocol defined in `nas_ipc_protocol.h`

### Message Structure
```
IpcMessage {
    uint8_t msg_type;      // NAS_IPC_MSG_UPLINK/DOWNLINK/ERROR
    uint16_t event_type;   // NAS_IPC_EVT_*
    uint32_t transaction_id;
    uint16_t nas_pdu_length;
    uint8_t nas_pdu[...];  // Raw NAS PDU bytes
}
```

### Error Handling
- Automatic reconnection on IPC failure
- Error responses indicated by `NAS_IPC_MSG_ERROR`
- Transaction ID correlation for request/response matching

## Hardcoded Values and Configuration

### Fixed Parameters
- **UE ID:** Always `1` (hardcoded_ueId)
- **AMF ID:** Always `2` (from simulated NGAP setup)
- **Initial Transaction ID:** `1000` (increments per message)
- **Message Sequence:** Controlled by `times` counter (1-5)
- **Guard Value:** `5` (maximum sequence number)

### Timing
- Initial NAS: 300ms delay before IPC
- Subsequent NAS: 500ms delay between messages

## Restoration Requirements

To restore NAS handling for the SCTP version:

1. **Re-enable Original Functions**
   - Uncomment original `deliverDownlinkNas` implementation
   - Remove empty stub function

2. **Restore NGAP Encapsulation**
   - Uncomment `sendNgapUeAssociated` calls in:
     - `handleInitialNasTransport()`
     - `handleUplinkNasTransport()`

3. **Remove IPC Integration**
   - Remove or conditionally compile `deliverDownlinkNasViaIpc()`
   - Remove NFLambda-specific includes
   - Remove state management variables (m_*UplinkNasPdu)

4. **Conditional Compilation**
   - Use `#ifdef USE_NFLAMBDA` to separate implementations
   - Ensure IPC code only compiles for NFLambda version
   - Maintain original flow for standard version

5. **Remove Hardcoded Logic**
   - Remove fixed UE/AMF IDs
   - Remove `times` counter and sequence enforcement
   - Remove artificial delays (usleep calls)

## Summary

The NFLambda version fundamentally changes NAS message handling from a protocol-based SCTP/NGAP transport to a direct IPC mechanism. This bypasses standard 3GPP protocols in favor of a simplified, synchronous communication pattern with the NFLambda 5G Core. The implementation maintains compatibility at the RRC interface level while completely replacing the transport mechanism underneath.