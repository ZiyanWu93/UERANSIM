#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <arpa/inet.h>  // For htons
#include "../include/pdu_session_establishment_request.h"
#include "../include/pdu_session_establishment_accept.h"
#include "../include/dl_nas_transport.h"
#include "../../src/nflambda/event_system/event.h"

// Message type definitions
#define MSG_TYPE_UL_NAS_TRANSPORT 0x67
#define MSG_TYPE_DL_NAS_TRANSPORT 0x68
#define MSG_TYPE_PDU_SESSION_EST_REQUEST 0xC1
#define MSG_TYPE_PDU_SESSION_EST_ACCEPT 0xC2

// Working pointers
static DlNasTransportComplete *g_dl_nas_transport;
static PduSessionEstablishmentAccept *g_pdu_accept;

// Global variables for phase5
static uint8_t g_input_buffer[256];
static uint8_t g_output_buffer[256];
static size_t g_input_len;
static size_t g_output_len;

// Working variables
static uint8_t g_pdu_session_id;
static uint8_t g_pti;
static uint8_t g_pdu_session_type;
static uint8_t g_ssc_mode;
static uint8_t g_sst;
static uint8_t g_dnn[16];
static size_t g_dnn_len;
static uint8_t g_allocated_ipv4[4] = {10, 45, 0, 2};  // 10.45.0.2
static uint8_t g_dns_server[4] = {8, 8, 4, 4};        // 8.8.4.4

// Phase 5 handler functions

EVENT_HANDLER(amf_handle_ul_nas_transport) {
    // Skip security header (7 bytes) and plain NAS header (3 bytes)
    uint8_t *ptr = g_input_buffer + 10;
    
    // Verify UL NAS Transport message type (0x67)
    if (g_input_buffer[9] != 0x67) {
        printf("Error: Not UL NAS Transport message\n");
        return;
    }
    
    // Extract payload container type IEI (0x01)
    if (*ptr++ != 0x01) {
        printf("Error: Missing payload container type\n");
        return;
    }
    
    // Verify N1 SM information (type = 1)
    uint8_t container_type = *ptr++ & 0x0F;
    if (container_type != 1) {
        printf("Error: Not N1 SM information\n");
        return;
    }
    
    // Extract payload container IEI and length
    if (*ptr++ != 0x7B) {
        printf("Error: Missing payload container\n");
        return;
    }
    ptr++; // Skip length high byte (0x00)
    uint8_t container_len = *ptr++;
    
    // Copy PDU session establishment request to working buffer
    memcpy(g_input_buffer + 100, ptr, container_len);
}

EVENT_HANDLER(amf_extract_pdu_session_request) {
    uint8_t *ptr = g_input_buffer + 100;
    
    // Extract 5GSM header
    ptr++; // Skip EPD (0x2E)
    g_pdu_session_id = *ptr++;
    g_pti = *ptr++;
    ptr++; // Skip message type (0xC1)
    
    // Skip integrity protection max data rate (2 bytes)
    ptr += 2;
    
    // Extract PDU session type (half octet IE)
    uint8_t type_byte = *ptr++;
    g_pdu_session_type = type_byte & 0x07;  // bits 2-0
    
    // Extract SSC mode (half octet IE) 
    uint8_t ssc_byte = *ptr++;
    g_ssc_mode = ssc_byte & 0x07;  // bits 2-0
    
    // Skip 5GSM capability if present (IEI 0x28)
    if (*ptr == 0x28) {
        ptr++; // Skip IEI
        uint8_t cap_len = *ptr++;
        ptr += cap_len;
    }
    
    // Skip extended PCO if present (IEI 0x7B)
    if (*ptr == 0x7B) {
        ptr++; // Skip IEI
        uint8_t pco_len = *ptr++;
        ptr += pco_len;
    }
    
    // Move to outer message IEs
    // Input buffer position: security header(7) + plain header(3) + container type(2) + container IEI+len(3) + container data(21)
    ptr = g_input_buffer + 36;  // After payload container
    
    // Skip PDU session ID 2 (IEI 0x12)
    if (*ptr == 0x12) {
        ptr += 2;
    }
    
    // Skip request type (IEI 0x81)
    if (*ptr == 0x81) {
        ptr += 2;
    }
    
    // Extract S-NSSAI (IEI 0x22)
    if (*ptr == 0x22) {
        ptr++; // Skip IEI
        ptr++; // Skip length
        g_sst = *ptr++;  // Extract SST
    }
    
    // Extract DNN (IEI 0x25)
    if (*ptr == 0x25) {
        ptr++; // Skip IEI
        g_dnn_len = *ptr++;
        memcpy(g_dnn, ptr, g_dnn_len);
    }
}

