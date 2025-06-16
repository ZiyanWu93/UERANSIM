
#define MAX_NAS_HEX_LEN 300             // big enough for the longest hard-coded hex string

#ifdef __cplusplus
extern "C" {
#endif


typedef struct EventNf {
    char payload[MAX_NAS_HEX_LEN];
} EventNf;

extern EventNf event_nf;

typedef void (*event_handler_t)();
#define EVENT_HANDLER(name)  void name()
#define EVENT_PAYLOAD event_nf.payload

EVENT_HANDLER(generate_auth_req);
EVENT_HANDLER(generate_security_cmd);
EVENT_HANDLER(generate_registration_accept);
EVENT_HANDLER(generate_configuration_update);
EVENT_HANDLER(generate_pdu_session_establishment);

#ifdef __cplusplus
}
#endif
