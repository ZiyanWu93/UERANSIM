#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "amf.h"
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// trigger_event is already declared in runtime.h

// AMF (Access and Mobility Management Function) Dispatcher
void amf_dispatcher(EventNf* event)
{
    printf("[AMF Dispatcher] Received event ID: %d\n", event->event_id);
    printf("[AMF Dispatcher] Message length: %d bytes\n", event->input_payload_length);
    
    if (event->input_payload_length == 0) {
        printf("[AMF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    uint8_t message_type;
    
    // Check if message has security header (0x7e followed by non-zero)
    if (event->input_payload[0] == 0x7e && event->input_payload[1] != 0x00) {
        // Security protected message - message type at byte 9
        if (event->input_payload_length > 9) {
            message_type = event->input_payload[9];
            printf("[AMF Dispatcher] Security protected message, type: 0x%02x\n", message_type);
        } else {
            printf("[AMF Dispatcher] Security protected message too short\n");
            return;
        }
    } else {
        // Plain message - type at byte 2
        if (event->input_payload_length > 2) {
            message_type = event->input_payload[2];
            printf("[AMF Dispatcher] Plain message, type: 0x%02x\n", message_type);
        } else {
            printf("[AMF Dispatcher] Plain message too short\n");
            return;
        }
    }
    
    // Route to appropriate internal handler
    switch(message_type) {
        case NAS_MSG_REGISTRATION_REQUEST:
            printf("[AMF Dispatcher] Routing to Registration Request handler\n");
            trigger_event(EVENT_AMF_REGISTRATION_REQUEST, event->input_payload, (int)event->input_payload_length);
            break;
            
        case NAS_MSG_AUTH_RESPONSE:
            printf("[AMF Dispatcher] Routing to Authentication Response handler\n");
            trigger_event(EVENT_AMF_AUTH_RESPONSE, event->input_payload, (int)event->input_payload_length);
            break;
            
        case NAS_MSG_SECURITY_MODE_COMPLETE:
            printf("[AMF Dispatcher] Routing to Security Mode Complete handler\n");
            trigger_event(EVENT_AMF_SECURITY_MODE_COMPLETE, event->input_payload, (int)event->input_payload_length);
            break;
            
        case NAS_MSG_REGISTRATION_COMPLETE:
            printf("[AMF Dispatcher] Routing to Registration Complete handler\n");
            trigger_event(EVENT_AMF_REGISTRATION_COMPLETE, event->input_payload, (int)event->input_payload_length);
            break;
            
        case NAS_MSG_PDU_SESSION_EST_REQ:
            printf("[AMF Dispatcher] PDU Session Request - forwarding to SMF\n");
            trigger_event(EVENT_TO_SMF, event->input_payload, (int)event->input_payload_length);
            break;
            
        default:
            printf("[AMF Dispatcher] Unknown message type: 0x%02x\n", message_type);
            break;
    }
}

// Actor state - moved from fiveg_core_actor.c
static ue_state_t ue_state = UE_STATE_DEREGISTERED;
static int sequence_number = 0;

// Phase 1: Registration Request → Authentication Request
EVENT_HANDLER(amf_handle_registration_request)
{
    printf("[AMF] Handling registration request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Save the input registration request for validation
    // In production, would extract and validate fields here
    
    // Start service function chain by setting headers
    // Clear the payload for building authentication request
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set EPD header and message type (bytes 0-2)
    EVENT_PAYLOAD[0] = 0x7e;  // EPD
    EVENT_PAYLOAD[1] = 0x00;  // Security header (plain)
    EVENT_PAYLOAD[2] = 0x56;  // Message type (Authentication Request)
    
    // Set initial length for authentication request message
    event_nf_ptr->input_payload_length = 42;  // Total size of auth request
    
    printf("[AMF] Set authentication request headers, starting service chain\n");
    ue_state = UE_STATE_REGISTERING;
    
    // Trigger next event in the chain
    trigger_event(EVENT_AMF_SET_AUTH_NGKSI, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

void amf_extract_suci_from_mobile_identity(void)
{
    printf("[AMF] Extracting SUCI from mobile identity\n");
}

void amf_validate_registration_request(void)
{
    printf("[AMF] Validating registration request\n");
}

void amf_build_authentication_request(void)
{
    printf("[AMF] Building authentication request\n");
}

void amf_send_authentication_request(void)
{
    printf("[AMF] Sending authentication request\n");
}

// Service Function Chain Handlers for Authentication Request

EVENT_HANDLER(amf_set_auth_ngksi)
{
    printf("[AMF] Setting authentication ngKSI field\n");
    
    // Set ngKSI field at byte 3
    EVENT_PAYLOAD[3] = 0x00;  // ngKSI: TSC=0, KSI=0
    
    // Trigger next event in the chain
    trigger_event(EVENT_AMF_SET_ABBA, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

EVENT_HANDLER(amf_set_abba)
{
    printf("[AMF] Setting ABBA IE\n");
    
    // Set ABBA IE at bytes 4-6
    EVENT_PAYLOAD[4] = 0x02;  // ABBA length
    EVENT_PAYLOAD[5] = 0x00;  // ABBA contents
    EVENT_PAYLOAD[6] = 0x00;  // ABBA contents
    
    // Trigger AUSF to set RAND
    trigger_event(EVENT_AUSF_SET_RAND, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

EVENT_HANDLER(amf_send_auth_request)
{
    printf("[AMF] Authentication request complete, sending to UE\n");
    
    print_nas_pdu("Generated Auth Request", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Send the complete authentication request back to UE
    trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Phase 2: Authentication Response → Security Mode Command
EVENT_HANDLER(amf_handle_authentication_response)
{
    printf("[AMF] Handling authentication response\n");
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
        printf("[AMF] ERROR: Security mode command too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, sec_mode_cmd, sec_mode_cmd_len);
    event_nf_ptr->input_payload_length = sec_mode_cmd_len;
    
    print_nas_pdu("Generated Security Mode Command", EVENT_PAYLOAD, sec_mode_cmd_len);
    ue_state = UE_STATE_AUTHENTICATED;
    sequence_number++;
    
    // Trigger security mode command event back to UE
    trigger_event(EVENT_SECURITY_MODE_COMMAND, EVENT_PAYLOAD, (int)sec_mode_cmd_len);
}

void amf_verify_res_star(void)
{
    printf("[AMF] Verifying RES*\n");
}

void amf_derive_security_keys(void)
{
    printf("[AMF] Deriving security keys\n");
}

void amf_select_security_algorithms(void)
{
    printf("[AMF] Selecting security algorithms\n");
}

void amf_build_security_mode_command(void)
{
    printf("[AMF] Building security mode command\n");
}

void amf_calculate_mac(void)
{
    printf("[AMF] Calculating MAC\n");
}

void amf_send_security_mode_command(void)
{
    printf("[AMF] Sending security mode command\n");
}

// Phase 3: Security Mode Complete → Registration Accept
EVENT_HANDLER(amf_handle_security_mode_complete)
{
    printf("[AMF] Handling security mode complete\n");
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
        printf("[AMF] ERROR: Registration accept too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, reg_accept, reg_accept_len);
    event_nf_ptr->input_payload_length = reg_accept_len;
    
    print_nas_pdu("Generated Registration Accept", EVENT_PAYLOAD, reg_accept_len);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    sequence_number++;
    
    // Trigger registration accept event back to UE
    trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD, (int)reg_accept_len);
}

void amf_validate_nas_message_container(void)
{
    printf("[AMF] Validating NAS message container\n");
}

void amf_extract_and_store_imeisv(void)
{
    printf("[AMF] Extracting and storing IMEISV\n");
}

void amf_allocate_5g_guti(void)
{
    printf("[AMF] Allocating 5G-GUTI\n");
}

void amf_build_registration_accept(void)
{
    printf("[AMF] Building registration accept\n");
}

void amf_send_registration_accept(void)
{
    printf("[AMF] Sending registration accept\n");
}

// Phase 4: Registration Complete → Configuration Update
EVENT_HANDLER(amf_handle_registration_complete)
{
    printf("[AMF] Handling registration complete\n");
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
        printf("[AMF] ERROR: Configuration update too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, config_update, config_update_len);
    event_nf_ptr->input_payload_length = config_update_len;
    
    print_nas_pdu("Generated Configuration Update", EVENT_PAYLOAD, config_update_len);
    ue_state = UE_STATE_REGISTERED;
    sequence_number++;
    
    // Trigger configuration update event back to UE
    trigger_event(EVENT_CONFIGURATION_UPDATE, EVENT_PAYLOAD, (int)config_update_len);
}

void amf_build_configuration_update_command(void)
{
    printf("[AMF] Building configuration update command\n");
}

void amf_send_configuration_update_command(void)
{
    printf("[AMF] Sending configuration update command\n");
}

// Phase 5: PDU Session Establishment
EVENT_HANDLER(amf_handle_pdu_session_request)
{
    printf("[AMF] Handling PDU session establishment request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // For now, directly generate PDU Session Accept (in real system, would forward to SMF)
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
        printf("[AMF] ERROR: PDU session accept too large\n");
        return;
    }
    
    memcpy(EVENT_PAYLOAD, pdu_accept, pdu_accept_len);
    event_nf_ptr->input_payload_length = pdu_accept_len;
    
    print_nas_pdu("Generated PDU Session Accept", EVENT_PAYLOAD, pdu_accept_len);
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    sequence_number++;
    
    // Trigger PDU session accept event back to UE
    trigger_event(EVENT_PDU_SESSION_ACCEPT, EVENT_PAYLOAD, (int)pdu_accept_len);
    
    // Complete the flow
    printf("[AMF] Registration and PDU session flow completed\n");
    sleep(1);
}

void amf_forward_sm_message_to_smf(void)
{
    printf("[AMF] Forwarding SM message to SMF\n");
}

void amf_relay_pdu_session_response(void)
{
    printf("[AMF] Relaying PDU session response\n");
}