# NFLambda Multi-User Framework Implementation Plan

## Overview
This plan outlines the development of a multi-user framework for NFLambda that utilizes pre-allocated memory and implements context matching rules for groups of network functions/event handlers.

## Implementation Plan

### 1. Create User Context Management Infrastructure
- Create a new module `src/nflambda/context/user_context.h/c` for user context management
- Define structures for:
  - User context (storing user-specific state)
  - Matching rules (criteria for context allocation)
  - Handler groups (collections of handlers with shared context rules)

### 2. Extend Event Handler Registration
- Modify the handler registration system to support handler groups
- Add a new registration function: `register_handler_group(group_id, matching_rule, handlers[])`
- Maintain a mapping between event IDs and their associated handler groups

### 3. Implement Context Matching and Allocation
- Create a context manager that:
  - Evaluates matching rules against incoming events
  - Allocates new contexts when needed
  - Retrieves existing contexts for returning users
  - Manages context lifecycle (creation, updates, cleanup)

### 4. Modify Event Processing Flow
- Update `process_events()` in runtime.c to:
  - Check if the event's handler belongs to a group with matching rules
  - Apply matching rules to determine/allocate user context
  - Set the context as thread-local or pass it to handlers
  - Clean up context references after handler execution

### 5. Define Matching Rule Types
- Implement common matching rule types:
  - By UE identifier (IMSI, SUPI, etc.)
  - By session ID
  - By source IP/port
  - By custom payload fields
  - Composite rules (AND/OR combinations)

### 6. Update Network Function Handlers
- Modify existing NF handlers to use dynamic user context instead of static variables
- Update the AMF, SMF, and other NFs to support multiple concurrent users
- Ensure thread-safety for concurrent event processing

### 7. Add Configuration Support
- Create configuration structures for defining:
  - Handler groups
  - Matching rules per group
  - Context allocation policies
  - Context timeout/cleanup policies

## Key Design Principles
- Pre-allocated memory pools for user contexts
- No re-matching required while event stays within a handler group
- Each handler group can specify its own matching rules
- Multiple handlers can share the same user context within a group
- Framework remains generic and not specific to 5G network functions