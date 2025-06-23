/*
 * Pong Actor - Responds to ping requests with pong responses
 */

#ifndef PONG_ACTOR_H
#define PONG_ACTOR_H

// Initialize the pong actor
void pong_init(void);

// Register pong actor's event handlers
void pong_register_handlers(void);

#endif // PONG_ACTOR_H