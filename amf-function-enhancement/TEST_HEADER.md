# High Level Plan

Make sure the transformation can be done from the input message to the output message
previously

All the programming should be in the amf-function-enhancement folder, not changing anywhere else.

The scheduling can be simplified. Just a master function call the functions in the chain in sequence.


# scatter the hard-coding into different functions

problem: right now the hard-coding is centralized in the current implementation

solution: scatter the hard-coding into different functions based on the fields that they are responsible for

verification: after the transformation, the hard-coding output of the original implementation should be the same as the output of the our implementation


# verfication strategy

- Generate output from the new scattered functions
- Compare byte-by-byte with original hardcoded implementation
- Any deviation indicates incorrect transformation
- Feed the same input message to both implementations
- Verify identical output messages are produced
- Test with the provided test vectors in each phase

## step 1: phase1 functinon. (see ROADMAP-phase1.md) ✓ COMPLETED
## step 2: phase2 function. (see ROADMAP-phase2.md) ✓ COMPLETED
## step 3: phase3 function. (see ROADMAP-phase3.md) ✓ COMPLETED
## step 4: phase4 function. (see ROADMAP-phase4.md) ✓ COMPLETED
## step 5: phase5 function. (see ROADMAP-phase5.md)

# Implementation Guidelines (from Phase 1 & 2 Experience)

## File Structure
- All implementation files go in `service_function_chain/` folder
- Name pattern: `phaseX_handlers.c` for implementation
- Test files: `test_phaseX.c` and `Makefile.phaseX` in same folder

## Implementation Pattern
1. **Event Handlers**: Use EVENT_HANDLER macro for all handler functions
   - Functions operate on global variables
   - EVENT_HANDLER macro expands to `void function_name()`
   - Include event.h: `#include "../../src/nflambda/event_system/event.h"`

2. **Global Variables**: 
   ```c
   static uint8_t g_input_buffer[256];
   static uint8_t g_output_buffer[256];
   static size_t g_input_len;
   static size_t g_output_len;
   ```

3. **Working with Structures**:
   - Cast buffers to message structures: `(RegistrationRequest *)g_input_buffer`
   - Use existing header structures from `include/` folder
   - **Important**: Verify header structures match ROADMAP binary layouts
   - If mismatch found, update the header structure to match actual binary format
   - Always use structure fields instead of pointer arithmetic

4. **Binary Operations**:
   - Focus on byte-level operations
   - Use memcpy for copying binary data
   - Hardcode values directly from ROADMAP specifications

## Implementation Steps
1. Review ROADMAP-phaseX.md for exact binary structure
2. Check if header structures match the binary layout
3. Update header files if necessary to match actual format
4. Create handler functions matching header file signatures
5. Implement sequential processing chain
6. Use helper functions for initialization and output retrieval
7. Create `phaseX_transform()` entry point for testing
8. Build and test immediately to catch structure mismatches

## Testing Pattern
```c
// Input from phase_X/<message>.hex
uint8_t input[] = { /* hex values */ };

// Expected output from phase_X/<message>.hex  
uint8_t expected[] = { /* hex values */ };

// Run transformation
int result = phaseX_transform(input, input_len, output, &output_len);

// Verify byte-by-byte match
if (memcmp(output, expected, expected_len) != 0) {
    // Print differences
}
```

## Common Issues and Solutions

### Phase 2 Specific Issues
1. **SecurityModeCommand structure mismatch**: Original header had bit fields and nested structures that didn't match binary layout
   - Solution: Simplified to direct byte fields matching actual message format
   
2. **Algorithm encoding**: Expected separate nibbles but actual format uses full bytes
   - Solution: Used `selected_algo_byte1` and `selected_algo_byte2` instead of bit fields

3. **MAC byte ordering**: uint32_t MAC field needs correct endianness
   - Solution: Store as `0x5a99bf13` to get `0x13bf995a` in buffer

4. **IMEISV request format**: Expected separate IEI and value fields, actual uses combined byte
   - Solution: Single `imeisv_request_combined` field with value `0xE1`

## Build Configuration
- Include paths: `-I.. -I../../../src/nflambda/event_system`
- Compile with: `gcc -Wall -Wextra -g`
- No external libraries needed

## Key Lessons
1. Keep implementations simple - just hardcode values as specified
2. Follow the exact handler chain from ROADMAP
3. Verify output matches expected hex values exactly
4. Use global variables for state between handlers
5. Test immediately after implementation
6. **Structure Alignment**: Check ROADMAP's binary structure definitions against header files
7. **MAC Handling**: For security headers, MAC is stored as uint32_t in little-endian
8. **Binary Layout**: Some fields may be combined differently than expected (e.g., IMEISV request as single byte)
9. **Algorithm Encoding**: Selected algorithms may use different byte ordering than structure suggests
10. **Debugging**: When output doesn't match, compare byte-by-byte with expected hex

### Phase 3 Specific Issues
1. **TAC Encoding**: Expected 2-byte TAC (uint16_t) but actual encoding uses 3 bytes (0x00:0x00:0x01)
   - Solution: Define TAC as 3-byte array in binary structure
   
2. **GUTI Length Field**: Original header had uint16_t length, but actual format has separate spare half-octet and length byte
   - Solution: Split into `guti_spare_half` (0x00) and `guti_length` (0x0B) fields
   
3. **Complex Structure Parsing**: Variable position IEs (IMEISV, NAS container) require careful pointer management
   - Solution: Create custom binary structure matching exact layout for output, parse input dynamically
   
4. **Structure-Based Implementation**: Always use structure fields instead of pointer arithmetic
   - Solution: Define complete binary structure (phase3_structures.h) matching exact byte layout

5. **Function Responsibility Distribution**: Each function must handle only its designated fields per ROADMAP
   - `amf_allocate_5g_guti()`: Sets GUTI-related fields only
   - `amf_determine_tai_list()`: Sets TAI list fields only
   - `amf_validate_network_slices()`: Sets NSSAI fields only
   - `amf_build_registration_accept()`: Sets headers, result, network features, and timer
   - `amf_apply_nas_security()`: Sets MAC field
   - Solution: Carefully follow ROADMAP function specifications for field assignments
