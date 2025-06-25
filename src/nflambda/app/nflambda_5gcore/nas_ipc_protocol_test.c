#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include "nas_ipc_protocol.h"

/* Test counter for reporting */
static int tests_run = 0;
static int tests_passed = 0;

/* Helper macro for tests */
#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("Running %s... ", #name); \
    tests_run++; \
    test_##name(); \
    tests_passed++; \
    printf("PASSED\n"); \
} while(0)

/* Test packing a basic uplink message */
TEST(pack_uplink_message)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d};
    uint16_t nas_len = sizeof(nas_pdu);
    
    int result = nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        12345,
        nas_pdu,
        nas_len
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg.length == NAS_IPC_HEADER_SIZE + nas_len);
    assert(msg.data[NAS_IPC_OFFSET_MSG_TYPE] == NAS_IPC_MSG_UPLINK);
    assert(msg.data[NAS_IPC_OFFSET_VERSION] == NAS_IPC_PROTOCOL_VERSION);
    
    /* Check PDU length */
    uint16_t stored_len;
    memcpy(&stored_len, &msg.data[NAS_IPC_OFFSET_PDU_LEN], sizeof(uint16_t));
    assert(ntohs(stored_len) == nas_len);
    
    /* Check NAS PDU data */
    assert(memcmp(&msg.data[NAS_IPC_OFFSET_PDU_DATA], nas_pdu, nas_len) == 0);
}

/* Test unpacking a message */
TEST(unpack_message)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0x7e, 0x00, 0x56, 0x00, 0x02};
    uint16_t nas_len = sizeof(nas_pdu);
    
    /* First pack a message */
    nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_DOWNLINK,
        NAS_IPC_EVT_AUTH_RESPONSE,
        67890,
        nas_pdu,
        nas_len
    );
    
    /* Now unpack it */
    uint8_t msg_type;
    uint16_t event_type;
    uint32_t transaction_id;
    const uint8_t* unpacked_pdu;
    uint16_t unpacked_len;
    
    int result = nas_ipc_unpack_message(
        &msg,
        &msg_type,
        &event_type,
        &transaction_id,
        &unpacked_pdu,
        &unpacked_len
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg_type == NAS_IPC_MSG_DOWNLINK);
    assert(event_type == NAS_IPC_EVT_AUTH_RESPONSE);
    assert(transaction_id == 67890);
    assert(unpacked_len == nas_len);
    assert(memcmp(unpacked_pdu, nas_pdu, nas_len) == 0);
}

/* Test packing with maximum PDU size */
TEST(pack_max_pdu_size)
{
    IpcMessage msg;
    uint8_t nas_pdu[NAS_IPC_MAX_PDU_SIZE];
    
    /* Fill with test pattern */
    for (int i = 0; i < NAS_IPC_MAX_PDU_SIZE; i++) {
        nas_pdu[i] = (uint8_t)(i & 0xFF);
    }
    
    int result = nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_PDU_SESSION,
        999999,
        nas_pdu,
        NAS_IPC_MAX_PDU_SIZE
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg.length == NAS_IPC_HEADER_SIZE + NAS_IPC_MAX_PDU_SIZE);
}

/* Test packing with PDU too large */
TEST(pack_pdu_too_large)
{
    IpcMessage msg;
    uint8_t nas_pdu[NAS_IPC_MAX_PDU_SIZE + 1];
    
    int result = nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        1,
        nas_pdu,
        NAS_IPC_MAX_PDU_SIZE + 1
    );
    
    assert(result == NAS_IPC_ERR_PDU_SIZE);
}

/* Test packing with invalid message type */
TEST(pack_invalid_msg_type)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0x01, 0x02};
    
    int result = nas_ipc_pack_message(
        &msg,
        0x99,  /* Invalid message type */
        NAS_IPC_EVT_REG_REQUEST,
        1,
        nas_pdu,
        sizeof(nas_pdu)
    );
    
    assert(result == NAS_IPC_ERR_MSG_TYPE);
}

/* Test packing with null pointers */
TEST(pack_null_pointers)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0x01};
    
    /* Null message pointer */
    int result = nas_ipc_pack_message(
        NULL,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        1,
        nas_pdu,
        sizeof(nas_pdu)
    );
    assert(result == NAS_IPC_ERR_INVALID_FORMAT);
    
    /* Null PDU pointer */
    result = nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        1,
        NULL,
        1
    );
    assert(result == NAS_IPC_ERR_INVALID_FORMAT);
}

