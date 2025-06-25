# 5G Core Without Runtime Library

## Overview

The 5G Core Without Runtime is a standalone library that provides NFLambda-compatible event handlers for 5G AMF (Access and Mobility Management Function) operations. Unlike the full runtime applications, this library focuses on providing reusable NAS (Non-Access Stratum) message generation functions that can be integrated into other NFLambda applications.

## Purpose

This library demonstrates:
- **Standalone AMF functionality** - Core 5G network functions without runtime dependencies
- **NAS message generation** - Complete registration and PDU session establishment flows
- **Reusable event handlers** - Functions that can be integrated into larger applications
- **Protocol compliance** - 3GPP-compliant NAS message construction

## Architecture

### Components

1. **AMF Event Handlers** (`amf.c/h`)
   - Five core NAS message generators following the complete 5G registration flow
   - Each handler implements a specific stage of UE registration
   - Uses NFLambda event system for integration

### NAS Message Flow

The library implements the complete 5G UE registration sequence:

```
UE Registration Flow:
┌─────────────────────────┐    ┌─────────────────────────┐
│    Registration         │───▶│   Authentication        │
│    Request              │    │   Request               │
└─────────────────────────┘    └─────────────────────────┘
                                           │
┌─────────────────────────┐    ┌─────────────────────────┐
│    Security Mode        │◀───│   Authentication        │
│    Command              │    │   Response              │
└─────────────────────────┘    └─────────────────────────┘
           │
┌─────────────────────────┐    ┌─────────────────────────┐
│    Registration         │◀───│   Security Mode         │
│    Accept               │    │   Complete              │
└─────────────────────────┘    └─────────────────────────┘
           │
┌─────────────────────────┐    ┌─────────────────────────┐
│    Configuration        │◀───│   Registration          │
│    Update Command       │    │   Complete              │
└─────────────────────────┘    └─────────────────────────┘
           │
┌─────────────────────────┐    ┌─────────────────────────┐
│    PDU Session          │◀───│   PDU Session           │
│    Establishment Accept │    │   Establishment Request │
└─────────────────────────┘    └─────────────────────────┘
```

## Event Handlers

### 1. `generate_auth_req`
**Purpose**: Generates Authentication Request NAS message  
**Input**: `7e004179000d0199f907...` (Initial Registration Request)  
**Output**: `7e005600020000215ca0df8c...` (Authentication Request)

Implements the AMF authentication initiation flow including:
- SUCI decryption via UDM
- 5G-AKA vector generation
- RAND/AUTN construction

### 2. `generate_security_cmd`
**Purpose**: Generates Security Mode Command NAS message  
**Input**: `7e00572d10ef2770c69e...` (Authentication Response)  
**Output**: `7e0313bf995a007e005d02...` (Security Mode Command)

Handles post-authentication security setup:
- RES* verification
- KAMF derivation
- Algorithm selection (EIA2 integrity, no ciphering)
- Security container MAC calculation

### 3. `generate_registration_accept`
**Purpose**: Generates Registration Accept NAS message  
**Input**: `7e0422e4ee19007e005e77...` (Security Mode Complete)  
**Output**: `7e027239674c017e004201...` (Registration Accept)

Completes registration with:
- 5G-GUTI allocation
- TAI list construction
- Allowed NSSAI configuration
- Timer assignments

### 4. `generate_configuration_update`
**Purpose**: Generates Configuration Update Command NAS message  
**Input**: `7e02469d6a8b017e0043` (Registration Complete)  
**Output**: `7e02de0d22e3027e0054430f...` (Configuration Update Command)

Provides network configuration:
- Network name IEs (Full: "Open5GS", Short: "Next")
- Local time zone information
- Daylight saving time parameters

### 5. `generate_pdu_session_establishment`
**Purpose**: Generates PDU Session Establishment Accept message  
**Input**: `7e02ba0292cd027e00670100...` (PDU Session Establishment Request)  
**Output**: `7e02fbd62d81037e00680100...` (PDU Session Establishment Accept)

Establishes data connectivity:
- QoS rule authorization
- Session-AMBR configuration
- IPv4 address assignment (10.45.0.2)
- DNS server configuration (8.8.8.8, 8.8.4.4)
- DNN "internet" binding

## Integration

This library is designed to be integrated into NFLambda applications:

```c
#include "amf.h"

// Register the handlers with your application
void my_app_register_handlers(void) {
    register_event_handler(EVENT_GENERATE_AUTH_REQ, generate_auth_req);
    register_event_handler(EVENT_GENERATE_SECURITY_CMD, generate_security_cmd);
    register_event_handler(EVENT_GENERATE_REG_ACCEPT, generate_registration_accept);
    register_event_handler(EVENT_GENERATE_CONFIG_UPDATE, generate_configuration_update);
    register_event_handler(EVENT_GENERATE_PDU_SESSION, generate_pdu_session_establishment);
}
```

## Building

The library is built as part of the NFLambda project:

```bash
# From UERANSIM root directory
make build
```

The library (`lib5gcore_without_runtime_lib.a`) is created in the build system and can be linked into other applications.

## Technical Details

### Message Construction
- **Format**: Hexadecimal strings representing binary NAS PDUs
- **Security**: Implements integrity protection and optional ciphering
- **Compliance**: Follows 3GPP TS 24.501 NAS protocol specifications

### Memory Management
- Uses `EVENT_PAYLOAD` buffer for output (max `MAX_EVENT_PAYLOAD_SIZE`)
- Safe string concatenation with overflow protection
- No dynamic memory allocation

### Protocol Features
- **Security**: MAC calculation, sequence number management
- **Mobility**: TAI lists, GUTI allocation, slice selection
- **Session**: QoS rules, AMBR, PDU session parameters
- **Configuration**: Network names, time zones, policy

## Differences from Runtime Applications

| Aspect | 5gcore_without_runtime | Runtime Applications |
|--------|------------------------|---------------------|
| **Runtime** | No runtime dependency | Full NFLambda runtime |
| **IPC** | No IPC integration | IPC event sources |
| **Actors** | Library functions only | Complete actor implementations |
| **Usage** | Link as library | Standalone executables |

## Use Cases

This library is ideal for:
- **Testing NAS message generation** without full runtime overhead
- **Protocol validation** in other 5G applications
- **Education** - understanding 5G AMF message flows
- **Integration** into custom NFLambda applications

## See Also

- **[NFLambda 5G Core](../nflambda_5gcore/)** - Full runtime implementation with IPC
- **[Simulated 5G Core](../simulated_5g_core_with_runtime/)** - Actor-based demonstration
- **[Event System](../../event_system/)** - NFLambda event handling framework
- **[Runtime Module](../../runtime/)** - NFLambda runtime documentation
- **[NFLambda Overview](../../README.md)** - System architecture and concepts