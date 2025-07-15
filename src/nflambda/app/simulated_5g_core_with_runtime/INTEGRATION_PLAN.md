# Multi-Network Function 5G Core Integration Plan

## Overview

This plan outlines the step-by-step integration of the AMF function enhancement service chains into the existing simulated 5G core runtime application, transforming it from a monolithic AMF implementation to a multi-network function 5G core system.

## Current State Analysis

### Existing Implementation
- **Location**: `src/nflambda/app/simulated_5g_core_with_runtime/`
- **Architecture**: Two-actor system (fiveg_core_actor + ueransim_actor)
- **Message Processing**: Hardcoded NAS PDU string concatenation
- **Event Flow**: Direct string-based event processing with EVENT_PAYLOAD

### Enhancement Materials
- **Location**: `amf-function-enhancement/`
- **Architecture**: Granular service function chains across 5 phases
- **Message Processing**: Structured binary message handling with proper header definitions
- **Event Flow**: Modular handler chains with proper state management

## Integration Goals

1. **Modular Network Functions**: Replace monolithic AMF with separate AMF, AUSF, UDM, SMF, and PCF functions
2. **Service Function Chains**: Implement the 5-phase service function chain architecture
3. **Runtime Integration**: Maintain NFLambda runtime event-driven architecture
4. **Backward Compatibility**: Preserve existing event IDs and message flow patterns

## Implementation Steps

### Step 1: Create Network Function Actor Infrastructure
**Objective**: Establish separate actors for each network function. Add function definitions/declarations with no implementation
**Verification**: Each network function actor can be compiledd into the simulator without no alternation of the execution outcome.

**Tasks**:
1.1. Create `amf_actor.c/h` - AMF (Access and Mobility Management Function)
1.2. Create `ausf_actor.c/h` - AUSF (Authentication Server Function)  
1.3. Create `udm_actor.c/h` - UDM (Unified Data Management)
1.4. Create `smf_actor.c/h` - SMF (Session Management Function)
1.5. Create `pcf_actor.c/h` - PCF (Policy Control Function)
1.6. Update `amf_common.h` with new inter-NF event definitions
1.7. Update `CMakeLists.txt` to include new actor files
1.8. Add verifucation at UE side so that the 

### Step 2: Implement Phase 1 Service Function Chain
**Objective**: Replace hardcoded authentication request with modular phase 1 handlers
**Verification**: Registration Request → Authentication Request produces identical output

**Tasks**:
2.1. Copy phase 1 handlers from `amf-function-enhancement/service_function_chain/phase1_handlers.c`
2.2. Adapt handlers to work with EVENT_PAYLOAD instead of buffer parameters
2.3. Integrate handlers into AMF actor's `handle_registration_request` function
2.4. Add inter-NF communication between AMF → AUSF → UDM for authentication vectors
2.5. Update message event triggers to use service function chain

### Step 3: Implement Phase 2 Service Function Chain  
**Objective**: Replace hardcoded security mode command with modular phase 2 handlers
**Verification**: Authentication Response → Security Mode Command produces identical output

**Tasks**:
3.1. Copy phase 2 handlers from `amf-function-enhancement/service_function_chain/phase2_handlers.c`
3.2. Adapt handlers for EVENT_PAYLOAD format
3.3. Integrate key derivation and security algorithm selection
3.4. Add AUSF authentication verification logic
3.5. Update `handle_authentication_response` in AMF actor

### Step 4: Implement Phase 3 Service Function Chain
**Objective**: Replace hardcoded registration accept with modular phase 3 handlers  
**Verification**: Security Mode Complete → Registration Accept produces identical output

**Tasks**:
4.1. Copy phase 3 handlers from `amf-function-enhancement/service_function_chain/phase3_handlers.c`
4.2. Implement GUTI allocation in AMF actor
4.3. Add UDM subscription data retrieval
4.4. Integrate security context activation
4.5. Update `handle_security_mode_complete` in AMF actor

### Step 5: Implement Phase 4 Service Function Chain
**Objective**: Replace hardcoded configuration update with modular phase 4 handlers
**Verification**: Registration Complete → Configuration Update produces identical output

