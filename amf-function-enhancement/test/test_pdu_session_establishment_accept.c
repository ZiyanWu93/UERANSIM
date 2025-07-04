#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/pdu_session_establishment_accept.h"

int main() {
    uint8_t buffer[256];
    
    // Read the hex file
    FILE *fp = fopen("phase_5/pdu_session_establishment_accept.hex", "r");
    if (!fp) {
        fprintf(stderr, "Failed to open pdu_session_establishment_accept.hex\n");
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
    
    // The PDU session accept message is embedded in DL NAS Transport
    // According to hex data analysis:
    // - Security header (7 bytes): 7E 02 FB D6 2D 81 03
    // - DL NAS Transport header (6 bytes): 7E 00 68 01 00 47
    // Total offset = 13 bytes
    
    PduSessionEstablishmentAccept *acc = (PduSessionEstablishmentAccept *)(buffer + 13);
    
    // Validate fields as per ROADMAP-phase5.md parse tree
    assert(acc->epd == 0x2E);  // 46 in decimal = 5GSM EPD
    assert(acc->pdu_session_id == 1);  // PDU Session ID from parse tree
    assert(acc->pti == 1);  // Procedure Transaction ID from parse tree
    assert(acc->message_type == 0xC2);  // PDU Session Establishment Accept
    assert(acc->selected_ssc_mode == 1);  // Selected SSC mode 1
    
    printf("✓ Test PASSED: PDU Session Establishment Accept validated successfully\n");
    return 0;
}