/* Test message validation */
TEST(validate_message)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0x01, 0x02, 0x03};
    
    /* Valid message */
    nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        1,
        nas_pdu,
        sizeof(nas_pdu)
    );
    assert(nas_ipc_validate_message(&msg) == NAS_IPC_OK);
    
    /* Invalid version */
    msg.data[NAS_IPC_OFFSET_VERSION] = 0xFF;
    assert(nas_ipc_validate_message(&msg) == NAS_IPC_ERR_VERSION);
    msg.data[NAS_IPC_OFFSET_VERSION] = NAS_IPC_PROTOCOL_VERSION;
    
    /* Invalid message type */
    msg.data[NAS_IPC_OFFSET_MSG_TYPE] = 0x99;
    assert(nas_ipc_validate_message(&msg) == NAS_IPC_ERR_MSG_TYPE);
    msg.data[NAS_IPC_OFFSET_MSG_TYPE] = NAS_IPC_MSG_UPLINK;
    
    /* Length mismatch */
    msg.length = 5;
    assert(nas_ipc_validate_message(&msg) == NAS_IPC_ERR_INVALID_FORMAT);
}

/* Test error response creation */
TEST(create_error_response)
{
    IpcMessage msg;
    
    int result = nas_ipc_create_error_response(
        &msg,
        NAS_IPC_ERR_PDU_SIZE,
        12345
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg.data[NAS_IPC_OFFSET_MSG_TYPE] == NAS_IPC_MSG_ERROR);
    assert(msg.data[NAS_IPC_OFFSET_VERSION] == NAS_IPC_PROTOCOL_VERSION);
    assert(msg.length == NAS_IPC_HEADER_SIZE + 1);
    assert(msg.data[NAS_IPC_OFFSET_PDU_DATA] == NAS_IPC_ERR_PDU_SIZE);
    
    /* Verify transaction ID */
    uint32_t trans_id;
    memcpy(&trans_id, &msg.data[NAS_IPC_OFFSET_TRANS_ID], sizeof(uint32_t));
    assert(ntohl(trans_id) == 12345);
}

/* Test times to event type mapping */
TEST(times_to_event_type)
{
    assert(nas_ipc_times_to_event_type(1) == NAS_IPC_EVT_REG_REQUEST);
    assert(nas_ipc_times_to_event_type(2) == NAS_IPC_EVT_AUTH_RESPONSE);
    assert(nas_ipc_times_to_event_type(3) == NAS_IPC_EVT_SEC_MODE_COMP);
    assert(nas_ipc_times_to_event_type(4) == NAS_IPC_EVT_REG_COMPLETE);
    assert(nas_ipc_times_to_event_type(5) == NAS_IPC_EVT_PDU_SESSION);
    assert(nas_ipc_times_to_event_type(0) == 0);
    assert(nas_ipc_times_to_event_type(6) == 0);
    assert(nas_ipc_times_to_event_type(-1) == 0);
}

/* Test string conversion functions */
TEST(string_conversions)
{
    /* Message type strings */
    assert(strcmp(nas_ipc_msg_type_to_string(NAS_IPC_MSG_UPLINK), "UPLINK") == 0);
    assert(strcmp(nas_ipc_msg_type_to_string(NAS_IPC_MSG_DOWNLINK), "DOWNLINK") == 0);
    assert(strcmp(nas_ipc_msg_type_to_string(NAS_IPC_MSG_ERROR), "ERROR") == 0);
    assert(strcmp(nas_ipc_msg_type_to_string(0x99), "UNKNOWN") == 0);
    
    /* Event type strings */
    assert(strcmp(nas_ipc_event_type_to_string(NAS_IPC_EVT_REG_REQUEST), "REGISTRATION_REQUEST") == 0);
    assert(strcmp(nas_ipc_event_type_to_string(NAS_IPC_EVT_AUTH_RESPONSE), "AUTHENTICATION_RESPONSE") == 0);
    assert(strcmp(nas_ipc_event_type_to_string(0x9999), "UNKNOWN") == 0);
    
    /* Error strings */
    assert(strcmp(nas_ipc_error_to_string(NAS_IPC_OK), "OK") == 0);
    assert(strcmp(nas_ipc_error_to_string(NAS_IPC_ERR_VERSION), "VERSION_MISMATCH") == 0);
    assert(strcmp(nas_ipc_error_to_string(0x99), "UNKNOWN_ERROR") == 0);
}

