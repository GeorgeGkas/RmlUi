#include "Harness.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <doctest.h>
#include <fstream>
#include <iterator>

namespace CApiTest {

namespace {

	SystemLog& Sys(void* user_data)
	{
		return *static_cast<SystemLog*>(user_data);
	}
	RenderLog& Ren(void* user_data)
	{
		return *static_cast<RenderLog*>(user_data);
	}

	double GetElapsedTime(void* user_data)
	{
		return Sys(user_data).elapsed;
	}
	bool LogMessage(void* user_data, RmlLogType type, RmlStringView message)
	{
		Sys(user_data).messages.emplace_back(type, ToString(message));
		if (type == RML_LOG_TYPE_ERROR || type == RML_LOG_TYPE_ASSERT)
			std::fprintf(stderr, "[rmlui] %s\n", Sys(user_data).messages.back().second.c_str());
		return true;
	}
	void SetMouseCursor(void* user_data, RmlStringView cursor_name)
	{
		Sys(user_data).cursors.push_back(ToString(cursor_name));
	}
	void SetClipboardText(void* user_data, RmlStringView text)
	{
		Sys(user_data).clipboard = ToString(text);
	}
	void GetClipboardText(void* user_data, RmlOwnedString* out_text)
	{
		SystemLog& log = Sys(user_data);
		log.clipboard_reads += 1;
		rml_owned_string_set(out_text, RmlStringView{log.clipboard.data(), log.clipboard.size()});
	}
	void ActivateKeyboard(void* user_data, RmlVector2f, float)
	{
		Sys(user_data).keyboard_activations += 1;
	}
	void DeactivateKeyboard(void* user_data)
	{
		Sys(user_data).keyboard_deactivations += 1;
	}

	RmlCompiledGeometryHandle CompileGeometry(void* user_data, const RmlVertex*, size_t, const int*, size_t)
	{
		return static_cast<RmlCompiledGeometryHandle>(++Ren(user_data).compiled_geometry);
	}
	void RenderGeometry(void* user_data, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle)
	{
		Ren(user_data).rendered_geometry += 1;
	}
	void ReleaseGeometry(void* user_data, RmlCompiledGeometryHandle)
	{
		Ren(user_data).released_geometry += 1;
	}
	RmlTextureHandle LoadTexture(void* user_data, RmlVector2i* out_dimensions, RmlStringView source)
	{
		RenderLog& log = Ren(user_data);
		log.texture_sources.push_back(ToString(source));
		*out_dimensions = RmlVector2i{32, 32};
		return static_cast<RmlTextureHandle>(1000 + log.texture_sources.size());
	}
	RmlTextureHandle GenerateTexture(void* user_data, const unsigned char*, size_t, RmlVector2i)
	{
		return static_cast<RmlTextureHandle>(2000 + ++Ren(user_data).generated_textures);
	}
	void ReleaseTexture(void* user_data, RmlTextureHandle)
	{
		Ren(user_data).released_textures += 1;
	}
	void EnableScissorRegion(void* user_data, bool)
	{
		Ren(user_data).scissor_enables += 1;
	}
	void SetScissorRegion(void*, RmlRectanglei) {}

	void EnableClipMask(void* user_data, bool)
	{
		Ren(user_data).clip_mask_enables += 1;
	}
	void RenderToClipMask(void* user_data, RmlClipMaskOperation, RmlCompiledGeometryHandle, RmlVector2f)
	{
		Ren(user_data).clip_mask_renders += 1;
	}
	void SetTransform(void* user_data, const RmlMatrix4f* transform)
	{
		RenderLog& log = Ren(user_data);
		if (transform)
		{
			log.transforms += 1;
			log.last_transform = *transform;
		}
		else
			log.null_transforms += 1;
	}
	RmlLayerHandle PushLayer(void* user_data)
	{
		return static_cast<RmlLayerHandle>(++Ren(user_data).pushed_layers);
	}
	void CompositeLayers(void* user_data, RmlLayerHandle, RmlLayerHandle, RmlBlendMode, const RmlCompiledFilterHandle* filters, size_t num_filters)
	{
		RenderLog& log = Ren(user_data);
		log.composites += 1;
		for (size_t i = 0; i < num_filters; i++)
			log.composited_filters.push_back(filters[i]);
	}
	void PopLayer(void* user_data)
	{
		Ren(user_data).popped_layers += 1;
	}
	RmlTextureHandle SaveLayerAsTexture(void* user_data)
	{
		return static_cast<RmlTextureHandle>(3000 + ++Ren(user_data).layers_saved_as_texture);
	}
	RmlCompiledFilterHandle SaveLayerAsMaskImage(void* user_data)
	{
		return static_cast<RmlCompiledFilterHandle>(4000 + ++Ren(user_data).layers_saved_as_mask);
	}
	RmlCompiledFilterHandle CompileFilter(void* user_data, RmlStringView name, const RmlDictionary*)
	{
		RenderLog& log = Ren(user_data);
		log.compiled_filters.push_back(ToString(name));
		return static_cast<RmlCompiledFilterHandle>(500 + log.compiled_filters.size());
	}
	void ReleaseFilter(void* user_data, RmlCompiledFilterHandle)
	{
		Ren(user_data).released_filters += 1;
	}
	void CollectKey(void* user_data, RmlStringView key, const RmlVariant*)
	{
		static_cast<std::vector<std::string>*>(user_data)->push_back(ToString(key));
	}
	RmlCompiledShaderHandle CompileShader(void* user_data, RmlStringView name, const RmlDictionary* parameters)
	{
		RenderLog& log = Ren(user_data);
		log.compiled_shaders.push_back(ToString(name));
		rml_dictionary_for_each(parameters, &CollectKey, &log.shader_parameter_keys);
		return static_cast<RmlCompiledShaderHandle>(600 + log.compiled_shaders.size());
	}
	void RenderShader(void* user_data, RmlCompiledShaderHandle, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle)
	{
		Ren(user_data).rendered_shaders += 1;
	}
	void ReleaseShader(void* user_data, RmlCompiledShaderHandle)
	{
		Ren(user_data).released_shaders += 1;
	}

