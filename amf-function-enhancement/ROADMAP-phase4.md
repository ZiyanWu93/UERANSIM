# ROADMAP-phase4.md Implementation Plan

## Parse Tree Analysis and Field Derivation

### Input Message Parse Tree: Registration Complete
```json
{
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS)
  "nas_5gs.security_header_type": "2",     // Integrity protected and ciphered
  "nas_5gs.msg_auth_code": "0xba0292cd",   // Message authentication code
  "nas_5gs.seq_no": "2",                   // Security sequence number
  "Plain NAS 5GS Message": {
    "nas_5gs.mm.message_type": "0x67",     // Registration Complete
    "Payload container": {                  // Contains PDU Session Establishment Request
      "nas_5gs.mm.pld_cont_type": "1",     // N1 SM information
      "PDU Session Establishment Request": {
        "nas_5gs.pdu_session_id": "1",
        "nas_5gs.proc_trans_id": "1",
        "nas_5gs.sm.message_type": "0xc1",
        "nas_5gs.sm.pdu_session_type": "1",  // IPv4
        "nas_5gs.sm.sc_mode": "1",           // SSC mode 1
        "nas_5gs.cmn.dnn": "internet"        // Data Network Name
      }
    }
  }
}
```

### Output Message Parse Tree: Configuration Update Command
```json
{
  "per.octet_string_length": "51",          // Total PDU length → See "Protocol Encoding Fields" section
  "ngap.NAS_PDU": "7e:02:de:0d:22:e3:02...", // Complete hex encoding → See "Complete NAS PDU Assembly" section
  "nas_5gs.epd": "126",                    // Extended Protocol Discriminator (5GS) → See "Protocol Encoding Fields" section
  "nas_5gs.spare_half_octet": "0",         // Always 0 for 5GS → See "Protocol Encoding Fields" section
  "nas_5gs.security_header_type": "2",     // Integrity protected and ciphered → See "Security Header Configuration" section
  "nas_5gs.msg_auth_code": "0xde0d22e3",   // Message authentication code → See "Security Calculations" section
  "nas_5gs.seq_no": "2",                   // Security sequence number → See "Security Context Management" section
  "Plain NAS 5GS Message": {
    "nas_5gs.epd": "126",                  // Extended Protocol Discriminator (5GS) → See "Protocol Encoding Fields" section
    "nas_5gs.spare_half_octet": "0",       // Always 0 for 5GS → See "Protocol Encoding Fields" section
    "nas_5gs.security_header_type": "0",   // Plain NAS message → See "Security Header Configuration" section
    "nas_5gs.mm.message_type": "0x54",     // Configuration Update Command → See "Message Type Constants" section
    "Network Name - Full name": {
      "gsm_a.dtap.elem_id": "0x43",        // Full network name IE ID → See "Information Element Identifiers" section
      "gsm_a.len": "15",                   // Length calculation → See "Network Name Length Calculations" section
      "gsm_a.extension": "1",              // Extension bit set → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.coding_scheme": "1",     // USC-2 (UTF-16) encoding → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.add_ci": "0",           // No country initials → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.number_of_spare_bits": "0", // No spare bits → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.text_string": "Open5GS"  // Network name in UTF-16 → See "UTF-16 Text Encoding" section
    },
    "Network Name - Short name": {
      "gsm_a.dtap.elem_id": "0x45",        // Short network name IE ID → See "Information Element Identifiers" section
      "gsm_a.len": "9",                    // Length calculation → See "Network Name Length Calculations" section
      "gsm_a.extension": "1",              // Extension bit set → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.coding_scheme": "1",     // USC-2 (UTF-16) encoding → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.add_ci": "0",           // No country initials → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.number_of_spare_bits": "0", // No spare bits → See "Network Name Header Byte Construction" section
      "gsm_a.dtap.text_string": "Next"    // Short name in UTF-16 → See "UTF-16 Text Encoding" section
    },
    "Time Zone - Local": {
      "gsm_a.dtap.elem_id": "0x46",        // Time zone IE ID → See "Information Element Identifiers" section
      "gsm_a.dtap.timezone": "0x14"        // UTC+5:00 (BCD encoded) → See "Time Zone BCD Encoding" section
    },
    "Time Zone and Time - Universal and Local": {
      "gsm_a.dtap.elem_id": "0x47",        // Time zone and time IE ID → See "Information Element Identifiers" section
      "gsm_a.dtap.time_zone_time": "Jun 9, 2025 08:53:35.000000000 UTC", // → See "Universal Time BCD Encoding" section
      "gsm_a.dtap.timezone": "0x14"        // UTC+5:00 (BCD encoded) → See "Time Zone BCD Encoding" section
    },
    "Daylight Saving Time": {
      "gsm_a.dtap.elem_id": "0x49",        // DST IE ID → See "Information Element Identifiers" section
      "gsm_a.len": "1",                    // Fixed length → See "Information Element Lengths" section
      "gsm_a.spare_bits": "0",             // Reserved bits → See "DST Value Encoding" section
      "gsm_a.dtap.dst_adjustment": "1"     // +1 hour DST adjustment → See "DST Value Encoding" section
    }
  }
}
```

