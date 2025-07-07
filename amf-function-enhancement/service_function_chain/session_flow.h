#ifndef SESSION_FLOW_H
#define SESSION_FLOW_H

#include "../../src/nflambda/event_system/event.h"
#include "../include/pdu_session_establishment_request.h"
#include "../include/pdu_session_establishment_accept.h"

// Phase 5: PDU Session Establishment Request Processing Functions

EVENT_HANDLER(amf_handle_ul_nas_transport) {
    /*
     * Purpose: Process UL NAS Transport with PDU session request
     * 
     * Input:
     * - UL NAS Transport binary structure
     * 
     * Calculated fields:
     * - Apply decryption and verify integrity using established NAS security
     * - Extract payload container type field - verify equals 0x01 (N1 SM information)
     * - Extract PDU session ID from request (value: 5)
     * - Extract the embedded PDU session establishment request buffer
     * - Create or find session context for PDU session ID
     * 
     * Output:
     * - Trigger SMF interaction
     * - PDU session context created
     * - Ready to extract session request
     */
}

EVENT_HANDLER(amf_extract_pdu_session_request) {
    /*
     * Purpose: Extract PDU session request from payload container
     * 
     * Input:
     * - Payload container from UL NAS transport
     * 
     * Calculated fields:
     * - Extract 5GSM header fields: EPD (0x2E), PSI (5), PTI (0)
     * - Extract PDU session type field (value: 1 for IPv4)
     * - Extract SSC mode field if present (default: mode 1)
     * - Extract integrity protection max data rate fields
     * - Extract 5GSM capability bitmap if present
     * - Extract optional IEs: S-NSSAI, DNN, etc.
     * 
     * Output:
     * - PDU session request structure
     * - Session parameters extracted
     * - Ready for validation
     */
}

EVENT_HANDLER(amf_validate_snssai_and_dnn) {
    /*
     * Purpose: Validate S-NSSAI and DNN for session
     * 
     * Input:
     * - Requested S-NSSAI from session request
     * - Requested DNN from session request
     * - UE subscription data (allowed S-NSSAIs)
     * - AMF configuration (supported slices/DNNs)
     * 
     * Validation process:
     * - Check S-NSSAI against UE subscription
     * - Verify DNN authorization for user
     * - Apply local policies
     * - Select default if not specified
     * 
     * Calculated fields:
     * - Authorized S-NSSAI for session
     * - Resolved DNN name
     * - Session authorization result
     * 
     * Output:
     * - Validation result
     * - Authorized session parameters
     * - Ready for SMF selection
     */
}

EVENT_HANDLER(amf_discover_and_select_smf) {
    /*
     * Purpose: Discover and select SMF for session
     * 
     * Input:
     * - Authorized S-NSSAI
     * - DNN name
     * - PDU session type
     * - UE location (TAI)
     * - Session requirements
     * 
     * SMF discovery process:
     * - Query local configuration first
     * - Query NRF if configured
     * - Apply SMF selection policies
     * - Consider load balancing
     * - Check SMF capabilities
     * 
     * Calculated fields:
     * - Selected SMF instance
     * - SMF endpoint URI
     * - SMF capabilities match
     * 
     * Output:
     * - SMF selected successfully
     * - SMF context information
     * - Ready to create SM context
     */
}

EVENT_HANDLER(amf_create_sm_context_request) {
    /*
     * Purpose: Create SM context request to SMF
     * 
     * Input:
     * - Selected SMF information
     * - PDU session request parameters
     * - UE context information
     * 
     * SM context creation:
     * - Build HTTP POST request to SMF
     * - Include SUPI, PDU session ID
     * - Include S-NSSAI, DNN
     * - Include UE location, access type
     * - Include N1 SM container
     * 
     * Calculated fields:
     * - SM context request structure
     * - N1 SM information container
     * - Session establishment parameters
     * 
     * Output:
     * - SM context creation request
     * - Ready to send to SMF
     * - Waiting for SMF response
     */
}

// SMF Processing Functions

