# ROADMAP-phase3.md Implementation Plan

## Parse Tree Analysis and Field Derivation

### Input Message Parse Tree: Security Mode Complete
```json
{
  "nas_5gs.epd": "126",                      // Extended Protocol Discriminator (5GS)
  "nas_5gs.security_header_type": "4",       // Integrity protected and ciphered
  "nas_5gs.msg_auth_code": "0x22e4ee19",     // Message Authentication Code
  "nas_5gs.seq_no": "0",                     // Sequence Number
  "nas_5gs.mm.message_type": "0x5e",         // Security Mode Complete
  "5GS mobile identity": {
    "nas_5gs.mm.type_id": "5",              // IMEISV type
    "nas_5gs.mm.imeisv": "4370816125816151", // International Mobile Equipment Identity and Software Version
  },
  "NAS message container": {
    "nas-5gs": {                             // Replayed Registration Request
      "nas_5gs.mm.message_type": "0x41",    // Registration Request
      "5GS registration type": {
        "nas_5gs.mm.for": "1",               // Follow-on Request flag
        "nas_5gs.mm.5gs_reg_type": "1"      // Initial registration
      },
      "5GS mobile identity": {
        "nas_5gs.mm.type_id": "1",          // SUCI
        "e212.mcc": "999",                   // Mobile Country Code
        "e212.mnc": "70",                    // Mobile Network Code
        "nas_5gs.mm.suci.msin": "0000000001"// Mobile Station ID
      },
      "UE security capability": {
        "nas_5gs.mm.5g_ea0": "1",           // Encryption algorithms
        "nas_5gs.mm.ia0": "1",              // Integrity algorithms
        "nas_5gs.mm.5g_128_ia1": "1",
        "nas_5gs.mm.5g_128_ia2": "1",
        "nas_5gs.mm.5g_128_ia3": "1"
      },
      "NSSAI - Requested NSSAI": {
        "nas_5gs.mm.sst": "1"                // Slice/Service Type (eMBB)
      }
    }
  }
}
```

