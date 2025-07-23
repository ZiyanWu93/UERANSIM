#include "ueransim_actor.h"
#include "end_to_end_events.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Actor state
static bool registration_started = false;
static ue_state_t ue_state = UE_STATE_DEREGISTERED;

// Initialize the UERANSIM actor
void ueransim_init(void)
{
    printf("[UERANSIM] Actor initialized\n");
    registration_started = false;
    ue_state = UE_STATE_DEREGISTERED;
}

// Utility function to print received PDU
static void print_received_pdu(const char* msg_type, const uint8_t* pdu, size_t len)
{
    printf("[UERANSIM] Received %s: ", msg_type);
    for (size_t i = 0; i < len; i++) {
        printf("%02x", pdu[i]);
    }
    printf("\n");
}

// Utility function to compare uint8_t arrays
static bool verify_pdu(const char* pdu_name, const uint8_t* received, size_t received_len, 
                      const uint8_t* expected, size_t expected_len)
{
    if (received_len != expected_len) {
        printf("[UERANSIM] ERROR: %s length mismatch. Expected %zu, got %zu\n", 
               pdu_name, expected_len, received_len);
        return false;
    }
    
    for (size_t i = 0; i < expected_len; i++) {
        if (received[i] != expected[i]) {
            printf("[UERANSIM] ERROR: %s mismatch at byte %zu. Expected 0x%02x, got 0x%02x\n", 
                   pdu_name, i, expected[i], received[i]);
            return false;
        }
    }
    
    printf("[UERANSIM] %s verification passed\n", pdu_name);
    return true;
}

// Event source that generates the initial registration request
void ueransim_startup_event_source(void)
{
    static bool started = false;
    static time_t start_time = 0;
    
    if (!started) {
        if (start_time == 0) {
            start_time = time(NULL);
            return;
        }
        
        // Wait 1 second before starting
        if (time(NULL) - start_time >= 1) {
            // Generate initial registration request
            uint8_t reg_request[] = {0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d, 0x01, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x2e, 0x04, 0x80, 0xf0, 0x80, 0xf0};
            size_t reg_request_len = sizeof(reg_request);
            
            // Copy to EVENT_PAYLOAD
            memcpy(EVENT_PAYLOAD, reg_request, reg_request_len);
            event_nf_ptr->input_payload_length = reg_request_len;
            
            printf("[UERANSIM] Initiating registration with Registration Request\n");
            print_received_pdu("Sending NAS PDU", EVENT_PAYLOAD, reg_request_len);
            ue_state = UE_STATE_REGISTERING;
            
            trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, reg_request_len);
            started = true;
        }
    }
}

// Handler for Authentication Request
EVENT_HANDLER(handle_auth_request)
{
    print_received_pdu("Authentication Request", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Expected authentication request format
    uint8_t expected_auth_req[] = {
        0x7e, 0x00,  // EPD header
        0x56,        // Message type (Authentication Request)
        0x00,        // ngKSI field
        0x02, 0x00, 0x00,  // ABBA IE
        0x21,        // RAND IEI
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69,  // RAND value
        0x20,        // AUTN IEI
        0x10,        // AUTN length
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00, 0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19   // AUTN value
    };
    
    // Verify the received authentication request
    if (!verify_pdu("Authentication Request", EVENT_PAYLOAD, event_nf_ptr->input_payload_length, 
                    expected_auth_req, sizeof(expected_auth_req))) {
        printf("[UERANSIM] ERROR: Authentication request verification failed\n");
        return;
    }
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Authentication Response
    uint8_t auth_response[] = {0x7e, 0x00, 0x57, 0x2d, 0x10, 0xef, 0x27, 0x70, 0xc6, 0x9e, 0x73, 0x82, 0xaa, 0x38, 0xe8, 0x13, 0x4f, 0x60, 0x22, 0x34, 0xe1};
    size_t auth_response_len = sizeof(auth_response);
    
    memcpy(EVENT_PAYLOAD, auth_response, auth_response_len);
    event_nf_ptr->input_payload_length = auth_response_len;
    
    print_received_pdu("Sending Authentication Response", EVENT_PAYLOAD, auth_response_len);
    ue_state = UE_STATE_AUTHENTICATED;
    
    // Trigger authentication response event
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, auth_response_len);
}

