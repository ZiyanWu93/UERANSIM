#include "amf_handlers.h"
#include "ue_state.h"
#include "nas_ipc_protocol.h"
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Internal event for IPC NAS response - offset to avoid collision */
#define EVENT_IPC_NAS_RESPONSE (EVENT_IPC_SEND_RESPONSE + 100)

/* Utility function to convert hex string to binary */
static int hex_to_binary(const char* hex, uint8_t* binary, size_t max_len)
{
    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0 || hex_len / 2 > max_len) {
        return -1;
    }
    
    for (size_t i = 0; i < hex_len / 2; i++) {
        char byte_str[3] = {hex[i*2], hex[i*2+1], '\0'};
        binary[i] = (uint8_t)strtol(byte_str, NULL, 16);
    }
    
    return hex_len / 2;
}

/* Utility to send NAS response via IPC */
static void send_nas_response(uint16_t event_type, const char* nas_hex)
{
    /* Convert hex to binary */
    uint8_t nas_pdu[NAS_IPC_MAX_PDU_SIZE];
    int nas_len = hex_to_binary(nas_hex, nas_pdu, NAS_IPC_MAX_PDU_SIZE);
    
    if (nas_len < 0) {
        fprintf(stderr, "[AMF] Failed to convert NAS PDU to binary\n");
        return;
    }
    
    /* Pack into IPC message */
    IpcMessage ipc_msg;
    uint32_t trans_id = ue_state_get_transaction_id();
    
    int result = nas_ipc_pack_message(&ipc_msg, NAS_IPC_MSG_DOWNLINK, event_type,
                                      trans_id, nas_pdu, nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "[AMF] Failed to pack NAS response: %s\n",
                nas_ipc_error_to_string(result));
        return;
    }
    
    /* Convert to string for event payload (in real impl would be binary) */
    char payload[IPC_MAX_PAYLOAD_SIZE + 1];
    memcpy(payload, ipc_msg.data, ipc_msg.length);
    payload[ipc_msg.length] = '\0';
    
    /* Trigger IPC response event */
    printf("[AMF] Triggering IPC NAS response event\n");
    trigger_event(EVENT_IPC_NAS_RESPONSE, payload);
}

/* Utility function to print NAS PDU */
void print_nas_pdu(const char* label, const char* pdu)
{
    printf("[AMF] %s: %s\n", label, pdu);
}

/* Handler for Registration Request */
EVENT_HANDLER(handle_registration_request)
{
    printf("[AMF] Processing Registration Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    /* Update state */
    ue_state_set(UE_STATE_REGISTERING);
    
    /* Generate Authentication Request (ported from existing AMF code) */
    char epd_header[] = "7e00";
    char msg_type[] = "56";
    char ngksi_field[] = "00";
    char abba_ie[] = "020000";
    char rand_iei[] = "21";
    char rand_val[] = "5ca0df8c9bb8dbcf3c2a7dd448da1369";
    char autn_iei[] = "20";
    char autn_len[] = "10";
    char autn_val[] = "406296993082800030b762455c890b19";

    char *parts[] = {epd_header, msg_type, ngksi_field, abba_ie, rand_iei, 
                     rand_val, autn_iei, autn_len, autn_val};

    char auth_request[MAX_NAS_HEX_LEN];
    size_t offset = 0;
    
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i) {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN) {
            fprintf(stderr, "[AMF] Auth request too large\n");
            return;
        }
        memcpy(auth_request + offset, part, len);
        offset += len;
    }
    auth_request[offset] = '\0';
    
    print_nas_pdu("Generated Auth Request", auth_request);
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_REG_REQUEST, auth_request);
}

/* Handler for Authentication Response */
EVENT_HANDLER(handle_auth_response)
{
    printf("[AMF] Processing Authentication Response\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    /* Update state */
    ue_state_set(UE_STATE_AUTHENTICATED);
    ue_state_establish_security();
    
    /* Generate Security Mode Command */
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

    char *parts[] = {outer_header, mac, seq_num, inner_header, msg_type, nas_alg, 
                     ngksi_field, ue_sec_cap_len, ue_sec_cap_val, imeisv_req, add_sec_info};

    char security_cmd[MAX_NAS_HEX_LEN];
    size_t offset = 0;
    
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i) {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN) {
            fprintf(stderr, "[AMF] Security command too large\n");
            return;
        }
        memcpy(security_cmd + offset, part, len);
        offset += len;
    }
    security_cmd[offset] = '\0';
    
    print_nas_pdu("Generated Security Mode Command", security_cmd);
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_AUTH_RESPONSE, security_cmd);
}

