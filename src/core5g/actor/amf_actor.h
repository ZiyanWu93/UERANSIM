#ifndef AMF_ACTOR_H
#define AMF_ACTOR_H

#include "../event_system/event.h"

// Authentication Request Functions
EVENT_HANDLER(amf_process_registration_request);
EVENT_HANDLER(amf_build_auth_request_pdu);

// Security Mode Command Functions
EVENT_HANDLER(amf_handle_auth_response);
EVENT_HANDLER(amf_verify_res_star);
EVENT_HANDLER(amf_derive_kamf);
EVENT_HANDLER(amf_select_security_algorithms);
EVENT_HANDLER(amf_derive_nas_keys);
EVENT_HANDLER(amf_build_security_mode_command_pdu);
EVENT_HANDLER(amf_compute_mac_and_wrap_security_container);

// Registration Accept Functions
EVENT_HANDLER(amf_handle_security_mode_complete);
EVENT_HANDLER(amf_verify_ue_security_capabilities);
EVENT_HANDLER(amf_create_ue_context);
EVENT_HANDLER(amf_select_allowed_nssai);
EVENT_HANDLER(amf_allocate_guti);
EVENT_HANDLER(amf_build_registration_accept_pdu);

// Configuration Update Functions
EVENT_HANDLER(amf_handle_configuration_update_trigger);
EVENT_HANDLER(amf_prepare_network_parameters);
EVENT_HANDLER(amf_build_configuration_update_pdu);

// PDU Session Establishment Functions
EVENT_HANDLER(amf_handle_pdu_session_establishment_request);
EVENT_HANDLER(amf_select_smf);
EVENT_HANDLER(amf_build_pdu_session_establishment_accept);

// Common Downlink Transport Function
EVENT_HANDLER(amf_send_downlink_nas_transport);

#endif // AMF_ACTOR_H