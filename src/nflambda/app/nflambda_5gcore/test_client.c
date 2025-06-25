/*
 * Test client for NFLambda 5G Core
 * Implements complete 5G registration flow with multiple messages
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../../event_system/ipc_client.h"
#include "nas_ipc_protocol.h"

/* Helper function to convert hex string to binary */
static int hex_to_binary(const char* hex, uint8_t* binary, size_t max_len)
{
    size_t hex_len = strlen(hex);
    if (hex_len % 2 != 0 || hex_len / 2 > max_len) {
        return -1;
    }
    
    for (size_t i = 0; i < hex_len / 2; i++) {
        char byte_str[3] = {hex[i*2], hex[i*2+1], '\0'};
        binary[i] = (uint8_t)strtol(byte_str, NULL, 16);
    }
    
    return hex_len / 2;
}

/* Helper function to print hex data */
static void print_hex(const uint8_t* data, int len)
{
    for (int i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
}

/* Send a message and receive response */
static int send_and_receive(int fd, uint8_t msg_type, uint16_t event_type,
                           uint32_t trans_id, const char* nas_hex,
                           const char* msg_name)
{
    printf("\n--- %s ---\n", msg_name);
    
    /* Convert hex to binary */
    uint8_t nas_pdu[256];
    int nas_len = hex_to_binary(nas_hex, nas_pdu, sizeof(nas_pdu));
    if (nas_len < 0) {
        fprintf(stderr, "Invalid hex string for %s\n", msg_name);
        return -1;
    }
    
    /* Pack into IPC message */
    IpcMessage request;
    int result = nas_ipc_pack_message(&request, msg_type, event_type,
                                      trans_id, nas_pdu, nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "Failed to pack %s: %s\n", 
                msg_name, nas_ipc_error_to_string(result));
        return -1;
    }
    
    printf("Sending %s (trans_id=%u, %d bytes)...\n", msg_name, trans_id, nas_len);
    printf("  NAS PDU: ");
    print_hex(nas_pdu, nas_len);
    printf("\n");
    
    /* Send and receive response */
    char response[2048];
    size_t response_len = sizeof(response);
    
    result = ipc_client_send_recv(fd, request.data, request.length,
                                  response, &response_len);
    
    if (result < 0) {
        fprintf(stderr, "Communication failed for %s\n", msg_name);
        return -1;
    }
    
    printf("Received response (%zu bytes)\n", response_len);
    
    /* Unpack response */
    IpcMessage resp_msg;
    resp_msg.length = response_len;
    memcpy(resp_msg.data, response, response_len);
    
    uint8_t resp_msg_type;
    uint16_t resp_event_type;
    uint32_t resp_trans_id;
    const uint8_t* resp_nas_pdu;
    uint16_t resp_nas_len;
    
    result = nas_ipc_unpack_message(&resp_msg, &resp_msg_type, &resp_event_type,
                                    &resp_trans_id, &resp_nas_pdu, &resp_nas_len);
    
    if (result != NAS_IPC_OK) {
        fprintf(stderr, "Failed to unpack response: %s\n",
                nas_ipc_error_to_string(result));
        return -1;
    }
    
    printf("Response details:\n");
    printf("  Message type: %s\n", nas_ipc_msg_type_to_string(resp_msg_type));
    printf("  Event type: %s\n", nas_ipc_event_type_to_string(resp_event_type));
    printf("  Transaction ID: %u\n", resp_trans_id);
    printf("  NAS PDU length: %u\n", resp_nas_len);
    printf("  NAS PDU: ");
    print_hex(resp_nas_pdu, resp_nas_len);
    printf("\n");
    
    return 0;
}

int main(int argc, char* argv[])
{
    printf("NFLambda 5G Core Test Client\n");
    printf("============================\n");
    printf("Complete 5G Registration Flow Test\n\n");
    
    /* Connect to 5G Core IPC server */
    printf("Connecting to NFLambda 5G Core...\n");
    int fd = ipc_client_connect("/tmp/nflambda_5gcore.sock");
    if (fd < 0) {
        fprintf(stderr, "Failed to connect to 5G Core\n");
        return 1;
    }
    printf("Connected successfully\n");
    
    int result = 0;
    uint32_t trans_id = 1000;
    
    /* Step 1: Registration Request */
    const char* reg_req_hex = "7e004179000d0199f9070000000000000000102e0480f080f0";
    result = send_and_receive(fd, NAS_IPC_MSG_UPLINK, NAS_IPC_EVT_REG_REQUEST,
                             ++trans_id, reg_req_hex, "Registration Request");
    if (result < 0) goto cleanup;
    
    /* Wait a bit between messages */
    usleep(100000); /* 100ms */
    
    /* Step 2: Authentication Response */
    const char* auth_resp_hex = "7e00572d01001021eaf66e3b8e64e8b335c0e5cb952307";
    result = send_and_receive(fd, NAS_IPC_MSG_UPLINK, NAS_IPC_EVT_AUTH_RESPONSE,
                             ++trans_id, auth_resp_hex, "Authentication Response");
    if (result < 0) goto cleanup;
    
    usleep(100000);
    
    /* Step 3: Security Mode Complete */
    const char* sec_complete_hex = "7e045e0300007e005e";
    result = send_and_receive(fd, NAS_IPC_MSG_UPLINK, NAS_IPC_EVT_SEC_MODE_COMP,
                             ++trans_id, sec_complete_hex, "Security Mode Complete");
    if (result < 0) goto cleanup;
    
    usleep(100000);
    
    /* Step 4: Registration Complete */
    const char* reg_complete_hex = "7e02430300007e0043";
    result = send_and_receive(fd, NAS_IPC_MSG_UPLINK, NAS_IPC_EVT_REG_COMPLETE,
                             ++trans_id, reg_complete_hex, "Registration Complete");
    if (result < 0) goto cleanup;
    
    usleep(100000);
    
    /* Step 5: PDU Session Establishment Request */
    const char* pdu_req_hex = "7e00670100420100c1ffff91a12801009110010101820001"
                              "8306696e7465726e657905010ac8100a";
    result = send_and_receive(fd, NAS_IPC_MSG_UPLINK, NAS_IPC_EVT_PDU_SESSION,
                             ++trans_id, pdu_req_hex, "PDU Session Request");
    if (result < 0) goto cleanup;
    
    printf("\n=== Registration Flow Complete ===\n");
    printf("Successfully completed all 5 steps of 5G registration!\n");
    
cleanup:
    /* Close connection */
    printf("\nClosing connection...\n");
    ipc_client_close(fd);
    
    if (result == 0) {
        printf("Test PASSED!\n");
    } else {
        printf("Test FAILED!\n");
    }
    
    return result == 0 ? 0 : 1;
}