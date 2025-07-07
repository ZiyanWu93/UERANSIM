#ifndef REGISTRATION_FLOW_H
#define REGISTRATION_FLOW_H

#include "../../src/nflambda/event_system/event.h"
#include "../include/registration_request.h"
#include "../include/registration_complete.h"

// Phase 1: Registration Request Processing Functions

EVENT_HANDLER(amf_handle_registration_request) {
    /*
     * Purpose: Main entry point for processing Registration Request messages
     * 
     * Input fields from EVENT_PAYLOAD:
     * - Complete Registration Request message (25 bytes)
     * - Byte 0: EPD (0x7E)
     * - Byte 1: Security header (0x00 for plain)
     * - Byte 2: Message type (0x41)
     * - Byte 3: Combined registration type and ngKSI
     *   - Bits 0-2: Registration type (1 = initial)
     *   - Bit 3: Follow-on request flag (FOR)
     *   - Bits 4-6: NAS key set identifier (7)
     *   - Bit 7: Type of security context (0)
     * - Bytes 4-19: Mobile Identity (SUCI)
     * - Bytes 20-24: UE Security Capability
     * 
     * Calculated fields:
     * - Extract registration type from input structure (nas_5gs.mm.5gs_reg_type)
     * - Extract NAS key set identifier (nas_5gs.mm.nas_key_set_id.h1)
     * - Extract UE security capabilities bitmap
     * - Store extracted values in UE context
     * 
     * Output: 
     * - Store UE context with registration parameters
     * - Call amf_extract_suci_from_mobile_identity() to process identity
     * - Call amf_validate_registration_request() to check validity
     * - Trigger authentication flow if initial registration
     */
}

EVENT_HANDLER(amf_extract_suci_from_mobile_identity) {
    /*
     * Purpose: Extract and parse SUCI from Mobile Identity IE
     * 
     * Input fields from EVENT_PAYLOAD:
     * - Mobile Identity IE starting at byte 4
     * - Byte 4: Spare half octet (0x00)
     * - Byte 5: Length of mobile identity (0x0D = 13 bytes)
     * - Byte 6: SUCI header
     *   - Bits 0-2: Type ID (1 = SUCI)
     *   - Bit 3: Spare
     *   - Bits 4-6: SUPI format (0 = IMSI)
     *   - Bit 7: Spare
     * - Bytes 7-9: PLMN in BCD format
     *   - MCC = 999, MNC = 70
     * - Bytes 10-11: Routing indicator (0x0000)
     * - Byte 12: Protection scheme (0x00 = null scheme)
     * - Bytes 13-18: MSIN in BCD format (0000000001)
     * 
     * Calculated fields:
     * - Verify mobile identity type field equals 1 (SUCI)
     * - Extract MCC (999), MNC (70), MSIN (0000000001) from binary fields
     * - Check protection scheme field equals 0 (null scheme)
     * - Format as SUCI string for AUSF interface
     * 
     * Output:
     * - Parsed SUCI structure
     * - Formatted SUCI string for AUSF interface
     * - Store in UE context for authentication
     */
}

EVENT_HANDLER(amf_validate_registration_request) {
    /*
     * Purpose: Validate registration request components
     * 
     * Input fields from EVENT_PAYLOAD:
     * - UE Security Capability IE starting at byte 20
     * - Byte 20: IEI (0x2E)
     * - Byte 21: Length (0x04 = 4 bytes)
     * - Byte 22: 5G EA algorithms
     *   - Bit 7: EA0 (1 = supported)
     *   - Bits 0-6: EA1-EA7 (0 = not supported)
     * - Byte 23: 5G IA algorithms  
     *   - Bit 7: IA0 (1 = supported)
     *   - Bit 6: IA1 (1 = supported)
     *   - Bit 5: IA2 (1 = supported)
     *   - Bit 4: IA3 (1 = supported)
     *   - Bits 0-3: IA4-IA7 (0 = not supported)
     * - Byte 24: EPS EA algorithms (0x80)
     * - Byte 25: EPS IA algorithms (0xF0)
     * 
     * Calculated fields:
     * - Check cleartext IEs only (per TS 33.501)
     * - Validate SUCI format
     * - Check registration type
     * - Validate UE security capabilities
     * 
     * Output:
     * - Validation result
     * - Store validated parameters in UE context
     * - Return validation status
     */
}

EVENT_HANDLER(ausf_authenticate_request) {
    /*
     * Purpose: Request authentication vectors from AUSF
     * 
     * Input fields from UE context:
     * - SUCI from amf_extract_suci_from_mobile_identity()
     * - Registration type (initial/mobility/periodic)
     * - ngKSI value from registration request
     * - UE security capabilities
     * 
     * Calculated fields:
     * - Build AuthenticationInfo request
     * - Set supiOrSuci field
     * - Set servingNetworkName: "5G:mnc070.mcc999.3gppnetwork.org"
     * - Send HTTP POST to AUSF
     * 
     * Output:
     * - Authentication vectors from AUSF
     * - Prepare for authentication request building
     * - Store vectors in UE context
     */
}

// Phase 4: Registration Complete Processing Functions

EVENT_HANDLER(amf_handle_registration_complete) {
    /*
     * Purpose: Process Registration Complete message from UE
     * 
     * Input fields from EVENT_PAYLOAD:
     * - Registration Complete NAS PDU (binary structure)
     * - Extract message type field from byte offset 8 (after security header)
     * - Verify message type equals 0x67 (Registration Complete)
     * - If payload container IEI present (0x01), extract PDU session request
     * 
     * Prerequisites:
     * - Security context must be established
     * - Registration Accept must have been sent
     * - NAS keys must be derived
     * 
     * Calculated fields:
     * - Clear T3550 timer (registration procedure timer)
     * - Update UE state to REGISTERED
     * - Check if configuration update needed
     * 
     * Output:
     * - Complete registration procedure
     * - Update UE state to REGISTERED
     * - Check for configuration update policy
     * - Notify other NFs of successful registration
     */
}

EVENT_HANDLER(amf_confirm_guti_assignment) {
    /*
     * Purpose: Confirm GUTI assignment after registration complete
     * 
     * Input from registration flow:
     * - AMF UE context
     * - Confirmed GUTI assignment
     * - Established security context
     * - Selected algorithms
     * - Registration area assignment
     * 
     * Calculated fields:
     * - Copy next.guti to current.guti
     * - Copy next.m_tmsi to current.m_tmsi
     * - Update GUTI hash table for fast lookup
     * - Clear next GUTI fields
     * - Log GUTI confirmation
     * 
     * Output:
     * - Updated GUTI state
     * - Complete UE context in AMF
     * - Ready for service requests
     */
}

#endif // REGISTRATION_FLOW_H