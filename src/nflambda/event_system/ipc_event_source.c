#include "ipc_event_source.h"
#include "event.h"
#include "../runtime/runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>

/* Global state for the IPC event source */
static struct {
    int server_fd;          /* Server socket file descriptor */
    int client_fd;          /* Connected client file descriptor */
    char socket_path[108];  /* Unix socket path (max length) */
    bool initialized;       /* Initialization flag */
} ipc_state = {
    .server_fd = -1,
    .client_fd = -1,
    .socket_path = "",
    .initialized = false
};

/* Set socket to non-blocking mode */
static int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int ipc_event_source_init(const char* socket_path)
{
    struct sockaddr_un addr;
    
    if (ipc_state.initialized) {
        fprintf(stderr, "IPC event source already initialized\n");
        return -1;
    }
    
    /* Use default path if none provided */
    if (socket_path == NULL) {
        socket_path = IPC_DEFAULT_SOCKET_PATH;
    }
    
    /* Create Unix domain socket */
    ipc_state.server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (ipc_state.server_fd == -1) {
        perror("socket");
        return -1;
    }
    
    /* Set non-blocking mode */
    if (set_nonblocking(ipc_state.server_fd) == -1) {
        perror("fcntl");
        close(ipc_state.server_fd);
        return -1;
    }
    
    /* Remove existing socket file */
    unlink(socket_path);
    
    /* Bind to socket path */
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);
    
    if (bind(ipc_state.server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(ipc_state.server_fd);
        return -1;
    }
    
    /* Listen for connections */
    if (listen(ipc_state.server_fd, 1) == -1) {
        perror("listen");
        close(ipc_state.server_fd);
        unlink(socket_path);
        return -1;
    }
    
    /* Save socket path for cleanup */
    strncpy(ipc_state.socket_path, socket_path, sizeof(ipc_state.socket_path) - 1);
    ipc_state.initialized = true;
    
    printf("IPC event source initialized on %s\n", socket_path);
    return 0;
}

void ipc_event_source_poll(void)
{
    struct timeval tv = {0, 0};  /* Zero timeout for non-blocking */
    fd_set readfds;
    int max_fd;
    
    if (!ipc_state.initialized) {
        return;
    }
    
    FD_ZERO(&readfds);
    FD_SET(ipc_state.server_fd, &readfds);
    max_fd = ipc_state.server_fd;
    
    /* Also check client socket if connected */
    if (ipc_state.client_fd != -1) {
        FD_SET(ipc_state.client_fd, &readfds);
        if (ipc_state.client_fd > max_fd) {
            max_fd = ipc_state.client_fd;
        }
    }
    
    /* Non-blocking select */
    int ret = select(max_fd + 1, &readfds, NULL, NULL, &tv);
    if (ret == -1) {
        if (errno != EINTR) {
            perror("select");
        }
        return;
    }
    
    if (ret == 0) {
        /* No activity */
        return;
    }
    
    /* Check for new connection */
    if (FD_ISSET(ipc_state.server_fd, &readfds)) {
        /* Accept new connection only if no client connected */
        if (ipc_state.client_fd == -1) {
            struct sockaddr_un client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int new_fd = accept(ipc_state.server_fd, 
                               (struct sockaddr*)&client_addr, &client_len);
            if (new_fd != -1) {
                /* Set non-blocking */
                if (set_nonblocking(new_fd) == 0) {
                    ipc_state.client_fd = new_fd;
                    printf("IPC client connected\n");
                } else {
                    close(new_fd);
                }
            }
        } else {
            /* Reject additional connections */
            struct sockaddr_un client_addr;
            socklen_t client_len = sizeof(client_addr);
            int reject_fd = accept(ipc_state.server_fd,
                                  (struct sockaddr*)&client_addr, &client_len);
            if (reject_fd != -1) {
                close(reject_fd);
            }
        }
    }
    
    /* Check for client data */
    if (ipc_state.client_fd != -1 && FD_ISSET(ipc_state.client_fd, &readfds)) {
        IpcMessage msg;
        ssize_t bytes_read;
        
        /* Read message header first */
        bytes_read = recv(ipc_state.client_fd, &msg.length, sizeof(msg.length), MSG_PEEK);
        if (bytes_read <= 0) {
            /* Client disconnected or error */
            close(ipc_state.client_fd);
            ipc_state.client_fd = -1;
            printf("IPC client disconnected\n");
            return;
        }
        
        /* Validate message length */
        if (msg.length > IPC_MAX_PAYLOAD_SIZE) {
            /* Invalid message, disconnect client */
            close(ipc_state.client_fd);
            ipc_state.client_fd = -1;
            fprintf(stderr, "IPC: Invalid message length %u\n", msg.length);
            return;
        }
        
        /* Read full message */
        size_t total_size = sizeof(msg.length) + msg.length;
        bytes_read = recv(ipc_state.client_fd, &msg, total_size, 0);
        if (bytes_read != (ssize_t)total_size) {
            /* Partial read or error */
            if (bytes_read <= 0) {
                close(ipc_state.client_fd);
                ipc_state.client_fd = -1;
                printf("IPC client disconnected\n");
            }
            return;
        }
        
        /* Trigger event with message data */
        /* Copy message data to event payload */
        char payload[MAX_NAS_HEX_LEN];
        if (msg.length < MAX_NAS_HEX_LEN) {
            memcpy(payload, msg.data, msg.length);
            payload[msg.length] = '\0';
        } else {
            memcpy(payload, msg.data, MAX_NAS_HEX_LEN - 1);
            payload[MAX_NAS_HEX_LEN - 1] = '\0';
        }
        
        trigger_event(EVENT_IPC_MESSAGE_RECEIVED, payload);
    }
}

int ipc_send_response(const void* data, size_t len)
{
    IpcMessage msg;
    ssize_t bytes_sent;
    
    if (!ipc_state.initialized || ipc_state.client_fd == -1) {
        return -1;
    }
    
    if (len > IPC_MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "IPC: Message too large (%zu > %d)\n", len, IPC_MAX_PAYLOAD_SIZE);
        return -1;
    }
    
    /* Prepare message */
    msg.length = (uint32_t)len;
    memcpy(msg.data, data, len);
    
    /* Send message */
    size_t total_size = sizeof(msg.length) + len;
    bytes_sent = send(ipc_state.client_fd, &msg, total_size, MSG_NOSIGNAL);
    
    if (bytes_sent == -1) {
        if (errno == EPIPE || errno == ECONNRESET) {
            /* Client disconnected */
            close(ipc_state.client_fd);
            ipc_state.client_fd = -1;
            printf("IPC client disconnected during send\n");
        }
        return -1;
    }
    
    return (bytes_sent == (ssize_t)total_size) ? (int)len : -1;
}

bool ipc_is_client_connected(void)
{
    return ipc_state.initialized && ipc_state.client_fd != -1;
}

void ipc_event_source_cleanup(void)
{
    if (!ipc_state.initialized) {
        return;
    }
    
    /* Close client connection */
    if (ipc_state.client_fd != -1) {
        close(ipc_state.client_fd);
        ipc_state.client_fd = -1;
    }
    
    /* Close server socket */
    if (ipc_state.server_fd != -1) {
        close(ipc_state.server_fd);
        ipc_state.server_fd = -1;
    }
    
    /* Remove socket file */
    if (strlen(ipc_state.socket_path) > 0) {
        unlink(ipc_state.socket_path);
        ipc_state.socket_path[0] = '\0';
    }
    
    ipc_state.initialized = false;
    printf("IPC event source cleaned up\n");
}