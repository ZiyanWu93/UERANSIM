# Goal: Refactor AMF Actor

## steps

### Refactoring for Initial Registration High Level Goal

1. **Refactor generate_auth_req**

   **Function Chain:**
   - `amf_process_registration_request()` - Parse registration request, cache UE context
   - `udm_decrypt_suci()` - Recover SUPI from SUCI
   - `ausf_initiate_authentication()` - Interface with AUSF
   - `udm_generate_5g_aka_vector()` - Generate authentication vectors
   - `ausf_prepare_hxres_star()` - Store HXRES* and return data to AMF
   - `amf_build_auth_request_pdu()` - Assemble authentication PDU
   - `amf_send_downlink_nas_transport()` - Deliver to gNB
2. **Refactor generate_security_cmd**

   **Function Chain:**
   - `amf_handle_auth_response()` - Process UE Authentication Response
   - `amf_verify_res_star()` - Verify the RES* against stored HXRES*
   - `amf_derive_kamf()` - Generate the KAMF anchor key
   - `amf_select_security_algorithms()` - Select integrity/ciphering algorithms
   - `amf_derive_nas_keys()` - Derive NAS integrity and encryption keys
   - `amf_build_security_mode_command_pdu()` - Assemble security mode command
   - `amf_compute_mac_and_wrap_security_container()` - Apply integrity protection
   - `amf_send_downlink_nas_transport()` - Deliver to gNB
3. **Refactor generate_registration_accept**

   **Function Chain:**
   - `amf_handle_security_mode_complete()` - Process security mode complete message
   - `amf_verify_ue_security_capabilities()` - Verify replayed security capabilities
   - `amf_create_ue_context()` - Create permanent UE context
   - `amf_select_allowed_nssai()` - Select allowed network slice information
   - `amf_allocate_guti()` - Allocate globally unique temporary ID
   - `amf_build_registration_accept_pdu()` - Assemble registration accept message
   - `amf_send_downlink_nas_transport()` - Deliver to gNB
4. **Refactor generate_configuration_update**

   **Function Chain:**
   - `amf_handle_configuration_update_trigger()` - Process configuration update trigger
   - `amf_prepare_network_parameters()` - Prepare network name, time zone data
   - `amf_build_configuration_update_pdu()` - Assemble configuration update command
   - `amf_compute_mac_and_wrap_security_container()` - Apply integrity protection
   - `amf_send_downlink_nas_transport()` - Deliver to gNB
5. **Refactor generate_pdu_session_establishment**

   **Function Chain:**
   - `amf_handle_pdu_session_establishment_request()` - Process PDU session request
   - `amf_select_smf()` - Select session management function
   - `smf_create_session()` - Create session in SMF
   - `smf_allocate_upf_resources()` - Allocate user plane resources
   - `smf_create_pdu_session()` - Create PDU session parameters
   - `amf_build_pdu_session_establishment_accept()` - Assemble accept message
   - `amf_compute_mac_and_wrap_security_container()` - Apply integrity protection
   - `amf_send_downlink_nas_transport()` - Deliver to gNB

### Steps

1. [X] Create {nf}_actor file for all related network functions
2. [X] Create related event handler for all the functions above, with EVENT_HANDLER macro; for the implementation, just leave it empty for now.
3. [X] Group all the actors into actor folder [Commit: f40a308]
    - create folder, move all the {nf}_actor.c and {nf}_actor.h files into the actor folder
    - create CMakeLists.txt file in the actor folder
    - update CMakeLists.txt in the root folder