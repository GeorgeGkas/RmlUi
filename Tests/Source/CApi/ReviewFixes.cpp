#include <RmlUi/CApi.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/FontEngineInterface.h>
#include <doctest.h>
#include <string>

namespace {

struct RenderState {
	std::string filter_name;
	int filter_sigma = -1;
};

double Elapsed(void*)
{
	return 0.0;
}
bool LogMessage(void*, RmlLogType, RmlStringView)
{
	return true;
}
RmlCompiledGeometryHandle CompileGeometry(void*, const RmlVertex*, size_t, const int*, size_t)
{
	return 1;
}
void NoopRender(void*, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle) {}
void NoopRelease(void*, RmlCompiledGeometryHandle) {}
RmlTextureHandle LoadTexture(void*, RmlVector2i* dimensions, RmlStringView)
{
	if (dimensions)
	{
		dimensions->x = 64;
		dimensions->y = 64;
	}
	return 1;
}
RmlTextureHandle GenerateTexture(void*, const unsigned char*, size_t, RmlVector2i)
{
	return 2;
}
void NoopTexture(void*, RmlTextureHandle) {}
void NoopScissor(void*, bool) {}
void NoopRegion(void*, RmlRectanglei) {}

RmlCompiledFilterHandle CompileRenderFilter(void* user_data, RmlStringView name, const RmlDictionary* parameters)
{
	RenderState* state = static_cast<RenderState*>(user_data);
	if (name.data)
		state->filter_name.assign(name.data, name.size);
	const RmlVariant* sigma = rml_dictionary_get(parameters, rml_sv("sigma"));
	float value = 0.f;
	if (sigma && rml_variant_get_float(sigma, &value))
		state->filter_sigma = static_cast<int>(value);
	return 9;
}

struct Session {
	RmlSystemInterface* system = nullptr;
	RmlRenderInterface* render = nullptr;
	RmlContext* ctx = nullptr;
	RenderState render_state;

