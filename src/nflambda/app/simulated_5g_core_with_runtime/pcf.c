#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "pcf.h"
#include "end_to_end_events.h"
#include "../../event_system/event.h"
#include "../../runtime/runtime.h"

// PCF (Policy Control Function) Dispatcher
EVENT_HANDLER(pcf_dispatcher)
{
    printf("[PCF Dispatcher] Received event ID: %d\n", event_nf_ptr->event_id);
    printf("[PCF Dispatcher] Message length: %d bytes\n", event_nf_ptr->input_payload_length);
    
    if (event_nf_ptr->input_payload_length == 0) {
        printf("[PCF Dispatcher] Empty message, ignoring\n");
        return;
    }
    
    // Check for service chain request type
    uint8_t request_type = EVENT_PAYLOAD[0];
    
    if (request_type == REQ_TYPE_GET_CONFIG_DATA) {
        // Configuration data request from AMF
        printf("[PCF Dispatcher] Received configuration data request from AMF\n");
        // Trigger internal event to provide configuration
        trigger_event(EVENT_PCF_PROVIDE_CONFIG, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    if (request_type == REQ_TYPE_GET_PCC_RULES) {
        // PCC rules request from SMF
        printf("[PCF Dispatcher] Received PCC rules request from SMF\n");
        // Trigger internal event to provide PCC rules
        trigger_event(EVENT_PCF_PROVIDE_PCC, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
        return;
    }
    
    printf("[PCF Dispatcher] Unknown request type: 0x%02x\n", request_type);
}

// Provide network configuration data
EVENT_HANDLER(pcf_provide_config)
{
    printf("[PCF] Providing network configuration data\n");
    
    // Add full network name at bytes 11-27 (17 bytes)
    EVENT_PAYLOAD[11] = 0x43;  // Full network name IE
    EVENT_PAYLOAD[12] = 0x0f;  // Length
    EVENT_PAYLOAD[13] = 0x90;  // Coding scheme
    EVENT_PAYLOAD[14] = 0x00;  // 'O'
    EVENT_PAYLOAD[15] = 0x4f;
    EVENT_PAYLOAD[16] = 0x00;  // 'p'
    EVENT_PAYLOAD[17] = 0x70;
    EVENT_PAYLOAD[18] = 0x00;  // 'e'
    EVENT_PAYLOAD[19] = 0x65;
    EVENT_PAYLOAD[20] = 0x00;  // 'n'
    EVENT_PAYLOAD[21] = 0x6e;
    EVENT_PAYLOAD[22] = 0x00;  // '5'
    EVENT_PAYLOAD[23] = 0x35;
    EVENT_PAYLOAD[24] = 0x00;  // 'G'
    EVENT_PAYLOAD[25] = 0x47;
    EVENT_PAYLOAD[26] = 0x00;  // 'S'
    EVENT_PAYLOAD[27] = 0x53;
    
    // Add short network name at bytes 28-38 (11 bytes)
    EVENT_PAYLOAD[28] = 0x45;  // Short network name IE
    EVENT_PAYLOAD[29] = 0x09;  // Length
    EVENT_PAYLOAD[30] = 0x90;  // Coding scheme
    EVENT_PAYLOAD[31] = 0x00;  // 'N'
    EVENT_PAYLOAD[32] = 0x4e;
    EVENT_PAYLOAD[33] = 0x00;  // 'e'
    EVENT_PAYLOAD[34] = 0x65;
    EVENT_PAYLOAD[35] = 0x00;  // 'x'
    EVENT_PAYLOAD[36] = 0x78;
    EVENT_PAYLOAD[37] = 0x00;  // 't'
    EVENT_PAYLOAD[38] = 0x74;
    
    // Set response type and forward to AMF
    EVENT_PAYLOAD[0] = RESP_TYPE_CONFIG_DATA;
    event_nf_ptr->input_payload_length = 39; // 39 bytes with response type
    
    printf("[PCF] Configuration data ready, sending to AMF\n");
    trigger_event(EVENT_TO_AMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}


// Provide PCC rules
EVENT_HANDLER(pcf_provide_pcc)
{
    printf("[PCF] Providing PCC rules for PDU session\n");
    
    // Add extended protocol configuration options at bytes 56-72
    EVENT_PAYLOAD[56] = 0x7b; // Extended protocol configuration options IE
    EVENT_PAYLOAD[57] = 0x00; // Length (high)
    EVENT_PAYLOAD[58] = 0x0f; // Length (low)
    EVENT_PAYLOAD[59] = 0x80; // Configuration protocol
    
    // DNS server addresses
    EVENT_PAYLOAD[60] = 0x00; // Container ID (high)
    EVENT_PAYLOAD[61] = 0x0d; // Container ID (low) - DNS Server IPv4
    EVENT_PAYLOAD[62] = 0x04; // Container length
    EVENT_PAYLOAD[63] = 0x08; // DNS: 8.8.8.8
    EVENT_PAYLOAD[64] = 0x08;
    EVENT_PAYLOAD[65] = 0x08;
    EVENT_PAYLOAD[66] = 0x08;
    
    EVENT_PAYLOAD[67] = 0x00; // Container ID (high)
    EVENT_PAYLOAD[68] = 0x0d; // Container ID (low) - DNS Server IPv4
    EVENT_PAYLOAD[69] = 0x04; // Container length
    EVENT_PAYLOAD[70] = 0x08; // DNS: 8.8.4.4
    EVENT_PAYLOAD[71] = 0x08;
    EVENT_PAYLOAD[72] = 0x04;
    EVENT_PAYLOAD[73] = 0x04;
    
    // Set response type and forward to SMF
    EVENT_PAYLOAD[0] = RESP_TYPE_PCC_RULES;
    event_nf_ptr->input_payload_length = 74;
    
    printf("[PCF] PCC rules ready, sending to SMF\n");
    trigger_event(EVENT_TO_SMF, EVENT_PAYLOAD, (int)event_nf_ptr->input_payload_length);
}