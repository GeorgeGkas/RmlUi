#include "Harness.h"
#include <algorithm>
#include <doctest.h>

using namespace CApiTest;

namespace {

// --- Files served through the C file interface ---

const char* virtual_document = R"(
<rml>
<head>
	<link type="text/rcss" href="style.rcss"/>
	<link type="text/template" href="frame.rml"/>
</head>
<body template="frame">
	<div id="styled"/>
	<span id="text">Text</span>
</body>
</rml>
)";

const char* virtual_style = R"(
body { width: 400px; height: 300px; font-family: LatoLatin; font-size: 16px; }
div { display: block; }
#styled { width: 123px; height: 10px; }
)";

const char* virtual_extra_style = R"(
body { width: 400px; height: 300px; font-family: LatoLatin; }
div { display: block; }
#styled { width: 77px; height: 10px; }
)";

const char* virtual_template = R"(
<template name="frame" content="content">
<head></head>
<body>
	<div id="frame_marker"/>
	<div id="content"/>
</body>
</template>
)";

void RunFileInterfaceScenario(bool provide_length)
{
	MemoryFileSystem fs;
	fs.files["virtual/doc.rml"] = virtual_document;
	fs.files["virtual/style.rcss"] = virtual_style;
	fs.files["virtual/extra.rcss"] = virtual_extra_style;
	fs.files["virtual/frame.rml"] = virtual_template;
	const std::vector<unsigned char> font = ReadAsset("LatoLatin-Regular.ttf");
	REQUIRE_FALSE(font.empty());
	fs.files["virtual/font.ttf"] = std::string(font.begin(), font.end());

	RmlFileInterface* file_interface = MakeFileInterface(&fs, provide_length);
	REQUIRE(file_interface);

	{
		SessionOptions options;
		options.file_interface = file_interface;
		options.load_default_font = false;
		Session session("files", options);
		RmlContext* ctx = session.ctx;
		CHECK(rml_get_file_interface() == file_interface);

		// Binary font data is read through the C file interface.
		REQUIRE(rml_load_font_face(rml_sv("virtual/font.ttf"), false, RML_FONT_WEIGHT_AUTO, 0));

		RmlElementDocument* document = rml_context_load_document(ctx, rml_sv("virtual/doc.rml"));
		REQUIRE(document);
		CHECK(ToString(rml_document_get_source_url(document)) == "virtual/doc.rml");
		rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
		session.Update();

		RmlElement* body = rml_document_as_element(document);
		RmlElement* styled = rml_element_get_element_by_id(body, rml_sv("styled"));
		RmlElement* text = rml_element_get_element_by_id(body, rml_sv("text"));
		REQUIRE(styled);
		REQUIRE(text);
		CHECK(rml_element_get_element_by_id(body, rml_sv("frame_marker")) != nullptr);
		CHECK(rml_element_get_offset_width(styled) == doctest::Approx(123.f));
		CHECK(rml_element_utilities_get_string_width(text, rml_sv("Text"), 0) > 0);

		// Without layout from a context update, update_document lays out the document on its own.
		rml_element_set_property(styled, rml_sv("width"), rml_sv("50px"));
		rml_document_update_document(document);
		CHECK(rml_element_get_offset_width(styled) == doctest::Approx(50.f));
		rml_element_remove_property(styled, rml_sv("width"));

		RmlStyleSheetContainer* extra = rml_style_sheet_container_create_from_file(rml_sv("virtual/extra.rcss"));
		REQUIRE(extra);
		rml_document_set_style_sheet_container(document, extra);
		rml_style_sheet_container_destroy(extra);
		session.Update();
		CHECK(rml_element_get_offset_width(styled) == doctest::Approx(77.f));

		const int document_opens = fs.Opens("virtual/doc.rml");
		rml_document_reload_style_sheet(document);
		session.Update();
		CHECK(fs.Opens("virtual/doc.rml") == document_opens + 1);
		CHECK(rml_element_get_offset_width(styled) == doctest::Approx(123.f));

		// Templates are cached until the cache is cleared.
		const int template_opens = fs.Opens("virtual/frame.rml");
		REQUIRE(rml_context_load_document(ctx, rml_sv("virtual/doc.rml")));
		CHECK(fs.Opens("virtual/frame.rml") == template_opens);
		rml_clear_template_cache();
		rml_clear_style_sheet_cache();
		REQUIRE(rml_context_load_document(ctx, rml_sv("virtual/doc.rml")));
		CHECK(fs.Opens("virtual/frame.rml") == template_opens + 1);

		CHECK(rml_context_load_document(ctx, rml_sv("virtual/missing.rml")) == nullptr);
		CHECK(session.system_log.HasMessage("virtual/missing.rml"));
	}

	CHECK(fs.open.empty());
	CHECK(fs.invalid_handle_calls == 0);
	if (provide_length)
		CHECK(fs.length_calls > 0);
	else
		CHECK(fs.length_calls == 0);
	rml_file_interface_destroy(file_interface);
}