// Handler for Security Mode Command
EVENT_HANDLER(handle_security_mode_command)
{
    print_received_pdu("Security Mode Command", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Expected security mode command format
    uint8_t expected_sec_cmd[] = {
        0x7e, 0x03,              // Outer header
        0x13, 0xbf, 0x99, 0x5a,  // MAC
        0x00,                    // Sequence number
        0x7e, 0x00,              // Inner header
        0x5d,                    // Message type (Security Mode Command)
        0x02,                    // NAS algorithms
        0x00,                    // ngKSI field
        0x04,                    // UE security capability length
        0x80, 0xf0, 0x80, 0xf0,  // UE security capability value
        0xe1,                    // IMEISV request
        0x36, 0x01, 0x02         // Additional security info
    };
    
    // Verify the received security mode command
    if (!verify_pdu("Security Mode Command", EVENT_PAYLOAD, event_nf_ptr->input_payload_length,
                    expected_sec_cmd, sizeof(expected_sec_cmd))) {
        printf("[UERANSIM] ERROR: Security mode command verification failed\n");
        return;
    }
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Security Mode Complete with embedded Registration Request
    uint8_t sec_mode_complete[] = {0x7e, 0x04, 0x22, 0xe4, 0xee, 0x19, 0x00, 0x7e, 0x00, 0x5e, 0x77, 0x00, 0x09, 0x45, 0x73, 0x80, 0x61, 0x21, 0x85, 0x61, 0x51, 0xf1, 0x71, 0x00, 0x23, 0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d, 0x01, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x01, 0x00, 0x2e, 0x04, 0x80, 0xf0, 0x80, 0xf0, 0x2f, 0x02, 0x01, 0x01, 0x53, 0x01, 0x00};
    size_t sec_mode_complete_len = sizeof(sec_mode_complete);
    
    memcpy(EVENT_PAYLOAD, sec_mode_complete, sec_mode_complete_len);
    event_nf_ptr->input_payload_length = sec_mode_complete_len;
    
    print_received_pdu("Sending Security Mode Complete", EVENT_PAYLOAD, sec_mode_complete_len);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    
    // Trigger security mode complete event
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, sec_mode_complete_len);
}

// Handler for Registration Accept
EVENT_HANDLER(handle_registration_accept)
{
    print_received_pdu("Registration Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Expected registration accept format
    uint8_t expected_reg_accept[] = {
        0x7e, 0x02,              // Outer header
        0x72, 0x39, 0x67, 0x4c,  // MAC
        0x01,                    // Sequence number
        0x7e, 0x00,              // Inner header
        0x42,                    // Message type (Registration Accept)
        0x01, 0x01,              // Registration result
        0x77, 0x00, 0x0b, 0xf2, 0x99, 0xf9, 0x07, 0x02, 0x00, 0x40, 0xc0, 0x00, 0x07, 0x27,  // GUTI IE
        0x54, 0x07, 0x40, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x01,  // TAI list
        0x15, 0x02, 0x01, 0x01,  // NSSAI
        0x21, 0x02, 0x01, 0x00,  // Network features
        0x5e, 0x01, 0x92         // GPRS timer
    };
    
    // Verify the received registration accept
    if (!verify_pdu("Registration Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length,
                    expected_reg_accept, sizeof(expected_reg_accept))) {
        printf("[UERANSIM] ERROR: Registration accept verification failed\n");
        return;
    }
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Registration Complete
    uint8_t reg_complete[] = {0x7e, 0x02, 0x46, 0x9d, 0x6a, 0x8b, 0x01, 0x7e, 0x00, 0x43};
    size_t reg_complete_len = sizeof(reg_complete);
    
    memcpy(EVENT_PAYLOAD, reg_complete, reg_complete_len);
    event_nf_ptr->input_payload_length = reg_complete_len;
    
    print_received_pdu("Sending Registration Complete", EVENT_PAYLOAD, reg_complete_len);
    ue_state = UE_STATE_REGISTERED;
    
    // Trigger registration complete event
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, reg_complete_len);
}

