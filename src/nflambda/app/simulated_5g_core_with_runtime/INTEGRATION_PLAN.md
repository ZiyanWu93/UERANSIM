# Service Function Chain Implementation Plan

## Overview
Transform the current monolithic message handlers into service function chains where each network function is responsible for specific message fields. The output messages remain identical, but the construction is distributed across multiple handlers.

## Implementation Strategy
- Split each hardcoded message array into field-specific handlers
- Each handler modifies EVENT_PAYLOAD directly by setting its designated fields
- Maintain the exact same byte sequences as the current implementation

## Message 1: Registration Request → Authentication Request

**Current Implementation**: Single array in `amf_handle_registration_request()`
```c
uint8_t auth_request[] = {
    0x7e, 0x00,  // EPD header
    0x56,        // Message type
    0x00,        // ngKSI field
    0x02, 0x00, 0x00,  // ABBA IE
    0x21,        // RAND IEI
    // ... RAND value (16 bytes)
    0x20,        // AUTN IEI
    0x10,        // AUTN length
    // ... AUTN value (16 bytes)
};
```

**Service Function Chain**:
1. `amf_handle_registration_request()` - Initialize message with headers (bytes 0-2)
2. `amf_set_auth_ngksi()` - Set ngKSI field at offset 3
3. `amf_set_abba()` - Set ABBA IE at offset 4-6
4. `ausf_set_rand()` - Set RAND IEI and value at offset 7-23
5. `udm_set_autn()` - Set AUTN IEI, length and value at offset 24-41

## Message 2: Authentication Response → Security Mode Command

**Current Implementation**: Single array in `amf_handle_authentication_response()`
```c
uint8_t sec_mode_cmd[] = {
    0x7e, 0x03,              // Outer header
    0x13, 0xbf, 0x99, 0x5a,  // MAC
    0x00,                    // Sequence number
    0x7e, 0x00,              // Inner header
    0x5d,                    // Message type
    0x02,                    // NAS algorithms
    0x00,                    // ngKSI field
    0x04,                    // UE security capability length
    0x80, 0xf0, 0x80, 0xf0,  // UE security capability value
    0xe1,                    // IMEISV request
    0x36, 0x01, 0x02         // Additional security info
};
```

**Service Function Chain**:
1. `amf_handle_authentication_response()` - Set outer header (bytes 0-1)
2. `amf_calculate_mac()` - Set MAC at offset 2-5
3. `amf_set_sequence_number()` - Set sequence at offset 6
4. `amf_set_inner_security_header()` - Set inner header and message type at offset 7-9
5. `amf_select_algorithms()` - Set NAS algorithms at offset 10
6. `amf_set_security_context()` - Set ngKSI at offset 11
7. `amf_set_ue_security_capability()` - Set UE security capability at offset 12-16
8. `amf_request_imeisv()` - Set IMEISV request at offset 17
9. `amf_add_security_info()` - Set additional security info at offset 18-20

## Message 3: Security Mode Complete → Registration Accept

**Current Implementation**: Single array in `amf_handle_security_mode_complete()`
```c
uint8_t reg_accept[] = {
    0x7e, 0x02,              // Outer header
    0x72, 0x39, 0x67, 0x4c,  // MAC
    0x01,                    // Sequence number
    0x7e, 0x00,              // Inner header
    0x42,                    // Message type
    0x01, 0x01,              // Registration result
    0x77, 0x00, 0x0b, 0xf2, 0x99, 0xf9, 0x07, 0x02, 0x00, 0x40, 0xc0, 0x00, 0x07, 0x27,  // GUTI IE
    0x54, 0x07, 0x40, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x01,  // TAI list
    0x15, 0x02, 0x01, 0x01,  // NSSAI
    0x21, 0x02, 0x01, 0x00,  // Network features
    0x5e, 0x01, 0x92         // GPRS timer
};
```

**Service Function Chain**:
1. `amf_handle_security_mode_complete()` - Set outer header (bytes 0-1)
2. `amf_set_reg_accept_mac()` - Set MAC at offset 2-5
3. `amf_increment_sequence()` - Set sequence number at offset 6
4. `amf_set_reg_accept_header()` - Set inner header and message type at offset 7-9
5. `amf_set_registration_result()` - Set registration result at offset 10-11
6. `amf_allocate_guti()` - Set GUTI IE at offset 12-25
7. `amf_set_tai_list()` - Set TAI list at offset 26-34
8. `udm_provide_nssai()` - Set allowed NSSAI at offset 35-38
9. `amf_set_network_features()` - Set 5GS network feature support at offset 39-42
10. `amf_set_t3512_timer()` - Set T3512 timer value at offset 43-45

## Message 4: Registration Complete → Configuration Update

