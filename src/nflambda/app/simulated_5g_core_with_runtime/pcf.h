#ifndef PCF_H
#define PCF_H

#include "../../event_system/event.h"

// PCF Dispatcher
EVENT_HANDLER(pcf_dispatcher);

// Internal Event Handlers
EVENT_HANDLER(pcf_provide_config);
EVENT_HANDLER(pcf_provide_pcc);

#endif // PCF_H