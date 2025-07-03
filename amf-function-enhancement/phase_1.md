# Phase 1: Authentication Request Implementation Plan

## Objective
Replace hardcoded `generate_auth_req` with actual authentication logic

## Steps

### 1. Message Analysis
- Capture Registration Request and Authentication Request in Wireshark
- Document message structures and field positions
- Identify required transformations

### 2. Research Open5GS Implementation
- Study AMF authentication handling
- Review UDM/AUSF interactions
- Identify cryptographic operations
- Review relevant 3GPP specifications

### 3. Design Function Chain
- Define function sequence
- Design data structures
- Plan state management
- Create interface specifications

### 4. Implementation
- Implement Milenage algorithms
- Implement SUCI to SUPI conversion
- Implement key derivation functions
- Build authentication request message
- Integrate with existing event handler

## Deliverables
- Message analysis documentation
- Function chain design
- Working implementation
- Test results