// Handler for Configuration Update Command
EVENT_HANDLER(handle_configuration_update)
{
    print_received_pdu("Configuration Update Command", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Expected configuration update format
    uint8_t expected_config_update[] = {
        0x7e, 0x02,              // Outer header
        0xde, 0x0d, 0x22, 0xe3,  // MAC
        0x02,                    // Sequence number
        0x7e, 0x00,              // Inner header
        0x54,                    // Message type (Configuration Update Command)
        0x43, 0x0f, 0x90, 0x00, 0x4f, 0x00, 0x70, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x35, 0x00, 0x47, 0x00, 0x53,  // Network name full
        0x45, 0x09, 0x90, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74,  // Network name short
        0x46, 0x0a,              // TZ local
        0x47, 0x52, 0x60, 0x90, 0x30, 0x35, 0x53, 0x0a,  // TZ and time
        0x49, 0x01, 0x01         // DST
    };
    
    // Verify the received configuration update
    if (!verify_pdu("Configuration Update Command", EVENT_PAYLOAD, event_nf_ptr->input_payload_length,
                    expected_config_update, sizeof(expected_config_update))) {
        printf("[UERANSIM] ERROR: Configuration update verification failed\n");
        return;
    }
    
    // After configuration update, initiate PDU session establishment
    usleep(500000); // 500ms delay before PDU session
    
    // Generate PDU Session Establishment Request
    uint8_t pdu_session_req[] = {0x7e, 0x02, 0xba, 0x02, 0x92, 0xcd, 0x02, 0x7e, 0x00, 0x67, 0x01, 0x00, 0x15, 0x2e, 0x01, 0x01, 0xc1, 0xff, 0xff, 0x91, 0xa1, 0x28, 0x01, 0x00, 0x7b, 0x00, 0x07, 0x80, 0x00, 0x0a, 0x00, 0x00, 0x0d, 0x00, 0x12, 0x01, 0x81, 0x22, 0x01, 0x01, 0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74};
    size_t pdu_session_req_len = sizeof(pdu_session_req);
    
    memcpy(EVENT_PAYLOAD, pdu_session_req, pdu_session_req_len);
    event_nf_ptr->input_payload_length = pdu_session_req_len;
    
    print_received_pdu("Sending PDU Session Establishment Request", EVENT_PAYLOAD, pdu_session_req_len);
    
    // Trigger PDU session request event
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, pdu_session_req_len);
}

// Handler for PDU Session Establishment Accept
EVENT_HANDLER(handle_pdu_session_accept)
{
    print_received_pdu("PDU Session Establishment Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Expected PDU session accept format
    uint8_t expected_pdu_accept[] = {
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
        0xc2,                    // SM message type (PDU Session Establishment Accept)
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
    
    // Verify the received PDU session accept
    if (!verify_pdu("PDU Session Establishment Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length,
                    expected_pdu_accept, sizeof(expected_pdu_accept))) {
        printf("[UERANSIM] ERROR: PDU session accept verification failed\n");
        return;
    }
    
    printf("[UERANSIM] PDU Session established successfully\n");
    printf("[UERANSIM] UE is now fully registered with active PDU session\n");
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    
    // Flow complete
    printf("\n[UERANSIM] === 5G Registration and PDU Session Flow Completed ===\n");
}

// Register all event handlers for the UERANSIM actor
void ueransim_register_handlers(void)
{
    printf("  - UERANSIM actor: Registering handlers\n");
    register_event_handler(EVENT_AUTH_REQUEST, handle_auth_request);
    register_event_handler(EVENT_SECURITY_MODE_COMMAND, handle_security_mode_command);
    register_event_handler(EVENT_REGISTRATION_ACCEPT, handle_registration_accept);
    register_event_handler(EVENT_CONFIGURATION_UPDATE, handle_configuration_update);
    register_event_handler(EVENT_PDU_SESSION_ACCEPT, handle_pdu_session_accept);
}