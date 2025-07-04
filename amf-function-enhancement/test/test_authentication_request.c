#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/authentication_request.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_1/authentication_request.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open authentication_request.hex\n");
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
    AuthenticationRequest *req = (AuthenticationRequest *)buffer;
    
    // Validate fields as per ROADMAP parse tree
    assert(req->epd == 0x7E);  // Extended Protocol Discriminator
    assert(req->security_header == 0);  // Plain NAS message
    assert(req->spare == 0);  // Spare half octet
    assert(req->message_type == 0x56);  // Authentication Request
    assert(req->nas_key_set_id == 0);  // New Key Set ID
    assert(req->tsc == 0);  // Type of Security Context
    assert(req->spare2 == 0);  // Spare bits
    
    // Validate ABBA
    assert(req->abba.length == 2);  // ABBA length
    assert(req->abba.contents[0] == 0x00);  // Anti-Bidding contents
    assert(req->abba.contents[1] == 0x00);
    
    // Validate RAND parameter
    assert(req->rand.iei == 0x21);  // RAND element identifier
    const uint8_t expected_rand[] = {
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf,
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69
    };
    assert(memcmp(req->rand.rand, expected_rand, 16) == 0);
    
    // Validate AUTN parameter
    assert(req->autn.iei == 0x20);  // AUTN element identifier
    assert(req->autn.length == 16);  // AUTN length
    const uint8_t expected_sqn_xor_ak[] = {0x40, 0x62, 0x96, 0x99, 0x30, 0x82};
    assert(memcmp(req->autn.sqn_xor_ak, expected_sqn_xor_ak, 6) == 0);
    assert(req->autn.amf[0] == 0x80);  // AMF field
    assert(req->autn.amf[1] == 0x00);
    const uint8_t expected_mac[] = {0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19};
    assert(memcmp(req->autn.mac, expected_mac, 8) == 0);
    
    printf("✓ Test PASSED: Authentication Request validated successfully\n");
    return 0;
}