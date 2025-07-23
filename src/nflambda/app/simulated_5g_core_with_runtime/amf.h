#ifndef AMF_H
#define AMF_H

#include "../../event_system/event.h"

// AMF Dispatcher
EVENT_HANDLER(amf_dispatcher);

// AMF Event Handlers
void amf_handle_registration_request(void);
void amf_handle_authentication_response(void);
void amf_handle_security_mode_complete(void);
void amf_handle_registration_complete(void);
void amf_handle_pdu_session_request(void);
void amf_finalize_auth_request(void);

#endif // AMF_H