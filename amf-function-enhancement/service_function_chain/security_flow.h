#ifndef SECURITY_FLOW_H
#define SECURITY_FLOW_H

#include "../../src/nflambda/event_system/event.h"
#include "../include/security_mode_command.h"
#include "../include/security_mode_complete.h"

// Phase 2: Security Mode Command Building Functions

EVENT_HANDLER(amf_select_security_algorithms) {
    /*
     * Purpose: Select NAS security algorithms based on UE capabilities
     * 
     * Input from UE security capabilities:
     * - 5G EA capabilities: EA0=1, EA1-EA7=0
     * - 5G IA capabilities: IA0=1, IA1=1, IA2=1, IA3=1, IA4-IA7=0
     * - EPS EA capabilities: EEA0=1, others=0
     * - EPS IA capabilities: EIA0=1, EIA1=1, EIA2=1, EIA3=1, others=0
     * 
     * AMF algorithm priority/policy:
     * - Prefer 5G-IA2 for integrity (128-EIA2)
     * - Use 5G-EA0 for encryption (null encryption)
     * - Must match UE capabilities
     * 
     * Calculated fields:
     * - Selected integrity algorithm: 0x2 (5G-IA2)
     * - Selected ciphering algorithm: 0x0 (5G-EA0)
     * - Algorithm IDs for SelectedAlgorithms field
     * 
     * Output:
     * - Selected algorithm pair stored
     * - Ready for NAS key derivation
     * - Algorithm IDs for Security Mode Command
     */
}

EVENT_HANDLER(amf_build_security_mode_command) {
    /*
     * Purpose: Build Security Mode Command message
     * 
     * Input:
     * - Selected algorithms (integrity=IA2, encryption=EA0)
     * - UE security capabilities from registration
     * - Derived NAS keys
     * 
     * Calculated message structure:
     * - Sets EPD in outer security header
     * - Sets security header type = 0x03 (integrity protected, new context)
     * - Sets message type = 0x5D (Security Mode Command)
     * - Sets selected algorithms (ciphering=0x0, integrity=0x2)
     * - Sets ngKSI (KSI=0, TSC=0)
     * - Replays UE security capabilities
     * - Sets IMEISV request = 0x1 (requested)
     * - Sets additional 5G security info (RINMR=1, HDP=0)
     * 
     * MAC calculation:
     * - Uses AES-CMAC-128 (for 5G-IA2)
     * - Input: KNASint, COUNT=0, Direction=1 (DL)
     * - Message: plain NAS message
     * - Output: 4-byte MAC
     * 
     * Output:
     * - Complete integrity protected message
     * - MAC calculated and inserted
     * - Ready for transmission
     */
}

EVENT_HANDLER(amf_calculate_mac) {
    /*
     * Purpose: Calculate MAC for Security Mode Command
     * 
     * Input for AES-CMAC:
     * - COUNT: 0x00000000 (4 bytes, first downlink message)
     * - BEARER: 0x01 (1 byte, 3GPP access)
     * - DIRECTION: 0x01 (1 byte, downlink)
     * - MESSAGE: Plain NAS message starting after sequence number
     * - Algorithm: AES-CMAC-128 (for 5G-IA2)
     * - Key: KNAS-int (16 bytes derived from KAMF)
     * 
     * Calculated fields:
     * - Full 128-bit MAC from AES-CMAC
     * - Truncated to first 32 bits = MAC value
     * - MAC inserted into security header
     * 
     * Output:
     * - Complete MAC calculation
     * - 4-byte MAC for message header
     * - Message ready for transmission
     */
}

EVENT_HANDLER(amf_send_security_mode_command) {
    /*
     * Purpose: Send Security Mode Command to UE
     * 
     * Input:
     * - Built Security Mode Command (21 bytes)
     * - Security context activated
     * 
     * Actions:
     * - Increment DL NAS COUNT
     * - Send via NGAP to RAN
     * - Start timer T3560
     * - Expect integrity protected response
     * 
     * Output:
     * - Message sent to UE
     * - Waiting for Security Mode Complete
     * - Security context active
     */
}

// Phase 3: Security Mode Complete Processing Functions

EVENT_HANDLER(amf_handle_security_mode_complete) {
    /*
     * Purpose: Process Security Mode Complete from UE
     * 
     * Input:
     * - Security Mode Complete NAS PDU
     * 
     * Verification steps:
     * - Verify integrity protection using established NAS security
     * - Check mandatory NAS message container
     * - Process IMEISV if present
     * - Validate replayed Registration Request
     * 
     * Logic from phase file:
     * - if (!verify_nas_integrity(nas_pdu, amf_ue)) return send_security_reject()
     * - Check presencemask for NAS_MESSAGE_CONTAINER_PRESENT
     * - If IMEISV present, call extract_and_store_imeisv()
     * 
     * Output:
     * - Trigger registration accept flow
     * - Security established successfully
     * - Ready for Registration Accept
     */
}

EVENT_HANDLER(amf_validate_nas_message_container) {
    /*
     * Purpose: Validate NAS message container with initial message
     * 
     * Input:
     * - NAS message container from Security Mode Complete
     * - Original Registration Request stored during initial processing
     * 
     * Validation logic:
     * - Decode container contents as Registration Request
     * - Compare with stored original message
     * - Verify SUCI matches
     * - Verify UE security capabilities match
     * - Verify registration type matches
     * 
     * Calculated fields:
     * - Message comparison result
     * - Field-by-field validation
     * - Replay protection verification
     * 
     * Output:
     * - Container validation result
     * - Replay protection confirmed
     * - Security mode procedure validated
     */
}