/* Test packing with zero-length PDU */
TEST(pack_zero_length_pdu)
{
    IpcMessage msg;
    uint8_t dummy_pdu = 0;
    
    int result = nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        NAS_IPC_EVT_REG_REQUEST,
        1,
        &dummy_pdu,
        0
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg.length == NAS_IPC_HEADER_SIZE);
    
    /* Verify PDU length is 0 */
    uint16_t stored_len;
    memcpy(&stored_len, &msg.data[NAS_IPC_OFFSET_PDU_LEN], sizeof(uint16_t));
    assert(ntohs(stored_len) == 0);
}

/* Test round-trip packing and unpacking */
TEST(round_trip)
{
    IpcMessage msg;
    uint8_t original_pdu[] = {0x7e, 0x00, 0x41, 0x79, 0x00, 0x0d, 0x01, 0x99, 0xf9, 0x07};
    uint16_t original_len = sizeof(original_pdu);
    uint8_t original_msg_type = NAS_IPC_MSG_UPLINK;
    uint16_t original_event_type = NAS_IPC_EVT_REG_REQUEST;
    uint32_t original_trans_id = 0xDEADBEEF;
    
    /* Pack */
    int result = nas_ipc_pack_message(
        &msg,
        original_msg_type,
        original_event_type,
        original_trans_id,
        original_pdu,
        original_len
    );
    assert(result == NAS_IPC_OK);
    
    /* Unpack */
    uint8_t msg_type;
    uint16_t event_type;
    uint32_t transaction_id;
    const uint8_t* unpacked_pdu;
    uint16_t unpacked_len;
    
    result = nas_ipc_unpack_message(
        &msg,
        &msg_type,
        &event_type,
        &transaction_id,
        &unpacked_pdu,
        &unpacked_len
    );
    
    assert(result == NAS_IPC_OK);
    assert(msg_type == original_msg_type);
    assert(event_type == original_event_type);
    assert(transaction_id == original_trans_id);
    assert(unpacked_len == original_len);
    assert(memcmp(unpacked_pdu, original_pdu, original_len) == 0);
}

/* Test endianness handling */
TEST(endianness)
{
    IpcMessage msg;
    uint8_t nas_pdu[] = {0xAB, 0xCD};
    
    /* Pack with specific values */
    nas_ipc_pack_message(
        &msg,
        NAS_IPC_MSG_UPLINK,
        0x1234,  /* Event type */
        0x12345678,  /* Transaction ID */
        nas_pdu,
        sizeof(nas_pdu)
    );
    
    /* Manually check byte order */
    /* PDU length (2) should be 0x00 0x02 in network byte order */
    assert(msg.data[NAS_IPC_OFFSET_PDU_LEN] == 0x00);
    assert(msg.data[NAS_IPC_OFFSET_PDU_LEN + 1] == 0x02);
    
    /* Transaction ID should be 0x12 0x34 0x56 0x78 in network byte order */
    assert(msg.data[NAS_IPC_OFFSET_TRANS_ID] == 0x12);
    assert(msg.data[NAS_IPC_OFFSET_TRANS_ID + 1] == 0x34);
    assert(msg.data[NAS_IPC_OFFSET_TRANS_ID + 2] == 0x56);
    assert(msg.data[NAS_IPC_OFFSET_TRANS_ID + 3] == 0x78);
    
    /* Event type should be 0x12 0x34 in network byte order */
    assert(msg.data[NAS_IPC_OFFSET_EVENT_TYPE] == 0x12);
    assert(msg.data[NAS_IPC_OFFSET_EVENT_TYPE + 1] == 0x34);
}

/* Main test runner */
int main(void)
{
    printf("=== NAS IPC Protocol Unit Tests ===\n\n");
    
    RUN_TEST(pack_uplink_message);
    RUN_TEST(unpack_message);
    RUN_TEST(pack_max_pdu_size);
    RUN_TEST(pack_pdu_too_large);
    RUN_TEST(pack_invalid_msg_type);
    RUN_TEST(pack_null_pointers);
    RUN_TEST(validate_message);
    RUN_TEST(create_error_response);
    RUN_TEST(times_to_event_type);
    RUN_TEST(string_conversions);
    RUN_TEST(pack_zero_length_pdu);
    RUN_TEST(round_trip);
    RUN_TEST(endianness);
    
    printf("\n=== Test Summary ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    if (tests_run == tests_passed) {
        printf("\nALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\nSOME TESTS FAILED!\n");
        return 1;
    }
}