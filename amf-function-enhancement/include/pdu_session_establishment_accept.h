#ifndef PDU_SESSION_ESTABLISHMENT_ACCEPT_H
#define PDU_SESSION_ESTABLISHMENT_ACCEPT_H

#include "nas_common.h"

#pragma pack(push, 1)

// PDU address
typedef struct {
    uint8_t iei;                        // IEI = 0x29
    uint8_t length;                     // Length of PDU address contents
    uint8_t pdu_session_type_value : 3; // PDU session type
    uint8_t spare : 5;                  // Spare bits
    union {
        uint32_t ipv4_address;          // IPv4 address (4 bytes)
        uint8_t ipv6_interface_id[8];   // IPv6 interface identifier
        struct {
            uint32_t ipv4_address;
            uint8_t ipv6_interface_id[8];
        } ipv4v6;
    } address;
} PduAddress;

// Authorized QoS rules
typedef struct {
    uint8_t iei;                        // IEI = 0x7A
    uint16_t length;                    // Length of QoS rules (big endian)
    uint8_t qos_rules[];                // QoS rule data (variable)
} AuthorizedQosRules;

// Session AMBR (Aggregate Maximum Bit Rate)
typedef struct {
    uint8_t iei;                        // IEI = 0x2A
    uint8_t length;                     // Length of session AMBR
    uint8_t unit_for_session_ambr_dl;   // Unit for downlink
    uint16_t session_ambr_dl;           // Downlink rate (big endian)
    uint8_t unit_for_session_ambr_ul;   // Unit for uplink
    uint16_t session_ambr_ul;           // Uplink rate (big endian)
} SessionAmbr;

// Complete PDU Session Establishment Accept message
typedef struct {
    // Extended protocol discriminator
    uint8_t epd;                        // Extended Protocol Discriminator (0x2E for 5GSM)
    
    // PDU session identity
    uint8_t pdu_session_id;             // PDU session identity
    
    // Procedure transaction identity
    uint8_t pti;                        // Procedure transaction identity
    
    // Session management message type
    uint8_t message_type;               // Message type (0xC2)
    
    // Selected PDU session type and SSC mode
    uint8_t selected_ssc_mode : 3;      // Selected SSC mode
    uint8_t spare1 : 1;                 // Spare bit
    uint8_t selected_pdu_session_type : 3; // Selected PDU session type
    uint8_t spare2 : 1;                 // Spare bit
    
    // Authorized QoS rules (mandatory)
    AuthorizedQosRules authorized_qos_rules;
    
    // Session AMBR (mandatory)
    // Note: Position depends on QoS rules length
    
    // Additional IEs follow
} PduSessionEstablishmentAccept;

#pragma pack(pop)

#endif // PDU_SESSION_ESTABLISHMENT_ACCEPT_H