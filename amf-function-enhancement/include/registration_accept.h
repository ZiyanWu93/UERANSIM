#ifndef REGISTRATION_ACCEPT_H
#define REGISTRATION_ACCEPT_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete Registration Accept message - flat structure
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
    uint8_t message_type;               // Message type (0x42)
    
    // 5GS registration result
    uint8_t reg_result_length;          // Length = 1
    uint8_t reg_result_value : 3;       // Registration result value
    uint8_t sms_allowed : 1;            // SMS over NAS allowed
    uint8_t nssaa_performed : 1;        // Network slice-specific auth performed
    uint8_t emergency_registered : 1;   // Emergency registered
    uint8_t reg_result_spare : 2;       // Spare bits
    
    // 5G-GUTI
    uint8_t guti_iei;                   // IEI = 0x77
    uint8_t guti_spare_half_octet;      // Spare half octet (0x00)
    uint8_t guti_length;                // Length of GUTI contents (0x0B = 11)
    uint8_t guti_type : 3;              // Type = 2 for 5G-GUTI
    uint8_t guti_spare1 : 1;            // Spare bit (0)
    uint8_t guti_spare2 : 4;            // Spare bits (0xF)
    
    // GUTI PLMN (MCC + MNC) in BCD format
    uint8_t mcc_digit2 : 4;
    uint8_t mcc_digit1 : 4;
    uint8_t mcc_digit3 : 4;
    uint8_t mnc_digit3 : 4;
    uint8_t mnc_digit1 : 4;
    uint8_t mnc_digit2 : 4;
    
    // AMF identifiers
    uint8_t amf_region_id;              // AMF Region ID
    uint8_t amf_bytes[2];               // AMF Set ID (10 bits) + AMF Pointer (6 bits)
    
    // 5G-TMSI
    uint32_t tmsi_5g;                   // 5G Temporary Mobile Subscriber Identity
    
    // TAI list
    uint8_t tai_list_iei;               // IEI = 0x54
    uint8_t tai_list_length;            // Length (7 bytes for phase 3)
    uint8_t tai_list_type;              // Type and number of elements
    uint8_t tai_plmn[3];                // TAI PLMN
    uint8_t tai_tac[3];                 // TAC (3 bytes)
    
    // Allowed NSSAI
    uint8_t nssai_iei;                  // IEI = 0x15
    uint8_t nssai_length;               // Length (2 bytes for phase 3)
    uint8_t s_nssai_length;             // S-NSSAI length (1 byte)
    uint8_t sst;                        // SST value
    
    // Network feature support
    uint8_t net_feat_iei;               // IEI = 0x21
    uint8_t net_feat_length;            // Length = 2
    uint8_t emcn3 : 1;                  // Emergency services via non-3GPP (bit 0)
    uint8_t mcsi : 1;                   // MCSI (bit 1)
    uint8_t ims_vops_3gpp : 1;          // IMS voice over PS via 3GPP (bit 2)
    uint8_t ims_vops_n3gpp : 1;         // IMS voice over PS via non-3GPP (bit 3)
    uint8_t emc : 1;                    // Emergency services (bit 4)
    uint8_t emf : 1;                    // Emergency fallback (bit 5)
    uint8_t iwk_n26 : 1;                // N26 interface for interworking (bit 6)
    uint8_t mpsi : 1;                   // MPSI indicator (bit 7)
    uint8_t net_feat_spare;             // Second octet (all spare)
    
    // T3512 timer
    uint8_t t3512_iei;                  // IEI = 0x5E
    uint8_t t3512_length;               // Length = 1
    uint8_t t3512_value : 5;            // Timer value (lower 5 bits)
    uint8_t t3512_unit : 3;             // Timer unit (upper 3 bits)
} RegistrationAcceptComplete;

#pragma pack(pop)

#endif // REGISTRATION_ACCEPT_H