EVENT_HANDLER(amf_extract_and_store_imeisv) {
    /*
     * Purpose: Extract and store IMEISV from Security Mode Complete
     * 
     * Input:
     * - IMEISV IE from Security Mode Complete
     * - AMF UE context for storage
     * 
     * IMEISV processing:
     * - Verify IEI = 0x77
     * - Extract length field
     * - Verify type of identity = 3 (IMEISV)
     * - Decode 16 BCD digits
     * - Parse TAC (8 digits), SNR (6 digits), SV (2 digits)
     * 
     * Calculated fields:
     * - IMEISV as string (16 digits)
     * - TAC for device type identification
     * - SNR for device instance
     * - SV for software version
     * 
     * Output:
     * - IMEISV stored in UE context
     * - Device tracking enabled
     * - Equipment identity available for checks
     */
}

EVENT_HANDLER(amf_allocate_5g_guti) {
    /*
     * Purpose: Allocate 5G-GUTI for UE
     * 
     * Input:
     * - AMF configuration (GUAMI)
     * - UE context requiring GUTI
     * 
     * GUTI allocation:
     * - Use configured GUAMI (MCC, MNC, AMF Region ID, AMF Set ID, AMF Pointer)
     * - Generate unique 5G-TMSI
     * - Ensure no collision with existing UEs
     * - Store in UE context as "next" GUTI
     * 
     * Calculated fields:
     * - Complete 5G-GUTI structure
     * - Unique 5G-TMSI value
     * - GUTI for Registration Accept
     * 
     * Output:
     * - 5G-GUTI allocated and stored
     * - Ready for Registration Accept inclusion
     * - UE identity management prepared
     */
}

EVENT_HANDLER(amf_determine_tai_list) {
    /*
     * Purpose: Determine TAI list for UE registration area
     * 
     * Input:
     * - Current TAI from registration
     * - AMF configuration (served TAIs)
     * - UE subscription data
     * 
     * TAI list construction:
     * - Include current TAI
     * - Add neighboring TAIs from configuration
     * - Consider UE mobility patterns
     * - Limit list size per 3GPP specifications
     * 
     * Calculated fields:
     * - TAI list for registration area
     * - Registration area size
     * - TAI encoding for NAS message
     * 
     * Output:
     * - TAI list for Registration Accept
     * - Registration area defined
     * - UE mobility area established
     */
}

EVENT_HANDLER(amf_validate_network_slices) {
    /*
     * Purpose: Validate and authorize network slices for UE
     * 
     * Input:
     * - Requested NSSAI from UE (if any)
     * - Subscribed S-NSSAI from UDM
     * - AMF configuration (supported slices)
     * 
     * NSSAI processing:
     * - Validate requested S-NSSAIs against subscription
     * - Check AMF slice support
     * - Apply operator policies
     * - Determine allowed NSSAI
     * - Set default S-NSSAI if needed
     * 
     * Calculated fields:
     * - Allowed NSSAI for UE
     * - Rejected NSSAI (if any)
     * - Default S-NSSAI selection
     * 
     * Output:
     * - Authorized network slices
     * - NSSAI for Registration Accept
     * - Slice access control established
     */
}

EVENT_HANDLER(amf_build_registration_accept) {
    /*
     * Purpose: Build Registration Accept message
     * 
     * Input:
     * - Allocated 5G-GUTI
     * - TAI list for registration area
     * - Allowed NSSAI
     * - Registration result
     * 
     * Message construction:
     * - Set message type = 0x42 (Registration Accept)
     * - Include 5G registration result
     * - Add 5G-GUTI if allocated
     * - Add TAI list
     * - Add allowed NSSAI
     * - Add equivalent PLMNs if configured
     * - Set registration timer values
     * 
     * Calculated fields:
     * - Complete Registration Accept structure
     * - All mandatory and optional IEs
     * - Message length calculation
     * 
     * Output:
     * - Registration Accept message
     * - Ready for NAS security protection
     * - UE registration parameters defined
     */
}

EVENT_HANDLER(amf_apply_nas_security) {
    /*
     * Purpose: Apply NAS security protection to outgoing message
     * 
     * Input:
     * - Plain NAS message (Registration Accept)
     * - Established NAS security context
     * - Selected algorithms
     * 
     * Security application:
     * - Apply ciphering if algorithm != null
     * - Calculate integrity protection
     * - Increment DL NAS COUNT
     * - Add security header
     * 
     * Calculated fields:
     * - Integrity protected message
     * - Ciphered message (if applicable)
     * - Complete security header
     * - Updated NAS COUNT
     * 
     * Output:
     * - Security protected NAS message
     * - Ready for transmission to UE
     * - NAS security context updated
     */
}

EVENT_HANDLER(amf_send_registration_accept) {
    /*
     * Purpose: Send Registration Accept to UE
     * 
     * Input:
     * - Security protected Registration Accept
     * - UE context with RAN association
     * 
     * Actions:
     * - Send via NGAP DL NAS Transport
     * - Start T3550 timer (Registration Accept timer)
     * - Update UE state
     * - Log successful registration
     * 
     * Output:
     * - Registration Accept sent to UE
     * - Waiting for Registration Complete
     * - UE state updated
     */
}

#endif // SECURITY_FLOW_H