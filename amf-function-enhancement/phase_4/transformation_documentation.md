# Phase 4: Registration Complete → Configuration Update Command Transformation

## Overview
This document details how open5gs AMF transforms a Registration Complete message into a Configuration Update Command message.

## Input Message: Registration Complete
- **Message Type**: 0x43 (Registration Complete)
- **Security**: Integrity protected and ciphered
- **Key Fields**:
  - Simple acknowledgment message
  - No additional parameters

## Output Message: Configuration Update Command
- **Message Type**: 0x54 (Configuration Update Command)
- **Security**: Integrity protected and ciphered
- **Key Fields**:
  - Full network name: "Open5GS" (UTF-16 encoded)
  - Short network name: "Next" (UTF-16 encoded)
  - Local time zone: UTC+5:00 (encoded as 0x14)
  - Universal time and time zone: BCD encoded timestamp
  - Daylight saving time: +1 hour adjustment

## Network Functions Involved

### 1. AMF (Access and Mobility Management Function)
- Receives and processes registration complete
- Confirms GUTI assignment
- Builds configuration update with operator information
- Encodes network names in UTF-16
- Calculates and encodes time zone information

### 2. Configuration Sources
- **YAML Configuration**: Network names, time zone settings
- **System Time**: Current UTC and local time
- **Operator Policy**: Whether to send configuration updates

## Detailed Function Call Flow

### Step 1: Registration Complete Reception
**File**: `src/amf/gmm-sm.c`
**Function**: State machine handler (lines 2689-2719)

```c
case OGS_NAS_5GS_REGISTRATION_COMPLETE:
    ogs_info("[%s] Registration complete", amf_ue->supi);
    
    // Clear registration timer T3550
    CLEAR_AMF_UE_TIMER(amf_ue->t3550);
    
    // Confirm GUTI assignment
    if (amf_ue->next.m_tmsi) {
        amf_ue_confirm_guti(amf_ue);
    } else {
        ogs_error("[%s] No GUTI allocated", amf_ue->supi);
    }
    
    // Trigger configuration update if configured
    if (self.network_name.full || self.network_name.short || 
        !self.no_time_zone_information) {
        gmm_configuration_update_command_param_t param;
        param.nitz = 1;  // Network Identity and Time Zone
        nas_5gs_send_configuration_update_command(amf_ue, &param);
    }
```

### Step 2: GUTI Confirmation
**File**: `src/amf/context.c`
**Function**: `amf_ue_confirm_guti()` (lines 1521-1566)

```c
void amf_ue_confirm_guti(amf_ue_t *amf_ue) {
    ogs_assert(amf_ue);
    
    // Copy from next to current GUTI
    amf_ue->current.guti = amf_ue->next.guti;
    amf_ue->current.m_tmsi = amf_ue->next.m_tmsi;
    
    // Update hash table for GUTI lookup
    if (amf_ue->current.m_tmsi) {
        ogs_hash_set(self.guti_hash, 
            &amf_ue->current.guti, sizeof(ogs_nas_5gs_guti_t), amf_ue);
    }
    
    // Clear next GUTI
    amf_ue->next.m_tmsi = NULL;
    memset(&amf_ue->next.guti, 0, sizeof(ogs_nas_5gs_guti_t));
}
```

### Step 3: Configuration Update Command Building
**File**: `src/amf/gmm-build.c`
**Function**: `gmm_build_configuration_update_command()` (lines 485-625)

```c
ogs_pkbuf_t *gmm_build_configuration_update_command(
    amf_ue_t *amf_ue,
    gmm_configuration_update_command_param_t *param) {
    
    // Initialize message structure
    message.h.security_header_type = 
        OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED;
    message.gmm.h.message_type = OGS_NAS_5GS_CONFIGURATION_UPDATE_COMMAND;
    
    // Include full network name if configured
    if (amf_self()->network_name.full) {
        configuration_update_command->presencemask |= 
            OGS_NAS_5GS_CONFIGURATION_UPDATE_COMMAND_FULL_NAME_FOR_NETWORK_PRESENT;
        configuration_update_command->full_name_for_network = 
            amf_self()->network_name.full;
    }
    
    // Include short network name if configured
    if (amf_self()->network_name.short) {
        configuration_update_command->presencemask |= 
            OGS_NAS_5GS_CONFIGURATION_UPDATE_COMMAND_SHORT_NAME_FOR_NETWORK_PRESENT;
        configuration_update_command->short_name_for_network = 
            amf_self()->network_name.short;
    }
    
    // Include time zone information
    if (param && param->nitz && !amf_self()->no_time_zone_information) {
        // Current time encoding...
    }
}
```

### Step 4: Network Name Encoding (UTF-16)
**File**: `src/amf/context.c`
**Function**: Network name configuration processing (lines 965-1013)

```c
// Configure full network name from YAML
if (amf_config->network_name.full) {
    c_network_name = amf_config->network_name.full;
    
    // Allocate network name structure
    network_full_name = &self.network_name.full;
    
    // Set encoding parameters
    network_full_name->coding_scheme = 1;  // USC-2 (UTF-16)
    network_full_name->add_ci = 0;         // No country initials
    network_full_name->spare = 0;
    network_full_name->ext = 1;            // Extension bit
    
    // Convert ASCII to USC-2 (UTF-16)
    network_full_name->length = strlen(c_network_name) * 2 + 1;
    for (i = 0; i < strlen(c_network_name); i++) {
        // Insert null byte for USC-2 encoding
        network_full_name->name[i * 2] = 0;
        network_full_name->name[i * 2 + 1] = c_network_name[i];
    }
}

// Similar process for short network name
```

