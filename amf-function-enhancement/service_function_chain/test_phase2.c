#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Declare the phase2_transform function
int phase2_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len);

int main() {
    // Input: Authentication Response from phase_2/authentication_response.hex
    uint8_t input[] = {
        0x7e, 0x00, 0x57, 0x2d, 0x10, 0xef, 0x27, 0x70,
        0xc6, 0x9e, 0x73, 0x82, 0xaa, 0x38, 0xe8, 0x13,
        0x4f, 0x60, 0x22, 0x34, 0xe1
    };
    size_t input_len = sizeof(input);
    
    // Expected output: Security Mode Command from phase_2/security_mode_command.hex
    // Hex: 7e0313bf995a007e005d02000480f080f0e1360102
    uint8_t expected[] = {
        0x7e, 0x03, 0x13, 0xbf, 0x99, 0x5a, 0x00, 0x7e,
        0x00, 0x5d, 0x02, 0x00, 0x04, 0x80, 0xf0, 0x80,
        0xf0, 0xe1, 0x36, 0x01, 0x02
    };
    size_t expected_len = sizeof(expected);
    
    // Output buffer
    uint8_t output[256];
    size_t output_len;
    
    // Run transformation
    int result = phase2_transform(input, input_len, output, &output_len);
    
    if (result != 0) {
        printf("Phase 2 transformation failed\n");
        return 1;
    }
    
    // Print input
    printf("Input Authentication Response (%zu bytes):\n", input_len);
    for (size_t i = 0; i < input_len; i++) {
        printf("%02x", input[i]);
        if (i < input_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print output
    printf("Output Security Mode Command (%zu bytes):\n", output_len);
    for (size_t i = 0; i < output_len; i++) {
        printf("%02x", output[i]);
        if (i < output_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print expected
    printf("Expected Security Mode Command (%zu bytes):\n", expected_len);
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
    
    printf("✓ SUCCESS: Phase 2 transformation verified!\n");
    printf("  Output matches expected Security Mode Command byte-for-byte\n");
    
    return 0;
}