# gNB Modifications for NFLambda 5G Core Integration

## Central Goal

**Restore the original SCTP-based gNB implementation while preserving the NFLambda IPC-based version, enabling both to coexist in a single source tree.**

This allows building two separate binaries:
- `nr-gnb` - Original SCTP-based communication with Open5GS/free5gc
- `nr-gnb-nflambda` - Modified IPC-based communication with NFLambda 5G Core

## Current State

The gNB code has been modified to work with NFLambda 5G Core, replacing SCTP with IPC. These modifications need to be separated from the original code to support both implementations.

### Architecture Difference
```
Original:  UE <--RRC--> gNB <--NGAP/SCTP--> AMF (Open5GS/free5gc)
Modified:  UE <--RRC--> gNB <--IPC--> NFLambda 5G Core
```

## High-Level Modifications

### 1. **Bypassed SCTP Communication**
- SCTP task disabled (`src/gnb/sctp/task.cpp`)
- NGAP setup hardcoded instead of negotiated
- Direct state transitions without network handshake

### 2. **IPC Integration for NAS Messages**
- NAS messages routed through Unix domain socket (`/tmp/nflambda_5gcore.sock`)
- Synchronous request/response pattern
- Two delivery mechanisms available:
  - IPC-based: `deliverDownlinkNasViaIpc()` 
  - Direct function calls: `deliverDownlinkNasRefactored()`

### 3. **Message Flow Control**
- Limited to 5 message exchanges (Registration → PDU Session)
- Hardcoded delays between messages (300-500ms)
- Message sequence tracked by global counter

### 4. **Hardcoded Configuration**
- AMF identity and parameters fixed (AMF ID: 2, PLMN: 999/0)
- Timestamp fixed to 2025-06-08
- No dynamic negotiation

## Modified Files Summary

### Build System
- `CMakeLists.txt`: Added NFLambda dependencies
- `src/gnb/CMakeLists.txt`: Linked IPC and event system libraries

### Core Changes
- `src/gnb/ngap/interface.cpp`: Hardcoded NG Setup response
- `src/gnb/ngap/nas.cpp`: Implemented IPC client, message storage
- `src/gnb/ngap/task.cpp`: Auto-initialization on startup
- `src/gnb/ngap/transport.cpp`: Fixed timestamps
- `src/gnb/sctp/task.cpp`: Disabled SCTP loop

## Implementation Plan

### Step 1: Restore Original Files
```bash
# Get original implementations from master branch
git checkout master -- src/gnb/ngap/nas.cpp
git checkout master -- src/gnb/ngap/interface.cpp
git checkout master -- src/gnb/sctp/task.cpp
# Save as _standard versions
```

### Step 2: Create Separate Implementation Files
```
src/gnb/
├── ngap/
│   ├── nas_standard.cpp     # Original SCTP implementation
│   ├── nas_nflambda.cpp     # Current IPC implementation
│   ├── interface_standard.cpp # Original NGAP setup
│   ├── interface_nflambda.cpp # Hardcoded setup
│   └── ...
└── sctp/
    ├── task_standard.cpp    # Original active SCTP
    └── task_nflambda.cpp    # Disabled SCTP stub
```

### Step 3: Update Build System
Modify CMakeLists.txt to create two separate libraries and executables:
- `gnb-standard` library → `nr-gnb` executable
- `gnb-nflambda` library → `nr-gnb-nflambda` executable

## Usage

### Standard Version
```bash
./build/nr-gnb -c config/open5gs-gnb.yaml
```

### NFLambda Version
```bash
# Start NFLambda 5G Core first
./build/runtime

# Then start gNB
./build/nr-gnb-nflambda -c config/open5gs-gnb.yaml
```

## Current Workaround

To temporarily restore original SCTP-based operation:
```bash
git checkout master -- src/gnb/ CMakeLists.txt
make clean && make build
```

Note: This loses the NFLambda modifications. The implementation plan above solves this by maintaining both versions.

## Key Differences

| Aspect | Standard | NFLambda |
|--------|----------|----------|
| Protocol | NGAP/SCTP | IPC |
| Connection | Real network | Simulated |
| Configuration | Dynamic | Hardcoded |
| Message limit | Unlimited | 5 exchanges |
| Error handling | Full | Limited |

## Implementation Notes

- Message sequence controlled by `times` counter (1-5)
- IPC connection maintained across messages with auto-reconnect
- Uplink NAS PDUs stored for each protocol stage
- Duplicate GUAMI configuration in `receiveNgSetupResponse_base_case()` needs cleanup