	void Open(const char* name)
	{
		RmlSystemInterfaceCallbacks system_callbacks = {};
		system_callbacks.get_elapsed_time = &Elapsed;
		system_callbacks.log_message = &LogMessage;
		RmlRenderInterfaceCallbacks render_callbacks = {};
		render_callbacks.compile_geometry = &CompileGeometry;
		render_callbacks.render_geometry = &NoopRender;
		render_callbacks.release_geometry = &NoopRelease;
		render_callbacks.load_texture = &LoadTexture;
		render_callbacks.generate_texture = &GenerateTexture;
		render_callbacks.release_texture = &NoopTexture;
		render_callbacks.enable_scissor_region = &NoopScissor;
		render_callbacks.set_scissor_region = &NoopRegion;
		render_callbacks.compile_filter = &CompileRenderFilter;

		system = rml_system_interface_create(&system_callbacks, nullptr);
		render = rml_render_interface_create(&render_callbacks, &render_state);
		rml_set_system_interface(system);
		rml_set_render_interface(render);
		REQUIRE(rml_initialise());
		std::string font = std::string(RMLUI_CAPI_TESTS_SAMPLES_ASSETS) + "/LatoLatin-Regular.ttf";
		REQUIRE(rml_load_font_face(rml_sv(font.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));
		ctx = rml_create_context(rml_sv(name), RmlVector2i{800, 600}, nullptr, nullptr);
		REQUIRE(ctx);
	}

	void Close(const char* name)
	{
		if (ctx)
			rml_remove_context(rml_sv(name));
		rml_shutdown();
		if (render)
			rml_render_interface_destroy(render);
		if (system)
			rml_system_interface_destroy(system);
		ctx = nullptr;
		render = nullptr;
		system = nullptr;
	}
};

std::string ViewString(RmlStringView view)
{
	if (!view.data || view.size == 0)
		return {};
	return std::string(view.data, view.size);
}

struct ElementState {
	int attribute_changes = 0;
	int property_changes = 0;
	int default_actions = 0;
	bool saw_color = false;
	uint8_t color_id = 0;
};

void OnAttribute(void* user_data, RmlElement*, const RmlDictionary*)
{
	static_cast<ElementState*>(user_data)->attribute_changes += 1;
}
void OnProperty(void* user_data, RmlElement*, const RmlPropertyIdSet* changed)
{
	ElementState* state = static_cast<ElementState*>(user_data);
	state->property_changes += 1;
	if (rml_property_id_set_contains(changed, state->color_id))
		state->saw_color = true;
}
void OnDefaultAction(void* user_data, RmlElement*, RmlEvent*)
{
	static_cast<ElementState*>(user_data)->default_actions += 1;
}

struct FileState {
	int calls = 0;
};
bool LoadFileCallback(void* user_data, RmlStringView, RmlOwnedString* out_data)
{
	FileState* state = static_cast<FileState*>(user_data);
	state->calls += 1;
	rml_owned_string_set(out_data, rml_sv("payload"));
	return state->calls > 1;
}
RmlFileHandle FileOpen(void*, RmlStringView)
{
	return 1;
}
void FileClose(void*, RmlFileHandle) {}
size_t FileRead(void*, void*, size_t, RmlFileHandle)
{
	return 0;
}
bool FileSeek(void*, RmlFileHandle, long, int)
{
	return false;
}
size_t FileTell(void*, RmlFileHandle)
{
	return 0;
}

struct FontState {
	float letter_spacing = -1.f;
	int prepare_calls = 0;
	int generate_calls = 0;
	RmlFontEffectsHandle generated_effects = 0;
};
bool FontLoad(void*, RmlStringView, int, bool, RmlFontWeight)
{
	return true;
}
bool FontMetrics(void*, RmlFontFaceHandle, RmlFontMetrics* out_metrics)
{
	out_metrics->size = 16;
	out_metrics->ascent = 12.f;
	out_metrics->descent = 4.f;
	out_metrics->line_spacing = 18.f;
	out_metrics->x_height = 8.f;
	out_metrics->underline_position = 2.f;
	out_metrics->underline_thickness = 1.f;
	out_metrics->has_ellipsis = true;
	return true;
}
RmlFontFaceHandle FontFace(void*, RmlStringView, RmlFontStyle, RmlFontWeight, int)
{
	return 1;
}
int FontWidth(void* user_data, RmlFontFaceHandle, RmlStringView, const RmlTextShapingContext* shaping, uint32_t prior_character)
{
	FontState* state = static_cast<FontState*>(user_data);
	if (shaping)
		state->letter_spacing = shaping->letter_spacing;
	return prior_character == 0 ? 10 : 40;
}
RmlFontEffectsHandle FontPrepare(void* user_data, RmlFontFaceHandle, const RmlFontEffect* const*, size_t)
{
	static_cast<FontState*>(user_data)->prepare_calls += 1;
	return 7;
}
int FontGenerate(void* user_data, RmlFontFaceHandle, RmlFontEffectsHandle font_effects_handle, RmlStringView, RmlVector2f, RmlColourbPremultiplied,
	float, const RmlTextShapingContext*, RmlTexturedMeshList* out_meshes)
{
	FontState* state = static_cast<FontState*>(user_data);
	state->generate_calls += 1;
	state->generated_effects = font_effects_handle;
	RmlVertex vertices[3] = {};
	const int indices[3] = {0, 1, 2};
	rml_textured_mesh_list_push(out_meshes, vertices, 3, indices, 3);
	return 12;
}

struct EventState {
	int instanced = 0;
	int released = 0;
	int marker = 0;
};
RmlEvent* InstanceMarkedEvent(void* user_data, RmlElement* target, uint16_t id, RmlStringView type, const RmlDictionary*, bool interruptible)
{
	static_cast<EventState*>(user_data)->instanced += 1;
	RmlDictionary* parameters = rml_dictionary_create();
	RmlVariant* marker = rml_variant_create();
	rml_variant_set_int(marker, 42);
	rml_dictionary_set(parameters, rml_sv("marker"), marker);
	RmlEvent* event = rml_event_create(target, id, type, parameters, interruptible);
	rml_variant_destroy(marker);
	rml_dictionary_destroy(parameters);
	return event;
}
void ReleaseMarkedEvent(void* user_data, RmlEvent*)
{
	static_cast<EventState*>(user_data)->released += 1;
}
void OnMarkedClick(void* user_data, RmlEvent* event)
{
	const RmlVariant* marker = rml_dictionary_get(rml_event_get_parameters(event), rml_sv("marker"));
	int value = 0;
	if (marker && rml_variant_get_int(marker, &value))
		static_cast<EventState*>(user_data)->marker = value;
}

bool CompileCapiFilter(void*, RmlElement*, RmlOwnedString* out_name, RmlDictionary* out_parameters)
{
	rml_owned_string_set(out_name, rml_sv("capi-tint"));
	RmlVariant* sigma = rml_variant_create();
	rml_variant_set_float(sigma, 3.f);
	rml_dictionary_set(out_parameters, rml_sv("sigma"), sigma);
	rml_variant_destroy(sigma);
	return true;
}
RmlFilter* InstanceCapiFilter(void* user_data, RmlStringView, const RmlPropertyDictionary*)
{
	RmlFilterCallbacks callbacks = {};
	callbacks.compile_filter = &CompileCapiFilter;
	return rml_filter_create(&callbacks, user_data);
}

int g_plugin_shutdowns = 0;
void OnPluginShutdown(void*)
{
	g_plugin_shutdowns += 1;
}

} // namespace

TEST_CASE("capi.review.file_load_frees_owned_string_when_callback_returns_false")
{
	FileState state;
	RmlFileInterfaceCallbacks callbacks = {};
	callbacks.open = &FileOpen;
	callbacks.close = &FileClose;
	callbacks.read = &FileRead;
	callbacks.seek = &FileSeek;
	callbacks.tell = &FileTell;
	callbacks.load_file = &LoadFileCallback;
	RmlFileInterface* iface = rml_file_interface_create(&callbacks, &state);
	REQUIRE(iface);

	auto* file = reinterpret_cast<Rml::FileInterface*>(iface);
	Rml::String data = "keep";
	CHECK_FALSE(file->LoadFile("missing", data));
	CHECK(data == "keep");
	CHECK(file->LoadFile("missing", data));
	CHECK(data == "payload");
	CHECK(state.calls == 2);

	rml_file_interface_destroy(iface);
}

TEST_CASE("capi.review.custom_element_keeps_base_attribute_property_and_default_action")
{
	Session session;
	session.Open("review-element");

	ElementState state;
	state.color_id = rml_style_sheet_get_property_id(rml_sv("color"));
	REQUIRE(state.color_id != 0);

	RmlElementCallbacks callbacks = {};
	callbacks.on_attribute_change = &OnAttribute;
	callbacks.on_property_change = &OnProperty;
	callbacks.process_default_action = &OnDefaultAction;
	RmlElement* element = rml_element_create_custom(rml_sv("div"), &callbacks, &state);
	REQUIRE(element);
	rml_element_set_attribute_string(element, rml_sv("id"), rml_sv("mine"));
	rml_element_set_attribute_string(element, rml_sv("class"), rml_sv("marked"));
	CHECK(ViewString(rml_element_get_id(element)) == "mine");
	CHECK(rml_element_is_class_set(element, rml_sv("marked")));
	CHECK(state.attribute_changes >= 2);

	const char* document_rml = R"(
		<rml><head><style>body { font-family: LatoLatin; width: 200px; height: 100px; }</style></head><body></body></rml>
	)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[review]"));
	REQUIRE(document);
	RmlElement* body = rml_document_as_element(document);
	REQUIRE(rml_element_append_child(body, element) == element);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	REQUIRE(rml_context_update(session.ctx));

