#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/registration_accept.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_3/registration_accept.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open registration_accept.hex\n");
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
    
    // Cast to RegistrationAccept structure
    RegistrationAccept *acc = (RegistrationAccept *)buffer;
    
    // Validate security header
    assert(acc->security_header.epd == 0x7E);  // Extended Protocol Discriminator
    assert(acc->security_header.security_header == 2);  // Integrity protected and ciphered
    
    // Validate inner message fields
    assert(acc->inner_epd == 0x7E);  // Extended Protocol Discriminator
    assert(acc->inner_security_header == 0);  // Plain NAS message (inner)
    assert(acc->inner_spare == 0);  // Spare half octet
    assert(acc->message_type == 0x42);  // Registration Accept
    
    // Validate 5GS registration result
    assert(acc->reg_result.length == 1);
    assert(acc->reg_result.sms_allowed == 0);  // SMS over NAS not allowed
    assert(acc->reg_result.registration_result == 1);  // 3GPP access
    
    // Validate 5G-GUTI (starts after fixed header)
    uint8_t *ptr = buffer + sizeof(NasSecurityHeader) + 3 + sizeof(RegistrationResult);
    Guti5G *guti = (Guti5G *)ptr;
    
    assert(guti->iei == 0x77);  // 5G-GUTI IEI
    printf("DEBUG: GUTI length field = 0x%04X (expected 0x0B00)\n", guti->length);
    fflush(stdout);
    assert(guti->length == 0x0B00);  // 11 in big-endian 16-bit
    assert(guti->type_of_identity == 2);  // 5G-GUTI
    assert(guti->spare2 == 0xF);
    
    // Validate PLMN (MCC=999, MNC=70)
    printf("DEBUG PLMN: mcc1=%d, mcc2=%d, mcc3=%d, mnc1=%d, mnc2=%d, mnc3=%X\n",
           guti->mcc_digit1, guti->mcc_digit2, guti->mcc_digit3,
           guti->mnc_digit1, guti->mnc_digit2, guti->mnc_digit3);
    fflush(stdout);
    assert(guti->mcc_digit2 == 9);
    assert(guti->mcc_digit1 == 9);
    assert(guti->mnc_digit3 == 0xF);  // No 3rd MNC digit
    assert(guti->mcc_digit3 == 9);
    assert(guti->mnc_digit1 == 7);
    assert(guti->mnc_digit2 == 0);
    
    // AMF identifiers (from parse tree)
    printf("DEBUG AMF: region=%d, set_id=%d, pointer=%d\n", 
           guti->amf_region_id, guti->amf_set_id, guti->amf_pointer);
    fflush(stdout);
    assert(guti->amf_region_id == 2);  // From parse tree
    assert(guti->amf_set_id == 256);   // 0x0040 >> 6 = 0x01 in upper 10 bits = 256
    assert(guti->amf_pointer == 0);    // From parse tree
    
    // 5G-TMSI (3221227303 from parse tree = 0xC0000727)
    assert(guti->tmsi_5g == 0x270700C0);  // Little-endian representation
    
    printf("✓ Test PASSED: Registration Accept validated successfully\n");
    return 0;
}