#include <RmlUi/CApi.h>
#include <doctest.h>
#include <cstring>
#include <string>
#include <vector>

namespace {

struct RenderCounters {
	int compile_geometry = 0;
	int render_geometry = 0;
	int release_geometry = 0;
	int load_texture = 0;
	int generate_texture = 0;
	int release_texture = 0;
	int enable_scissor = 0;
	int set_scissor = 0;
};

struct SystemState {
	std::vector<std::string> messages;
	double elapsed = 0.0;
};

double SystemGetElapsedTime(void* user_data)
{
	return static_cast<SystemState*>(user_data)->elapsed;
}

bool SystemLogMessage(void* user_data, RmlLogType /*type*/, RmlStringView message)
{
	SystemState* state = static_cast<SystemState*>(user_data);
	if (message.data && message.size > 0)
		state->messages.emplace_back(message.data, message.size);
	return true;
}

RmlCompiledGeometryHandle RenderCompileGeometry(void* user_data, const RmlVertex* /*vertices*/, size_t /*num_vertices*/, const int* /*indices*/,
	size_t /*num_indices*/)
{
	RenderCounters* counters = static_cast<RenderCounters*>(user_data);
	counters->compile_geometry += 1;
	return static_cast<RmlCompiledGeometryHandle>(counters->compile_geometry);
}

void RenderRenderGeometry(void* user_data, RmlCompiledGeometryHandle /*geometry*/, RmlVector2f /*translation*/, RmlTextureHandle /*texture*/)
{
	static_cast<RenderCounters*>(user_data)->render_geometry += 1;
}

void RenderReleaseGeometry(void* user_data, RmlCompiledGeometryHandle /*geometry*/)
{
	static_cast<RenderCounters*>(user_data)->release_geometry += 1;
}

RmlTextureHandle RenderLoadTexture(void* user_data, RmlVector2i* out_dimensions, RmlStringView /*source*/)
{
	static_cast<RenderCounters*>(user_data)->load_texture += 1;
	if (out_dimensions)
	{
		out_dimensions->x = 64;
		out_dimensions->y = 64;
	}
	return 1;
}

RmlTextureHandle RenderGenerateTexture(void* user_data, const unsigned char* /*source*/, size_t /*source_size*/, RmlVector2i /*source_dimensions*/)
{
	static_cast<RenderCounters*>(user_data)->generate_texture += 1;
	return 2;
}

void RenderReleaseTexture(void* user_data, RmlTextureHandle /*texture*/)
{
	static_cast<RenderCounters*>(user_data)->release_texture += 1;
}

void RenderEnableScissor(void* user_data, bool /*enable*/)
{
	static_cast<RenderCounters*>(user_data)->enable_scissor += 1;
}

void RenderSetScissor(void* user_data, RmlRectanglei /*region*/)
{
	static_cast<RenderCounters*>(user_data)->set_scissor += 1;
}

std::string SamplesAsset(const char* filename)
{
	std::string path = RMLUI_CAPI_TESTS_SAMPLES_ASSETS;
	path.push_back('/');
	path.append(filename);
	return path;
}

} // namespace

