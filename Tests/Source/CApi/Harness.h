#pragma once

#include <RmlUi/CApi.h>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace CApiTest {

struct SystemLog {
	double elapsed = 0.0;
	std::vector<std::pair<RmlLogType, std::string>> messages;
	std::vector<std::string> cursors;
	std::string clipboard;
	int clipboard_reads = 0;
	int keyboard_activations = 0;
	int keyboard_deactivations = 0;
	// Free for callbacks installed through SessionOptions::customize_system.
	void* extra = nullptr;

	bool HasMessage(const std::string& needle) const;
};

struct RenderLog {
	int compiled_geometry = 0;
	int rendered_geometry = 0;
	int released_geometry = 0;
	std::vector<std::string> texture_sources;
	int generated_textures = 0;
	int released_textures = 0;
	int scissor_enables = 0;
	int clip_mask_enables = 0;
	int clip_mask_renders = 0;
	int transforms = 0;
	int null_transforms = 0;
	RmlMatrix4f last_transform = {};
	int pushed_layers = 0;
	int popped_layers = 0;
	int composites = 0;
	std::vector<RmlCompiledFilterHandle> composited_filters;
	int layers_saved_as_texture = 0;
	int layers_saved_as_mask = 0;
	std::vector<std::string> compiled_filters;
	int released_filters = 0;
	std::vector<std::string> compiled_shaders;
	std::vector<std::string> shader_parameter_keys;
	int rendered_shaders = 0;
	int released_shaders = 0;
};

struct SessionOptions {
	// Installs every optional render callback so layer, filter, shader and clip-mask paths are observable.
	bool full_render_interface = false;
	bool load_default_font = true;
	RmlFileInterface* file_interface = nullptr;
	RmlFontEngineInterface* font_engine = nullptr;
	RmlTextInputHandler* context_text_input_handler = nullptr;
	// Applied to the system callbacks before the system interface is created. Their user_data is the session's SystemLog.
	void (*customize_system)(RmlSystemInterfaceCallbacks& callbacks) = nullptr;
	void* system_extra = nullptr;
};

// Owns one initialised RmlUi instance with a single context. Shuts everything down on destruction.
class Session {
public:
	explicit Session(const char* context_name, const SessionOptions& options = {});
	~Session();
	Session(const Session&) = delete;
	Session& operator=(const Session&) = delete;

	// Loads a document from memory, optionally shows it, and runs one context update.
	RmlElementDocument* Load(const char* rml, bool show = true, const char* source_url = "capi-test.rml");
	RmlElement* ById(const char* id);
	void Update();
	void Render();

	RmlContext* ctx = nullptr;
	RmlSystemInterface* system = nullptr;
	RmlRenderInterface* render = nullptr;
	SystemLog system_log;
	RenderLog render_log;

private:
	std::string name;
	RmlElementDocument* last_document = nullptr;
};

std::string ToString(RmlStringView view);
// Copies and frees an owned string.
std::string Take(RmlOwnedString owned);
std::string AssetPath(const char* file_name);
std::vector<unsigned char> ReadAsset(const char* file_name);

// Read-only in-memory files served through the C file interface.
struct MemoryFileSystem {
	struct OpenFile {
		const std::string* data = nullptr;
		size_t position = 0;
	};

	std::map<std::string, std::string> files;
	std::map<RmlFileHandle, OpenFile> open;
	RmlFileHandle next_handle = 1;
	std::vector<std::string> opened_paths;
	int length_calls = 0;
	// Calls made with a handle that open never returned (including 0).
	int invalid_handle_calls = 0;

	int Opens(const std::string& path) const;
};
RmlFileInterface* MakeFileInterface(MemoryFileSystem* file_system, bool provide_length = true);

// Creates an event listener that counts invocations into a ListenerLog.
struct ListenerLog {
	int calls = 0;
	std::vector<std::string> types;
	std::vector<RmlEventPhase> phases;
	std::vector<RmlElement*> targets;
	std::vector<RmlElement*> current;
	bool stop_propagation = false;
	bool stop_immediate = false;
};
RmlEventListener* MakeListener(ListenerLog* log);

} // namespace CApiTest