### Output Message Parse Tree: Registration Accept
```json
{
  "per.octet_string_length": "46",                      // Total NAS PDU length → See "Protocol Encoding Fields" section
  "ngap.NAS_PDU": "7e:02:72:39:67:4c:01:7e:00:42...", // Complete hex encoding → See "NAS PDU Encoding" section
  "ngap.NAS_PDU_tree": {
    "nas-5gs": {
      "Security protected NAS 5GS message": {
        "nas_5gs.epd": "126",                           // Extended Protocol Discriminator → See "Protocol Encoding Fields" section
        "nas_5gs.spare_half_octet": "0",                // Spare bits (always 0) → See "Protocol Encoding Fields" section
        "nas_5gs.security_header_type": "2",            // Integrity protected and ciphered → See "Security Context Fields" section
        "nas_5gs.msg_auth_code": "0x7239674c",          // Message Authentication Code → See "Security Context Fields" section
        "nas_5gs.seq_no": "1"                           // Sequence Number → See "Security Context Fields" section
      },
      "Plain NAS 5GS Message": {
        "nas_5gs.epd": "126",                           // EPD for inner message → See "Protocol Encoding Fields" section
        "nas_5gs.spare_half_octet": "0",                // Spare bits → See "Protocol Encoding Fields" section
        "nas_5gs.security_header_type": "0",            // Plain NAS message → See "Protocol Encoding Fields" section
        "nas_5gs.mm.message_type": "0x42",              // Registration Accept → See "Static Fields" section
        "5GS registration result": {
          "gsm_a.len": "1",                             // IE length → See "Information Element Identifiers" section
          "nas_5gs.mm.reg_res.emergency_reg": "0",      // Emergency registration → See "Complete Registration Result Encoding" section
          "nas_5gs.mm.reg_res.nssaa_perf": "0",         // NSSAA performed → See "Complete Registration Result Encoding" section
          "nas_5gs.mm.reg_res.sms_all": "0",            // SMS allowed → See "Complete Registration Result Encoding" section
          "nas_5gs.mm.reg_res.res": "1"                 // 3GPP access → See "Complete Registration Result Encoding" section
        },
        "5GS mobile identity - 5G-GUTI": {
          "nas_5gs.mm.elem_id": "0x77",                 // GUTI element ID → See "Information Element Identifiers" section
          "gsm_a.len": "11",                            // GUTI IE length → See "Length Calculations" section
          "nas_5gs.spare_b7": "1",                      // Spare bit 7 → See "GUTI Spare Fields" section
          "nas_5gs.spare_b6": "1",                      // Spare bit 6 → See "GUTI Spare Fields" section
          "nas_5gs.spare_b5": "1",                      // Spare bit 5 → See "GUTI Spare Fields" section
          "nas_5gs.spare_b4": "1",                      // Spare bit 4 → See "GUTI Spare Fields" section
          "nas_5gs.spare_b3": "0",                      // Spare bit 3 → See "GUTI Spare Fields" section
          "nas_5gs.mm.type_id": "2",                    // GUTI type → See "UE Identity Management" section
          "e212.guami.mcc": "999",                      // Mobile Country Code → See "GUTI Allocation" section
          "e212.guami.mnc": "70",                       // Mobile Network Code → See "GUTI Allocation" section
          "nas_5gs.amf_region_id": "2",                 // AMF Region ID → See "GUTI Allocation" section
          "nas_5gs.amf_set_id": "1",                    // AMF Set ID → See "GUTI Allocation" section
          "nas_5gs.amf_pointer": "0",                   // AMF Pointer → See "GUTI Allocation" section
          "nas_5gs.5g_tmsi": "3221227303",              // 5G-TMSI → See "GUTI Allocation" section
          "3gpp.tmsi": "3221227303"                     // Same as 5G-TMSI → See "GUTI Allocation" section
        },
        "5GS tracking area identity list": {
          "nas_5gs.mm.elem_id": "0x54",                 // TAI list element ID → See "Information Element Identifiers" section
          "gsm_a.len": "7",                             // TAI list IE length → See "Length Calculations" section
          "Partial tracking area list  1": {
            "nas_5gs.mm.tal_t_li": "2",                 // TAI list type → See "Location and Mobility Management" section
            "nas_5gs.mm.tal_num_e": "0",                // Number of elements → See "Location and Mobility Management" section
            "e212.5gstai.mcc": "999",                   // MCC → See "Location and Mobility Management" section
            "e212.5gstai.mnc": "70",                    // MNC → See "Location and Mobility Management" section
            "nas_5gs.tac": "1"                          // TAC → See "Location and Mobility Management" section
          }
        },
        "NSSAI - Allowed NSSAI": {
          "nas_5gs.mm.elem_id": "0x15",                 // NSSAI element ID → See "Information Element Identifiers" section
          "gsm_a.len": "2",                             // NSSAI IE length → See "Length Calculations" section
          "S-NSSAI 1": {
            "nas_5gs.mm.length": "1",                   // S-NSSAI length → See "Network Slice Selection" section
            "nas_5gs.mm.sst": "1"                       // SST (eMBB) → See "Network Slice Selection" section
          }
        },
        "5GS network feature support": {
          "nas_5gs.mm.elem_id": "0x21",                 // Network feature element ID → See "Information Element Identifiers" section
          "gsm_a.len": "2",                             // Feature support IE length → See "Length Calculations" section
          "nas_5gs.nw_feat_sup.mpsi": "0",              // MPSI indicator → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.iwk_n26": "0",           // N26 interface → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.emf": "0",               // Emergency fallback → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.emc": "0",               // Emergency services → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.vops_n3gpp": "0",        // VoPS non-3GPP → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.vops_3gpp": "1",         // VoPS 3GPP → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.5g_ciot_up": "0",        // 5G CIoT UP → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.5g_iphc_cp_ciot": "0",   // 5G IPHC CP CIoT → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.n3_data": "0",           // N3 data transfer → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.5g_cp_ciot": "0",        // 5G CP CIoT → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.restrict_ec": "0",       // Restricted EC → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.mcsi": "0",              // MCSI → See "Complete Network Feature Support Bitmap" section
          "nas_5gs.nw_feat_sup.emcn3": "0"              // EMCN3 → See "Complete Network Feature Support Bitmap" section
        },
        "GPRS Timer 3 - T3512 value": {
          "gsm_a.gm.elem_id": "0x5e",                   // Timer element ID → See "Information Element Identifiers" section
          "gsm_a.len": "1",                             // Timer IE length → See "Length Calculations" section
          "gsm_a.gm.gmm.gprs_timer3": "0x1c",           // Timer raw value → See "Timer Configuration" section
          "gsm_a.gm.gmm.gprs_timer3_tree": {
            "gsm_a.gm.gmm.gprs_timer3_unit": "4",       // Timer unit → See "Timer Configuration" section
            "gsm_a.gm.gmm.gprs_timer3_value": "18"      // Timer value → See "Timer Configuration" section
          }
        }
      }
    }
  }
}
```

## Field Derivation Logic

### Protocol Encoding Fields
1. **per.octet_string_length (46)**: Total length of the NAS PDU in octets
   - Calculated as: Security header (7) + Plain message (39) = 46 bytes
2. **nas_5gs.epd (126)**: Extended Protocol Discriminator = 0x7E
   - Always 126 (0x7E) for 5GS messages per TS 24.501
3. **nas_5gs.spare_half_octet (0)**: Always set to 0 for alignment
4. **ngap.NAS_PDU**: Complete hex-encoded NAS message
   - Starts with 7E:02 (EPD + Security header type)
   - Contains full security protected message

### NAS PDU Encoding
Complete hex breakdown of ngap.NAS_PDU:
```
7e:02:72:39:67:4c:01:7e:00:42:01:01:77:00:0b:f2:99:f9:07:02:00:40:c0:00:07:27:54:07:40:99:f9:07:00:00:01:15:02:01:01:21:02:01:00:5e:01:92
```

