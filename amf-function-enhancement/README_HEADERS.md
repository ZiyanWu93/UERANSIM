# 5G NAS Message Structure Headers

This directory contains C header files that define binary structures for 5G NAS (Non-Access Stratum) messages as specified in 3GPP TS 24.501. Each header file maps directly to the binary format of specific NAS messages.

## Header Files Overview

### Common Headers

- **nas_common.h** - Common definitions, constants, and utility functions used across all message types
  - NAS protocol constants (EPD, message types, IEIs)
  - Security header structures
  - Helper functions for hex/binary conversion and BCD encoding

### Phase 1: Initial Registration and Authentication

- **registration_request.h** - UE → AMF initial registration request
  - 5GS registration type and follow-on request
  - NAS key set identifier
  - Mobile identity (SUCI format)
  - UE security capabilities

- **authentication_request.h** - AMF → UE authentication challenge
  - NAS key set identifier (ngKSI)
  - ABBA parameter
  - RAND (16-byte random challenge)
  - AUTN (authentication token with SQN⊕AK, AMF, MAC)

### Phase 2: Authentication Response and Security Mode

- **authentication_response.h** - UE → AMF authentication response
  - RES* parameter (16 bytes)

- **security_mode_command.h** - AMF → UE security mode establishment
  - Security header with MAC and sequence number
  - Selected NAS security algorithms
  - Replayed UE security capabilities
  - IMEISV request
  - Additional 5G security information

### Phase 3: Security Mode Complete and Registration Accept

- **security_mode_complete.h** - UE → AMF security mode confirmation
  - Security protected header
  - Optional IMEISV
  - NAS message container (replayed registration request)

- **registration_accept.h** - AMF → UE registration acceptance
  - 5GS registration result
  - 5G-GUTI allocation
  - TAI list
  - Allowed NSSAI
  - Network feature support
  - T3512 timer

### Phase 4: Registration Complete and Configuration Update

- **registration_complete.h** - UE → AMF registration completion
  - Simple acknowledgment message with security header

- **configuration_update_command.h** - AMF → UE configuration update
  - Network names (full and short) in UTF-16
  - Time zone information
  - Universal time and local time zone
  - Daylight saving time

### Phase 5: PDU Session Establishment

- **pdu_session_establishment_request.h** - UE → SMF (via AMF) PDU session request
  - PDU session identity and type
  - SSC mode
  - 5GSM capabilities
  - Integrity protection maximum data rates

- **pdu_session_establishment_accept.h** - SMF → UE (via AMF) PDU session acceptance
  - Selected PDU session type and SSC mode
  - Authorized QoS rules
  - Session AMBR
  - PDU address (IPv4/IPv6)
  - 5GSM cause

## Structure Design Principles

1. **Byte Alignment**: All structures use `#pragma pack(1)` for exact byte alignment
2. **Bit Fields**: Sub-byte fields are defined using C bit fields with careful attention to endianness
3. **Network Byte Order**: Multi-byte fields use network byte order (big-endian)
4. **Variable Length**: Variable-length fields use flexible array members or are handled separately
5. **Type Safety**: Exact-width integer types (uint8_t, uint16_t, etc.) ensure portability

## Test Programs

Each header file has a corresponding test program in the `test/` directory:
- test_registration_request.c
- test_authentication_request.c
- test_authentication_response.c
- test_security_mode_command.c
- (additional tests for remaining messages)

These tests:
1. Read actual hex message data from phase directories
2. Cast binary data to C structures
3. Verify field values match expected parse trees
4. Convert back to hex to ensure round-trip accuracy

## Building and Testing

```bash
# Build all test programs
make all

# Run all tests
make test

# Run individual test
./bin/test_registration_request

# Clean build artifacts
make clean
```

## Usage Example

```c
#include "registration_request.h"

// Read binary message data
uint8_t buffer[256];
read_hex_file("message.hex", buffer, size);

// Cast to structure
RegistrationRequest *req = (RegistrationRequest *)buffer;

// Access fields
printf("Message type: 0x%02X\n", req->message_type);
printf("Registration type: %d\n", req->reg_type_ngksi.registration_type);
printf("MCC: %d%d%d\n", 
    req->mobile_identity.mcc_digit1,
    req->mobile_identity.mcc_digit2,
    req->mobile_identity.mcc_digit3);
```

## References

- 3GPP TS 24.501 - Non-Access-Stratum (NAS) protocol for 5G System (5GS)
- 3GPP TS 24.007 - Mobile radio interface signalling layer 3
- 3GPP TS 33.501 - Security architecture and procedures for 5G System