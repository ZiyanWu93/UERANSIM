#ifndef IPC_CLIENT_H
#define IPC_CLIENT_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Connect to nflambda IPC server
 * 
 * @param socket_path Path to the Unix domain socket (NULL for default)
 * @return Socket file descriptor on success, -1 on error
 */
int ipc_client_connect(const char* socket_path);

/**
 * Send a message and wait for response (blocking)
 * 
 * @param fd Socket file descriptor from ipc_client_connect
 * @param send_data Data to send
 * @param send_len Length of data to send
 * @param recv_data Buffer to receive response
 * @param recv_len Pointer to size of recv_data buffer, updated with actual length
 * @return 0 on success, -1 on error
 */
int ipc_client_send_recv(int fd, const void* send_data, size_t send_len,
                        void* recv_data, size_t* recv_len);

/**
 * Close IPC connection
 * 
 * @param fd Socket file descriptor to close
 */
void ipc_client_close(int fd);

#ifdef __cplusplus
}
#endif

#endif /* IPC_CLIENT_H */