// --- System callbacks ---

int TranslateString(void*, RmlOwnedString* out, RmlStringView input)
{
	if (ToString(input) == "Hello")
	{
		rml_owned_string_set(out, rml_sv("Bonjour"));
		return 1;
	}
	rml_owned_string_set(out, input);
	return 0;
}

void JoinPath(void*, RmlOwnedString* out, RmlStringView document_path, RmlStringView path)
{
	const std::string joined = "joined/" + ToString(path) + "?from=" + ToString(document_path);
	rml_owned_string_set(out, RmlStringView{joined.data(), joined.size()});
}

void InstallTranslateAndJoin(RmlSystemInterfaceCallbacks& callbacks)
{
	callbacks.translate_string = &TranslateString;
	callbacks.join_path = &JoinPath;
}

// --- Text input handler ---

struct TextInputLog {
	int activations = 0;
	int deactivations = 0;
	int destructions = 0;
	RmlTextInputContext* last = nullptr;
};

void OnTextActivate(void* user_data, RmlTextInputContext* context)
{
	TextInputLog* log = static_cast<TextInputLog*>(user_data);
	log->activations += 1;
	log->last = context;
}
void OnTextDeactivate(void* user_data, RmlTextInputContext* context)
{
	TextInputLog* log = static_cast<TextInputLog*>(user_data);
	log->deactivations += 1;
	CHECK(context == log->last);
}
void OnTextDestroy(void* user_data, RmlTextInputContext* context)
{
	TextInputLog* log = static_cast<TextInputLog*>(user_data);
	log->destructions += 1;
	CHECK(context == log->last);
}

RmlTextInputHandler* MakeTextInputHandler(TextInputLog* log)
{
	RmlTextInputHandlerCallbacks callbacks = {};
	callbacks.on_activate = &OnTextActivate;
	callbacks.on_deactivate = &OnTextDeactivate;
	callbacks.on_destroy = &OnTextDestroy;
	return rml_text_input_handler_create(&callbacks, log);
}

// --- Font engine implemented in C ---

struct FontLog {
	int initialized = 0;
	int shut_down = 0;
	int released = 0;
	int file_loads = 0;
	int family_loads = 0;
	int memory_loads = 0;
	std::string last_path;
	std::string last_family;
	RmlFontStyle last_style = RML_FONT_STYLE_NORMAL;
	int last_weight = -1;
	bool last_fallback = false;
	int last_face_index = -1;
	const void* last_data = nullptr;
	size_t last_size = 0;
	int generated = 0;
	int version_calls = 0;
	size_t last_num_effects = 0;
	const RmlFontEffect* last_effect = nullptr;
	const RmlFontEffect* instanced_effect = nullptr;
	int effect_instances = 0;
	std::vector<std::string> requested_families;
};

FontLog& Fl(void* user_data)
{
	return *static_cast<FontLog*>(user_data);
}

