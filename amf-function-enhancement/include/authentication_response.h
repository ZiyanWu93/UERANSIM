#ifndef AUTHENTICATION_RESPONSE_H
#define AUTHENTICATION_RESPONSE_H

#include "nas_common.h"

#pragma pack(push, 1)

// Authentication Response Parameter (RES*)
typedef struct {
    uint8_t iei;                     // IEI = 0x2D
    uint8_t length;                  // Length of RES* (16)
    uint8_t res_star[16];            // RES* value (16 bytes)
} AuthResponseParameter;

// Complete Authentication Response message
typedef struct {
    // Message header
    uint8_t epd;                     // Extended Protocol Discriminator (0x7E)
    uint8_t security_header : 4;     // Security header type (0 = plain)
    uint8_t spare : 4;               // Spare half octet
    uint8_t message_type;            // Message type (0x57)
    
    // Authentication response parameter
    AuthResponseParameter auth_resp;
} AuthenticationResponse;

#pragma pack(pop)

#endif // AUTHENTICATION_RESPONSE_H