Breakdown by field:
- **7e**: EPD (5GS)
- **02**: Security header type (integrity protected and ciphered)
- **72:39:67:4c**: MAC (Message Authentication Code)
- **01**: Sequence number
- **7e**: Inner EPD
- **00**: Plain NAS message security header
- **42**: Registration Accept message type
- **01**: Registration result length
- **01**: Registration result value (3GPP access)
- **77**: 5G-GUTI element ID
- **00:0b**: GUTI length (11 bytes)
- **f2**: Type field with spare bits
- **99:f9:07**: PLMN (MCC=999, MNC=70)
- **02:00:40**: AMF ID (Region=2, Set=1, Pointer=0)
- **c0:00:07:27**: 5G-TMSI
- **54**: TAI list element ID
- **07**: TAI list length
- **40**: TAI list type 2 + count
- **99:f9:07**: TAI PLMN
- **00:00:01**: TAC
- **15**: NSSAI element ID
- **02**: NSSAI length
- **01**: S-NSSAI length
- **01**: SST value
- **21**: Network feature element ID
- **02**: Feature length
- **01:00**: Feature bitmap
- **5e**: T3512 element ID
- **01**: Timer length
- **92**: Timer value (unit=4, value=18)

### Information Element Identifiers
1. **0x77**: 5G-GUTI element identifier
2. **0x54**: TAI list element identifier  
3. **0x15**: Allowed NSSAI element identifier
4. **0x21**: Network feature support element identifier
5. **0x5e**: T3512 timer element identifier

### Length Calculations
1. **GUTI IE length (11)**: 
   - Type/spare byte (1) + PLMN (3) + AMF ID (3) + 5G-TMSI (4) = 11 bytes
2. **TAI list IE length (7)**:
   - Type/number byte (1) + TAI entry [PLMN (3) + TAC (2)] = 7 bytes
3. **NSSAI IE length (2)**:
   - S-NSSAI length (1) + SST (1) = 2 bytes
4. **Network feature length (2)**: Two octets of feature bitmap
5. **Timer IE length (1)**: Single octet encoding unit and value

### Static Fields (Direct Assignment)
1. **EPD (126)**: Always 0x7E for 5GS messages
2. **Security Header Type (2)**: Integrity protected and ciphered for DL
3. **Message Type (0x42)**: Registration Accept constant
4. **Registration Result (1)**: 3GPP access type

### Security Context Fields
1. **Sequence Number**: 0 → 1 (incremented for next downlink message)
2. **MAC Calculation**: New MAC using DL count, NAS integrity key, and message content
3. **Security Algorithm**: Uses established 5G-IA2 (from security mode command phase)

### Complete Registration Result Encoding
1. **gsm_a.len (1)**: Length of registration result IE = 1 byte
2. **emergency_reg (0)**: Emergency registration not performed
3. **nssaa_perf (0)**: Network slice-specific authentication not performed
4. **sms_all (0)**: SMS over NAS not allowed in this registration
5. **res (1)**: Registration result = 3GPP access

### GUTI Spare Fields
1. **nas_5gs.spare_b7 to b4**: Set to 1111b (0xF) per specification
2. **nas_5gs.spare_b3**: Set to 0 for odd/even indication
3. **Combined value**: 0xF0 (11110010b) for type field
4. **Type ID (2)**: Lower 3 bits = 010b for 5G-GUTI

### UE Identity Management
1. **IMEISV Storage**: Extract and store 4370816125816151 in UE context
2. **GUTI Allocation**:
   - PLMN ID: From serving network (MCC=999, MNC=70)
   - AMF ID: From AMF configuration (Region=2, Set=1, Pointer=0)
   - 5G-TMSI: Allocated from pool (0xC0000727 = 3221227303)
   - 3gpp.tmsi: Mirrors the 5G-TMSI value for compatibility

### Location and Mobility Management
1. **TAI List Generation**:
   - Type 2: TAIs belonging to different PLMNs
   - Single entry: PLMN(999,70) + TAC(1)
   - Based on UE's current location from RAN

### Network Slice Selection
1. **Requested NSSAI**: SST=1 from registration request
2. **Allowed NSSAI**: SST=1 (validated against subscription)
3. **Rejected NSSAI**: None in this case

### Timer Configuration
1. **T3512 (Periodic Registration Timer)**:
   - Element ID: 0x5e (GPRS Timer 3 identifier)
   - Raw value: 0x92 (combines unit and value)
   - Unit: 4 (multiples of deci hours = 6 minutes) 
   - Value: 18 (0x12 in lower 5 bits)
   - Total: 18 × 6 = 108 minutes
   - Encoding: Upper 3 bits (100b = unit 4) + Lower 5 bits (10010b = value 18)

