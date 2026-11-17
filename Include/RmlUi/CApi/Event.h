#ifndef RMLUI_CAPI_EVENT_H
#define RMLUI_CAPI_EVENT_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API RmlEventPhase rml_event_get_phase(const RmlEvent* event);
RMLUI_CAPI_API RmlElement* rml_event_get_current_element(RmlEvent* event);
RMLUI_CAPI_API RmlElement* rml_event_get_target_element(const RmlEvent* event);
RMLUI_CAPI_API RmlStringView rml_event_get_type(const RmlEvent* event);
RMLUI_CAPI_API uint16_t rml_event_get_id(const RmlEvent* event);
RMLUI_CAPI_API void rml_event_stop_propagation(RmlEvent* event);
RMLUI_CAPI_API void rml_event_stop_immediate_propagation(RmlEvent* event);
RMLUI_CAPI_API bool rml_event_is_interruptible(const RmlEvent* event);
RMLUI_CAPI_API bool rml_event_is_propagating(const RmlEvent* event);
RMLUI_CAPI_API bool rml_event_is_immediate_propagating(const RmlEvent* event);
RMLUI_CAPI_API const RmlDictionary* rml_event_get_parameters(const RmlEvent* event);
RMLUI_CAPI_API RmlVector2f rml_event_get_unprojected_mouse_screen_pos(const RmlEvent* event);

typedef struct RmlEventListenerCallbacks {
	void (*process_event)(void* user_data, RmlEvent* event);
	void (*on_attach)(void* user_data, RmlElement* element);
	/* Called when the listener is detached. May call rml_event_listener_destroy on this listener. */
	void (*on_detach)(void* user_data, RmlElement* element);
} RmlEventListenerCallbacks;

/* Creates an event owned by the caller until it is returned from an event instancer or passed to rml_event_destroy. */
RMLUI_CAPI_API RmlEvent* rml_event_create(RmlElement* target, uint16_t id, RmlStringView type, const RmlDictionary* parameters, bool interruptible);
RMLUI_CAPI_API void rml_event_destroy(RmlEvent* event);

RMLUI_CAPI_API RmlEventListener* rml_event_listener_create(const RmlEventListenerCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_event_listener_destroy(RmlEventListener* listener);

#ifdef __cplusplus
}
#endif

#endif