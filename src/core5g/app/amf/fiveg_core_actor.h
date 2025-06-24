#ifndef FIVEG_CORE_ACTOR_H
#define FIVEG_CORE_ACTOR_H

#include <stdbool.h>

// Initialize the 5G Core actor
void fiveg_core_init(void);

// Register all event handlers for the 5G Core actor
void fiveg_core_register_handlers(void);

// Check if the 5G Core actor has stopped
bool fiveg_core_stopped(void);

#endif // FIVEG_CORE_ACTOR_H