**Tasks**:
5.1. Copy phase 4 handlers from `amf-function-enhancement/service_function_chain/phase4_handlers.c`
5.2. Implement configuration parameter encoding
5.3. Add PCF policy retrieval for configuration updates
5.4. Update `handle_registration_complete` in AMF actor

### Step 6: Implement Phase 5 Service Function Chain
**Objective**: Replace hardcoded PDU session establishment with modular phase 5 handlers
**Verification**: PDU Session Request → PDU Session Accept produces identical output

**Tasks**:
6.1. Copy phase 5 handlers from `amf-function-enhancement/service_function_chain/phase5_handlers.c`
6.2. Implement SMF actor for session management
6.3. Add UPF selection logic in SMF
6.4. Integrate QoS flow creation
6.5. Update `handle_pdu_session_request` in AMF actor

### Step 7: Implement Inter-NF Communication
**Objective**: Enable proper communication between network function actors
**Verification**: All network functions can send/receive events and maintain state

**Tasks**:
7.1. Define inter-NF event types (AMF_TO_AUSF, AUSF_TO_UDM, etc.)
7.2. Implement message routing between actors
7.3. Add proper event payload marshalling/unmarshalling
7.4. Create shared context management for UE state across NFs
7.5. Add proper error handling for inter-NF communication failures

### Step 8: Update Runtime Integration
**Objective**: Maintain seamless integration with NFLambda runtime
**Verification**: All events are properly handled by runtime and statistics are correct

**Tasks**:
8.1. Update `amf_main.c` to initialize all network function actors
8.2. Register all new actors with runtime system
8.3. Update event registration for new inter-NF events
8.4. Maintain existing external event IDs for UERANSIM compatibility
8.5. Add runtime statistics for multi-NF processing

### Step 9: Testing and Validation
**Objective**: Ensure all functionality works correctly and produces expected outputs
**Verification**: Complete end-to-end flow produces identical results to original implementation

**Tasks**:
9.1. Create unit tests for each network function actor
9.2. Test individual service function chains
9.3. Test complete end-to-end registration flow
9.4. Test PDU session establishment flow
9.5. Validate runtime performance metrics
9.6. Test error scenarios and recovery

### Step 10: Documentation and Cleanup
**Objective**: Ensure code is well-documented and maintainable
**Verification**: All code is documented and follows project conventions

**Tasks**:
10.1. Update README.md with new multi-NF architecture
10.2. Add code documentation for all new actors and handlers
10.3. Create architecture diagrams showing inter-NF communication
10.4. Clean up any temporary/development code
10.5. Update build instructions and examples

## Event Flow Architecture

### Current Event Flow
```
UE → AMF (monolithic) → UE
```

### Target Event Flow  
```
UE → AMF → AUSF → UDM → AMF → UE
UE → AMF → SMF → UPF → PCF → SMF → AMF → UE
```

## File Structure After Integration

```
src/nflambda/app/simulated_5g_core_with_runtime/
├── CMakeLists.txt
├── README.md
├── INTEGRATION_PLAN.md (this file)
├── amf_common.h (updated with inter-NF events)
├── amf_main.c (updated for multi-NF initialization)
├── amf_actor.c/h (core AMF functionality)
├── ausf_actor.c/h (authentication server)
├── udm_actor.c/h (unified data management)
├── smf_actor.c/h (session management)
├── pcf_actor.c/h (policy control)
├── ueransim_actor.c/h (existing UE simulator)
└── handlers/
    ├── phase1_handlers.c (registration → authentication)
    ├── phase2_handlers.c (authentication → security)
    ├── phase3_handlers.c (security → registration accept)
    ├── phase4_handlers.c (registration complete → config update)
    └── phase5_handlers.c (PDU session establishment)
```

## Success Criteria

1. **Functional**: All existing test cases pass with identical outputs
2. **Performance**: Runtime performance is maintained or improved
3. **Maintainable**: Code is modular and follows project conventions
4. **Extensible**: New network functions can be easily added
5. **Compatible**: Existing UERANSIM integration continues to work

## Risk Mitigation

1. **Backup**: Keep original implementation as reference
2. **Incremental**: Implement one phase at a time with validation
3. **Testing**: Comprehensive testing at each step
4. **Documentation**: Clear documentation of all changes and integration points