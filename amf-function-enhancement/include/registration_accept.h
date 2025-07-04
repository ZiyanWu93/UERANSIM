#ifndef REGISTRATION_ACCEPT_H
#define REGISTRATION_ACCEPT_H

#include "nas_common.h"

#pragma pack(push, 1)

// 5GS registration result
typedef struct {
    uint8_t length;                  // Length = 1
    uint8_t registration_result : 3; // Registration result value
    uint8_t sms_allowed : 1;         // SMS over NAS allowed
    uint8_t nssaa_performed : 1;     // Network slice-specific auth performed
    uint8_t emergency_registered : 1; // Emergency registered
    uint8_t spare : 2;               // Spare bits
} RegistrationResult;

// 5G-GUTI (5G Globally Unique Temporary Identity)
typedef struct {
    uint8_t iei;                     // IEI = 0x77
    uint16_t length;                 // Length of GUTI contents (11) - 16-bit field
    uint8_t type_of_identity : 3;    // Type = 2 for 5G-GUTI
    uint8_t spare1 : 1;              // Spare bit
    uint8_t spare2 : 4;              // Spare bits (0xF)
    
    // PLMN (MCC + MNC) in BCD format
    uint8_t mcc_digit2 : 4;
    uint8_t mcc_digit1 : 4;
    uint8_t mcc_digit3 : 4;
    uint8_t mnc_digit3 : 4;
    uint8_t mnc_digit1 : 4;
    uint8_t mnc_digit2 : 4;
    
    // AMF identifiers
    uint8_t amf_region_id;           // AMF Region ID
    uint16_t amf_pointer : 6;        // AMF Pointer (6 bits)
    uint16_t amf_set_id : 10;        // AMF Set ID (10 bits)
    
    // 5G-TMSI
    uint32_t tmsi_5g;                // 5G Temporary Mobile Subscriber Identity
} __attribute__((packed)) Guti5G;

// TAI list entry
typedef struct {
    uint8_t type_of_list : 2;        // Type of TAI list
    uint8_t number_of_elements : 5;  // Number of elements
    uint8_t spare : 1;               // Spare bit
    
    // TAI entries (variable based on type)
    uint8_t tai_data[];              // Variable length TAI data
} TaiList;

// Allowed NSSAI
typedef struct {
    uint8_t iei;                     // IEI = 0x15
    uint8_t length;                  // Length of NSSAI contents
    uint8_t nssai_data[];            // S-NSSAI entries (variable)
} AllowedNssai;

// 5GS network feature support
typedef struct {
    uint8_t iei;                     // IEI = 0x21
    uint8_t length;                  // Length = 2
    uint8_t mpsi : 1;                // MPSI indicator
    uint8_t iwk_n26 : 1;             // N26 interface for interworking
    uint8_t emf : 1;                 // Emergency fallback
    uint8_t emc : 1;                 // Emergency services
    uint8_t ims_vops_n3gpp : 1;      // IMS voice over PS via non-3GPP
    uint8_t ims_vops_3gpp : 1;       // IMS voice over PS via 3GPP
    uint8_t mcsi : 1;                // MCSI
    uint8_t emcn3 : 1;               // Emergency services via non-3GPP
    
    uint8_t spare : 8;               // Second octet (all spare)
} NetworkFeatureSupport;

// Complete Registration Accept message (fixed part only)
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4; // Security header type (0 = plain)
    uint8_t inner_spare : 4;         // Spare half octet
    uint8_t message_type;            // Message type (0x42)
    
    // 5GS registration result
    RegistrationResult reg_result;
    
    // Variable IEs follow (GUTI, TAI list, etc.)
} RegistrationAccept;

#pragma pack(pop)

#endif // REGISTRATION_ACCEPT_H