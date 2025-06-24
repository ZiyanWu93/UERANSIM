#ifndef UERANSIM_ACTOR_H
#define UERANSIM_ACTOR_H

#include <stdbool.h>

// Initialize the UERANSIM actor
void ueransim_init(void);

// Register all event handlers for the UERANSIM actor
void ueransim_register_handlers(void);

// Event source that generates the initial registration request
void ueransim_startup_event_source(void);

#endif // UERANSIM_ACTOR_H