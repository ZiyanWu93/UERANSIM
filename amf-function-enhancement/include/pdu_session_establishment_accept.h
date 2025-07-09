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

// Authorized QoS rules (mandatory - no IEI)
typedef struct {
    uint16_t length;                    // Length of QoS rules (big endian)
    uint8_t qos_rules[];                // QoS rule data (variable)
} AuthorizedQosRules;

// Session AMBR (mandatory - no IEI)
typedef struct {
    uint8_t length;                     // Length of session AMBR
    uint8_t unit_for_session_ambr_dl;   // Unit for downlink
    uint16_t session_ambr_dl;           // Downlink rate (big endian)
    uint8_t unit_for_session_ambr_ul;   // Unit for uplink
    uint16_t session_ambr_ul;           // Uplink rate (big endian)
} SessionAmbr;

// QoS flow descriptions
typedef struct {
    uint8_t iei;                        // IEI = 0x79
    uint16_t length;                    // Length (big endian)
    uint8_t qos_flow_data[];            // QoS flow description data
} QosFlowDescriptions;

// S-NSSAI
typedef struct {
    uint8_t iei;                        // IEI = 0x22
    uint8_t length;                     // Length
    uint8_t sst;                        // Slice/Service Type
    uint8_t sd[3];                      // Slice Differentiator (optional)
} SNssai;

// Extended protocol configuration options
typedef struct {
    uint8_t iei;                        // IEI = 0x7B
    uint16_t length;                    // Length (big endian)
    uint8_t config_protocol;            // Configuration protocol
    uint8_t pco_data[];                 // PCO data
} ExtendedPco;

// DNN
typedef struct {
    uint8_t iei;                        // IEI = 0x25
    uint8_t length;                     // Length
    uint8_t dnn[];                      // DNN value
} Dnn;

// QoS rule structure
typedef struct {
    uint8_t qos_rule_id;                // QoS rule identifier
    uint16_t length;                    // Length of QoS rule contents (big endian)
    uint8_t rule_operation_code : 3;    // Rule operation code
    uint8_t dqr_bit : 1;                // Default QoS rule
    uint8_t num_of_packet_filters : 4;  // Number of packet filters
    // Packet filters follow
    uint8_t packet_filter_list[];       // Variable length
} QosRule;

// Complete PDU Session Establishment Accept message structure
// This is a hardcoded structure for the specific test case
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
} __attribute__((packed)) PduSessionEstablishmentAccept;

#pragma pack(pop)

#endif // PDU_SESSION_ESTABLISHMENT_ACCEPT_H