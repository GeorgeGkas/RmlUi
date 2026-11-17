#include <RmlUi/CApi.h>
#include <doctest.h>
#include <string>

namespace {

struct State {
	int initialised = 0;
	int documents_loaded = 0;
	int updates = 0;
	int decorator_instances = 0;
	int decorator_renders = 0;
	int listener_values = 0;
	int font_initialised = 0;
	std::string last_listener_value;
	RmlElementCallbacks element_callbacks = {};
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

void OnInitialise(void* user_data)
{
	static_cast<State*>(user_data)->initialised += 1;
}
void OnDocumentLoad(void* user_data, RmlElementDocument*)
{
	static_cast<State*>(user_data)->documents_loaded += 1;
}
void OnUpdate(void* user_data, RmlElement*)
{
	static_cast<State*>(user_data)->updates += 1;
}
RmlElement* InstanceWidget(void* user_data, RmlElement*, RmlStringView, const RmlDictionary*)
{
	return rml_element_create_custom(rml_sv("widget"), &static_cast<State*>(user_data)->element_callbacks, user_data);
}
void ReleaseWidget(void*, RmlElement*) {}

RmlDecoratorDataHandle GenerateDecoratorData(void*, RmlElement*, RmlBoxArea)
{
	return 1;
}
void ReleaseDecoratorData(void*, RmlDecoratorDataHandle) {}
void RenderDecorator(void* user_data, RmlElement*, RmlDecoratorDataHandle)
{
	static_cast<State*>(user_data)->decorator_renders += 1;
}
RmlDecorator* InstanceDecorator(void* user_data, RmlStringView, const RmlPropertyDictionary*)
{
	static_cast<State*>(user_data)->decorator_instances += 1;
	RmlDecoratorCallbacks callbacks = {};
	callbacks.generate_element_data = &GenerateDecoratorData;
	callbacks.release_element_data = &ReleaseDecoratorData;
	callbacks.render_element = &RenderDecorator;
	return rml_decorator_create(&callbacks, user_data);
}

void ListenerProcess(void* user_data, RmlEvent*)
{
	static_cast<State*>(user_data)->listener_values += 1;
}
RmlEventListener* InstanceListener(void* user_data, RmlStringView value, RmlElement*)
{
	State* state = static_cast<State*>(user_data);
	if (value.data)
		state->last_listener_value.assign(value.data, value.size);
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = &ListenerProcess;
	return rml_event_listener_create(&callbacks, user_data);
}

void FontInitialise(void* user_data)
{
	static_cast<State*>(user_data)->font_initialised += 1;
}

} // namespace

TEST_CASE("capi.extensions.plugin_element_decorator_listener_and_property")
{
	State state;
	state.element_callbacks.on_update = &OnUpdate;

	RmlPluginCallbacks plugin_callbacks = {};
	plugin_callbacks.on_initialise = &OnInitialise;
	plugin_callbacks.on_document_load = &OnDocumentLoad;
	RmlPlugin* plugin = rml_plugin_create(&plugin_callbacks, &state);
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

	RmlSystemInterface* system_interface = rml_system_interface_create(&system_callbacks, nullptr);
	RmlRenderInterface* render_interface = rml_render_interface_create(&render_callbacks, nullptr);
	rml_set_system_interface(system_interface);
	rml_set_render_interface(render_interface);
	REQUIRE(rml_initialise());
	CHECK(state.initialised == 1);

	RmlElementInstancerCallbacks instancer_callbacks = {};
	instancer_callbacks.instance = &InstanceWidget;
	instancer_callbacks.release = &ReleaseWidget;
	RmlElementInstancer* element_instancer = rml_element_instancer_create(&instancer_callbacks, &state);
	rml_register_element_instancer(rml_sv("widget"), element_instancer);

	RmlDecoratorInstancer* decorator_instancer = rml_decorator_instancer_create(&InstanceDecorator, &state);
	rml_register_decorator_instancer(rml_sv("marker"), decorator_instancer);
	RmlPropertyDefinition* decorator_value = rml_decorator_instancer_register_property(decorator_instancer, rml_sv("value"), rml_sv("none"));
	rml_property_definition_add_parser(decorator_value, rml_sv("keyword"), rml_sv("none"));
	rml_decorator_instancer_register_shorthand(decorator_instancer, rml_sv("decorator"), rml_sv("value"), RML_SHORTHAND_TYPE_FALL_THROUGH);

	RmlEventListenerInstancer* listener_instancer = rml_event_listener_instancer_create(&InstanceListener, &state);
	rml_register_event_listener_instancer(listener_instancer);

	std::string font = std::string(RMLUI_CAPI_TESTS_SAMPLES_ASSETS) + "/LatoLatin-Regular.ttf";
	REQUIRE(rml_load_font_face(rml_sv(font.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));

	RmlContext* ctx = rml_create_context(rml_sv("ext"), RmlVector2i{800, 600}, nullptr, nullptr);
	REQUIRE(ctx);

	const char* document_rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: LatoLatin; font-size: 16dp; width: 200px; height: 100px; decorator: marker(none); }
				widget { display: block; width: 40dp; height: 20dp; }
			</style>
		</head>
		<body>
			<widget id="w"></widget>
			<button id="save" onclick="save_game">Save</button>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(ctx, rml_sv(document_rml), rml_sv("[extensions]"));
	REQUIRE(document);
	CHECK(state.documents_loaded >= 1);
	CHECK(state.decorator_instances >= 1);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_NONE, RML_SCROLL_FLAG_NONE);
	REQUIRE(rml_context_update(ctx));
	CHECK(state.updates >= 1);
	REQUIRE(rml_context_render(ctx));
	CHECK(state.decorator_renders >= 1);

