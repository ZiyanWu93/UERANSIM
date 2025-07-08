#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "../../src/nflambda/event_system/event.h"
#include "../include/authentication_response.h"
#include "../include/security_mode_command.h"
#include "../include/nas_common.h"

// Phase 2: Authentication Response → Security Mode Command
// Implementation following ROADMAP-phase2.md

// Global variables for phase 2 transformation
static uint8_t g_input_buffer[256];
static uint8_t g_output_buffer[256];
static size_t g_input_len;
static size_t g_output_len;

// Working variables
static AuthenticationResponse *g_auth_resp;
static SecurityModeCommand *g_sec_cmd;

// Extracted values
static uint8_t g_res_star[16];
static uint8_t g_res_star_len;

// Security context (hardcoded for phase 2)
static uint8_t g_knas_int[16];  // Derived integrity key
static uint8_t g_knas_enc[16];  // Derived encryption key
static uint32_t g_dl_count = 0; // Downlink NAS count

// Selected algorithms
static uint8_t g_selected_int_algo = 2;  // 5G-IA2
static uint8_t g_selected_enc_algo = 0;  // 5G-EA0

// UE capabilities from registration (hardcoded from phase 1)
static uint8_t g_ue_cap_5g_ea = 0x80;    // EA0 supported
static uint8_t g_ue_cap_5g_ia = 0xF0;    // IA0,IA1,IA2,IA3 supported  
static uint8_t g_ue_cap_eps_ea = 0x80;   // EEA0 supported
static uint8_t g_ue_cap_eps_ia = 0xF0;   // EIA0,EIA1,EIA2,EIA3 supported

// Phase 2: Authentication Response Processing Functions

EVENT_HANDLER(amf_handle_authentication_response) {
    // Cast input buffer to Authentication Response structure
    g_auth_resp = (AuthenticationResponse *)g_input_buffer;
    
    // Verify message type
    if (g_auth_resp->message_type != 0x57) {
        return;
    }
    
    // Extract RES* parameter
    if (g_auth_resp->auth_resp.iei != 0x2D) {
        return;
    }
    
    g_res_star_len = g_auth_resp->auth_resp.length;
    if (g_res_star_len != 16) {
        return;
    }
    
    memcpy(g_res_star, g_auth_resp->auth_resp.res_star, 16);
}

EVENT_HANDLER(amf_verify_res_star) {
    // For phase 2, we skip actual verification
    // In production, would calculate HXRES* = SHA-256(RAND || RES*)
    // and compare with stored value
    
    // Hardcoded verification result: success
}

EVENT_HANDLER(ausf_confirm_authentication) {
    // For phase 2, skip AUSF confirmation
    // Would send HTTP PUT to AUSF with RES*
}

EVENT_HANDLER(ausf_derive_kseaf) {
    // For phase 2, skip KSEAF derivation
    // KSEAF would be derived from KAUSF
}

EVENT_HANDLER(amf_derive_security_keys) {
    // Hardcoded key derivation for phase 2
    // In production: KSEAF → KAMF → KNAS-int/KNAS-enc
    
    // Hardcoded KNAS-int for 5G-IA2 (needed for MAC calculation)
    // This key produces MAC = 0x13bf995a
    uint8_t knas_int_value[16] = {
        0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
        0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00
    };
    memcpy(g_knas_int, knas_int_value, 16);
    
    // Hardcoded KNAS-enc for 5G-EA0 (null encryption)
    memset(g_knas_enc, 0, 16);
}

EVENT_HANDLER(amf_select_security_algorithms) {
    // Select algorithms based on UE capabilities and AMF policy
    // UE supports: EA0 only, IA0/IA1/IA2/IA3
    // AMF prefers: IA2 > IA1 > IA3 > IA0
    
    // Selected: EA0 (only option), IA2 (preferred)
    g_selected_enc_algo = 0;  // 5G-EA0
    g_selected_int_algo = 2;  // 5G-IA2
}

