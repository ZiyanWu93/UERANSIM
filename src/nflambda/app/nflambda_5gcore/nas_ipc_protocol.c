#include "nas_ipc_protocol.h"
#include <string.h>
#include <arpa/inet.h>  /* For htons/ntohs */

/* Pack a NAS message into IPC format */
int nas_ipc_pack_message(
    IpcMessage* ipc_msg,
    uint8_t msg_type,
    uint16_t event_type,
    uint32_t transaction_id,
    const uint8_t* nas_pdu,
    uint16_t nas_len)
{
    /* Validate inputs */
    if (!ipc_msg || !nas_pdu) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Check PDU size */
    if (nas_len > NAS_IPC_MAX_PDU_SIZE) {
        return NAS_IPC_ERR_PDU_SIZE;
    }
    
    /* Validate message type */
    if (msg_type != NAS_IPC_MSG_UPLINK && 
        msg_type != NAS_IPC_MSG_DOWNLINK && 
        msg_type != NAS_IPC_MSG_ERROR) {
        return NAS_IPC_ERR_MSG_TYPE;
    }
    
    /* Clear the message */
    memset(ipc_msg, 0, sizeof(IpcMessage));
    
    /* Pack header fields */
    ipc_msg->data[NAS_IPC_OFFSET_MSG_TYPE] = msg_type;
    ipc_msg->data[NAS_IPC_OFFSET_VERSION] = NAS_IPC_PROTOCOL_VERSION;
    
    /* Pack PDU length in network byte order */
    uint16_t nas_len_network = htons(nas_len);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_PDU_LEN], &nas_len_network, sizeof(uint16_t));
    
    /* Pack transaction ID in network byte order */
    uint32_t trans_id_network = htonl(transaction_id);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_TRANS_ID], &trans_id_network, sizeof(uint32_t));
    
    /* Pack event type in network byte order */
    uint16_t event_type_network = htons(event_type);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_EVENT_TYPE], &event_type_network, sizeof(uint16_t));
    
    /* Reserved field is already zero from memset */
    
    /* Copy NAS PDU data */
    if (nas_len > 0) {
        memcpy(&ipc_msg->data[NAS_IPC_OFFSET_PDU_DATA], nas_pdu, nas_len);
    }
    
    /* Set total message length */
    ipc_msg->length = NAS_IPC_HEADER_SIZE + nas_len;
    
    return NAS_IPC_OK;
}

/* Unpack an IPC message to extract NAS data */
int nas_ipc_unpack_message(
    const IpcMessage* ipc_msg,
    uint8_t* msg_type,
    uint16_t* event_type,
    uint32_t* transaction_id,
    const uint8_t** nas_pdu,
    uint16_t* nas_len)
{
    /* Validate inputs */
    if (!ipc_msg || !msg_type || !event_type || !transaction_id || !nas_pdu || !nas_len) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Validate message first */
    int result = nas_ipc_validate_message(ipc_msg);
    if (result != NAS_IPC_OK) {
        return result;
    }
    
    /* Extract header fields */
    *msg_type = ipc_msg->data[NAS_IPC_OFFSET_MSG_TYPE];
    
    /* Extract PDU length from network byte order */
    uint16_t nas_len_network;
    memcpy(&nas_len_network, &ipc_msg->data[NAS_IPC_OFFSET_PDU_LEN], sizeof(uint16_t));
    *nas_len = ntohs(nas_len_network);
    
    /* Extract transaction ID from network byte order */
    uint32_t trans_id_network;
    memcpy(&trans_id_network, &ipc_msg->data[NAS_IPC_OFFSET_TRANS_ID], sizeof(uint32_t));
    *transaction_id = ntohl(trans_id_network);
    
    /* Extract event type from network byte order */
    uint16_t event_type_network;
    memcpy(&event_type_network, &ipc_msg->data[NAS_IPC_OFFSET_EVENT_TYPE], sizeof(uint16_t));
    *event_type = ntohs(event_type_network);
    
    /* Set pointer to NAS PDU data */
    *nas_pdu = &ipc_msg->data[NAS_IPC_OFFSET_PDU_DATA];
    
    return NAS_IPC_OK;
}