EVENT_HANDLER(smf_handle_create_sm_context) {
    /*
     * Purpose: Handle SM context creation in SMF
     * 
     * Input:
     * - SM context creation request from AMF
     * - PDU session establishment request
     * - UE and session information
     * 
     * SM context processing:
     * - Create new SM context
     * - Validate session parameters
     * - Select UPF for session
     * - Allocate session resources
     * 
     * Calculated fields:
     * - SM context identifier
     * - Session management state
     * - Resource allocation plan
     * 
     * Output:
     * - SM context created
     * - Ready for IP allocation
     * - UPF selection completed
     */
}

EVENT_HANDLER(smf_allocate_ip_address) {
    /*
     * Purpose: Allocate IP address for PDU session
     * 
     * Input:
     * - PDU session type (IPv4/IPv6/IPv4v6)
     * - DNN configuration
     * - UE subscription information
     * 
     * IP allocation process:
     * - Select IP pool based on DNN
     * - Allocate IPv4 address if requested
     * - Allocate IPv6 prefix if requested
     * - Ensure no address conflicts
     * - Update DHCP/IP management
     * 
     * Calculated fields:
     * - Allocated IPv4 address
     * - Allocated IPv6 prefix
     * - IP address lease information
     * 
     * Output:
     * - IP address allocated
     * - PDU address for UE
     * - Ready for QoS setup
     */
}

EVENT_HANDLER(smf_create_default_qos_flows) {
    /*
     * Purpose: Create default QoS flows for session
     * 
     * Input:
     * - Session requirements
     * - UE subscription QoS
     * - DNN QoS policies
     * 
     * QoS flow creation:
     * - Create default QoS flow
     * - Assign QFI (QoS Flow Identifier)
     * - Set QoS parameters (5QI, ARP, GFBR, MFBR)
     * - Create QoS rules for UE
     * 
     * Calculated fields:
     * - Default QoS flow parameters
     * - QFI assignment
     * - QoS rule encoding
     * - Session AMBR values
     * 
     * Output:
     * - QoS flows established
     * - QoS rules for UE
     * - Ready to build accept message
     */
}

EVENT_HANDLER(smf_build_pdu_session_accept) {
    /*
     * Purpose: Build PDU Session Establishment Accept
     * 
     * Input:
     * - Allocated IP address
     * - Created QoS flows
     * - Session parameters
     * 
     * Message construction:
     * - Set message type = 0xC2 (Accept)
     * - Include selected PDU session type
     * - Include selected SSC mode
     * - Add PDU address IE
     * - Add authorized QoS rules
     * - Add session AMBR
     * - Add optional IEs as needed
     * 
     * Calculated fields:
     * - Complete PDU Session Accept
     * - N1 SM information container
     * - Message length and encoding
     * 
     * Output:
     * - PDU Session Accept message
     * - N1 SM container for AMF
     * - Ready for N1N2 transfer
     */
}

// AMF Processing Functions

EVENT_HANDLER(amf_build_dl_nas_transport) {
    /*
     * Purpose: Build DL NAS Transport for session accept
     * 
     * Input:
     * - PDU Session Accept from SMF
     * - N2 SM information from SMF
     * - UE context and security
     * 
     * DL NAS Transport construction:
     * - Set message type = 0x04 (DL NAS Transport)
     * - Set payload container type = 0x01 (N1 SM)
     * - Include PDU session ID
     * - Include N1 SM container
     * - Apply NAS security protection
     * 
     * Calculated fields:
     * - Complete DL NAS Transport
     * - Integrity protection applied
     * - Ciphering applied if enabled
     * 
     * Output:
     * - DL NAS Transport message
     * - Ready for transmission
     * - N2 information for RAN
     */
}

EVENT_HANDLER(amf_send_dl_nas_transport) {
    /*
     * Purpose: Send DL NAS Transport to UE
     * 
     * Input:
     * - Built DL NAS Transport message
     * - N2 SM information for RAN
     * - UE context with RAN connection
     * 
     * N1N2 message transfer:
     * - Send N1 message (DL NAS Transport) to UE
     * - Send N2 SM information to RAN
     * - Include resource setup request
     * - Start relevant timers
     * 
     * Calculated fields:
     * - N1N2 transfer parameters
     * - Resource setup requirements
     * - Timer management
     * 
     * Output:
     * - PDU session accept sent to UE
     * - RAN resource setup initiated
     * - Session establishment in progress
     */
}

#endif // SESSION_FLOW_H