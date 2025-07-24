#ifndef AMF_H
#define AMF_H

#include "../../event_system/event.h"

// AMF Dispatcher
EVENT_HANDLER(amf_dispatcher);

// AMF Event Handlers
EVENT_HANDLER(amf_handle_registration_request);
EVENT_HANDLER(amf_handle_authentication_response);
EVENT_HANDLER(amf_handle_security_mode_complete);
EVENT_HANDLER(amf_handle_registration_complete);
EVENT_HANDLER(amf_handle_pdu_session_request);
EVENT_HANDLER(amf_finalize_auth_request);
EVENT_HANDLER(amf_finalize_sec_mode);
EVENT_HANDLER(amf_finalize_reg_accept);
EVENT_HANDLER(amf_finalize_config_update);
EVENT_HANDLER(amf_finalize_pdu_accept);

#endif // AMF_H