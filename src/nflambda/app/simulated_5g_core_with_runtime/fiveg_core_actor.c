#include "fiveg_core_actor.h"
#include "end_to_end_events.h"
#include "amf.h"
#include "ausf.h"
#include "udm.h"
#include "pcf.h"
#include "smf.h"
#include "upf.h"
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
    
    // Register NF dispatchers
    register_event_handler(EVENT_TO_AMF, amf_dispatcher);
    register_event_handler(EVENT_TO_AUSF, ausf_dispatcher);
    register_event_handler(EVENT_TO_UDM, udm_dispatcher);
    register_event_handler(EVENT_TO_PCF, pcf_dispatcher);
    register_event_handler(EVENT_TO_SMF, smf_dispatcher);
    register_event_handler(EVENT_TO_UPF, upf_dispatcher);
    
    // Register AMF internal events
    register_event_handler(EVENT_AMF_REGISTRATION_REQUEST, amf_handle_registration_request);
    register_event_handler(EVENT_AMF_AUTH_RESPONSE, amf_handle_authentication_response);
    register_event_handler(EVENT_AMF_SECURITY_MODE_COMPLETE, amf_handle_security_mode_complete);
    register_event_handler(EVENT_AMF_REGISTRATION_COMPLETE, amf_handle_registration_complete);
    register_event_handler(EVENT_AMF_PDU_SESSION_REQUEST, amf_handle_pdu_session_request);
    register_event_handler(EVENT_AMF_FINALIZE_AUTH_REQUEST, amf_finalize_auth_request);
    register_event_handler(EVENT_AMF_FINALIZE_SEC_MODE, amf_finalize_sec_mode);
    register_event_handler(EVENT_AMF_FINALIZE_REG_ACCEPT, amf_finalize_reg_accept);
    register_event_handler(EVENT_AMF_FINALIZE_CONFIG_UPDATE, amf_finalize_config_update);
    register_event_handler(EVENT_AMF_FINALIZE_PDU_ACCEPT, amf_finalize_pdu_accept);
    
    // Register AUSF internal events
    register_event_handler(EVENT_AUSF_PROCESS_AUTH_REQ, ausf_process_auth_request);
    register_event_handler(EVENT_AUSF_COMPLETE_AUTH_DATA, ausf_complete_auth_data);
    register_event_handler(EVENT_AUSF_PROCESS_SEC_MODE, ausf_process_sec_mode);
    register_event_handler(EVENT_AUSF_COMPLETE_SEC_MODE, ausf_complete_sec_mode);
    
    // Register UDM internal events
    register_event_handler(EVENT_UDM_GEN_AUTH_VECTORS, udm_gen_auth_vectors);
    register_event_handler(EVENT_UDM_PROVIDE_SEC_CAP, udm_provide_sec_cap);
    register_event_handler(EVENT_UDM_PROVIDE_SUBSCRIPTION, udm_provide_subscription);
    register_event_handler(EVENT_UDM_PROVIDE_DNN, udm_provide_dnn);
    
    // Register PCF internal events
    register_event_handler(EVENT_PCF_PROVIDE_CONFIG, pcf_provide_config);
    register_event_handler(EVENT_PCF_PROVIDE_PCC, pcf_provide_pcc);
    
    // Register SMF internal events
    register_event_handler(EVENT_SMF_PROCESS_PDU_REQ, smf_process_pdu_request);
    register_event_handler(EVENT_SMF_REQUEST_POLICY, smf_request_policy);
    register_event_handler(EVENT_SMF_REQUEST_DNN, smf_request_dnn);
    register_event_handler(EVENT_SMF_COMPLETE_PDU, smf_complete_pdu_session);
    
    // Register UPF internal events
    register_event_handler(EVENT_UPF_ALLOCATE_IP, upf_allocate_ip);
    
    // Control events
    register_event_handler(EVENT_STOP, handle_stop);
}

// Check if the 5G Core actor has stopped
bool fiveg_core_stopped(void)
{
    return stopped;
}