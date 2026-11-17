#ifndef RMLUI_CAPI_FACTORY_H
#define RMLUI_CAPI_FACTORY_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Plugin lifetime, matching Rml::Plugin:
   - get_event_classes is called once, by rml_plugin_create. Returns a mask of RML_PLUGIN_EVENT_* (all when null).
   - on_initialise is called when the plugin is registered while RmlUi is initialised, or by rml_initialise for plugins
     registered before it that include RML_PLUGIN_EVENT_BASIC.
   - on_shutdown is called when the plugin is unregistered (or destroyed while registered) while RmlUi is initialised,
     or by rml_shutdown for plugins that include RML_PLUGIN_EVENT_BASIC.
   rml_shutdown unregisters every plugin, so register plugins again after reinitialising. */
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

/* Returns a caller-owned element (see Element.h). Hand it to RmlUi by returning it from an element instancer, whose
   release callback then runs before it is destroyed, or through any function that takes ownership of caller-owned
   elements. Otherwise destroy it with rml_element_release. on_destroy runs in every case. */
RMLUI_CAPI_API RmlElement* rml_element_create_custom(RmlStringView tag, const RmlElementCallbacks* callbacks, void* user_data);

/* Instancers and other objects registered with RmlUi must stay alive until after rml_shutdown. */
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

/* Ownership of decorators, filters and font effects passes to RmlUi when an instance callback returns them. Destroy
   objects that are never returned with the matching rml_*_destroy function. */
