#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/security_mode_command.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_2/security_mode_command.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open security_mode_command.hex\n");
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
    
    // Cast to structure
    SecurityModeCommand *cmd = (SecurityModeCommand *)buffer;
    
    // Validate security header
    assert(cmd->security_header.epd == 0x7E);  // Extended Protocol Discriminator
    assert(cmd->security_header.security_header == 3);  // Integrity protected with new context
    
    // Validate inner message fields
    assert(cmd->inner_epd == 0x7E);  // Extended Protocol Discriminator
    assert(cmd->inner_security_header == 0);  // Plain NAS message
    assert(cmd->inner_spare == 0);  // Spare half octet
    assert(cmd->message_type == 0x5D);  // Security Mode Command
    
    // Validate NAS security algorithms
    assert(cmd->algorithms.ciphering_algo == 0);  // 5G-EA0
    assert(cmd->algorithms.integrity_algo == 2);  // 5G-IA2
    
    // Validate ngKSI
    assert(cmd->tsc == 0);  // Native security context
    assert(cmd->nas_key_set_id == 0);  // Key set ID
    
    // Validate UE security capability
    assert(cmd->ue_capability_length == 4);
    assert(cmd->ea_byte1 == 0x80);  // EA0 supported (bit 7)
    assert(cmd->ia_byte1 == 0xF0);  // IA0-IA3 supported
    assert(cmd->ea_byte2 == 0x80);  // EEA0 supported
    assert(cmd->ia_byte2 == 0xF0);  // EIA0-EIA3 supported
    
    printf("✓ Test PASSED: Security Mode Command validated successfully\n");
    return 0;
}