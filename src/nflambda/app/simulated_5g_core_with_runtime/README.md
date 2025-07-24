# Simulated 5G Core with Runtime Application

Actor-based 5G Core implementation demonstrating complete registration and PDU session flows with modular network function architecture.

## Architecture

Multi-actor architecture with Network Function separation:

### Core Actors
- **fiveg_core_actor** - Main 5G core orchestrator
- **ueransim_actor** - UE/gNB simulator (initiates registration, responds to core)

### Network Functions (NFs)
Each NF has its own module with dispatcher and service functions:
- **AMF** (amf.c) - Access and Mobility Management Function
- **SMF** (smf.c) - Session Management Function  
- **UPF** (upf.c) - User Plane Function
- **AUSF** (ausf.c) - Authentication Server Function
- **UDM** (udm.c) - Unified Data Management
- **PCF** (pcf.c) - Policy Control Function
- **NSSF** (nssf.c) - Network Slice Selection Function

## Message Flow

### Event Routing Architecture
```
UERANSIM ──EVENT_TO_AMF──▶ AMF Dispatcher ──Internal Event──▶ AMF Handler
                               │
                               ├─ Examines NAS message type
                               ├─ Routes to correct handler
                               └─ Forwards SM messages to SMF
```

### NAS Message Flow
```
UE                                AMF
│                                 │
├──Registration Request──────────▶│ (via EVENT_TO_AMF → EVENT_AMF_REGISTRATION_REQUEST)
│◀─────Authentication Request─────┤
├──Authentication Response───────▶│ (via EVENT_TO_AMF → EVENT_AMF_AUTH_RESPONSE)
│◀─────Security Mode Command──────┤
├──Security Mode Complete────────▶│ (via EVENT_TO_AMF → EVENT_AMF_SECURITY_MODE_COMPLETE)
│◀─────Registration Accept────────┤
├──Registration Complete─────────▶│ (via EVENT_TO_AMF → EVENT_AMF_REGISTRATION_COMPLETE)
│◀─────Configuration Update───────┤
├──PDU Session Request───────────▶│ (via EVENT_TO_AMF → EVENT_TO_SMF)
│◀─────PDU Session Accept─────────┤
```

## Event IDs

Events defined in `end_to_end_events.h`:

### NAS Message Events
- **Uplink**: 160-164 (Registration Request through PDU Session Request)
- **Downlink**: 170-174 (Auth Request through PDU Session Accept)
- **Control**: 180-181 (Start, Stop)

### Network Function Dispatcher Events
Each NF has a main dispatcher event and 99 reserved events for internal use:
- **EVENT_TO_AMF** (1000): AMF dispatcher, range 1000-1099
  - EVENT_AMF_REGISTRATION_REQUEST (1001)
  - EVENT_AMF_AUTH_RESPONSE (1002)
  - EVENT_AMF_SECURITY_MODE_COMPLETE (1003)
  - EVENT_AMF_REGISTRATION_COMPLETE (1004)
  - EVENT_AMF_PDU_SESSION_REQUEST (1005)
- **EVENT_TO_SMF** (1100): SMF dispatcher, range 1100-1199
- **EVENT_TO_UPF** (1200): UPF dispatcher, range 1200-1299
- **EVENT_TO_AUSF** (1300): AUSF dispatcher, range 1300-1399
- **EVENT_TO_UDM** (1400): UDM dispatcher, range 1400-1499
- **EVENT_TO_PCF** (1500): PCF dispatcher, range 1500-1599
- **EVENT_TO_NSSF** (1600): NSSF dispatcher, range 1600-1699

## Building & Running

```bash
# Build (from UERANSIM root)
make -j

# Run
./build/end_to_end_5g
```

## Output

The demo shows:
- Complete NAS message exchange with hex PDUs
- State transitions (DEREGISTERED → REGISTERED → PDU_SESSION_ACTIVE)
- Runtime statistics on completion

Example:
```
[UERANSIM] Sending NAS PDU: 7e004179000d0199f907...
[5G Core] Generated Auth Request: 7e00560002000021...
[UERANSIM] === 5G Registration Flow Completed ===
Events processed: 10
```

## Implementation Details

### State Management
- UE states tracked in AMF module (`amf.c`)
- Sequence numbers for security context maintained by AMF
- Fixed NAS PDUs moved to appropriate handler functions

### Event Flow Example
1. UERANSIM generates Registration Request
2. Triggers EVENT_TO_AMF with NAS PDU
3. AMF dispatcher examines message type (0x41)
4. Dispatcher triggers EVENT_AMF_REGISTRATION_REQUEST
5. Handler processes request and sends Authentication Request back

