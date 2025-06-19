//
// Created by ziyan on 6/10/25.
//

#include "amf.h"


/*
 * generate_auth_req - Generates an Authentication Request NAS message
 * Input NAS PDU: 7e004179000d0199f9070000000000000000102e0480f080f0 (Initial Registration Request)
 * Output NAS PDU: 7e005600020000215ca0df8c9bb8dbcf3c2a7dd448da13692010406296993082800030b762455c890b19 (Authentication Request)
 */
EVENT_HANDLER(generate_auth_req)
{

    // The call graph that replaces `EVENT_HANDLER(generate_auth_req)` flows through the network-function layers in this order: **amf\_process\_registration\_request() → udm\_decrypt\_suci() → ausf\_initiate\_authentication() → udm\_generate\_5g\_aka\_vector() → ausf\_prepare\_hxres\_star() → amf\_build\_auth\_request\_pdu() → amf\_send\_downlink\_nas\_transport()**.

    // `amf_process_registration_request()` parses the plain Registration Request and caches a provisional UE context. `udm_decrypt_suci()` recovers the SUPI from the SUCI using the home-network private key. `ausf_initiate_authentication()` raises a Nausf\_UEAuthentication request that in turn triggers `udm_generate_5g_aka_vector()`, where the RAND, AUTN, XRES\* and Kseaf are calculated from the long-term key K. Back at the AUSF, `ausf_prepare_hxres_star()` stores HXRES\* and returns RAND + AUTN to the AMF. The AMF then calls `amf_build_auth_request_pdu()` to concatenate the header, ABBA, RAND and AUTN fields exactly as in your original snippet, and finally `amf_send_downlink_nas_transport()` wraps that PDU in a Downlink NAS Transport NGAP message and hands it to the gNB for delivery to the UE.

    // Prepare parts using C-style character arrays (modifiable).
    char epd_header[] = "7e00"; // Extended Protocol Discriminator (126) + spare + security header type (plain)
    char msg_type[] = "56";     // Message Type = Authentication Request
    char ngksi_field[] = "00";  // NAS Key Set Identifier (ngKSI) + spare half-octet (native, id=0)
    char abba_ie[] = "020000";  // ABBA IE: length 2, value 0x0000
    char rand_iei[] = "21";     // IEI for RAND
    char rand_val[] = "5ca0df8c9bb8dbcf3c2a7dd448da1369"; // 128-bit RAND challenge (example)
    char autn_iei[] = "20";                               // IEI for AUTN
    char autn_len[] = "10";                               // AUTN length = 16 bytes (0x10)
    char autn_val[] = "406296993082800030b762455c890b19"; // AUTN value (SQN⊕AK|AMF|MAC)

    char *parts[] = {epd_header, msg_type, ngksi_field, abba_ie, rand_iei, rand_val, autn_iei, autn_len, autn_val};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, part, len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0'; // null-terminate
}



/*
 * generate_security_cmd - Generates a Security Mode Command NAS message
 * Input NAS PDU: 7e00572d10ef2770c69e7382aa38e8134f602234e1 (Authentication Response)
 * Output NAS PDU: 7e0313bf995a007e005d02000480f080f0e1360102 (Security Mode Command)
 */
