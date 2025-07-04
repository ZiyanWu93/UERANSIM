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
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4; // Security header type (0 = plain)
    uint8_t inner_spare : 4;         // Spare half octet
    uint8_t message_type;            // Message type (0x5D)
    
    // Selected algorithms
    SelectedAlgorithms algorithms;
    
    // NAS key set identifier
    uint8_t nas_key_set_id : 3;      // Key set identifier
    uint8_t tsc : 1;                 // Type of security context
    uint8_t spare2 : 4;              // Spare bits
    
    // Replayed UE security capability (without IEI, mandatory field)
    uint8_t ue_capability_length;    // Length of security capability
    uint8_t ea_byte1;                // EA algorithms byte 1
    uint8_t ia_byte1;                // IA algorithms byte 1
    uint8_t ea_byte2;                // EA algorithms byte 2 
    uint8_t ia_byte2;                // IA algorithms byte 2
    
    // Optional IEs
    uint8_t imeisv_request_iei : 4;  // IEI = 0xE
    uint8_t imeisv_request_value : 3; // IMEISV request value
    uint8_t imeisv_request_spare : 1; // Spare bit
    
    // Additional 5G security information
    Additional5GSecurityInfo additional_security_info;
} SecurityModeCommand;

#pragma pack(pop)

#endif // SECURITY_MODE_COMMAND_H