RMLUI_CAPI_API RmlDecorator* rml_decorator_create(const RmlDecoratorCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_decorator_destroy(RmlDecorator* decorator);

/* properties is borrowed for this call only. Look up values with rml_property_dictionary_get_string using the names
   registered on the instancer. */
typedef RmlDecorator* (*RmlDecoratorInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);
/* Like RmlDecoratorInstanceFunc, with instancer_interface for loading textures and sprites. instancer_interface is
   borrowed for this call only. */
typedef RmlDecorator* (*RmlDecoratorInstanceWithInterfaceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties,
	const RmlDecoratorInstancerInterface* instancer_interface);

RMLUI_CAPI_API RmlDecoratorInstancer* rml_decorator_instancer_create(RmlDecoratorInstanceFunc instance, void* user_data);
RMLUI_CAPI_API RmlDecoratorInstancer* rml_decorator_instancer_create_with_interface(RmlDecoratorInstanceWithInterfaceFunc instance,
	void* user_data);
RMLUI_CAPI_API void rml_decorator_instancer_destroy(RmlDecoratorInstancer* instancer);
RMLUI_CAPI_API void rml_register_decorator_instancer(RmlStringView name, RmlDecoratorInstancer* instancer);
RMLUI_CAPI_API RmlPropertyDefinition* rml_decorator_instancer_register_property(RmlDecoratorInstancer* instancer, RmlStringView name,
	RmlStringView default_value);
RMLUI_CAPI_API uint8_t rml_decorator_instancer_register_shorthand(RmlDecoratorInstancer* instancer, RmlStringView shorthand_name,
	RmlStringView property_names, RmlShorthandType type);

/* Loads a texture for the decorator's render manager. Relative filenames are resolved against the style sheet that
   declared the decorator. */
RMLUI_CAPI_API RmlTexture rml_decorator_instancer_interface_get_texture(const RmlDecoratorInstancerInterface* instancer_interface,
	RmlStringView filename);
/* Looks up a sprite from an @spritesheet in the active style sheet. Returns a zero texture when there is no such sprite.
   out_rectangle receives the sprite's area of the texture in px, and out_display_scale the inverse of the sprite sheet
   resolution. Both are optional. */
RMLUI_CAPI_API RmlTexture rml_decorator_instancer_interface_get_sprite(const RmlDecoratorInstancerInterface* instancer_interface,
	RmlStringView name, RmlRectanglef* out_rectangle, float* out_display_scale);

typedef struct RmlFilterCallbacks {
	/* Fill out_name and, optionally, out_parameters. Return true to compile that filter through the element's render manager.
	   out_name is freed by the library. out_parameters is borrowed for this call only. */
	bool (*compile_filter)(void* user_data, RmlElement* element, RmlOwnedString* out_name, RmlDictionary* out_parameters);
	void (*extend_ink_overflow)(void* user_data, RmlElement* element, RmlRectanglef* overflow);
} RmlFilterCallbacks;

RMLUI_CAPI_API RmlFilter* rml_filter_create(const RmlFilterCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_filter_destroy(RmlFilter* filter);
/* properties is borrowed for this call only. */
typedef RmlFilter* (*RmlFilterInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);
RMLUI_CAPI_API RmlFilterInstancer* rml_filter_instancer_create(RmlFilterInstanceFunc instance, void* user_data);
RMLUI_CAPI_API void rml_filter_instancer_destroy(RmlFilterInstancer* instancer);
RMLUI_CAPI_API void rml_register_filter_instancer(RmlStringView name, RmlFilterInstancer* instancer);
RMLUI_CAPI_API RmlPropertyDefinition* rml_filter_instancer_register_property(RmlFilterInstancer* instancer, RmlStringView name,
	RmlStringView default_value);
RMLUI_CAPI_API uint8_t rml_filter_instancer_register_shorthand(RmlFilterInstancer* instancer, RmlStringView shorthand_name,
	RmlStringView property_names, RmlShorthandType type);

typedef enum RmlFontEffectLayer {
	RML_FONT_EFFECT_LAYER_BACK = 0,
	RML_FONT_EFFECT_LAYER_FRONT = 1
} RmlFontEffectLayer;

typedef enum RmlColorFormat {
	RML_COLOR_FORMAT_RGBA8 = 0,
	RML_COLOR_FORMAT_A8 = 1
} RmlColorFormat;

/* bitmap_data is borrowed for the duration of the callback that receives the glyph. */
typedef struct RmlFontGlyph {
	RmlVector2i bearing;
	int advance;
	const unsigned char* bitmap_data;
	RmlVector2i bitmap_dimensions;
	RmlColorFormat color_format;
} RmlFontGlyph;

/* Overrides for Rml::FontEffect. Null callbacks use the base class behaviour. on_destroy is called when the effect is
   destroyed, by RmlUi or by rml_font_effect_destroy. */
typedef struct RmlFontEffectCallbacks {
	bool (*has_unique_texture)(void* user_data);
	bool (*get_glyph_metrics)(void* user_data, RmlVector2i* origin, RmlVector2i* dimensions, const RmlFontGlyph* glyph);
	void (*generate_glyph_texture)(void* user_data, unsigned char* destination, RmlVector2i destination_dimensions, int destination_stride,
		const RmlFontGlyph* glyph);
	void (*on_destroy)(void* user_data);
} RmlFontEffectCallbacks;

/* properties is borrowed for this call only. RmlUi sets the fingerprint of the returned effect from its properties. */
typedef RmlFontEffect* (*RmlFontEffectInstanceFunc)(void* user_data, RmlStringView name, const RmlPropertyDictionary* properties);
RMLUI_CAPI_API RmlFontEffect* rml_font_effect_create(void);
RMLUI_CAPI_API RmlFontEffect* rml_font_effect_create_custom(const RmlFontEffectCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_font_effect_destroy(RmlFontEffect* effect);
RMLUI_CAPI_API void rml_font_effect_set_layer(RmlFontEffect* effect, RmlFontEffectLayer layer);
RMLUI_CAPI_API RmlFontEffectLayer rml_font_effect_get_layer(const RmlFontEffect* effect);
RMLUI_CAPI_API void rml_font_effect_set_colour(RmlFontEffect* effect, RmlColourb colour);
RMLUI_CAPI_API RmlColourb rml_font_effect_get_colour(const RmlFontEffect* effect);
RMLUI_CAPI_API void rml_font_effect_set_fingerprint(RmlFontEffect* effect, size_t fingerprint);
RMLUI_CAPI_API size_t rml_font_effect_get_fingerprint(const RmlFontEffect* effect);
/* The following call into the effect, including C++ effects, for use by font engines implemented in C. */
RMLUI_CAPI_API bool rml_font_effect_has_unique_texture(const RmlFontEffect* effect);
RMLUI_CAPI_API bool rml_font_effect_get_glyph_metrics(const RmlFontEffect* effect, RmlVector2i* origin, RmlVector2i* dimensions,
	const RmlFontGlyph* glyph);
RMLUI_CAPI_API void rml_font_effect_generate_glyph_texture(const RmlFontEffect* effect, unsigned char* destination,
	RmlVector2i destination_dimensions, int destination_stride, const RmlFontGlyph* glyph);

RMLUI_CAPI_API RmlFontEffectInstancer* rml_font_effect_instancer_create(RmlFontEffectInstanceFunc instance, void* user_data);
RMLUI_CAPI_API void rml_font_effect_instancer_destroy(RmlFontEffectInstancer* instancer);
RMLUI_CAPI_API void rml_register_font_effect_instancer(RmlStringView name, RmlFontEffectInstancer* instancer);
/* affects_generation: whether changing the property requires regenerating the effect's glyph textures. */
RMLUI_CAPI_API RmlPropertyDefinition* rml_font_effect_instancer_register_property(RmlFontEffectInstancer* instancer, RmlStringView name,
	RmlStringView default_value, bool affects_generation);
RMLUI_CAPI_API uint8_t rml_font_effect_instancer_register_shorthand(RmlFontEffectInstancer* instancer, RmlStringView shorthand_name,
	RmlStringView property_names, RmlShorthandType type);

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
