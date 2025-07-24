#ifndef SMF_H
#define SMF_H

#include "../../event_system/event.h"

// SMF Dispatcher
EVENT_HANDLER(smf_dispatcher);

// Internal Event Handlers
EVENT_HANDLER(smf_process_pdu_request);
EVENT_HANDLER(smf_request_policy);
EVENT_HANDLER(smf_request_dnn);
EVENT_HANDLER(smf_complete_pdu_session);

#endif // SMF_H