	CHECK(rml_element_set_property(element, rml_sv("color"), rml_sv("#123456")));
	CHECK(state.property_changes >= 1);
	CHECK(state.saw_color);

	CHECK(rml_element_focus(element, true));
	CHECK(rml_element_is_pseudo_class_set(element, rml_sv("focus")));
	CHECK(state.default_actions >= 1);

	session.Close("review-element");
}

TEST_CASE("capi.review.parented_element_is_not_adopted_or_released")
{
	Session session;
	session.Open("review-own");

	const char* document_rml = R"(<rml><head></head><body></body></rml>)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[own]"));
	REQUIRE(document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* child = rml_document_create_element(document, rml_sv("div"));
	rml_element_set_attribute_string(child, rml_sv("id"), rml_sv("kept"));
	REQUIRE(rml_element_append_child(body, child) == child);

	RmlElement* other = rml_document_create_element(document, rml_sv("div"));
	REQUIRE(rml_element_append_child(body, other) == other);
	CHECK(rml_element_append_child(other, child) == nullptr);
	CHECK(rml_element_insert_before(other, child, other) == nullptr);
	CHECK(rml_element_get_parent_node(child) == body);
	CHECK(ViewString(rml_element_get_id(child)) == "kept");

	rml_element_release(child);
	CHECK(rml_element_get_parent_node(child) == body);
	CHECK(ViewString(rml_element_get_id(child)) == "kept");

	RmlElement* removed = rml_element_remove_child(body, child);
	REQUIRE(removed == child);
	rml_element_release(removed);

	session.Close("review-own");
}

