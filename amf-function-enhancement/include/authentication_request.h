#ifndef AUTHENTICATION_REQUEST_H
#define AUTHENTICATION_REQUEST_H

#include "nas_common.h"

#pragma pack(push, 1)

// ABBA (Anti-Bidding down Between Architectures) parameter
typedef struct {
    uint8_t length;                  // Length of ABBA contents
    uint8_t contents[2];             // ABBA contents (usually 00:00)
} AbbaParameter;

// RAND (Random challenge) parameter
typedef struct {
    uint8_t iei;                     // IEI = 0x21
    uint8_t rand[16];                // 16-byte random challenge
} RandParameter;

// AUTN (Authentication Token) parameter
typedef struct {
    uint8_t iei;                     // IEI = 0x20
    uint8_t length;                  // Length = 16
    uint8_t sqn_xor_ak[6];          // SQN ⊕ AK (Sequence Number XOR Anonymity Key)
    uint8_t amf[2];                  // Authentication Management Field
    uint8_t mac[8];                  // Message Authentication Code
} AutnParameter;

// Complete Authentication Request message
typedef struct {
    // Message header
    uint8_t epd;                     // Extended Protocol Discriminator (0x7E)
    uint8_t security_header : 4;     // Security header type (0 = plain)
    uint8_t spare : 4;               // Spare half octet
    uint8_t message_type;            // Message type (0x56)
    
    // NAS key set identifier
    uint8_t nas_key_set_id : 3;      // Key set identifier value (0)
    uint8_t tsc : 1;                 // Type of security context (0)
    uint8_t spare2 : 4;              // Spare bits
    
    // ABBA
    AbbaParameter abba;
    
    // Authentication parameters
    RandParameter rand;
    AutnParameter autn;
} AuthenticationRequest;

#pragma pack(pop)

#endif // AUTHENTICATION_REQUEST_H