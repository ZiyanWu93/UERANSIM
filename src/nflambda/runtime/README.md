# Runtime Module

Event-driven execution framework that orchestrates NFLambda components.

## Overview

The runtime provides:
- **Event loop** for continuous processing
- **Event routing** with O(1) dispatch
- **Plugin architecture** for event sources and handlers
- **Monitoring** via built-in statistics

## Architecture

```
┌─────────────────┐     ┌──────────────┐     ┌─────────────┐
│  Event Sources  │────▶│   Mailbox    │────▶│  Handlers   │
└─────────────────┘     └──────────────┘     └─────────────┘
                              │                      │
                              └──────────────────────┘
                                   (Event Loop)
```

## Key APIs

### Main Entry Point
```c
void runtime(void)  // Starts the event loop
```

### Event Management
```c
void trigger_event(int event_id, const char* payload)
int register_event_handler(int event_id, event_handler_t handler)
```

### Plugin Registration
```c
int register_event_source(const char* name, event_source_fn fn)
int register_exit_condition(const char* name, exit_condition_fn fn)
int register_handler_registrar(handler_registrar_fn fn)
```

### Configuration & Monitoring
```c
void set_error_handler(error_handler_fn fn)
RuntimeStats* get_runtime_stats(void)
```

## Usage

### As a Library
```c
// main.c
#include "runtime/runtime.h"

int main() {
    // Register components
    register_handler_registrar(my_handlers);
    register_event_source("my_source", my_source_fn);
    register_exit_condition("done", is_done);
    
    // Run
    runtime();
    
    return 0;
}
```

### Build Configuration
```cmake
target_link_libraries(my_app
    runtime_lib
    pthread
)
```

## Handler Pattern
```c
EVENT_HANDLER(my_handler) {
    // Access event data
    char* data = EVENT_PAYLOAD;
    int id = EVENT_ID;
    
    // Process event
    process(data);
    
    // Trigger follow-up if needed
    trigger_event(NEXT_EVENT, result);
}
```

## Configuration

Compile-time settings in `runtime.h`:
- `MAX_EVENTS`: 200 (event types)
- `MAX_EVENT_SOURCES`: 32
- `MAX_EXIT_CONDITIONS`: 16
- `MAX_HANDLER_REGISTRARS`: 32

## Performance

- **Event dispatch**: < 1μs overhead
- **Throughput**: 100K+ events/sec
- **Memory**: Fixed allocation
- **CPU**: Single core

## Examples

See working implementations:
- [Ping-Pong Demo](../app/ping_pong/ping_pong_main.c)
- [AMF Demo](../app/amf/amf_main.c)

## API Details

For complete API documentation, see [API Reference](../docs/api-reference.md).
For integration guidance, see [Integration Guide](../docs/integration.md).