TEST_CASE("capi.review.plugin_destroy_unregisters_before_delete")
{
	g_plugin_shutdowns = 0;
	RmlPluginCallbacks callbacks = {};
	callbacks.on_shutdown = &OnPluginShutdown;
	RmlPlugin* plugin = rml_plugin_create(&callbacks, nullptr);
	rml_register_plugin(plugin);

	RmlSystemInterfaceCallbacks system_callbacks = {};
	system_callbacks.get_elapsed_time = &Elapsed;
	system_callbacks.log_message = &LogMessage;
	RmlRenderInterfaceCallbacks render_callbacks = {};
	render_callbacks.compile_geometry = &CompileGeometry;
	render_callbacks.render_geometry = &NoopRender;
	render_callbacks.release_geometry = &NoopRelease;
	render_callbacks.load_texture = &LoadTexture;
	render_callbacks.generate_texture = &GenerateTexture;
	render_callbacks.release_texture = &NoopTexture;
	render_callbacks.enable_scissor_region = &NoopScissor;
	render_callbacks.set_scissor_region = &NoopRegion;
	RmlSystemInterface* system = rml_system_interface_create(&system_callbacks, nullptr);
	RmlRenderInterface* render = rml_render_interface_create(&render_callbacks, nullptr);
	rml_set_system_interface(system);
	rml_set_render_interface(render);
	REQUIRE(rml_initialise());

	rml_plugin_destroy(plugin);
	CHECK(g_plugin_shutdowns == 1);
	rml_shutdown();
	CHECK(g_plugin_shutdowns == 1);
	rml_render_interface_destroy(render);
	rml_system_interface_destroy(system);
}