## Field Derivation Logic

### Protocol Encoding Fields
1. **per.octet_string_length (51)**:
   - Counts total bytes in the NAS PDU
   - Security header (6 bytes) + Plain message (45 bytes) = 51 bytes
   - Used by NGAP layer for PDU container encoding
   - Function: `amf_send_configuration_update_command()` → calculates total PDU length before NGAP encapsulation

2. **nas_5gs.epd (126)**:
   - Extended Protocol Discriminator = 0x7E
   - Identifies 5GS mobility management messages
   - Appears twice: once in security header, once in plain message
   - Function: `amf_build_configuration_update_command()` → sets protocol discriminator in message header

3. **nas_5gs.spare_half_octet (0)**:
   - Always set to 0 for 5GS messages
   - Reserved for future use per 3GPP TS 24.501
   - Function: `amf_build_configuration_update_command()` → initializes spare bits to zero

### Security Header Configuration
1. **nas_5gs.security_header_type**:
   - Value 2 = Integrity protected and ciphered
   - Value 0 = Plain NAS message (inner header)
   - Determines security processing required
   - Function: `amf_apply_nas_security()` → sets security header type based on security context

### Security Context Management
1. **nas_5gs.seq_no (2)**:
   - Downlink NAS count least significant 8 bits
   - Incremented for each downlink NAS message
   - Prevents replay attacks
   - Function: `amf_apply_nas_security()` → increments and applies downlink NAS count

### Security Calculations
1. **nas_5gs.msg_auth_code (0xde0d22e3)**:
   - 32-bit MAC calculated using NIA algorithm
   - Inputs: K_AMF, downlink count, bearer ID, direction, message
   - Algorithm specified during security mode setup
   - Function: `amf_apply_nas_security()` → calculates MAC using selected NIA algorithm

### Message Type Constants
1. **nas_5gs.mm.message_type (0x54)**:
   - Configuration Update Command per TS 24.501 Table 9.7.1
   - Fixed value for this message type
   - Function: `amf_build_configuration_update_command()` → sets message type for Configuration Update Command

### Information Element Identifiers
1. **gsm_a.dtap.elem_id (0x43)**: Full name for network (TS 24.008 10.5.3.5a)
   - Function: `amf_build_configuration_update_command()` → adds full network name IE with ID 0x43
2. **gsm_a.dtap.elem_id (0x45)**: Short name for network (TS 24.008 10.5.3.5a)
   - Function: `amf_build_configuration_update_command()` → adds short network name IE with ID 0x45
3. **gsm_a.dtap.elem_id (0x46)**: Time Zone (TS 24.008 10.5.3.8)
   - Function: `amf_build_configuration_update_command()` → adds time zone IE with ID 0x46
4. **gsm_a.dtap.elem_id (0x47)**: Time Zone and Time (TS 24.008 10.5.3.9)
   - Function: `amf_build_configuration_update_command()` → adds time zone and time IE with ID 0x47
5. **gsm_a.dtap.elem_id (0x49)**: Daylight Saving Time (TS 24.008 10.5.3.12)
   - Function: `amf_build_configuration_update_command()` → adds DST IE with ID 0x49

### Information Element Lengths
1. **Network Name Lengths**:
   - Full name: 15 = 1 (header) + 14 (7 UTF-16 chars)
   - Short name: 9 = 1 (header) + 8 (4 UTF-16 chars)
   - Function: `amf_encode_network_names_utf16()` → calculates length based on UTF-16 encoding