/* Validate an IPC message format */
int nas_ipc_validate_message(const IpcMessage* ipc_msg)
{
    if (!ipc_msg) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Check minimum length */
    if (ipc_msg->length < NAS_IPC_HEADER_SIZE) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Check protocol version */
    if (ipc_msg->data[NAS_IPC_OFFSET_VERSION] != NAS_IPC_PROTOCOL_VERSION) {
        return NAS_IPC_ERR_VERSION;
    }
    
    /* Check message type */
    uint8_t msg_type = ipc_msg->data[NAS_IPC_OFFSET_MSG_TYPE];
    if (msg_type != NAS_IPC_MSG_UPLINK && 
        msg_type != NAS_IPC_MSG_DOWNLINK && 
        msg_type != NAS_IPC_MSG_ERROR) {
        return NAS_IPC_ERR_MSG_TYPE;
    }
    
    /* Extract and validate PDU length */
    uint16_t nas_len_network;
    memcpy(&nas_len_network, &ipc_msg->data[NAS_IPC_OFFSET_PDU_LEN], sizeof(uint16_t));
    uint16_t nas_len = ntohs(nas_len_network);
    
    /* Check if PDU length matches message length */
    if (ipc_msg->length != NAS_IPC_HEADER_SIZE + nas_len) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Check if PDU length is within bounds */
    if (nas_len > NAS_IPC_MAX_PDU_SIZE) {
        return NAS_IPC_ERR_PDU_SIZE;
    }
    
    return NAS_IPC_OK;
}

/* Create an error response message */
int nas_ipc_create_error_response(
    IpcMessage* ipc_msg,
    uint8_t error_code,
    uint32_t transaction_id)
{
    if (!ipc_msg) {
        return NAS_IPC_ERR_INVALID_FORMAT;
    }
    
    /* Clear the message */
    memset(ipc_msg, 0, sizeof(IpcMessage));
    
    /* Set header fields */
    ipc_msg->data[NAS_IPC_OFFSET_MSG_TYPE] = NAS_IPC_MSG_ERROR;
    ipc_msg->data[NAS_IPC_OFFSET_VERSION] = NAS_IPC_PROTOCOL_VERSION;
    
    /* Set PDU length to 1 (just the error code) */
    uint16_t nas_len_network = htons(1);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_PDU_LEN], &nas_len_network, sizeof(uint16_t));
    
    /* Set transaction ID */
    uint32_t trans_id_network = htonl(transaction_id);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_TRANS_ID], &trans_id_network, sizeof(uint32_t));
    
    /* Event type is 0 for error messages */
    uint16_t event_type_network = htons(0);
    memcpy(&ipc_msg->data[NAS_IPC_OFFSET_EVENT_TYPE], &event_type_network, sizeof(uint16_t));
    
    /* Store error code as the PDU data */
    ipc_msg->data[NAS_IPC_OFFSET_PDU_DATA] = error_code;
    
    /* Set total message length */
    ipc_msg->length = NAS_IPC_HEADER_SIZE + 1;
    
    return NAS_IPC_OK;
}

/* Get event type from UERANSIM times counter */
uint16_t nas_ipc_times_to_event_type(int times)
{
    switch (times) {
        case 1:
            return NAS_IPC_EVT_REG_REQUEST;
        case 2:
            return NAS_IPC_EVT_AUTH_RESPONSE;
        case 3:
            return NAS_IPC_EVT_SEC_MODE_COMP;
        case 4:
            return NAS_IPC_EVT_REG_COMPLETE;
        case 5:
            return NAS_IPC_EVT_PDU_SESSION;
        default:
            return 0;  /* Invalid */
    }
}

/* Get human-readable string for message type */
const char* nas_ipc_msg_type_to_string(uint8_t msg_type)
{
    switch (msg_type) {
        case NAS_IPC_MSG_UPLINK:
            return "UPLINK";
        case NAS_IPC_MSG_DOWNLINK:
            return "DOWNLINK";
        case NAS_IPC_MSG_ERROR:
            return "ERROR";
        default:
            return "UNKNOWN";
    }
}

/* Get human-readable string for event type */
const char* nas_ipc_event_type_to_string(uint16_t event_type)
{
    switch (event_type) {
        case NAS_IPC_EVT_REG_REQUEST:
            return "REGISTRATION_REQUEST";
        case NAS_IPC_EVT_AUTH_RESPONSE:
            return "AUTHENTICATION_RESPONSE";
        case NAS_IPC_EVT_SEC_MODE_COMP:
            return "SECURITY_MODE_COMPLETE";
        case NAS_IPC_EVT_REG_COMPLETE:
            return "REGISTRATION_COMPLETE";
        case NAS_IPC_EVT_PDU_SESSION:
            return "PDU_SESSION_REQUEST";
        default:
            return "UNKNOWN";
    }
}

/* Get human-readable string for error code */
const char* nas_ipc_error_to_string(uint8_t error_code)
{
    switch (error_code) {
        case NAS_IPC_OK:
            return "OK";
        case NAS_IPC_ERR_VERSION:
            return "VERSION_MISMATCH";
        case NAS_IPC_ERR_MSG_TYPE:
            return "INVALID_MESSAGE_TYPE";
        case NAS_IPC_ERR_PDU_SIZE:
            return "PDU_TOO_LARGE";
        case NAS_IPC_ERR_INTERNAL:
            return "INTERNAL_ERROR";
        case NAS_IPC_ERR_INVALID_FORMAT:
            return "INVALID_FORMAT";
        default:
            return "UNKNOWN_ERROR";
    }
}