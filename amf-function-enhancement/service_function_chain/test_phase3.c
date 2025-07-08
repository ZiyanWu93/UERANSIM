#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Declare the phase3_transform function
int phase3_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len);

int main() {
    // Input: Security Mode Complete from phase_3/security_mode_complete.hex
    // Hex: 7e0422e4ee19007e005e7700094573806121856151f17100237e004179000d0199f9070000000000000000101001002e0480f080f02f020101530100
    uint8_t input[] = {
        0x7e, 0x04, 0x22, 0xe4, 0xee, 0x19, 0x00, 0x7e,
        0x00, 0x5e, 0x77, 0x00, 0x09, 0x45, 0x73, 0x80,
        0x61, 0x21, 0x85, 0x61, 0x51, 0xf1, 0x71, 0x00,
        0x23, 0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d, 0x01,
        0x99, 0xf9, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x10, 0x10, 0x01, 0x00, 0x2e,
        0x04, 0x80, 0xf0, 0x80, 0xf0, 0x2f, 0x02, 0x01,
        0x01, 0x53, 0x01, 0x00
    };
    size_t input_len = sizeof(input);
    
    // Expected output: Registration Accept from phase_3/registration_accept.hex
    // Hex: 7e027239674c017e0042010177000bf299f907020040c000072754074099f90700000115020101210201005e0192
    uint8_t expected[] = {
        0x7e, 0x02, 0x72, 0x39, 0x67, 0x4c, 0x01, 0x7e,
        0x00, 0x42, 0x01, 0x01, 0x77, 0x00, 0x0b, 0xf2,
        0x99, 0xf9, 0x07, 0x02, 0x00, 0x40, 0xc0, 0x00,
        0x07, 0x27, 0x54, 0x07, 0x40, 0x99, 0xf9, 0x07,
        0x00, 0x00, 0x01, 0x15, 0x02, 0x01, 0x01, 0x21,
        0x02, 0x01, 0x00, 0x5e, 0x01, 0x92
    };
    size_t expected_len = sizeof(expected);
    
    // Output buffer
    uint8_t output[256];
    size_t output_len;
    
    // Run transformation
    int result = phase3_transform(input, input_len, output, &output_len);
    
    if (result != 0) {
        printf("Phase 3 transformation failed\n");
        return 1;
    }
    
    // Print input
    printf("Input Security Mode Complete (%zu bytes):\n", input_len);
    for (size_t i = 0; i < input_len; i++) {
        printf("%02x", input[i]);
        if (i < input_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print output
    printf("Output Registration Accept (%zu bytes):\n", output_len);
    for (size_t i = 0; i < output_len; i++) {
        printf("%02x", output[i]);
        if (i < output_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print expected
    printf("Expected Registration Accept (%zu bytes):\n", expected_len);
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
    
    printf("✓ SUCCESS: Phase 3 transformation verified!\n");
    printf("  Output matches expected Registration Accept byte-for-byte\n");
    
    return 0;
}