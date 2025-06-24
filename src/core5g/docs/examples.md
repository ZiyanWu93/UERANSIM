# Core5G Examples

## Basic Event Handler
```c
EVENT_HANDLER(handle_simple_event) {
    printf("Received: %s\n", EVENT_PAYLOAD);
    strcpy(EVENT_PAYLOAD, "response");
}
```

## Actor with State
```c
static int counter = 0;

EVENT_HANDLER(handle_increment) {
    counter++;
    sprintf(EVENT_PAYLOAD, "count=%d", counter);
    
    if (counter >= 10) {
        trigger_event(EVENT_STOP, "limit_reached");
    }
}
```

## Event Source with Timer
```c
void timer_source(void) {
    static time_t last = 0;
    time_t now = time(NULL);
    
    if (now - last >= 5) {
        trigger_event(EVENT_TIMER_TICK, "tick");
        last = now;
    }
}
```

## Network Event Source
```c
void network_source(void) {
    static int sockfd = -1;
    
    if (sockfd < 0) {
        sockfd = setup_udp_socket(5000);
        set_nonblocking(sockfd);
    }
    
    char buffer[1024];
    int n = recv(sockfd, buffer, sizeof(buffer), MSG_DONTWAIT);
    if (n > 0) {
        buffer[n] = '\0';
        trigger_event(EVENT_NETWORK_DATA, buffer);
    }
}
```

## Chained Event Processing
```c
EVENT_HANDLER(step1) {
    process_step1(EVENT_PAYLOAD);
    trigger_event(EVENT_STEP2, EVENT_PAYLOAD);
}

EVENT_HANDLER(step2) {
    process_step2(EVENT_PAYLOAD);
    trigger_event(EVENT_STEP3, EVENT_PAYLOAD);
}

EVENT_HANDLER(step3) {
    finalize(EVENT_PAYLOAD);
    trigger_event(EVENT_COMPLETE, "done");
}
```

## Error Handling
```c
EVENT_HANDLER(handle_with_errors) {
    if (!validate_input(EVENT_PAYLOAD)) {
        log_error("Invalid input: %s", EVENT_PAYLOAD);
        trigger_event(EVENT_ERROR, "validation_failed");
        return;
    }
    
    if (!process_data(EVENT_PAYLOAD)) {
        log_error("Processing failed");
        trigger_event(EVENT_ERROR, "processing_failed");
        return;
    }
    
    trigger_event(EVENT_SUCCESS, EVENT_PAYLOAD);
}
```

## Custom Exit Conditions
```c
// Exit after timeout
bool timeout_exit(void) {
    static time_t start = 0;
    if (start == 0) start = time(NULL);
    return (time(NULL) - start) > 300; // 5 minutes
}

// Exit on error count
bool error_limit_exit(void) {
    static int errors = 0;
    RuntimeStats* stats = get_runtime_stats();
    if (stats->handler_errors > errors) {
        errors = stats->handler_errors;
        return errors > 10;
    }
    return false;
}

// Exit on file marker
bool file_exit(void) {
    return access("/tmp/stop", F_OK) == 0;
}
```

## Complete Mini-Application
```c
// counter_app.c
#include "runtime/runtime.h"

static int value = 0;
static bool done = false;

EVENT_HANDLER(increment) {
    value++;
    printf("Value: %d\n", value);
    if (value >= 10) done = true;
}

EVENT_HANDLER(decrement) {
    value--;
    printf("Value: %d\n", value);
}

void tick_source(void) {
    static int tick = 0;
    if (tick++ % 2 == 0) {
        trigger_event(100, "");
    } else {
        trigger_event(101, "");
    }
    usleep(100000); // 100ms
}

void register_handlers(void) {
    register_event_handler(100, increment);
    register_event_handler(101, decrement);
}

bool is_done(void) {
    return done;
}

int main() {
    register_handler_registrar(register_handlers);
    register_event_source("tick", tick_source);
    register_exit_condition("done", is_done);
    
    runtime();
    
    printf("Final value: %d\n", value);
    return 0;
}
```

## See Also
- [Ping-Pong Demo](../app/ping_pong/) - Complete actor example
- [AMF Demo](../app/amf/) - Protocol implementation example