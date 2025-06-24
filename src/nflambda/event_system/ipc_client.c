#include "ipc_client.h"
#include "ipc_event_source.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

int ipc_client_connect(const char* socket_path)
{
    int fd;
    struct sockaddr_un addr;
    
    /* Use default path if none provided */
    if (socket_path == NULL) {
        socket_path = IPC_DEFAULT_SOCKET_PATH;
    }
    
    /* Create Unix domain socket */
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket");
        return -1;
    }
    
    /* Connect to server */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    
    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(fd);
        return -1;
    }
    
    return fd;
}

int ipc_client_send_recv(int fd, const void* send_data, size_t send_len,
                        void* recv_data, size_t* recv_len)
{
    IpcMessage send_msg, recv_msg;
    ssize_t bytes;
    
    if (fd < 0 || send_data == NULL || recv_data == NULL || recv_len == NULL) {
        return -1;
    }
    
    if (send_len > IPC_MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "Send data too large (%zu > %d)\n", send_len, IPC_MAX_PAYLOAD_SIZE);
        return -1;
    }
    
    /* Prepare send message */
    send_msg.length = (uint32_t)send_len;
    memcpy(send_msg.data, send_data, send_len);
    
    /* Send message */
    size_t total_send_size = sizeof(send_msg.length) + send_len;
    bytes = send(fd, &send_msg, total_send_size, 0);
    if (bytes != (ssize_t)total_send_size) {
        perror("send");
        return -1;
    }
    
    /* Receive response header */
    bytes = recv(fd, &recv_msg.length, sizeof(recv_msg.length), MSG_WAITALL);
    if (bytes != sizeof(recv_msg.length)) {
        if (bytes == 0) {
            fprintf(stderr, "Server closed connection\n");
        } else {
            perror("recv header");
        }
        return -1;
    }
    
    /* Validate response length */
    if (recv_msg.length > IPC_MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "Invalid response length %u\n", recv_msg.length);
        return -1;
    }
    
    /* Receive response data */
    if (recv_msg.length > 0) {
        bytes = recv(fd, recv_msg.data, recv_msg.length, MSG_WAITALL);
        if (bytes != (ssize_t)recv_msg.length) {
            perror("recv data");
            return -1;
        }
    }
    
    /* Copy to user buffer */
    size_t copy_len = recv_msg.length;
    if (copy_len > *recv_len) {
        copy_len = *recv_len;
    }
    memcpy(recv_data, recv_msg.data, copy_len);
    *recv_len = recv_msg.length;
    
    return 0;
}

void ipc_client_close(int fd)
{
    if (fd >= 0) {
        close(fd);
    }
}