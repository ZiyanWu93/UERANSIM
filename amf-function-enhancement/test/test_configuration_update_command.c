#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/configuration_update_command.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_4/configuration_update_command.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open configuration_update_command.hex\n");
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
    
    // Cast to ConfigurationUpdateCommand structure
    ConfigurationUpdateCommand *cmd = (ConfigurationUpdateCommand *)buffer;
    
    // Validate security header
    assert(cmd->security_header.epd == 0x7E);  // Extended Protocol Discriminator
    assert(cmd->security_header.security_header == 2);  // Integrity protected and ciphered
    
    // Validate inner message fields
    assert(cmd->inner_epd == 0x7E);  // Extended Protocol Discriminator
    assert(cmd->inner_security_header == 0);  // Plain NAS message (inner)
    assert(cmd->inner_spare == 0);  // Spare half octet
    assert(cmd->message_type == 0x54);  // Configuration Update Command
    
    // Validate first IE - Full network name
    uint8_t *ptr = buffer + sizeof(NasSecurityHeader) + 3;  // Skip to IEs
    assert(*ptr == 0x43);  // Full network name IEI
    ptr++;
    assert(*ptr == 0x0F);  // Length = 15
    ptr++;
    assert(*ptr == 0x90);  // Extension=1, coding=1, add_ci=0, spare=0
    
    printf("✓ Test PASSED: Configuration Update Command validated successfully\n");
    return 0;
}