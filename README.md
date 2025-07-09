<p align="center">
  <a href="https://github.com/aligungr/UERANSIM"><img src="/.github/logo.png" width="75" title="UERANSIM"></a>
</p>
<p align="center">
<img src="https://img.shields.io/badge/UERANSIM-v3.2.7-blue" />
<img src="https://img.shields.io/badge/3GPP-R15-orange" />
<img src="https://img.shields.io/badge/License-GPL--3.0-green"/>
</p>

**UERANSIM** <small>(pronounced "ju-i ræn sɪm")</small>, is the open source state-of-the-art 5G UE and RAN (gNodeB)
simulator. UE and RAN can be considered as a 5G mobile phone and a base station in basic terms. The project can be used for
testing 5G Core Network and studying 5G System.

UERANSIM introduces the world's first open source 5G-SA UE and gNodeB implementation.

> [!IMPORTANT]
> UERANSIM is no longer actively developed by the maintainer. However community contributions are welcomed.

## Key Components

UERANSIM provides multiple executables for different use cases and architectures:

### Core Simulators

#### **nr-ue** - 5G User Equipment (Mobile Device) Simulator  
- Simulates 5G-SA UE devices (mobile phones, IoT devices, etc.)
- Supports simultaneous simulation of multiple UEs with unique IMSI identities
- Implements NAS (Non-Access Stratum) and RRC protocol layers
- Handles authentication, registration, and session management procedures
- Configurable routing and network settings via YAML configuration

#### **nr-cli** - Command Line Interface Tool
- Provides runtime control and monitoring of active gNodeB and UE instances
- Discovers running UERANSIM processes automatically
- Enables sending commands to specific nodes by name or ID
- Supports operations like status queries, procedure triggers, and configuration updates
- Essential tool for testing and debugging 5G scenarios

### gNodeB (Base Station) Implementations

UERANSIM provides three different gNodeB executables, each serving a specific purpose:

#### 1. **nr-gnb** - Traditional Implementation
- Original 5G-SA gNodeB simulator using standard network protocols
- Communicates with external 5G Core via SCTP/NGAP protocol
- Implements full RRC, NGAP, and GTP protocol stacks
- Production-ready for testing with real 5G Core networks (Open5GS, free5GC, etc.)
- Best choice for end-to-end 5G network testing

#### 2. **nr-gnb-nflambda** - Event-Driven Implementation  
- Integrates with NFLambda event-driven runtime framework
- Uses IPC (Inter-Process Communication) via Unix sockets to communicate with NFLambda-based 5G Core
- Leverages NFLambda's actor model for high-performance, cache-efficient processing
- Ideal for testing with NFLambda's experimental 5G Core implementation
- Demonstrates event-driven architecture for network functions

#### 3. **nr-gnb-direct** - Direct Call Testing
- Testing and development version for rapid prototyping
- Directly calls NAS message generation functions from the `5gcore_without_runtime` library
- Bypasses both network protocols and IPC for immediate function execution
- Useful for unit testing, debugging NAS flows, and development
- Provides fastest iteration cycle for testing 5G message sequences

## Current Status

Basic functionalities of UE and gNodeB are fully functional and ready to use. However some of the features are not complete.
More details can be found at [Feature Set](https://github.com/aligungr/UERANSIM/wiki/Feature-Set).

On the other hand, UERANSIM does not fully provide physical layer. 5G-NR radio interface is partially implemented, and simply simulated over UDP protocol.

<p align="center">
<img src="https://img.shields.io/badge/Radio%20Interface-simulated-orange" alt="OS Linux"/>
<img src="https://img.shields.io/badge/Control%20Plane-functional-green" alt="OS Linux"/>  
<img src="https://img.shields.io/badge/User%20Plane-functional-green" alt="OS Linux"/>
</p>

## Documentation

- **[UERANSIM Wiki](https://github.com/aligungr/UERANSIM/wiki)** - Comprehensive user documentation
- **[CLAUDE.md](CLAUDE.md)** - Developer guidance and build instructions  
- **[NFLambda](src/nflambda/README.md)** - Event-driven runtime framework

For the latest features and updates, ensure you have the [latest release](https://github.com/aligungr/UERANSIM/releases).

## Contributing

Any contributions you make are greatly appreciated via [Pull Request](https://github.com/aligungr/UERANSIM/pulls).

## Supporting

You can support UERANSIM by:

- Starring the GitHub repository,
- Donating on [Open Collective](https://opencollective.com/UERANSIM)
- Creating pull requests, submitting bugs, suggesting new features or documentation updates.

## License

Copyright (c) 2025 ALİ GÜNGÖR.

All source code and related files including documentation and wiki pages are
dual licensed with [GPL-3.0](https://www.gnu.org/licenses/gpl-3.0.en.html) and a commercial license.

> [!WARNING]
> Closed-source commercial usage of UERANSIM is **not** permitted with the GPL-3.0. If that license is not compatable with your use case, please contact [ueransim@gmail.com](mailto:ueransim@gmail.com) to buy a commercial license.
