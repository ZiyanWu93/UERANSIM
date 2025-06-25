#include "amf_handlers.h"
#include "ue_state.h"
#include "nas_ipc_protocol.h"
#include "../../runtime/runtime.h"
#include "../../event_system/event.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Internal event for IPC NAS response - using EVENT_IPC_SEND_RESPONSE directly */
#define EVENT_IPC_NAS_RESPONSE EVENT_IPC_SEND_RESPONSE

/* Utility to send NAS response via IPC */
static void send_nas_response(uint16_t event_type, const uint8_t* nas_pdu, int nas_len)
{
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
    
    /* Trigger IPC response event with binary data */
    printf("[AMF] Triggering IPC NAS response event (%u bytes)\n", ipc_msg.length);
    trigger_event(EVENT_IPC_NAS_RESPONSE, &ipc_msg, sizeof(ipc_msg.length) + ipc_msg.length);
}

/* Utility function to print NAS PDU */
void print_nas_pdu(const char* label, const uint8_t* pdu, int len)
{
    printf("[AMF] %s (%d bytes): ", label, len);
    for (int i = 0; i < len; i++) {
        printf("%02x", pdu[i]);
    }
    printf("\n");
}

/* Handler for Registration Request */
EVENT_HANDLER(handle_registration_request)
{
    const uint8_t* nas_pdu = (const uint8_t*)EVENT_PAYLOAD;
    int nas_len = event_nf_ptr->input_payload_length;
    
    printf("[AMF] Processing Registration Request\n");
    print_nas_pdu("Input NAS PDU", nas_pdu, nas_len);
    
    /* Update state */
    ue_state_set(UE_STATE_REGISTERING);
    
    /* Generate Authentication Request (ported from existing AMF code) */
    uint8_t auth_request[] = {
        0x7e, 0x00,  /* EPD header */
        0x56,        /* Message type */
        0x00,        /* ngKSI field */
        0x02, 0x00, 0x00,  /* ABBA IE */
        0x21,        /* RAND IEI */
        /* RAND value */
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf,
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69,
        0x20,        /* AUTN IEI */
        0x10,        /* AUTN length */
        /* AUTN value */
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00,
        0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19
    };
    
    print_nas_pdu("Generated Auth Request", auth_request, sizeof(auth_request));
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_REG_REQUEST, auth_request, sizeof(auth_request));
}

/* Handler for Authentication Response */
EVENT_HANDLER(handle_auth_response)
{
    const uint8_t* nas_pdu = (const uint8_t*)EVENT_PAYLOAD;
    int nas_len = event_nf_ptr->input_payload_length;
    
    printf("[AMF] Processing Authentication Response\n");
    print_nas_pdu("Input NAS PDU", nas_pdu, nas_len);
    
    /* Update state */
    ue_state_set(UE_STATE_AUTHENTICATED);
    ue_state_establish_security();
    
    /* Generate Security Mode Command */
    uint8_t sec_cmd[] = {
        0x7e, 0x03,              /* Outer header */
        0x13, 0xbf, 0x99, 0x5a,  /* MAC */
        0x00,                    /* Sequence number */
        0x7e, 0x00,              /* Inner header */
        0x5d,                    /* Message type */
        0x02,                    /* NAS algorithms */
        0x00,                    /* ngKSI field */
        0x04,                    /* UE security capability length */
        0x80, 0xf0, 0x80, 0xf0,  /* UE security capability value */
        0xe1,                    /* IMEISV request */
        0x36, 0x01, 0x02         /* Additional security info */
    };
    
    print_nas_pdu("Generated Security Mode Command", sec_cmd, sizeof(sec_cmd));
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_AUTH_RESPONSE, sec_cmd, sizeof(sec_cmd));
}

