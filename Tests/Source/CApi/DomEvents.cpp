#include <RmlUi/CApi.h>
#include <doctest.h>
#include <string>

namespace {

struct RenderCounters {
	int compile_geometry = 0;
};

struct SystemState {
	double elapsed = 0.0;
};

double SystemGetElapsedTime(void* user_data)
{
	return static_cast<SystemState*>(user_data)->elapsed;
}

bool SystemLogMessage(void* /*user_data*/, RmlLogType /*type*/, RmlStringView /*message*/)
{
	return true;
}

RmlCompiledGeometryHandle RenderCompileGeometry(void* user_data, const RmlVertex*, size_t, const int*, size_t)
{
	auto* counters = static_cast<RenderCounters*>(user_data);
	counters->compile_geometry += 1;
	return static_cast<RmlCompiledGeometryHandle>(counters->compile_geometry);
}

void RenderRenderGeometry(void*, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle) {}
void RenderReleaseGeometry(void*, RmlCompiledGeometryHandle) {}
RmlTextureHandle RenderLoadTexture(void*, RmlVector2i* out_dimensions, RmlStringView)
{
	if (out_dimensions)
	{
		out_dimensions->x = 64;
		out_dimensions->y = 64;
	}
	return 1;
}
RmlTextureHandle RenderGenerateTexture(void*, const unsigned char*, size_t, RmlVector2i) { return 2; }
void RenderReleaseTexture(void*, RmlTextureHandle) {}
void RenderEnableScissor(void*, bool) {}
void RenderSetScissor(void*, RmlRectanglei) {}

struct Harness {
	SystemState system_state;
	RenderCounters render_counters;
	RmlSystemInterface* system_interface = nullptr;
	RmlRenderInterface* render_interface = nullptr;
	RmlContext* ctx = nullptr;

