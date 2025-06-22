//
// AMF Actor Implementation
// Created on 2025-06-18
//

#include "../event_system/event.h"
#include "../amf.h"
#include "../event_utils.h"

// ----------------------------------------------------------------------------------
// Authentication Request Functions
// ----------------------------------------------------------------------------------

// Authentication Request - First step in registration sequence
EVENT_HANDLER(amf_process_registration_request)
{
    // Print the input NAS PDU using the shared utility function
    print_event_input_payload("amf_process_registration_request");
    
    // Parse registration request and cache provisional UE context
    // For now, directly generate the authentication request
    extern void generate_auth_req(void);
    generate_auth_req();
}

// Authentication Request - Build PDU
EVENT_HANDLER(amf_build_auth_request_pdu)
{
    // Concatenate header, ABBA, RAND and AUTN fields
    // To be implemented
}

// ----------------------------------------------------------------------------------
// Security Mode Command Functions
// ----------------------------------------------------------------------------------

// Security Command - Process UE Authentication Response
EVENT_HANDLER(amf_handle_auth_response)
{
    // Print the input NAS PDU using the shared utility function
    print_event_input_payload("amf_handle_auth_response");
    
    // Process the Authentication Response from UE
    // For now, directly generate the security command
    extern void generate_security_cmd(void);
    generate_security_cmd();
}

// Security Command - Verify RES*
EVENT_HANDLER(amf_verify_res_star)
{
    // Verify the RES* against stored HXRES*
    // To be implemented
}

// Security Command - Generate KAMF
EVENT_HANDLER(amf_derive_kamf)
{
    // Generate the KAMF anchor key
    // To be implemented
}

// Security Command - Select Security Algorithms
EVENT_HANDLER(amf_select_security_algorithms)
{
    // Select integrity/ciphering algorithms
    // To be implemented
}

// Security Command - Derive NAS Keys
EVENT_HANDLER(amf_derive_nas_keys)
{
    // Derive NAS integrity and encryption keys
    // To be implemented
}

// Security Command - Build PDU
EVENT_HANDLER(amf_build_security_mode_command_pdu)
{
    // Assemble security mode command
    // To be implemented
}

// Security Command - Apply Integrity Protection
EVENT_HANDLER(amf_compute_mac_and_wrap_security_container)
{
    // Apply integrity protection
    // To be implemented
}

// ----------------------------------------------------------------------------------
// Registration Accept Functions
// ----------------------------------------------------------------------------------

// Registration Accept - Process Security Mode Complete
EVENT_HANDLER(amf_handle_security_mode_complete)
{
    // Print the input NAS PDU using the shared utility function
    print_event_input_payload("amf_handle_security_mode_complete");
    
    // Process security mode complete message
    // For now, directly generate the registration accept
    extern void generate_registration_accept(void);
    generate_registration_accept();
}

// Registration Accept - Verify UE Security Capabilities
EVENT_HANDLER(amf_verify_ue_security_capabilities)
{
    // Verify replayed security capabilities
    // To be implemented
}

// Registration Accept - Create UE Context
EVENT_HANDLER(amf_create_ue_context)
{
    // Create permanent UE context
    // To be implemented
}

// Registration Accept - Select Network Slices
EVENT_HANDLER(amf_select_allowed_nssai)
{
    // Select allowed network slice information
    // To be implemented
}

// Registration Accept - Allocate GUTI
EVENT_HANDLER(amf_allocate_guti)
{
    // Allocate globally unique temporary ID
    // To be implemented
}

// Registration Accept - Build PDU
EVENT_HANDLER(amf_build_registration_accept_pdu)
{
    // Assemble registration accept message
    // To be implemented
}

// ----------------------------------------------------------------------------------
// Configuration Update Functions
// ----------------------------------------------------------------------------------

// Configuration Update - Process Trigger
EVENT_HANDLER(amf_handle_configuration_update_trigger)
{
    // Process configuration update trigger
    // For now, directly generate the configuration update
    extern void generate_configuration_update(void);
    generate_configuration_update();
}

// Configuration Update - Prepare Network Parameters
EVENT_HANDLER(amf_prepare_network_parameters)
{
    // Prepare network name, time zone data
    // To be implemented
}

// Configuration Update - Build PDU
EVENT_HANDLER(amf_build_configuration_update_pdu)
{
    // Assemble configuration update command
    // To be implemented
}

// ----------------------------------------------------------------------------------
// PDU Session Establishment Functions
// ----------------------------------------------------------------------------------

// PDU Session - Process Establishment Request
EVENT_HANDLER(amf_handle_pdu_session_establishment_request)
{
    // Print the input NAS PDU using the shared utility function
    print_event_input_payload("amf_handle_pdu_session_establishment_request");
    
    // Process PDU session request
    // For now, directly generate the PDU session establishment accept
    extern void generate_pdu_session_establishment(void);
    generate_pdu_session_establishment();
}

// PDU Session - Select SMF
EVENT_HANDLER(amf_select_smf)
{
    // Select session management function
    // To be implemented
}

// PDU Session - Build Accept PDU
EVENT_HANDLER(amf_build_pdu_session_establishment_accept)
{
    // Assemble accept message
    // To be implemented
}

// ----------------------------------------------------------------------------------
// Common Downlink Transport Function
// ----------------------------------------------------------------------------------

// Common - Send to gNB
EVENT_HANDLER(amf_send_downlink_nas_transport)
{
    // Print the input NAS PDU using the shared utility function
    print_event_input_payload("amf_send_downlink_nas_transport");
    
    // Wrap PDU in Downlink NAS Transport NGAP message for gNB
    // To be implemented
}