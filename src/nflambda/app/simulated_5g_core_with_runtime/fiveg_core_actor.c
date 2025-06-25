#include "fiveg_core_actor.h"
#include "amf_common.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Actor state
static ue_state_t ue_state = UE_STATE_DEREGISTERED;
static bool stopped = false;
static int sequence_number = 0;

// Initialize the 5G Core actor
void fiveg_core_init(void)
{
    printf("[5G Core] Actor initialized\n");
    ue_state = UE_STATE_DEREGISTERED;
    stopped = false;
    sequence_number = 0;
}

// Utility function to print NAS PDU
void print_nas_pdu(const char* label, const char* pdu)
{
    printf("[5G Core] %s: %s\n", label, pdu);
}

// Handler for Registration Request
EVENT_HANDLER(handle_registration_request)
{
    printf("[5G Core] Received Registration Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Authentication Request
    char epd_header[] = "7e00";
    char msg_type[] = "56";
    char ngksi_field[] = "00";
    char abba_ie[] = "020000";
    char rand_iei[] = "21";
    char rand_val[] = "5ca0df8c9bb8dbcf3c2a7dd448da1369";
    char autn_iei[] = "20";
    char autn_len[] = "10";
    char autn_val[] = "406296993082800030b762455c890b19";

    char *parts[] = {epd_header, msg_type, ngksi_field, abba_ie, rand_iei, rand_val, autn_iei, autn_len, autn_val};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, part, len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Auth Request", EVENT_PAYLOAD);
    ue_state = UE_STATE_REGISTERING;
    
    // Trigger authentication request event
    trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Authentication Response
EVENT_HANDLER(handle_auth_response)
{
    printf("[5G Core] Received Authentication Response\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Security Mode Command
    char outer_header[] = "7e03";
    char mac[] = "13bf995a";
    char seq_num[] = "00";
    char inner_header[] = "7e00";
    char msg_type[] = "5d";
    char nas_alg[] = "02";
    char ngksi_field[] = "00";
    char ue_sec_cap_len[] = "04";
    char ue_sec_cap_val[] = "80f080f0";
    char imeisv_req[] = "e1";
    char add_sec_info[] = "360102";

    char *parts[] = {outer_header, mac, seq_num, inner_header, msg_type, nas_alg, ngksi_field, 
                     ue_sec_cap_len, ue_sec_cap_val, imeisv_req, add_sec_info};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, part, len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Security Mode Command", EVENT_PAYLOAD);
    ue_state = UE_STATE_AUTHENTICATED;
    sequence_number++;
    
    // Trigger security mode command event
    trigger_event(EVENT_SECURITY_MODE_COMMAND, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Security Mode Complete
EVENT_HANDLER(handle_security_mode_complete)
{
    printf("[5G Core] Received Security Mode Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Registration Accept
    char outer_header[] = "7e02";
    char mac[] = "7239674c";
    char seq_num[] = "01";
    char inner_header[] = "7e00";
    char msg_type[] = "42";
    char reg_result[] = "0101";
    char guti_ie[] = "77000bf299f907020040c0000727";
    char tai_list[] = "54074099f907000001";
    char nssai[] = "15020101";
    char net_feat[] = "21020100";
    char gprs_timer[] = "5e0192";

    char *parts[] = {outer_header, mac, seq_num, inner_header, msg_type, reg_result,
                     guti_ie, tai_list, nssai, net_feat, gprs_timer};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (offset + len >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, parts[i], len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Registration Accept", EVENT_PAYLOAD);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    sequence_number++;
    
    // Trigger registration accept event
    trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Registration Complete
EVENT_HANDLER(handle_registration_complete)
{
    printf("[5G Core] Received Registration Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Configuration Update Command
    char outer_header[] = "7e02";
    char mac[] = "de0d22e3";
    char seq_num[] = "02";
    char inner_header[] = "7e00";
    char msg_type[] = "54";
    char net_name_full[] = "430f90004f00700065006e003500470053";
    char net_name_short[] = "450990004e006500780074";
    char tz_local[] = "460a";
    char tz_and_time[] = "475260903035530a";
    char dst[] = "490101";

    char *parts[] = {outer_header, mac, seq_num, inner_header, msg_type, net_name_full,
                     net_name_short, tz_local, tz_and_time, dst};

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (offset + len >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + offset, parts[i], len);
        offset += len;
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Configuration Update", EVENT_PAYLOAD);
    ue_state = UE_STATE_REGISTERED;
    sequence_number++;
    
    // Trigger configuration update event
    trigger_event(EVENT_CONFIGURATION_UPDATE, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for PDU Session Establishment Request
EVENT_HANDLER(handle_pdu_session_request)
{
    printf("[5G Core] Received PDU Session Establishment Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate PDU Session Establishment Accept
    char outer_hdr[] = "7e02";
    char mac[] = "fbd62d81";
    char seq[] = "03";
    char inner_hdr[] = "7e00";
    char dl_msg[] = "68";
    char spare_pt[] = "01";
    char pc_len[] = "0047";
    char sm_epd[] = "2e";
    char pdu_id[] = "01";
    char pti[] = "01";
    char sm_msg[] = "c2";
    char ssc_pdu[] = "11";
    char qos_rule[] = "000901000631310101ff01";
    char ambr[] = "0603f42403f424";
    char pdu_addr[] = "2905010a2d0002";
    char snssai[] = "220101";
    char qos_flow[] = "790006012041010109";
    char epco[] = "7b000f80000d0408080808000d0408080404";
    char dnn[] = "250908696e7465726e6574";
    char pdu_id_ie[] = "1201";

    char *parts[] = {outer_hdr, mac, seq, inner_hdr, dl_msg, spare_pt, pc_len, sm_epd, pdu_id, pti,
                     sm_msg, ssc_pdu, qos_rule, ambr, pdu_addr, snssai, qos_flow, epco, dnn, pdu_id_ie};

    size_t off = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        size_t len = strlen(parts[i]);
        if (off + len >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        memcpy(EVENT_PAYLOAD + off, parts[i], len);
        off += len;
    }
    EVENT_PAYLOAD[off] = '\0';
    
    print_nas_pdu("Generated PDU Session Accept", EVENT_PAYLOAD);
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    sequence_number++;
    
    // Trigger PDU session accept event
    trigger_event(EVENT_PDU_SESSION_ACCEPT, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Complete the flow - stop after PDU session
    printf("[5G Core] Registration and PDU session flow completed\n");
    sleep(1);
    stopped = true;
}

// Handler for stop event
EVENT_HANDLER(handle_stop)
{
    printf("[5G Core] Received stop event\n");
    stopped = true;
}

// Register all event handlers for the 5G Core actor
void fiveg_core_register_handlers(void)
{
    printf("  - 5G Core actor: Registering handlers\n");
    register_event_handler(EVENT_REGISTRATION_REQUEST, handle_registration_request);
    register_event_handler(EVENT_AUTH_RESPONSE, handle_auth_response);
    register_event_handler(EVENT_SECURITY_MODE_COMPLETE, handle_security_mode_complete);
    register_event_handler(EVENT_REGISTRATION_COMPLETE, handle_registration_complete);
    register_event_handler(EVENT_PDU_SESSION_REQUEST, handle_pdu_session_request);
    register_event_handler(EVENT_STOP, handle_stop);
}

// Check if the 5G Core actor has stopped
bool fiveg_core_stopped(void)
{
    return stopped;
}