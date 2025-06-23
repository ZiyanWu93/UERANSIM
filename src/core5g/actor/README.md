# Actor Framework

*Part of [Core5G](../overview.md) > Actor Framework*

## Overview

The Actor framework implements network functions (NFs) as independent actors in the Core5G system. Each actor represents a specific 5G network function (AMF, SMF, UDM, AUSF) and processes events according to 3GPP specifications. The framework provides a clean abstraction for network function implementation with event-driven processing and message-based communication.

## Architecture

### Design Principles

- **Event-Driven**: All processing triggered by NAS message events
- **Stateless Processing**: Each event handler is self-contained
- **Message-Based Communication**: Actors communicate through the mailbox system
- **Extensible**: Easy to add new network functions
- **Standards Compliant**: Implements 3GPP Release 15 procedures

### Actor Model

Each network function actor follows this pattern:
1. Registers event handlers for specific message types
2. Processes incoming events from the mailbox
3. Generates responses or triggers new events
4. Maintains minimal state for session management

## Network Functions

### AMF (Access and Mobility Management)

The AMF is the primary entry point for UE control plane signaling and handles:

#### Implemented Features
- **Registration Management**
  - Initial registration request processing
  - Registration accept generation
  - Registration reject for error cases
  
- **Authentication**
  - Authentication request generation
  - Authentication result processing
  - SUCI/SUPI handling
  
- **Security Mode Control**
  - Security mode command generation
  - Algorithm negotiation
  - Key derivation coordination
  
- **Session Management**
  - PDU session establishment requests
  - Session context management
  - QoS profile handling

#### Event Handlers
```c
// Registration flow
void amf_handle_registration_request(void);
void amf_handle_registration_complete(void);

// Authentication flow
void amf_handle_authentication_response(void);
void amf_handle_authentication_failure(void);

// Security mode flow
void amf_handle_security_mode_complete(void);
void amf_handle_security_mode_reject(void);

// Session management
void amf_handle_pdu_session_establishment_request(void);
void amf_handle_pdu_session_modification_request(void);
```

### SMF (Session Management Function)

*Status: Placeholder implementation*

The SMF will handle:
- PDU session lifecycle management
- IP address allocation
- QoS enforcement
- User plane selection

### UDM (Unified Data Management)

*Status: Placeholder implementation*

The UDM will provide:
- Subscriber data management
- Authentication data generation
- Registration data handling
- Policy data retrieval

### AUSF (Authentication Server Function)

*Status: Placeholder implementation*

The AUSF will implement:
- Authentication vector generation
- SUPI de-concealment
- Authentication result processing
- Key agreement procedures

## API Reference

### Actor Registration

```c
void amf_actor_init(void);
void smf_actor_init(void);
void udm_actor_init(void);
void ausf_actor_init(void);
```

Each init function:
- Registers event handlers with the runtime
- Initializes actor-specific resources
- Sets up internal state

### Event Handler Pattern