void FeInitialize(void* user_data)
{
	Fl(user_data).initialized += 1;
}
void FeShutdown(void* user_data)
{
	Fl(user_data).shut_down += 1;
}
bool FeLoadFile(void* user_data, RmlStringView file_name, int face_index, bool fallback_face, RmlFontWeight weight)
{
	FontLog& log = Fl(user_data);
	log.file_loads += 1;
	log.last_path = ToString(file_name);
	log.last_face_index = face_index;
	log.last_fallback = fallback_face;
	log.last_weight = static_cast<int>(weight);
	return true;
}
bool FeLoadFamily(void* user_data, RmlStringView file_name, int face_index, RmlStringView family, RmlFontStyle style, RmlFontWeight weight,
	bool fallback_face)
{
	FontLog& log = Fl(user_data);
	log.family_loads += 1;
	log.last_path = ToString(file_name);
	log.last_face_index = face_index;
	log.last_family = ToString(family);
	log.last_style = style;
	log.last_weight = static_cast<int>(weight);
	log.last_fallback = fallback_face;
	return true;
}
bool FeLoadMemory(void* user_data, const void* data, size_t data_size, int face_index, RmlStringView family, RmlFontStyle style,
	RmlFontWeight weight, bool fallback_face)
{
	FontLog& log = Fl(user_data);
	log.memory_loads += 1;
	log.last_data = data;
	log.last_size = data_size;
	log.last_face_index = face_index;
	log.last_family = ToString(family);
	log.last_style = style;
	log.last_weight = static_cast<int>(weight);
	log.last_fallback = fallback_face;
	return false;
}
RmlFontFaceHandle FeGetHandle(void* user_data, RmlStringView family, RmlFontStyle, RmlFontWeight, int size)
{
	Fl(user_data).requested_families.push_back(ToString(family));
	return ToString(family) == "fake" ? static_cast<RmlFontFaceHandle>(size) : 0;
}
bool FeMetrics(void*, RmlFontFaceHandle handle, RmlFontMetrics* out)
{
	out->size = static_cast<int>(handle);
	out->ascent = 12.f;
	out->descent = 4.f;
	out->line_spacing = 20.f;
	out->x_height = 8.f;
	out->underline_position = -2.f;
	out->underline_thickness = 1.f;
	out->has_ellipsis = false;
	return true;
}
int FeStringWidth(void*, RmlFontFaceHandle, RmlStringView string, const RmlTextShapingContext*, uint32_t)
{
	return static_cast<int>(string.size) * 8;
}
RmlFontEffectsHandle FePrepareEffects(void* user_data, RmlFontFaceHandle, const RmlFontEffect* const* effects, size_t num_effects)
{
	FontLog& log = Fl(user_data);
	log.last_num_effects = num_effects;
	log.last_effect = num_effects > 0 ? effects[0] : nullptr;
	return num_effects;
}
int FeGenerate(void* user_data, RmlFontFaceHandle, RmlFontEffectsHandle, RmlStringView string, RmlVector2f position, RmlColourbPremultiplied colour,
	float, const RmlTextShapingContext*, RmlTexturedMeshList* out_meshes)
{
	Fl(user_data).generated += 1;
	const float width = static_cast<float>(string.size * 8);
	const RmlVertex vertices[4] = {
		{position, colour, {0, 0}},
		{{position.x + width, position.y}, colour, {1, 0}},
		{{position.x + width, position.y + 16}, colour, {1, 1}},
		{{position.x, position.y + 16}, colour, {0, 1}},
	};
	const int indices[6] = {0, 1, 2, 0, 2, 3};
	rml_textured_mesh_list_push(out_meshes, vertices, 4, indices, 6);
	return static_cast<int>(width);
}
int FeVersion(void* user_data, RmlFontFaceHandle)
{
	Fl(user_data).version_calls += 1;
	return 1;
}
void FeRelease(void* user_data)
{
	Fl(user_data).released += 1;
}

RmlFontEffect* InstanceFontEffect(void* user_data, RmlStringView, const RmlPropertyDictionary*)
{
	FontLog& log = Fl(user_data);
	log.effect_instances += 1;
	RmlFontEffect* effect = rml_font_effect_create();
	log.instanced_effect = effect;
	return effect;
}

} // namespace