	MemoryFileSystem& Fs(void* user_data)
	{
		return *static_cast<MemoryFileSystem*>(user_data);
	}
	MemoryFileSystem::OpenFile* FindOpen(void* user_data, RmlFileHandle file)
	{
		MemoryFileSystem& fs = Fs(user_data);
		auto it = fs.open.find(file);
		if (it == fs.open.end())
		{
			fs.invalid_handle_calls += 1;
			return nullptr;
		}
		return &it->second;
	}
	RmlFileHandle FsOpen(void* user_data, RmlStringView path)
	{
		MemoryFileSystem& fs = Fs(user_data);
		const std::string key = ToString(path);
		fs.opened_paths.push_back(key);
		auto it = fs.files.find(key);
		if (it == fs.files.end())
			return 0;
		const RmlFileHandle handle = fs.next_handle++;
		fs.open[handle] = MemoryFileSystem::OpenFile{&it->second, 0};
		return handle;
	}
	void FsClose(void* user_data, RmlFileHandle file)
	{
		if (FindOpen(user_data, file))
			Fs(user_data).open.erase(file);
	}
	size_t FsRead(void* user_data, void* buffer, size_t size, RmlFileHandle file)
	{
		MemoryFileSystem::OpenFile* open = FindOpen(user_data, file);
		if (!open)
			return 0;
		const size_t count = std::min(size, open->data->size() - open->position);
		std::memcpy(buffer, open->data->data() + open->position, count);
		open->position += count;
		return count;
	}
	bool FsSeek(void* user_data, RmlFileHandle file, long offset, int origin)
	{
		MemoryFileSystem::OpenFile* open = FindOpen(user_data, file);
		if (!open)
			return false;
		const long size = static_cast<long>(open->data->size());
		const long base = (origin == SEEK_SET ? 0 : origin == SEEK_CUR ? static_cast<long>(open->position) : size);
		const long target = base + offset;
		if (target < 0 || target > size)
			return false;
		open->position = static_cast<size_t>(target);
		return true;
	}
	size_t FsTell(void* user_data, RmlFileHandle file)
	{
		MemoryFileSystem::OpenFile* open = FindOpen(user_data, file);
		return open ? open->position : 0;
	}
	size_t FsLength(void* user_data, RmlFileHandle file)
	{
		Fs(user_data).length_calls += 1;
		MemoryFileSystem::OpenFile* open = FindOpen(user_data, file);
		return open ? open->data->size() : 0;
	}

