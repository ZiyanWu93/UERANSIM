#ifndef AUSF_H
#define AUSF_H

#include "../../event_system/event.h"

// AUSF Dispatcher
EVENT_HANDLER(ausf_dispatcher);


// Internal Event Handlers
EVENT_HANDLER(ausf_process_auth_request);
EVENT_HANDLER(ausf_complete_auth_data);
EVENT_HANDLER(ausf_process_sec_mode);
EVENT_HANDLER(ausf_complete_sec_mode);

#endif // AUSF_H