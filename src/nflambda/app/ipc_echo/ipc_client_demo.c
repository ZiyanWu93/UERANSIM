/*
 * IPC Client Demo
 * 
 * Simple client application that sends messages to the nflambda
 * IPC echo server and displays the responses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "../../event_system/ipc_client.h"

static void print_usage(const char* program)
{
    printf("Usage: %s [message]\n", program);
    printf("  If no message is provided, enters interactive mode\n");
    printf("  In interactive mode, type 'quit' to exit\n");
}

static double get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000.0 + ts.tv_nsec / 1000000.0;
}

int main(int argc, char* argv[])
{
    int fd;
    char send_buffer[2048];
    char recv_buffer[2048];
    size_t recv_len;
    int interactive = (argc == 1);
    
    printf("IPC Client Demo\n");
    printf("===============\n\n");
    
    // Connect to server
    printf("Connecting to nflambda IPC server...\n");
    fd = ipc_client_connect(NULL);
    if (fd < 0) {
        fprintf(stderr, "Failed to connect to IPC server\n");
        fprintf(stderr, "Make sure the IPC echo demo is running\n");
        return 1;
    }
    printf("Connected successfully\n\n");
    
    if (!interactive) {
        // Single message mode
        const char* message = argv[1];
        double start_time = get_time_ms();
        
        printf("Sending: '%s'\n", message);
        recv_len = sizeof(recv_buffer);
        if (ipc_client_send_recv(fd, message, strlen(message),
                                recv_buffer, &recv_len) < 0) {
            fprintf(stderr, "Failed to send/receive message\n");
            ipc_client_close(fd);
            return 1;
        }
        
        double end_time = get_time_ms();
        recv_buffer[recv_len] = '\0';
        
        printf("Received: '%s'\n", recv_buffer);
        printf("Round-trip time: %.2f ms\n", end_time - start_time);
    } else {
        // Interactive mode
        printf("Interactive mode - type messages to send\n");
        printf("Type 'quit' to exit\n\n");
        
        while (1) {
            printf("> ");
            fflush(stdout);
            
            if (fgets(send_buffer, sizeof(send_buffer), stdin) == NULL) {
                break;
            }
            
            // Remove newline
            size_t len = strlen(send_buffer);
            if (len > 0 && send_buffer[len-1] == '\n') {
                send_buffer[len-1] = '\0';
                len--;
            }
            
            // Check for quit command
            if (strcmp(send_buffer, "quit") == 0) {
                break;
            }
            
            if (len == 0) {
                continue;
            }
            
            // Send and receive
            double start_time = get_time_ms();
            recv_len = sizeof(recv_buffer);
            
            if (ipc_client_send_recv(fd, send_buffer, len,
                                    recv_buffer, &recv_len) < 0) {
                fprintf(stderr, "Failed to send/receive message\n");
                break;
            }
            
            double end_time = get_time_ms();
            recv_buffer[recv_len] = '\0';
            
            printf("< %s\n", recv_buffer);
            printf("  (%.2f ms)\n\n", end_time - start_time);
        }
    }
    
    // Cleanup
    printf("\nClosing connection...\n");
    ipc_client_close(fd);
    printf("Goodbye!\n");
    
    return 0;
}