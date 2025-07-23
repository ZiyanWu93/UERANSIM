#include "fiveg_core_actor.h"
#include "end_to_end_events.h"
#include "amf.h"
#include "ausf.h"
#include "udm.h"
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

// The AMF-related handlers have been moved to amf.c

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
    
    // Register NF dispatcher for AMF
    register_event_handler(EVENT_TO_AMF, amf_dispatcher);
    
    // Register AMF internal events
    register_event_handler(EVENT_AMF_REGISTRATION_REQUEST, amf_handle_registration_request);
    register_event_handler(EVENT_AMF_AUTH_RESPONSE, amf_handle_authentication_response);
    register_event_handler(EVENT_AMF_SECURITY_MODE_COMPLETE, amf_handle_security_mode_complete);
    register_event_handler(EVENT_AMF_REGISTRATION_COMPLETE, amf_handle_registration_complete);
    register_event_handler(EVENT_AMF_PDU_SESSION_REQUEST, amf_handle_pdu_session_request);
    
    // Register AMF service function chain events
    register_event_handler(EVENT_AMF_SET_AUTH_NGKSI, amf_set_auth_ngksi);
    register_event_handler(EVENT_AMF_SET_ABBA, amf_set_abba);
    register_event_handler(EVENT_AMF_SEND_AUTH_REQUEST, amf_send_auth_request);
    
    // Register AUSF service function chain events
    register_event_handler(EVENT_AUSF_SET_RAND, ausf_set_rand);
    
    // Register UDM service function chain events
    register_event_handler(EVENT_UDM_SET_AUTN, udm_set_autn);
    
    // Control events
    register_event_handler(EVENT_STOP, handle_stop);
}

// Check if the 5G Core actor has stopped
bool fiveg_core_stopped(void)
{
    return stopped;
}