#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/authentication_response.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_2/authentication_response.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open authentication_response.hex\n");
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
    AuthenticationResponse *resp = (AuthenticationResponse *)buffer;
    
    // Validate fields as per ROADMAP-phase2.md parse tree
    assert(resp->epd == 0x7E);  // Extended Protocol Discriminator
    assert(resp->security_header == 0);  // Plain NAS message
    assert(resp->spare == 0);  // Spare half octet
    assert(resp->message_type == 0x57);  // Authentication Response
    
    // Validate Authentication Response Parameter RES
    assert(resp->auth_resp.iei == 0x2D);  // RES element identifier
    assert(resp->auth_resp.length == 16);  // RES* length
    const uint8_t expected_res[] = {
        0xEF, 0x27, 0x70, 0xC6, 0x9E, 0x73, 0x82, 0xAA,
        0x38, 0xE8, 0x13, 0x4F, 0x60, 0x22, 0x34, 0xE1
    };
    
    printf("DEBUG: Expected RES*: ");
    for (int i = 0; i < 16; i++) printf("%02X ", expected_res[i]);
    printf("\nDEBUG: Actual RES*: ");
    for (int i = 0; i < 16; i++) printf("%02X ", resp->auth_resp.res_star[i]);
    printf("\n");
    
    assert(memcmp(resp->auth_resp.res_star, expected_res, 16) == 0);
    
    printf("✓ Test PASSED: Authentication Response validated successfully\n");
    return 0;
}