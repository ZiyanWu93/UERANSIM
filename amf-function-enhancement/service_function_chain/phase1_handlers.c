#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../src/nflambda/event_system/event.h"
#include "../include/registration_request.h"
#include "../include/authentication_request.h"

// Phase 1: Registration Request → Authentication Request
// Implementation following ROADMAP-phase1.md

// Global variables for phase 1 transformation
static uint8_t g_input_buffer[256];
static uint8_t g_output_buffer[256];
static size_t g_input_len;
static size_t g_output_len;

// Working variables
static RegistrationRequest *g_reg_req;
static AuthenticationRequest *g_auth_req;
static uint8_t g_rand[16];
static uint8_t g_autn[16];

// Extracted values
static uint16_t g_mcc;
static uint16_t g_mnc;
static uint8_t g_msin[6];  // BCD format

// Phase 1: Registration Request Processing Functions

EVENT_HANDLER(amf_handle_registration_request) {
    // Cast input buffer to Registration Request structure
    g_reg_req = (RegistrationRequest *)g_input_buffer;
    
    // According to ROADMAP, this function should:
    // - Extract registration type from input structure
    // - Extract NAS key set identifier
    // - Extract UE security capabilities bitmap
    // - Store extracted values in UE context
    
    // Extract registration type from combined field
    uint8_t reg_type = g_reg_req->reg_type_ngksi.registration_type;
    uint8_t ksi = g_reg_req->reg_type_ngksi.nas_key_set_id;
    uint8_t tsc = g_reg_req->reg_type_ngksi.tsc;
    uint8_t for_bit = g_reg_req->reg_type_ngksi.follow_on_request;
    
    // Store extracted values for later use
    // For phase 1, we validate and store these values
    if (reg_type != 1 || ksi != 7 || tsc != 0 || for_bit != 1) {
        return;
    }
    
    // Extract UE security capabilities
    // In production, would store full capability bitmap
}

EVENT_HANDLER(amf_extract_suci_from_mobile_identity) {
    // According to ROADMAP, this function should:
    // - Verify mobile identity type field equals 1 (SUCI)
    // - Extract MCC, MNC, MSIN from binary fields
    // - Check protection scheme field equals 0 (null scheme)
    // - Format as SUCI string for AUSF interface
    
    // Work with mobile identity from global registration request
    MobileIdentitySuci *mobile_id = &g_reg_req->mobile_identity;
    
    // Verify mobile identity type field equals 1 (SUCI)
    if (mobile_id->type_id != 1) {
        return;
    }
    
    // Check protection scheme (should be 0 for null scheme)
    if (mobile_id->protection_scheme_id != 0) {
        return;
    }
    
    // Extract MCC (999) from BCD format
    g_mcc = mobile_id->mcc_digit1 * 100 + 
            mobile_id->mcc_digit2 * 10 + 
            mobile_id->mcc_digit3;
    
    // Extract MNC (70) - 2-digit MNC (mnc_digit3 = 0xF)
    g_mnc = mobile_id->mnc_digit1 * 10 + mobile_id->mnc_digit2;
    
    // Store MSIN in BCD format for later use
    memcpy(g_msin, mobile_id->msin, 6);
    
    // In production, would format as SUCI string for AUSF
}

EVENT_HANDLER(amf_validate_registration_request) {
    // Check cleartext IEs only (per TS 33.501)
    if (g_reg_req->epd != 0x7E) return;
    if (g_reg_req->message_type != 0x41) return;
    if (g_reg_req->reg_type_ngksi.registration_type != 1) return;
    
    // Validate SUCI format
    if (g_reg_req->mobile_identity.type_id != 1) return;
    if (g_reg_req->mobile_identity.supi_format != 0) return;
    
    // Check UE security capabilities
    if (g_reg_req->security_capability.iei != 0x2E) return;
    if (g_reg_req->security_capability.length != 4) return;
}

EVENT_HANDLER(ausf_authenticate_request) {
    // For phase 1, we skip the actual AUSF call
    // servingNetworkName would be: "5G:mnc070.mcc999.3gppnetwork.org"
    // But we use hardcoded values
}

