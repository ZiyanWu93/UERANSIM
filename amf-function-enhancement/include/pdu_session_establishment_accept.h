#ifndef PDU_SESSION_ESTABLISHMENT_ACCEPT_H
#define PDU_SESSION_ESTABLISHMENT_ACCEPT_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete PDU Session Establishment Accept message structure - flat structure
typedef struct {
    // Extended protocol discriminator
    uint8_t epd;                        // Extended Protocol Discriminator (0x2E for 5GSM)
    
    // PDU session identity
    uint8_t pdu_session_id;             // PDU session identity
    
    // Procedure transaction identity
    uint8_t pti;                        // Procedure transaction identity
    
    // Session management message type
    uint8_t message_type;               // Message type (0xC2)
    
    // Selected PDU session type and SSC mode (combined byte)
    uint8_t type_and_ssc;               // Bits 5-7: SSC mode, Bits 1-3: PDU type
    
    // Authorized QoS rules (mandatory - no IEI)
    uint16_t qos_rules_length;          // Length in network byte order
    
    // QoS rule 1 (hardcoded)
    uint8_t qos_rule_id;                // 0x01
    uint16_t qos_rule_length;           // 0x0006 in network byte order
    uint8_t rule_operation_code;        // 0x31 (create new, default, 1 filter)
    uint8_t packet_filter_list[3];      // 0x31 0x01 0x01 (filter)
    uint8_t qos_rule_precedence;        // 0xFF
    uint8_t qfi;                        // 0x01
    
    // Session AMBR (mandatory - no IEI)
    uint8_t ambr_length;                // 0x06
    uint8_t dl_unit;                    // 0x03 (Mbps)
    uint16_t dl_rate;                   // 0xF424 (62500 in network byte order)
    uint8_t ul_unit;                    // 0x03 (Mbps)
    uint16_t ul_rate;                   // 0xF424 (62500 in network byte order)
    
    // PDU address
    uint8_t pdu_addr_iei;               // 0x29
    uint8_t pdu_addr_length;            // 0x05
    uint8_t pdu_addr_type;              // 0x01 (IPv4)
    uint8_t ipv4_addr[4];               // IPv4 address
    
    // S-NSSAI
    uint8_t snssai_iei;                 // 0x22
    uint8_t snssai_length;              // 0x01
    uint8_t sst;                        // Slice/Service Type
    
    // QoS flow descriptions
    uint8_t qos_flow_iei;               // 0x79
    uint16_t qos_flow_length;           // 0x0006 in network byte order
    uint8_t qos_flow_data[6];           // QFI + operation + parameters
    
    // Extended PCO
    uint8_t pco_iei;                    // 0x7B
    uint16_t pco_length;                // 0x000F in network byte order
    uint8_t pco_data[15];               // PCO content
    
    // DNN
    uint8_t dnn_iei;                    // 0x25
    uint8_t dnn_length;                 // 0x09
    uint8_t dnn_value[9];               // "internet" in DNN format
} PduSessionEstablishmentAccept;

#pragma pack(pop)

#endif // PDU_SESSION_ESTABLISHMENT_ACCEPT_H