#ifndef RMLUI_CAPI_FACTORY_H
#define RMLUI_CAPI_FACTORY_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RmlPluginCallbacks {
	int (*get_event_classes)(void* user_data);
	void (*on_initialise)(void* user_data);
	void (*on_shutdown)(void* user_data);
	void (*on_context_create)(void* user_data, RmlContext* ctx);
	void (*on_context_destroy)(void* user_data, RmlContext* ctx);
	void (*on_document_open)(void* user_data, RmlContext* ctx, RmlStringView document_path);
	void (*on_document_load)(void* user_data, RmlElementDocument* document);
	void (*on_document_unload)(void* user_data, RmlElementDocument* document);
	void (*on_element_create)(void* user_data, RmlElement* element);
	void (*on_element_destroy)(void* user_data, RmlElement* element);
	void (*on_data_model_create)(void* user_data, RmlContext* ctx, RmlStringView name);
	void (*on_data_model_destroy)(void* user_data, RmlContext* ctx, RmlStringView name);
} RmlPluginCallbacks;

RMLUI_CAPI_API RmlPlugin* rml_plugin_create(const RmlPluginCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_plugin_destroy(RmlPlugin* plugin);

typedef struct RmlElementCallbacks {
	void (*on_update)(void* user_data, RmlElement* element);
	void (*on_render)(void* user_data, RmlElement* element);
	void (*on_layout)(void* user_data, RmlElement* element);
	void (*on_resize)(void* user_data, RmlElement* element);
	void (*on_attribute_change)(void* user_data, RmlElement* element, const RmlDictionary* changed_attributes);
	/* Called after Element::OnPropertyChange. changed_properties is borrowed for this call only. */
	void (*on_property_change)(void* user_data, RmlElement* element, const RmlPropertyIdSet* changed_properties);
	void (*on_child_add)(void* user_data, RmlElement* element, RmlElement* child);
	void (*on_child_remove)(void* user_data, RmlElement* element, RmlElement* child);
	void (*process_default_action)(void* user_data, RmlElement* element, RmlEvent* event);
	bool (*get_intrinsic_dimensions)(void* user_data, RmlElement* element, RmlVector2f* dimensions, float* ratio);
	void (*on_destroy)(void* user_data, RmlElement* element);
} RmlElementCallbacks;

/* Returns an element owned by the caller until it is given to an instancer or appended. */
RMLUI_CAPI_API RmlElement* rml_element_create_custom(RmlStringView tag, const RmlElementCallbacks* callbacks, void* user_data);

typedef struct RmlElementInstancerCallbacks {
	RmlElement* (*instance)(void* user_data, RmlElement* parent, RmlStringView tag, const RmlDictionary* attributes);
	/* Notification that RmlUi is releasing the element. The element is deleted after this returns.
	   Must not call rml_element_release or otherwise free the element. element is never null. */
	void (*release)(void* user_data, RmlElement* element);
} RmlElementInstancerCallbacks;

RMLUI_CAPI_API RmlElementInstancer* rml_element_instancer_create(const RmlElementInstancerCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_element_instancer_destroy(RmlElementInstancer* instancer);
RMLUI_CAPI_API void rml_register_element_instancer(RmlStringView tag, RmlElementInstancer* instancer);

typedef struct RmlDecoratorCallbacks {
	RmlDecoratorDataHandle (*generate_element_data)(void* user_data, RmlElement* element, RmlBoxArea paint_area);
	void (*release_element_data)(void* user_data, RmlDecoratorDataHandle element_data);
	void (*render_element)(void* user_data, RmlElement* element, RmlDecoratorDataHandle element_data);
} RmlDecoratorCallbacks;

RMLUI_CAPI_API RmlDecorator* rml_decorator_create(const RmlDecoratorCallbacks* callbacks, void* user_data);

typedef RmlDecorator* (*RmlDecoratorInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);

RMLUI_CAPI_API RmlDecoratorInstancer* rml_decorator_instancer_create(RmlDecoratorInstanceFunc instance, void* user_data);
RMLUI_CAPI_API void rml_decorator_instancer_destroy(RmlDecoratorInstancer* instancer);
RMLUI_CAPI_API void rml_register_decorator_instancer(RmlStringView name, RmlDecoratorInstancer* instancer);
RMLUI_CAPI_API RmlPropertyDefinition* rml_decorator_instancer_register_property(RmlDecoratorInstancer* instancer, RmlStringView name,
	RmlStringView default_value);
RMLUI_CAPI_API uint8_t rml_decorator_instancer_register_shorthand(RmlDecoratorInstancer* instancer, RmlStringView shorthand_name,
	RmlStringView property_names, RmlShorthandType type);

typedef struct RmlFilterCallbacks {
	/* Fill out_name and, optionally, out_parameters. Return true to compile that filter through the element's render manager.
	   out_name is freed by the library. out_parameters is borrowed for this call only. */
	bool (*compile_filter)(void* user_data, RmlElement* element, RmlOwnedString* out_name, RmlDictionary* out_parameters);
	void (*extend_ink_overflow)(void* user_data, RmlElement* element, RmlRectanglef* overflow);
} RmlFilterCallbacks;

RMLUI_CAPI_API RmlFilter* rml_filter_create(const RmlFilterCallbacks* callbacks, void* user_data);
typedef RmlFilter* (*RmlFilterInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);
RMLUI_CAPI_API RmlFilterInstancer* rml_filter_instancer_create(RmlFilterInstanceFunc instance, void* user_data);
RMLUI_CAPI_API void rml_filter_instancer_destroy(RmlFilterInstancer* instancer);
RMLUI_CAPI_API void rml_register_filter_instancer(RmlStringView name, RmlFilterInstancer* instancer);

typedef RmlFontEffect* (*RmlFontEffectInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);
RMLUI_CAPI_API RmlFontEffect* rml_font_effect_create(void);
RMLUI_CAPI_API RmlFontEffectInstancer* rml_font_effect_instancer_create(RmlFontEffectInstanceFunc instance, void* user_data);
RMLUI_CAPI_API void rml_font_effect_instancer_destroy(RmlFontEffectInstancer* instancer);
RMLUI_CAPI_API void rml_register_font_effect_instancer(RmlStringView name, RmlFontEffectInstancer* instancer);

typedef struct RmlEventInstancerCallbacks {
	/* Return an event from rml_event_create to supply the event (ownership transfers to RmlUi).
	   Return NULL to use a default event. */
	RmlEvent* (*instance)(void* user_data, RmlElement* target, uint16_t id, RmlStringView type, const RmlDictionary* parameters,
		bool interruptible);
	/* Notification that RmlUi is releasing the event. The event is deleted after this returns.
	   Must not free the event. */
	void (*release)(void* user_data, RmlEvent* event);
} RmlEventInstancerCallbacks;

RMLUI_CAPI_API RmlEventInstancer* rml_event_instancer_create(const RmlEventInstancerCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_event_instancer_destroy(RmlEventInstancer* instancer);
RMLUI_CAPI_API void rml_register_event_instancer(RmlEventInstancer* instancer);

typedef RmlEventListener* (*RmlEventListenerInstancerFunc)(void* user_data, RmlStringView value, RmlElement* element);
RMLUI_CAPI_API RmlEventListenerInstancer* rml_event_listener_instancer_create(RmlEventListenerInstancerFunc instance, void* user_data);
RMLUI_CAPI_API void rml_event_listener_instancer_destroy(RmlEventListenerInstancer* instancer);
RMLUI_CAPI_API void rml_register_event_listener_instancer(RmlEventListenerInstancer* instancer);

#ifdef __cplusplus
}
#endif

#endif