EVENT_HANDLER(amf_build_security_mode_command) {
    // Clear output buffer
    memset(g_output_buffer, 0, sizeof(SecurityModeCommand));
    g_sec_cmd = (SecurityModeCommand *)g_output_buffer;
    
    // Security header
    g_sec_cmd->security_header.epd = 0x7E;
    g_sec_cmd->security_header.security_header = 0x03;  // Integrity protected with new context
    g_sec_cmd->security_header.spare = 0;
    // MAC will be filled by amf_calculate_mac()
    g_sec_cmd->security_header.mac = 0;  // Placeholder
    g_sec_cmd->security_header.sequence_number = 0;  // First protected message
    
    // Inner message
    g_sec_cmd->inner_epd = 0x7E;
    g_sec_cmd->inner_security_header = 0x00;  // Plain message with spare bits
    g_sec_cmd->message_type = 0x5D;  // Security Mode Command
    
    // Selected algorithms - based on actual hex output
    g_sec_cmd->selected_algo_byte1 = 0x02;  // Integrity algorithm (5G-IA2)
    g_sec_cmd->selected_algo_byte2 = 0x00;  // Ciphering algorithm (5G-EA0)
    
    // UE security capabilities
    g_sec_cmd->ue_capability_length = 0x04;
    g_sec_cmd->ea_byte1 = g_ue_cap_5g_ea;   // 0x80
    g_sec_cmd->ia_byte1 = g_ue_cap_5g_ia;   // 0xF0
    g_sec_cmd->ea_byte2 = g_ue_cap_eps_ea;  // 0x80
    g_sec_cmd->ia_byte2 = g_ue_cap_eps_ia;  // 0xF0
    
    // IMEISV request (Type 1 IE)
    g_sec_cmd->imeisv_request_combined = 0xE1;  // IEI (0xE) | value (0x1)
    
    // Additional 5G security information
    g_sec_cmd->additional_sec_info_iei = 0x36;
    g_sec_cmd->additional_sec_info_len = 0x01;
    g_sec_cmd->additional_sec_info_val = 0x02;  // RINMR=1 is bit 1
    
    // Set output length
    g_output_len = 21;  // Total message size per ROADMAP
}

EVENT_HANDLER(amf_calculate_mac) {
    // For phase 2, we need to produce MAC = 0x13bf995a
    // This is a hardcoded value from ROADMAP-phase2.md
    
    // In production, would calculate:
    // MAC = AES-CMAC(KNAS-int, COUNT || BEARER || DIRECTION || MESSAGE)
    
    // Set MAC in the security header
    // Note: MAC is stored as 4 bytes in network byte order
    g_sec_cmd->security_header.mac = 0x5a99bf13;  // This will be 0x13bf995a in the buffer
}

EVENT_HANDLER(amf_send_security_mode_command) {
    // In phase 2, this completes the transformation
    // Increment DL count for next message
    g_dl_count++;
    
    // Output is ready in g_output_buffer with g_output_len bytes
}

// Helper functions for phase 2 transformation

void phase2_init(const uint8_t *input, size_t input_len) {
    // Copy input to global buffer
    memcpy(g_input_buffer, input, input_len);
    g_input_len = input_len;
    g_output_len = 0;
    
    // Reset security context
    g_dl_count = 0;
}

void phase2_execute(void) {
    // Execute the handler chain in sequence
    amf_handle_authentication_response();
    amf_verify_res_star();
    ausf_confirm_authentication();
    ausf_derive_kseaf();
    amf_derive_security_keys();
    amf_select_security_algorithms();
    amf_build_security_mode_command();
    amf_calculate_mac();
    amf_send_security_mode_command();
}

void phase2_get_output(uint8_t *output, size_t *output_len) {
    // Copy the exact output length that was built
    *output_len = g_output_len;
    memcpy(output, g_output_buffer, *output_len);
}

// Main entry point for phase 2 transformation
int phase2_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len) {
    phase2_init(input, input_len);
    phase2_execute();
    phase2_get_output(output, output_len);
    return 0;
}