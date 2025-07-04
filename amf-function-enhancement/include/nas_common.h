#ifndef NAS_COMMON_H
#define NAS_COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Force byte-aligned structures
#pragma pack(push, 1)

// Common NAS 5GS constants
#define NAS_5GS_EPD 0x7E

// Security header types
#define NAS_SECURITY_HEADER_PLAIN                  0x00
#define NAS_SECURITY_HEADER_INTEGRITY_PROTECTED    0x01
#define NAS_SECURITY_HEADER_INTEGRITY_CIPHERED     0x02
#define NAS_SECURITY_HEADER_INTEGRITY_NEW_CONTEXT  0x03
#define NAS_SECURITY_HEADER_INTEGRITY_CIPHERED_NEW 0x04

// Message types
#define NAS_5GS_REGISTRATION_REQUEST     0x41
#define NAS_5GS_REGISTRATION_ACCEPT      0x42
#define NAS_5GS_REGISTRATION_COMPLETE    0x43
#define NAS_5GS_AUTHENTICATION_REQUEST   0x56
#define NAS_5GS_AUTHENTICATION_RESPONSE  0x57
#define NAS_5GS_SECURITY_MODE_COMMAND    0x5D
#define NAS_5GS_SECURITY_MODE_COMPLETE   0x5E
#define NAS_5GS_CONFIGURATION_UPDATE_CMD 0x54

// 5GSM message types
#define NAS_5GSM_PDU_SESSION_EST_REQUEST  0xC1
#define NAS_5GSM_PDU_SESSION_EST_ACCEPT   0xC2

// Information Element Identifiers
#define IEI_5G_GUTI                 0x77
#define IEI_TAI_LIST                0x54
#define IEI_ALLOWED_NSSAI           0x15
#define IEI_5GS_NETWORK_FEATURE     0x21
#define IEI_T3512_TIMER             0x5E
#define IEI_IMEISV_REQUEST          0xE1
#define IEI_ADDITIONAL_5G_SECURITY  0x36
#define IEI_UE_SECURITY_CAPABILITY  0x2E
#define IEI_FULL_NAME_NETWORK       0x43
#define IEI_SHORT_NAME_NETWORK      0x45
#define IEI_LOCAL_TIME_ZONE         0x46
#define IEI_TIME_ZONE_AND_TIME      0x47
#define IEI_DAYLIGHT_SAVING_TIME    0x49

// Common NAS header structure
typedef struct {
    uint8_t epd : 8;              // Extended Protocol Discriminator
    uint8_t security_header : 4;  // Security header type
    uint8_t spare : 4;            // Spare half octet
} NasHeader;

// Security protected NAS header
typedef struct {
    uint8_t epd : 8;              // Extended Protocol Discriminator
    uint8_t security_header : 4;  // Security header type
    uint8_t spare : 4;            // Spare half octet
    uint32_t mac;                 // Message Authentication Code
    uint8_t sequence_number;      // Sequence number
} NasSecurityHeader;

// Helper functions
static inline uint16_t swap16(uint16_t value) {
    return ((value & 0xFF00) >> 8) | ((value & 0x00FF) << 8);
}

static inline uint32_t swap32(uint32_t value) {
    return ((value & 0xFF000000) >> 24) |
           ((value & 0x00FF0000) >> 8) |
           ((value & 0x0000FF00) << 8) |
           ((value & 0x000000FF) << 24);
}

// BCD encoding/decoding helpers
static inline uint8_t bcd_encode(uint8_t digit) {
    return (digit < 10) ? digit : 0xF;
}

static inline uint8_t bcd_decode(uint8_t bcd) {
    return (bcd <= 9) ? bcd : 0;
}

// Hex string to binary conversion
static inline int hex_to_bin(const char *hex, uint8_t *bin, size_t bin_size) {
    size_t hex_len = strlen(hex);
    if (hex_len != bin_size * 2) return -1;
    
    for (size_t i = 0; i < bin_size; i++) {
        unsigned int byte;
        if (sscanf(hex + i * 2, "%2x", &byte) != 1) return -1;
        bin[i] = (uint8_t)byte;
    }
    return 0;
}

// Binary to hex string conversion
static inline void bin_to_hex(const uint8_t *bin, size_t bin_size, char *hex) {
    for (size_t i = 0; i < bin_size; i++) {
        sprintf(hex + i * 2, "%02x", bin[i]);
    }
    hex[bin_size * 2] = '\0';
}

// Read hex file into binary buffer
static inline int read_hex_file(const char *filename, uint8_t *buffer, size_t buffer_size) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Failed to open file");
        return -1;
    }
    
    char hex_str[1024];
    if (fgets(hex_str, sizeof(hex_str), fp) == NULL) {
        fclose(fp);
        return -1;
    }
    fclose(fp);
    
    // Remove newline if present
    size_t len = strlen(hex_str);
    if (len > 0 && hex_str[len-1] == '\n') {
        hex_str[len-1] = '\0';
    }
    
    return hex_to_bin(hex_str, buffer, buffer_size);
}

#pragma pack(pop)

#endif // NAS_COMMON_H