TEST_CASE("capi.interfaces.file_interface_serves_documents_styles_templates_and_fonts")
{
	RunFileInterfaceScenario(true);
}

TEST_CASE("capi.interfaces.file_interface_without_length_falls_back_to_seek_and_tell")
{
	RunFileInterfaceScenario(false);
}

TEST_CASE("capi.interfaces.create_validates_required_callbacks")
{
	RmlFileInterfaceCallbacks file_callbacks = {};
	file_callbacks.open = [](void*, RmlStringView) -> RmlFileHandle { return 0; };
	CHECK(rml_file_interface_create(&file_callbacks, nullptr) == nullptr);
	CHECK(rml_file_interface_create(nullptr, nullptr) == nullptr);

	RmlRenderInterfaceCallbacks render_callbacks = {};
	CHECK(rml_render_interface_create(&render_callbacks, nullptr) == nullptr);
	CHECK(rml_render_interface_create(nullptr, nullptr) == nullptr);

	CHECK(rml_system_interface_create(nullptr, nullptr) == nullptr);
	CHECK(rml_font_engine_interface_create(nullptr, nullptr) == nullptr);
	CHECK(rml_plugin_create(nullptr, nullptr) == nullptr);

	RmlTextInputHandler* handler = rml_text_input_handler_create(nullptr, nullptr);
	REQUIRE(handler);
	rml_text_input_handler_destroy(handler);
}

TEST_CASE("capi.interfaces.global_interface_getters_return_installed_interfaces")
{
	Session session("getters");
	CHECK(rml_get_system_interface() == session.system);
	CHECK(rml_get_render_interface() == session.render);
	CHECK(rml_get_file_interface() != nullptr);
	CHECK(rml_get_font_engine_interface() != nullptr);
	CHECK(rml_get_text_input_handler() != nullptr);
}

TEST_CASE("capi.interfaces.system_translate_string_and_join_path")
{
	SessionOptions options;
	options.customize_system = &InstallTranslateAndJoin;
	Session session("system_callbacks", options);
	session.Load(R"(
<rml>
<head><style>body { font-family: LatoLatin; } p { display: block; } img { display: block; }</style></head>
<body><p id="greeting">Hello</p><p id="other">Other</p><img src="pic.png"/></body>
</rml>
)",
		true, "docs/page.rml");

	RmlElement* greeting_text = rml_element_get_first_child(session.ById("greeting"));
	REQUIRE(greeting_text);
	REQUIRE(rml_element_as_text(greeting_text));
	CHECK(ToString(rml_text_get_text(rml_element_as_text(greeting_text))) == "Bonjour");
	RmlElement* other_text = rml_element_get_first_child(session.ById("other"));
	CHECK(ToString(rml_text_get_text(rml_element_as_text(other_text))) == "Other");

	session.Render();
	REQUIRE_FALSE(session.render_log.texture_sources.empty());
	// RmlUi passes the document's directory, not its full source URL.
	CHECK(session.render_log.texture_sources[0] == "joined/pic.png?from=docs/");
}

