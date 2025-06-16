#include "core5g.h"
#include <string.h>

EventNf event_nf;
EVENT_HANDLER(generate_auth_req)
{
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

EVENT_HANDLER(generate_security_cmd)
{
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

EVENT_HANDLER(generate_registration_accept)
{
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

EVENT_HANDLER(generate_configuration_update)
{
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
