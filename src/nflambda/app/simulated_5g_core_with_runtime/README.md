# Simulated 5G Core with Runtime Application

Actor-based 5G AMF implementation demonstrating complete registration and PDU session flows.

## Architecture

Two actors simulate the 5G network:

- **fiveg_core_actor** - AMF functionality (processes NAS, maintains UE state)
- **ueransim_actor** - UE/gNB simulator (initiates registration, responds to AMF)

## Message Flow

```
UE                                AMF
│                                 │
├──Registration Request──────────▶│
│◀─────Authentication Request─────┤
├──Authentication Response───────▶│
│◀─────Security Mode Command──────┤
├──Security Mode Complete────────▶│
│◀─────Registration Accept────────┤
├──Registration Complete─────────▶│
│◀─────Configuration Update───────┤
├──PDU Session Request───────────▶│
│◀─────PDU Session Accept─────────┤
```

## Event IDs

Events defined in `amf_common.h`:
- **Uplink**: 160-164 (Registration Request through PDU Session Request)
- **Downlink**: 170-174 (Auth Request through PDU Session Accept)
- **Control**: 180-181 (Start, Stop)

## Building & Running

```bash
# Build (from UERANSIM root)
make build

# Run
./build/simulated_5g_core_with_runtime_demo
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
- UE states tracked throughout registration
- Sequence numbers for security context
- Fixed NAS PDUs from original `amf.c`

### Code Structure
- `amf_common.h` - Event definitions
- `fiveg_core_actor.c/h` - AMF implementation  
- `ueransim_actor.c/h` - UE/gNB implementation
- `amf_main.c` - Runtime integration

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

## See Also

- **[Runtime Framework](../../runtime/)** - Event processing fundamentals
- **[NFLambda 5G Core](../nflambda_5gcore/)** - Production-ready implementation
- **[5gcore_without_runtime](../5gcore_without_runtime/)** - Reusable AMF components
- **[Event System](../../event_system/)** - Advanced event handling
- **[Ping-Pong Demo](../ping_pong/)** - Basic actor communication
- **[Integration Guide](../../docs/integration.md)** - Building with NFLambda