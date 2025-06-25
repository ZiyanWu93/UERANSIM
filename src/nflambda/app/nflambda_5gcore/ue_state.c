#include "ue_state.h"
#include <stdio.h>

/* Single UE state variables */
static struct {
    ue_state_t registration_state;
    security_context_t security;
    uint32_t transaction_id;
    uint32_t transaction_counter;
} ue_context = {
    .registration_state = UE_STATE_DEREGISTERED,
    .security = { .security_established = false, .sequence_number = 0 },
    .transaction_id = 0,
    .transaction_counter = 1000  /* Start from 1000 for easy debugging */
};

/* Initialize UE state management */
void ue_state_init(void)
{
    printf("[UE State] Initializing single UE state\n");
    ue_state_reset();
}

/* Get current UE registration state */
ue_state_t ue_state_get(void)
{
    return ue_context.registration_state;
}

/* Set UE registration state */
void ue_state_set(ue_state_t state)
{
    printf("[UE State] State transition: %s -> %s\n",
           ue_state_to_string(ue_context.registration_state),
           ue_state_to_string(state));
    ue_context.registration_state = state;
}

/* Get current transaction ID */
uint32_t ue_state_get_transaction_id(void)
{
    return ue_context.transaction_id;
}

/* Set transaction ID for correlation */
void ue_state_set_transaction_id(uint32_t trans_id)
{
    ue_context.transaction_id = trans_id;
}

/* Get next transaction ID (auto-increment) */
uint32_t ue_state_next_transaction_id(void)
{
    return ++ue_context.transaction_counter;
}

/* Check if security context is established */
bool ue_state_is_secure(void)
{
    return ue_context.security.security_established;
}

/* Mark security context as established */
void ue_state_establish_security(void)
{
    printf("[UE State] Security context established\n");
    ue_context.security.security_established = true;
    ue_context.security.sequence_number = 0;
}

/* Get current sequence number for integrity protection */
uint8_t ue_state_get_sequence_number(void)
{
    return ue_context.security.sequence_number;
}

/* Increment sequence number */
uint8_t ue_state_increment_sequence_number(void)
{
    ue_context.security.sequence_number++;
    return ue_context.security.sequence_number;
}

/* Get human-readable state name */
const char* ue_state_to_string(ue_state_t state)
{
    switch (state) {
        case UE_STATE_DEREGISTERED:
            return "DEREGISTERED";
        case UE_STATE_REGISTERING:
            return "REGISTERING";
        case UE_STATE_AUTHENTICATED:
            return "AUTHENTICATED";
        case UE_STATE_REGISTERED:
            return "REGISTERED";
        default:
            return "UNKNOWN";
    }
}

/* Reset all UE state to initial values */
void ue_state_reset(void)
{
    printf("[UE State] Resetting UE state\n");
    ue_context.registration_state = UE_STATE_DEREGISTERED;
    ue_context.security.security_established = false;
    ue_context.security.sequence_number = 0;
    ue_context.transaction_id = 0;
    /* Keep transaction_counter to avoid ID reuse */
}