/* Handler for Security Mode Complete */
EVENT_HANDLER(handle_security_mode_complete)
{
    printf("[AMF] Processing Security Mode Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    /* Generate Registration Accept */
    uint8_t seq = ue_state_get_sequence_number();
    char seq_str[3];
    sprintf(seq_str, "%02x", seq);
    
    char outer_header[] = "7e02";
    char mac[] = "7239674c";
    char inner_header[] = "7e00";
    char msg_type[] = "42";
    char reg_result[] = "0101";
    char guti_ie[] = "77000bf299f907020040c0000727";
    char tai_list[] = "54074099f907000001";
    char nssai[] = "15020101";
    char net_feat[] = "21020100";
    char gprs_timer[] = "5e0192";

    char *parts[] = {outer_header, mac, seq_str, inner_header, msg_type, reg_result,
                     guti_ie, tai_list, nssai, net_feat, gprs_timer};

    char reg_accept[MAX_NAS_HEX_LEN];
    size_t offset = 0;
    
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i) {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN) {
            fprintf(stderr, "[AMF] Registration accept too large\n");
            return;
        }
        memcpy(reg_accept + offset, part, len);
        offset += len;
    }
    reg_accept[offset] = '\0';
    
    print_nas_pdu("Generated Registration Accept", reg_accept);
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_SEC_MODE_COMP, reg_accept);
}

/* Handler for Registration Complete */
EVENT_HANDLER(handle_registration_complete)
{
    printf("[AMF] Processing Registration Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    /* Update state to fully registered */
    ue_state_set(UE_STATE_REGISTERED);
    
    printf("[AMF] UE successfully registered\n");
    
    /* No response needed for Registration Complete */
}

/* Handler for PDU Session Request */
EVENT_HANDLER(handle_pdu_session_request)
{
    printf("[AMF] Processing PDU Session Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    /* Generate PDU Session Response */
    uint8_t seq = ue_state_get_sequence_number();
    char seq_str[3];
    sprintf(seq_str, "%02x", seq);
    
    char outer_header[] = "7e02";
    char mac[] = "8f40fe1f";
    char inner_header[] = "7e00";
    char msg_type[] = "68";
    char pdu_session_id[] = "120501";
    char pti[] = "00";
    char pdu_msg_type[] = "c2";
    char qos_rules[] = "250901000601010000000101";
    char session_ambr[] = "2e0201c1";
    char pdu_address[] = "29050402020202";
    char dnn[] = "790008696e7465726e6574";

    char *parts[] = {outer_header, mac, seq_str, inner_header, msg_type, pdu_session_id,
                     pti, pdu_msg_type, qos_rules, session_ambr, pdu_address, dnn};

    char pdu_response[MAX_NAS_HEX_LEN];
    size_t offset = 0;
    
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i) {
        const char *part = parts[i];
        size_t len = strlen(part);
        if (offset + len >= MAX_NAS_HEX_LEN) {
            fprintf(stderr, "[AMF] PDU session response too large\n");
            return;
        }
        memcpy(pdu_response + offset, part, len);
        offset += len;
    }
    pdu_response[offset] = '\0';
    
    print_nas_pdu("Generated PDU Session Response", pdu_response);
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_PDU_SESSION, pdu_response);
}

/* Register all AMF NAS event handlers */
void amf_register_handlers(void)
{
    printf("[AMF] Registering NAS event handlers\n");
    
    /* Register NAS message handlers */
    register_event_handler(EVENT_NAS_REGISTRATION_REQUEST, handle_registration_request);
    register_event_handler(EVENT_NAS_AUTH_RESPONSE, handle_auth_response);
    register_event_handler(EVENT_NAS_SECURITY_MODE_COMPLETE, handle_security_mode_complete);
    register_event_handler(EVENT_NAS_REGISTRATION_COMPLETE, handle_registration_complete);
    register_event_handler(EVENT_NAS_PDU_SESSION_REQUEST, handle_pdu_session_request);
}