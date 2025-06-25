#ifndef AMF_HANDLERS_H
#define AMF_HANDLERS_H

#include "../../event_system/event.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Register all AMF NAS event handlers
 */
void amf_register_handlers(void);

/**
 * Utility function to print NAS PDU for debugging
 * @param label Description of the PDU
 * @param pdu Hex string representation of the PDU
 */
void print_nas_pdu(const char* label, const char* pdu);

/* NAS event handlers */
EVENT_HANDLER(handle_registration_request);
EVENT_HANDLER(handle_auth_response);
EVENT_HANDLER(handle_security_mode_complete);
EVENT_HANDLER(handle_registration_complete);
EVENT_HANDLER(handle_pdu_session_request);

#ifdef __cplusplus
}
#endif

#endif /* AMF_HANDLERS_H */