TEST_CASE("capi.foundation.headless_load_and_render")
{
	SystemState system_state;
	RenderCounters render_counters;

	RmlSystemInterfaceCallbacks system_callbacks = {};
	system_callbacks.get_elapsed_time = &SystemGetElapsedTime;
	system_callbacks.log_message = &SystemLogMessage;

	RmlRenderInterfaceCallbacks render_callbacks = {};
	render_callbacks.compile_geometry = &RenderCompileGeometry;
	render_callbacks.render_geometry = &RenderRenderGeometry;
	render_callbacks.release_geometry = &RenderReleaseGeometry;
	render_callbacks.load_texture = &RenderLoadTexture;
	render_callbacks.generate_texture = &RenderGenerateTexture;
	render_callbacks.release_texture = &RenderReleaseTexture;
	render_callbacks.enable_scissor_region = &RenderEnableScissor;
	render_callbacks.set_scissor_region = &RenderSetScissor;

	RmlSystemInterface* system_interface = rml_system_interface_create(&system_callbacks, &system_state);
	RmlRenderInterface* render_interface = rml_render_interface_create(&render_callbacks, &render_counters);
	REQUIRE(system_interface);
	REQUIRE(render_interface);

	rml_set_system_interface(system_interface);
	rml_set_render_interface(render_interface);
	REQUIRE(rml_initialise());

	const std::string font_path = SamplesAsset("LatoLatin-Regular.ttf");
	REQUIRE(rml_load_font_face(rml_sv(font_path.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));

	RmlVector2i dimensions = {800, 600};
	RmlContext* ctx = rml_create_context(rml_sv("main"), dimensions, nullptr, nullptr);
	REQUIRE(ctx);
	CHECK(rml_get_num_contexts() == 1);

	const char* document_rml = R"(
		<rml>
		<head>
			<title>C API Foundation</title>
			<style>
				body { font-family: LatoLatin; font-size: 18dp; color: #111; background: #eee; }
				div { width: 200dp; height: 80dp; background: #4af; margin: 40dp; }
			</style>
		</head>
		<body>
			<div>Hello C API</div>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(ctx, rml_sv(document_rml), rml_sv("[capi foundation]"));
	REQUIRE(document);
	CHECK(rml_context_get_num_documents(ctx) == 1);

	RmlStringView title = rml_document_get_title(document);
	REQUIRE(title.data);
	CHECK(std::string(title.data, title.size) == "C API Foundation");

	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	REQUIRE(rml_context_update(ctx));
	REQUIRE(rml_context_render(ctx));

	CHECK(render_counters.compile_geometry > 0);
	CHECK(render_counters.render_geometry > 0);

	REQUIRE(rml_debugger_initialise(ctx));
	rml_debugger_set_visible(true);
	CHECK(rml_debugger_is_visible());
	rml_debugger_shutdown();

	rml_document_close(document);
	REQUIRE(rml_context_update(ctx));
	REQUIRE(rml_remove_context(rml_sv("main")));

	rml_shutdown();
	rml_render_interface_destroy(render_interface);
	rml_system_interface_destroy(system_interface);
}

TEST_CASE("capi.foundation.owned_string_and_version")
{
	SystemState system_state;
	RenderCounters render_counters;

	RmlSystemInterfaceCallbacks system_callbacks = {};
	system_callbacks.get_elapsed_time = &SystemGetElapsedTime;
	system_callbacks.log_message = &SystemLogMessage;

	RmlRenderInterfaceCallbacks render_callbacks = {};
	render_callbacks.compile_geometry = &RenderCompileGeometry;
	render_callbacks.render_geometry = &RenderRenderGeometry;
	render_callbacks.release_geometry = &RenderReleaseGeometry;
	render_callbacks.load_texture = &RenderLoadTexture;
	render_callbacks.generate_texture = &RenderGenerateTexture;
	render_callbacks.release_texture = &RenderReleaseTexture;
	render_callbacks.enable_scissor_region = &RenderEnableScissor;
	render_callbacks.set_scissor_region = &RenderSetScissor;

	RmlSystemInterface* system_interface = rml_system_interface_create(&system_callbacks, &system_state);
	RmlRenderInterface* render_interface = rml_render_interface_create(&render_callbacks, &render_counters);
	REQUIRE(system_interface);
	REQUIRE(render_interface);

	rml_set_system_interface(system_interface);
	rml_set_render_interface(render_interface);
	REQUIRE(rml_initialise());

	RmlOwnedString version = rml_get_version();
	REQUIRE(version.data);
	CHECK(version.size > 0);
	rml_owned_string_free(version);

	rml_shutdown();
	rml_render_interface_destroy(render_interface);
	rml_system_interface_destroy(system_interface);
}
