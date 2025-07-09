# Direct Function Call Integration Plan

## Step 1: Create new handler files in 5gcore_without_runtime
Copy the individual SFC handlers from amf-function-enhancement and create new files (amf_handlers.c, ausf_handlers.c, udm_handlers.c) in src/nflambda/app/5gcore_without_runtime/ - initially as direct copies

## Step 2: Adapt handlers for EVENT_PAYLOAD
Modify the copied handlers to work directly with event_nf_ptr->input_payload and output_payload instead of buffer parameters, ensuring they read/write in the same format as original amf.c

## Step 3: Update existing amf.c
Replace the monolithic implementations in amf.c with calls to the new granular handlers, maintaining the same external API (generate_auth_req, generate_security_cmd, etc.)

## Step 4: Update build configuration
Modify CMakeLists.txt to include the new handler files (amf_handlers.c, ausf_handlers.c, udm_handlers.c) in the 5gcore_without_runtime_lib build

## Step 5: Validate backward compatibility
Test that the refactored amf.c produces identical EVENT_PAYLOAD outputs as the original implementation for all message types

Note: The phase handlers in amf-function-enhancement remain as development/testing reference only, not used in production code.