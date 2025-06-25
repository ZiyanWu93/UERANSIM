#ifndef NAS_IPC_PROTOCOL_H
#define NAS_IPC_PROTOCOL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "../../event_system/ipc_event_source.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Protocol version */
#define NAS_IPC_PROTOCOL_VERSION    0x01

/* Message types */
#define NAS_IPC_MSG_UPLINK         0x01  /* UE → AMF */
#define NAS_IPC_MSG_DOWNLINK       0x02  /* AMF → UE */
#define NAS_IPC_MSG_ERROR          0xFF  /* Error response */

/* Event types - maps to UERANSIM times counter */
#define NAS_IPC_EVT_REG_REQUEST    0x0001  /* Registration Request (times=1) */
#define NAS_IPC_EVT_AUTH_RESPONSE  0x0002  /* Authentication Response (times=2) */
#define NAS_IPC_EVT_SEC_MODE_COMP  0x0003  /* Security Mode Complete (times=3) */
#define NAS_IPC_EVT_REG_COMPLETE   0x0004  /* Registration Complete (times=4) */
#define NAS_IPC_EVT_PDU_SESSION    0x0005  /* PDU Session Request (times=5) */

/* Error codes */
#define NAS_IPC_OK                 0x00    /* Success */
#define NAS_IPC_ERR_VERSION        0x01    /* Protocol version mismatch */
#define NAS_IPC_ERR_MSG_TYPE       0x02    /* Invalid message type */
#define NAS_IPC_ERR_PDU_SIZE       0x03    /* PDU too large */
#define NAS_IPC_ERR_INTERNAL       0x04    /* Internal processing error */
#define NAS_IPC_ERR_INVALID_FORMAT 0x05    /* Invalid message format */

/* Maximum NAS PDU size (2044 - protocol overhead) */
#define NAS_IPC_MAX_PDU_SIZE       2032

/* Protocol header offsets */
#define NAS_IPC_OFFSET_MSG_TYPE    0
#define NAS_IPC_OFFSET_VERSION     1
#define NAS_IPC_OFFSET_PDU_LEN     2
#define NAS_IPC_OFFSET_TRANS_ID    4
#define NAS_IPC_OFFSET_EVENT_TYPE  8
#define NAS_IPC_OFFSET_RESERVED    10
#define NAS_IPC_OFFSET_PDU_DATA    12

/* Protocol header size */
#define NAS_IPC_HEADER_SIZE        12

/**
 * Pack a NAS message into IPC format
 * 
 * @param ipc_msg       Output IPC message structure
 * @param msg_type      Message type (NAS_IPC_MSG_*)
 * @param event_type    Event type (NAS_IPC_EVT_*)
 * @param transaction_id Transaction ID for correlation
 * @param nas_pdu       NAS PDU data
 * @param nas_len       Length of NAS PDU
 * @return NAS_IPC_OK on success, error code otherwise
 */
int nas_ipc_pack_message(
    IpcMessage* ipc_msg,
    uint8_t msg_type,
    uint16_t event_type,
    uint32_t transaction_id,
    const uint8_t* nas_pdu,
    uint16_t nas_len
);

/**
 * Unpack an IPC message to extract NAS data
 * 
 * @param ipc_msg       Input IPC message
 * @param msg_type      Output message type
 * @param event_type    Output event type
 * @param transaction_id Output transaction ID
 * @param nas_pdu       Output pointer to NAS PDU (points into ipc_msg)
 * @param nas_len       Output NAS PDU length
 * @return NAS_IPC_OK on success, error code otherwise
 */
int nas_ipc_unpack_message(
    const IpcMessage* ipc_msg,
    uint8_t* msg_type,
    uint16_t* event_type,
    uint32_t* transaction_id,
    const uint8_t** nas_pdu,
    uint16_t* nas_len
);

/**
 * Validate an IPC message format
 * 
 * @param ipc_msg       IPC message to validate
 * @return NAS_IPC_OK if valid, error code otherwise
 */
int nas_ipc_validate_message(const IpcMessage* ipc_msg);

/**
 * Create an error response message
 * 
 * @param ipc_msg       Output IPC message
 * @param error_code    Error code to include
 * @param transaction_id Transaction ID from request
 * @return NAS_IPC_OK on success, error code otherwise
 */
int nas_ipc_create_error_response(
    IpcMessage* ipc_msg,
    uint8_t error_code,
    uint32_t transaction_id
);

/**
 * Get event type from UERANSIM times counter
 * 
 * @param times         UERANSIM times counter value
 * @return Event type or 0 if invalid
 */
uint16_t nas_ipc_times_to_event_type(int times);

/**
 * Get human-readable string for message type
 * 
 * @param msg_type      Message type
 * @return String representation
 */
const char* nas_ipc_msg_type_to_string(uint8_t msg_type);

/**
 * Get human-readable string for event type
 * 
 * @param event_type    Event type
 * @return String representation
 */
const char* nas_ipc_event_type_to_string(uint16_t event_type);

/**
 * Get human-readable string for error code
 * 
 * @param error_code    Error code
 * @return String representation
 */
const char* nas_ipc_error_to_string(uint8_t error_code);

#ifdef __cplusplus
}
#endif

#endif /* NAS_IPC_PROTOCOL_H */