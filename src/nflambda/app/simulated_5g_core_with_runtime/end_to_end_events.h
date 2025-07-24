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
#define EVENT_SMF_PROCESS_PDU_REQ      1101  // Internal: Process PDU session request
#define EVENT_SMF_REQUEST_POLICY       1102  // Internal: Request policy from PCF
#define EVENT_SMF_REQUEST_DNN          1103  // Internal: Request DNN from UDM
#define EVENT_SMF_COMPLETE_PDU         1104  // Internal: Complete PDU session

// UPF (User Plane Function) - Range: 1200-1299
#define EVENT_TO_UPF                   1200  // Main UPF dispatcher event
// Reserved: 1201-1299 for UPF internal events
#define EVENT_UPF_ALLOCATE_IP          1201  // Internal: Allocate IP address

// AUSF (Authentication Server Function) - Range: 1300-1399
#define EVENT_TO_AUSF                  1300  // Main AUSF dispatcher event
// Reserved: 1301-1399 for AUSF internal events
#define EVENT_AUSF_PROCESS_AUTH_REQ    1301  // Internal: Process auth data request
#define EVENT_AUSF_COMPLETE_AUTH_DATA  1302  // Internal: Complete auth data response
#define EVENT_AUSF_PROCESS_SEC_MODE    1303  // Internal: Process security mode request
#define EVENT_AUSF_COMPLETE_SEC_MODE   1304  // Internal: Complete security mode data

// UDM (Unified Data Management) - Range: 1400-1499
#define EVENT_TO_UDM                   1400  // Main UDM dispatcher event
// Reserved: 1401-1499 for UDM internal events
#define EVENT_UDM_GEN_AUTH_VECTORS     1401  // Internal: Generate auth vectors
#define EVENT_UDM_PROVIDE_SEC_CAP      1402  // Internal: Provide UE security capabilities
#define EVENT_UDM_PROVIDE_SUBSCRIPTION 1403  // Internal: Provide subscription data
#define EVENT_UDM_PROVIDE_DNN          1404  // Internal: Provide DNN information

// PCF (Policy Control Function) - Range: 1500-1599
#define EVENT_TO_PCF                   1500  // Main PCF dispatcher event
// Reserved: 1501-1599 for PCF internal events
#define EVENT_PCF_PROVIDE_CONFIG       1501  // Internal: Provide network configuration
#define EVENT_PCF_PROVIDE_PCC          1502  // Internal: Provide PCC rules

// NSSF (Network Slice Selection Function) - Range: 1600-1699
#define EVENT_TO_NSSF                  1600  // Main NSSF dispatcher event
// Reserved: 1601-1699 for NSSF internal events

// AMF Internal Events (1001-1099)
#define EVENT_AMF_REGISTRATION_REQUEST    1001
#define EVENT_AMF_AUTH_RESPONSE          1002
#define EVENT_AMF_SECURITY_MODE_COMPLETE 1003
#define EVENT_AMF_REGISTRATION_COMPLETE  1004
#define EVENT_AMF_PDU_SESSION_REQUEST    1005
#define EVENT_AMF_FINALIZE_AUTH_REQUEST  1006  // Internal: Finalize and send auth request
#define EVENT_AMF_FINALIZE_SEC_MODE      1007  // Internal: Finalize and send security mode command
#define EVENT_AMF_FINALIZE_REG_ACCEPT    1008  // Internal: Finalize and send registration accept
#define EVENT_AMF_FINALIZE_CONFIG_UPDATE 1009  // Internal: Finalize and send configuration update
#define EVENT_AMF_FINALIZE_PDU_ACCEPT    1010  // Internal: Finalize and send PDU session accept

// Request/Response Types (stored in payload[0])
#define REQ_TYPE_REGULAR_MESSAGE         0x00  // Regular NAS message processing
#define REQ_TYPE_GEN_AUTH_DATA           0x10  // AMF → AUSF: Generate Authentication Data
#define REQ_TYPE_GET_AUTH_VECTORS        0x11  // AUSF → UDM: Get Authentication Vectors
#define REQ_TYPE_PREPARE_SEC_MODE        0x12  // AMF → AUSF: Prepare Security Mode
#define REQ_TYPE_GET_UE_SEC_CAP          0x13  // AUSF → UDM: Get UE Security Capabilities
#define REQ_TYPE_PREPARE_REG_ACCEPT      0x14  // AMF → UDM: Prepare Registration Accept
#define REQ_TYPE_GET_SUBSCRIPTION_DATA   0x15  // UDM → PCF: Get Policy Data (optional)
#define REQ_TYPE_GET_CONFIG_DATA         0x16  // AMF → PCF: Get Configuration Data
#define REQ_TYPE_CREATE_PDU_SESSION      0x17  // AMF → SMF: Create PDU Session
#define REQ_TYPE_ALLOCATE_IP             0x18  // SMF → UPF: Allocate IP Address
#define REQ_TYPE_GET_PCC_RULES           0x19  // SMF → PCF: Get PCC Rules
#define REQ_TYPE_GET_DNN_INFO            0x1A  // SMF → UDM: Get DNN Information
#define RESP_TYPE_AUTH_VECTORS           0x20  // UDM → AUSF: Auth Vectors Response
#define RESP_TYPE_AUTH_DATA              0x21  // AUSF → AMF: Auth Data Response
#define RESP_TYPE_UE_SEC_CAP             0x22  // UDM → AUSF: UE Security Capabilities
#define RESP_TYPE_SEC_MODE_DATA          0x23  // AUSF → AMF: Security Mode Data
#define RESP_TYPE_SUBSCRIPTION_DATA      0x24  // UDM → AMF: Subscription Data
#define RESP_TYPE_POLICY_DATA            0x25  // PCF → UDM: Policy Data (optional)
#define RESP_TYPE_CONFIG_DATA            0x26  // PCF → AMF: Configuration Data
#define RESP_TYPE_IP_ALLOCATED           0x27  // UPF → SMF: IP Address Allocated
#define RESP_TYPE_PCC_RULES              0x28  // PCF → SMF: PCC Rules
#define RESP_TYPE_DNN_INFO               0x29  // UDM → SMF: DNN Information
#define RESP_TYPE_PDU_SESSION_CREATED    0x2A  // SMF → AMF: PDU Session Created

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