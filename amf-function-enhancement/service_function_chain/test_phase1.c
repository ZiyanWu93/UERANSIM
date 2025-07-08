#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Declare the phase1_transform function
int phase1_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len);

int main() {
    // Input: Registration Request from phase_1/registration_request.hex
    uint8_t input[] = {
        0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d, 0x01, 0x99, 
        0xf9, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x10, 0x2e, 0x04, 0x80, 0xf0, 0x80, 
        0xf0
    };
    size_t input_len = sizeof(input);
    
    // Expected output: Authentication Request from phase_1/authentication_request.hex
    uint8_t expected[] = {
        0x7e, 0x00, 0x56, 0x00, 0x02, 0x00, 0x00, 0x21, 
        0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 
        0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69, 
        0x20, 0x10, 0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 
        0x80, 0x00, 0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 
        0x0b, 0x19
    };
    size_t expected_len = sizeof(expected);
    
    // Output buffer
    uint8_t output[256];
    size_t output_len;
    
    // Run transformation
    int result = phase1_transform(input, input_len, output, &output_len);
    
    if (result != 0) {
        printf("Phase 1 transformation failed\n");
        return 1;
    }
    
    // Print input
    printf("Input Registration Request (%zu bytes):\n", input_len);
    for (size_t i = 0; i < input_len; i++) {
        printf("%02x", input[i]);
        if (i < input_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print output
    printf("Output Authentication Request (%zu bytes):\n", output_len);
    for (size_t i = 0; i < output_len; i++) {
        printf("%02x", output[i]);
        if (i < output_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print expected
    printf("Expected Authentication Request (%zu bytes):\n", expected_len);
    for (size_t i = 0; i < expected_len; i++) {
        printf("%02x", expected[i]);
        if (i < expected_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Verify output matches expected
    if (output_len != expected_len) {
        printf("ERROR: Output length mismatch. Got %zu, expected %zu\n", 
               output_len, expected_len);
        return 1;
    }
    
    if (memcmp(output, expected, expected_len) != 0) {
        printf("ERROR: Output content mismatch\n");
        printf("Byte-by-byte comparison:\n");
        for (size_t i = 0; i < expected_len; i++) {
            if (output[i] != expected[i]) {
                printf("  Byte %zu: got 0x%02x, expected 0x%02x\n", 
                       i, output[i], expected[i]);
            }
        }
        return 1;
    }
    
    printf("✓ SUCCESS: Phase 1 transformation verified!\n");
    printf("  Output matches expected Authentication Request byte-for-byte\n");
    
    return 0;
}