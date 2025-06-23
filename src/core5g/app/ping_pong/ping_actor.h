/*
 * Ping Actor - Sends ping requests and handles pong responses
 */

#ifndef PING_ACTOR_H
#define PING_ACTOR_H

#include <stdbool.h>

// Initialize the ping actor
void ping_init(void);

// Register ping actor's event handlers
void ping_register_handlers(void);

// Check if ping actor has stopped (for exit condition)
bool ping_is_stopped(void);

#endif // PING_ACTOR_H