#ifndef CORE_5G_ACTOR_H
#define CORE_5G_ACTOR_H

#include <stdbool.h>
#include "../../event_system/event.h"
#include "../../event_system/ipc_event_source.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the Core 5G actor
 * Sets up initial state and prepares for message handling
 */
void core_5g_init(void);

/**
 * Register all Core 5G event handlers
 * This includes both IPC and NAS event handlers
 */
void core_5g_register_handlers(void);

/**
 * Check if the Core 5G actor has stopped
 * @return true if stopped, false otherwise
 */
bool core_5g_is_stopped(void);

/**
 * Stop the Core 5G actor
 * Triggers cleanup and shutdown procedures
 */
void core_5g_stop(void);

/* Event handler declarations */
EVENT_HANDLER(handle_ipc_nas_message);
EVENT_HANDLER(handle_ipc_send_nas_response);

#ifdef __cplusplus
}
#endif

#endif /* CORE_5G_ACTOR_H */