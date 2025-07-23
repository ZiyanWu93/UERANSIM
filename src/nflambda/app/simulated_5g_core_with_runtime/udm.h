#ifndef UDM_H
#define UDM_H

#include "../../event_system/event.h"

// UDM Dispatcher
void udm_dispatcher(EventNf* event);

// Phase 1: Authentication vector generation
void udm_generate_authentication_vector(void);

// Phase 3: Subscriber data retrieval
void udm_retrieve_subscription_data(void);
void udm_update_subscriber_status(void);

// Service Function Chain Handler
void udm_set_autn(void);

#endif // UDM_H