#ifndef AUTHENTICATION_FLOW_H
#define AUTHENTICATION_FLOW_H

#include "../../src/nflambda/event_system/event.h"
#include "../include/authentication_request.h"
#include "../include/authentication_response.h"

// Phase 1: Authentication Request Building Functions

EVENT_HANDLER(udm_generate_authentication_vector) {
    /*
     * Purpose: Generate 5G AKA authentication data
     * 
     * Input:
     * - SUCI: "suci-0-999-70-0-0-0000000001"
     * - Serving network: "5G:mnc070.mcc999.3gppnetwork.org"
     * 
     * Calculated fields:
     * - Decrypt SUCI to SUPI (if encrypted)
     * - Retrieve subscriber key K
     * - Generate RAND (16 random bytes)
     * - Calculate AUTN using Milenage:
     *   - Calculate XRES*, CK', IK', AK
     *   - Calculate MAC = f1(K, SQN||RAND||AMF)
     *   - Calculate AUTN = SQN⊕AK || AMF || MAC
     * - Calculate HXRES* = SHA-256(RAND || XRES*)
     * 
     * Output:
     * - 5G AKA authentication data
     * - RAND and AUTN for Authentication Request
     * - XRES* for later verification
     */
}

EVENT_HANDLER(amf_build_authentication_request) {
    /*
     * Purpose: Build Authentication Request message
     * 
     * Input authentication vectors:
     * - RAND: 16 bytes (e.g., 5c:a0:df:8c:9b:b8:db:cf:3c:2a:7d:d4:48:da:13:69)
     * - AUTN: 16 bytes total
     *   - SQN ⊕ AK: 6 bytes (e.g., 40:62:96:99:30:82)
     *   - AMF: 2 bytes (e.g., 80:00)
     *   - MAC: 8 bytes (e.g., 30:b7:62:45:5c:89:0b:19)
     * 
     * Calculated message fields:
     * - Set message type = 0x56
     * - Set ngKSI.tsc = 0, ngKSI.ksi = 0
     * - Set ABBA = 0x0000
     * - Copy RAND (16 bytes)
     * - Copy AUTN (16 bytes)
     * - Encode as plain NAS message
     * 
     * Output:
     * - Authentication Request NAS PDU
     * - Total message size: 42 bytes
     * - Ready for transmission to UE
     */
}

EVENT_HANDLER(amf_send_authentication_request) {
    /*
     * Purpose: Send Authentication Request to UE via RAN
     * 
     * Input:
     * - Built Authentication Request message (42 bytes)
     * - RAN UE context for delivery
     * 
     * Actions:
     * - Get RAN UE context
     * - Send via NGAP interface
     * - Start T3560 timer for response timeout
     * 
     * Output:
     * - Message sent to RAN
     * - Timer started for response
     * - Ready to receive Authentication Response
     */
}

// Phase 2: Authentication Response Processing Functions

EVENT_HANDLER(amf_handle_authentication_response) {
    /*
     * Purpose: Process Authentication Response from UE
     * 
     * Input:
     * - Authentication Response binary structure
     * 
     * Calculated fields:
     * - Extract RES* parameter from nas_5gs.mm.auth_resp_par field (16 bytes)
     * - Verify message type equals 0x57 (Authentication Response)
     * - Check RES* length field matches expected 16 bytes
     * - Store RES* value in UE context for verification
     * 
     * Output:
     * - Trigger security establishment flow
     * - RES* extracted for verification
     * - Ready for authentication verification
     */
}

EVENT_HANDLER(amf_verify_res_star) {
    /*
     * Purpose: Verify RES* matches expected XRES*
     * 
     * Input:
     * - Received RES* from UE (16 bytes)
     * - Stored authentication data from UDM
     * 
     * Calculated fields:
     * - Calculate HXRES* from received RES*:
     *   - HXRES* = SHA-256(RAND || RES*)
     * - Compare with expected HXRES* from authentication vector
     * - Both should be 32-byte SHA-256 hash outputs
     * - Must match exactly for success
     * 
     * Output:
     * - Verification result
     * - If successful, proceed to key derivation
     * - If failed, reject with cause
     */
}

EVENT_HANDLER(ausf_confirm_authentication) {
    /*
     * Purpose: Confirm authentication with AUSF
     * 
     * Input:
     * - HXRES* verification result
     * - Authentication vectors from UDM
     * 
     * Actions:
     * - Send confirmation to AUSF
     * - Include HXRES* and result
     * - Receive KSEAF if successful
     * 
     * Output:
     * - Authentication confirmation
     * - KSEAF for key derivation
     * - Ready for AMF key derivation
     */
}

EVENT_HANDLER(ausf_derive_kseaf) {
    /*
     * Purpose: Derive KSEAF in AUSF
     * 
     * Input:
     * - KAUSF from authentication vector
     * - Serving network name
     * 
     * Key derivation:
     * - KSEAF = KDF(KAUSF, serving network name)
     * - KSEAF is the anchor key for SEAF (AMF)
     * - Used by AMF to derive KAMF
     * 
     * Calculated fields:
     * - KSEAF (32 bytes)
     * - Key derivation uses HMAC-SHA-256
     * - Send to AMF for further derivation
     * 
     * Output:
     * - KSEAF delivered to AMF
     * - Ready for AMF security key derivation
     */
}

EVENT_HANDLER(amf_derive_security_keys) {
    /*
     * Purpose: Derive AMF security keys after authentication
     * 
     * Input:
     * - KSEAF from AUSF
     * - SUPI (from de-concealed SUCI)
     * - ABBA parameter (0x0000)
     * 
     * Key derivation:
     * - KAMF = KDF(KSEAF, SUPI, ABBA)
     * - KAMF is the anchor key for AMF
     * - Used to derive NAS keys
     * 
     * Calculated fields:
     * - KAMF (32 bytes)
     * - Key derivation uses HMAC-SHA-256
     * - Store for NAS key derivation
     * 
     * Output:
     * - KAMF stored in security context
     * - Ready for algorithm selection
     * - Proceed to security mode command
     */
}

#endif // AUTHENTICATION_FLOW_H