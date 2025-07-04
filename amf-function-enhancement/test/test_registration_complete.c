#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/registration_complete.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_4/registration_complete.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open registration_complete.hex\n");
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
    
    // Cast to RegistrationComplete structure
    RegistrationComplete *comp = (RegistrationComplete *)buffer;
    
    // Validate security header
    assert(comp->security_header.epd == 0x7E);  // Extended Protocol Discriminator
    assert(comp->security_header.security_header == 2);  // Integrity protected and ciphered
    
    // Validate inner message fields
    assert(comp->inner_epd == 0x7E);  // Extended Protocol Discriminator
    assert(comp->inner_security_header == 0);  // Plain NAS message (inner)
    assert(comp->inner_spare == 0);  // Spare half octet
    assert(comp->message_type == 0x43);  // Registration Complete
    
    printf("✓ Test PASSED: Registration Complete validated successfully\n");
    return 0;
}