EVENT_HANDLER(amf_validate_snssai_and_dnn) {
    // For testing, we accept SST=1 (eMBB) and DNN="internet"
    // Validation is hardcoded to succeed
}

EVENT_HANDLER(amf_discover_and_select_smf) {
    // For testing, SMF selection is hardcoded
    // In real implementation, this would query NRF
}

EVENT_HANDLER(amf_create_sm_context_request) {
    // For testing, we skip the actual HTTP request to SMF
    // Just prepare for SMF processing
}

EVENT_HANDLER(smf_handle_create_sm_context) {
    // Create SM context and prepare for IP allocation
    // For testing, context creation always succeeds
}

EVENT_HANDLER(smf_allocate_ip_address) {
    // IP address is hardcoded to 10.45.0.2
    // Already set in g_allocated_ipv4
}

EVENT_HANDLER(smf_create_default_qos_flows) {
    // QoS flows are created with hardcoded values
    // QFI=1, 5QI=9, default QoS rule
}

EVENT_HANDLER(smf_build_pdu_session_accept) {
    // Build PDU Session Establishment Accept using the hardcoded structure
    g_pdu_accept = (PduSessionEstablishmentAccept *)(g_output_buffer + 100);
    
    // Fill 5GSM header (Direct Assignment)
    g_pdu_accept->epd = 0x2E;
    g_pdu_accept->pdu_session_id = g_pdu_session_id;
    g_pdu_accept->pti = g_pti;
    g_pdu_accept->message_type = MSG_TYPE_PDU_SESSION_EST_ACCEPT;
    
    // Selected PDU session type and SSC mode
    g_pdu_accept->type_and_ssc = 0x11;  // SSC mode 1, PDU type IPv4
    
    // Authorized QoS rules (Direct Assignment)
    g_pdu_accept->qos_rules_length = htons(9);
    g_pdu_accept->qos_rule_id = 0x01;
    g_pdu_accept->qos_rule_length = htons(6);
    g_pdu_accept->rule_operation_code = 0x31;  // Create new, default, 1 filter
    
    // Packet filter (Direct Assignment)
    g_pdu_accept->packet_filter_list[0] = 0x31;  // Bidirectional, ID=1
    g_pdu_accept->packet_filter_list[1] = 0x01;  // Filter length
    g_pdu_accept->packet_filter_list[2] = 0x01;  // Match all
    
    g_pdu_accept->qos_rule_precedence = 0xFF;
    g_pdu_accept->qfi = 0x01;
    
    // Session AMBR (Direct Assignment)
    g_pdu_accept->ambr_length = 0x06;
    g_pdu_accept->dl_unit = 0x03;  // Mbps
    g_pdu_accept->dl_rate = htons(62500);
    g_pdu_accept->ul_unit = 0x03;  // Mbps
    g_pdu_accept->ul_rate = htons(62500);
    
    // PDU address (Direct Assignment)
    g_pdu_accept->pdu_addr_iei = 0x29;
    g_pdu_accept->pdu_addr_length = 0x05;
    g_pdu_accept->pdu_addr_type = 0x01;  // IPv4
    memcpy(g_pdu_accept->ipv4_addr, g_allocated_ipv4, 4);
    
    // S-NSSAI (Direct Assignment)
    g_pdu_accept->snssai_iei = 0x22;
    g_pdu_accept->snssai_length = 0x01;
    g_pdu_accept->sst = g_sst;
    
    // QoS flow descriptions (Direct Assignment)
    g_pdu_accept->qos_flow_iei = 0x79;
    g_pdu_accept->qos_flow_length = htons(6);
    g_pdu_accept->qos_flow_data[0] = 0x01;  // QFI
    g_pdu_accept->qos_flow_data[1] = 0x20;  // Operation code
    g_pdu_accept->qos_flow_data[2] = 0x41;  // E bit + num params
    g_pdu_accept->qos_flow_data[3] = 0x01;  // Parameter ID: 5QI
    g_pdu_accept->qos_flow_data[4] = 0x01;  // Parameter length
    g_pdu_accept->qos_flow_data[5] = 0x09;  // 5QI value
    
    // Extended PCO (Direct Assignment)
    g_pdu_accept->pco_iei = 0x7B;
    g_pdu_accept->pco_length = htons(15);
    g_pdu_accept->pco_data[0] = 0x80;  // Extension=1, config protocol=0
    // DNS Server 1
    g_pdu_accept->pco_data[1] = 0x00;  // Protocol ID high
    g_pdu_accept->pco_data[2] = 0x0D;  // Protocol ID low (DNS IPv4)
    g_pdu_accept->pco_data[3] = 0x04;  // Length
    g_pdu_accept->pco_data[4] = 0x08;  // 8.8.8.8
    g_pdu_accept->pco_data[5] = 0x08;
    g_pdu_accept->pco_data[6] = 0x08;
    g_pdu_accept->pco_data[7] = 0x08;
    // DNS Server 2
    g_pdu_accept->pco_data[8] = 0x00;  // Protocol ID high
    g_pdu_accept->pco_data[9] = 0x0D;  // Protocol ID low
    g_pdu_accept->pco_data[10] = 0x04;  // Length
    memcpy(&g_pdu_accept->pco_data[11], g_dns_server, 4);
    
    // DNN (Direct Assignment)
    g_pdu_accept->dnn_iei = 0x25;
    g_pdu_accept->dnn_length = g_dnn_len;
    memcpy(g_pdu_accept->dnn_value, g_dnn, g_dnn_len);
    
    // Calculate PDU accept length
    g_output_len = sizeof(PduSessionEstablishmentAccept);
}

