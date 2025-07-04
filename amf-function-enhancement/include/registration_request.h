#ifndef REGISTRATION_REQUEST_H
#define REGISTRATION_REQUEST_H

#include "nas_common.h"

#pragma pack(push, 1)

// 5GS registration type
typedef struct {
    uint8_t spare : 4;                // Spare bits
    uint8_t registration_type : 3;    // Registration type (1 = initial)
    uint8_t follow_on_request : 1;    // FOR bit
} RegistrationType;

// NAS key set identifier
typedef struct {
    uint8_t spare : 4;                // Spare bits
    uint8_t tsc : 1;                  // Type of security context (0 = native, 1 = mapped)
    uint8_t nas_key_set_id : 3;       // Key set identifier value
} NasKeySetId;

// Mobile Identity - SUCI format
typedef struct {
    uint16_t length;                  // Length of mobile identity contents (big-endian)
    uint8_t type_id : 3;              // Type of identity (1 = SUCI)
    uint8_t spare1 : 1;               // Spare bit
    uint8_t supi_format : 3;          // SUPI format (0 = IMSI)
    uint8_t spare2 : 1;               // Spare bit
    
    // MCC and MNC in BCD format
    uint8_t mcc_digit2 : 4;          // MCC digit 2
    uint8_t mcc_digit1 : 4;          // MCC digit 1
    uint8_t mcc_digit3 : 4;          // MCC digit 3
    uint8_t mnc_digit3 : 4;          // MNC digit 3 (or 0xF if 2-digit MNC)
    uint8_t mnc_digit1 : 4;          // MNC digit 1
    uint8_t mnc_digit2 : 4;          // MNC digit 2
    
    // Routing indicator
    uint8_t routing_ind_digit2 : 4;   // Routing indicator digit 2
    uint8_t routing_ind_digit1 : 4;   // Routing indicator digit 1
    uint8_t routing_ind_digit4 : 4;   // Routing indicator digit 4
    uint8_t routing_ind_digit3 : 4;   // Routing indicator digit 3
    
    // Protection scheme
    uint8_t protection_scheme_id : 4; // Protection scheme (0 = null scheme)
    uint8_t spare3 : 3;              // Spare bits
    uint8_t home_network_pki : 1;    // Home network PKI (0 = not used)
    
    // MSIN (Mobile Subscriber Identification Number) in BCD
    uint8_t msin[6];                 // MSIN field is 6 bytes in this message
} MobileIdentitySuci;

// UE Security Capability
typedef struct {
    uint8_t iei;                     // IEI = 0x2E
    uint8_t length;                  // Length of security capability
    // Byte 0x80 = 10000000, bit 7 is MSB = ea0
    uint8_t ea7 : 1;                 // 5G-EA7 supported (bit 0)
    uint8_t ea6 : 1;                 // 5G-EA6 supported (bit 1)
    uint8_t ea5 : 1;                 // 5G-EA5 supported (bit 2)
    uint8_t ea4 : 1;                 // 5G-EA4 supported (bit 3)
    uint8_t ea3_128 : 1;             // 128-5G-EA3 supported (bit 4)
    uint8_t ea2_128 : 1;             // 128-5G-EA2 supported (bit 5)
    uint8_t ea1_128 : 1;             // 128-5G-EA1 supported (bit 6)
    uint8_t ea0 : 1;                 // 5G-EA0 supported (bit 7)
    
    // Byte 0xF0 = 11110000
    uint8_t ia7 : 1;                 // 5G-IA7 supported (bit 0)
    uint8_t ia6 : 1;                 // 5G-IA6 supported (bit 1)
    uint8_t ia5 : 1;                 // 5G-IA5 supported (bit 2)
    uint8_t ia4 : 1;                 // 5G-IA4 supported (bit 3)
    uint8_t ia3_128 : 1;             // 128-5G-IA3 supported (bit 4)
    uint8_t ia2_128 : 1;             // 128-5G-IA2 supported (bit 5)
    uint8_t ia1_128 : 1;             // 128-5G-IA1 supported (bit 6)
    uint8_t ia0 : 1;                 // 5G-IA0 supported (bit 7)
    
    // Byte 0x80 = 10000000
    uint8_t eea7 : 1;                // EEA7 supported (bit 0)
    uint8_t eea6 : 1;                // EEA6 supported (bit 1)
    uint8_t eea5 : 1;                // EEA5 supported (bit 2)
    uint8_t eea4 : 1;                // EEA4 supported (bit 3)
    uint8_t eea3_128 : 1;            // 128-EEA3 supported (bit 4)
    uint8_t eea2_128 : 1;            // 128-EEA2 supported (bit 5)
    uint8_t eea1_128 : 1;            // 128-EEA1 supported (bit 6)
    uint8_t eea0 : 1;                // EPS-EA0 supported (bit 7)
    
    // Byte 0xF0 = 11110000
    uint8_t eia7 : 1;                // EIA7 supported (bit 0)
    uint8_t eia6 : 1;                // EIA6 supported (bit 1)
    uint8_t eia5 : 1;                // EIA5 supported (bit 2)
    uint8_t eia4 : 1;                // EIA4 supported (bit 3)
    uint8_t eia3_128 : 1;            // 128-EIA3 supported (bit 4)
    uint8_t eia2_128 : 1;            // 128-EIA2 supported (bit 5)
    uint8_t eia1_128 : 1;            // 128-EIA1 supported (bit 6)
    uint8_t eia0 : 1;                // EPS-IA0 supported (bit 7)
} UeSecurityCapability;

// Combined registration type and ngKSI byte
// Byte format: [ngKSI half-octet][reg type half-octet]
// Based on actual hex analysis of 0x79:
// - High nibble 0x7 = 0111 = tsc(0) + key_id(111=7)
// - Low nibble 0x9 = 1001 = FOR(1) + type(001=1) when FOR is MSB
typedef struct {
    // Low half-octet
    uint8_t registration_type : 3;   // Bits 0-2: Registration type
    uint8_t follow_on_request : 1;   // Bit 3: FOR bit
    // High half-octet  
    uint8_t nas_key_set_id : 3;      // Bits 4-6: NAS key set identifier
    uint8_t tsc : 1;                 // Bit 7: Type of security context
} RegTypeAndNgksi;

// Complete Registration Request message
typedef struct {
    // Message header
    uint8_t epd;                     // Extended Protocol Discriminator (0x7E)
    uint8_t security_header : 4;     // Security header type (0 = plain)
    uint8_t spare : 4;               // Spare half octet
    uint8_t message_type;            // Message type (0x41)
    
    // Registration type and ngKSI (combined in one byte)
    RegTypeAndNgksi reg_type_ngksi;  // Combined field
    
    // Mobile identity (SUCI)
    MobileIdentitySuci mobile_identity;
    
    // UE security capability
    UeSecurityCapability security_capability;
} RegistrationRequest;

#pragma pack(pop)

#endif // REGISTRATION_REQUEST_H