#include "Adapters.h"
#include "Conversions.h"
#include <cstring>

namespace Rml {
namespace CApi {
namespace {

void MoveOwnedStringToRmlString(RmlOwnedString& owned, String& out)
{
	if (owned.data)
	{
		out.assign(owned.data, owned.size);
		rml_owned_string_free(owned);
		owned = {};
	}
	else
	{
		out.clear();
	}
}

} // namespace

SystemInterfaceAdapter::SystemInterfaceAdapter(const RmlSystemInterfaceCallbacks& callbacks, void* user_data) :
	callbacks(callbacks), user_data(user_data)
{}

double SystemInterfaceAdapter::GetElapsedTime()
{
	if (callbacks.get_elapsed_time)
		return callbacks.get_elapsed_time(user_data);
	return SystemInterface::GetElapsedTime();
}

int SystemInterfaceAdapter::TranslateString(String& translated, const String& input)
{
	if (!callbacks.translate_string)
		return SystemInterface::TranslateString(translated, input);

	RmlOwnedString owned = {};
	const int count = callbacks.translate_string(user_data, &owned, ToStringView(input));
	MoveOwnedStringToRmlString(owned, translated);
	return count;
}

void SystemInterfaceAdapter::JoinPath(String& translated_path, const String& document_path, const String& path)
{
	if (!callbacks.join_path)
	{
		SystemInterface::JoinPath(translated_path, document_path, path);
		return;
	}

	RmlOwnedString owned = {};
	callbacks.join_path(user_data, &owned, ToStringView(document_path), ToStringView(path));
	MoveOwnedStringToRmlString(owned, translated_path);
}

bool SystemInterfaceAdapter::LogMessage(Log::Type type, const String& message)
{
	if (!callbacks.log_message)
		return SystemInterface::LogMessage(type, message);
	return callbacks.log_message(user_data, static_cast<RmlLogType>(type), ToStringView(message));
}

void SystemInterfaceAdapter::SetMouseCursor(const String& cursor_name)
{
	if (!callbacks.set_mouse_cursor)
	{
		SystemInterface::SetMouseCursor(cursor_name);
		return;
	}
	callbacks.set_mouse_cursor(user_data, ToStringView(cursor_name));
}

void SystemInterfaceAdapter::SetClipboardText(const String& text)
{
	if (!callbacks.set_clipboard_text)
	{
		SystemInterface::SetClipboardText(text);
		return;
	}
	callbacks.set_clipboard_text(user_data, ToStringView(text));
}

void SystemInterfaceAdapter::GetClipboardText(String& text)
{
	if (!callbacks.get_clipboard_text)
	{
		SystemInterface::GetClipboardText(text);
		return;
	}

	RmlOwnedString owned = {};
	callbacks.get_clipboard_text(user_data, &owned);
	MoveOwnedStringToRmlString(owned, text);
}

void SystemInterfaceAdapter::ActivateKeyboard(Vector2f caret_position, float line_height)
{
	if (!callbacks.activate_keyboard)
	{
		SystemInterface::ActivateKeyboard(caret_position, line_height);
		return;
	}
	callbacks.activate_keyboard(user_data, ToC(caret_position), line_height);
}

void SystemInterfaceAdapter::DeactivateKeyboard()
{
	if (!callbacks.deactivate_keyboard)
		SystemInterface::DeactivateKeyboard();
	else
		callbacks.deactivate_keyboard(user_data);
}

FileInterfaceAdapter::FileInterfaceAdapter(const RmlFileInterfaceCallbacks& callbacks, void* user_data) :
	callbacks(callbacks), user_data(user_data)
{}

FileHandle FileInterfaceAdapter::Open(const String& path)
{
	return callbacks.open(user_data, ToStringView(path));
}

void FileInterfaceAdapter::Close(FileHandle file)
{
	callbacks.close(user_data, file);
}

size_t FileInterfaceAdapter::Read(void* buffer, size_t size, FileHandle file)
{
	return callbacks.read(user_data, buffer, size, file);
}

bool FileInterfaceAdapter::Seek(FileHandle file, long offset, int origin)
{
	return callbacks.seek(user_data, file, offset, origin);
}

size_t FileInterfaceAdapter::Tell(FileHandle file)
{
	return callbacks.tell(user_data, file);
}

size_t FileInterfaceAdapter::Length(FileHandle file)
{
	if (callbacks.length)
		return callbacks.length(user_data, file);
	return FileInterface::Length(file);
}

bool FileInterfaceAdapter::LoadFile(const String& path, String& out_data)
{
	if (!callbacks.load_file)
		return FileInterface::LoadFile(path, out_data);

	RmlOwnedString owned = {};
	const bool loaded = callbacks.load_file(user_data, ToStringView(path), &owned);
	if (!loaded)
	{
		rml_owned_string_free(owned);
		return false;
	}

	MoveOwnedStringToRmlString(owned, out_data);
	return true;
}

RenderInterfaceAdapter::RenderInterfaceAdapter(const RmlRenderInterfaceCallbacks& callbacks, void* user_data) :
	callbacks(callbacks), user_data(user_data)
{}

CompiledGeometryHandle RenderInterfaceAdapter::CompileGeometry(Span<const Vertex> vertices, Span<const int> indices)
{
	return callbacks.compile_geometry(user_data, reinterpret_cast<const RmlVertex*>(vertices.data()), vertices.size(), indices.data(),
		indices.size());
}

void RenderInterfaceAdapter::RenderGeometry(CompiledGeometryHandle geometry, Vector2f translation, TextureHandle texture)
{
	callbacks.render_geometry(user_data, geometry, ToC(translation), texture);
}

void RenderInterfaceAdapter::ReleaseGeometry(CompiledGeometryHandle geometry)
{
	callbacks.release_geometry(user_data, geometry);
}

TextureHandle RenderInterfaceAdapter::LoadTexture(Vector2i& texture_dimensions, const String& source)
{
	RmlVector2i out_dimensions = {};
	const TextureHandle handle = callbacks.load_texture(user_data, &out_dimensions, ToStringView(source));
	texture_dimensions = FromC(out_dimensions);
	return handle;
}

TextureHandle RenderInterfaceAdapter::GenerateTexture(Span<const byte> source, Vector2i source_dimensions)
{
	return callbacks.generate_texture(user_data, source.data(), source.size(), ToC(source_dimensions));
}

void RenderInterfaceAdapter::ReleaseTexture(TextureHandle texture)
{
	callbacks.release_texture(user_data, texture);
}

void RenderInterfaceAdapter::EnableScissorRegion(bool enable)
{
	callbacks.enable_scissor_region(user_data, enable);
}

void RenderInterfaceAdapter::SetScissorRegion(Rectanglei region)
{
	callbacks.set_scissor_region(user_data, ToC(region));
}

void RenderInterfaceAdapter::EnableClipMask(bool enable)
{
	if (!callbacks.enable_clip_mask)
		RenderInterface::EnableClipMask(enable);
	else
		callbacks.enable_clip_mask(user_data, enable);
}

void RenderInterfaceAdapter::RenderToClipMask(ClipMaskOperation operation, CompiledGeometryHandle geometry, Vector2f translation)
{
	if (!callbacks.render_to_clip_mask)
		RenderInterface::RenderToClipMask(operation, geometry, translation);
	else
		callbacks.render_to_clip_mask(user_data, static_cast<RmlClipMaskOperation>(operation), geometry, ToC(translation));
}

void RenderInterfaceAdapter::SetTransform(const Matrix4f* transform)
{
	if (!callbacks.set_transform)
	{
		RenderInterface::SetTransform(transform);
		return;
	}
	if (transform)
	{
		RmlMatrix4f matrix = ToC(*transform);
		callbacks.set_transform(user_data, &matrix);
	}
	else
		callbacks.set_transform(user_data, nullptr);
}

LayerHandle RenderInterfaceAdapter::PushLayer()
{
	if (!callbacks.push_layer)
		return RenderInterface::PushLayer();
	return callbacks.push_layer(user_data);
}

void RenderInterfaceAdapter::CompositeLayers(LayerHandle source, LayerHandle destination, BlendMode blend_mode,
	Span<const CompiledFilterHandle> filters)
{
	if (!callbacks.composite_layers)
	{
		RenderInterface::CompositeLayers(source, destination, blend_mode, filters);
		return;
	}
	callbacks.composite_layers(user_data, source, destination, static_cast<RmlBlendMode>(blend_mode), filters.data(), filters.size());
}

void RenderInterfaceAdapter::PopLayer()
{
	if (!callbacks.pop_layer)
		RenderInterface::PopLayer();
	else
		callbacks.pop_layer(user_data);
}

TextureHandle RenderInterfaceAdapter::SaveLayerAsTexture()
{
	if (!callbacks.save_layer_as_texture)
		return RenderInterface::SaveLayerAsTexture();
	return callbacks.save_layer_as_texture(user_data);
}

CompiledFilterHandle RenderInterfaceAdapter::SaveLayerAsMaskImage()
{
	if (!callbacks.save_layer_as_mask_image)
		return RenderInterface::SaveLayerAsMaskImage();
	return callbacks.save_layer_as_mask_image(user_data);
}

CompiledFilterHandle RenderInterfaceAdapter::CompileFilter(const String& name, const Dictionary& parameters)
{
	if (!callbacks.compile_filter)
		return RenderInterface::CompileFilter(name, parameters);
	return callbacks.compile_filter(user_data, ToStringView(name), reinterpret_cast<const RmlDictionary*>(&parameters));
}

void RenderInterfaceAdapter::ReleaseFilter(CompiledFilterHandle filter)
{
	if (!callbacks.release_filter)
		RenderInterface::ReleaseFilter(filter);
	else
		callbacks.release_filter(user_data, filter);
}

CompiledShaderHandle RenderInterfaceAdapter::CompileShader(const String& name, const Dictionary& parameters)
{
	if (!callbacks.compile_shader)
		return RenderInterface::CompileShader(name, parameters);
	return callbacks.compile_shader(user_data, ToStringView(name), reinterpret_cast<const RmlDictionary*>(&parameters));
}

void RenderInterfaceAdapter::RenderShader(CompiledShaderHandle shader, CompiledGeometryHandle geometry, Vector2f translation, TextureHandle texture)
{
	if (!callbacks.render_shader)
		RenderInterface::RenderShader(shader, geometry, translation, texture);
	else
		callbacks.render_shader(user_data, shader, geometry, ToC(translation), texture);
}

void RenderInterfaceAdapter::ReleaseShader(CompiledShaderHandle shader)
{
	if (!callbacks.release_shader)
		RenderInterface::ReleaseShader(shader);
	else
		callbacks.release_shader(user_data, shader);
}

TextInputHandlerAdapter::TextInputHandlerAdapter(const RmlTextInputHandlerCallbacks& callbacks, void* user_data) :
	callbacks(callbacks), user_data(user_data)
{}

void TextInputHandlerAdapter::OnActivate(TextInputContext* input_context)
{
	if (callbacks.on_activate)
		callbacks.on_activate(user_data, ToC(input_context));
}

void TextInputHandlerAdapter::OnDeactivate(TextInputContext* input_context)
{
	if (callbacks.on_deactivate)
		callbacks.on_deactivate(user_data, ToC(input_context));
}

void TextInputHandlerAdapter::OnDestroy(TextInputContext* input_context)
{
	if (callbacks.on_destroy)
		callbacks.on_destroy(user_data, ToC(input_context));
}

EventListenerAdapter::EventListenerAdapter(const RmlEventListenerCallbacks& callbacks, void* user_data) :
	callbacks(callbacks), user_data(user_data)
{}

void EventListenerAdapter::ProcessEvent(Event& event)
{
	callbacks.process_event(user_data, ToC(&event));
}

void EventListenerAdapter::OnAttach(Element* element)
{
	if (callbacks.on_attach)
		callbacks.on_attach(user_data, ToC(element));
}

void EventListenerAdapter::OnDetach(Element* element)
{
	const auto on_detach = callbacks.on_detach;
	void* data = user_data;
	RmlElement* c_element = ToC(element);
	if (on_detach)
		on_detach(data, c_element);
}

} // namespace CApi
} // namespace Rml