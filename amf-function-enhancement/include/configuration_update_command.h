#ifndef CONFIGURATION_UPDATE_COMMAND_H
#define CONFIGURATION_UPDATE_COMMAND_H

#include "nas_common.h"

#pragma pack(push, 1)

// Complete Configuration Update Command message - flat structure
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
    uint8_t message_type;               // Message type (0x54)
    
    // Full Network Name
    uint8_t full_name_iei;              // IEI = 0x43
    uint8_t full_name_length;           // Length of name
    uint8_t full_name_header;           // Header byte: ext=1, coding=01, add_ci=0, spare=0000
    uint8_t full_name_text[14];         // UTF-16 encoded "Open5GS"
    
    // Short Network Name
    uint8_t short_name_iei;             // IEI = 0x45
    uint8_t short_name_length;          // Length of name
    uint8_t short_name_header;          // Header byte: ext=1, coding=01, add_ci=0, spare=0000
    uint8_t short_name_text[8];         // UTF-16 encoded "Next"
    
    // Local Time Zone
    uint8_t tz_iei;                     // IEI = 0x46
    uint8_t tz_value;                   // Time zone value (BCD encoded)
    
    // Universal Time and Local Time Zone
    uint8_t utz_iei;                    // IEI = 0x47
    uint8_t utz_year;                   // Year (BCD)
    uint8_t utz_month;                  // Month (BCD)
    uint8_t utz_day;                    // Day (BCD)
    uint8_t utz_hour;                   // Hour (BCD)
    uint8_t utz_minute;                 // Minute (BCD)
    uint8_t utz_second;                 // Second (BCD)
    uint8_t utz_time_zone;              // Time zone (BCD with sign)
    
    // Daylight Saving Time
    uint8_t dst_iei;                    // IEI = 0x49
    uint8_t dst_length;                 // Length = 1
    uint8_t dst_value;                  // DST adjustment value byte
} ConfigurationUpdateCommandComplete;

#pragma pack(pop)

#endif // CONFIGURATION_UPDATE_COMMAND_H