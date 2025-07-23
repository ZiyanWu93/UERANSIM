#ifndef AMF_H
#define AMF_H

#include "../../event_system/event.h"

// AMF Dispatcher
void amf_dispatcher(EventNf* event);

// AMF Event Handlers
void amf_handle_registration_request(void);
void amf_handle_authentication_response(void);
void amf_handle_security_mode_complete(void);
void amf_handle_registration_complete(void);
void amf_handle_pdu_session_request(void);

// Service Function Chain Handlers for Authentication Request
void amf_set_auth_ngksi(void);
void amf_set_abba(void);
void amf_send_auth_request(void);

#endif // AMF_H