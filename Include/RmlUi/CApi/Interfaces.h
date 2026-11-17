#ifndef RMLUI_CAPI_INTERFACES_H
#define RMLUI_CAPI_INTERFACES_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RmlSystemInterfaceCallbacks {
	double (*get_elapsed_time)(void* user_data);
	int (*translate_string)(void* user_data, RmlOwnedString* out_translated, RmlStringView input);
	void (*join_path)(void* user_data, RmlOwnedString* out_path, RmlStringView document_path, RmlStringView path);
	bool (*log_message)(void* user_data, RmlLogType type, RmlStringView message);
	void (*set_mouse_cursor)(void* user_data, RmlStringView cursor_name);
	void (*set_clipboard_text)(void* user_data, RmlStringView text);
	void (*get_clipboard_text)(void* user_data, RmlOwnedString* out_text);
	void (*activate_keyboard)(void* user_data, RmlVector2f caret_position, float line_height);
	void (*deactivate_keyboard)(void* user_data);
} RmlSystemInterfaceCallbacks;

typedef struct RmlFileInterfaceCallbacks {
	RmlFileHandle (*open)(void* user_data, RmlStringView path);
	void (*close)(void* user_data, RmlFileHandle file);
	size_t (*read)(void* user_data, void* buffer, size_t size, RmlFileHandle file);
	bool (*seek)(void* user_data, RmlFileHandle file, long offset, int origin);
	size_t (*tell)(void* user_data, RmlFileHandle file);
	size_t (*length)(void* user_data, RmlFileHandle file);
	bool (*load_file)(void* user_data, RmlStringView path, RmlOwnedString* out_data);
} RmlFileInterfaceCallbacks;

typedef struct RmlRenderInterfaceCallbacks {
	RmlCompiledGeometryHandle (*compile_geometry)(void* user_data, const RmlVertex* vertices, size_t num_vertices, const int* indices,
		size_t num_indices);
	void (*render_geometry)(void* user_data, RmlCompiledGeometryHandle geometry, RmlVector2f translation, RmlTextureHandle texture);
	void (*release_geometry)(void* user_data, RmlCompiledGeometryHandle geometry);
	RmlTextureHandle (*load_texture)(void* user_data, RmlVector2i* out_dimensions, RmlStringView source);
	RmlTextureHandle (*generate_texture)(void* user_data, const unsigned char* source, size_t source_size, RmlVector2i source_dimensions);
	void (*release_texture)(void* user_data, RmlTextureHandle texture);
	void (*enable_scissor_region)(void* user_data, bool enable);
	void (*set_scissor_region)(void* user_data, RmlRectanglei region);
	void (*enable_clip_mask)(void* user_data, bool enable);
	void (*render_to_clip_mask)(void* user_data, RmlClipMaskOperation operation, RmlCompiledGeometryHandle geometry, RmlVector2f translation);
	void (*set_transform)(void* user_data, const RmlMatrix4f* transform);
	RmlLayerHandle (*push_layer)(void* user_data);
	void (*composite_layers)(void* user_data, RmlLayerHandle source, RmlLayerHandle destination, RmlBlendMode blend_mode,
		const RmlCompiledFilterHandle* filters, size_t num_filters);
	void (*pop_layer)(void* user_data);
	RmlTextureHandle (*save_layer_as_texture)(void* user_data);
	RmlCompiledFilterHandle (*save_layer_as_mask_image)(void* user_data);
	RmlCompiledFilterHandle (*compile_filter)(void* user_data, RmlStringView name, const RmlDictionary* parameters);
	void (*release_filter)(void* user_data, RmlCompiledFilterHandle filter);
	RmlCompiledShaderHandle (*compile_shader)(void* user_data, RmlStringView name, const RmlDictionary* parameters);
	void (*render_shader)(void* user_data, RmlCompiledShaderHandle shader, RmlCompiledGeometryHandle geometry, RmlVector2f translation,
		RmlTextureHandle texture);
	void (*release_shader)(void* user_data, RmlCompiledShaderHandle shader);
} RmlRenderInterfaceCallbacks;

typedef struct RmlTextInputHandlerCallbacks {
	void (*on_activate)(void* user_data, RmlTextInputContext* input_context);
	void (*on_deactivate)(void* user_data, RmlTextInputContext* input_context);
	void (*on_destroy)(void* user_data, RmlTextInputContext* input_context);
} RmlTextInputHandlerCallbacks;

RMLUI_CAPI_API RmlSystemInterface* rml_system_interface_create(const RmlSystemInterfaceCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_system_interface_destroy(RmlSystemInterface* iface);

RMLUI_CAPI_API RmlFileInterface* rml_file_interface_create(const RmlFileInterfaceCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_file_interface_destroy(RmlFileInterface* iface);

RMLUI_CAPI_API RmlRenderInterface* rml_render_interface_create(const RmlRenderInterfaceCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_render_interface_destroy(RmlRenderInterface* iface);

RMLUI_CAPI_API RmlTextInputHandler* rml_text_input_handler_create(const RmlTextInputHandlerCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_text_input_handler_destroy(RmlTextInputHandler* handler);

typedef struct RmlFontEngineInterfaceCallbacks {
	void (*initialize)(void* user_data);
	void (*shutdown)(void* user_data);
	bool (*load_font_face)(void* user_data, RmlStringView file_name, int face_index, bool fallback_face, RmlFontWeight weight);
	bool (*load_font_face_with_family)(void* user_data, RmlStringView file_name, int face_index, RmlStringView family, RmlFontStyle style,
		RmlFontWeight weight, bool fallback_face);
	bool (*load_font_face_from_memory)(void* user_data, const void* data, size_t data_size, int face_index, RmlStringView family, RmlFontStyle style,
		RmlFontWeight weight, bool fallback_face);
	RmlFontFaceHandle (*get_font_face_handle)(void* user_data, RmlStringView family, RmlFontStyle style, RmlFontWeight weight, int size);
	bool (*get_font_metrics)(void* user_data, RmlFontFaceHandle handle, RmlFontMetrics* out_metrics);
	int (*get_string_width)(void* user_data, RmlFontFaceHandle handle, RmlStringView string, const RmlTextShapingContext* text_shaping_context,
		uint32_t prior_character);
	RmlFontEffectsHandle (*prepare_font_effects)(void* user_data, RmlFontFaceHandle handle, const RmlFontEffect* const* font_effects,
		size_t num_font_effects);
	int (*generate_string)(void* user_data, RmlFontFaceHandle face_handle, RmlFontEffectsHandle font_effects_handle, RmlStringView string,
		RmlVector2f position, RmlColourbPremultiplied colour, float opacity, const RmlTextShapingContext* text_shaping_context,
		RmlTexturedMeshList* out_meshes);
	int (*get_version)(void* user_data, RmlFontFaceHandle handle);
	void (*release_font_resources)(void* user_data);
} RmlFontEngineInterfaceCallbacks;

/* Appends one untextured mesh. Vertices are copied. out_meshes is valid only during generate_string. */
RMLUI_CAPI_API void rml_textured_mesh_list_push(RmlTexturedMeshList* list, const RmlVertex* vertices, size_t num_vertices, const int* indices,
	size_t num_indices);

RMLUI_CAPI_API RmlFontEngineInterface* rml_font_engine_interface_create(const RmlFontEngineInterfaceCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_font_engine_interface_destroy(RmlFontEngineInterface* iface);

#ifdef __cplusplus
}
#endif

#endif