// Authentication Request Building Functions

EVENT_HANDLER(udm_generate_authentication_vector) {
    // Hardcoded RAND from ROADMAP-phase1.md
    uint8_t rand_value[16] = {
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf,
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69
    };
    memcpy(g_rand, rand_value, 16);
    
    // Hardcoded AUTN = SQN⊕AK || AMF || MAC
    uint8_t autn_value[16] = {
        // SQN⊕AK (6 bytes)
        0x40, 0x62, 0x96, 0x99, 0x30, 0x82,
        // AMF (2 bytes)
        0x80, 0x00,
        // MAC (8 bytes)
        0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19
    };
    memcpy(g_autn, autn_value, 16);
}

EVENT_HANDLER(amf_build_authentication_request) {
    // According to ROADMAP, this function should:
    // - Set message type = 0x56
    // - Set ngKSI.tsc = 0, ngKSI.ksi = 0 (derived fields)
    // - Set ABBA = 0x0000
    // - Copy RAND (16 bytes) from authentication vector
    // - Copy AUTN (16 bytes) from authentication vector
    // - Encode as plain NAS message
    
    // Clear output buffer
    memset(g_output_buffer, 0, sizeof(AuthenticationRequest));
    g_auth_req = (AuthenticationRequest *)g_output_buffer;
    
    // Static fields (Direct Assignment)
    g_auth_req->epd = 0x7E;                    // Extended Protocol Discriminator
    g_auth_req->security_header = 0;            // Plain message
    g_auth_req->spare = 0;                      // Reserved bits
    g_auth_req->message_type = 0x56;            // Authentication Request
    
    // Derived fields from Registration Request (KSI transformation)
    g_auth_req->nas_key_set_id = 0;            // Reset from 7 → 0
    g_auth_req->tsc = 0;                        // New security context
    g_auth_req->spare2 = 0;                     // Spare bits
    
    // ABBA (Anti-Bidding down Between Architectures)
    g_auth_req->abba.length = 2;                // ABBA length
    g_auth_req->abba.contents[0] = 0x00;        // ABBA contents
    g_auth_req->abba.contents[1] = 0x00;
    
    // Authentication Parameter RAND (copy from UDM vector)
    g_auth_req->rand.iei = 0x21;                // RAND element identifier
    memcpy(g_auth_req->rand.rand, g_rand, 16);
    
    // Authentication Parameter AUTN (copy from UDM vector)
    g_auth_req->autn.iei = 0x20;                // AUTN element identifier
    g_auth_req->autn.length = 16;               // AUTN length
    memcpy(g_auth_req->autn.sqn_xor_ak, g_autn, 6);      // SQN⊕AK
    memcpy(g_auth_req->autn.amf, g_autn + 6, 2);         // AMF
    memcpy(g_auth_req->autn.mac, g_autn + 8, 8);         // MAC
    
    // Set output length
    g_output_len = sizeof(AuthenticationRequest);
}

EVENT_HANDLER(amf_send_authentication_request) {
    // In phase 1, this completes the transformation
    // Output is ready in g_output_buffer with g_output_len bytes
}

// Helper functions for phase 1 transformation

void phase1_init(const uint8_t *input, size_t input_len) {
    // Copy input to global buffer
    memcpy(g_input_buffer, input, input_len);
    g_input_len = input_len;
    g_output_len = 0;
}

void phase1_execute(void) {
    // Execute the handler chain in sequence
    amf_handle_registration_request();
    amf_extract_suci_from_mobile_identity();
    amf_validate_registration_request();
    ausf_authenticate_request();
    udm_generate_authentication_vector();
    amf_build_authentication_request();
    amf_send_authentication_request();
}

void phase1_get_output(uint8_t *output, size_t *output_len) {
    // Copy output from global buffer
    memcpy(output, g_output_buffer, g_output_len);
    *output_len = g_output_len;
}

// Main entry point for phase 1 transformation
int phase1_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len) {
    phase1_init(input, input_len);
    phase1_execute();
    phase1_get_output(output, output_len);
    return 0;
}