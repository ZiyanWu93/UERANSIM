# Core5G API Reference

## Runtime API

### Core Functions
```c
void runtime(void)                    // Main event loop
void trigger_event(int event_id, const char* payload)
```

### Registration Functions
```c
int register_event_handler(int event_id, event_handler_t handler)
int register_event_source(const char* name, event_source_fn fn)
int register_exit_condition(const char* name, exit_condition_fn fn)
int register_handler_registrar(handler_registrar_fn fn)
```

### Configuration
```c
void set_error_handler(error_handler_fn fn)
int set_event_source_enabled(const char* name, bool enabled)
RuntimeStats* get_runtime_stats(void)
```

### Handler Definition
```c
EVENT_HANDLER(handler_name) {
    // Access event via EVENT_ID and EVENT_PAYLOAD macros
    char* payload = EVENT_PAYLOAD;
    int id = EVENT_ID;
}
```

## Event System API

### Event Structure
```c
typedef struct EventNf {
    int event_id;
    char input_payload[MAX_NAS_HEX_LEN];
    int input_payload_length;
} EventNf;
```

### Pool Management
```c
void initialize_event_pool(void)
EventNf* allocate_event(void)
void return_event_to_pool(EventNf* event)
int get_event_pool_usage(void)
```

### Utilities
```c
const char* event_name(int event_id)
void event_dump(const EventNf* event)
void print_nas_pdu(const char* hex_pdu)
```

## Mailbox API

### Lifecycle
```c
Mailbox* mailbox_create(size_t queue_size, size_t event_pool_size)
void mailbox_destroy(Mailbox* mailbox)
```

### Operations
```c
EventNf* mailbox_alloc(Mailbox* mailbox)
void mailbox_free(Mailbox* mailbox, EventNf* event)
bool mailbox_put(Mailbox* mailbox, EventNf* event)
EventNf* mailbox_pop(Mailbox* mailbox)
size_t mailbox_count(Mailbox* mailbox)
```

## Memory API

### Allocator Interface
```c
typedef struct Allocator {
    void* (*alloc)(struct Allocator* allocator, size_t size);
    void (*free)(struct Allocator* allocator, void* ptr);
    void (*destroy)(struct Allocator* allocator);
    void* impl;
} Allocator;
```

### Pool Allocator
```c
PoolAllocator* pool_allocator_create(size_t block_size, size_t num_blocks)
void pool_allocator_destroy(PoolAllocator* allocator)
```

### Memory System
```c
bool memory_system_init(void)
void memory_system_shutdown(void)
Allocator* memory_get_allocator(size_t size, size_t count)
```

## Common Patterns

### Creating an Actor
```c
// Define handlers
EVENT_HANDLER(handle_my_event) {
    process_event(EVENT_PAYLOAD);
    trigger_event(RESPONSE_EVENT, response_data);
}

// Register handlers
void my_actor_register_handlers(void) {
    register_event_handler(MY_EVENT, handle_my_event);
}

// Initialize
void my_actor_init(void) {
    register_handler_registrar(my_actor_register_handlers);
}
```

### Creating an Event Source
```c
void my_event_source(void) {
    if (has_data()) {
        trigger_event(event_type, data);
    }
}

// Register
register_event_source("my_source", my_event_source);
```

### Exit Conditions
```c
bool my_exit_condition(void) {
    return should_exit;
}

register_exit_condition("my_exit", my_exit_condition);
```