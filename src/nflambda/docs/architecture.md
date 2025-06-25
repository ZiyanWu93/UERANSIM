# NFLambda Architecture

## Design Philosophy

NFLambda follows an **event-driven, actor-based architecture** where:
- Network functions are implemented as actors
- All communication happens through events
- Single-threaded execution eliminates concurrency issues
- Memory is pre-allocated for predictable performance

## System Components

### Runtime Framework
The heart of NFLambda - manages event processing and actor lifecycle:
- Polls event sources for new work
- Dispatches events to registered handlers
- Manages graceful shutdown via exit conditions
- Provides statistics and monitoring

### Event Flow
```
[Event Source] → [Mailbox Queue] → [Runtime Dispatcher] → [Event Handler]
                                                              ↓
                                                        [Trigger New Event]
```

### Memory Architecture
All components use pre-allocated memory pools:
```
Memory System
├── Pool Allocator (generic)
├── Event Pool (32 EventNf objects)
└── Mailbox Memory (integrated)
```

## Actor Model

Actors are independent processing units that:
1. Maintain private state
2. Communicate only through events
3. Process one event at a time
4. Can trigger new events

Example actors:
- **fiveg_core_actor**: Processes NAS messages, maintains UE state
- **ueransim_actor**: Simulates UE/gNB behavior

## Event Types

Events carry binary payloads up to 2048 bytes:
- **Uplink**: Registration, Authentication Response, PDU Session Request
- **Downlink**: Authentication Request, Security Command, Registration Accept
- **Control**: Start, Stop, Timer events

### Binary Event System
- Events use `uint8_t` arrays for binary data support
- Length tracking with `input_payload_length` field
- Zero-copy IPC message passing
- Support for both text and binary protocols

## Performance Characteristics

- **Latency**: Sub-millisecond event processing
- **Throughput**: 100K+ events/second (simple handlers)
- **Memory**: Fixed allocation, no fragmentation
- **CPU**: Single core, cache-optimized

## Integration Points

NFLambda integrates with UERANSIM by:
- Replacing HTTP-based 5G core
- Processing NAS messages directly
- Maintaining 3GPP compliance
- Supporting standard procedures