#ifndef AUSF_H
#define AUSF_H

#include "../../event_system/event.h"

// AUSF Dispatcher
EVENT_HANDLER(ausf_dispatcher);


// Internal Event Handlers
EVENT_HANDLER(ausf_process_auth_request);
EVENT_HANDLER(ausf_complete_auth_data);

#endif // AUSF_H