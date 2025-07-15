#include "fiveg_core_actor.h"
#include "amf_common.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

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
void print_nas_pdu(const char* label, const uint8_t* pdu, size_t len)
{
    printf("[5G Core] %s: ", label);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", pdu[i]);
    }
    printf("\n");
}

// Handler for Registration Request
EVENT_HANDLER(handle_registration_request)
{
    printf("[5G Core] Received Registration Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Generate Authentication Request
    uint8_t auth_request[] = {
        0x7e, 0x00,  // EPD header
        0x56,        // Message type
        0x00,        // ngKSI field
        0x02, 0x00, 0x00,  // ABBA IE
        0x21,        // RAND IEI
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69,  // RAND value
        0x20,        // AUTN IEI
        0x10,        // AUTN length
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00, 0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19   // AUTN value
    };

    size_t auth_request_len = sizeof(auth_request);
    if (auth_request_len > MAX_EVENT_PAYLOAD_SIZE) {
        printf("[5G Core] ERROR: Auth request too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, auth_request, auth_request_len);
    event_nf_ptr->input_payload_length = auth_request_len;
    
    print_nas_pdu("Generated Auth Request", EVENT_PAYLOAD, auth_request_len);
    ue_state = UE_STATE_REGISTERING;
    
    // Trigger authentication request event
    trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD, auth_request_len);
}

// Handler for Authentication Response
EVENT_HANDLER(handle_auth_response)
{
    printf("[5G Core] Received Authentication Response\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Generate Security Mode Command
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

    size_t sec_mode_cmd_len = sizeof(sec_mode_cmd);
    if (sec_mode_cmd_len > MAX_EVENT_PAYLOAD_SIZE) {
        printf("[5G Core] ERROR: Security mode command too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, sec_mode_cmd, sec_mode_cmd_len);
    event_nf_ptr->input_payload_length = sec_mode_cmd_len;
    
    print_nas_pdu("Generated Security Mode Command", EVENT_PAYLOAD, sec_mode_cmd_len);
    ue_state = UE_STATE_AUTHENTICATED;
    sequence_number++;
    
    // Trigger security mode command event
    trigger_event(EVENT_SECURITY_MODE_COMMAND, EVENT_PAYLOAD, sec_mode_cmd_len);
}

// Handler for Security Mode Complete
EVENT_HANDLER(handle_security_mode_complete)
{
    printf("[5G Core] Received Security Mode Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Generate Registration Accept
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

    size_t reg_accept_len = sizeof(reg_accept);
    if (reg_accept_len > MAX_EVENT_PAYLOAD_SIZE) {
        printf("[5G Core] ERROR: Registration accept too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, reg_accept, reg_accept_len);
    event_nf_ptr->input_payload_length = reg_accept_len;
    
    print_nas_pdu("Generated Registration Accept", EVENT_PAYLOAD, reg_accept_len);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    sequence_number++;
    
    // Trigger registration accept event
    trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD, reg_accept_len);
}

// Handler for Registration Complete
EVENT_HANDLER(handle_registration_complete)
{
    printf("[5G Core] Received Registration Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Generate Configuration Update Command
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

    size_t config_update_len = sizeof(config_update);
    if (config_update_len > MAX_EVENT_PAYLOAD_SIZE) {
        printf("[5G Core] ERROR: Configuration update too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, config_update, config_update_len);
    event_nf_ptr->input_payload_length = config_update_len;
    
    print_nas_pdu("Generated Configuration Update", EVENT_PAYLOAD, config_update_len);
    ue_state = UE_STATE_REGISTERED;
    sequence_number++;
    
    // Trigger configuration update event
    trigger_event(EVENT_CONFIGURATION_UPDATE, EVENT_PAYLOAD, config_update_len);
}

// Handler for PDU Session Establishment Request
EVENT_HANDLER(handle_pdu_session_request)
{
    printf("[5G Core] Received PDU Session Establishment Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Generate PDU Session Establishment Accept
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
        0x00, 0x09, 0x01, 0x00, 0x06, 0x31, 0x31, 0x01, 0x01, 0xff, 0x01,  // QoS rules
        0x06, 0x03, 0xf4, 0x24, 0x03, 0xf4, 0x24,  // Session-AMBR
        0x29, 0x05, 0x01, 0x0a, 0x2d, 0x00, 0x02,  // PDU address
        0x22, 0x01, 0x01,        // S-NSSAI
        0x79, 0x00, 0x06, 0x01, 0x20, 0x41, 0x01, 0x01, 0x09,  // QoS flow descriptions
        0x7b, 0x00, 0x0f, 0x80, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x04, 0x04,  // Extended protocol configuration options
        0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74,  // DNN
        0x12, 0x01               // 5GSM cause IE
    };

    size_t pdu_accept_len = sizeof(pdu_accept);
    if (pdu_accept_len > MAX_EVENT_PAYLOAD_SIZE) {
        printf("[5G Core] ERROR: PDU session accept too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, pdu_accept, pdu_accept_len);
    event_nf_ptr->input_payload_length = pdu_accept_len;
    
    print_nas_pdu("Generated PDU Session Accept", EVENT_PAYLOAD, pdu_accept_len);
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    sequence_number++;
    
    // Trigger PDU session accept event
    trigger_event(EVENT_PDU_SESSION_ACCEPT, EVENT_PAYLOAD, pdu_accept_len);
    
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