### Complete Network Feature Support Bitmap
1. **First octet (LSB to MSB)**:
   - Bit 0: vops_n3gpp (0) - Voice over PS via non-3GPP access
   - Bit 1: vops_3gpp (1) - Voice over PS via 3GPP access supported
   - Bit 2: emc (0) - Emergency services not supported
   - Bit 3: emf (0) - Emergency fallback not supported
   - Bit 4: iwk_n26 (0) - Interworking without N26 interface
   - Bit 5: mpsi (0) - MPSI indicator not supported
   - Bits 6-7: Spare (00)
   
2. **Second octet (LSB to MSB)**:
   - Bit 0: emcn3 (0) - Emergency services via N3 not supported
   - Bit 1: mcsi (0) - MCSI not supported
   - Bit 2: restrict_ec (0) - Restricted enhanced coverage
   - Bit 3: 5g_cp_ciot (0) - 5G CP CIoT not supported
   - Bit 4: n3_data (0) - N3 data transfer not supported
   - Bit 5: 5g_iphc_cp_ciot (0) - IP header compression not supported
   - Bit 6: 5g_ciot_up (0) - 5G CIoT user plane not supported
   - Bit 7: Spare (0)

### Network Capabilities
1. **Network Feature Support**:
   - Length: 2 bytes (element ID 0x21)
   - VoPS 3GPP: 1 (Voice over PS supported)
   - Other features: 0 (not supported)
   - Encoded value: 0x02 0x00 (bit 1 set in first octet)

## Implementation Function Chain

### Phase 3: NFLambda AMF Function Implementation

```
nflambda_amf_handle_security_mode_complete()
    ↓
validate_nas_message_container()
    ↓
extract_and_store_imeisv()
    ↓
allocate_5g_guti()
    ↓
determine_tai_list()
    ↓
validate_network_slices()
    ↓
build_registration_accept()
    ↓
apply_nas_security()
    ↓
send_registration_accept()
```

### Detailed Function Specifications

#### 1. `nflambda_amf_handle_security_mode_complete()`
**Input**: Security Mode Complete NAS PDU
**Output**: Trigger registration accept flow
**Logic**:
```c
// Verify integrity protection
if (!verify_nas_integrity(nas_pdu, amf_ue)) {
    return send_security_reject();
}

// Check mandatory NAS message container
if (!(security_mode_complete->presencemask & 
      OGS_NAS_5GS_SECURITY_MODE_COMPLETE_NAS_MESSAGE_CONTAINER_PRESENT)) {
    ogs_error("No NAS Message Container");
    return OGS_ERROR;
}

// Process IMEISV if present
if (security_mode_complete->presencemask & 
    OGS_NAS_5GS_SECURITY_MODE_COMPLETE_IMEISV_PRESENT) {
    extract_and_store_imeisv(amf_ue, &security_mode_complete->imeisv);
}
```

#### 2. `validate_nas_message_container()`
**Input**: NAS message container from security mode complete
**Output**: Validation result
**Logic**:
```c
// Decode the replayed registration request
ogs_nas_5gs_decode(&nas_message, 
    nas_message_container->buffer, 
    nas_message_container->length);

// Verify it matches original registration request
if (nas_message.gmm.h.message_type != OGS_NAS_5GS_REGISTRATION_REQUEST) {
    return OGS_ERROR;
}

// Compare with stored registration request hash
if (memcmp(amf_ue->registration_request_hash, 
           calculated_hash, HASH_LEN) != 0) {
    ogs_error("Registration request mismatch");
    return OGS_ERROR;
}
```

#### 3. `extract_and_store_imeisv()`
**Input**: IMEISV mobile identity IE
**Output**: Stored IMEISV in UE context
**Logic**:
```c
// Convert BCD to buffer format
imeisv_bcd_to_buffer(&mobile_identity_imeisv->imeisv,
    amf_ue->imeisv, &amf_ue->imeisv_len);

// Create masked IMEISV (last 2 digits = software version masked)
memset(amf_ue->masked_imeisv, 0xff, OGS_MAX_IMEISV_LEN);
amf_ue->masked_imeisv_len = amf_ue->imeisv_len;
memcpy(amf_ue->masked_imeisv, amf_ue->imeisv, 
    amf_ue->imeisv_len - 1);

// Store for equipment identity register check
amf_ue->imeisv_received = true;
```

#### 4. `allocate_5g_guti()`
**Input**: AMF UE context
**Output**: New 5G-GUTI allocated
**Logic**:
```c
// Structure: GUAMI + 5G-TMSI
// GUAMI = PLMN ID + AMF ID

// Set PLMN ID from serving network
ogs_nas_from_plmn_id(&amf_ue->next.guti.nas_plmn_id,
    &amf_ue->guami->plmn_id);

// Copy AMF ID (Region + Set + Pointer)
memcpy(&amf_ue->next.guti.amf_id, 
    &amf_ue->guami->amf_id, sizeof(ogs_amf_id_t));

// Allocate new M-TMSI from pool
amf_ue->next.m_tmsi = amf_m_tmsi_alloc();
amf_ue->next.guti.m_tmsi = *(amf_ue->next.m_tmsi);

// Example allocation result:
// MCC=999, MNC=70, Region=2, Set=1, Pointer=0
// 5G-TMSI = 0xC0000727 (3221227303)
```

