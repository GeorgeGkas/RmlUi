#ifndef RMLUI_CAPI_RENDER_H
#define RMLUI_CAPI_RENDER_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Rendering helpers for custom elements and decorators, wrapping Rml::RenderManager resources.
   Geometry and compiled shaders belong to the render manager of the element they were created for. Destroy them before
   that render manager goes away: before the element's context is removed, rml_release_render_managers or rml_shutdown.
   Decorators usually create them in generate_element_data and destroy them in release_element_data. */

/* --- Texture sources ---
   A texture source loads or generates a texture separately for each render manager that asks for it, and caches it.
   Destroy texture sources before the render managers are released by rml_release_render_managers or rml_shutdown. A
   font engine can destroy its texture sources in its shutdown callback, which rml_shutdown calls first. */

RMLUI_CAPI_API RmlTextureSource* rml_texture_source_create_from_file(RmlStringView source, RmlStringView document_path);

/* Called whenever a render manager needs the texture, including after rml_release_textures. The callback must submit
   the texture exactly once through generator, which is borrowed for this call only, and return true on success. */
typedef bool (*RmlTextureCallback)(void* user_data, const RmlTextureGenerator* generator);
RMLUI_CAPI_API RmlTextureSource* rml_texture_source_create_from_callback(RmlTextureCallback callback, void* user_data);
RMLUI_CAPI_API void rml_texture_source_destroy(RmlTextureSource* source);
/* Returns the texture for the element's render manager, or a zero texture if the element has none. */
RMLUI_CAPI_API RmlTexture rml_texture_source_get_texture(const RmlTextureSource* source, RmlElement* element);

/* source holds 8-bit RGBA pixels with premultiplied alpha, row by row from the top. */
RMLUI_CAPI_API bool rml_texture_generator_generate(const RmlTextureGenerator* generator, const unsigned char* source, size_t source_size,
	RmlVector2i dimensions);
/* Submits a texture created directly through the render backend. Ownership of handle passes to RmlUi, which releases it
   through the render interface. */
RMLUI_CAPI_API void rml_texture_generator_set_handle(const RmlTextureGenerator* generator, RmlTextureHandle handle, RmlVector2i dimensions);

RMLUI_CAPI_API bool rml_texture_is_valid(RmlTexture texture);
/* Loads the texture if needed. Returns zero dimensions for an invalid texture. */
RMLUI_CAPI_API RmlVector2i rml_texture_get_dimensions(RmlTexture texture);

/* --- Geometry --- */

/* Copies the mesh. Returns null when the element has no render manager, as when it is not in a context. */
RMLUI_CAPI_API RmlGeometry* rml_geometry_create(RmlElement* element, const RmlVertex* vertices, size_t num_vertices, const int* indices,
	size_t num_indices);
RMLUI_CAPI_API void rml_geometry_destroy(RmlGeometry* geometry);
/* texture may be zero-initialised and shader may be null. The texture must come from the same render manager. */
RMLUI_CAPI_API void rml_geometry_render(const RmlGeometry* geometry, RmlVector2f translation, RmlTexture texture, const RmlCompiledShader* shader);

/* --- Shaders --- */

/* Compiles a shader through the render interface of the element's render manager. Returns null when the element has no
   render manager or the render interface fails to compile the shader. parameters may be null. */
RMLUI_CAPI_API RmlCompiledShader* rml_compiled_shader_create(RmlElement* element, RmlStringView name, const RmlDictionary* parameters);
RMLUI_CAPI_API void rml_compiled_shader_destroy(RmlCompiledShader* shader);

#ifdef __cplusplus
}
#endif

#endif
