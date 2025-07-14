#ifndef SECURITY_MODE_COMMAND_H
#define SECURITY_MODE_COMMAND_H

#include "nas_common.h"
#include "registration_request.h"  // For UeSecurityCapability

#pragma pack(push, 1)

// Complete Security Mode Command message - flat structure
typedef struct {
    // Security header
    uint8_t epd;                        // Extended Protocol Discriminator (0x7E)
    uint8_t security_header_type : 4;   // Security header type
    uint8_t spare_half : 4;             // Spare half octet
    uint32_t mac;                       // Message Authentication Code
    uint8_t sequence_number;            // Sequence number
    
    // Inner message
    uint8_t inner_epd;                  // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4;  // Security header type (0 = plain)
    uint8_t inner_spare : 4;            // Spare half octet
    uint8_t message_type;               // Message type (0x5D)
    
    // Selected NAS security algorithms
    uint8_t selected_integrity_algo;    // Selected integrity algorithm (0x02 = 5G-IA2)
    uint8_t selected_ciphering_algo;    // Selected ciphering algorithm (0x00 = 5G-EA0)
    
    // UE security capabilities
    uint8_t ue_capability_length;       // Length of security capability (0x04)
    uint8_t ue_5g_ea;                   // 5G EA algorithms (0x80)
    uint8_t ue_5g_ia;                   // 5G IA algorithms (0xF0)
    uint8_t ue_eps_ea;                  // EPS EA algorithms (0x80)
    uint8_t ue_eps_ia;                  // EPS IA algorithms (0xF0)
    
    // IMEISV request (Type 1 IE) - combined byte
    uint8_t imeisv_request_combined;    // IEI in upper nibble, value in lower nibble
    
    // Additional 5G security information
    uint8_t additional_sec_info_iei;    // IEI = 0x36
    uint8_t additional_sec_info_len;    // Length = 0x01
    uint8_t additional_sec_info_val;    // Value byte (RINMR, HDP, etc.)
} SecurityModeCommand;

#pragma pack(pop)

#endif // SECURITY_MODE_COMMAND_H