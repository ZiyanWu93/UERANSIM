#ifndef AMF_COMMON_H
#define AMF_COMMON_H

#include <stdbool.h>
#include <stdint.h>

// Event IDs for NAS message flow between UE/gNB and AMF
// Start at 160 to avoid conflicts with existing events
// Uplink events (from UE to AMF)
#define EVENT_REGISTRATION_REQUEST      160
#define EVENT_AUTH_RESPONSE            161
#define EVENT_SECURITY_MODE_COMPLETE   162
#define EVENT_REGISTRATION_COMPLETE    163
#define EVENT_PDU_SESSION_REQUEST      164

// Downlink events (from AMF to UE)
#define EVENT_AUTH_REQUEST             170
#define EVENT_SECURITY_MODE_COMMAND    171
#define EVENT_REGISTRATION_ACCEPT      172
#define EVENT_CONFIGURATION_UPDATE     173
#define EVENT_PDU_SESSION_ACCEPT       174

// Control events
#define EVENT_START_REGISTRATION       180
#define EVENT_STOP                     181

// Maximum NAS PDU length is already defined in event.h

// UE context state
typedef enum {
    UE_STATE_DEREGISTERED,
    UE_STATE_REGISTERING,
    UE_STATE_AUTHENTICATED,
    UE_STATE_SECURITY_ESTABLISHED,
    UE_STATE_REGISTERED,
    UE_STATE_PDU_SESSION_ACTIVE
} ue_state_t;

// Event payload is accessed via EVENT_PAYLOAD macro from event.h

// Utility function to print hex PDU
void print_nas_pdu(const char* label, const char* pdu);

#endif // AMF_COMMON_H