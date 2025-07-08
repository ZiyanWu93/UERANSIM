#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Declare the phase4_transform function
int phase4_transform(const uint8_t *input, size_t input_len, 
                    uint8_t *output, size_t *output_len);

int main() {
    // Input: Registration Complete from phase_4/registration_complete.hex
    // Hex: 7e02469d6a8b017e0043
    uint8_t input[] = {
        0x7e, 0x02, 0x46, 0x9d, 0x6a, 0x8b, 0x01, 0x7e,
        0x00, 0x43
    };
    size_t input_len = sizeof(input);
    
    // Expected output: Configuration Update Command from phase_4/configuration_update_command.hex
    // Hex: 7e02de0d22e3027e0054430f90004f00700065006e003500470053450990004e006500780074460a475260903035530a490101
    uint8_t expected[] = {
        0x7e, 0x02, 0xde, 0x0d, 0x22, 0xe3, 0x02, 0x7e,
        0x00, 0x54, 0x43, 0x0f, 0x90, 0x00, 0x4f, 0x00,
        0x70, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x35, 0x00,
        0x47, 0x00, 0x53, 0x45, 0x09, 0x90, 0x00, 0x4e,
        0x00, 0x65, 0x00, 0x78, 0x00, 0x74, 0x46, 0x0a,
        0x47, 0x52, 0x60, 0x90, 0x30, 0x35, 0x53, 0x0a,
        0x49, 0x01, 0x01
    };
    size_t expected_len = sizeof(expected);
    
    // Output buffer
    uint8_t output[256];
    size_t output_len;
    
    // Run transformation
    int result = phase4_transform(input, input_len, output, &output_len);
    
    if (result != 0) {
        printf("Phase 4 transformation failed\n");
        return 1;
    }
    
    // Print input
    printf("Input Registration Complete (%zu bytes):\n", input_len);
    for (size_t i = 0; i < input_len; i++) {
        printf("%02x", input[i]);
        if (i < input_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print output
    printf("Output Configuration Update Command (%zu bytes):\n", output_len);
    for (size_t i = 0; i < output_len; i++) {
        printf("%02x", output[i]);
        if (i < output_len - 1) printf(":");
    }
    printf("\n\n");
    
    // Print expected
    printf("Expected Configuration Update Command (%zu bytes):\n", expected_len);
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
    
    printf("✓ SUCCESS: Phase 4 transformation verified!\n");
    printf("  Output matches expected Configuration Update Command byte-for-byte\n");
    
    return 0;
}