EVENT_HANDLER(generate_security_cmd)
{

    // The execution path that replaces `EVENT_HANDLER(generate_security_cmd)` stays entirely inside the AMF because, once authentication has succeeded, only the AMF needs to derive keys and create the **Security Mode Command**.  The functions are invoked in this strict order: **amf\_handle\_auth\_response() → amf\_verify\_res\_star() → amf\_derive\_kamf() → amf\_select\_security\_algorithms() → amf\_derive\_nas\_keys() → amf\_build\_security\_mode\_command\_pdu() → amf\_compute\_mac\_and\_wrap\_security\_container() → amf\_send\_downlink\_nas\_transport()**.

    // When the UE’s Authentication Response reaches the AMF, `amf_handle_auth_response()` lifts the NAS container, hands the RES\* value to `amf_verify_res_star()`, and that routine compares it with the previously stored HXRES\* to prove possession of the long-term key.  A positive match lets `amf_derive_kamf()` run the 5G-AKA key-derivation function on KSEAF, the serving-network name and the SUPI to obtain the anchor key KAMF.  Next, `amf_select_security_algorithms()` consults the UE-supplied capability bitmap and local policy to pick the integrity algorithm **EIA2** (id 2) and keep ciphering disabled (id 0).  With those choices, `amf_derive_nas_keys()` derives the downlink integrity key KNASint and, if ciphering were enabled, KNASenc as well.

    // `amf_build_security_mode_command_pdu()` then concatenates the inner plain NAS header **7e00**, the message-type byte **5d**, the chosen algorithm octet **02**, the ngKSI **00**, the UE-security-capability TLV and the optional IMEISV-request and Additional-5G-Security-Information IEs exactly as in your original array.  Because the message must be integrity-protected, `amf_compute_mac_and_wrap_security_container()` prepends the outer security header **7e03**, inserts the freshly generated 32-bit MAC and sequence number, and records the same sequence number in the UE context for replay detection.  Finally, `amf_send_downlink_nas_transport()` places the completed hex string in the NGAP *NAS-PDU* field of a **Downlink NASTransport** message, which the gNB forwards to the UE, marking the moment when both sides switch to protected NAS signalling.


    // Prepare parts using C-style character arrays (modifiable).
    char outer_header[] = "7e03"; // Extended Protocol Discriminator + Security header type (3)
    char mac[] = "13bf995a";      // Message authentication code (example)
    char seq_num[] = "00";        // Sequence number

    char inner_header[] = "7e00"; // Extended Protocol Discriminator + Security header type (0)
    char msg_type[] = "5d";       // Message Type = Security Mode Command

    char nas_alg[] = "02";     // NAS security algorithms: ciphering (0), integrity (2)
    char ngksi_field[] = "00"; // NAS Key Set Identifier (ngKSI) + spare half-octet

    char ue_sec_cap_len[] = "04";       // UE security capability length
    char ue_sec_cap_val[] = "80f080f0"; // UE security capabilities bitmap

    char imeisv_req[] = "e1";       // IMEISV request IE: ID (0xe), value (0x1)
    char add_sec_info[] = "360102"; // Additional 5G security information: IEI (0x36), length (1), value (0x02)

    char *parts[] = {outer_header,   mac,        seq_num,     inner_header,
                     msg_type,       nas_alg,    ngksi_field, ue_sec_cap_len,
                     ue_sec_cap_val, imeisv_req, add_sec_info};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, part, len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0'; // null-terminate
}

/*
 * generate_registration_accept - Generates a Registration Accept NAS message
 * Input NAS PDU: 7e0422e4ee19007e005e7700094573806121856151f17100237e004179000d0199f9070000000000000000101001002e0480f080f02f020101530100 (Security Mode Complete)
 * Output NAS PDU: 7e027239674c017e0042010177000bf299f907020040c000072754074099f90700000115020101210201005e0192 (Registration Accept)
 */
EVENT_HANDLER(generate_registration_accept)
{

    // After the UE returns Security Mode Complete, the AMF continues the mobility-management procedure entirely inside its own process context. First it invokes **amf\_handle\_security\_mode\_complete()**, which flips the UE context to “MM-REGISTER-PENDING” and notes the negotiated NAS algorithms and KAMF. Because the AMF now needs subscription, slice and timer details, it calls **nudm\_get\_subscription\_data()** to fetch the UE’s service profile, authentication status and registration timers from the UDM; that data arrives securely over the service-based Nudm interface. Using the subscription’s slice-selection information, the AMF next triggers **nssf\_select\_allowed\_slices()** so the Network Slice Selection Function can filter and/or reorder the list of Single NSSAI values that are permitted in the current PLMN; the result forms the Allowed NSSAI IE that will appear in the NAS message. With the slice choice settled, **amf\_allocate\_5g\_guti()** constructs a fresh 5G-GUTI by concatenating the serving PLMN identifier, the AMF region and set IDs, and a locally unique temporary mobile identifier; it also compiles the Tracking Area List and decides on the T3512 periodic-registration timer, honouring any policy hints received from the PCF. Now the actual NAS payload is produced by **amf\_build\_registration\_accept\_pdu()**, which strings together the inner plain-NAS header “7e00”, the message-type byte “42”, the Registration-Result IE, the GUTI IE, the TAI list, the Allowed NSSAI, the Network-Feature-Support IE and the T3512 timer exactly as in your original C arrays. Because the UE is already in secured NAS mode, **amf\_compute\_mac\_and\_wrap\_security\_container()** wraps this plaintext in a security container: it prepends the outer header “7e02”, computes the 32-bit MAC with KNASint, inserts an incremented sequence number “01” and records that SQN for replay detection. Finally, **amf\_send\_downlink\_nas\_transport()** places the completed hex string in the NAS-PDU field of a Downlink NAS Transport NGAP message and hands it to the gNB, which delivers the Registration Accept to the UE and moves the UE context to the “MM-REGISTERED” state.

    /* Outer (security-protected) header */
    char outer_header[] = "7e02"; /* EPD 0x7e, security hdr type 0x02 */
    char mac[] = "7239674c";      /* 32-bit message authentication code */
    char seq_num[] = "01";        /* Sequence number */

    /* Inner plain NAS header */
    char inner_header[] = "7e00"; /* EPD 0x7e, plain message */
    char msg_type[] = "42";       /* Registration Accept */

    /* Information elements */
    char reg_result[] = "0101";                      /* IEI 0x01, value 0x01 */
    char guti_ie[] = "77000bf299f907020040c0000727"; /* 5G-GUTI, length 0x000b */
    char tai_list[] = "54074099f907000001";          /* TAI list, length 0x07 */
    char nssai[] = "15020101";                       /* Allowed NSSAI */
    char net_feat[] = "21020100";                    /* Network feature support */
    char gprs_timer[] = "5e0192";                    /* T3512 value: 540 s */

    /* Assemble */
    char *parts[] = {outer_header, mac,      seq_num, inner_header, msg_type,  reg_result,
                     guti_ie,      tai_list, nssai,   net_feat,     gprs_timer};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, parts[i], len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
}