#### 5. `determine_tai_list()`
**Input**: UE's current TAI from RAN
**Output**: TAI list for registration area
**Logic**:
```c
// Find which configured TAI list contains UE's current TAI
served_tai_index = amf_find_served_tai(&amf_ue->nr_tai);

// Get the TAI list configuration
tai_list = &amf_self()->served_tai[served_tai_index];

// For this example: Type 2 list with single TAI
// Type 2 = TAIs belonging to different PLMNs
// TAI = PLMN(999,70) + TAC(1)

// Build TAI list IE
registration_accept->tai_list.type = OGS_TAI2_TYPE;
registration_accept->tai_list.num = 1;
registration_accept->tai_list.tai[0].plmn_id = amf_ue->nr_tai.plmn_id;
registration_accept->tai_list.tai[0].tac = amf_ue->nr_tai.tac;
```

#### 6. `validate_network_slices()`
**Input**: Requested NSSAI from registration request
**Output**: Allowed and rejected NSSAIs
**Logic**:
```c
// Get subscription data from UDM
udm_get_subscriber_data(amf_ue->supi);

// Check requested S-NSSAI against subscription
for (i = 0; i < requested_nssai->num_of_s_nssai; i++) {
    s_nssai = &requested_nssai->s_nssai[i];
    
    // Check if SST=1 (eMBB) is allowed
    if (s_nssai->sst == 1 && 
        is_subscribed_to_slice(amf_ue, s_nssai)) {
        // Add to allowed NSSAI
        allowed_nssai->s_nssai[allowed_nssai->num_of_s_nssai++] = *s_nssai;
    } else {
        // Add to rejected NSSAI with cause
        rejected_nssai->s_nssai[rejected_nssai->num_of_s_nssai].sst = s_nssai->sst;
        rejected_nssai->rejection_cause[rejected_nssai->num_of_s_nssai] = 
            OGS_5GMM_CAUSE_S_NSSAI_NOT_AVAILABLE_IN_PLMN;
        rejected_nssai->num_of_s_nssai++;
    }
}
```

#### 7. `build_registration_accept()`
**Input**: UE context with all derived parameters
**Output**: Registration Accept NAS PDU
**Logic**:
```c
// Set message headers
message.h.security_header_type = 
    OGS_NAS_SECURITY_HEADER_INTEGRITY_PROTECTED_AND_CIPHERED;
message.gmm.h.message_type = OGS_NAS_5GS_REGISTRATION_ACCEPT;

// Set registration result
registration_accept->registration_result.access_type = 
    OGS_5GS_REGISTRATION_TYPE_3GPP_ACCESS;

// Include 5G-GUTI (mandatory for initial registration)
registration_accept->presencemask |= 
    OGS_NAS_5GS_REGISTRATION_ACCEPT_5G_GUTI_PRESENT;
ogs_nas_5gs_nas_guti_to_mobility_identity_guti(
    &amf_ue->next.guti, &registration_accept->guti);

// Include TAI list (mandatory)
ogs_nas_5gs_tai_list_build(&registration_accept->tai_list,
    &amf_self()->served_tai[served_tai_index]);

// Include allowed NSSAI
if (amf_ue->allowed_nssai.num_of_s_nssai) {
    registration_accept->presencemask |= 
        OGS_NAS_5GS_REGISTRATION_ACCEPT_ALLOWED_NSSAI_PRESENT;
    allowed_nssai->length = ogs_nas_build_nssai(
        allowed_nssai->buffer, &amf_ue->allowed_nssai);
}

// Set T3512 timer (periodic registration update)
registration_accept->presencemask |= 
    OGS_NAS_5GS_REGISTRATION_ACCEPT_T3512_VALUE_PRESENT;
registration_accept->t3512_value.unit = 
    OGS_NAS_GRPS_TIMER_3_UNIT_MULTIPLES_OF_DECI_HOURS;
registration_accept->t3512_value.value = 18; // 108 minutes

// Set network feature support
registration_accept->presencemask |= 
    OGS_NAS_5GS_REGISTRATION_ACCEPT_5GS_NETWORK_FEATURE_SUPPORT_PRESENT;
network_feature_support->length = 2;
network_feature_support->ims_vops_3gpp = 1; // VoPS supported
```

