#ifndef EVENT_H
#define EVENT_H

#include <string.h>
#define MAX_NAS_HEX_LEN 1000             // big enough for the longest hard-coded hex string

typedef struct EventNf {
    int event_id;
    char input_payload[MAX_NAS_HEX_LEN];
    int input_payload_length;      // Actual binary length of input_payload
} EventNf;

extern EventNf event_nf;
extern EventNf* event_nf_ptr;

typedef void (*event_handler_t)();
#define EVENT_HANDLER(name)  void name()
#define EVENT_ID event_nf_ptr->event_id
#define EVENT_PAYLOAD event_nf_ptr->input_payload

// NAS Event Type Definitions
// These events simulate uplink NAS messages from UE
#define EVENT_NAS_REGISTRATION_REQUEST     100
#define EVENT_NAS_AUTH_RESPONSE           101
#define EVENT_NAS_SECURITY_MODE_COMPLETE  102
#define EVENT_NAS_REGISTRATION_COMPLETE   103
#define EVENT_NAS_PDU_SESSION_REQUEST     104

#endif //EVENT_H