	RmlElement* button = rml_element_get_element_by_id(rml_document_as_element(document), rml_sv("save"));
	REQUIRE(button);
	rml_element_click(button);
	CHECK(state.last_listener_value == "save_game");
	CHECK(state.listener_values >= 1);

	RmlPropertyDefinition* definition = rml_style_sheet_register_property(rml_sv("capi-phase4-size"), rml_sv("1"), false, false);
	REQUIRE(definition);
	rml_property_definition_add_parser(definition, rml_sv("number"), rml_sv(""));
	RmlPropertyDictionary* dictionary = rml_property_dictionary_create();
	REQUIRE(rml_style_sheet_parse_property_declaration(dictionary, rml_sv("capi-phase4-size"), rml_sv("12")));
	RmlOwnedString parsed = rml_property_dictionary_get_string(dictionary, rml_sv("capi-phase4-size"));
	REQUIRE(parsed.data);
	CHECK(std::string(parsed.data, parsed.size).find('1') != std::string::npos);
	rml_owned_string_free(parsed);
	rml_property_dictionary_destroy(dictionary);

	RmlStyleSheetContainer* sheet = rml_style_sheet_container_create_from_string(rml_sv("body { color: #ffffff; }"));
	REQUIRE(sheet);
	rml_document_set_style_sheet_container(document, sheet);
	rml_clear_style_sheet_cache();
	rml_style_sheet_container_destroy(sheet);

	rml_remove_context(rml_sv("ext"));
	rml_shutdown();
	rml_plugin_destroy(plugin);
	rml_element_instancer_destroy(element_instancer);
	rml_decorator_instancer_destroy(decorator_instancer);
	rml_event_listener_instancer_destroy(listener_instancer);
	rml_render_interface_destroy(render_interface);
	rml_system_interface_destroy(system_interface);
}

TEST_CASE("capi.extensions.font_engine_initialize")
{
	State state;
	RmlFontEngineInterfaceCallbacks font_callbacks = {};
	font_callbacks.initialize = &FontInitialise;
	RmlFontEngineInterface* font_engine = rml_font_engine_interface_create(&font_callbacks, &state);
	RmlFontEngineInterface* previous_font_engine = rml_get_font_engine_interface();

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
	RmlSystemInterface* system_interface = rml_system_interface_create(&system_callbacks, nullptr);
	RmlRenderInterface* render_interface = rml_render_interface_create(&render_callbacks, nullptr);

	rml_set_font_engine_interface(font_engine);
	rml_set_system_interface(system_interface);
	rml_set_render_interface(render_interface);
	REQUIRE(rml_initialise());
	CHECK(state.font_initialised == 1);
	rml_shutdown();
	rml_set_font_engine_interface(previous_font_engine);

	rml_font_engine_interface_destroy(font_engine);
	rml_render_interface_destroy(render_interface);
	rml_system_interface_destroy(system_interface);
}
