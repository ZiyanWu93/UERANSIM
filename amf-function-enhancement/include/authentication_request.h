#ifndef AUTHENTICATION_REQUEST_H
#define AUTHENTICATION_REQUEST_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete Authentication Request message - flat structure
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
    
    // ABBA (Anti-Bidding down Between Architectures)
    uint8_t abba_length;             // Length of ABBA contents
    uint8_t abba_contents[2];        // ABBA contents (usually 00:00)
    
    // RAND (Random challenge)
    uint8_t rand_iei;                // IEI = 0x21
    uint8_t rand[16];                // 16-byte random challenge
    
    // AUTN (Authentication Token)
    uint8_t autn_iei;                // IEI = 0x20
    uint8_t autn_length;             // Length = 16
    uint8_t autn_sqn_xor_ak[6];      // SQN ⊕ AK (Sequence Number XOR Anonymity Key)
    uint8_t autn_amf[2];             // Authentication Management Field
    uint8_t autn_mac[8];             // Message Authentication Code
} AuthenticationRequest;

#pragma pack(pop)

#endif // AUTHENTICATION_REQUEST_H