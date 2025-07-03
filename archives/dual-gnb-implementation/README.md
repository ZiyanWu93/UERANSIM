# Archived Documents - Dual gNB Implementation

This folder contains completed documentation for the dual gNB implementation project.

## Completed Documents

### 1. ROADMAP-COMPLETED.md
The complete roadmap for implementing dual gNB support in UERANSIM, enabling both SCTP-based and IPC-based (NFLambda) implementations.

**Status:** ✓ All 3 phases completed successfully
- Phase 1: Dual compilation infrastructure
- Phase 2: NGAP setup restoration for SCTP version
- Phase 3: Full registration and PDU session support

### 2. nflambda_ngap_changes-COMPLETED.md
Detailed analysis of code changes introduced for NFLambda integration, used as reference for implementing conditional compilation.

## Project Summary

The dual gNB implementation project successfully achieved:
- Two separate binaries from single source tree:
  - `nr-gnb`: Original SCTP-based communication with Open5GS/free5gc
  - `nr-gnb-nflambda`: IPC-based communication with NFLambda 5G Core
- Full backward compatibility with original UERANSIM behavior
- Clean separation of concerns using conditional compilation (`USE_NFLAMBDA` macro)

**Completion Date:** July 3, 2025