#ifndef END_TO_END_EVENTS_H
#define END_TO_END_EVENTS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

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

// Network Function Dispatcher Events
// Each NF has a main dispatcher event and reserved space for internal events (100 events per NF)

// AMF (Access and Mobility Management Function) - Range: 1000-1099
#define EVENT_TO_AMF                   1000  // Main AMF dispatcher event
// Reserved: 1001-1099 for AMF internal events

// SMF (Session Management Function) - Range: 1100-1199
#define EVENT_TO_SMF                   1100  // Main SMF dispatcher event
// Reserved: 1101-1199 for SMF internal events

// UPF (User Plane Function) - Range: 1200-1299
#define EVENT_TO_UPF                   1200  // Main UPF dispatcher event
// Reserved: 1201-1299 for UPF internal events

// AUSF (Authentication Server Function) - Range: 1300-1399
#define EVENT_TO_AUSF                  1300  // Main AUSF dispatcher event
// Reserved: 1301-1399 for AUSF internal events
#define EVENT_AUSF_SET_RAND            1301  // AUSF sets RAND in authentication request

// UDM (Unified Data Management) - Range: 1400-1499
#define EVENT_TO_UDM                   1400  // Main UDM dispatcher event
// Reserved: 1401-1499 for UDM internal events
#define EVENT_UDM_SET_AUTN             1401  // UDM sets AUTN in authentication request

// PCF (Policy Control Function) - Range: 1500-1599
#define EVENT_TO_PCF                   1500  // Main PCF dispatcher event
// Reserved: 1501-1599 for PCF internal events

// NSSF (Network Slice Selection Function) - Range: 1600-1699
#define EVENT_TO_NSSF                  1600  // Main NSSF dispatcher event
// Reserved: 1601-1699 for NSSF internal events

// AMF Internal Events (1001-1099)
#define EVENT_AMF_REGISTRATION_REQUEST    1001
#define EVENT_AMF_AUTH_RESPONSE          1002
#define EVENT_AMF_SECURITY_MODE_COMPLETE 1003
#define EVENT_AMF_REGISTRATION_COMPLETE  1004
#define EVENT_AMF_PDU_SESSION_REQUEST    1005

// AMF Service Function Chain Events for Authentication Request
#define EVENT_AMF_SET_AUTH_HEADERS       1006
#define EVENT_AMF_SET_AUTH_NGKSI         1007
#define EVENT_AMF_SET_ABBA               1008
#define EVENT_AMF_SEND_AUTH_REQUEST      1009

// NAS Message Type Constants
#define NAS_MSG_REGISTRATION_REQUEST     0x41
#define NAS_MSG_AUTH_REQUEST            0x56
#define NAS_MSG_AUTH_RESPONSE           0x57
#define NAS_MSG_SECURITY_MODE_COMMAND   0x5d
#define NAS_MSG_SECURITY_MODE_COMPLETE  0x5e
#define NAS_MSG_REGISTRATION_ACCEPT     0x42
#define NAS_MSG_REGISTRATION_COMPLETE   0x43
#define NAS_MSG_CONFIGURATION_UPDATE    0x54
#define NAS_MSG_PDU_SESSION_EST_REQ     0xc1

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
void print_nas_pdu(const char* label, const uint8_t* pdu, size_t len);

#endif // END_TO_END_EVENTS_H