#ifndef DL_NAS_TRANSPORT_H
#define DL_NAS_TRANSPORT_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete DL NAS Transport message - flat structure
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
    uint8_t message_type;               // Message type (0x68)
    
    // Payload container type (inline)
    uint8_t payload_container_type : 4; // Container type (1 = N1 SM info)
    uint8_t spare_bits : 4;             // Spare bits
    
    // Payload container
    uint8_t payload_length_high;        // Length high byte
    uint8_t payload_length_low;         // Length low byte
    
    // PDU Session Establishment Accept payload (71 bytes fixed)
    uint8_t payload[71];                // Complete PDU Session Est Accept
    
    // PDU session ID 2
    uint8_t pdu_session_id2_iei;        // IEI = 0x12
    uint8_t pdu_session_id2_value;      // PDU session identity value
} DlNasTransportComplete;

#pragma pack(pop)

#endif // DL_NAS_TRANSPORT_H