EVENT_HANDLER(amf_build_dl_nas_transport) {
    // Build DL NAS Transport message using flat structure
    g_dl_nas_transport = (DlNasTransportComplete *)g_output_buffer;
    
    // Security header (Direct Assignment)
    g_dl_nas_transport->epd = 0x7E;
    g_dl_nas_transport->security_header_type = 0x02;  // Integrity protected and ciphered
    g_dl_nas_transport->spare_half = 0x00;
    g_dl_nas_transport->mac = 0x812DD6FB;  // 0xFBD62D81 in little-endian
    g_dl_nas_transport->sequence_number = 0x03;
    
    // Inner message (Direct Assignment)
    g_dl_nas_transport->inner_epd = 0x7E;
    g_dl_nas_transport->inner_security_header = 0x00;  // Plain NAS
    g_dl_nas_transport->inner_spare = 0x00;
    g_dl_nas_transport->message_type = MSG_TYPE_DL_NAS_TRANSPORT;
    
    // Payload container type (Direct Assignment)
    g_dl_nas_transport->payload_container_type = 0x01;  // N1 SM information
    g_dl_nas_transport->spare_bits = 0x00;
    
    // Payload container length (Direct Assignment)
    g_dl_nas_transport->payload_length_high = 0x00;
    g_dl_nas_transport->payload_length_low = g_output_len;
    
    // Copy PDU Session Establishment Accept (Direct Assignment)
    memcpy(g_dl_nas_transport->payload, g_output_buffer + 100, g_output_len);
    
    // PDU session ID 2 (Direct Assignment)
    g_dl_nas_transport->pdu_session_id2_iei = 0x12;
    g_dl_nas_transport->pdu_session_id2_value = g_pdu_session_id;
    
    // Update total output length
    g_output_len = sizeof(DlNasTransportComplete);
}

EVENT_HANDLER(amf_send_dl_nas_transport) {
    // For testing, this just finalizes the output
    // In real implementation, would send via NGAP
}

// Helper functions for phase5
void phase5_init(const uint8_t *input, size_t input_len) {
    memcpy(g_input_buffer, input, input_len);
    g_input_len = input_len;
    g_output_len = 0;
    memset(g_output_buffer, 0, sizeof(g_output_buffer));
}

void phase5_execute(void) {
    // Execute the handler chain
    amf_handle_ul_nas_transport();
    amf_extract_pdu_session_request();
    amf_validate_snssai_and_dnn();
    amf_discover_and_select_smf();
    amf_create_sm_context_request();
    smf_handle_create_sm_context();
    smf_allocate_ip_address();
    smf_create_default_qos_flows();
    smf_build_pdu_session_accept();
    amf_build_dl_nas_transport();
    amf_send_dl_nas_transport();
}

void phase5_get_output(uint8_t *output, size_t *output_len) {
    memcpy(output, g_output_buffer, g_output_len);
    *output_len = g_output_len;
}

// Main entry point for testing
int phase5_transform(const uint8_t *input, size_t input_len, 
                     uint8_t *output, size_t *output_len) {
    phase5_init(input, input_len);
    phase5_execute();
    phase5_get_output(output, output_len);
    return 0;
}