2. **Fixed Length IEs**:
   - Time Zone: No length field (fixed 1 byte)
   - Time Zone and Time: No length field (fixed 7 bytes)
   - DST: Length = 1 (value byte only)
   - Function: `amf_build_configuration_update_command()` → applies fixed lengths per 3GPP spec

### Network Name Header Byte Construction
1. **Header Byte (0x90)**:
   - Bit 7: Extension = 1
   - Bits 6-5: Coding Scheme = 01 (USC-2/UTF-16)
   - Bit 4: Add Country Initials = 0
   - Bits 3-0: Number of spare bits = 0000
   - Binary: 1001 0000 = 0x90
   - Function: `amf_encode_network_names_utf16()` → constructs header byte with UTF-16 encoding

### Network Name Length Calculations
1. **Full Name "Open5GS" (15 bytes)**:
   - Header byte: 1 byte (0x90)
   - UTF-16 text: 7 chars × 2 bytes/char = 14 bytes
   - Total: 1 + 14 = 15 bytes
   - Function: `amf_encode_network_names_utf16()` → calculates total length for full network name

2. **Short Name "Next" (9 bytes)**:
   - Header byte: 1 byte (0x90)
   - UTF-16 text: 4 chars × 2 bytes/char = 8 bytes
   - Total: 1 + 8 = 9 bytes
   - Function: `amf_encode_network_names_utf16()` → calculates total length for short network name

### UTF-16 Text Encoding
1. **Full Name "Open5GS"**:
   - 'O' (0x4F) → 0x00:4F
   - 'p' (0x70) → 0x00:70
   - 'e' (0x65) → 0x00:65
   - 'n' (0x6E) → 0x00:6E
   - '5' (0x35) → 0x00:35
   - 'G' (0x47) → 0x00:47
   - 'S' (0x53) → 0x00:53
   - Result: 00:4F:00:70:00:65:00:6E:00:35:00:47:00:53
   - Function: `amf_encode_network_names_utf16()` → converts ASCII to UTF-16 for full name

2. **Short Name "Next"**:
   - 'N' (0x4E) → 0x00:4E
   - 'e' (0x65) → 0x00:65
   - 'x' (0x78) → 0x00:78
   - 't' (0x74) → 0x00:74
   - Result: 00:4E:00:65:00:78:00:74
   - Function: `amf_encode_network_names_utf16()` → converts ASCII to UTF-16 for short name

### Time Zone BCD Encoding
1. **gsm_a.dtap.timezone values**:
   - Parse tree shows 0x14 but actual PDU has 0x0A
   - 0x14 interpretation: UTC+5:00 (20 quarter-hours)
   - 0x0A interpretation: UTC+2:30 (10 quarter-hours)
   - Discrepancy between parse tree display and actual encoding
   - BCD format: [units nibble][tens nibble + sign bit]
   - Sign bit (bit 3 of high nibble): 0 for positive offset
   - Function: `amf_calculate_time_zone_info()` → encodes time zone offset in BCD format

### Universal Time BCD Encoding
1. **Time String "Jun 9, 2025 08:53:35"**:
   - Year: 25 → 0x52 (swapped nibbles: 5|2)
   - Month: 06 → 0x60 (swapped: 6|0)
   - Day: 09 → 0x90 (swapped: 9|0)
   - Hour: 08 → 0x30 (error in trace, should be 0x80)
   - Minute: 53 → 0x35 (swapped: 3|5)
   - Second: 35 → 0x53 (swapped: 5|3)
   - Time zone: 0x0A (UTC+5:00, different encoding)
   - Actual in trace: 52:60:90:30:35:53:0A
   - Function: `amf_calculate_time_zone_info()` → converts time components to BCD format

### DST Value Encoding
1. **gsm_a.spare_bits (0)**:
   - Bits 7-2 reserved, set to 0
   - Function: `amf_calculate_time_zone_info()` → sets spare bits to zero

2. **gsm_a.dtap.dst_adjustment (1)**:
   - 0 = No adjustment
   - 1 = +1 hour adjustment
   - 2 = +2 hours adjustment
   - 3 = Reserved
   - Function: `amf_calculate_time_zone_info()` → determines DST adjustment from system time

