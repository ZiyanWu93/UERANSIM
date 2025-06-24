# NFLambda 5G Core

NFLambda-based 5G Core implementation that integrates with UERANSIM via IPC.

## Overview

This application provides an event-driven 5G Core (currently AMF functionality) that communicates with UERANSIM through IPC instead of direct function calls. It leverages the NFLambda runtime for high-performance, predictable NAS message processing.

## Status

🚧 **Under Development** - See [ROADMAP.md](ROADMAP.md) for implementation plan and progress.

## Architecture

```
UERANSIM ←──IPC──→ NFLambda 5G Core
                         │
                   Event-driven AMF
                   - Registration
                   - Authentication  
                   - Security Mode
                   - PDU Sessions
```

## Features (Planned)

- Event-driven NAS message processing
- IPC-based communication with UERANSIM
- Support for multiple concurrent UEs
- Full AMF functionality from existing codebase
- High performance with predictable latency

## Building

```bash
# From UERANSIM root directory
make build
```

## Running

```bash
# Start NFLambda 5G Core
./build/nflambda_5gcore

# In another terminal, run UERANSIM with IPC mode
./build/nr-gnb -c config/free5gc-gnb.yaml --use-ipc
```

## Configuration

(To be implemented - will support configuration of IPC paths, logging, etc.)

## Documentation

- [ROADMAP.md](ROADMAP.md) - Implementation plan and progress
- [Integration Guide](#) - How to integrate with UERANSIM (coming soon)
- [API Documentation](#) - IPC protocol specification (coming soon)

## See Also

- [NFLambda Overview](../../README.md)
- [IPC System](../../event_system/IPC.md)
- [Runtime Framework](../../runtime/)