/*
 * generate_configuration_update - Generates a Configuration Update Command NAS message
 * Input NAS PDU: 7e02469d6a8b017e0043 (Registration Complete)
 * Output NAS PDU: 7e02de0d22e3027e0054430f90004f00700065006e003500470053450990004e006500780074460a475260903035530a490101 (Configuration Update Command)
 */
EVENT_HANDLER(generate_configuration_update)
{

    //     When the AMF receives the NAS **PDU Session Establishment Request** from the UE, it first passes control to `amf_handle_pdu_session_establishment_request()`, which validates the request and calls `amf_select_smf()` to discover a suitable SMF instance via NRF.  `amf_select_smf()` itself issues an **Nnrf\_Discover** query, then invokes the **Nsmf\_PDUSession\_CreateSMContext** service operation on the chosen SMF; the asynchronous result returns a provisional SM-context reference.  Directly afterward the AMF asks the NSSF to verify that the requested Single NSSAI can be served in the current tracking area by calling `nssf_select_allowed_slices()`; if the slice is admitted, the AMF proceeds.

    // Inside the SMF a paired routine `smf_create_session_context()` contacts the UDM through **Nudm\_SubscriberDataManagement\_GetSessionManagementData** to pull the subscriber’s session-level subscription profile and uses `pcf_get_policy_for_session()` (an **Npcf\_SMPolicyControl\_Create** transaction) to obtain dynamic policy—including whether the UE should be informed of operator-specific display strings, local time-zone parameters or daylight-saving rules.  PCF delivers those values in the SM Policy Decision, and `smf_apply_session_policy()` stores them in the SM-context before handing the result back to the AMF via **Nsmf\_PDUSession\_UpdateSMContext**.

    // Once the AMF receives that update, `amf_prepare_configuration_parameters()` extracts the Full and Short network names, the current local time zone, the current network time and the applicable daylight-saving offset from the policy blob.  Those parameters are cached in the UE context, after which `amf_build_configuration_update_pdu()` concatenates the inner plain-NAS header “7e00”, the message-type byte “54”, and the five Information Elements exactly as you coded them.  Because NAS signalling is already integrity-protected and ciphered, `amf_compute_mac_and_wrap_security_container()` prepends the outer header “7e02”, computes the 32-bit MAC with KNASint, inserts the next sequence number “02” and encrypts the payload with KNASenc where required.  Finally, `amf_send_downlink_nas_transport()` embeds the finished hex string in the NGAP **Downlink NASTransport** message that the gNB relays to the UE.  Only after this Configuration Update Command is acknowledged does the SMF continue toward the PDU Session Establishment Accept, but all of the network-function work that produces the configuration message is contained in the chain:

    // `amf_handle_pdu_session_establishment_request() → amf_select_smf() → nssf_select_allowed_slices() → smf_create_session_context() → pcf_get_policy_for_session() → smf_apply_session_policy() → amf_prepare_configuration_parameters() → amf_build_configuration_update_pdu() → amf_compute_mac_and_wrap_security_container() → amf_send_downlink_nas_transport()`.

    /* outer security-protected header */
    char outer_header[] = "7e02"; /* EPD 0x7e, integrity-ciphered (2)       */
    char mac[] = "de0d22e3";      /* message authentication code            */
    char seq_num[] = "02";        /* sequence number                        */

    /* inner plain NAS header */
    char inner_header[] = "7e00"; /* EPD 0x7e, plain message                */
    char msg_type[] = "54";       /* configuration update command           */

    /* information elements */
    char net_name_full[] = "430f90004f00700065006e003500470053"; /* “Open5GS” */
    char net_name_short[] = "450990004e006500780074";            /* “Next”    */
    char tz_local[] = "460a";                                    /* GMT-5     */
    char tz_and_time[] = "475260903035530a";                     /* 2025-06-09 03:53:35, GMT-5 */
    char dst[] = "490101";                                       /* DST +1 h  */

    char *parts[] = {outer_header,   mac,      seq_num,     inner_header, msg_type, net_name_full,
                     net_name_short, tz_local, tz_and_time, dst};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (offset + len >= MAX_NAS_HEX_LEN)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, parts[i], len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
}

