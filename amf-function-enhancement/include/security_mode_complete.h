#ifndef SECURITY_MODE_COMPLETE_H
#define SECURITY_MODE_COMPLETE_H

#include "nas_common.h"

#pragma pack(push, 1)

// IMEISV (International Mobile Equipment Identity and Software Version)
typedef struct {
    uint8_t iei;                     // IEI = 0x77
    uint8_t length;                  // Length of IMEISV contents
    uint8_t type_of_identity : 3;    // Type = 3 for IMEISV
    uint8_t odd_even : 1;            // Odd/even indication
    uint8_t spare : 4;               // Spare bits
    uint8_t identity[8];             // IMEISV digits in BCD format (16 digits)
} Imeisv;

// NAS message container
typedef struct {
    uint8_t iei;                     // IEI = 0x71
    uint8_t length[2];               // Length of container (2 bytes, big endian)
    uint8_t nas_message[];           // Replayed NAS message (variable length)
} NasMessageContainer;

// Complete Security Mode Complete message
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4; // Security header type (0 = plain)
    uint8_t inner_spare : 4;         // Spare half octet
    uint8_t message_type;            // Message type (0x5E)
    
    // Optional IEs (IMEISV and NAS message container)
    // Note: These are variable position, so we'll handle them separately
} SecurityModeComplete;

#pragma pack(pop)

#endif // SECURITY_MODE_COMPLETE_H