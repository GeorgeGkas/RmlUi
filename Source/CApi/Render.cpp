#include "Conversions.h"
#include <RmlUi/CApi/Render.h>
#include <RmlUi/Core/CallbackTexture.h>
#include <RmlUi/Core/CompiledFilterShader.h>
#include <RmlUi/Core/Geometry.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Mesh.h>
#include <RmlUi/Core/RenderManager.h>
#include <RmlUi/Core/Texture.h>

namespace {

struct TextureSourceBox {
	Rml::UniquePtr<Rml::TextureSource> file;
	Rml::UniquePtr<Rml::CallbackTextureSource> callback;
};

struct GeometryBox {
	Rml::Geometry geometry;
	Rml::RenderManager* render_manager;
};

const Rml::CallbackTextureInterface& FromC(const RmlTextureGenerator* generator)
{
	return *reinterpret_cast<const Rml::CallbackTextureInterface*>(generator);
}

Rml::RenderManager* GetRenderManager(RmlElement* element)
{
	return element ? Rml::CApi::FromC(element)->GetRenderManager() : nullptr;
}

} // namespace

namespace Rml {
namespace CApi {

Mesh MakeMesh(const RmlVertex* vertices, size_t num_vertices, const int* indices, size_t num_indices)
{
	Mesh mesh;
	if (vertices && num_vertices > 0)
	{
		const auto* source = reinterpret_cast<const Vertex*>(vertices);
		mesh.vertices.assign(source, source + num_vertices);
	}
	if (indices && num_indices > 0)
		mesh.indices.assign(indices, indices + num_indices);
	return mesh;
}

Texture GetTexture(const RmlTextureSource* source, RenderManager& render_manager)
{
	const auto& box = *reinterpret_cast<const TextureSourceBox*>(source);
	if (box.file)
		return box.file->GetTexture(render_manager);
	return box.callback->GetTexture(render_manager);
}

} // namespace CApi
} // namespace Rml

RmlTextureSource* rml_texture_source_create_from_file(RmlStringView source, RmlStringView document_path)
{
	auto* box = new TextureSourceBox();
	box->file = Rml::MakeUnique<Rml::TextureSource>(Rml::CApi::ToString(source), Rml::CApi::ToString(document_path));
	return reinterpret_cast<RmlTextureSource*>(box);
}

RmlTextureSource* rml_texture_source_create_from_callback(RmlTextureCallback callback, void* user_data)
{
	RMLUI_CAPI_REQUIRE(callback, nullptr);
	auto* box = new TextureSourceBox();
	box->callback = Rml::MakeUnique<Rml::CallbackTextureSource>([callback, user_data](const Rml::CallbackTextureInterface& texture_interface) {
		return callback(user_data, reinterpret_cast<const RmlTextureGenerator*>(&texture_interface));
	});
	return reinterpret_cast<RmlTextureSource*>(box);
}

void rml_texture_source_destroy(RmlTextureSource* source)
{
	delete reinterpret_cast<TextureSourceBox*>(source);
}

RmlTexture rml_texture_source_get_texture(const RmlTextureSource* source, RmlElement* element)
{
	RMLUI_CAPI_REQUIRE(source && element, {});
	Rml::RenderManager* render_manager = GetRenderManager(element);
	if (!render_manager)
		return {};
	return Rml::CApi::ToC(Rml::CApi::GetTexture(source, *render_manager));
}

bool rml_texture_generator_generate(const RmlTextureGenerator* generator, const unsigned char* source, size_t source_size, RmlVector2i dimensions)
{
	RMLUI_CAPI_REQUIRE(generator && (source || source_size == 0), false);
	return FromC(generator).GenerateTexture(Rml::Span<const Rml::byte>(source, source_size), Rml::CApi::FromC(dimensions));
}

void rml_texture_generator_set_handle(const RmlTextureGenerator* generator, RmlTextureHandle handle, RmlVector2i dimensions)
{
	RMLUI_CAPI_REQUIRE_VOID(generator);
	FromC(generator).SetTextureHandle(static_cast<Rml::TextureHandle>(handle), Rml::CApi::FromC(dimensions));
}

bool rml_texture_is_valid(RmlTexture texture)
{
	return static_cast<bool>(Rml::CApi::FromC(texture));
}

RmlVector2i rml_texture_get_dimensions(RmlTexture texture)
{
	return Rml::CApi::ToC(Rml::CApi::FromC(texture).GetDimensions());
}

RmlGeometry* rml_geometry_create(RmlElement* element, const RmlVertex* vertices, size_t num_vertices, const int* indices, size_t num_indices)
{
	RMLUI_CAPI_REQUIRE(element, nullptr);
	Rml::RenderManager* render_manager = GetRenderManager(element);
	if (!render_manager)
		return nullptr;
	auto* box = new GeometryBox{render_manager->MakeGeometry(Rml::CApi::MakeMesh(vertices, num_vertices, indices, num_indices)), render_manager};
	return reinterpret_cast<RmlGeometry*>(box);
}

void rml_geometry_destroy(RmlGeometry* geometry)
{
	delete reinterpret_cast<GeometryBox*>(geometry);
}

void rml_geometry_render(const RmlGeometry* geometry, RmlVector2f translation, RmlTexture texture, const RmlCompiledShader* shader)
{
	RMLUI_CAPI_REQUIRE_VOID(geometry);
	const auto& box = *reinterpret_cast<const GeometryBox*>(geometry);
	if (texture.internal_render_manager && texture.internal_render_manager != box.render_manager)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "rml_geometry_render: the texture belongs to a different render manager than the geometry.");
		return;
	}
	if (shader)
		box.geometry.Render(Rml::CApi::FromC(translation), Rml::CApi::FromC(texture), *reinterpret_cast<const Rml::CompiledShader*>(shader));
	else
		box.geometry.Render(Rml::CApi::FromC(translation), Rml::CApi::FromC(texture));
}

RmlCompiledShader* rml_compiled_shader_create(RmlElement* element, RmlStringView name, const RmlDictionary* parameters)
{
	RMLUI_CAPI_REQUIRE(element, nullptr);
	Rml::RenderManager* render_manager = GetRenderManager(element);
	if (!render_manager)
		return nullptr;
	Rml::CompiledShader shader =
		render_manager->CompileShader(Rml::CApi::ToString(name), parameters ? *Rml::CApi::FromC(parameters) : Rml::Dictionary());
	if (!shader)
		return nullptr;
	return reinterpret_cast<RmlCompiledShader*>(new Rml::CompiledShader(std::move(shader)));
}

void rml_compiled_shader_destroy(RmlCompiledShader* shader)
{
	delete reinterpret_cast<Rml::CompiledShader*>(shader);
}