### Complete NAS PDU Assembly
1. **Security Header (7 bytes)**:
   - EPD + Security type: 7E:02 (126 decimal, type 2)
   - MAC: DE:0D:22:E3 (4 bytes)
   - Sequence number: 02 (1 byte)
   - Function: `amf_apply_nas_security()` → constructs complete security header

2. **Plain Message Header (3 bytes)**:
   - EPD + Security type: 7E:00 (126 decimal, type 0)
   - Message type: 54 (Configuration Update Command)
   - Function: `amf_build_configuration_update_command()` → constructs plain message header

3. **Information Elements (41 bytes total)**:
   - Full name IE (17 bytes): 43:0F:90:00:4F:00:70:00:65:00:6E:00:35:00:47:00:53
     - IE ID: 43
     - Length: 0F (15 bytes)
     - Content: 90 + UTF-16 text (14 bytes)
   - Short name IE (11 bytes): 45:09:90:00:4E:00:65:00:78:00:74
     - IE ID: 45
     - Length: 09 (9 bytes)
     - Content: 90 + UTF-16 text (8 bytes)
   - Time zone IE (2 bytes): 46:0A
     - IE ID: 46
     - Time zone value: 0A (not 14 as shown in parse tree)
   - Time zone and time IE (9 bytes): 47:52:60:90:30:35:53:0A
     - IE ID: 47
     - Time BCD: 52:60:90:30:35:53 (6 bytes)
     - Time zone: 0A (1 byte)
   - DST IE (3 bytes): 49:01:01
     - IE ID: 49
     - Length: 01
     - Value: 01 (+1 hour)
   - Function: `amf_build_configuration_update_command()` → assembles all IEs in correct order

4. **Complete PDU (51 bytes)**:
   ```
   7E:02:DE:0D:22:E3:02:7E:00:54:43:0F:90:00:4F:00:70:00:65:00:6E:00:35:00:47:00:53:45:09:90:00:4E:00:65:00:78:00:74:46:0A:47:52:60:90:30:35:53:0A:49:01:01
   ```
   - Matches exactly the ngap.NAS_PDU field from parse tree
   - Function: `amf_send_configuration_update_command()` → sends complete PDU via NGAP

## Implementation Function Chain

### Phase 4: NFLambda AMF Function Implementation

```
amf_handle_registration_complete()
    ↓
amf_confirm_guti_assignment()
    ↓
amf_check_configuration_update_policy()
    ↓
amf_encode_network_names_utf16()
    ↓
amf_calculate_time_zone_info()
    ↓
amf_build_configuration_update_command()
    ↓
amf_apply_nas_security()
    ↓
amf_send_configuration_update_command()
```

### Detailed Function Specifications

#### 1. `amf_handle_registration_complete()`
**Input**: Registration Complete NAS PDU
**Output**: Trigger configuration update flow
**Logic**:
- Parse NAS PDU using ASN.1 decoder
- Clear T3550 timer (registration procedure timer)
- Extract PDU session request if present
- Update UE state to REGISTERED
- Check if configuration update needed

#### 2. `amf_confirm_guti_assignment()`
**Input**: AMF UE context
**Output**: Updated GUTI state
**Logic**:
- Copy next.guti to current.guti
- Copy next.m_tmsi to current.m_tmsi
- Update GUTI hash table for fast lookup
- Clear next GUTI fields
- Log GUTI confirmation

#### 3. `amf_check_configuration_update_policy()`
**Input**: AMF configuration, UE context
**Output**: Boolean decision and parameters
**Logic**:
- Check if network names configured
- Check if time zone info enabled
- Check if UE supports configuration updates
- Determine which IEs to include
- Set acknowledgment requirement

#### 4. `amf_encode_network_names_utf16()`
**Input**: ASCII network names from config
**Output**: UTF-16 encoded network name structures
**Logic**:
- For each ASCII character:
  - Insert null byte (0x00) for high byte
  - Copy ASCII value for low byte
- Set coding scheme = 1 (USC-2/UTF-16)
- Set extension bit = 1
- Calculate total length (ASCII length * 2 + 1)
- Set spare bits and country initials to 0