TEST_CASE("capi.interfaces.render_optional_callbacks_are_forwarded")
{
	SessionOptions options;
	options.full_render_interface = true;
	Session session("render_full", options);
	session.Load(R"(
<rml>
<head>
<style>
	body { width: 400px; height: 600px; font-family: LatoLatin; }
	div { display: block; width: 100px; height: 40px; }
	#transformed { transform: translateX(10px); background-color: red; }
	#filtered { filter: opacity(0.5); background-color: red; }
	#gradient { decorator: linear-gradient(90deg, red, blue); }
	#clipped { border-radius: 10px; overflow: hidden; background-color: blue; }
	#clipped div { width: 200px; height: 200px; background-color: green; }
	#shadow { box-shadow: 2px 2px 4px black; background-color: white; }
	#masked { mask-image: linear-gradient(red, transparent); background-color: red; }
</style>
</head>
<body>
	<div id="transformed"/>
	<div id="filtered"/>
	<div id="gradient"/>
	<div id="clipped"><div/></div>
	<div id="shadow"/>
	<div id="masked"/>
</body>
</rml>
)");
	session.Render();
	const RenderLog& log = session.render_log;

	CHECK(log.transforms > 0);
	CHECK(log.null_transforms > 0);
	const int translation_x = rml_matrix_is_row_major() ? 3 : 12;
	CHECK(log.last_transform.data[translation_x] == doctest::Approx(10.f));

	REQUIRE(std::find(log.compiled_filters.begin(), log.compiled_filters.end(), "opacity") != log.compiled_filters.end());
	CHECK(log.pushed_layers > 0);
	CHECK(log.popped_layers == log.pushed_layers);
	CHECK(log.composites > 0);
	CHECK_FALSE(log.composited_filters.empty());

	REQUIRE(std::find(log.compiled_shaders.begin(), log.compiled_shaders.end(), "linear-gradient") != log.compiled_shaders.end());
	CHECK(std::find(log.shader_parameter_keys.begin(), log.shader_parameter_keys.end(), "color_stop_list") != log.shader_parameter_keys.end());
	CHECK(log.rendered_shaders > 0);

	CHECK(log.clip_mask_enables > 0);
	CHECK(log.clip_mask_renders > 0);
	CHECK(log.layers_saved_as_texture > 0);
	CHECK(log.layers_saved_as_mask > 0);

	rml_context_unload_all_documents(session.ctx);
	session.Update();
	session.Render();
	CHECK(log.released_filters > 0);
	CHECK(log.released_shaders > 0);
}

TEST_CASE("capi.interfaces.release_functions_reach_the_render_interface")
{
	Session session("release");
	session.Load("<rml><head><style>img { display: block; }</style></head><body><img src='pic.png'/><img src='other.png'/></body></rml>");
	session.Render();
	REQUIRE(session.render_log.texture_sources.size() == 2);

	const int released_textures = session.render_log.released_textures;
	CHECK(rml_release_texture(rml_sv(session.render_log.texture_sources[0].c_str()), session.render));
	CHECK(session.render_log.released_textures == released_textures + 1);
	CHECK_FALSE(rml_release_texture(rml_sv("never-loaded.png"), nullptr));

	rml_release_textures(session.render);
	CHECK(session.render_log.released_textures == released_textures + 2);

	const int released_geometry = session.render_log.released_geometry;
	rml_release_compiled_geometry(nullptr);
	CHECK(session.render_log.released_geometry > released_geometry);

	// Rendering again reloads what was released.
	session.Render();
	CHECK(session.render_log.texture_sources.size() == 4);
}

