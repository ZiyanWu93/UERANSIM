# UERANSIM Dual gNB Implementation Roadmap

## Goal
Enable both SCTP-based (original) and IPC-based (NFLambda) gNB implementations to coexist in a single source tree, producing two separate binaries:
- `nr-gnb` - Original SCTP-based communication with Open5GS/free5gc
- `nr-gnb-nflambda` - Modified IPC-based communication with NFLambda 5G Core

## Phase 1: Dual Compilation with Identical Behavior

**Objective:** Establish separate compilation targets for SCTP and NFLambda versions while maintaining identical runtime behavior.

This phase focuses on creating the build infrastructure to produce two distinct binaries from the same codebase. Both binaries will function identically, allowing us to verify the compilation framework before introducing behavioral differences.

### TODO:
- [X] Update CMakeLists.txt to build both executables simultaneously:
  - `nr-gnb` (SCTP version)
  - `nr-gnb-nflambda` (IPC version)
- [X] Configure build system to compile both targets with single `make` command
- [X] Ensure both binaries are identical in Phase 1 (same source files, different names)
- [X] Update Makefile to include both targets in default build
- [X] Verify both binaries produce identical behavior with existing tests (By the programmers)

## Phase 2: Restore NGAP Setup for SCTP Version

**Objective:** Enable successful NGAP setup between Open5GS and the SCTP-based nr-gnb while preserving NFLambda functionality.

**Approach:** Analyze the original UERANSIM code (pre-NFLambda modifications) to identify and restore SCTP-specific NGAP handling. Use conditional compilation to separate SCTP and IPC communication paths. Define `USE_NFLAMBDA` preprocessor macro for nr-gnb-nflambda target. If there is specific implementation to the nflambda version, we need to decouple it to a separate file.

### TODO:
- [X] Analyze specific code changes of the nflambda version compared to the original version and output the change to a file called 'nflambda_ngap_changes.md' in the root directory: [nflambda_ngap_changes.md](nflambda_ngap_changes.md)
- [X] decouple the changes, enable conditional compilation of the feature between the original and nflambda version.

### Conditional Compilation Implementation Notes:

Successfully implemented conditional compilation using the `USE_NFLAMBDA` macro to separate SCTP-based (original) and IPC-based (NFLambda) implementations.

#### Key Changes:
1. **Build System**: Created two separate gnb libraries in `src/gnb/CMakeLists.txt`:
   - `gnb` library: Original version without USE_NFLAMBDA
   - `gnb-nflambda` library: NFLambda version with -DUSE_NFLAMBDA flag

2. **Source Code Modifications**:
   - `src/gnb/sctp/task.cpp`: Added `#ifdef USE_NFLAMBDA` to bypass SCTP processing in `onLoop()` and `receiveSctpConnectionSetupRequest()`
   - `src/gnb/ngap/interface.cpp`: Conditional compilation in `sendNgSetupRequest()` to switch between real NGAP and simulated response
   - `src/gnb/ngap/task.cpp`: Added `#ifdef USE_NFLAMBDA` to trigger automatic AMF connection on startup
   - `src/gnb/ngap/transport.cpp`: Conditional bypass of NGAP response processing

3. **Important Discovery**: The initial attempt failed because CMake was building a single shared library for both executables. The `-DUSE_NFLAMBDA` flag on the executable target doesn't affect the library compilation. The solution was to build two separate libraries with different compilation flags.

#### Result:
- `./build/nr-gnb`: Uses real SCTP connections and standard NGAP procedures
- `./build/nr-gnb-nflambda`: Bypasses SCTP and uses simulated AMF connection for NFLambda integration


**Expected Results:** 
- `nr-gnb-nflambda` maintains current IPC-based behavior
- `nr-gnb` successfully establishes NGAP associations with Open5GS/free5gc

## Phase 3: Restore Registration and PDU Session Establishment ✓

**Objective:** Enable complete UE registration and PDU session establishment for the SCTP version while maintaining NFLambda compatibility.

**Approach:** Continue analyzing original code to restore NAS message routing and session management for SCTP communication. Implement abstraction layers where necessary to support both communication methods.

### Completed Implementation:

Successfully restored full NAS message handling for the SCTP version through conditional compilation:

#### Key Changes:
1. **NAS Message Handling** (`src/gnb/ngap/nas.cpp`):
   - Moved `extractSliceInfoAndModifyPdu()` outside of `#ifdef USE_NFLAMBDA` block (original behavior)
   - Added conditional compilation for NAS PDU storage (NFLambda only)
   - Restored original `deliverDownlinkNas()` implementation
   - Wrapped IPC-specific functions in `#ifdef USE_NFLAMBDA`

2. **UE Context Management** (`src/gnb/ngap/management.cpp`):
   - Restored original `createUeContext(int ueId)` implementation
   - Maintained slice-aware version for backward compatibility
   - Fixed AMF selection logic for SCTP version

3. **Function Declarations** (`src/gnb/ngap/task.hpp`):
   - Conditionally declared NFLambda-specific state variables
   - Conditionally declared IPC-related methods

4. **Bug Fixes**:
   - Fixed commented out `handleAssociationSetup` call that was preventing NG Setup
   - Ensured `extractSliceInfoAndModifyPdu()` is called for both versions (matching original behavior)

**Results Achieved:**
- ✓ `nr-gnb-nflambda` continues to function with NFLambda 5G Core via IPC
- ✓ `nr-gnb` successfully completes:
  - NGAP association establishment with Open5GS/free5gc
  - Full UE registration (Initial Registration → Authentication → Security Mode → Registration Accept)
  - PDU session establishment (requires UPF to be running)

**Note:** PDU session establishment requires the UPF (User Plane Function) to be running and properly configured in the 5G Core.