#include "event.h"
EventNf event_nf;
EventNf* event_nf_ptr = &event_nf; // by default, point to the global variable, but can be overridden
