#ifndef REGISTRATION_COMPLETE_H
#define REGISTRATION_COMPLETE_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete Registration Complete message
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4; // Security header type (0 = plain)
    uint8_t inner_spare : 4;         // Spare half octet
    uint8_t message_type;            // Message type (0x43)
    
    // No mandatory IEs for Registration Complete
    // Optional IEs may follow
} RegistrationComplete;

#pragma pack(pop)

#endif // REGISTRATION_COMPLETE_H