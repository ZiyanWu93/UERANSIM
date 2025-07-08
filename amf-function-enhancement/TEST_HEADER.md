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
## step 2: phase2 function. (see ROADMAP-phase2.md)
## step 3: phase3 function. (see ROADMAP-phase3.md)
## step 4: phase4 function. (see ROADMAP-phase4.md)
## step 5: phase5 function. (see ROADMAP-phase5.md)

# Implementation Guidelines (from Phase 1 Experience)

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
   - No custom structures needed

4. **Binary Operations**:
   - Focus on byte-level operations
   - Use memcpy for copying binary data
   - Hardcode values directly from ROADMAP specifications

## Implementation Steps
1. Create handler functions matching header file signatures
2. Implement sequential processing chain
3. Use helper functions for initialization and output retrieval
4. Create `phaseX_transform()` entry point for testing

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
