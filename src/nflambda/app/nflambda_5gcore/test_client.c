/*
 * Test client for NFLambda 5G Core
 * Sends a simple Registration Request to test the IPC interface
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "../../event_system/ipc_client.h"
#include "nas_ipc_protocol.h"

int main(int argc, char* argv[])
{
    printf("NFLambda 5G Core Test Client\n");
    printf("============================\n\n");
    
    /* Connect to 5G Core IPC server */
    printf("Connecting to NFLambda 5G Core...\n");
    int fd = ipc_client_connect("/tmp/nflambda_5gcore.sock");
    if (fd < 0) {
        fprintf(stderr, "Failed to connect to 5G Core\n");
        return 1;
    }
    printf("Connected successfully\n\n");
    
    /* Prepare Registration Request NAS PDU */
    const char* reg_req_hex = "7e004179000d0199f9070000000000000000102e0480f080f0";
    uint8_t nas_pdu[256];
    int nas_len = 0;
    
    /* Convert hex to binary */
    for (size_t i = 0; i < strlen(reg_req_hex) / 2; i++) {
        char byte_str[3] = {reg_req_hex[i*2], reg_req_hex[i*2+1], '\0'};
        nas_pdu[nas_len++] = (uint8_t)strtol(byte_str, NULL, 16);
    }
    
    /* Pack into IPC message */
    IpcMessage request;
    uint32_t trans_id = 1001;
    
    int result = nas_ipc_pack_message(&request, NAS_IPC_MSG_UPLINK, 
                                      NAS_IPC_EVT_REG_REQUEST, trans_id,
                                      nas_pdu, nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "Failed to pack NAS message: %s\n", 
                nas_ipc_error_to_string(result));
        ipc_client_close(fd);
        return 1;
    }
    
    printf("Sending Registration Request (trans_id=%u)...\n", trans_id);
    
    /* Send and receive response */
    char response[2048];
    size_t response_len = sizeof(response);
    
    result = ipc_client_send_recv(fd, request.data, request.length,
                                  response, &response_len);
    
    if (result < 0) {
        fprintf(stderr, "Communication failed\n");
        ipc_client_close(fd);
        return 1;
    }
    
    printf("Received response (%zu bytes)\n", response_len);
    
    /* Unpack response */
    IpcMessage resp_msg;
    resp_msg.length = response_len;
    memcpy(resp_msg.data, response, response_len);
    
    uint8_t msg_type;
    uint16_t event_type;
    uint32_t resp_trans_id;
    const uint8_t* resp_nas_pdu;
    uint16_t resp_nas_len;
    
    result = nas_ipc_unpack_message(&resp_msg, &msg_type, &event_type,
                                    &resp_trans_id, &resp_nas_pdu, &resp_nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "Failed to unpack response: %s\n",
                nas_ipc_error_to_string(result));
    } else {
        printf("\nResponse details:\n");
        printf("  Message type: %s\n", nas_ipc_msg_type_to_string(msg_type));
        printf("  Event type: %s\n", nas_ipc_event_type_to_string(event_type));
        printf("  Transaction ID: %u\n", resp_trans_id);
        printf("  NAS PDU length: %u\n", resp_nas_len);
        
        /* Print NAS PDU as hex */
        printf("  NAS PDU: ");
        for (int i = 0; i < resp_nas_len; i++) {
            printf("%02x", resp_nas_pdu[i]);
        }
        printf("\n");
    }
    
    /* Close connection */
    printf("\nClosing connection...\n");
    ipc_client_close(fd);
    
    printf("Test complete!\n");
    return 0;
}