#ifndef UDM_H
#define UDM_H

#include "../../event_system/event.h"

// UDM Dispatcher
EVENT_HANDLER(udm_dispatcher);


// Internal Event Handlers
EVENT_HANDLER(udm_gen_auth_vectors);
EVENT_HANDLER(udm_provide_sec_cap);
EVENT_HANDLER(udm_provide_subscription);
EVENT_HANDLER(udm_provide_dnn);

#endif // UDM_H