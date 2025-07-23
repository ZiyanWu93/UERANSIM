#ifndef AUSF_H
#define AUSF_H

#include "../../event_system/event.h"

// AUSF Dispatcher
void ausf_dispatcher(EventNf* event);

// Phase 1: Authentication procedures
void ausf_authenticate_request(void);

// Phase 2: Authentication confirmation
void ausf_confirm_authentication(void);
void ausf_derive_kseaf(void);

// Service Function Chain Handler
void ausf_set_rand(void);

#endif // AUSF_H