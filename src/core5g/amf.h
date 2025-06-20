//
// Created by ziyan on 6/10/25.
//

#ifndef AMF_H
#define AMF_H
#include "event_system/event.h"

EVENT_HANDLER(generate_auth_req);
EVENT_HANDLER(generate_security_cmd);
EVENT_HANDLER(generate_registration_accept);
EVENT_HANDLER(generate_configuration_update);
EVENT_HANDLER(generate_pdu_session_establishment);

#endif //AMF_H
