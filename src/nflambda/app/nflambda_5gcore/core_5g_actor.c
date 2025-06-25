#include "core_5g_actor.h"
#include "amf_handlers.h"
#include "ue_state.h"
#include "nas_ipc_protocol.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Actor state */
static bool stopped = false;

/* Initialize the Core 5G actor */
void core_5g_init(void)
{
    printf("[Core 5G] Actor initialized\n");
    stopped = false;
    ue_state_init();
}

/* Main IPC message handler */
EVENT_HANDLER(handle_ipc_nas_message)
{
    const uint8_t* raw_message = (const uint8_t*)EVENT_PAYLOAD;
    int message_length = event_nf_ptr->input_payload_length;
    printf("[Core 5G] Received IPC message (%d bytes)\n", message_length);
    
    /* Validate minimum message size */
    if (message_length < sizeof(uint32_t)) {
        fprintf(stderr, "[Core 5G] IPC message too small\n");
        return;
    }
    
    /* Copy the IPC message */
    IpcMessage ipc_msg;
    memcpy(&ipc_msg, raw_message, message_length);
    
    /* Validate the length */
    if (ipc_msg.length > IPC_MAX_PAYLOAD_SIZE) {
        fprintf(stderr, "[Core 5G] IPC message too large: %u\n", ipc_msg.length);
        return;
    }
    
    /* Unpack the NAS message */
    uint8_t msg_type;
    uint16_t event_type;
    uint32_t transaction_id;
    const uint8_t* nas_pdu;
    uint16_t nas_len;
    
    int result = nas_ipc_unpack_message(&ipc_msg, &msg_type, &event_type, 
                                        &transaction_id, &nas_pdu, &nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "[Core 5G] Failed to unpack NAS message: %s\n", 
                nas_ipc_error_to_string(result));
        /* Trigger error response */
        IpcMessage error_msg;
        nas_ipc_create_error_response(&error_msg, result, transaction_id);
        /* Send full IPC message including length header */
        trigger_event(EVENT_IPC_SEND_RESPONSE, &error_msg, sizeof(error_msg.length) + error_msg.length);
        return;
    }
    
    printf("[Core 5G] Unpacked NAS message: type=%s, event=%s, trans_id=%u, nas_len=%u\n",
           nas_ipc_msg_type_to_string(msg_type),
           nas_ipc_event_type_to_string(event_type),
           transaction_id, nas_len);
    
    /* Only process uplink messages */
    if (msg_type != NAS_IPC_MSG_UPLINK) {
        fprintf(stderr, "[Core 5G] Unexpected message type: %u\n", msg_type);
        return;
    }
    
    /* Store transaction ID for response correlation */
    ue_state_set_transaction_id(transaction_id);
    
    /* Trigger appropriate NAS event based on event type */
    switch (event_type) {
        case NAS_IPC_EVT_REG_REQUEST:
            printf("[Core 5G] Triggering registration request event\n");
            trigger_event(EVENT_NAS_REGISTRATION_REQUEST, nas_pdu, nas_len);
            break;
            
        case NAS_IPC_EVT_AUTH_RESPONSE:
            printf("[Core 5G] Triggering auth response event\n");
            trigger_event(EVENT_NAS_AUTH_RESPONSE, nas_pdu, nas_len);
            break;
            
        case NAS_IPC_EVT_SEC_MODE_COMP:
            printf("[Core 5G] Triggering security mode complete event\n");
            trigger_event(EVENT_NAS_SECURITY_MODE_COMPLETE, nas_pdu, nas_len);
            break;
            
        case NAS_IPC_EVT_REG_COMPLETE:
            printf("[Core 5G] Triggering registration complete event\n");
            trigger_event(EVENT_NAS_REGISTRATION_COMPLETE, nas_pdu, nas_len);
            break;
            
        case NAS_IPC_EVT_PDU_SESSION:
            printf("[Core 5G] Triggering PDU session request event\n");
            trigger_event(EVENT_NAS_PDU_SESSION_REQUEST, nas_pdu, nas_len);
            break;
            
        default:
            fprintf(stderr, "[Core 5G] Unknown event type: %u\n", event_type);
            break;
    }
}


/* Register all Core 5G event handlers */
void core_5g_register_handlers(void)
{
    printf("[Core 5G] Registering event handlers\n");
    
    /* Register IPC handlers */
    register_event_handler(EVENT_IPC_MESSAGE_RECEIVED, handle_ipc_nas_message);
    
    /* Register AMF NAS handlers */
    amf_register_handlers();
    
    /* Register IPC internal handlers for actual response sending */
    ipc_register_handlers();
}

/* Check if the Core 5G actor has stopped */
bool core_5g_is_stopped(void)
{
    return stopped;
}

/* Stop the Core 5G actor */
void core_5g_stop(void)
{
    printf("[Core 5G] Stopping actor\n");
    stopped = true;
}