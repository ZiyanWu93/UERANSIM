# AMF Function Enhancement

This directory contains the modular AMF (Access and Mobility Management Function) service function chain implementation for UERANSIM. It provides a granular, event-driven approach to processing 5G NAS (Non-Access Stratum) messages.

## Overview

The AMF function enhancement breaks down the monolithic AMF message processing into discrete, composable service functions organized in phases:

- **Phase 1**: Registration Request → Authentication Request
- **Phase 2**: Authentication Response → Security Mode Command
- **Phase 3**: Security Mode Complete → Registration Accept
- **Phase 4**: Registration Complete → Configuration Update Command

## Directory Structure

```
amf-function-enhancement/
├── bin/                    # Compiled test binaries
├── include/                # NAS message structure headers
├── service_function_chain/ # Core implementation
│   ├── phase1_handlers.c   # Registration → Authentication handlers
│   ├── phase2_handlers.c   # Authentication → Security handlers
│   ├── phase3_handlers.c   # Security → Registration Accept handlers
│   ├── phase4_handlers.c   # Registration Complete → Config handlers
│   └── test_phase*.c       # Test files for each phase
├── phase_*/               # Test data and documentation for each phase
└── test/                  # Additional unit tests
```

## Building and Testing

### Building Individual Phases

Each phase can be built independently from the `service_function_chain` directory:

```bash
cd service_function_chain

# Build Phase 1
make -f Makefile.phase1

# Build Phase 2
make -f Makefile.phase2

# Build Phase 3
make -f Makefile.phase3

# Build Phase 4
make -f Makefile.phase4
```

All binaries are output to the `bin/` directory.

### Running Tests

After building, run the test binaries from the `bin` directory:

```bash
# Test all phases
./bin/test_phase1
./bin/test_phase2
./bin/test_phase3
./bin/test_phase4

# Or run from service_function_chain directory
cd service_function_chain
make -f Makefile.phase1 test
```

### Building All Tests

To build all unit tests in the `test/` directory:

```bash
make
```

## Integration with UERANSIM

The service function chain is designed to integrate with UERANSIM in two ways:

1. **Development/Testing**: Use the phase handlers in `service_function_chain/` with buffer-based APIs
2. **Production Integration**: Follow the plan in `DIRECT_CALL_INTEGRATION_PLAN.md` to integrate with the NFLambda runtime

### Direct Function Call Integration

For integration with `nr-gnb-direct`, the handlers will be:
1. Copied to `src/nflambda/app/5gcore_without_runtime/`
2. Modified to work with `EVENT_PAYLOAD` structure
3. Called directly from the refactored `amf.c`

See `DIRECT_CALL_INTEGRATION_PLAN.md` for detailed integration steps.

## Service Function Chain Architecture

Each phase contains multiple granular handlers that process specific aspects of the NAS message flow:

### Phase 1 Handlers
- `amf_handle_registration_request()` - Parse incoming registration
- `amf_extract_suci_from_mobile_identity()` - Extract subscriber ID
- `amf_validate_registration_request()` - Validate request format
- `ausf_authenticate_request()` - Request authentication vectors
- `udm_generate_authentication_vector()` - Generate auth data
- `amf_build_authentication_request()` - Build response message
- `amf_send_authentication_request()` - Prepare for transmission

### Phase 2 Handlers
- `amf_handle_authentication_response()` - Process auth response
- `amf_verify_authentication_response()` - Verify RES value
- `ausf_confirm_authentication()` - Confirm with AUSF
- `amf_derive_security_keys()` - Derive NAS keys
- `amf_select_security_algorithms()` - Choose algorithms
- `amf_build_security_mode_command()` - Build command
- `amf_send_security_mode_command()` - Prepare transmission

### Phase 3 Handlers
- `amf_handle_security_mode_complete()` - Process completion
- `amf_verify_nas_mac()` - Verify message auth code
- `amf_activate_security_context()` - Activate NAS security
- `udm_get_subscription_data()` - Fetch subscriber data
- `amf_allocate_guti()` - Allocate temporary ID
- `amf_build_registration_accept()` - Build accept message
- `amf_send_registration_accept()` - Prepare transmission

### Phase 4 Handlers
- `amf_handle_registration_complete()` - Process completion
- `amf_confirm_guti_allocation()` - Confirm GUTI
- `amf_prepare_configuration_parameters()` - Prepare config
- `amf_build_configuration_update_command()` - Build update
- `amf_send_configuration_update_command()` - Send config

## Development Guidelines

1. **Modularity**: Each handler should perform a single, well-defined function
2. **State Management**: Use proper context structures, not global variables
3. **Error Handling**: Return appropriate error codes and handle failures
4. **Testing**: Each phase has comprehensive test coverage
5. **Documentation**: Update phase documentation when modifying handlers

## Future Enhancements

- Support for additional NAS procedures (Service Request, Deregistration)
- Integration with real AUSF/UDM interfaces
- Performance optimization for high-throughput scenarios
- Support for 5G-AKA and EAP-AKA' authentication methods