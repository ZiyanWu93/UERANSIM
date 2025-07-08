#ifndef SECURITY_MODE_COMMAND_H
#define SECURITY_MODE_COMMAND_H

#include "nas_common.h"
#include "registration_request.h"  // For UeSecurityCapability

#pragma pack(push, 1)

// Selected NAS security algorithms
typedef struct {
    uint8_t integrity_algo : 4;      // Selected integrity algorithm
    uint8_t ciphering_algo : 4;      // Selected ciphering algorithm
} SelectedAlgorithms;

// Additional 5G security information
typedef struct {
    uint8_t iei;                     // IEI = 0x36
    uint8_t length;                  // Length = 1
    uint8_t rinmr : 1;               // Retransmission of initial NAS message required
    uint8_t hdp : 1;                 // Horizontal derivation parameter
    uint8_t spare : 6;               // Spare bits
} Additional5GSecurityInfo;

// Complete Security Mode Command message
// Based on ROADMAP-phase2.md actual structure
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header;   // Security header type (0 = plain) with spare
    uint8_t message_type;            // Message type (0x5D)
    
    // Selected NAS security algorithms - actual binary shows different layout
    uint8_t selected_algo_byte1;     // Contains integrity algorithm
    uint8_t selected_algo_byte2;     // Contains ciphering algorithm
    
    // UE security capability length
    uint8_t ue_capability_length;    // Length of security capability (0x04)
    
    // UE security capabilities (4 bytes)
    uint8_t ea_byte1;                // 5G EA algorithms (0x80)
    uint8_t ia_byte1;                // 5G IA algorithms (0xF0)
    uint8_t ea_byte2;                // EPS EA algorithms (0x80)
    uint8_t ia_byte2;                // EPS IA algorithms (0xF0)
    
    // IMEISV request (Type 1 IE) - combined byte
    uint8_t imeisv_request_combined; // IEI in upper nibble, value in lower
    
    // Additional 5G security information
    uint8_t additional_sec_info_iei; // IEI = 0x36
    uint8_t additional_sec_info_len; // Length = 0x01
    uint8_t additional_sec_info_val; // Value byte (RINMR bit, etc)
} SecurityModeCommand;

#pragma pack(pop)

#endif // SECURITY_MODE_COMMAND_H