Event handlers follow a standardized pattern for consistency and maintainability. 
For detailed handler implementation guidelines and best practices, see the 
[Runtime Module documentation](../runtime/README.md#handler-best-practices).

Key handler requirements:
- Access event data via `EVENT_PAYLOAD` macro
- Process according to 3GPP specifications  
- Modify payload in-place for responses
- Use `trigger_event()` for follow-up events
- Keep execution time under 1ms

### Message Generation

The AMF provides utilities for generating NAS messages:
```c
// Registration messages
void generate_registration_accept(char* output, const char* guti, 
                                 const char* tai_list);
void generate_registration_reject(char* output, uint8_t cause);

// Authentication messages  
void generate_authentication_request(char* output, const char* rand,
                                   const char* autn);
void generate_authentication_reject(char* output);

// Security messages
void generate_security_mode_command(char* output, uint8_t algorithms,
                                   const char* replayed_ue_capability);

// Session messages
void generate_pdu_session_establishment_accept(char* output, 
                                              const session_params_t* params);
```

## Usage Examples

### Adding a New Event Handler

```c
// 1. Define the handler
void amf_handle_service_request(void) {
    char* nas_pdu = EVENT_PAYLOAD;
    
    // Parse service request
    service_request_t req;
    parse_service_request(nas_pdu, &req);
    
    // Verify security context
    if (!verify_security_context(req.ksi)) {
        generate_service_reject(EVENT_PAYLOAD, CAUSE_SECURITY_FAILURE);
        return;
    }
    
    // Generate service accept
    generate_service_accept(EVENT_PAYLOAD, req.tmsi);
}

// 2. Register in amf_actor_init()
void amf_actor_init(void) {
    // ... existing registrations ...
    register_event_handler(EVENT_NAS_SERVICE_REQUEST, 
                          amf_handle_service_request);
}
```

### Implementing Actor Communication

```c
// AMF requests authentication from AUSF
void amf_request_authentication(const char* supi) {
    char request[256];
    snprintf(request, sizeof(request), "AUTH_REQ:%s", supi);
    trigger_event(EVENT_AUSF_AUTH_REQUEST, request);
}

// AUSF responds with authentication vectors
void ausf_handle_auth_request(void) {
    char* request = EVENT_PAYLOAD;
    char supi[64];
    
    // Parse request
    sscanf(request, "AUTH_REQ:%s", supi);
    
    // Generate auth vectors
    auth_vector_t av;
    generate_auth_vector(supi, &av);
    
    // Format response
    snprintf(EVENT_PAYLOAD, MAX_PAYLOAD_SIZE, 
             "AUTH_RESP:%s:%s:%s", av.rand, av.autn, av.xres);
}
```

## Testing

### Unit Testing

Each actor can be tested independently:
```c
// Test registration handling
void test_amf_registration(void) {
    // Set up test event
    strcpy(EVENT_PAYLOAD, "7e004179000d0101f11111111100000001");
    
    // Call handler
    amf_handle_registration_request();
    
    // Verify response
    assert(strstr(EVENT_PAYLOAD, "7e00") != NULL); // Check for NAS header
}
```

### Integration Testing

Test actor interactions:
```c
// Test full authentication flow
void test_authentication_flow(void) {
    // 1. AMF receives registration
    trigger_event(EVENT_NAS_REGISTRATION_REQUEST, reg_pdu);
    process_events();
    
    // 2. Verify AMF triggered auth request
    assert(last_event_id == EVENT_AUSF_AUTH_REQUEST);
    
    // 3. AUSF responds
    trigger_event(EVENT_AUSF_AUTH_RESPONSE, auth_vectors);
    process_events();
    
    // 4. Verify authentication request sent to UE
    assert(last_event_id == EVENT_NAS_AUTHENTICATION_REQUEST);
}
```

## Implementation Notes

### Thread Safety
- Actors run in a single-threaded environment
- No locking required within actors
- Event processing is sequential

### Memory Management
- Use EVENT_PAYLOAD for input/output (pre-allocated)
- Avoid dynamic allocation in handlers
- Temporary buffers should be stack-allocated

### Error Handling
- Always validate input NAS PDUs
- Generate appropriate reject messages for errors
- Log errors for debugging
- Never crash on invalid input

### Performance Considerations
- Keep handlers lightweight (< 1ms execution)
- Avoid blocking operations
- Use event chaining for multi-step procedures
- Profile hot paths for optimization

## Future Enhancements

### Planned Features
- Stateful session management
- Timer-based procedures
- Paging and idle mode handling
- Network slicing support
- 5G-AKA authentication
- Service-based interface adapters

### Architecture Evolution
- Actor pooling for scalability
- Distributed actor deployment
- Inter-NF secure communication
- Configuration management
- Metrics and monitoring

## See Also

- [Core5G Overview](../overview.md) - System architecture and component overview
- [Event System](../event_system/README.md) - Event definitions and handling
- [Runtime Module](../runtime/README.md) - Handler registration and execution
- [Mailbox System](../mailbox/README.md) - Inter-actor communication mechanisms