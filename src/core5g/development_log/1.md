# Goal: Implement Network Functions in the Actor Framework

## Current Status:

The event management system foundation has been implemented with:

1. **Event Structure (event.h):**
   - Defined `EventNf` struct with `event_id`, `input_payload`, and `output_payload` fields
   - Created helper macros for accessing event data (`EVENT_ID`, `EVENT_PAYLOAD`, `EVENT_OUTPUT_PAYLOAD`)
   - Set up event handler type definitions and macros

2. **AMF Event Handlers (amf.c):**
   - Implemented key 5G network function handlers:
     - `generate_auth_req`: Creates authentication request payload
     - `generate_security_cmd`: Creates security mode command payload
     - `generate_registration_accept`: Handles registration acceptance
     - `generate_configuration_update`: Creates configuration update payload
     - `generate_pdu_session_establishment`: Creates PDU session establishment payload

3. **Runtime Framework (runtime.c):**
   - Basic event routing table structure with example handlers
   - Signal handling for graceful shutdown
   - Runtime loop structure

Additionally, work has begun on:
- Modified `process_events()` to check for discarded events (event_id == -1)
- Updated `runtime()` to inject events only in first iteration and exit when all tasks complete

## Next Steps:

1. **Implement Mailbox and Event Scheduling System:**
   - Design a mailbox structure to queue events for future processing
   - Implement the `trigger_event()` function to schedule events via the mailbox system
   - Create proper synchronization mechanisms for the mailbox
   - Finish the `process_events()` function to consume events from the mailbox and use the routing table
   - Add proper event validation and error handling

2. **Enhance Runtime Loop:**
   - Update `runtime()` to properly inject events only in the first iteration
   - Implement robust exit condition when all tasks are complete
   - Add tracing/logging for event flow

3. **Extend Event Handlers:**
   - Complete any remaining network function handlers
   - Test each handler with properly formatted input payloads
   - Ensure proper error handling in all event handlers

4. **Testing and Documentation:**
   - Create unit tests for each component of the event system
   - Add integration tests for end-to-end event flow
   - Document the event system architecture and usage patterns