#### 8. `apply_nas_security()`
**Input**: Plain NAS message
**Output**: Security protected NAS message
**Logic**:
```c
// Increment downlink NAS count
amf_ue->dl_count.i32++;  // Now equals 1

// Apply integrity protection
ogs_nas_mac_calculate(
    amf_ue->selected_int_algorithm,    // 5G-IA2
    amf_ue->knas_int,                  // NAS integrity key
    amf_ue->dl_count.i32,              // Count = 1
    0x01,                              // Access type (3GPP)
    OGS_NAS_SECURITY_DOWNLINK_DIRECTION,
    &nas_message,
    mac                                // Output: 0x7239674c
);

// Apply ciphering if not NULL algorithm
if (amf_ue->selected_enc_algorithm != OGS_NAS_SECURITY_ALGORITHMS_5G_EA0) {
    ogs_nas_encrypt(
        amf_ue->selected_enc_algorithm,
        amf_ue->knas_enc,
        amf_ue->dl_count.i32,
        0x01,
        OGS_NAS_SECURITY_DOWNLINK_DIRECTION,
        &nas_message
    );
}

// Set security header
security_header->sequence_number = amf_ue->dl_count.overflow & 0xff;
```

#### 9. `send_registration_accept()`
**Input**: Security protected Registration Accept
**Output**: Message sent to RAN
**Logic**:
```c
// Build NGAP message
ngap_send_downlink_nas_transport(
    amf_ue->ran_ue,
    amf_ue->nas.native_ksi,
    nas_pdu);

// Update UE state
amf_ue->nas.connection.state = AMF_NAS_REGISTERED;

// Activate GUTI
amf_ue->current.guti = amf_ue->next.guti;
amf_ue->next.m_tmsi = NULL;

// Start T3550 timer for registration complete
amf_ue->t3550.timer = ogs_timer_add(
    ogs_app()->timer_mgr,
    amf_timer_t3550_expire, amf_ue);
ogs_timer_start(amf_ue->t3550.timer, 
    amf_timer_cfg(AMF_TIMER_T3550)->duration);
```

## Detailed Field Calculations

### 5G-GUTI Structure Breakdown
```
Total: 80 bits (10 bytes)
├── PLMN ID: 24 bits
│   ├── MCC: 12 bits (999 = 0x999 in BCD)
│   └── MNC: 12 bits (70 = 0x70F in BCD, F=filler)
├── AMF ID: 24 bits
│   ├── AMF Region ID: 8 bits (2 = 0x02)
│   ├── AMF Set ID: 10 bits (1 = 0x001)
│   └── AMF Pointer: 6 bits (0 = 0x00)
└── 5G-TMSI: 32 bits (0xC0000727)
```

**Encoding Example**:
```
PLMN: 99 F9 07 (BCD encoded 999-70)
AMF ID: 02 00 40 (Region=2, Set=1, Pointer=0)
5G-TMSI: C0 00 07 27
```

### TAI List Type 2 Encoding
```
Type 2: TAIs belonging to different PLMNs
├── Type/Number: 1 byte
│   ├── Type: 2 bits (10b = Type 2)
│   └── Number: 5 bits (00000b = 1 TAI)
└── TAI entries: 5 bytes each
    ├── PLMN ID: 3 bytes (99 F9 07)
    └── TAC: 2 bytes (00 01)
```

### T3512 Timer Encoding (GPRS Timer 3)
```
Format: 1 byte
├── Unit: 3 bits (100b = deci hours)
└── Value: 5 bits (10010b = 18)

Calculation: 18 × 6 minutes = 108 minutes
```

### NSSAI Encoding
```
Allowed NSSAI:
├── Length: 1 byte (2)
└── S-NSSAI[0]: 2 bytes
    ├── Length: 1 byte (1)
    └── SST: 1 byte (1 = eMBB)
```

### Network Feature Support Encoding
```
2 bytes bitmap:
Byte 0: Basic features
├── Bit 0: IMS VoPS N3GPP (0)
├── Bit 1: IMS VoPS 3GPP (1)
├── Bit 2: Emergency service (0)
├── Bit 3: Emergency fallback (0)
├── Bit 4: IWK N26 (0)
├── Bit 5: MPSI (0)
└── Bits 6-7: Spare (0)

Byte 1: Extended features (all 0)
```

## Binary Message Structures

### Security Mode Complete Message Structure