### Code Structure
- `end_to_end_events.h` - Event definitions for all NFs
- `fiveg_core_actor.c/h` - Main 5G core orchestrator
- `ueransim_actor.c/h` - UE/gNB implementation
- `end_to_end_5g_main.c` - Runtime integration
- Network Function modules:
  - `amf.c` - AMF dispatcher and handlers
  - `smf.c` - SMF dispatcher and handlers
  - `upf.c` - UPF dispatcher and handlers
  - `ausf.c` - AUSF dispatcher and handlers
  - `udm.c` - UDM dispatcher and handlers
  - `pcf.c` - PCF dispatcher and handlers
  - `nssf.c` - NSSF dispatcher and handlers

## Network Function Modules

### AMF Dispatcher Pattern
The AMF dispatcher demonstrates the new event routing architecture:

1. **Message Reception**: All AMF-bound messages arrive via EVENT_TO_AMF
2. **Message Classification**: 
   - Plain NAS messages: Message type at byte 2
   - Security-protected messages: Message type at byte 9
3. **Internal Routing**: Based on message type, triggers appropriate internal event
4. **Handler Execution**: Internal events handled by specific AMF functions

### NAS Message Type Detection
```c
// AMF dispatcher intelligently detects message format
if (event->input_payload[0] == 0x7e && event->input_payload[1] != 0x00) {
    // Security protected - message type at byte 9
    message_type = event->input_payload[9];
} else {
    // Plain message - type at byte 2
    message_type = event->input_payload[2];
}
```

### Service Function Chain Implementation
The AMF module now contains the complete handler implementations:
- **Registration Request** → Generates Authentication Request
- **Authentication Response** → Generates Security Mode Command
- **Security Mode Complete** → Generates Registration Accept
- **Registration Complete** → Generates Configuration Update
- **PDU Session Request** → Forwards to SMF (EVENT_TO_SMF)

## Technical Details

### Event Payload Format
NAS messages in this demo use string representations, but NFLambda also supports binary payloads:

```c
// Current string-based approach
char nas_pdu[] = "7e004179000d0199f907...";
trigger_event(EVENT_NAS_MESSAGE, nas_pdu);

// Binary payload alternative
uint8_t binary_nas[] = {0x7e, 0x00, 0x41, 0x79, ...};
trigger_event_binary(EVENT_BINARY_NAS_MESSAGE, binary_nas, sizeof(binary_nas));
```

### Integration with Other Apps
This demo's patterns can be extended:
- **IPC Integration**: Add external communication like [IPC Echo](../ipc_echo/)
- **Modular Components**: Use [5gcore_without_runtime](../5gcore_without_runtime/) handlers
- **Production Deployment**: Scale to [NFLambda 5G Core](../nflambda_5gcore/) architecture

## Implementation Status

### Completed
- ✅ Basic event-driven architecture with AMF, AUSF, UDM, SMF, UPF, PCF, and NSSF actors
- ✅ Complete 5G registration and PDU session establishment flow
- ✅ NAS message handling and verification
- ✅ Event-based communication between NFs using NFLambda runtime
- ✅ Hex PDU validation for all messages
- ✅ Message 1 Service Function Chain: Registration Request → Authentication Request
  - AMF → AUSF → UDM → AUSF → AMF event flow
  - Each NF triggers internal events for processing
  - Proper separation of concerns with request/response types
- ✅ Message 2 Service Function Chain: Authentication Response → Security Mode Command
  - AMF → AUSF → UDM → AUSF → AMF event flow
  - Security context established through inter-NF collaboration
  - MAC calculation, NAS algorithms, and UE security capabilities distributed across NFs
- ✅ Message 3 Service Function Chain: Security Mode Complete → Registration Accept
  - AMF → UDM → AMF event flow
  - UDM provides subscription data (allowed NSSAI)
  - AMF completes registration with GUTI allocation, TAI list, network features, and timer values
- ✅ Message 4 Service Function Chain: Registration Complete → Configuration Update
  - AMF → PCF → AMF event flow
  - PCF provides network names (Open5GS, Next) in UCS2 encoding
  - AMF adds timezone, universal time, and daylight saving information
  - Demonstrates PCF's role in providing network configuration data
- ✅ Message 5 Service Function Chain: PDU Session Request → PDU Session Accept
  - AMF → SMF → UPF → SMF → PCF → SMF → UDM → SMF → AMF event flow
  - Most complex chain involving 5 network functions
  - SMF orchestrates session establishment with QoS rules and Session-AMBR
  - UPF allocates IP address (10.45.0.2)
  - PCF provides PCC rules and DNS configuration
  - UDM provides DNN and S-NSSAI information
  - Demonstrates full 5G service-based architecture in action

### In Progress
- 🔄 Integration with NFLambda's memory management system

## See Also

- **[Runtime Framework](../../runtime/)** - Event processing fundamentals
- **[NFLambda 5G Core](../nflambda_5gcore/)** - Production-ready implementation
- **[5gcore_without_runtime](../5gcore_without_runtime/)** - Reusable AMF components
- **[Event System](../../event_system/)** - Advanced event handling
- **[Ping-Pong Demo](../ping_pong/)** - Basic actor communication
- **[Integration Guide](../../docs/integration.md)** - Building with NFLambda