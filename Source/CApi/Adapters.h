#pragma once

#include <RmlUi/CApi/Event.h>
#include <RmlUi/CApi/Interfaces.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/TextInputHandler.h>

namespace Rml {
namespace CApi {

class SystemInterfaceAdapter : public SystemInterface {
public:
	SystemInterfaceAdapter(const RmlSystemInterfaceCallbacks& callbacks, void* user_data);

	double GetElapsedTime() override;
	int TranslateString(String& translated, const String& input) override;
	void JoinPath(String& translated_path, const String& document_path, const String& path) override;
	bool LogMessage(Log::Type type, const String& message) override;
	void SetMouseCursor(const String& cursor_name) override;
	void SetClipboardText(const String& text) override;
	void GetClipboardText(String& text) override;
	void ActivateKeyboard(Vector2f caret_position, float line_height) override;
	void DeactivateKeyboard() override;

private:
	RmlSystemInterfaceCallbacks callbacks;
	void* user_data;
};

class FileInterfaceAdapter : public FileInterface {
public:
	FileInterfaceAdapter(const RmlFileInterfaceCallbacks& callbacks, void* user_data);

	FileHandle Open(const String& path) override;
	void Close(FileHandle file) override;
	size_t Read(void* buffer, size_t size, FileHandle file) override;
	bool Seek(FileHandle file, long offset, int origin) override;
	size_t Tell(FileHandle file) override;
	size_t Length(FileHandle file) override;
	bool LoadFile(const String& path, String& out_data) override;

private:
	RmlFileInterfaceCallbacks callbacks;
	void* user_data;
};

class RenderInterfaceAdapter : public RenderInterface {
public:
	RenderInterfaceAdapter(const RmlRenderInterfaceCallbacks& callbacks, void* user_data);

	CompiledGeometryHandle CompileGeometry(Span<const Vertex> vertices, Span<const int> indices) override;
	void RenderGeometry(CompiledGeometryHandle geometry, Vector2f translation, TextureHandle texture) override;
	void ReleaseGeometry(CompiledGeometryHandle geometry) override;
	TextureHandle LoadTexture(Vector2i& texture_dimensions, const String& source) override;
	TextureHandle GenerateTexture(Span<const byte> source, Vector2i source_dimensions) override;
	void ReleaseTexture(TextureHandle texture) override;
	void EnableScissorRegion(bool enable) override;
	void SetScissorRegion(Rectanglei region) override;
	void EnableClipMask(bool enable) override;
	void RenderToClipMask(ClipMaskOperation operation, CompiledGeometryHandle geometry, Vector2f translation) override;
	void SetTransform(const Matrix4f* transform) override;
	LayerHandle PushLayer() override;
	void CompositeLayers(LayerHandle source, LayerHandle destination, BlendMode blend_mode, Span<const CompiledFilterHandle> filters) override;
	void PopLayer() override;
	TextureHandle SaveLayerAsTexture() override;
	CompiledFilterHandle SaveLayerAsMaskImage() override;
	CompiledFilterHandle CompileFilter(const String& name, const Dictionary& parameters) override;
	void ReleaseFilter(CompiledFilterHandle filter) override;
	CompiledShaderHandle CompileShader(const String& name, const Dictionary& parameters) override;
	void RenderShader(CompiledShaderHandle shader, CompiledGeometryHandle geometry, Vector2f translation, TextureHandle texture) override;
	void ReleaseShader(CompiledShaderHandle shader) override;

private:
	RmlRenderInterfaceCallbacks callbacks;
	void* user_data;
};

class TextInputHandlerAdapter : public TextInputHandler {
public:
	TextInputHandlerAdapter(const RmlTextInputHandlerCallbacks& callbacks, void* user_data);

	void OnActivate(TextInputContext* input_context) override;
	void OnDeactivate(TextInputContext* input_context) override;
	void OnDestroy(TextInputContext* input_context) override;

private:
	RmlTextInputHandlerCallbacks callbacks;
	void* user_data;
};

class EventListenerAdapter : public EventListener {
public:
	EventListenerAdapter(const RmlEventListenerCallbacks& callbacks, void* user_data);

	void ProcessEvent(Event& event) override;
	void OnAttach(Element* element) override;
	void OnDetach(Element* element) override;

private:
	RmlEventListenerCallbacks callbacks;
	void* user_data;
};

} // namespace CApi
} // namespace Rml