```c
#pragma pack(1)  // Ensure no padding between fields

// Security Mode Complete structure matching the binary layout
// Hex: 7e:04:22:e4:ee:19:00:7e:00:5e:77:00:09:45:73:80:61:21:85:61:51:f1:71:00:23:7e:00:41:79:00:0d:01:99:f9:07:00:00:00:00:00:00:00:00:10:10:01:00:2e:04:80:f0:80:f0:2f:02:01:01:53:01:00
typedef struct {
    // Security Header (7 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header_type;       // 0x04 - Integrity protected and ciphered (bits 7-4: spare, bits 3-0: type)
    uint32_t mac;                       // 0x22E4EE19 - Message Authentication Code (big-endian)
    uint8_t sequence_number;            // 0x00 - Sequence number
    
    // Plain NAS Message Header (3 bytes)
    uint8_t inner_epd;                  // 0x7E - Inner EPD
    uint8_t inner_security_header;      // 0x00 - Plain NAS message
    uint8_t message_type;               // 0x5E - Security Mode Complete
    
    // IMEISV Mobile Identity (Optional IE)
    uint8_t imeisv_iei;                 // 0x77 - IMEISV IEI
    uint8_t imeisv_spare_length;        // 0x00 - Spare half octet
    uint8_t imeisv_length;              // 0x09 - Length (9 bytes)
    union {
        uint8_t imeisv_header;          // 0x45
        struct {
            uint8_t type_id:3;          // bits 2-0: Identity type (5 = IMEISV)
            uint8_t odd_even:1;         // bit 3: Odd/even indication (0)
            uint8_t spare:4;            // bits 7-4: Spare
        } bits;
    } imeisv_hdr;
    
    // IMEISV digits in BCD format (8 bytes)
    struct {
        uint8_t tac_digits[4];          // TAC: 43708161 (stored as 0x73:80:61:21)
        uint8_t snr_digits[3];          // SNR: 258165 (stored as 0x85:61:51)
        uint8_t sv_digit;               // SV: 15 (stored as 0xF1, F=filler)
    } imeisv_bcd;
    
    // NAS Message Container
    uint8_t nas_container_iei;          // 0x71 - NAS message container IEI
    uint8_t nas_container_length;       // 0x00 - Spare half octet
    uint8_t nas_container_len_value;    // 0x23 - Length (35 bytes)
    
    // Replayed Registration Request (35 bytes)
    struct {
        uint8_t epd;                    // 0x7E
        uint8_t security_header;        // 0x00
        uint8_t message_type;           // 0x41 - Registration Request
        
        // Registration type and ngKSI
        union {
            uint8_t reg_type_and_ksi;   // 0x79
            struct {
                uint8_t reg_type:3;     // bits 2-0: Registration type (1)
                uint8_t for_bit:1;      // bit 3: Follow-on request (1)
                uint8_t ksi:3;          // bits 6-4: Key set identifier (7)
                uint8_t tsc:1;          // bit 7: Type of security context (0)
            } bits;
        } reg_type_ksi;
        
        // Mobile Identity header
        uint8_t spare_half_octet;       // 0x00
        uint8_t mobile_id_length;       // 0x0D - Length (13 bytes)
        
        // SUCI header
        union {
            uint8_t suci_header;        // 0x01
            struct {
                uint8_t type_id:3;      // bits 2-0: Identity type (1 = SUCI)
                uint8_t spare_b3:1;     // bit 3: Spare
                uint8_t supi_format:3;  // bits 6-4: SUPI format (0)
                uint8_t spare_b7:1;     // bit 7: Spare
            } bits;
        } suci_hdr;
        
        // PLMN (3 bytes BCD)
        uint8_t plmn[3];                // 0x99:F9:07
        
        // Routing indicator
        uint16_t routing_indicator;     // 0x0000
        
        // Protection scheme
        uint8_t protection_scheme;      // 0x00
        
        // MSIN (5 bytes BCD)
        uint8_t msin[5];                // 0x00:00:00:00:10
        
        // 5GMM Capability
        uint8_t gmm_cap_iei;            // 0x10
        uint8_t gmm_cap_length;         // 0x01
        uint8_t gmm_capability;         // 0x00
        
        // UE Security Capability
        uint8_t ue_sec_cap_iei;         // 0x2E
        uint8_t ue_sec_cap_length;      // 0x04
        uint8_t sec_algorithms[4];      // 0x80:F0:80:F0
        
        // Requested NSSAI
        uint8_t nssai_iei;              // 0x2F
        uint8_t nssai_length;           // 0x02
        uint8_t s_nssai_length;         // 0x01
        uint8_t sst;                    // 0x01
        
        // 5GS Update Type
        uint8_t update_type_iei;        // 0x53
        uint8_t update_type_length;     // 0x01
        uint8_t update_type_value;      // 0x00
    } registration_request;
} security_mode_complete_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(security_mode_complete_t) == 60, "Security Mode Complete structure size mismatch");
```

### Registration Accept Message Structure

