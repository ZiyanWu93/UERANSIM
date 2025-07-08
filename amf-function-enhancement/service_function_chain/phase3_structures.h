#ifndef PHASE3_STRUCTURES_H
#define PHASE3_STRUCTURES_H

#include <stdint.h>

#pragma pack(push, 1)

// Complete Registration Accept as it appears in the binary
typedef struct {
    // Security header
    uint8_t epd;                        // 0x7E
    uint8_t security_header_type;       // 0x02
    uint8_t mac[4];                     // 0x72396C74
    uint8_t sequence_number;            // 0x01
    
    // Inner message
    uint8_t inner_epd;                  // 0x7E
    uint8_t inner_security_header;      // 0x00
    uint8_t message_type;               // 0x42
    
    // 5GS registration result
    uint8_t reg_result_length;          // 0x01
    uint8_t reg_result_value;           // 0x01
    
    // 5G-GUTI
    uint8_t guti_iei;                   // 0x77
    uint8_t guti_spare_half;            // 0x00
    uint8_t guti_length;                // 0x0B
    uint8_t guti_type_and_spare;        // 0xF2
    uint8_t guti_plmn[3];               // 0x99F907
    uint8_t amf_region_id;              // 0x02
    uint8_t amf_set_and_ptr[2];         // 0x0040
    uint8_t tmsi_5g[4];                 // 0xC0000727
    
    // TAI list
    uint8_t tai_list_iei;               // 0x54
    uint8_t tai_list_length;            // 0x07
    uint8_t tai_list_type;              // 0x40
    uint8_t tai_plmn[3];                // 0x99F907
    uint8_t tac[3];                     // 0x000001 (3 bytes!)
    
    // Allowed NSSAI
    uint8_t nssai_iei;                  // 0x15
    uint8_t nssai_length;               // 0x02
    uint8_t s_nssai_length;             // 0x01
    uint8_t sst;                        // 0x01
    
    // Network feature support
    uint8_t nw_feat_iei;                // 0x21
    uint8_t nw_feat_length;             // 0x02
    uint8_t nw_feat_byte1;              // 0x01
    uint8_t nw_feat_byte2;              // 0x00
    
    // T3512 timer
    uint8_t t3512_iei;                  // 0x5E
    uint8_t t3512_length;               // 0x01
    uint8_t t3512_value;                // 0x92
} RegistrationAcceptBinary;

#pragma pack(pop)

#endif // PHASE3_STRUCTURES_H