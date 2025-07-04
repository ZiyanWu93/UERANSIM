#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/security_mode_complete.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_3/security_mode_complete.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open security_mode_complete.hex\n");
        return 1;
    }
    
    char hex_str[512];
    if (fgets(hex_str, sizeof(hex_str), fp) == NULL) {
        fclose(fp);
        return 1;
    }
    fclose(fp);
    
    // Remove newline
    size_t len = strlen(hex_str);
    if (len > 0 && hex_str[len-1] == '\n') {
        hex_str[len-1] = '\0';
        len--;
    }
    
    // Convert hex to binary
    size_t byte_count = len / 2;
    for (size_t i = 0; i < byte_count; i++) {
        unsigned int byte;
        sscanf(hex_str + i * 2, "%2x", &byte);
        buffer[i] = (uint8_t)byte;
    }
    
    // Cast to SecurityModeComplete structure
    SecurityModeComplete *comp = (SecurityModeComplete *)buffer;
    
    // Validate security header
    assert(comp->security_header.epd == 0x7E);  // Extended Protocol Discriminator
    assert(comp->security_header.security_header == 4);  // Integrity protected and ciphered with new context
    
    // Validate inner message fields
    assert(comp->inner_epd == 0x7E);  // Extended Protocol Discriminator
    assert(comp->inner_security_header == 0);  // Plain NAS message (inner)
    assert(comp->inner_spare == 0);  // Spare half octet
    assert(comp->message_type == 0x5E);  // Security Mode Complete
    
    // Check for NAS message container IEI at the appropriate offset
    // The container starts after the fixed header
    uint8_t *ptr = buffer + sizeof(NasSecurityHeader) + 3;  // Skip security header + inner header
    assert(*ptr == 0x77);  // 5G-GUTI IEI (from IMEISV)
    
    printf("✓ Test PASSED: Security Mode Complete validated successfully\n");
    return 0;
}