TEST_CASE("capi.interfaces.context_render_interface_released_with_render_managers")
{
	Session session("primary");

	RenderLog secondary_log;
	RmlRenderInterfaceCallbacks callbacks = {};
	callbacks.compile_geometry = [](void* user_data, const RmlVertex*, size_t, const int*, size_t) -> RmlCompiledGeometryHandle {
		return static_cast<RmlCompiledGeometryHandle>(++static_cast<RenderLog*>(user_data)->compiled_geometry);
	};
	callbacks.render_geometry = [](void* user_data, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle) {
		static_cast<RenderLog*>(user_data)->rendered_geometry += 1;
	};
	callbacks.release_geometry = [](void* user_data, RmlCompiledGeometryHandle) { static_cast<RenderLog*>(user_data)->released_geometry += 1; };
	callbacks.load_texture = [](void*, RmlVector2i* dimensions, RmlStringView) -> RmlTextureHandle {
		*dimensions = RmlVector2i{1, 1};
		return 1;
	};
	callbacks.generate_texture = [](void*, const unsigned char*, size_t, RmlVector2i) -> RmlTextureHandle { return 2; };
	callbacks.release_texture = [](void*, RmlTextureHandle) {};
	callbacks.enable_scissor_region = [](void*, bool) {};
	callbacks.set_scissor_region = [](void*, RmlRectanglei) {};
	RmlRenderInterface* secondary = rml_render_interface_create(&callbacks, &secondary_log);
	REQUIRE(secondary);

	RmlContext* other = rml_create_context(rml_sv("secondary"), RmlVector2i{200, 200}, secondary, nullptr);
	REQUIRE(other);
	RmlElementDocument* document =
		rml_context_load_document_from_memory(other, rml_sv("<rml><body style='background-color: red; width: 50px; height: 50px;'/></rml>"),
			rml_sv("secondary.rml"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	CHECK(rml_context_update(other));
	CHECK(rml_context_render(other));
	CHECK(secondary_log.rendered_geometry > 0);
	CHECK(session.render_log.rendered_geometry == 0);

	CHECK(rml_remove_context(rml_sv("secondary")));
	rml_release_render_managers();
	CHECK(secondary_log.released_geometry == secondary_log.compiled_geometry);
	rml_render_interface_destroy(secondary);

	// The primary context is unaffected.
	session.Load("<rml><body style='background-color: red; width: 50px; height: 50px;'/></rml>");
	session.Render();
	CHECK(session.render_log.rendered_geometry > 0);
}

TEST_CASE("capi.interfaces.context_text_input_handler_lifecycle")
{
	TextInputLog log;
	RmlTextInputHandler* handler = MakeTextInputHandler(&log);
	REQUIRE(handler);
	{
		SessionOptions options;
		options.context_text_input_handler = handler;
		Session session("text_input_context", options);
		CHECK(rml_context_get_text_input_handler(session.ctx) == handler);
		CHECK(rml_get_text_input_handler() != handler);

		session.Load("<rml><head><style>input { display: block; width: 100px; height: 20px; }</style></head>"
					 "<body><input id='field' type='text'/></body></rml>");
		RmlElement* field = session.ById("field");
		REQUIRE(field);

		rml_element_focus(field, false);
		CHECK(log.activations == 1);
		CHECK(log.last != nullptr);
		rml_element_blur(field);
		CHECK(log.deactivations == 1);

		RmlElement* removed = rml_element_remove_child(rml_element_get_parent_node(field), field);
		rml_element_release(removed);
		CHECK(log.destructions == 1);
	}
	rml_text_input_handler_destroy(handler);
}

TEST_CASE("capi.interfaces.global_text_input_handler")
{
	TextInputLog log;
	RmlTextInputHandler* handler = MakeTextInputHandler(&log);
	rml_set_text_input_handler(handler);
	{
		Session session("text_input_global");
		CHECK(rml_get_text_input_handler() == handler);
		CHECK(rml_context_get_text_input_handler(session.ctx) == handler);

		session.Load("<rml><head><style>input { display: block; width: 100px; height: 20px; }</style></head>"
					 "<body><input id='field' type='text'/></body></rml>");
		rml_element_focus(session.ById("field"), false);
		CHECK(log.activations == 1);
	}
	CHECK(log.deactivations + log.destructions >= 1);
	rml_text_input_handler_destroy(handler);
}

TEST_CASE("capi.interfaces.font_engine_callbacks_and_font_effects")
{
	FontLog log;
	RmlFontEngineInterfaceCallbacks callbacks = {};
	callbacks.initialize = &FeInitialize;
	callbacks.shutdown = &FeShutdown;
	callbacks.load_font_face = &FeLoadFile;
	callbacks.load_font_face_with_family = &FeLoadFamily;
	callbacks.load_font_face_from_memory = &FeLoadMemory;
	callbacks.get_font_face_handle = &FeGetHandle;
	callbacks.get_font_metrics = &FeMetrics;
	callbacks.get_string_width = &FeStringWidth;
	callbacks.prepare_font_effects = &FePrepareEffects;
	callbacks.generate_string = &FeGenerate;
	callbacks.get_version = &FeVersion;
	callbacks.release_font_resources = &FeRelease;
	RmlFontEngineInterface* engine = rml_font_engine_interface_create(&callbacks, &log);
	REQUIRE(engine);
	RmlFontEffectInstancer* effect_instancer = rml_font_effect_instancer_create(&InstanceFontEffect, &log);
	REQUIRE(effect_instancer);

	{
		SessionOptions options;
		options.font_engine = engine;
		options.load_default_font = false;
		Session session("font_engine", options);
		CHECK(log.initialized == 1);
		CHECK(rml_get_font_engine_interface() == engine);
		rml_register_font_effect_instancer(rml_sv("capi-glow"), effect_instancer);

		CHECK(rml_load_font_face(rml_sv("a.ttf"), true, RML_FONT_WEIGHT_BOLD, 2));
		CHECK(log.file_loads == 1);
		CHECK(log.last_path == "a.ttf");
		CHECK(log.last_fallback);
		CHECK(log.last_weight == 700);
		CHECK(log.last_face_index == 2);

		// Font weights are numeric and not limited to the named enumerators.
		CHECK(rml_load_font_face_with_family(rml_sv("b.ttf"), rml_sv("Fake"), RML_FONT_STYLE_ITALIC, static_cast<RmlFontWeight>(300), false, 1));
		CHECK(log.family_loads == 1);
		CHECK(log.last_path == "b.ttf");
		CHECK(log.last_family == "Fake");
		CHECK(log.last_style == RML_FONT_STYLE_ITALIC);
		CHECK(log.last_weight == 300);
		CHECK_FALSE(log.last_fallback);
		CHECK(log.last_face_index == 1);

		const unsigned char bytes[4] = {1, 2, 3, 4};
		CHECK_FALSE(rml_load_font_face_from_memory(bytes, sizeof(bytes), rml_sv("Mem"), RML_FONT_STYLE_NORMAL, RML_FONT_WEIGHT_AUTO, true, 0));
		CHECK(log.memory_loads == 1);
		CHECK(log.last_data == bytes);
		CHECK(log.last_size == 4);
		CHECK(log.last_family == "Mem");
		CHECK(log.last_fallback);

		session.Load(R"(
<rml>
<head><style>
	body { font-family: Fake; font-size: 20px; }
	span { display: inline-block; }
	#glowing { font-effect: capi-glow(); }
</style></head>
<body><span id="plain">abcd</span><span id="glowing">xy</span></body>
</rml>
)");
		// Family names reach the font engine lowercased.
		REQUIRE_FALSE(log.requested_families.empty());
		CHECK(log.requested_families[0] == "fake");
		CHECK(rml_element_get_font_face_handle(session.ById("plain")) == 20);
		CHECK(rml_element_get_offset_width(session.ById("plain")) == doctest::Approx(32.f));
		CHECK(rml_element_utilities_get_string_width(session.ById("plain"), rml_sv("abc"), 0) == 24);

		session.Render();
		CHECK(log.generated > 0);
		CHECK(log.version_calls > 0);
		CHECK(session.render_log.compiled_geometry > 0);

		CHECK(log.effect_instances == 1);
		CHECK(log.last_num_effects == 1);
		CHECK(log.last_effect == log.instanced_effect);

		rml_release_font_resources();
		CHECK(log.released == 1);
	}
	CHECK(log.shut_down == 1);

	rml_font_effect_instancer_destroy(effect_instancer);
	rml_font_engine_interface_destroy(engine);
}

TEST_CASE("capi.interfaces.default_font_engine_loads_from_memory_and_with_family")
{
	// RmlUi keeps a pointer to in-memory font data until shutdown.
	const std::vector<unsigned char> font = ReadAsset("LatoLatin-Regular.ttf");
	REQUIRE_FALSE(font.empty());

	SessionOptions options;
	options.load_default_font = false;
	Session session("default_fonts", options);

	CHECK(rml_load_font_face_from_memory(font.data(), font.size(), rml_sv("MemoryLato"), RML_FONT_STYLE_NORMAL, RML_FONT_WEIGHT_AUTO, false, 0));
	const std::string path = AssetPath("LatoLatin-Bold.ttf");
	CHECK(rml_load_font_face_with_family(rml_sv(path.c_str()), rml_sv("FamilyLato"), RML_FONT_STYLE_NORMAL, RML_FONT_WEIGHT_BOLD, false, 0));
	CHECK_FALSE(rml_load_font_face(rml_sv("does/not/exist.ttf"), false, RML_FONT_WEIGHT_AUTO, 0));

	session.Load(R"(
<rml>
<head><style>
	body { font-size: 16px; }
	span { display: inline-block; }
	#memory { font-family: MemoryLato; }
	#family { font-family: FamilyLato; font-weight: bold; }
	#missing { font-family: Nope; }
</style></head>
<body><span id="memory">abc</span><span id="family">abc</span><span id="missing">abc</span></body>
</rml>
)");
	CHECK(rml_element_get_font_face_handle(session.ById("memory")) != 0);
	CHECK(rml_element_get_font_face_handle(session.ById("family")) != 0);
	CHECK(rml_element_get_font_face_handle(session.ById("missing")) == 0);
	CHECK(rml_element_get_offset_width(session.ById("memory")) > 0.f);
}

TEST_CASE("capi.interfaces.debugger_can_switch_context")
{
	Session session("debug_primary");
	RmlContext* other = rml_create_context(rml_sv("debug_secondary"), RmlVector2i{300, 300}, nullptr, nullptr);
	REQUIRE(other);

	REQUIRE(rml_debugger_initialise(session.ctx));
	CHECK(rml_debugger_set_context(other));
	rml_debugger_set_visible(true);
	session.Update();
	CHECK(rml_debugger_is_visible());
	CHECK(rml_context_update(other));

	CHECK(rml_debugger_set_context(session.ctx));
	rml_debugger_set_visible(false);
	session.Update();
	CHECK_FALSE(rml_debugger_is_visible());
	rml_debugger_shutdown();

	CHECK(rml_remove_context(rml_sv("debug_secondary")));
}

TEST_CASE("capi.interfaces.debugger_shutdown_releases_hooks_left_in_previous_contexts")
{
	Session session("debug_hooks");
	RmlContext* second = rml_create_context(rml_sv("debug_hooks_second"), RmlVector2i{300, 300}, nullptr, nullptr);
	RmlContext* third = rml_create_context(rml_sv("debug_hooks_third"), RmlVector2i{300, 300}, nullptr, nullptr);
	REQUIRE(second);
	REQUIRE(third);

	// Each switch unloads the hook document from the previous context, which only releases it on its next update.
	REQUIRE(rml_debugger_initialise(session.ctx));
	CHECK(rml_debugger_set_context(second));
	CHECK(rml_debugger_set_context(third));
	CHECK(rml_debugger_set_context(session.ctx));

	CHECK(rml_remove_context(rml_sv("debug_hooks_third")));
	rml_debugger_shutdown();

	CHECK(rml_context_update(second));
	CHECK(rml_remove_context(rml_sv("debug_hooks_second")));
}

TEST_CASE("capi.interfaces.style_sheet_from_missing_file_returns_null")
{
	SUBCASE("custom file interface")
	{
		MemoryFileSystem fs;
		RmlFileInterface* file_interface = MakeFileInterface(&fs);
		SessionOptions options;
		options.file_interface = file_interface;
		options.load_default_font = false;
		{
			Session session("missing_sheet_custom", options);
			CHECK(rml_style_sheet_container_create_from_file(rml_sv("missing.rcss")) == nullptr);
			CHECK(fs.Opens("missing.rcss") == 1);
			// No read, seek, tell, or close on a handle that open never returned.
			CHECK(fs.invalid_handle_calls == 0);
		}
		rml_file_interface_destroy(file_interface);
	}

	SUBCASE("default file interface")
	{
		Session session("missing_sheet_default");
		CHECK(rml_style_sheet_container_create_from_file(rml_sv("does/not/exist.rcss")) == nullptr);
	}
}
