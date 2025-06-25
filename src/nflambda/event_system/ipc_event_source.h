#ifndef IPC_EVENT_SOURCE_H
#define IPC_EVENT_SOURCE_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* IPC message structure - 2KB total */
typedef struct {
    uint32_t length;      /* Message length (excluding header) */
    uint8_t data[2044];   /* Payload data */
} IpcMessage;

/* Maximum payload size */
#define IPC_MAX_PAYLOAD_SIZE 2044

/* Default socket path */
#define IPC_DEFAULT_SOCKET_PATH "/tmp/nflambda_ipc.sock"

/**
 * Initialize the IPC event source
 * Creates a Unix domain socket server at the specified path
 * 
 * @param socket_path Path to the Unix domain socket (NULL for default)
 * @return 0 on success, -1 on error
 */
int ipc_event_source_init(const char* socket_path);

/**
 * Poll for IPC events (non-blocking)
 * This function should be registered as an event source with the runtime
 * It checks for new connections and incoming messages
 */
void ipc_event_source_poll(void);

/**
 * Send a response message to the connected client
 * 
 * @param data Pointer to the data to send
 * @param len Length of the data (must be <= IPC_MAX_PAYLOAD_SIZE)
 * @return Number of bytes sent, or -1 on error
 */
int ipc_send_response(const void* data, size_t len);

/**
 * Check if a client is currently connected
 * 
 * @return true if connected, false otherwise
 */
bool ipc_is_client_connected(void);

/**
 * Cleanup the IPC event source
 * Closes connections and removes the socket file
 */
void ipc_event_source_cleanup(void);

/**
 * Register IPC internal event handlers
 * Should be called during handler registration phase
 */
void ipc_register_handlers(void);

#ifdef __cplusplus
}
#endif

#endif /* IPC_EVENT_SOURCE_H */