TEST_CASE("capi.review.event_instancer_uses_returned_event_and_release_does_not_free")
{
	Session session;
	session.Open("review-event");
	EventState state;

	RmlEventInstancerCallbacks callbacks = {};
	callbacks.instance = &InstanceMarkedEvent;
	callbacks.release = &ReleaseMarkedEvent;
	RmlEventInstancer* instancer = rml_event_instancer_create(&callbacks, &state);
	rml_register_event_instancer(instancer);

	const char* document_rml = R"(<rml><head></head><body><button id="go">Go</button></body></rml>)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[event]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	RmlElement* button = rml_element_get_element_by_id(rml_document_as_element(document), rml_sv("go"));
	REQUIRE(button);

	RmlEventListenerCallbacks listener_callbacks = {};
	listener_callbacks.process_event = &OnMarkedClick;
	RmlEventListener* listener = rml_event_listener_create(&listener_callbacks, &state);
	rml_element_add_event_listener(button, rml_sv("click"), listener, false);
	rml_element_click(button);
	CHECK(state.marker == 42);
	CHECK(state.instanced >= 1);
	CHECK(state.released >= 1);

	session.Close("review-event");
	rml_event_listener_destroy(listener);
	rml_event_instancer_destroy(instancer);
}

TEST_CASE("capi.review.font_width_uses_prior_character_and_string_generation_is_forwarded")
{
	FontState state;
	RmlFontEngineInterfaceCallbacks font_callbacks = {};
	font_callbacks.load_font_face = &FontLoad;
	font_callbacks.get_font_face_handle = &FontFace;
	font_callbacks.get_font_metrics = &FontMetrics;
	font_callbacks.get_string_width = &FontWidth;
	font_callbacks.prepare_font_effects = &FontPrepare;
	font_callbacks.generate_string = &FontGenerate;
	RmlFontEngineInterface* font_engine = rml_font_engine_interface_create(&font_callbacks, &state);
	RmlFontEngineInterface* previous = rml_get_font_engine_interface();
	rml_set_font_engine_interface(font_engine);

	Session session;
	session.Open("review-font");

	const char* document_rml = R"(
		<rml>
		<head><style>body { font-family: TestFace; font-size: 16px; letter-spacing: 4px; width: 300px; height: 80px; }</style></head>
		<body>Hi</body>
		</rml>
	)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[font]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	REQUIRE(rml_context_update(session.ctx));

	RmlElement* body = rml_document_as_element(document);
	REQUIRE(rml_element_get_font_face_handle(body) != 0);
	const int plain = rml_element_utilities_get_string_width(body, rml_sv("V"), 0);
	const int kerned = rml_element_utilities_get_string_width(body, rml_sv("V"), static_cast<uint32_t>('A'));
	CHECK(plain == 10);
	CHECK(kerned == 40);
	CHECK(state.letter_spacing > 0.f);

	REQUIRE(rml_context_render(session.ctx));
	CHECK(state.prepare_calls >= 1);
	CHECK(state.generate_calls >= 1);
	CHECK(state.generated_effects == 7);

	session.Close("review-font");
	rml_set_font_engine_interface(previous);
	rml_font_engine_interface_destroy(font_engine);
}

bool MetricsForHandle(void*, RmlFontFaceHandle handle, RmlFontMetrics* out_metrics)
{
	if (handle == 1)
	{
		out_metrics->size = 40;
		out_metrics->ascent = 30.f;
		out_metrics->descent = 10.f;
		out_metrics->line_spacing = 48.f;
		out_metrics->x_height = 20.f;
		out_metrics->underline_position = 2.f;
		out_metrics->underline_thickness = 1.f;
		out_metrics->has_ellipsis = true;
		return true;
	}
	if (handle == 2)
	{
		out_metrics->size = 20;
		out_metrics->ascent = 8.f;
		out_metrics->descent = 2.f;
		out_metrics->line_spacing = 24.f;
		out_metrics->x_height = 6.f;
		out_metrics->underline_position = 1.f;
		out_metrics->underline_thickness = 1.f;
		out_metrics->has_ellipsis = false;
		return true;
	}
	return false;
}

RmlFontFaceHandle FaceForFamily(void*, RmlStringView family, RmlFontStyle, RmlFontWeight, int)
{
	if (family.data && family.size >= 5 && std::string(family.data, family.size).find("child") != std::string::npos)
		return 2;
	return 1;
}

