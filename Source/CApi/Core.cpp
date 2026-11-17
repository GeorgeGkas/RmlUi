#include "Conversions.h"
#include <RmlUi/CApi/Core.h>
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/StyleTypes.h>

bool rml_initialise(void)
{
	return Rml::Initialise();
}

void rml_shutdown(void)
{
	Rml::Shutdown();
	Rml::CApi::NotePluginsShutdown();
}

RmlOwnedString rml_get_version(void)
{
	return Rml::CApi::ToOwnedString(Rml::GetVersion());
}

void rml_set_system_interface(RmlSystemInterface* system_interface)
{
	Rml::SetSystemInterface(Rml::CApi::FromC(system_interface));
}

RmlSystemInterface* rml_get_system_interface(void)
{
	return Rml::CApi::ToC(Rml::GetSystemInterface());
}

void rml_set_render_interface(RmlRenderInterface* render_interface)
{
	Rml::SetRenderInterface(Rml::CApi::FromC(render_interface));
}

RmlRenderInterface* rml_get_render_interface(void)
{
	return Rml::CApi::ToC(Rml::GetRenderInterface());
}

void rml_set_file_interface(RmlFileInterface* file_interface)
{
	Rml::SetFileInterface(Rml::CApi::FromC(file_interface));
}

RmlFileInterface* rml_get_file_interface(void)
{
	return Rml::CApi::ToC(Rml::GetFileInterface());
}

void rml_set_font_engine_interface(RmlFontEngineInterface* font_engine_interface)
{
	Rml::SetFontEngineInterface(Rml::CApi::FromC(font_engine_interface));
}

RmlFontEngineInterface* rml_get_font_engine_interface(void)
{
	return Rml::CApi::ToC(Rml::GetFontEngineInterface());
}

void rml_set_text_input_handler(RmlTextInputHandler* text_input_handler)
{
	Rml::SetTextInputHandler(Rml::CApi::FromC(text_input_handler));
}

RmlTextInputHandler* rml_get_text_input_handler(void)
{
	return Rml::CApi::ToC(Rml::GetTextInputHandler());
}

RmlContext* rml_create_context(RmlStringView name, RmlVector2i dimensions, RmlRenderInterface* render_interface,
	RmlTextInputHandler* text_input_handler)
{
	return Rml::CApi::ToC(Rml::CreateContext(Rml::CApi::ToString(name), Rml::CApi::FromC(dimensions),
		Rml::CApi::FromC(render_interface), Rml::CApi::FromC(text_input_handler)));
}

bool rml_remove_context(RmlStringView name)
{
	return Rml::RemoveContext(Rml::CApi::ToString(name));
}

RmlContext* rml_get_context_by_name(RmlStringView name)
{
	return Rml::CApi::ToC(Rml::GetContext(Rml::CApi::ToString(name)));
}

RmlContext* rml_get_context_by_index(int index)
{
	return Rml::CApi::ToC(Rml::GetContext(index));
}

int rml_get_num_contexts(void)
{
	return Rml::GetNumContexts();
}

bool rml_load_font_face(RmlStringView path, bool fallback, RmlFontWeight weight, int face_index)
{
	return Rml::LoadFontFace(Rml::CApi::ToString(path), fallback, static_cast<Rml::Style::FontWeight>(weight), face_index);
}

bool rml_load_font_face_with_family(RmlStringView path, RmlStringView family, RmlFontStyle style, RmlFontWeight weight, bool fallback,
	int face_index)
{
	return Rml::LoadFontFace(Rml::CApi::ToString(path), Rml::CApi::ToString(family), static_cast<Rml::Style::FontStyle>(style),
		static_cast<Rml::Style::FontWeight>(weight), fallback, face_index);
}

bool rml_load_font_face_from_memory(const void* data, size_t data_size, RmlStringView family, RmlFontStyle style, RmlFontWeight weight,
	bool fallback, int face_index)
{
	RMLUI_CAPI_REQUIRE(data || data_size == 0, {});
	return Rml::LoadFontFace(Rml::Span<const Rml::byte>(static_cast<const Rml::byte*>(data), data_size), Rml::CApi::ToString(family),
		static_cast<Rml::Style::FontStyle>(style), static_cast<Rml::Style::FontWeight>(weight), fallback, face_index);
}

void rml_register_plugin(RmlPlugin* plugin)
{
	RMLUI_CAPI_REQUIRE_VOID(plugin);
	Rml::CApi::NotePluginRegistered(plugin, true);
	Rml::RegisterPlugin(Rml::CApi::FromC(plugin));
}

void rml_unregister_plugin(RmlPlugin* plugin)
{
	RMLUI_CAPI_REQUIRE_VOID(plugin);
	Rml::CApi::NotePluginRegistered(plugin, false);
	Rml::UnregisterPlugin(Rml::CApi::FromC(plugin));
}

uint16_t rml_register_event_type(RmlStringView type, bool interruptible, bool bubbles, RmlDefaultActionPhase default_action_phase)
{
	const Rml::EventId event_id = Rml::RegisterEventType(Rml::CApi::ToString(type), interruptible, bubbles,
		static_cast<Rml::DefaultActionPhase>(default_action_phase));
	return static_cast<uint16_t>(event_id);
}

void rml_release_textures(RmlRenderInterface* render_interface)
{
	Rml::ReleaseTextures(Rml::CApi::FromC(render_interface));
}

bool rml_release_texture(RmlStringView source, RmlRenderInterface* render_interface)
{
	return Rml::ReleaseTexture(Rml::CApi::ToString(source), Rml::CApi::FromC(render_interface));
}

void rml_release_compiled_geometry(RmlRenderInterface* render_interface)
{
	Rml::ReleaseCompiledGeometry(Rml::CApi::FromC(render_interface));
}

void rml_release_font_resources(void)
{
	Rml::ReleaseFontResources();
}

void rml_release_render_managers(void)
{
	Rml::ReleaseRenderManagers();
}

void rml_get_texture_source_list(RmlStringList* out_sources)
{
	RMLUI_CAPI_REQUIRE_VOID(out_sources);
	*Rml::CApi::FromC(out_sources) = Rml::GetTextureSourceList();
}