#ifndef EVENT_UTILS_H
#define EVENT_UTILS_H

#include <stdio.h>
#include <string.h>
#include "event_system/event.h"

/**
 * Print the input NAS PDU in hexadecimal format
 * This function can be called by any event handler to display its input payload
 */
static inline void print_event_input_payload(const char* handler_name) {
    printf("Event Handler (%s) received input NAS PDU:", handler_name);
    // Use input_payload_length instead of relying on null termination
    int display_length = event_nf_ptr->input_payload_length;
    if (display_length > MAX_NAS_HEX_LEN) {
        display_length = MAX_NAS_HEX_LEN;
    }
    
    for (int i = 0; i < display_length; i++) {
        printf("%02x", (unsigned char)EVENT_PAYLOAD[i]);
    }
    
    printf("\n");
}

#endif // EVENT_UTILS_H
