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
	RMLUI_ASSERT(iface);
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
	RMLUI_ASSERT(iface);
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
	RMLUI_ASSERT(iface);
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
	RMLUI_ASSERT(handler);
	delete Rml::CApi::FromC(handler);
}