#### 5. `amf_calculate_time_zone_info()`
**Input**: System time and timezone
**Output**: BCD encoded time zone and universal time
**Logic**:
- Get current UTC time using time(NULL)
- Get local time using localtime()
- Calculate offset: local.tm_gmtoff / 900 (15-min units)
- Encode offset in BCD format
- If negative, set sign bit (0x08)
- Convert time components to BCD:
  - Year % 100 → two BCD digits
  - Month, day, hour, minute, second → BCD pairs
- Check DST status from tm_isdst

#### 6. `amf_build_configuration_update_command()`
**Input**: Configuration parameters and encoded values
**Output**: Configuration Update Command NAS PDU
**Logic**:
- Set message type = 0x54
- Add network names if configured:
  - Full name with IE 0x43
  - Short name with IE 0x45
- Add time zone info if enabled:
  - Local time zone with IE 0x46
  - Universal time and zone with IE 0x47
  - DST adjustment with IE 0x49
- Set presence masks for optional IEs

#### 7. `amf_apply_nas_security()`
**Input**: Plain NAS message, security context
**Output**: Protected NAS message
**Logic**:
- Set security header type = 2 (protected and ciphered)
- Increment downlink NAS count
- Calculate MAC using NIA algorithm
- Encrypt using NEA algorithm
- Prepend security header

#### 8. `amf_send_configuration_update_command()`
**Input**: Protected Configuration Update Command
**Output**: Message sent to RAN
**Logic**:
- Get RAN UE NGAP context
- Send via downlink NAS transport
- Start T3555 timer if acknowledgment requested
- Log configuration update transmission

## Detailed Field Calculations

### Network Name UTF-16 Encoding
#### Full Network Name: "Open5GS"
- **ASCII Input**: 0x4F 0x70 0x65 0x6E 0x35 0x47 0x53
- **UTF-16 Output**: 0x00:4F:00:70:00:65:00:6E:00:35:00:47:00:53
- **Length Calculation**: 7 characters × 2 bytes + 1 header = 15 bytes
- **Header Byte**: 
  - Bits 7-4: Extension (1), Coding (01), Add CI (0) = 0x90
  - Bits 3-0: Spare bits (0000) = 0x00
  - Result: 0x90

#### Short Network Name: "Next"
- **ASCII Input**: 0x4E 0x65 0x78 0x74
- **UTF-16 Output**: 0x00:4E:00:65:00:78:00:74
- **Length Calculation**: 4 characters × 2 bytes + 1 header = 9 bytes
- **Header Byte**: 0x90 (same encoding parameters)

### Time Zone BCD Encoding
- **UTC+5:00 Example**:
  - Offset in minutes: 5 × 60 = 300 minutes
  - Offset in quarter-hours: 300 ÷ 15 = 20
  - BCD encoding of 20: 0x14
  - Sign bit: 0 (positive offset)
  - Final value: 0x14

- **UTC-3:30 Example**:
  - Offset in minutes: -3.5 × 60 = -210 minutes
  - Offset in quarter-hours: 210 ÷ 15 = 14
  - BCD encoding of 14: 0x14
  - Sign bit: 1 (negative offset)
  - Final value: 0x14 | 0x08 = 0x1C

### Universal Time BCD Encoding
For timestamp "Jun 9, 2025 08:53:35":
- **Year**: 25 → 0x25
- **Month**: 06 → 0x06
- **Day**: 09 → 0x09
- **Hour**: 08 → 0x08
- **Minute**: 53 → 0x53
- **Second**: 35 → 0x35
- **Time Zone**: 0x14 (UTC+5:00)
- **Complete**: 0x25:06:09:08:53:35:14

### Daylight Saving Time Values
- **0x00**: No adjustment
- **0x01**: +1 hour adjustment
- **0x02**: +2 hours adjustment
- **0x03**: Reserved

## Open5GS Code Reference Mapping

### Registration Complete Handler
**File**: `open5gs/src/amf/gmm-sm.c:2689-2719`
```c
case OGS_NAS_5GS_REGISTRATION_COMPLETE:
    ogs_info("[%s] Registration complete", amf_ue->supi);
    CLEAR_AMF_UE_TIMER(amf_ue->t3550);
    amf_ue_confirm_guti(amf_ue);
    nas_5gs_send_configuration_update_command(amf_ue, &param);
```

