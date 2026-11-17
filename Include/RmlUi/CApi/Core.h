#ifndef RMLUI_CAPI_CORE_H
#define RMLUI_CAPI_CORE_H

#include "RmlUi/CApi/Types.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API bool rml_initialise(void);
RMLUI_CAPI_API void rml_shutdown(void);

RMLUI_CAPI_API RmlOwnedString rml_get_version(void);

RMLUI_CAPI_API void rml_set_system_interface(RmlSystemInterface* system_interface);
RMLUI_CAPI_API RmlSystemInterface* rml_get_system_interface(void);

RMLUI_CAPI_API void rml_set_render_interface(RmlRenderInterface* render_interface);
RMLUI_CAPI_API RmlRenderInterface* rml_get_render_interface(void);

RMLUI_CAPI_API void rml_set_file_interface(RmlFileInterface* file_interface);
RMLUI_CAPI_API RmlFileInterface* rml_get_file_interface(void);

RMLUI_CAPI_API void rml_set_font_engine_interface(RmlFontEngineInterface* font_engine_interface);
RMLUI_CAPI_API RmlFontEngineInterface* rml_get_font_engine_interface(void);

RMLUI_CAPI_API void rml_set_text_input_handler(RmlTextInputHandler* text_input_handler);
RMLUI_CAPI_API RmlTextInputHandler* rml_get_text_input_handler(void);

RMLUI_CAPI_API RmlContext* rml_create_context(RmlStringView name, RmlVector2i dimensions, RmlRenderInterface* render_interface,
	RmlTextInputHandler* text_input_handler);
RMLUI_CAPI_API bool rml_remove_context(RmlStringView name);
RMLUI_CAPI_API RmlContext* rml_get_context_by_name(RmlStringView name);
RMLUI_CAPI_API RmlContext* rml_get_context_by_index(int index);
RMLUI_CAPI_API int rml_get_num_contexts(void);

/* Font family names are matched case-insensitively: the default font engine stores them in lowercase, so a family
   loaded as "Delicious" is referred to by font-family: delicious or Delicious alike. */
RMLUI_CAPI_API bool rml_load_font_face(RmlStringView path, bool fallback, RmlFontWeight weight, int face_index);
RMLUI_CAPI_API bool rml_load_font_face_with_family(RmlStringView path, RmlStringView family, RmlFontStyle style, RmlFontWeight weight,
	bool fallback, int face_index);
/* data is not copied and must stay valid until after rml_shutdown. */
RMLUI_CAPI_API bool rml_load_font_face_from_memory(const void* data, size_t data_size, RmlStringView family, RmlFontStyle style,
	RmlFontWeight weight, bool fallback, int face_index);

/* The plugin must stay alive while registered. See RmlPluginCallbacks for when its callbacks are called. */
RMLUI_CAPI_API void rml_register_plugin(RmlPlugin* plugin);
RMLUI_CAPI_API void rml_unregister_plugin(RmlPlugin* plugin);

RMLUI_CAPI_API uint16_t rml_register_event_type(RmlStringView type, bool interruptible, bool bubbles,
	RmlDefaultActionPhase default_action_phase);

RMLUI_CAPI_API void rml_release_textures(RmlRenderInterface* render_interface);
RMLUI_CAPI_API bool rml_release_texture(RmlStringView source, RmlRenderInterface* render_interface);
RMLUI_CAPI_API void rml_release_compiled_geometry(RmlRenderInterface* render_interface);
RMLUI_CAPI_API void rml_release_font_resources(void);
RMLUI_CAPI_API void rml_release_render_managers(void);
/* Replaces the contents of out_sources with the sources of all file textures loaded by any render manager. */
RMLUI_CAPI_API void rml_get_texture_source_list(RmlStringList* out_sources);

#ifdef __cplusplus
}
#endif

#endif