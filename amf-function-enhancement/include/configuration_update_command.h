#ifndef CONFIGURATION_UPDATE_COMMAND_H
#define CONFIGURATION_UPDATE_COMMAND_H

#include "nas_common.h"

#pragma pack(push, 1)

// Network name in UTF-16 format
typedef struct {
    uint8_t iei;                     // IEI (0x43 for full, 0x45 for short)
    uint8_t length;                  // Length of name
    uint8_t ext : 1;                 // Extension bit (1)
    uint8_t coding_scheme : 3;       // Coding scheme (1 = UCS2/UTF-16)
    uint8_t add_ci : 1;              // Add country initials
    uint8_t spare : 3;               // Spare bits
    uint8_t text[];                  // UTF-16 encoded text (variable length)
} NetworkName;

// Local time zone
typedef struct {
    uint8_t iei;                     // IEI = 0x46
    uint8_t time_zone;               // Time zone value (BCD encoded)
} LocalTimeZone;

// Universal time and local time zone
typedef struct {
    uint8_t iei;                     // IEI = 0x47
    uint8_t year;                    // Year (BCD)
    uint8_t month;                   // Month (BCD)
    uint8_t day;                     // Day (BCD)
    uint8_t hour;                    // Hour (BCD)
    uint8_t minute;                  // Minute (BCD)
    uint8_t second;                  // Second (BCD)
    uint8_t time_zone;               // Time zone (BCD with sign)
} UniversalTimeAndLocalTimeZone;

// Daylight saving time
typedef struct {
    uint8_t iei;                     // IEI = 0x49
    uint8_t length;                  // Length = 1
    uint8_t value : 2;               // DST adjustment (0, 1, or 2 hours)
    uint8_t spare : 6;               // Spare bits
} DaylightSavingTime;

// Complete Configuration Update Command message (fixed part)
typedef struct {
    // Security header
    NasSecurityHeader security_header;
    
    // Inner message
    uint8_t inner_epd;               // Extended Protocol Discriminator (0x7E)
    uint8_t inner_security_header : 4; // Security header type (0 = plain)
    uint8_t inner_spare : 4;         // Spare half octet
    uint8_t message_type;            // Message type (0x54)
    
    // Optional IEs follow (network names, time zone, etc.)
} ConfigurationUpdateCommand;

#pragma pack(pop)

#endif // CONFIGURATION_UPDATE_COMMAND_H