TEST_CASE("capi.review.font_metrics_stay_valid_across_another_face")
{
	RmlFontEngineInterfaceCallbacks font_callbacks = {};
	font_callbacks.load_font_face = &FontLoad;
	font_callbacks.get_font_face_handle = &FaceForFamily;
	font_callbacks.get_font_metrics = &MetricsForHandle;
	font_callbacks.get_string_width = &FontWidth;
	font_callbacks.generate_string = &FontGenerate;
	FontState font_state;
	RmlFontEngineInterface* font_engine = rml_font_engine_interface_create(&font_callbacks, &font_state);
	RmlFontEngineInterface* previous = rml_get_font_engine_interface();
	rml_set_font_engine_interface(font_engine);

	auto* engine = reinterpret_cast<Rml::FontEngineInterface*>(font_engine);
	const Rml::FontMetrics& parent_metrics = engine->GetFontMetrics(1);
	engine->GetFontMetrics(2);
	CHECK(parent_metrics.ascent == doctest::Approx(30.f));
	CHECK(parent_metrics.descent == doctest::Approx(10.f));
	CHECK(engine->GetFontMetrics(2).ascent == doctest::Approx(8.f));

	const Rml::FontMetrics& missing = engine->GetFontMetrics(99);
	CHECK(missing.size == 0);
	CHECK(missing.ascent == doctest::Approx(0.f));
	CHECK(engine->GetFontMetrics(1).ascent == doctest::Approx(30.f));

	Session session;
	session.Open("review-metrics");

	const char* document_rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: ParentFace; font-size: 40px; width: 400px; height: 120px; }
				#child { font-family: ChildFace; font-size: 20px; vertical-align: middle; }
			</style>
		</head>
		<body>P<span id="child">c</span></body>
		</rml>
	)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[metrics]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	REQUIRE(rml_context_update(session.ctx));

	RmlElement* body = rml_document_as_element(document);
	RmlElement* child = rml_element_get_element_by_id(body, rml_sv("child"));
	REQUIRE(child);
	CHECK(rml_element_get_font_face_handle(body) == 1);
	CHECK(rml_element_get_font_face_handle(child) == 2);
	CHECK(rml_element_get_offset_height(child) == doctest::Approx(24.f));

	const Rml::FontMetrics& child_metrics = engine->GetFontMetrics(2);
	engine->GetFontMetrics(1);
	CHECK(child_metrics.ascent == doctest::Approx(8.f));
	CHECK(child_metrics.descent == doctest::Approx(2.f));

	rml_release_font_resources();
	CHECK(engine->GetFontMetrics(2).ascent == doctest::Approx(8.f));
	CHECK(engine->GetFontMetrics(1).x_height == doctest::Approx(20.f));

	session.Close("review-metrics");
	rml_set_font_engine_interface(previous);
	rml_font_engine_interface_destroy(font_engine);
}

TEST_CASE("capi.review.filter_compile_reaches_the_render_interface")
{
	Session session;
	session.Open("review-filter");

	RmlFilterInstancer* instancer = rml_filter_instancer_create(&InstanceCapiFilter, nullptr);
	rml_register_filter_instancer(rml_sv("capi-tint"), instancer);

	const char* document_rml = R"(
		<rml>
		<head><style>body { font-family: LatoLatin; width: 120px; height: 40px; filter: capi-tint(); }</style></head>
		<body>Tint</body>
		</rml>
	)";
	RmlElementDocument* document = rml_context_load_document_from_memory(session.ctx, rml_sv(document_rml), rml_sv("[filter]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	REQUIRE(rml_context_update(session.ctx));
	REQUIRE(rml_context_render(session.ctx));
	CHECK(session.render_state.filter_name == "capi-tint");
	CHECK(session.render_state.filter_sigma == 3);

	session.Close("review-filter");
	rml_filter_instancer_destroy(instancer);
}
