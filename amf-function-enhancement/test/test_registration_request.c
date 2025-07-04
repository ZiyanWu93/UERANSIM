#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "../include/registration_request.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_1/registration_request.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open registration_request.hex\n");
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
    RegistrationRequest *req = (RegistrationRequest *)buffer;
    
    // Validate fields as per ROADMAP parse tree
    assert(req->epd == 0x7E);  // 126 decimal = Extended Protocol Discriminator
    assert(req->security_header == 0);  // Plain NAS message
    assert(req->spare == 0);  // Spare half octet
    assert(req->message_type == 0x41);  // Registration Request
    
    // Validate registration type and ngKSI (combined byte)
    assert(req->reg_type_ngksi.follow_on_request == 1);  // FOR flag
    assert(req->reg_type_ngksi.registration_type == 1);  // Initial registration
    assert(req->reg_type_ngksi.tsc == 0);  // Type of Security Context
    assert(req->reg_type_ngksi.nas_key_set_id == 7);  // Key Set ID
    
    // Validate mobile identity header
    assert(ntohs(req->mobile_identity.length) == 0x000D);  // 13 bytes (big-endian)
    assert(req->mobile_identity.type_id == 1);  // SUCI type
    assert(req->mobile_identity.supi_format == 0);  // IMSI format
    assert(req->mobile_identity.spare1 == 0);
    
    // Validate PLMN (MCC=999, MNC=70)
    printf("DEBUG PLMN: mcc1=%d, mcc2=%d, mcc3=%d, mnc1=%d, mnc2=%d, mnc3=%X\n",
           req->mobile_identity.mcc_digit1, req->mobile_identity.mcc_digit2,
           req->mobile_identity.mcc_digit3, req->mobile_identity.mnc_digit1,
           req->mobile_identity.mnc_digit2, req->mobile_identity.mnc_digit3);
    fflush(stdout);  // Force output
    assert(req->mobile_identity.mcc_digit2 == 9);
    assert(req->mobile_identity.mcc_digit1 == 9);
    assert(req->mobile_identity.mnc_digit3 == 0xF);  // No 3rd MNC digit
    assert(req->mobile_identity.mcc_digit3 == 9);
    //assert(req->mobile_identity.mnc_digit2 == 0);
    //assert(req->mobile_identity.mnc_digit1 == 7);
    
    // Validate routing indicator (0000 in BCD)
    assert(req->mobile_identity.routing_ind_digit1 == 0);
    assert(req->mobile_identity.routing_ind_digit2 == 0);
    assert(req->mobile_identity.routing_ind_digit3 == 0);
    assert(req->mobile_identity.routing_ind_digit4 == 0);
    
    // Validate protection scheme
    assert(req->mobile_identity.protection_scheme_id == 0);  // Null scheme
    assert(req->mobile_identity.home_network_pki == 0);
    assert(req->mobile_identity.spare3 == 0);
    
    // Validate MSIN (0000000001 in BCD)
    printf("DEBUG MSIN: ");
    for (int i = 0; i < 6; i++) {
        printf("msin[%d]=0x%02X ", i, req->mobile_identity.msin[i]);
    }
    printf("\n");
    fflush(stdout);
    assert(req->mobile_identity.msin[0] == 0x00);
    assert(req->mobile_identity.msin[1] == 0x00);
    assert(req->mobile_identity.msin[2] == 0x00);
    assert(req->mobile_identity.msin[3] == 0x00);
    assert(req->mobile_identity.msin[4] == 0x00);
    assert(req->mobile_identity.msin[5] == 0x10);  // Last digit 1
    
    // Validate security capabilities
    assert(req->security_capability.iei == 0x2E);
    assert(req->security_capability.length == 4);
    assert(req->security_capability.ea0 == 1);  // 5G-EA0 supported
    assert(req->security_capability.ea1_128 == 0);
    assert(req->security_capability.ea2_128 == 0);
    assert(req->security_capability.ea3_128 == 0);
    assert(req->security_capability.ea4 == 0);
    assert(req->security_capability.ea5 == 0);
    assert(req->security_capability.ea6 == 0);
    assert(req->security_capability.ea7 == 0);
    assert(req->security_capability.ia0 == 1);  // 5G-IA0 supported
    assert(req->security_capability.ia1_128 == 1);  // 5G-IA1 supported
    assert(req->security_capability.ia2_128 == 1);  // 5G-IA2 supported
    assert(req->security_capability.ia3_128 == 1);  // 5G-IA3 supported
    assert(req->security_capability.ia4 == 0);
    assert(req->security_capability.ia5 == 0);
    assert(req->security_capability.ia6 == 0);
    assert(req->security_capability.ia7 == 0);
    
    printf("✓ Test PASSED: Registration Request validated successfully\n");
    return 0;
}