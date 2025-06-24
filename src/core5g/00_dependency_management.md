# Core5G Dependency Management

## Module Dependencies

```
┌─────────────────┐
│     Runtime     │ (Orchestrator)
└────────┬────────┘
         │ depends on
         ▼
┌─────────────────┐     ┌─────────────────┐
│  Event System   │────▶│     Memory      │
└────────┬────────┘     └────────┬────────┘
         │                       │
         ▼                       ▼
┌─────────────────┐     ┌─────────────────┐
│     Mailbox     │────▶│  Pool Allocator │
└─────────────────┘     └─────────────────┘
```

## Initialization Order

1. **Memory System** - Must be initialized first
2. **Event Pool** - Depends on memory system
3. **Mailbox** - Can be created after memory init
4. **Runtime** - Requires all components initialized

```c
// Correct initialization sequence
memory_system_init();      // 1. Memory
initialize_event_pool();   // 2. Event pool
mailbox = mailbox_create(); // 3. Mailbox
runtime();                 // 4. Runtime
```

## Component Dependencies

### Runtime
- **Depends on**: Event System, Mailbox
- **Used by**: All applications

### Event System  
- **Depends on**: Memory System
- **Used by**: Runtime, all actors

### Mailbox
- **Depends on**: Memory System (internal pool)
- **Used by**: Runtime

### Memory System
- **Depends on**: None (base component)
- **Used by**: All components

## Build Dependencies

### Libraries
```cmake
runtime_lib
├── event_system
├── mailbox
└── memory

Applications link with:
- runtime_lib
- pthread
```

### Header Dependencies
- `runtime.h` includes `event.h`
- `event_pool.h` includes `allocator.h`
- `mailbox.h` is standalone
- Applications include `runtime/runtime.h`

## Application Structure

### Minimal App
```
my_app/
├── CMakeLists.txt
└── main.c (includes runtime/runtime.h)
```

### Actor-based App
```
my_app/
├── CMakeLists.txt
├── main.c
├── my_actor.c (EVENT_HANDLERs)
└── my_actor.h
```

See [app/ping_pong/](app/ping_pong/) and [app/amf/](app/amf/) for examples.