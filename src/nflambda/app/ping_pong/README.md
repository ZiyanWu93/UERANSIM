# Ping-Pong Demo Application

## Overview

The Ping-Pong demo is a minimal example application that demonstrates how to use the NFLambda Runtime Framework. It implements a simple message exchange between two actors (Ping and Pong) to showcase the event-driven architecture.

## Purpose

This demo illustrates:
- How to create actors with event handlers
- The handler registration process
- Event source implementation
- Exit condition configuration
- The complete application lifecycle

## Architecture

### Components

1. **Ping Actor** (`ping_actor.c`)
   - Initiates the ping-pong sequence
   - Sends ping requests
   - Handles pong responses
   - Tracks exchange count
   - Triggers application stop

2. **Pong Actor** (`pong_actor.c`)
   - Listens for ping requests
   - Responds with pong messages
   - Simple echo-style behavior

3. **Main Application** (`ping_pong_main.c`)
   - Demonstrates the full registration flow
   - Implements event sources
   - Configures exit conditions
   - Shows runtime integration

### Event Flow

```
[Startup] → EVENT_START → [Ping Actor]
                              ↓
                    EVENT_PING_REQUEST
                              ↓
                        [Pong Actor]
                              ↓
                    EVENT_PONG_RESPONSE
                              ↓
                        [Ping Actor]
                              ↓
                    (repeat or EVENT_STOP)
```

## Building

The demo is built automatically with the NFLambda project:
```bash
make build
```

The executable `ping_pong_demo` will be copied to the `build/` directory.

## Running

```bash
./build/ping_pong_demo
```

### Expected Output

```
=================================
   Ping-Pong Demo Application    
=================================
Demonstrating the NFLambda Runtime Framework

1. Initializing actors...
2. Registering handler registrars...
3. Registering event sources...
4. Registering exit conditions...
5. Setting custom error handler...
6. Setting up signal handler (Ctrl+C to exit)...
7. Starting runtime...

Press Enter to stop the demo...
---------------------------------
Initializing runtime...
  - Ping actor: Registering handlers
  - Pong actor: Registering handlers
Runtime initialized successfully
Registered 2 event sources, 3 exit conditions

=== Starting ping-pong sequence ===

PING: Starting ping-pong sequence
PING[1]: Sending "ping_1"
PONG: Received ping: "ping_1"
PONG: Sending response: "pong_for_ping_1"
PING[1]: Received pong: "pong_for_ping_1"
...
PING: Reached maximum count (5), stopping
Exit condition 'demo_complete' triggered

---------------------------------

=== Demo Complete ===
Statistics:
  Events processed: 12
  Events dropped: 0
  Handler errors: 0
  Max queue depth: 2

Thank you for trying the Ping-Pong demo!
```

## Key Concepts Demonstrated

### 1. Actor Pattern
Each actor is self-contained with:
- Initialization function
- Handler registration function
- Event handlers for specific events

### 2. Handler Registration
The main function shows the explicit registration steps:
```c
register_handler_registrar(ping_register_handlers);
register_handler_registrar(pong_register_handlers);
```

### 3. Event Sources
Two event sources demonstrate different patterns:
- **Startup Source**: One-time event after delay
- **Console Source**: Continuous monitoring for user input

### 4. Exit Conditions
Multiple conditions show flexibility:
- Stop event processed
- Timeout (safety mechanism)
- Built-in signal handler

### 5. Event-Driven Communication
Actors communicate only through events, demonstrating:
- Loose coupling
- Asynchronous processing
- Event chaining

## Customization

You can modify this demo to experiment:
- Change `MAX_PING_COUNT` in `common.h`
- Add new event types
- Implement additional actors
- Create more complex event flows
- Add custom statistics tracking

## Learning Path

After understanding this demo:
1. Study the runtime framework documentation
2. Examine how AMF actor is implemented
3. Create your own actors
4. Build more complex applications

## See Also

- [Runtime Module](../../runtime/) - Framework documentation
- [Event System](../../event_system/) - Event handling details
- [NFLambda Overview](../../README.md) - System architecture
- [AMF Demo](../amf/) - Protocol implementation example