#ifndef UE_STATE_H
#define UE_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* UE registration states */
typedef enum {
    UE_STATE_DEREGISTERED = 0,
    UE_STATE_REGISTERING,
    UE_STATE_AUTHENTICATED,
    UE_STATE_REGISTERED
} ue_state_t;

/* Security context (simplified for single UE) */
typedef struct {
    bool security_established;
    uint8_t sequence_number;
    /* Additional security parameters can be added here */
} security_context_t;

/**
 * Initialize UE state management
 * Resets all state to initial values
 */
void ue_state_init(void);

/**
 * Get current UE registration state
 * @return Current UE state
 */
ue_state_t ue_state_get(void);

/**
 * Set UE registration state
 * @param state New UE state
 */
void ue_state_set(ue_state_t state);

/**
 * Get current transaction ID
 * @return Current transaction ID
 */
uint32_t ue_state_get_transaction_id(void);

/**
 * Set transaction ID for correlation
 * @param trans_id Transaction ID from IPC message
 */
void ue_state_set_transaction_id(uint32_t trans_id);

/**
 * Get next transaction ID (auto-increment)
 * @return Next transaction ID
 */
uint32_t ue_state_next_transaction_id(void);

/**
 * Check if security context is established
 * @return true if security is active, false otherwise
 */
bool ue_state_is_secure(void);

/**
 * Mark security context as established
 */
void ue_state_establish_security(void);

/**
 * Get current sequence number for integrity protection
 * @return Current sequence number
 */
uint8_t ue_state_get_sequence_number(void);

/**
 * Increment sequence number
 * @return New sequence number after increment
 */
uint8_t ue_state_increment_sequence_number(void);

/**
 * Get human-readable state name
 * @param state UE state
 * @return String representation of state
 */
const char* ue_state_to_string(ue_state_t state);

/**
 * Reset all UE state to initial values
 * Used when UE deregisters or on error conditions
 */
void ue_state_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* UE_STATE_H */