/*
 * generate_pdu_session_establishment - Generates a PDU Session Establishment Accept message
 * Input NAS PDU: 7e02ba0292cd027e00670100152e0101c1ffff91a12801007b000780000a00000d00120181220101250908696e7465726e6574 (PDU Session Establishment Request)
 * Output NAS PDU: 7e02fbd62d81037e00680100472e0101c211000901000631310101ff010603f42403f4242905010a2d00022201017900060120410101097b000f80000d0408080808000d0408080404250908696e7465726e65741201 (PDU Session Establishment Accept)
 */
EVENT_HANDLER(generate_pdu_session_establishment)
{
    /* outer security-protected header */
    char outer_hdr[] = "7e02"; /* EPD 0x7e, integrity-protected & ciphered (2) */
    char mac[] = "fbd62d81";   /* message authentication code                  */
    char seq[] = "03";         /* sequence number                              */

    /* inner plain NAS header (DL NAS Transport) */
    char inner_hdr[] = "7e00"; /* EPD 0x7e, plain message                      */
    char dl_msg[] = "68";      /* DL NAS transport                             */

    /* DL transport payload-container header */
    char spare_pt[] = "01"; /* spare half-octet + payload-container type N1SM */
    char pc_len[] = "0047"; /* payload-container length = 0x47 (= 71)          */

    /* N1 SM payload: PDU Session Establishment Accept */
    char sm_epd[] = "2e";                                 /* EPD 0x2e (SM)            */
    char pdu_id[] = "01";                                 /* PDU Session ID           */
    char pti[] = "01";                                    /* Procedure Tx ID          */
    char sm_msg[] = "c2";                                 /* Establishment Accept     */
    char ssc_pdu[] = "11";                                /* SSC mode 1, IPv4         */
    char qos_rule[] = "000901000631310101ff01";           /* authorised QoS rule      */
    char ambr[] = "0603f42403f424";                       /* Session-AMBR 1 000 000 kbps */
    char pdu_addr[] = "2905010a2d0002";                   /* IPv4 address 10.45.0.2   */
    char snssai[] = "220101";                             /* SST = eMBB               */
    char qos_flow[] = "790006012041010109";               /* QoS flow 5QI = 9         */
    char epco[] = "7b000f80000d0408080808000d0408080404"; /* DNS 8.8.8.8 & 8.8.4.4    */
    char dnn[] = "250908696e7465726e6574";                /* DNN “internet”           */
    char pdu_id_ie[] = "1201";                            /* PDU Session ID (IE)      */

    char *parts[] = {outer_hdr, mac,     seq,      inner_hdr, dl_msg,   spare_pt, pc_len,   sm_epd, pdu_id, pti,
                     sm_msg,    ssc_pdu, qos_rule, ambr,      pdu_addr, snssai,   qos_flow, epco,   dnn,    pdu_id_ie};

    size_t off = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (off + len >= MAX_NAS_HEX_LEN)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + off, parts[i], len);
        off += len;
    }
    EVENT_PAYLOAD[off] = '\0';
}
