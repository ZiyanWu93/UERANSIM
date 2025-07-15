#include "fiveg_core_actor.h"
#include "amf_common.h"
#include "../../runtime/runtime.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

// Actor state
static ue_state_t ue_state = UE_STATE_DEREGISTERED;
static bool stopped = false;
static int sequence_number = 0;

// Initialize the 5G Core actor
void fiveg_core_init(void)
{
    printf("[5G Core] Actor initialized\n");
    ue_state = UE_STATE_DEREGISTERED;
    stopped = false;
    sequence_number = 0;
}

// Utility function to print NAS PDU
void print_nas_pdu(const char* label, const char* pdu)
{
    printf("[5G Core] %s: %s\n", label, pdu);
}

// Handler for Registration Request
EVENT_HANDLER(handle_registration_request)
{
    printf("[5G Core] Received Registration Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Authentication Request
    uint8_t epd_header[] = {0x7e, 0x00};
    uint8_t msg_type[] = {0x56};
    uint8_t ngksi_field[] = {0x00};
    uint8_t abba_ie[] = {0x02, 0x00, 0x00};
    uint8_t rand_iei[] = {0x21};
    uint8_t rand_val[] = {0x5c, 0xa0, 0xdf, 0x8c, 0x9b, 0xb8, 0xdb, 0xcf, 0x3c, 0x2a, 0x7d, 0xd4, 0x48, 0xda, 0x13, 0x69};
    uint8_t autn_iei[] = {0x20};
    uint8_t autn_len[] = {0x10};
    uint8_t autn_val[] = {0x40, 0x62, 0x96, 0x99, 0x30, 0x82, 0x80, 0x00, 0x30, 0xb7, 0x62, 0x45, 0x5c, 0x89, 0x0b, 0x19};

    struct { uint8_t *data; size_t len; } parts[] = {
        {epd_header, sizeof(epd_header)},
        {msg_type, sizeof(msg_type)},
        {ngksi_field, sizeof(ngksi_field)},
        {abba_ie, sizeof(abba_ie)},
        {rand_iei, sizeof(rand_iei)},
        {rand_val, sizeof(rand_val)},
        {autn_iei, sizeof(autn_iei)},
        {autn_len, sizeof(autn_len)},
        {autn_val, sizeof(autn_val)}
    };

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        if (offset + parts[i].len * 2 >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        for (size_t j = 0; j < parts[i].len; ++j)
        {
            sprintf(EVENT_PAYLOAD + offset, "%02x", parts[i].data[j]);
            offset += 2;
        }
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Auth Request", EVENT_PAYLOAD);
    ue_state = UE_STATE_REGISTERING;
    
    // Trigger authentication request event
    trigger_event(EVENT_AUTH_REQUEST, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Authentication Response
EVENT_HANDLER(handle_auth_response)
{
    printf("[5G Core] Received Authentication Response\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Security Mode Command
    uint8_t outer_header[] = {0x7e, 0x03};
    uint8_t mac[] = {0x13, 0xbf, 0x99, 0x5a};
    uint8_t seq_num[] = {0x00};
    uint8_t inner_header[] = {0x7e, 0x00};
    uint8_t msg_type[] = {0x5d};
    uint8_t nas_alg[] = {0x02};
    uint8_t ngksi_field[] = {0x00};
    uint8_t ue_sec_cap_len[] = {0x04};
    uint8_t ue_sec_cap_val[] = {0x80, 0xf0, 0x80, 0xf0};
    uint8_t imeisv_req[] = {0xe1};
    uint8_t add_sec_info[] = {0x36, 0x01, 0x02};

    struct { uint8_t *data; size_t len; } parts[] = {
        {outer_header, sizeof(outer_header)},
        {mac, sizeof(mac)},
        {seq_num, sizeof(seq_num)},
        {inner_header, sizeof(inner_header)},
        {msg_type, sizeof(msg_type)},
        {nas_alg, sizeof(nas_alg)},
        {ngksi_field, sizeof(ngksi_field)},
        {ue_sec_cap_len, sizeof(ue_sec_cap_len)},
        {ue_sec_cap_val, sizeof(ue_sec_cap_val)},
        {imeisv_req, sizeof(imeisv_req)},
        {add_sec_info, sizeof(add_sec_info)}
    };

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        if (offset + parts[i].len * 2 >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        for (size_t j = 0; j < parts[i].len; ++j)
        {
            sprintf(EVENT_PAYLOAD + offset, "%02x", parts[i].data[j]);
            offset += 2;
        }
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Security Mode Command", EVENT_PAYLOAD);
    ue_state = UE_STATE_AUTHENTICATED;
    sequence_number++;
    
    // Trigger security mode command event
    trigger_event(EVENT_SECURITY_MODE_COMMAND, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Security Mode Complete
EVENT_HANDLER(handle_security_mode_complete)
{
    printf("[5G Core] Received Security Mode Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Registration Accept
    uint8_t outer_header[] = {0x7e, 0x02};
    uint8_t mac[] = {0x72, 0x39, 0x67, 0x4c};
    uint8_t seq_num[] = {0x01};
    uint8_t inner_header[] = {0x7e, 0x00};
    uint8_t msg_type[] = {0x42};
    uint8_t reg_result[] = {0x01, 0x01};
    uint8_t guti_ie[] = {0x77, 0x00, 0x0b, 0xf2, 0x99, 0xf9, 0x07, 0x02, 0x00, 0x40, 0xc0, 0x00, 0x07, 0x27};
    uint8_t tai_list[] = {0x54, 0x07, 0x40, 0x99, 0xf9, 0x07, 0x00, 0x00, 0x01};
    uint8_t nssai[] = {0x15, 0x02, 0x01, 0x01};
    uint8_t net_feat[] = {0x21, 0x02, 0x01, 0x00};
    uint8_t gprs_timer[] = {0x5e, 0x01, 0x92};

    struct { uint8_t *data; size_t len; } parts[] = {
        {outer_header, sizeof(outer_header)},
        {mac, sizeof(mac)},
        {seq_num, sizeof(seq_num)},
        {inner_header, sizeof(inner_header)},
        {msg_type, sizeof(msg_type)},
        {reg_result, sizeof(reg_result)},
        {guti_ie, sizeof(guti_ie)},
        {tai_list, sizeof(tai_list)},
        {nssai, sizeof(nssai)},
        {net_feat, sizeof(net_feat)},
        {gprs_timer, sizeof(gprs_timer)}
    };

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        if (offset + parts[i].len * 2 >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        for (size_t j = 0; j < parts[i].len; ++j)
        {
            sprintf(EVENT_PAYLOAD + offset, "%02x", parts[i].data[j]);
            offset += 2;
        }
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Registration Accept", EVENT_PAYLOAD);
    ue_state = UE_STATE_SECURITY_ESTABLISHED;
    sequence_number++;
    
    // Trigger registration accept event
    trigger_event(EVENT_REGISTRATION_ACCEPT, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for Registration Complete
EVENT_HANDLER(handle_registration_complete)
{
    printf("[5G Core] Received Registration Complete\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate Configuration Update Command
    uint8_t outer_header[] = {0x7e, 0x02};
    uint8_t mac[] = {0xde, 0x0d, 0x22, 0xe3};
    uint8_t seq_num[] = {0x02};
    uint8_t inner_header[] = {0x7e, 0x00};
    uint8_t msg_type[] = {0x54};
    uint8_t net_name_full[] = {0x43, 0x0f, 0x90, 0x00, 0x4f, 0x00, 0x70, 0x00, 0x65, 0x00, 0x6e, 0x00, 0x35, 0x00, 0x47, 0x00, 0x53};
    uint8_t net_name_short[] = {0x45, 0x09, 0x90, 0x00, 0x4e, 0x00, 0x65, 0x00, 0x78, 0x00, 0x74};
    uint8_t tz_local[] = {0x46, 0x0a};
    uint8_t tz_and_time[] = {0x47, 0x52, 0x60, 0x90, 0x30, 0x35, 0x53, 0x0a};
    uint8_t dst[] = {0x49, 0x01, 0x01};

    struct { uint8_t *data; size_t len; } parts[] = {
        {outer_header, sizeof(outer_header)},
        {mac, sizeof(mac)},
        {seq_num, sizeof(seq_num)},
        {inner_header, sizeof(inner_header)},
        {msg_type, sizeof(msg_type)},
        {net_name_full, sizeof(net_name_full)},
        {net_name_short, sizeof(net_name_short)},
        {tz_local, sizeof(tz_local)},
        {tz_and_time, sizeof(tz_and_time)},
        {dst, sizeof(dst)}
    };

    size_t offset = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        if (offset + parts[i].len * 2 >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        for (size_t j = 0; j < parts[i].len; ++j)
        {
            sprintf(EVENT_PAYLOAD + offset, "%02x", parts[i].data[j]);
            offset += 2;
        }
    }
    EVENT_PAYLOAD[offset] = '\0';
    
    print_nas_pdu("Generated Configuration Update", EVENT_PAYLOAD);
    ue_state = UE_STATE_REGISTERED;
    sequence_number++;
    
    // Trigger configuration update event
    trigger_event(EVENT_CONFIGURATION_UPDATE, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
}

// Handler for PDU Session Establishment Request
EVENT_HANDLER(handle_pdu_session_request)
{
    printf("[5G Core] Received PDU Session Establishment Request\n");
    print_nas_pdu("Input NAS PDU", EVENT_PAYLOAD);
    
    // Generate PDU Session Establishment Accept
    uint8_t outer_hdr[] = {0x7e, 0x02};
    uint8_t mac[] = {0xfb, 0xd6, 0x2d, 0x81};
    uint8_t seq[] = {0x03};
    uint8_t inner_hdr[] = {0x7e, 0x00};
    uint8_t dl_msg[] = {0x68};
    uint8_t spare_pt[] = {0x01};
    uint8_t pc_len[] = {0x00, 0x47};
    uint8_t sm_epd[] = {0x2e};
    uint8_t pdu_id[] = {0x01};
    uint8_t pti[] = {0x01};
    uint8_t sm_msg[] = {0xc2};
    uint8_t ssc_pdu[] = {0x11};
    uint8_t qos_rule[] = {0x00, 0x09, 0x01, 0x00, 0x06, 0x31, 0x31, 0x01, 0x01, 0xff, 0x01};
    uint8_t ambr[] = {0x06, 0x03, 0xf4, 0x24, 0x03, 0xf4, 0x24};
    uint8_t pdu_addr[] = {0x29, 0x05, 0x01, 0x0a, 0x2d, 0x00, 0x02};
    uint8_t snssai[] = {0x22, 0x01, 0x01};
    uint8_t qos_flow[] = {0x79, 0x00, 0x06, 0x01, 0x20, 0x41, 0x01, 0x01, 0x09};
    uint8_t epco[] = {0x7b, 0x00, 0x0f, 0x80, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x08, 0x08, 0x00, 0x0d, 0x04, 0x08, 0x08, 0x04, 0x04};
    uint8_t dnn[] = {0x25, 0x09, 0x08, 0x69, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74};
    uint8_t pdu_id_ie[] = {0x12, 0x01};

    struct { uint8_t *data; size_t len; } parts[] = {
        {outer_hdr, sizeof(outer_hdr)},
        {mac, sizeof(mac)},
        {seq, sizeof(seq)},
        {inner_hdr, sizeof(inner_hdr)},
        {dl_msg, sizeof(dl_msg)},
        {spare_pt, sizeof(spare_pt)},
        {pc_len, sizeof(pc_len)},
        {sm_epd, sizeof(sm_epd)},
        {pdu_id, sizeof(pdu_id)},
        {pti, sizeof(pti)},
        {sm_msg, sizeof(sm_msg)},
        {ssc_pdu, sizeof(ssc_pdu)},
        {qos_rule, sizeof(qos_rule)},
        {ambr, sizeof(ambr)},
        {pdu_addr, sizeof(pdu_addr)},
        {snssai, sizeof(snssai)},
        {qos_flow, sizeof(qos_flow)},
        {epco, sizeof(epco)},
        {dnn, sizeof(dnn)},
        {pdu_id_ie, sizeof(pdu_id_ie)}
    };

    size_t off = 0;
    for (size_t i = 0; i < sizeof(parts) / sizeof(parts[0]); ++i)
    {
        if (off + parts[i].len * 2 >= MAX_EVENT_PAYLOAD_SIZE)
        {
            EVENT_PAYLOAD[0] = '\0';
            return;
        }
        for (size_t j = 0; j < parts[i].len; ++j)
        {
            sprintf(EVENT_PAYLOAD + off, "%02x", parts[i].data[j]);
            off += 2;
        }
    }
    EVENT_PAYLOAD[off] = '\0';
    
    print_nas_pdu("Generated PDU Session Accept", EVENT_PAYLOAD);
    ue_state = UE_STATE_PDU_SESSION_ACTIVE;
    sequence_number++;
    
    // Trigger PDU session accept event
    trigger_event(EVENT_PDU_SESSION_ACCEPT, EVENT_PAYLOAD, event_nf_ptr->input_payload_length);
    
    // Complete the flow - stop after PDU session
    printf("[5G Core] Registration and PDU session flow completed\n");
    sleep(1);
    stopped = true;
}

// Handler for stop event
EVENT_HANDLER(handle_stop)
{
    printf("[5G Core] Received stop event\n");
    stopped = true;
}

// Register all event handlers for the 5G Core actor
void fiveg_core_register_handlers(void)
{
    printf("  - 5G Core actor: Registering handlers\n");
    register_event_handler(EVENT_REGISTRATION_REQUEST, handle_registration_request);
    register_event_handler(EVENT_AUTH_RESPONSE, handle_auth_response);
    register_event_handler(EVENT_SECURITY_MODE_COMPLETE, handle_security_mode_complete);
    register_event_handler(EVENT_REGISTRATION_COMPLETE, handle_registration_complete);
    register_event_handler(EVENT_PDU_SESSION_REQUEST, handle_pdu_session_request);
    register_event_handler(EVENT_STOP, handle_stop);
}

// Check if the 5G Core actor has stopped
bool fiveg_core_stopped(void)
{
    return stopped;
}