	void ListenerProcess(void* user_data, RmlEvent* event)
	{
		ListenerLog& log = *static_cast<ListenerLog*>(user_data);
		log.calls += 1;
		log.types.push_back(ToString(rml_event_get_type(event)));
		log.phases.push_back(rml_event_get_phase(event));
		log.targets.push_back(rml_event_get_target_element(event));
		log.current.push_back(rml_event_get_current_element(event));
		if (log.stop_immediate)
			rml_event_stop_immediate_propagation(event);
		else if (log.stop_propagation)
			rml_event_stop_propagation(event);
	}

} // namespace

bool SystemLog::HasMessage(const std::string& needle) const
{
	for (const auto& message : messages)
		if (message.second.find(needle) != std::string::npos)
			return true;
	return false;
}

Session::Session(const char* context_name, const SessionOptions& options) : name(context_name)
{
	RmlSystemInterfaceCallbacks system_callbacks = {};
	system_callbacks.get_elapsed_time = &GetElapsedTime;
	system_callbacks.log_message = &LogMessage;
	system_callbacks.set_mouse_cursor = &SetMouseCursor;
	system_callbacks.set_clipboard_text = &SetClipboardText;
	system_callbacks.get_clipboard_text = &GetClipboardText;
	system_callbacks.activate_keyboard = &ActivateKeyboard;
	system_callbacks.deactivate_keyboard = &DeactivateKeyboard;
	if (options.customize_system)
		options.customize_system(system_callbacks);

	RmlRenderInterfaceCallbacks render_callbacks = {};
	render_callbacks.compile_geometry = &CompileGeometry;
	render_callbacks.render_geometry = &RenderGeometry;
	render_callbacks.release_geometry = &ReleaseGeometry;
	render_callbacks.load_texture = &LoadTexture;
	render_callbacks.generate_texture = &GenerateTexture;
	render_callbacks.release_texture = &ReleaseTexture;
	render_callbacks.enable_scissor_region = &EnableScissorRegion;
	render_callbacks.set_scissor_region = &SetScissorRegion;
	if (options.full_render_interface)
	{
		render_callbacks.enable_clip_mask = &EnableClipMask;
		render_callbacks.render_to_clip_mask = &RenderToClipMask;
		render_callbacks.set_transform = &SetTransform;
		render_callbacks.push_layer = &PushLayer;
		render_callbacks.composite_layers = &CompositeLayers;
		render_callbacks.pop_layer = &PopLayer;
		render_callbacks.save_layer_as_texture = &SaveLayerAsTexture;
		render_callbacks.save_layer_as_mask_image = &SaveLayerAsMaskImage;
		render_callbacks.compile_filter = &CompileFilter;
		render_callbacks.release_filter = &ReleaseFilter;
		render_callbacks.compile_shader = &CompileShader;
		render_callbacks.render_shader = &RenderShader;
		render_callbacks.release_shader = &ReleaseShader;
	}

	system_log.extra = options.system_extra;
	system = rml_system_interface_create(&system_callbacks, &system_log);
	render = rml_render_interface_create(&render_callbacks, &render_log);
	REQUIRE(system);
	REQUIRE(render);
	rml_set_system_interface(system);
	rml_set_render_interface(render);
	if (options.file_interface)
		rml_set_file_interface(options.file_interface);
	if (options.font_engine)
		rml_set_font_engine_interface(options.font_engine);

	REQUIRE(rml_initialise());
	if (options.load_default_font)
	{
		const std::string font = AssetPath("LatoLatin-Regular.ttf");
		REQUIRE(rml_load_font_face(rml_sv(font.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));
	}

	ctx = rml_create_context(rml_sv(name.c_str()), RmlVector2i{800, 600}, nullptr, options.context_text_input_handler);
	REQUIRE(ctx);
}

Session::~Session()
{
	if (ctx)
		rml_remove_context(rml_sv(name.c_str()));
	rml_shutdown();
	if (render)
		rml_render_interface_destroy(render);
	if (system)
		rml_system_interface_destroy(system);
}

RmlElementDocument* Session::Load(const char* rml, bool show, const char* source_url)
{
	RmlElementDocument* document = rml_context_load_document_from_memory(ctx, rml_sv(rml), rml_sv(source_url));
	REQUIRE(document);
	if (show)
		rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	last_document = document;
	Update();
	return document;
}

RmlElement* Session::ById(const char* id)
{
	REQUIRE(last_document);
	return rml_element_get_element_by_id(rml_document_as_element(last_document), rml_sv(id));
}

void Session::Update()
{
	CHECK(rml_context_update(ctx));
}

void Session::Render()
{
	CHECK(rml_context_render(ctx));
}

std::string ToString(RmlStringView view)
{
	if (!view.data || view.size == 0)
		return {};
	return std::string(view.data, view.size);
}

std::string Take(RmlOwnedString owned)
{
	std::string result = owned.data ? std::string(owned.data, owned.size) : std::string();
	rml_owned_string_free(owned);
	return result;
}

std::string AssetPath(const char* file_name)
{
	return std::string(RMLUI_CAPI_TESTS_SAMPLES_ASSETS) + "/" + file_name;
}

std::vector<unsigned char> ReadAsset(const char* file_name)
{
	std::ifstream stream(AssetPath(file_name), std::ios::binary);
	return std::vector<unsigned char>(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
}

int MemoryFileSystem::Opens(const std::string& path) const
{
	return static_cast<int>(std::count(opened_paths.begin(), opened_paths.end(), path));
}

RmlFileInterface* MakeFileInterface(MemoryFileSystem* file_system, bool provide_length)
{
	RmlFileInterfaceCallbacks callbacks = {};
	callbacks.open = &FsOpen;
	callbacks.close = &FsClose;
	callbacks.read = &FsRead;
	callbacks.seek = &FsSeek;
	callbacks.tell = &FsTell;
	callbacks.length = provide_length ? &FsLength : nullptr;
	return rml_file_interface_create(&callbacks, file_system);
}

RmlEventListener* MakeListener(ListenerLog* log)
{
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = &ListenerProcess;
	return rml_event_listener_create(&callbacks, log);
}

} // namespace CApiTest