**Current Implementation**: Single array in `amf_handle_registration_complete()`
```c
uint8_t config_update[] = {
    0x7e, 0x02,              // Outer header
    0xde, 0x0d, 0x22, 0xe3,  // MAC
    0x02,                    // Sequence number
    0x7e, 0x00,              // Inner header
    0x54,                    // Message type
    0x43, 0x0f, 0x90, 0x00, 0x4f, 0x00, 0x70, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x35, 0x00, 0x47, 0x00, 0x53,  // Network name full
    0x45, 0x09, 0x90, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74,  // Network name short
    0x46, 0x0a,              // TZ local
    0x47, 0x52, 0x60, 0x90, 0x30, 0x35, 0x53, 0x0a,  // TZ and time
    0x49, 0x01, 0x01         // DST
};
```

**Service Function Chain**:
1. `amf_handle_registration_complete()` - Set outer header (bytes 0-1)
2. `amf_set_config_mac()` - Set MAC at offset 2-5
3. `amf_set_config_sequence()` - Set sequence number at offset 6
4. `amf_set_config_header()` - Set inner header and message type at offset 7-9
5. `pcf_provide_network_name_full()` - Set full network name at offset 10-26
6. `pcf_provide_network_name_short()` - Set short network name at offset 27-37
7. `amf_set_timezone_info()` - Set timezone information at offset 38-39
8. `amf_set_time_info()` - Set universal time and timezone at offset 40-47
9. `amf_set_daylight_saving()` - Set daylight saving time at offset 48-50

## Message 5: PDU Session Request → PDU Session Accept

**Current Implementation**: Single array in `amf_handle_pdu_session_request()`
```c
uint8_t pdu_accept[] = {
    0x7e, 0x02,              // Outer header
    0xfb, 0xd6, 0x2d, 0x81,  // MAC
    0x03,                    // Sequence
    0x7e, 0x00,              // Inner header
    0x68,                    // DL message
    0x01,                    // Spare/PT
    0x00, 0x47,              // Procedure code length
    0x2e,                    // SM EPD
    0x01,                    // PDU session ID
    0x01,                    // PTI
    0xc2,                    // SM message type
    0x11,                    // SSC mode/PDU session type
    // ... QoS rules, AMBR, PDU address, etc.
};
```

**Service Function Chain**:
1. `amf_handle_pdu_session_request()` - Set outer header and forward to SMF (bytes 0-1)
2. `amf_set_pdu_mac()` - Set MAC at offset 2-5
3. `amf_set_pdu_sequence()` - Set sequence at offset 6
4. `smf_handle_pdu_session_create()` - Set PDU session headers at offset 7-14
5. `smf_set_pdu_session_type()` - Set SSC mode and PDU session type at offset 15
6. `smf_set_qos_rules()` - Set authorized QoS rules at offset 16-26
7. `smf_set_session_ambr()` - Set session AMBR at offset 27-33
8. `upf_allocate_pdu_address()` - Set PDU address at offset 34-39
9. `smf_set_allowed_snssai()` - Set S-NSSAI at offset 40-42
10. `smf_set_qos_flow_descriptions()` - Set QoS flow descriptions at offset 43-51
11. `pcf_provide_pcc_rules()` - Set extended protocol configuration options at offset 52-67
12. `smf_set_dnn()` - Set DNN at offset 68-77
13. `smf_set_5gsm_cause()` - Set 5GSM cause at offset 78-79

## Implementation Details

Each handler will:
1. Access EVENT_PAYLOAD directly
2. Write its specific fields at the correct offset
3. Not modify fields outside its responsibility
4. Maintain the exact byte values from the original arrays

Example implementation pattern:
```c
// In amf.c
void amf_set_auth_ngksi(void) {
    EVENT_PAYLOAD[3] = 0x00;  // ngKSI field
}

// In ausf.c
void ausf_set_rand(void) {
    EVENT_PAYLOAD[7] = 0x21;  // RAND IEI
    // Copy RAND value
    uint8_t rand[] = {0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 
                      0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69};
    memcpy(&EVENT_PAYLOAD[8], rand, 16);
}

// In udm.c
void udm_set_autn(void) {
    EVENT_PAYLOAD[24] = 0x20;  // AUTN IEI
    EVENT_PAYLOAD[25] = 0x10;  // AUTN length
    // Copy AUTN value
    uint8_t autn[] = {0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00,
                      0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19};
    memcpy(&EVENT_PAYLOAD[26], autn, 16);
}
```

## Benefits
- Demonstrates service function chaining architecture
- Shows how different NFs contribute to message construction
- Maintains exact compatibility with current implementation
- Provides clear separation of concerns between network functions
- Easy to extend or modify individual field handlers