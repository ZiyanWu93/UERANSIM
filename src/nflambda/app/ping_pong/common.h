/*
 * Common definitions for Ping-Pong demo application
 */

#ifndef PING_PONG_COMMON_H
#define PING_PONG_COMMON_H

// Application-specific event IDs (start at 150 to avoid conflicts with NAS events)
#define EVENT_START         150  // Initial event to start the ping-pong sequence
#define EVENT_PING_REQUEST  151  // Ping actor sends this
#define EVENT_PONG_RESPONSE 152  // Pong actor sends this in response
#define EVENT_STOP          153  // Signal to stop the application

// Configuration
#define MAX_PING_COUNT      5    // Number of ping-pong exchanges before stopping

#endif // PING_PONG_COMMON_H