```c
// Registration Accept structure matching the binary layout
// Hex: 7e:02:72:39:67:4c:01:7e:00:42:01:01:77:00:0b:f2:99:f9:07:02:00:40:c0:00:07:27:54:07:40:99:f9:07:00:00:01:15:02:01:01:21:02:01:00:5e:01:92
typedef struct {
    // Security Header (7 bytes)
    uint8_t epd;                        // 0x7E - Extended Protocol Discriminator
    uint8_t security_header_type;       // 0x02 - Integrity protected and ciphered
    uint32_t mac;                       // 0x7239674C - Message Authentication Code (big-endian)
    uint8_t sequence_number;            // 0x01 - Sequence number
    
    // Plain NAS Message Header (3 bytes)
    uint8_t inner_epd;                  // 0x7E - Inner EPD
    uint8_t inner_security_header;      // 0x00 - Plain NAS message
    uint8_t message_type;               // 0x42 - Registration Accept
    
    // 5GS Registration Result (1 byte)
    uint8_t reg_result_length;          // 0x01 - Length
    union {
        uint8_t reg_result_value;       // 0x01
        struct {
            uint8_t reg_result:3;       // bits 2-0: Registration result (1 = 3GPP access)
            uint8_t sms_allowed:1;      // bit 3: SMS allowed (0)
            uint8_t nssaa_perf:1;       // bit 4: NSSAA performed (0)
            uint8_t emergency_reg:1;    // bit 5: Emergency registered (0)
            uint8_t spare:2;            // bits 7-6: Spare
        } bits;
    } registration_result;
    
    // 5G-GUTI
    uint8_t guti_iei;                   // 0x77 - 5G-GUTI IEI
    uint8_t guti_spare_length;          // 0x00 - Spare half octet
    uint8_t guti_length;                // 0x0B - Length (11 bytes)
    
    union {
        uint8_t guti_header;            // 0xF2
        struct {
            uint8_t type_id:3;          // bits 2-0: Identity type (2 = 5G-GUTI)
            uint8_t spare_b3:1;         // bit 3: Spare (0)
            uint8_t spare_b4:1;         // bit 4: Spare (1)
            uint8_t spare_b5:1;         // bit 5: Spare (1)
            uint8_t spare_b6:1;         // bit 6: Spare (1)
            uint8_t spare_b7:1;         // bit 7: Spare (1)
        } bits;
    } guti_hdr;
    
    // GUAMI components
    uint8_t plmn[3];                    // 0x99:F9:07 - PLMN (MCC=999, MNC=70)
    struct {
        uint8_t amf_region_id;          // 0x02 - AMF Region ID
        uint16_t amf_set_id_and_ptr;   // 0x0040 - AMF Set ID (10 bits) + Pointer (6 bits)
    } amf_id;
    uint32_t _5g_tmsi;                  // 0xC0000727 - 5G-TMSI (big-endian)
    
    // TAI List
    uint8_t tai_list_iei;               // 0x54 - TAI list IEI
    uint8_t tai_list_length;            // 0x07 - Length (7 bytes)
    
    union {
        uint8_t tai_list_type_num;      // 0x40
        struct {
            uint8_t num_elements:5;     // bits 4-0: Number of elements (0 = 1 TAI)
            uint8_t spare:1;            // bit 5: Spare
            uint8_t list_type:2;        // bits 7-6: TAI list type (2)
        } bits;
    } tai_type;
    
    // TAI entry
    uint8_t tai_plmn[3];                // 0x99:F9:07 - TAI PLMN
    uint16_t tac;                       // 0x0001 - Tracking Area Code (big-endian)
    
    // Allowed NSSAI
    uint8_t nssai_iei;                  // 0x15 - Allowed NSSAI IEI
    uint8_t nssai_length;               // 0x02 - Length
    uint8_t s_nssai_length;             // 0x01 - S-NSSAI length
    uint8_t sst;                        // 0x01 - SST (eMBB)
    
    // 5GS Network Feature Support
    uint8_t nw_feat_iei;                // 0x21 - Network feature support IEI
    uint8_t nw_feat_length;             // 0x02 - Length
    
    union {
        uint8_t nw_feat_octet1;         // 0x01
        struct {
            uint8_t ims_vops_n3gpp:1;   // bit 0: IMS VoPS non-3GPP (0)
            uint8_t ims_vops_3gpp:1;    // bit 1: IMS VoPS 3GPP (1)
            uint8_t emc:1;              // bit 2: Emergency services (0)
            uint8_t emf:1;              // bit 3: Emergency fallback (0)
            uint8_t iwk_n26:1;          // bit 4: N26 interface (0)
            uint8_t mpsi:1;             // bit 5: MPSI (0)
            uint8_t spare:2;            // bits 7-6: Spare
        } bits;
    } network_features_1;
    
    union {
        uint8_t nw_feat_octet2;         // 0x00
        struct {
            uint8_t emcn3:1;            // bit 0: Emergency N3 (0)
            uint8_t mcsi:1;             // bit 1: MCSI (0)
            uint8_t restrict_ec:1;      // bit 2: Restricted EC (0)
            uint8_t _5g_cp_ciot:1;      // bit 3: 5G CP CIoT (0)
            uint8_t n3_data:1;          // bit 4: N3 data (0)
            uint8_t _5g_iphc_cp_ciot:1; // bit 5: 5G IPHC CP CIoT (0)
            uint8_t _5g_ciot_up:1;      // bit 6: 5G CIoT UP (0)
            uint8_t spare:1;            // bit 7: Spare
        } bits;
    } network_features_2;
    
    // T3512 Timer
    uint8_t t3512_iei;                  // 0x5E - T3512 IEI
    uint8_t t3512_length;               // 0x01 - Length
    union {
        uint8_t timer_value;            // 0x92
        struct {
            uint8_t value:5;            // bits 4-0: Timer value (18)
            uint8_t unit:3;             // bits 7-5: Timer unit (4 = deci hours)
        } bits;
    } t3512;
} registration_accept_t;

// Static assert to ensure structure size matches message
_Static_assert(sizeof(registration_accept_t) == 46, "Registration Accept structure size mismatch");
```
