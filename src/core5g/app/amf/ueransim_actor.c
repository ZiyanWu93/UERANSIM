#include "ueransim_actor.h"
#include "amf_common.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

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
static void print_received_pdu(const char* msg_type, const char* pdu)
{
    printf("[UERANSIM] Received %s: %s\n", msg_type, pdu);
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
            const char* reg_request = "7e004179000d0199f9070000000000000000102e0480f080f0";
            
            printf("[UERANSIM] Initiating registration with Registration Request\n");
            printf("[UERANSIM] Sending NAS PDU: %s\n", reg_request);
            ue_state = UE_STATE_REGISTERING;
            
            trigger_event(EVENT_REGISTRATION_REQUEST, reg_request);
            started = true;
        }
    }
}

// Handler for Authentication Request
EVENT_HANDLER(handle_auth_request)
{
    print_received_pdu("Authentication Request", EVENT_PAYLOAD);
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Authentication Response
    const char* auth_response = "7e00572d10ef2770c69e7382aa38e8134f602234e1";
    strcpy(EVENT_PAYLOAD, auth_response);
    
    printf("[UERANSIM] Sending Authentication Response: %s\n", EVENT_PAYLOAD);
    ue_state = UE_STATE_AUTHENTICATED;
    
    // Trigger authentication response event
    trigger_event(EVENT_AUTH_RESPONSE, EVENT_PAYLOAD);
}

// Handler for Security Mode Command
EVENT_HANDLER(handle_security_mode_command)
{
    print_received_pdu("Security Mode Command", EVENT_PAYLOAD);
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Security Mode Complete with embedded Registration Request
    const char* sec_mode_complete = "7e0422e4ee19007e005e7700094573806121856151f17100237e004179000d0199f9070000000000000000101001002e0480f080f02f020101530100";
    strcpy(EVENT_PAYLOAD, sec_mode_complete);
    
    printf("[UERANSIM] Sending Security Mode Complete: %s\n", EVENT_PAYLOAD);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    
    // Trigger security mode complete event
    trigger_event(EVENT_SECURITY_MODE_COMPLETE, EVENT_PAYLOAD);
}

// Handler for Registration Accept
EVENT_HANDLER(handle_registration_accept)
{
    print_received_pdu("Registration Accept", EVENT_PAYLOAD);
    
    // Simulate processing time
    usleep(100000); // 100ms
    
    // Generate Registration Complete
    const char* reg_complete = "7e02469d6a8b017e0043";
    strcpy(EVENT_PAYLOAD, reg_complete);
    
    printf("[UERANSIM] Sending Registration Complete: %s\n", EVENT_PAYLOAD);
    ue_state = UE_STATE_REGISTERED;
    
    // Trigger registration complete event
    trigger_event(EVENT_REGISTRATION_COMPLETE, EVENT_PAYLOAD);
}

// Handler for Configuration Update Command
EVENT_HANDLER(handle_configuration_update)
{
    print_received_pdu("Configuration Update Command", EVENT_PAYLOAD);
    
    // After configuration update, initiate PDU session establishment
    usleep(500000); // 500ms delay before PDU session
    
    // Generate PDU Session Establishment Request
    const char* pdu_session_req = "7e02ba0292cd027e00670100152e0101c1ffff91a12801007b000780000a00000d00120181220101250908696e7465726e6574";
    strcpy(EVENT_PAYLOAD, pdu_session_req);
    
    printf("[UERANSIM] Sending PDU Session Establishment Request: %s\n", EVENT_PAYLOAD);
    
    // Trigger PDU session request event
    trigger_event(EVENT_PDU_SESSION_REQUEST, EVENT_PAYLOAD);
}

// Handler for PDU Session Establishment Accept
EVENT_HANDLER(handle_pdu_session_accept)
{
    print_received_pdu("PDU Session Establishment Accept", EVENT_PAYLOAD);
    
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