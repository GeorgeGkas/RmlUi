#include "Adapters.h"
#include "Conversions.h"
#include <RmlUi/CApi/Interfaces.h>
#include <RmlUi/CApi/Log.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Log.h>

namespace {

bool ValidateFileInterfaceCallbacks(const RmlFileInterfaceCallbacks* callbacks)
{
	if (!callbacks)
		return false;
	if (!callbacks->open || !callbacks->close || !callbacks->read || !callbacks->seek || !callbacks->tell)
		return false;
	return true;
}

bool ValidateRenderInterfaceCallbacks(const RmlRenderInterfaceCallbacks* callbacks)
{
	if (!callbacks)
		return false;
	return callbacks->compile_geometry && callbacks->render_geometry && callbacks->release_geometry && callbacks->load_texture &&
		callbacks->generate_texture && callbacks->release_texture && callbacks->enable_scissor_region && callbacks->set_scissor_region;
}

} // namespace

RmlSystemInterface* rml_system_interface_create(const RmlSystemInterfaceCallbacks* callbacks, void* user_data)
{
	if (!callbacks)
	{
		rml_log_message(RML_LOG_TYPE_ERROR, rml_sv("rml_system_interface_create: callbacks must not be NULL"));
		return nullptr;
	}

	auto* adapter = new Rml::CApi::SystemInterfaceAdapter(*callbacks, user_data);
	return Rml::CApi::ToC(static_cast<Rml::SystemInterface*>(adapter));
}

void rml_system_interface_destroy(RmlSystemInterface* iface)
{
	RMLUI_CAPI_REQUIRE_VOID(iface);
	delete Rml::CApi::FromC(iface);
}

RmlFileInterface* rml_file_interface_create(const RmlFileInterfaceCallbacks* callbacks, void* user_data)
{
	if (!ValidateFileInterfaceCallbacks(callbacks))
	{
		rml_log_message(RML_LOG_TYPE_ERROR, rml_sv("rml_file_interface_create: missing required callback"));
		return nullptr;
	}

	auto* adapter = new Rml::CApi::FileInterfaceAdapter(*callbacks, user_data);
	return Rml::CApi::ToC(static_cast<Rml::FileInterface*>(adapter));
}

void rml_file_interface_destroy(RmlFileInterface* iface)
{
	RMLUI_CAPI_REQUIRE_VOID(iface);
	delete Rml::CApi::FromC(iface);
}

RmlRenderInterface* rml_render_interface_create(const RmlRenderInterfaceCallbacks* callbacks, void* user_data)
{
	if (!ValidateRenderInterfaceCallbacks(callbacks))
	{
		rml_log_message(RML_LOG_TYPE_ERROR, rml_sv("rml_render_interface_create: missing required callback"));
		return nullptr;
	}

	auto* adapter = new Rml::CApi::RenderInterfaceAdapter(*callbacks, user_data);
	return Rml::CApi::ToC(static_cast<Rml::RenderInterface*>(adapter));
}

void rml_render_interface_destroy(RmlRenderInterface* iface)
{
	RMLUI_CAPI_REQUIRE_VOID(iface);
	delete Rml::CApi::FromC(iface);
}

RmlTextInputHandler* rml_text_input_handler_create(const RmlTextInputHandlerCallbacks* callbacks, void* user_data)
{
	RmlTextInputHandlerCallbacks empty_callbacks = {};
	if (!callbacks)
		callbacks = &empty_callbacks;

	auto* adapter = new Rml::CApi::TextInputHandlerAdapter(*callbacks, user_data);
	return Rml::CApi::ToC(static_cast<Rml::TextInputHandler*>(adapter));
}

void rml_text_input_handler_destroy(RmlTextInputHandler* handler)
{
	RMLUI_CAPI_REQUIRE_VOID(handler);
	delete Rml::CApi::FromC(handler);
}

static const Rml::TextInputContext* FromC(const RmlTextInputContext* input_context)
{
	return reinterpret_cast<const Rml::TextInputContext*>(input_context);
}

bool rml_text_input_context_get_bounding_box(const RmlTextInputContext* input_context, RmlRectanglef* out_rectangle)
{
	RMLUI_CAPI_REQUIRE(input_context && out_rectangle, false);
	Rml::Rectanglef rectangle;
	if (!FromC(input_context)->GetBoundingBox(rectangle))
		return false;
	*out_rectangle = Rml::CApi::ToC(rectangle);
	return true;
}

void rml_text_input_context_get_selection_range(const RmlTextInputContext* input_context, int* out_start, int* out_end)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	int start = 0, end = 0;
	FromC(input_context)->GetSelectionRange(start, end);
	if (out_start)
		*out_start = start;
	if (out_end)
		*out_end = end;
}

void rml_text_input_context_set_selection_range(RmlTextInputContext* input_context, int start, int end)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	Rml::CApi::FromC(input_context)->SetSelectionRange(start, end);
}

void rml_text_input_context_set_cursor_position(RmlTextInputContext* input_context, int position)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	Rml::CApi::FromC(input_context)->SetCursorPosition(position);
}

void rml_text_input_context_set_text(RmlTextInputContext* input_context, RmlStringView text, int start, int end)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	Rml::CApi::FromC(input_context)->SetText(Rml::StringView(text.data, text.data + (text.data ? text.size : 0)), start, end);
}

void rml_text_input_context_set_composition_range(RmlTextInputContext* input_context, int start, int end)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	Rml::CApi::FromC(input_context)->SetCompositionRange(start, end);
}

void rml_text_input_context_commit_composition(RmlTextInputContext* input_context, RmlStringView composition)
{
	RMLUI_CAPI_REQUIRE_VOID(input_context);
	Rml::CApi::FromC(input_context)->CommitComposition(
		Rml::StringView(composition.data, composition.data + (composition.data ? composition.size : 0)));
}