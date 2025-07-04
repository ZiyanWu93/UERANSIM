#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/pdu_session_establishment_request.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_5/pdu_session_establishment_request.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open pdu_session_establishment_request.hex\n");
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
    
    // The PDU session message is embedded in UL NAS Transport
    // According to ROADMAP, PDU session establishment request starts after:
    // - Security header (7 bytes)
    // - UL NAS Transport header (4 bytes)  
    // - Payload container type (1 byte)
    // - Length (1 byte)
    // Total offset = 13 bytes
    
    PduSessionEstablishmentRequest *req = (PduSessionEstablishmentRequest *)(buffer + 13);
    
    // Validate fields as per ROADMAP parse tree
    assert(req->epd == 0x2E);  // 46 in decimal = 5GSM EPD
    assert(req->pdu_session_id == 1);  // PDU Session ID from parse tree
    assert(req->pti == 1);  // Procedure Transaction ID from parse tree
    assert(req->message_type == 0xC1);  // PDU Session Establishment Request
    assert(req->integrity_protection_max_data_rate_ul == 0xFF);  // 255 from parse tree
    assert(req->integrity_protection_max_data_rate_dl == 0xFF);  // 255 from parse tree
    
    printf("✓ Test PASSED: PDU Session Establishment Request validated successfully\n");
    return 0;
}