/* Handler for Security Mode Complete */
EVENT_HANDLER(handle_security_mode_complete)
{
    const uint8_t* nas_pdu = (const uint8_t*)EVENT_PAYLOAD;
    int nas_len = event_nf_ptr->input_payload_length;
    
    printf("[AMF] Processing Security Mode Complete\n");
    print_nas_pdu("Input NAS PDU", nas_pdu, nas_len);
    
    /* Update state */
    ue_state_establish_security();
    
    /* Generate Registration Accept */
    uint8_t seq = ue_state_get_sequence_number();
    
    /* Build Registration Accept message as binary */
    uint8_t reg_accept[] = {
        0x7e, 0x02,              /* Outer header */
        0x0f, 0x84, 0xfa, 0xea,  /* MAC */
        seq,                     /* Sequence number */
        0x7e, 0x00,              /* Inner header */
        0x42,                    /* Message type (Registration Accept) */
        0x01,                    /* 5GS registration result */
        /* TAI list */
        0x54, 0x08, 0x02, 0x02, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01,
        0x15, 0x02, 0x90, 0x70,  /* Allowed NSSAI */
        /* 5G-GUTI */
        0x77, 0x00, 0x16, 0x7e, 0x00, 0x42, 0x01, 0x54, 0x08, 0x02,
        0x02, 0xf8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x77, 0x00, 0x0d,
        0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3a, 0x58, 0xd1, 0xc6,
        0xed, 0x21,
        /* Registration Complete required */
        0x5e, 0x01, 0x02
    };
    
    print_nas_pdu("Generated Registration Accept", reg_accept, sizeof(reg_accept));
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_SEC_MODE_COMP, reg_accept, sizeof(reg_accept));
}

/* Handler for Registration Complete */
EVENT_HANDLER(handle_registration_complete)
{
    const uint8_t* nas_pdu = (const uint8_t*)EVENT_PAYLOAD;
    int nas_len = event_nf_ptr->input_payload_length;
    
    printf("[AMF] Processing Registration Complete\n");
    print_nas_pdu("Input NAS PDU", nas_pdu, nas_len);
    
    /* Update state */
    ue_state_set(UE_STATE_REGISTERED);
    
    /* Generate Configuration Update Command */
    uint8_t seq = ue_state_get_sequence_number();
    
    uint8_t config_update[] = {
        0x7e, 0x02,              /* Outer header */
        0x9f, 0x09, 0x66, 0x09,  /* MAC */
        seq,                     /* Sequence number */
        0x7e, 0x00,              /* Inner header */
        0x54,                    /* Message type (Configuration Update Command) */
        0x7a, 0x00, 0x04,        /* 5G-GUTI type and length */
        0x80, 0xf0, 0x80, 0xf0   /* 5G-GUTI value (simplified) */
    };
    
    print_nas_pdu("Generated Configuration Update Command", config_update, sizeof(config_update));
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_REG_COMPLETE, config_update, sizeof(config_update));
}

/* Handler for PDU Session Request */
EVENT_HANDLER(handle_pdu_session_request)
{
    const uint8_t* nas_pdu = (const uint8_t*)EVENT_PAYLOAD;
    int nas_len = event_nf_ptr->input_payload_length;
    
    printf("[AMF] Processing PDU Session Request\n");
    print_nas_pdu("Input NAS PDU", nas_pdu, nas_len);
    
    /* Generate PDU Session Response */
    uint8_t seq = ue_state_get_sequence_number();
    
    uint8_t pdu_response[] = {
        0x7e, 0x02,              /* Outer header */
        0x8f, 0x40, 0xfe, 0x1f,  /* MAC */
        seq,                     /* Sequence number */
        0x7e, 0x00,              /* Inner header */
        0x68,                    /* Message type */
        0x12, 0x05, 0x01,        /* PDU session ID */
        0x00,                    /* PTI */
        0xc2,                    /* PDU session msg type */
        /* QoS rules */
        0x25, 0x09, 0x01, 0x00, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01, 0x01,
        /* Session AMBR */
        0x2e, 0x02, 0x01, 0xc1,
        /* PDU address */
        0x29, 0x05, 0x04, 0x02, 0x02, 0x02, 0x02,
        /* DNN */
        0x79, 0x00, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74
    };
    
    print_nas_pdu("Generated PDU Session Response", pdu_response, sizeof(pdu_response));
    
    /* Increment sequence number */
    ue_state_increment_sequence_number();
    
    /* Send response via IPC */
    send_nas_response(NAS_IPC_EVT_PDU_SESSION, pdu_response, sizeof(pdu_response));
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