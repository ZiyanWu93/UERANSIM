#ifndef PDU_SESSION_ESTABLISHMENT_REQUEST_H
#define PDU_SESSION_ESTABLISHMENT_REQUEST_H

#include "nas_common.h"

#pragma pack(push, 1)

// PDU session type
typedef struct {
    uint8_t pdu_session_type_value : 3; // PDU session type (1=IPv4, 2=IPv6, 3=IPv4v6)
    uint8_t spare : 5;                  // Spare bits
} PduSessionType;

// SSC mode
typedef struct {
    uint8_t ssc_mode_value : 3;         // SSC mode (1, 2, or 3)
    uint8_t spare : 5;                  // Spare bits
} SscMode;

// 5GSM capability
typedef struct {
    uint8_t iei;                        // IEI = 0x28
    uint8_t length;                     // Length of capability
    uint8_t rqos : 1;                   // Reflective QoS supported
    uint8_t mh6_pdu : 1;                // Multi-homed IPv6 PDU session
    uint8_t spare : 6;                  // Spare bits
} Capability5GSM;

// Complete PDU Session Establishment Request message
typedef struct {
    // Extended protocol discriminator
    uint8_t epd;                        // Extended Protocol Discriminator (0x2E for 5GSM)
    
    // PDU session identity
    uint8_t pdu_session_id;             // PDU session identity (1-15)
    
    // Procedure transaction identity
    uint8_t pti;                        // Procedure transaction identity
    
    // Session management message type
    uint8_t message_type;               // Message type (0xC1)
    
    // Integrity protection maximum data rate
    uint8_t integrity_protection_max_data_rate_ul;  // Uplink
    uint8_t integrity_protection_max_data_rate_dl;  // Downlink
    
    // Optional IEs
    uint8_t pdu_session_type_iei : 4;   // IEI = 0x9
    uint8_t spare1 : 4;                 // Spare half octet
    PduSessionType pdu_session_type;
    
    // Additional optional IEs may follow
} PduSessionEstablishmentRequest;

#pragma pack(pop)

#endif // PDU_SESSION_ESTABLISHMENT_REQUEST_H