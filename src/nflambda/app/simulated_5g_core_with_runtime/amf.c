#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "amf.h"
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// trigger_event is already declared in runtime.h

// Actor state - moved from fiveg_core_actor.c
static ue_state_t ue_state = UE_STATE_DEREGISTERED;
static int sequence_number = 0;


// AMF (Access and Mobility Management Function) Dispatcher
EVENT_HANDLER(amf_dispatcher)
{
    printf("[AMF Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[AMF Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    printf("[AMF Dispatcher] Current UE state: %d\n", ue_state);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[AMF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Message classification
    uint8_t request_type = EVENT_PAYLOAD[0];
    uint8_t message_type = 0;
    bool is_service_response = false;
    bool is_nas_message = false;
    
    // Determine message category
    if (request_type >= 0x20 && request_type <= 0x2F) {
        // Service chain response
        is_service_response = true;
        printf("[AMF Dispatcher] Service chain response type: 0x%02x\n", request_type);
    } else if (request_type == REQ_TYPE_REGULAR_MESSAGE || EVENT_PAYLOAD[0] == 0x7e) {
        // NAS message - extract actual message type
        is_nas_message = true;
        if (EVENT_PAYLOAD[0] == 0x7e && EVENT_PAYLOAD[1] != 0x00) {
            // Security protected message - message type at byte 9
            if (event_nf_ptr->input_payload_length > 9) {
                message_type = EVENT_PAYLOAD[9];
                printf("[AMF Dispatcher] Security protected NAS message, type: 0x%02x\n", message_type);
            } else {
                printf("[AMF Dispatcher] Security protected message too short\n");
                return;
            }
        } else {
            // Plain message - type at byte 2
            if (event_nf_ptr->input_payload_length > 2) {
                message_type = EVENT_PAYLOAD[2];
                printf("[AMF Dispatcher] Plain NAS message, type: 0x%02x\n", message_type);
            } else {
                printf("[AMF Dispatcher] Plain message too short\n");
                return;
            }
        }
    } else {
        printf("[AMF Dispatcher] Unknown message format, request_type: 0x%02x\n", request_type);
        return;
    }
    
    // Unified state machine for all message types
    switch (ue_state) {
        case UE_STATE_DEREGISTERED:
            if (is_nas_message && message_type == NAS_MSG_REGISTRATION_REQUEST) {
                printf("[AMF] DEREGISTERED: Accepting Registration Request\n");
                trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
            } else {
                printf("[AMF] DEREGISTERED: Invalid message - only Registration Request allowed\n");
            }
            break;
            
        case UE_STATE_REGISTERING:
            if (is_nas_message) {
                if (message_type == NAS_MSG_AUTH_RESPONSE) {
                    printf("[AMF] REGISTERING: Accepting Authentication Response\n");
                    trigger_event(EVENT_AMF_AUTH_RESPONSE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_REQUEST) {
                    printf("[AMF] REGISTERING: Accepting Registration Request (re-registration)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] REGISTERING: Invalid NAS message 0x%02x - expecting Authentication Response\n", message_type);
                }
            } else if (is_service_response) {
                if (request_type == RESP_TYPE_AUTH_DATA) {
                    printf("[AMF] REGISTERING: Accepting authentication data response from AUSF\n");
                    trigger_event(EVENT_AMF_FINALIZE_AUTH_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] REGISTERING: Invalid response type 0x%02x - only auth data expected\n", request_type);
                }
            }
            break;
            
        case UE_STATE_AUTHENTICATED:
            if (is_nas_message) {
                if (message_type == NAS_MSG_SECURITY_MODE_COMPLETE) {
                    printf("[AMF] AUTHENTICATED: Accepting Security Mode Complete\n");
                    trigger_event(EVENT_AMF_SECURITY_MODE_COMPLETE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_AUTH_RESPONSE) {
                    printf("[AMF] AUTHENTICATED: Accepting Authentication Response (re-authentication)\n");
                    trigger_event(EVENT_AMF_AUTH_RESPONSE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_REQUEST) {
                    printf("[AMF] AUTHENTICATED: Accepting Registration Request (restart)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] AUTHENTICATED: Invalid NAS message 0x%02x - expecting Security Mode Complete\n", message_type);
                }
            } else if (is_service_response) {
                if (request_type == RESP_TYPE_SEC_MODE_DATA) {
                    printf("[AMF] AUTHENTICATED: Accepting security mode data response from AUSF\n");
                    trigger_event(EVENT_AMF_FINALIZE_SEC_MODE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] AUTHENTICATED: Invalid response type 0x%02x - only security mode data expected\n", request_type);
                }
            }
            break;
            
        case UE_STATE_SECURITY_ESTABLISHED:
            if (is_nas_message) {
                if (message_type == NAS_MSG_REGISTRATION_COMPLETE) {
                    printf("[AMF] SECURITY_ESTABLISHED: Accepting Registration Complete\n");
                    trigger_event(EVENT_AMF_REGISTRATION_COMPLETE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_SECURITY_MODE_COMPLETE) {
                    printf("[AMF] SECURITY_ESTABLISHED: Accepting Security Mode Complete (retry)\n");
                    trigger_event(EVENT_AMF_SECURITY_MODE_COMPLETE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_REQUEST) {
                    printf("[AMF] SECURITY_ESTABLISHED: Accepting Registration Request (restart)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] SECURITY_ESTABLISHED: Invalid NAS message 0x%02x - expecting Registration Complete\n", message_type);
                }
            } else if (is_service_response) {
                if (request_type == RESP_TYPE_SUBSCRIPTION_DATA) {
                    printf("[AMF] SECURITY_ESTABLISHED: Accepting subscription data response from UDM\n");
                    trigger_event(EVENT_AMF_FINALIZE_REG_ACCEPT, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] SECURITY_ESTABLISHED: Invalid response type 0x%02x - only subscription data expected\n", request_type);
                }
            }
            break;
            
        case UE_STATE_REGISTERED:
            if (is_nas_message) {
                if (message_type == NAS_MSG_PDU_SESSION_EST_REQ || message_type == 0x67) {
                    printf("[AMF] REGISTERED: Accepting PDU Session Request\n");
                    trigger_event(EVENT_AMF_PDU_SESSION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_COMPLETE) {
                    printf("[AMF] REGISTERED: Accepting Registration Complete (retry)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_COMPLETE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_REQUEST) {
                    printf("[AMF] REGISTERED: Accepting Registration Request (re-registration)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] REGISTERED: Invalid NAS message 0x%02x\n", message_type);
                }
            } else if (is_service_response) {
                if (request_type == RESP_TYPE_CONFIG_DATA) {
                    printf("[AMF] REGISTERED: Accepting configuration data response from PCF\n");
                    trigger_event(EVENT_AMF_FINALIZE_CONFIG_UPDATE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (request_type == RESP_TYPE_PDU_SESSION_CREATED) {
                    printf("[AMF] REGISTERED: Accepting PDU session created response from SMF\n");
                    trigger_event(EVENT_AMF_FINALIZE_PDU_ACCEPT, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] REGISTERED: Invalid response type 0x%02x\n", request_type);
                }
            }
            break;
            
        case UE_STATE_PDU_SESSION_ACTIVE:
            if (is_nas_message) {
                if (message_type == NAS_MSG_PDU_SESSION_EST_REQ || message_type == 0x67) {
                    printf("[AMF] PDU_SESSION_ACTIVE: Accepting PDU Session Request (new session)\n");
                    trigger_event(EVENT_AMF_PDU_SESSION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_REQUEST) {
                    printf("[AMF] PDU_SESSION_ACTIVE: Accepting Registration Request (mobility)\n");
                    trigger_event(EVENT_AMF_REGISTRATION_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else if (message_type == NAS_MSG_REGISTRATION_COMPLETE) {
                    printf("[AMF] PDU_SESSION_ACTIVE: Accepting Registration Complete\n");
                    trigger_event(EVENT_AMF_REGISTRATION_COMPLETE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
                } else {
                    printf("[AMF] PDU_SESSION_ACTIVE: Invalid NAS message 0x%02x\n", message_type);
                }
            } else if (is_service_response) {
                // Could handle PDU modification responses here in the future
                printf("[AMF] PDU_SESSION_ACTIVE: Response type 0x%02x not expected in this state\n", request_type);
            }
            break;
            
        default:
            printf("[AMF] ERROR: Unknown UE state %d\n", ue_state);
            break;
    }
}

// Phase 1: Registration Request → Authentication Request
EVENT_HANDLER(amf_handle_registration_request)
{
    printf("[AMF] Handling registration request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Save the input registration request for validation
    // In production, would extract SUCI and validate fields here
    
    // Build authentication request with AMF's portion
    // Clear the payload for building authentication request
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set request type for AUSF
    EVENT_PAYLOAD[0] = REQ_TYPE_GEN_AUTH_DATA;
    
    // Set EPD header and message type (bytes 1-3)
    EVENT_PAYLOAD[1] = 0x7e;  // EPD
    EVENT_PAYLOAD[2] = 0x00;  // Security header (plain)
    EVENT_PAYLOAD[3] = 0x56;  // Message type (Authentication Request)
    
    // Set ngKSI field (byte 4)
    EVENT_PAYLOAD[4] = 0x00;  // ngKSI: TSC=0, KSI=0
    
    // Set ABBA IE (bytes 5-7)
    EVENT_PAYLOAD[5] = 0x02;  // ABBA length
    EVENT_PAYLOAD[6] = 0x00;  // ABBA contents
    EVENT_PAYLOAD[7] = 0x00;  // ABBA contents
    
    // Set length to account for request type + headers + space for RAND/AUTN
    event_nf_ptr->input_payload_length = 43;  // 1 (type) + 42 (auth request)
    
    printf("[AMF] Prepared authentication headers, requesting AUSF to generate auth data\n");
    printf("[AMF] State transition: DEREGISTERED/REGISTERING → REGISTERING\n");
    ue_state = UE_STATE_REGISTERING;
    
    // Send request to AUSF
    trigger_event(EVENT_TO_AUSF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}


// Handle authentication data response from AUSF
EVENT_HANDLER(amf_finalize_auth_request)
{
    printf("[AMF] Authentication request complete, sending to UE\n");
    
    // Remove the request type byte (shift bytes 1-42 to 0-41)
    memmove(EVENT_PAYLOAD, EVENT_PAYLOAD + 1, 42);
    event_nf_ptr->input_payload_length = 42;
    
    print_nas_pdu("Generated Auth Request", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Send the complete authentication request back to UE
    trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Finalize and send security mode command
EVENT_HANDLER(amf_finalize_sec_mode)
{
    printf("[AMF] Security mode command complete, sending to UE\n");
    
    // Remove the request type byte by shifting everything left by 1
    memmove(EVENT_PAYLOAD, EVENT_PAYLOAD + 1, event_nf_ptr->input_payload_length - 1);
    event_nf_ptr->input_payload_length--;
    
    print_nas_pdu("Generated Security Mode Command", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Send the complete security mode command back to UE
    trigger_event(EVENT_SECURITY_MODE_COMMAND, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Finalize and send registration accept
EVENT_HANDLER(amf_finalize_reg_accept)
{
    printf("[AMF] Registration accept complete, sending to UE\n");
    
    // Add network features at bytes 40-43
    EVENT_PAYLOAD[40] = 0x21;  // Network features IE
    EVENT_PAYLOAD[41] = 0x02;  // Length
    EVENT_PAYLOAD[42] = 0x01;  // 5GS network feature support
    EVENT_PAYLOAD[43] = 0x00;  // Additional features
    
    // Add T3512 timer at bytes 44-46
    EVENT_PAYLOAD[44] = 0x5e;  // GPRS timer IE
    EVENT_PAYLOAD[45] = 0x01;  // Length
    EVENT_PAYLOAD[46] = 0x92;  // Timer value
    
    // Remove the request type byte by shifting everything left by 1
    memmove(EVENT_PAYLOAD, EVENT_PAYLOAD + 1, 46);
    event_nf_ptr->input_payload_length = 46;
    
    print_nas_pdu("Generated Registration Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    printf("[AMF] State transition: SECURITY_ESTABLISHED → REGISTERED\n");
    ue_state = UE_STATE_REGISTERED;
    
    // Send the complete registration accept back to UE
    trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Phase 2: Authentication Response → Security Mode Command
EVENT_HANDLER(amf_handle_authentication_response)
{
    printf("[AMF] Handling authentication response\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Clear the payload for building security mode command
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set request type for AUSF
    EVENT_PAYLOAD[0] = REQ_TYPE_PREPARE_SEC_MODE;
    
    // Build initial structure (bytes 1-10)
    EVENT_PAYLOAD[1] = 0x7e;  // Outer header
    EVENT_PAYLOAD[2] = 0x03;  // Outer header continued
    // MAC will be set by AUSF at bytes 3-6
    EVENT_PAYLOAD[7] = 0x00;  // Sequence number
    EVENT_PAYLOAD[8] = 0x7e;  // Inner header
    EVENT_PAYLOAD[9] = 0x00;  // Inner header continued
    EVENT_PAYLOAD[10] = 0x5d; // Message type
    
    // Update state
    printf("[AMF] State transition: REGISTERING → AUTHENTICATED\n");
    ue_state = UE_STATE_AUTHENTICATED;
    sequence_number++;
    
    // Set initial length (will be expanded by AUSF and UDM)
    event_nf_ptr->input_payload_length = 11;
    
    printf("[AMF] Prepared security mode headers, requesting AUSF to process security mode\n");
    
    // Send to AUSF for security mode processing
    trigger_event(EVENT_TO_AUSF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}


// Phase 3: Security Mode Complete → Registration Accept
EVENT_HANDLER(amf_handle_security_mode_complete)
{
    printf("[AMF] Handling security mode complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Clear the payload for building registration accept
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set request type for UDM
    EVENT_PAYLOAD[0] = REQ_TYPE_PREPARE_REG_ACCEPT;
    
    // Build initial structure (bytes 1-34)
    EVENT_PAYLOAD[1] = 0x7e;  // Outer header
    EVENT_PAYLOAD[2] = 0x02;  // Outer header continued
    EVENT_PAYLOAD[3] = 0x72;  // MAC
    EVENT_PAYLOAD[4] = 0x39;  // MAC
    EVENT_PAYLOAD[5] = 0x67;  // MAC
    EVENT_PAYLOAD[6] = 0x4c;  // MAC
    EVENT_PAYLOAD[7] = 0x01;  // Sequence number
    EVENT_PAYLOAD[8] = 0x7e;  // Inner header
    EVENT_PAYLOAD[9] = 0x00;  // Inner header continued
    EVENT_PAYLOAD[10] = 0x42; // Message type (Registration Accept)
    EVENT_PAYLOAD[11] = 0x01; // Registration result
    EVENT_PAYLOAD[12] = 0x01; // Registration result continued
    
    // GUTI IE (bytes 13-26)
    uint8_t guti_ie[] = {0x77, 0x00, 0x0b, 0xf2, 0x99, 0xf9, 0x07, 0x02, 0x00, 0x40, 0xc0, 0x00, 0x07, 0x27};
    memcpy(&EVENT_PAYLOAD[13], guti_ie, 14);
    
    // TAI list (bytes 27-35)
    uint8_t tai_list[] = {0x54, 0x07, 0x40, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x01};
    memcpy(&EVENT_PAYLOAD[27], tai_list, 9);
    
    // Update state
    printf("[AMF] State transition: AUTHENTICATED → SECURITY_ESTABLISHED\n");
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    sequence_number++;
    
    // Set initial length (will be expanded by UDM)
    event_nf_ptr->input_payload_length = 36;
    
    printf("[AMF] Prepared registration accept headers, requesting UDM to provide subscription data\n");
    
    // Send to UDM for subscription data
    trigger_event(EVENT_TO_UDM, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}


// Phase 4: Registration Complete → Configuration Update
EVENT_HANDLER(amf_handle_registration_complete)
{
    printf("[AMF] Handling registration complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Clear the payload for building configuration update
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set request type for PCF
    EVENT_PAYLOAD[0] = REQ_TYPE_GET_CONFIG_DATA;
    
    // Build initial structure (bytes 1-11)
    EVENT_PAYLOAD[1] = 0x7e;  // Outer header
    EVENT_PAYLOAD[2] = 0x02;  // Outer header continued
    EVENT_PAYLOAD[3] = 0xde;  // MAC
    EVENT_PAYLOAD[4] = 0x0d;  // MAC
    EVENT_PAYLOAD[5] = 0x22;  // MAC
    EVENT_PAYLOAD[6] = 0xe3;  // MAC
    EVENT_PAYLOAD[7] = 0x02;  // Sequence number
    EVENT_PAYLOAD[8] = 0x7e;  // Inner header
    EVENT_PAYLOAD[9] = 0x00;  // Inner header continued
    EVENT_PAYLOAD[10] = 0x54; // Message type (Configuration Update)
    EVENT_PAYLOAD[11] = 0x43; // Start of first IE (will be overwritten by PCF)
    
    // Update state
    sequence_number++;
    
    // Set initial length (will be expanded by PCF)
    event_nf_ptr->input_payload_length = 12;
    
    printf("[AMF] Prepared configuration update headers, requesting PCF to provide configuration data\n");
    
    // Send to PCF for configuration data
    trigger_event(EVENT_TO_PCF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Finalize and send configuration update
EVENT_HANDLER(amf_finalize_config_update)
{
    printf("[AMF] Configuration update complete, sending to UE\n");
    
    // Add timezone info at bytes 39-40
    EVENT_PAYLOAD[39] = 0x46;  // TZ local IE
    EVENT_PAYLOAD[40] = 0x0a;  // TZ value
    
    // Add universal time and timezone at bytes 41-48
    EVENT_PAYLOAD[41] = 0x47;  // TZ and time IE
    EVENT_PAYLOAD[42] = 0x52;  // Year
    EVENT_PAYLOAD[43] = 0x60;  // Month  
    EVENT_PAYLOAD[44] = 0x90;  // Day
    EVENT_PAYLOAD[45] = 0x30;  // Hour
    EVENT_PAYLOAD[46] = 0x35;  // Minute
    EVENT_PAYLOAD[47] = 0x53;  // Second
    EVENT_PAYLOAD[48] = 0x0a;  // Timezone
    
    // Add daylight saving time at bytes 49-51
    EVENT_PAYLOAD[49] = 0x49;  // DST IE
    EVENT_PAYLOAD[50] = 0x01;  // Length
    EVENT_PAYLOAD[51] = 0x01;  // DST value
    
    // Remove the request type byte by shifting everything left by 1
    memmove(EVENT_PAYLOAD, EVENT_PAYLOAD + 1, 51);
    event_nf_ptr->input_payload_length = 51;
    
    print_nas_pdu("Generated Configuration Update", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    // State remains REGISTERED (no transition)
    
    // Send the complete configuration update back to UE
    trigger_event(EVENT_CONFIGURATION_UPDATE, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}


// Phase 5: PDU Session Establishment
EVENT_HANDLER(amf_handle_pdu_session_request)
{
    printf("[AMF] Handling PDU session establishment request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Clear the payload for building PDU session accept
    memset(EVENT_PAYLOAD, 0, MAX_EVENT_PAYLOAD_SIZE);
    
    // Set request type for SMF
    EVENT_PAYLOAD[0] = REQ_TYPE_CREATE_PDU_SESSION;
    
    // Build initial structure (bytes 1-8)
    EVENT_PAYLOAD[1] = 0x7e;  // Outer header
    EVENT_PAYLOAD[2] = 0x02;  // Outer header continued
    EVENT_PAYLOAD[3] = 0xfb;  // MAC
    EVENT_PAYLOAD[4] = 0xd6;  // MAC
    EVENT_PAYLOAD[5] = 0x2d;  // MAC
    EVENT_PAYLOAD[6] = 0x81;  // MAC
    EVENT_PAYLOAD[7] = 0x03;  // Sequence number
    EVENT_PAYLOAD[8] = 0x7e;  // Inner header
    EVENT_PAYLOAD[9] = 0x00;  // Inner header continued
    
    // Update state
    sequence_number++;
    
    // Set initial length (will be expanded by SMF and other NFs)
    event_nf_ptr->input_payload_length = 10;
    
    printf("[AMF] Prepared PDU session headers, forwarding to SMF\n");
    
    // Forward to SMF for PDU session establishment
    trigger_event(EVENT_TO_SMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}

// Finalize and send PDU session accept
EVENT_HANDLER(amf_finalize_pdu_accept)
{
    printf("[AMF] PDU session accept complete, sending to UE\n");
    
    // Remove the request type byte by shifting everything left by 1
    memmove(EVENT_PAYLOAD, EVENT_PAYLOAD + 1, event_nf_ptr->input_payload_length - 1);
    event_nf_ptr->input_payload_length--;
    
    print_nas_pdu("Generated PDU Session Accept", EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    printf("[AMF] State transition: REGISTERED → PDU_SESSION_ACTIVE\n");
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    
    // Send the complete PDU session accept back to UE
    trigger_event(EVENT_PDU_SESSION_ACCEPT, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
    
    // Complete the flow
    printf("[AMF] Registration and PDU session flow completed\n");
    
    // Trigger stop event to exit the application
    trigger_event(EVENT_STOP, NULL, 0);
}