### GUTI Confirmation
**File**: `open5gs/src/amf/context.c:1521-1566`
```c
void amf_ue_confirm_guti(amf_ue_t *amf_ue) {
    amf_ue->current.guti = amf_ue->next.guti;
    ogs_hash_set(self.guti_hash, &amf_ue->current.guti, 
                 sizeof(ogs_nas_5gs_guti_t), amf_ue);
}
```

### UTF-16 Network Name Encoding
**File**: `open5gs/src/amf/context.c:965-1013`
```c
network_full_name->coding_scheme = 1;  // USC-2 (UTF-16)
for (i = 0; i < strlen(c_network_name); i++) {
    network_full_name->name[i * 2] = 0;
    network_full_name->name[i * 2 + 1] = c_network_name[i];
}
```

### Time Zone Calculation
**File**: `open5gs/src/amf/gmm-build.c:546-605`
```c
if (local.tm_gmtoff >= 0) {
    *local_time_zone = OGS_NAS_TIME_TO_BCD(local.tm_gmtoff / 900);
} else {
    *local_time_zone = OGS_NAS_TIME_TO_BCD((-local.tm_gmtoff) / 900);
    *local_time_zone |= 0x08;  // Set sign bit
}
```

### Configuration Update Command Building
**File**: `open5gs/src/amf/gmm-build.c:485-625`
```c
message.gmm.h.message_type = OGS_NAS_5GS_CONFIGURATION_UPDATE_COMMAND;
configuration_update_command->full_name_for_network = 
    amf_self()->network_name.full;
configuration_update_command->short_name_for_network = 
    amf_self()->network_name.short;
```

## Binary Message Structures

