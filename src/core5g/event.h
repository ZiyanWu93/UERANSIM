#ifndef EVENT_H
#define EVENT_H

#include <string.h>
#define MAX_NAS_HEX_LEN 300             // big enough for the longest hard-coded hex string

typedef struct EventNf {
    int event_id;
    char input_payload[MAX_NAS_HEX_LEN];
    char output_payload[MAX_NAS_HEX_LEN];
} EventNf;

extern EventNf event_nf;
extern EventNf* event_nf_ptr;

typedef void (*event_handler_t)();
#define EVENT_HANDLER(name)  void name()
#define EVENT_ID event_nf_ptr->event_id
#define EVENT_PAYLOAD event_nf_ptr->input_payload
#define EVENT_OUTPUT_PAYLOAD event_nf_ptr->output_payload

#endif //EVENT_H