**UTF-16 Encoding Result**:
```
"Open5GS" → 0x00:4f:00:70:00:65:00:6e:00:35:00:47:00:53
"Next"    → 0x00:4e:00:65:00:78:00:74
```

### Step 5: Time Zone and Daylight Saving Time Calculation
**File**: `src/amf/gmm-build.c`
**Function**: Time zone encoding (lines 546-605)

```c
// Get current time in UTC and local time
struct tm utc, local;
time_t now = time(NULL);

// Convert to UTC and local time structures
ogs_gmtime(now, &utc);
ogs_localtime(now, &local);

// Calculate timezone offset (in 15-minute units)
if (local.tm_gmtoff >= 0) {
    *local_time_zone = OGS_NAS_TIME_TO_BCD(local.tm_gmtoff / 900);
} else {
    *local_time_zone = OGS_NAS_TIME_TO_BCD((-local.tm_gmtoff) / 900);
    *local_time_zone |= 0x08;  // Set sign bit for negative offset
}

// Encode current time in BCD format
ogs_nas_time_to_bcd(&local, universal_time_and_local_time_zone);

// Set daylight saving time adjustment
*daylight_saving_time = local.tm_isdst ? 1 : 0;  // +1 hour if DST active
```

**Time Zone Encoding**:
```
UTC+5:00 → offset = 5 * 60 / 15 = 20 → BCD = 0x14
DST active → daylight_saving_time = 1
```

### Step 6: Message Assembly and Transmission
**File**: `src/amf/nas-path.c`
**Function**: `nas_5gs_send_configuration_update_command()`

```c
void nas_5gs_send_configuration_update_command(
    amf_ue_t *amf_ue,
    gmm_configuration_update_command_param_t *param) {
    
    // Build the message
    ogs_pkbuf_t *gmmbuf = gmm_build_configuration_update_command(amf_ue, param);
    
    // Apply security protection
    ogs_pkbuf_t *nasbuf = nas_5gs_security_encode(amf_ue, &message);
    
    // Send to RAN
    nas_5gs_send_to_downlink_nas_transport(amf_ue, nasbuf);
    
    // Start timer if acknowledgment requested
    if (param && param->acknowledgement_requested) {
        ogs_timer_start(amf_ue->t3555.timer);
    }
}
```

## Field Transformations Summary

| Field | Registration Complete | Configuration Update Command | Transformation Logic |
|-------|----------------------|------------------------------|---------------------|
| Message Type | 0x43 | 0x54 | Changed to configuration update |
| Network Names | Not present | "Open5GS" (UTF-16) | From YAML configuration |
| Time Zone | Not present | UTC+5:00 (0x14) | Calculated from system time |
| UTC Time | Not present | BCD encoded timestamp | Current system time |
| DST | Not present | +1 hour (0x01) | From system DST status |
| Security | Protected | Protected | Maintains security context |

## Configuration Parameters

### 1. Network Name Configuration (YAML)
```yaml
network_name:
  full: "Open5GS"
  short: "Next"
```

### 2. Time Zone Configuration
```yaml
time:
  no_time_zone_information: false  # Enable time zone info
```

### 3. Parameter Control Structure
```c
typedef struct gmm_configuration_update_command_param_s {
    int registration_requested;     // Request new registration
    int acknowledgement_requested;  // Request UE acknowledgment
    int nitz;                      // Include Network Identity and Time Zone
    int guti;                      // Include new GUTI
} gmm_configuration_update_command_param_t;
```

## Key Algorithms and Calculations

### 1. UTF-16 Encoding Algorithm
```c
// For each ASCII character
for (i = 0; i < strlen(ascii_string); i++) {
    utf16_buffer[i * 2] = 0x00;              // High byte (null)
    utf16_buffer[i * 2 + 1] = ascii_string[i]; // Low byte (ASCII)
}
```

### 2. Time Zone Offset Calculation
```c
// Convert seconds to 15-minute units
offset_quarters = timezone_offset_seconds / 900;

// Encode in BCD with sign bit
if (offset >= 0) {
    encoded = BCD_ENCODE(offset_quarters);
} else {
    encoded = BCD_ENCODE(-offset_quarters) | 0x08;
}
```

### 3. BCD Time Encoding
```c
// Encode: YY MM DD HH MM SS TZ
bcd_time[0] = ((year % 100) / 10) << 4 | ((year % 100) % 10);
bcd_time[1] = ((month) / 10) << 4 | ((month) % 10);
bcd_time[2] = ((day) / 10) << 4 | ((day) % 10);
bcd_time[3] = ((hour) / 10) << 4 | ((hour) % 10);
bcd_time[4] = ((minute) / 10) << 4 | ((minute) % 10);
bcd_time[5] = ((second) / 10) << 4 | ((second) % 10);
bcd_time[6] = timezone_encoded;
```

## Operator Customization Points

1. **Network Names**: Configurable via YAML for operator branding
2. **Time Zone**: Can be disabled for privacy or roaming scenarios
3. **Update Triggers**: Can be sent periodically or on demand
4. **Language Support**: UTF-16 enables international character sets
5. **Acknowledgment**: Optional UE confirmation for critical updates

## Security Considerations
1. Configuration updates are integrity protected and encrypted
2. Network names prevent spoofing attacks
3. Time synchronization ensures accurate billing and logging
4. Operator control prevents unauthorized network information