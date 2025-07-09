#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Forward declaration of the phase5 transform function
int phase5_transform(const uint8_t *input, size_t input_len, 
                     uint8_t *output, size_t *output_len);

int main() {
    // Input: PDU Session Establishment Request
    uint8_t input[] = {
        0x7e, 0x02, 0xba, 0x02, 0x92, 0xcd, 0x02, 0x7e, 0x00, 0x67,
        0x01, 0x01, 0x7b, 0x00, 0x15, 0x2e, 0x01, 0x01, 0xc1, 0xff,
        0xff, 0x91, 0xa1, 0x28, 0x01, 0x00, 0x7b, 0x00, 0x07, 0x80,
        0x00, 0x0a, 0x00, 0x00, 0x0d, 0x00, 0x12, 0x01, 0x81, 0x01,
        0x22, 0x01, 0x01, 0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65,
        0x72, 0x6e, 0x65, 0x74
    };
    size_t input_len = sizeof(input);
    
    // Expected output: PDU Session Establishment Accept (from phase_5/messages.json)
    uint8_t expected[] = {
        0x7e, 0x02, 0xfb, 0xd6, 0x2d, 0x81, 0x03, 0x7e, 0x00, 0x68,
        0x01, 0x00, 0x47, 0x2e, 0x01, 0x01, 0xc2, 0x11, 0x00, 0x09,
        0x01, 0x00, 0x06, 0x31, 0x31, 0x01, 0x01, 0xff, 0x01, 0x06,
        0x03, 0xf4, 0x24, 0x03, 0xf4, 0x24, 0x29, 0x05, 0x01, 0x0a,
        0x2d, 0x00, 0x02, 0x22, 0x01, 0x01, 0x79, 0x00, 0x06, 0x01,
        0x20, 0x41, 0x01, 0x01, 0x09, 0x7b, 0x00, 0x0f, 0x80, 0x00,
        0x0d, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00, 0x0d, 0x04, 0x08,
        0x08, 0x04, 0x04, 0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65,
        0x72, 0x6e, 0x65, 0x74, 0x12, 0x01
    };
    size_t expected_len = sizeof(expected);
    
    // Output buffer
    uint8_t output[256];
    size_t output_len = 0;
    
    printf("Input PDU Session Establishment Request (%zu bytes):\n", input_len);
    for (size_t i = 0; i < input_len; i++) {
        printf("%02x", input[i]);
        if (i < input_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Run the transformation
    int result = phase5_transform(input, input_len, output, &output_len);
    
    if (result != 0) {
        printf("ERROR: phase5_transform failed with code %d\n", result);
        return 1;
    }
    
    printf("Output PDU Session Establishment Accept (%zu bytes):\n", output_len);
    for (size_t i = 0; i < output_len; i++) {
        printf("%02x", output[i]);
        if (i < output_len - 1) printf(":");
    }
    printf("\n\n");
    
    printf("Expected PDU Session Establishment Accept (%zu bytes):\n", expected_len);
    for (size_t i = 0; i < expected_len; i++) {
        printf("%02x", expected[i]);
        if (i < expected_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Compare output with expected
    if (output_len != expected_len) {
        printf("ERROR: Output length mismatch! Got %zu, expected %zu\n", 
               output_len, expected_len);
        return 1;
    }
    
    int mismatch = 0;
    for (size_t i = 0; i < expected_len; i++) {
        if (output[i] != expected[i]) {
            if (!mismatch) {
                printf("ERROR: Output mismatch starting at byte %zu:\n", i);
            }
            printf("  Byte %zu: got 0x%02x, expected 0x%02x\n", 
                   i, output[i], expected[i]);
            mismatch = 1;
        }
    }
    
    if (!mismatch) {
        printf("✓ SUCCESS: Phase 5 transformation verified!\n");
        printf("  Output matches expected PDU Session Establishment Accept byte-for-byte\n");
        return 0;
    }
    
    return 1;
}