```c
#pragma pack(1)

// Registration Complete message structure (0x67)
typedef struct {
    // Security header (when security_header_type = 2)
    uint8_t epd_security_type;           // 0x7E:02 - EPD (126) | Security header type (2)
    uint32_t msg_auth_code;              // 0xBA0292CD - Message authentication code
    uint8_t seq_no;                      // 0x02 - Sequence number
    
    // Plain NAS 5GS message header
    uint8_t epd_security_type_plain;     // 0x7E:00 - EPD (126) | Security header type (0)
    uint8_t message_type;                // 0x67 - Registration Complete
    
    // Payload container (PDU Session Establishment Request)
    uint8_t payload_container_type_iei;  // 0x01 - IEI for payload container type
    uint8_t payload_container_type : 4;  // 0x1 - N1 SM information
    uint8_t spare : 4;                   // 0x0 - Spare half octet
    uint8_t payload_container_iei;       // 0x00 - IEI for payload container
    uint8_t payload_container_len;       // 0x15 - Length (21 bytes)
    
    // PDU Session Establishment Request embedded in payload
    struct {
        uint8_t epd;                     // 0x2E - Extended protocol discriminator
        uint8_t pdu_session_id;          // 0x01 - PDU session identity
        uint8_t pti;                     // 0x01 - Procedure transaction identity
        uint8_t message_type;            // 0xC1 - PDU Session Establishment Request
        
        // Integrity protection maximum data rate
        uint8_t max_data_rate_ul;        // 0xFF - 255 (64 kbps units)
        uint8_t max_data_rate_dl;        // 0xFF - 255 (64 kbps units)
        
        // Optional IEs
        uint8_t pdu_session_type_iei;    // 0x91 - PDU session type IEI
        uint8_t pdu_session_type : 3;    // 0x1 - IPv4
        uint8_t spare1 : 5;              // 0x10 - Spare bits
        
        uint8_t ssc_mode_iei;            // 0x28 - SSC mode IEI
        uint8_t ssc_mode : 3;            // 0x1 - SSC mode 1
        uint8_t spare2 : 5;              // 0x0 - Spare bits
        
        // 5GSM capability
        uint8_t capability_iei;          // 0x00 - 5GSM capability IEI
        uint8_t capability_len;          // 0x7B - Length (should be 1)
        uint8_t capability_value;        // 0x00 - Capability flags
        
        // Extended protocol configuration options
        uint8_t epco_iei;                // 0x07 - Extended PCO IEI
        uint8_t epco_len;                // 0x80 - Length (7 bytes) with extension bit
        uint8_t epco_ext : 1;            // 0x1 - Extension bit
        uint8_t epco_spare : 4;          // 0x0 - Spare bits
        uint8_t epco_config_protocol : 3; // 0x0 - Configuration protocol
        uint16_t epco_protocol_id;       // 0x0A00 - Protocol ID (0x000A in network order)
        uint8_t epco_protocol_len;       // 0x00 - Protocol content length
        uint8_t epco_content[1];         // 0x0D - Protocol content
    } pdu_session_request;
    
    // Additional IEs after payload container
    uint8_t pdu_session_id_2_iei;       // 0x00 - PDU session ID 2 IEI
    uint8_t pdu_session_id_2_len;       // 0x12 - Length (should be 1)
    uint8_t pdu_session_id_2;           // 0x01 - PDU session identity
    
    uint8_t request_type_iei;           // 0x81 - Request type IEI
    uint8_t request_type : 3;           // 0x1 - Initial request
    uint8_t spare3 : 5;                 // 0x4 - Spare bits
    
    uint8_t s_nssai_iei;                // 0x01 - S-NSSAI IEI
    uint8_t s_nssai_len;                // 0x01 - Length
    uint8_t sst;                        // 0x25 - Slice/Service Type
    
    uint8_t dnn_iei;                    // 0x09 - DNN IEI
    uint8_t dnn_len;                    // 0x08 - Length
    uint8_t dnn_value[8];               // "internet" encoded
} registration_complete_t;

// Configuration Update Command message structure (0x54)
typedef struct {
    // Security header (when security_header_type = 2)
    uint8_t epd_security_type;           // 0x7E:02 - EPD (126) | Security header type (2)
    uint32_t msg_auth_code;              // 0xDE0D22E3 - Message authentication code
    uint8_t seq_no;                      // 0x02 - Sequence number
    
    // Plain NAS 5GS message header
    uint8_t epd_security_type_plain;     // 0x7E:00 - EPD (126) | Security header type (0)
    uint8_t message_type;                // 0x54 - Configuration Update Command
    
    // Full name for network
    uint8_t full_name_iei;               // 0x43 - Full name IEI
    uint8_t full_name_len;               // 0x0F - Length (15 bytes)
    uint8_t full_name_header;            // 0x90 - Ext=1, Coding=USC2, Add_CI=0, Spare=0
    uint16_t full_name_chars[7];         // UTF-16 "Open5GS": 00:4F:00:70:00:65:00:6E:00:35:00:47:00:53
    
    // Short name for network
    uint8_t short_name_iei;              // 0x45 - Short name IEI
    uint8_t short_name_len;              // 0x09 - Length (9 bytes)
    uint8_t short_name_header;           // 0x90 - Ext=1, Coding=USC2, Add_CI=0, Spare=0
    uint16_t short_name_chars[4];        // UTF-16 "Next": 00:4E:00:65:00:78:00:74
    
    // Time zone
    uint8_t time_zone_iei;               // 0x46 - Time zone IEI
    uint8_t time_zone;                   // 0x0A - Time zone value (UTC+2:30)
    
    // Time zone and time
    uint8_t time_zone_time_iei;          // 0x47 - Time zone and time IEI
    uint8_t year;                        // 0x52 - Year (2025 in BCD)
    uint8_t month;                       // 0x60 - Month (06 in BCD)
    uint8_t day;                         // 0x90 - Day (09 in BCD)
    uint8_t hour;                        // 0x30 - Hour (03 in BCD, should be 08)
    uint8_t minute;                      // 0x35 - Minute (53 in BCD)
    uint8_t second;                      // 0x53 - Second (35 in BCD)
    uint8_t time_zone_2;                 // 0x0A - Time zone value
    
    // Daylight saving time
    uint8_t dst_iei;                     // 0x49 - DST IEI
    uint8_t dst_len;                     // 0x01 - Length
    uint8_t dst_value;                   // 0x01 - +1 hour adjustment
} configuration_update_command_t;

// Static assertions to verify structure sizes
static_assert(sizeof(registration_complete_t) == 51, "Registration Complete size mismatch");
static_assert(sizeof(configuration_update_command_t) == 51, "Configuration Update Command size mismatch");

#pragma pack()
```