	void Setup()
	{
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

		system_interface = rml_system_interface_create(&system_callbacks, &system_state);
		render_interface = rml_render_interface_create(&render_callbacks, &render_counters);
		REQUIRE(system_interface);
		REQUIRE(render_interface);

		rml_set_system_interface(system_interface);
		rml_set_render_interface(render_interface);
		REQUIRE(rml_initialise());

		std::string font = std::string(RMLUI_CAPI_TESTS_SAMPLES_ASSETS) + "/LatoLatin-Regular.ttf";
		REQUIRE(rml_load_font_face(rml_sv(font.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));

		ctx = rml_create_context(rml_sv("dom"), RmlVector2i{800, 600}, nullptr, nullptr);
		REQUIRE(ctx);
	}

	void Teardown()
	{
		if (ctx)
			rml_remove_context(rml_sv("dom"));
		rml_shutdown();
		if (render_interface)
			rml_render_interface_destroy(render_interface);
		if (system_interface)
			rml_system_interface_destroy(system_interface);
	}
};

struct ListenerState {
	int count = 0;
	std::string last_type;
};

void ListenerProcess(void* user_data, RmlEvent* event)
{
	auto* state = static_cast<ListenerState*>(user_data);
	state->count += 1;
	RmlStringView type = rml_event_get_type(event);
	if (type.data)
		state->last_type.assign(type.data, type.size);
}

void ListenerOnDetach(void* user_data, RmlElement* /*element*/)
{
	// Listener is destroyed by the test after detach; no-op here.
	(void)user_data;
}

} // namespace

TEST_CASE("capi.dom.query_attribute_and_child_ownership")
{
	Harness h;
	h.Setup();

	const char* rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: LatoLatin; font-size: 16dp; }
				#host { width: 100dp; height: 100dp; }
			</style>
		</head>
		<body>
			<div id="host">Hello</div>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(h.ctx, rml_sv(rml), rml_sv("[dom]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);

	RmlElement* body = rml_document_as_element(document);
	RmlElement* host = rml_element_get_element_by_id(body, rml_sv("host"));
	REQUIRE(host);

	rml_element_set_attribute_string(host, rml_sv("data-x"), rml_sv("42"));
	RmlOwnedString attr = rml_element_get_attribute_string(host, rml_sv("data-x"));
	REQUIRE(attr.data);
	CHECK(std::string(attr.data, attr.size) == "42");
	rml_owned_string_free(attr);

	RmlElement* child = rml_document_create_element(document, rml_sv("span"));
	REQUIRE(child);
	rml_element_set_attribute_string(child, rml_sv("id"), rml_sv("kid"));
	rml_element_append_child(host, child);

	RmlElement* found = rml_element_get_element_by_id(body, rml_sv("kid"));
	REQUIRE(found);
	CHECK(found == child);

	RmlElement* removed = rml_element_remove_child(host, child);
	REQUIRE(removed == child);
	CHECK(rml_element_get_element_by_id(body, rml_sv("kid")) == nullptr);
	rml_element_release(removed);

	REQUIRE(rml_context_update(h.ctx));
	h.Teardown();
}

TEST_CASE("capi.events.listener_on_click")
{
	Harness h;
	h.Setup();

	const char* rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: LatoLatin; font-size: 16dp; }
				#btn { width: 80dp; height: 40dp; background: #4af; }
			</style>
		</head>
		<body>
			<div id="btn">Go</div>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(h.ctx, rml_sv(rml), rml_sv("[events]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	REQUIRE(rml_context_update(h.ctx));

	RmlElement* btn = rml_element_get_element_by_id(rml_document_as_element(document), rml_sv("btn"));
	REQUIRE(btn);

	ListenerState listener_state;
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = &ListenerProcess;
	callbacks.on_detach = &ListenerOnDetach;
	RmlEventListener* listener = rml_event_listener_create(&callbacks, &listener_state);
	REQUIRE(listener);
	rml_element_add_event_listener(btn, rml_sv("click"), listener, false);

	rml_element_click(btn);
	CHECK(listener_state.count >= 1);
	CHECK(listener_state.last_type == "click");

	rml_element_remove_event_listener(btn, rml_sv("click"), listener, false);
	rml_event_listener_destroy(listener);

	h.Teardown();
}

TEST_CASE("capi.variant.roundtrip")
{
	RmlVariant* variant = rml_variant_create();
	REQUIRE(variant);

	rml_variant_set_int(variant, 123);
	int value = 0;
	REQUIRE(rml_variant_get_int(variant, &value));
	CHECK(value == 123);

	rml_variant_set_string(variant, rml_sv("hello"));
	RmlStringView view = rml_variant_get_string(variant);
	REQUIRE(view.data);
	CHECK(std::string(view.data, view.size) == "hello");

	RmlVector2f vec = {1.5f, 2.5f};
	rml_variant_set_vector2f(variant, vec);
	RmlVector2f out = {};
	REQUIRE(rml_variant_get_vector2f(variant, &out));
	CHECK(out.x == doctest::Approx(1.5f));
	CHECK(out.y == doctest::Approx(2.5f));

	rml_variant_destroy(variant);
}

TEST_CASE("capi.forms.input_value_and_text_node")
{
	Harness h;
	h.Setup();

	const char* rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: LatoLatin; font-size: 16dp; }
			</style>
		</head>
		<body>
			<input id="name" type="text" value="initial"/>
			<div id="label"></div>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(h.ctx, rml_sv(rml), rml_sv("[forms]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	REQUIRE(rml_context_update(h.ctx));

	RmlElement* body = rml_document_as_element(document);
	RmlElement* input_el = rml_element_get_element_by_id(body, rml_sv("name"));
	REQUIRE(input_el);
	RmlElementFormControl* control = rml_element_as_form_control(input_el);
	REQUIRE(control);

	RmlOwnedString value = rml_form_control_get_value(control);
	REQUIRE(value.data);
	CHECK(std::string(value.data, value.size) == "initial");
	rml_owned_string_free(value);

	rml_form_control_set_value(control, rml_sv("updated"));
	value = rml_form_control_get_value(control);
	REQUIRE(value.data);
	CHECK(std::string(value.data, value.size) == "updated");
	rml_owned_string_free(value);

	RmlElement* label = rml_element_get_element_by_id(body, rml_sv("label"));
	REQUIRE(label);

	RmlElement* text_el = rml_document_create_text_node(document, rml_sv("Label text"));
	REQUIRE(text_el);
	RmlElementText* text = rml_element_as_text(text_el);
	REQUIRE(text);
	RmlStringView text_view = rml_text_get_text(text);
	REQUIRE(text_view.data);
	CHECK(std::string(text_view.data, text_view.size) == "Label text");
	rml_text_set_text(text, rml_sv("Changed"));
	text_view = rml_text_get_text(text);
	CHECK(std::string(text_view.data, text_view.size) == "Changed");

	rml_element_append_child(label, text_el);
	REQUIRE(rml_context_update(h.ctx));

	h.Teardown();
}
