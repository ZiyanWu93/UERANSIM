#ifndef UPF_H
#define UPF_H

#include "../../event_system/event.h"

// UPF Dispatcher
EVENT_HANDLER(upf_dispatcher);

// Internal Event Handlers
EVENT_HANDLER(upf_allocate_ip);

#endif // UPF_H