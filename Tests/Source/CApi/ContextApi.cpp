#include "Harness.h"
#include <cmath>
#include <doctest.h>

using namespace CApiTest;

namespace {

const char* pointer_document = R"(
<rml>
<head>
<style>
	body { position: absolute; left: 0; top: 0; width: 400px; height: 300px; margin: 0; padding: 0; }
	#a { position: absolute; left: 10px; top: 10px; width: 100px; height: 100px; cursor: pointer; }
	#b { position: absolute; left: 200px; top: 10px; width: 100px; height: 100px; cursor: move; }
	#scroller { position: absolute; left: 10px; top: 150px; width: 100px; height: 50px; overflow: auto; }
	#scroller div { display: block; height: 500px; }
</style>
</head>
<body id="pointer_doc">
	<div id="a"/>
	<div id="b"/>
	<div id="scroller"><div/></div>
</body>
</rml>
)";

const char* input_document = R"(
<rml>
<head>
<style>
	body { width: 400px; height: 300px; }
	input { display: block; width: 200px; height: 30px; }
</style>
</head>
<body id="input_doc">
	<input id="field" type="text" value=""/>
</body>
</rml>
)";

int Count(const std::vector<std::string>& values, const std::string& value)
{
	int count = 0;
	for (const std::string& entry : values)
		count += (entry == value);
	return count;
}

} // namespace

TEST_CASE("capi.context.properties_registry_and_themes")
{
	Session session("ctx_props");
	RmlContext* ctx = session.ctx;

	CHECK(ToString(rml_context_get_name(ctx)) == "ctx_props");
	CHECK(rml_get_num_contexts() == 1);
	CHECK(rml_get_context_by_name(rml_sv("ctx_props")) == ctx);
	CHECK(rml_get_context_by_index(0) == ctx);
	CHECK(rml_get_context_by_name(rml_sv("missing")) == nullptr);
	CHECK(rml_get_context_by_index(5) == nullptr);

	// Duplicate names are rejected by RmlUi.
	CHECK(rml_create_context(rml_sv("ctx_props"), RmlVector2i{10, 10}, nullptr, nullptr) == nullptr);
	CHECK(rml_get_num_contexts() == 1);
	CHECK_FALSE(rml_remove_context(rml_sv("missing")));

	rml_context_set_dimensions(ctx, RmlVector2i{640, 480});
	const RmlVector2i dimensions = rml_context_get_dimensions(ctx);
	CHECK(dimensions.x == 640);
	CHECK(dimensions.y == 480);

	rml_context_set_dp_ratio(ctx, 2.f);
	CHECK(rml_context_get_dp_ratio(ctx) == doctest::Approx(2.f));
	rml_context_set_dp_ratio(ctx, 1.f);

	CHECK(ToString(rml_context_get_documents_base_tag(ctx)) == "body");
	rml_context_set_documents_base_tag(ctx, rml_sv("panel"));
	CHECK(ToString(rml_context_get_documents_base_tag(ctx)) == "panel");
	rml_context_set_documents_base_tag(ctx, rml_sv("body"));

	CHECK_FALSE(rml_context_is_theme_active(ctx, rml_sv("dark")));
	rml_context_activate_theme(ctx, rml_sv("dark"), true);
	CHECK(rml_context_is_theme_active(ctx, rml_sv("dark")));
	rml_context_activate_theme(ctx, rml_sv("dark"), false);
	CHECK_FALSE(rml_context_is_theme_active(ctx, rml_sv("dark")));

	session.Update();
	CHECK(std::isinf(rml_context_get_next_update_delay(ctx)));
	rml_context_request_next_update(ctx, 0.25);
	CHECK(rml_context_get_next_update_delay(ctx) == doctest::Approx(0.25));

	CHECK(rml_context_get_text_input_handler(ctx) != nullptr);
	CHECK(rml_context_get_text_input_handler(ctx) == rml_get_text_input_handler());

	RmlElement* root = rml_context_get_root_element(ctx);
	REQUIRE(root);
	CHECK(rml_element_get_parent_node(root) == nullptr);
	CHECK(rml_element_get_owner_document(root) == nullptr);
}

TEST_CASE("capi.context.themes_select_media_blocks")
{
	Session session("ctx_theme");
	session.Load(R"(
<rml>
<head>
<style>
	#probe { display: block; width: 10px; }
	@media (theme: wide) { #probe { width: 50px; } }
</style>
</head>
<body><div id="probe"/></body>
</rml>
)");
	RmlElement* probe = session.ById("probe");
	REQUIRE(probe);
	CHECK(rml_element_get_offset_width(probe) == doctest::Approx(10.f));

	rml_context_activate_theme(session.ctx, rml_sv("wide"), true);
	session.Update();
	CHECK(rml_element_get_offset_width(probe) == doctest::Approx(50.f));
}

TEST_CASE("capi.context.document_lifecycle_and_ordering")
{
	Session session("ctx_docs");
	RmlContext* ctx = session.ctx;

	RmlElementDocument* empty = rml_context_create_document(ctx, rml_sv("body"));
	REQUIRE(empty);
	CHECK(rml_context_get_num_documents(ctx) == 1);

	RmlElementDocument* first = session.Load("<rml><head><title>First</title></head><body id='first'/></rml>", true, "first.rml");
	RmlElementDocument* second = session.Load("<rml><body id='second'/></rml>", true, "second.rml");
	CHECK(rml_context_get_num_documents(ctx) == 3);

	CHECK(rml_context_get_document_by_id(ctx, rml_sv("first")) == first);
	CHECK(rml_context_get_document_by_id(ctx, rml_sv("second")) == second);
	CHECK(rml_context_get_document_by_id(ctx, rml_sv("nope")) == nullptr);
	CHECK(rml_context_get_document_by_index(ctx, 99) == nullptr);

	CHECK(rml_document_get_context(first) == ctx);
	CHECK(ToString(rml_document_get_source_url(first)) == "first.rml");
	CHECK(ToString(rml_document_get_title(first)) == "First");
	rml_document_set_title(first, rml_sv("Renamed"));
	CHECK(ToString(rml_document_get_title(first)) == "Renamed");

	RmlElement* first_element = rml_document_as_element(first);
	CHECK(rml_element_as_document(first_element) == first);
	CHECK(rml_element_get_owner_document(first_element) == first);
	CHECK(rml_element_as_document(rml_context_get_root_element(ctx)) == nullptr);

	rml_context_pull_document_to_front(ctx, first);
	CHECK(rml_context_get_document_by_index(ctx, rml_context_get_num_documents(ctx) - 1) == first);
	rml_context_push_document_to_back(ctx, first);
	CHECK(rml_context_get_document_by_index(ctx, 0) == first);
	rml_document_pull_to_front(second);
	CHECK(rml_context_get_document_by_index(ctx, rml_context_get_num_documents(ctx) - 1) == second);
	rml_document_push_to_back(second);
	CHECK(rml_context_get_document_by_index(ctx, 0) == second);

	CHECK_FALSE(rml_document_is_modal(first));
	rml_document_show(first, RML_MODAL_FLAG_MODAL, RML_FOCUS_FLAG_DOCUMENT, RML_SCROLL_FLAG_NONE);
	CHECK(rml_document_is_modal(first));
	CHECK(rml_context_get_focus_element(ctx) == first_element);
	rml_document_hide(first);
	session.Update();
	CHECK_FALSE(rml_element_is_visible(first_element, false));
	rml_document_show(first, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_DOCUMENT, RML_SCROLL_FLAG_NONE);
	CHECK_FALSE(rml_document_is_modal(first));

	rml_context_unfocus_document(ctx, first);
	CHECK(rml_context_get_focus_element(ctx) != first_element);

	rml_context_unload_document(ctx, second);
	CHECK(rml_context_get_num_documents(ctx) == 2);
	rml_document_close(empty);
	CHECK(rml_context_get_num_documents(ctx) == 1);
	session.Update();

	rml_context_unload_all_documents(ctx);
	CHECK(rml_context_get_num_documents(ctx) == 0);
	session.Update();
}

TEST_CASE("capi.context.element_at_point_hover_and_cursor")
{
	Session session("ctx_pointer");
	RmlContext* ctx = session.ctx;
	RmlElementDocument* document = session.Load(pointer_document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* a = session.ById("a");
	RmlElement* b = session.ById("b");
	REQUIRE(a);
	REQUIRE(b);

	CHECK(rml_context_get_element_at_point(ctx, RmlVector2f{50, 50}, nullptr) == a);
	CHECK(rml_context_get_element_at_point(ctx, RmlVector2f{50, 50}, a) == body);
	CHECK(rml_context_get_element_at_point(ctx, RmlVector2f{250, 50}, nullptr) == b);
	CHECK(rml_context_get_element_at_point(ctx, RmlVector2f{700, 500}, nullptr) == rml_context_get_root_element(ctx));

	CHECK(rml_context_process_mouse_move(ctx, 700, 500, 0));
	CHECK_FALSE(rml_context_is_mouse_interacting(ctx));

	CHECK_FALSE(rml_context_process_mouse_move(ctx, 50, 50, 0));
	CHECK(rml_context_is_mouse_interacting(ctx));
	CHECK(rml_context_get_hover_element(ctx) == a);
	CHECK(rml_element_is_pseudo_class_set(a, rml_sv("hover")));
	CHECK(Count(session.system_log.cursors, "pointer") == 1);

	CHECK_FALSE(rml_context_process_mouse_move(ctx, 250, 50, 0));
	CHECK(rml_context_get_hover_element(ctx) == b);
	CHECK_FALSE(rml_element_is_pseudo_class_set(a, rml_sv("hover")));
	CHECK(Count(session.system_log.cursors, "move") == 1);

	rml_context_enable_mouse_cursor(ctx, false);
	const size_t cursor_calls = session.system_log.cursors.size();
	rml_context_process_mouse_move(ctx, 50, 50, 0);
	CHECK(session.system_log.cursors.size() == cursor_calls);
	rml_context_enable_mouse_cursor(ctx, true);

	CHECK(rml_context_process_mouse_leave(ctx));
	CHECK(rml_context_get_hover_element(ctx) == nullptr);
	CHECK_FALSE(rml_context_is_mouse_interacting(ctx));
}

TEST_CASE("capi.context.mouse_buttons_and_wheel_dispatch_events")
{
	Session session("ctx_mouse");
	RmlContext* ctx = session.ctx;
	session.Load(pointer_document);
	RmlElement* a = session.ById("a");
	RmlElement* scroller = session.ById("scroller");
	REQUIRE(a);
	REQUIRE(scroller);

	ListenerLog log;
	RmlEventListener* listener = MakeListener(&log);
	REQUIRE(listener);
	rml_element_add_event_listener(a, rml_sv("mousedown"), listener, false);
	rml_element_add_event_listener(a, rml_sv("mouseup"), listener, false);
	rml_element_add_event_listener(a, rml_sv("click"), listener, false);
	rml_element_add_event_listener(a, rml_sv("dblclick"), listener, false);

	rml_context_process_mouse_move(ctx, 50, 50, 0);
	rml_context_process_mouse_button_down(ctx, 0, RML_KEY_MODIFIER_SHIFT);
	CHECK(rml_element_is_pseudo_class_set(a, rml_sv("active")));
	rml_context_process_mouse_button_up(ctx, 0, 0);
	CHECK_FALSE(rml_element_is_pseudo_class_set(a, rml_sv("active")));
	CHECK(Count(log.types, "mousedown") == 1);
	CHECK(Count(log.types, "mouseup") == 1);
	CHECK(Count(log.types, "click") == 1);

	rml_context_process_mouse_button_down(ctx, 0, 0);
	rml_context_process_mouse_button_up(ctx, 0, 0);
	CHECK(Count(log.types, "dblclick") == 1);

	rml_element_remove_event_listener(a, rml_sv("mousedown"), listener, false);
	rml_element_remove_event_listener(a, rml_sv("mouseup"), listener, false);
	rml_element_remove_event_listener(a, rml_sv("click"), listener, false);
	rml_element_remove_event_listener(a, rml_sv("dblclick"), listener, false);

	ListenerLog scroll_log;
	RmlEventListener* scroll_listener = MakeListener(&scroll_log);
	rml_element_add_event_listener(scroller, rml_sv("mousescroll"), scroll_listener, false);
	rml_context_set_default_scroll_behavior(ctx, RML_SCROLL_BEHAVIOR_INSTANT, 1.f);
	rml_context_process_mouse_move(ctx, 50, 170, 0);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(0.f));
	rml_context_process_mouse_wheel(ctx, RmlVector2f{0.f, 2.f}, 0);
	session.Update();
	CHECK(scroll_log.calls == 1);
	CHECK(rml_element_get_scroll_top(scroller) > 0.f);
	rml_element_remove_event_listener(scroller, rml_sv("mousescroll"), scroll_listener, false);

	rml_event_listener_destroy(listener);
	rml_event_listener_destroy(scroll_listener);
}

TEST_CASE("capi.context.touch_input_emulates_primary_pointer")
{
	Session session("ctx_touch");
	RmlContext* ctx = session.ctx;
	session.Load(pointer_document);
	RmlElement* a = session.ById("a");
	RmlElement* scroller = session.ById("scroller");
	REQUIRE(a);
	REQUIRE(scroller);

	ListenerLog log;
	RmlEventListener* listener = MakeListener(&log);
	rml_element_add_event_listener(a, rml_sv("click"), listener, false);

	RmlTouch touch = {7, RmlVector2f{50, 50}};
	rml_context_process_touch_start(ctx, &touch, 1, 0);
	CHECK(rml_context_get_hover_element(ctx) == a);
	rml_context_process_touch_end(ctx, &touch, 1, 0);
	CHECK(log.calls == 1);
	rml_element_remove_event_listener(a, rml_sv("click"), listener, false);

	// Dragging a touch inside a scroll container scrolls it instantly.
	RmlTouch drag = {8, RmlVector2f{50, 190}};
	rml_context_process_touch_start(ctx, &drag, 1, 0);
	drag.position.y = 160;
	rml_context_process_touch_move(ctx, &drag, 1, 0);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(30.f));
	const bool cancel_result = rml_context_process_touch_cancel(ctx, &drag, 1);
	CHECK(cancel_result == !rml_context_is_mouse_interacting(ctx));

	// Unknown touches and empty lists are ignored.
	RmlTouch unknown = {99, RmlVector2f{0, 0}};
	CHECK(rml_context_process_touch_move(ctx, &unknown, 1, 0));
	CHECK(rml_context_process_touch_move(ctx, nullptr, 0, 0));

	rml_event_listener_destroy(listener);
}

TEST_CASE("capi.context.keyboard_and_text_input_reach_focused_input")
{
	Session session("ctx_keys");
	RmlContext* ctx = session.ctx;
	session.Load(input_document);
	RmlElement* field = session.ById("field");
	REQUIRE(field);
	RmlElementFormControl* control = rml_element_as_form_control(field);
	REQUIRE(control);

	// Without focus, key events go to the root element and are not consumed.
	CHECK(rml_context_process_key_down(ctx, RML_KEY_A, 0));
	CHECK(rml_context_process_key_up(ctx, RML_KEY_A, 0));

	ListenerLog log;
	RmlEventListener* listener = MakeListener(&log);
	rml_element_add_event_listener(field, rml_sv("keydown"), listener, false);

	CHECK(rml_element_focus(field, true));
	CHECK(rml_context_get_focus_element(ctx) == field);
	session.Update();
	CHECK(session.system_log.keyboard_activations > 0);

	rml_context_process_text_input_string(ctx, rml_sv("Hi"));
	rml_context_process_text_input_ascii(ctx, '!');
	CHECK_FALSE(rml_context_process_text_input_ascii(ctx, static_cast<char>(0xE9)));
	rml_context_process_text_input_char(ctx, 0x00E9);
	CHECK(Take(rml_form_control_get_value(control)) == "Hi!\xC3\xA9");

	rml_context_process_key_down(ctx, RML_KEY_BACK, 0);
	CHECK(Take(rml_form_control_get_value(control)) == "Hi!");
	REQUIRE(log.calls >= 1);

	rml_context_process_key_down(ctx, RML_KEY_A, RML_KEY_MODIFIER_CTRL);
	rml_context_process_key_down(ctx, RML_KEY_C, RML_KEY_MODIFIER_CTRL);
	CHECK(session.system_log.clipboard == "Hi!");

	rml_context_process_key_down(ctx, RML_KEY_END, 0);
	rml_context_process_key_down(ctx, RML_KEY_V, RML_KEY_MODIFIER_CTRL);
	CHECK(session.system_log.clipboard_reads == 1);
	CHECK(Take(rml_form_control_get_value(control)) == "Hi!Hi!");

	rml_element_remove_event_listener(field, rml_sv("keydown"), listener, false);
	rml_element_blur(field);
	session.Update();
	CHECK(rml_context_get_focus_element(ctx) != field);
	CHECK(session.system_log.keyboard_deactivations > 0);

	rml_event_listener_destroy(listener);
}

TEST_CASE("capi.context.key_event_parameters_are_exposed")
{
	Session session("ctx_keyparams");
	RmlContext* ctx = session.ctx;
	RmlElementDocument* document = session.Load(input_document);
	RmlElement* body = rml_document_as_element(document);

	struct Seen {
		int key_identifier = -1;
		int ctrl = -1;
		int shift = -1;
	} seen;
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = [](void* user_data, RmlEvent* event) {
		Seen& s = *static_cast<Seen*>(user_data);
		const RmlDictionary* parameters = rml_event_get_parameters(event);
		const RmlVariant* key = rml_dictionary_get(parameters, rml_sv("key_identifier"));
		const RmlVariant* ctrl = rml_dictionary_get(parameters, rml_sv("ctrl_key"));
		const RmlVariant* shift = rml_dictionary_get(parameters, rml_sv("shift_key"));
		if (key)
			rml_variant_get_int(key, &s.key_identifier);
		if (ctrl)
			rml_variant_get_int(ctrl, &s.ctrl);
		if (shift)
			rml_variant_get_int(shift, &s.shift);
	};
	RmlEventListener* listener = rml_event_listener_create(&callbacks, &seen);
	rml_element_focus(body, false);
	rml_element_add_event_listener(body, rml_sv("keyup"), listener, false);

	rml_context_process_key_up(ctx, RML_KEY_F5, RML_KEY_MODIFIER_CTRL);
	CHECK(seen.key_identifier == RML_KEY_F5);
	CHECK(seen.ctrl == 1);
	CHECK(seen.shift == 0);

	rml_element_remove_event_listener(body, rml_sv("keyup"), listener, false);
	rml_event_listener_destroy(listener);
}

TEST_CASE("capi.context.context_listeners_receive_bubbling_events")
{
	Session session("ctx_listen");
	RmlContext* ctx = session.ctx;
	session.Load(pointer_document);
	RmlElement* a = session.ById("a");
	REQUIRE(a);

	ListenerLog bubble;
	ListenerLog capture;
	RmlEventListener* bubble_listener = MakeListener(&bubble);
	RmlEventListener* capture_listener = MakeListener(&capture);
	rml_context_add_event_listener(ctx, rml_sv("click"), bubble_listener, false);
	rml_context_add_event_listener(ctx, rml_sv("click"), capture_listener, true);

	rml_element_click(a);
	CHECK(bubble.calls == 1);
	CHECK(capture.calls == 1);
	REQUIRE(capture.phases.size() == 1);
	CHECK(capture.phases[0] == RML_EVENT_PHASE_CAPTURE);
	CHECK(bubble.phases[0] == RML_EVENT_PHASE_BUBBLE);
	CHECK(bubble.targets[0] == a);
	CHECK(bubble.current[0] == rml_context_get_root_element(ctx));

	rml_context_remove_event_listener(ctx, rml_sv("click"), bubble_listener, false);
	rml_context_remove_event_listener(ctx, rml_sv("click"), capture_listener, true);
	rml_element_click(a);
	CHECK(bubble.calls == 1);
	CHECK(capture.calls == 1);

	rml_event_listener_destroy(bubble_listener);
	rml_event_listener_destroy(capture_listener);
}

TEST_CASE("capi.context.data_model_lookup_dirty_and_removal")
{
	Session session("ctx_models");
	RmlContext* ctx = session.ctx;

	int value = 1;
	RmlDataModelConstructor constructor = rml_context_create_data_model(ctx, rml_sv("stats"), false);
	REQUIRE(rml_data_model_constructor_is_valid(constructor));
	REQUIRE(rml_data_model_bind_int(constructor, rml_sv("value"), &value));

	// A second model with the same name is rejected.
	CHECK_FALSE(rml_data_model_constructor_is_valid(rml_context_create_data_model(ctx, rml_sv("stats"), false)));

	RmlDataModelConstructor fetched = rml_context_get_data_model(ctx, rml_sv("stats"));
	CHECK(rml_data_model_constructor_is_valid(fetched));
	CHECK_FALSE(rml_data_model_constructor_is_valid(rml_context_get_data_model(ctx, rml_sv("missing"))));

	session.Load("<rml><body data-model='stats'><p id='out'>{{ value }}</p></body></rml>");
	RmlElement* out = session.ById("out");
	REQUIRE(out);
	CHECK(Take(rml_element_get_inner_rml(out)) == "1");

	RmlDataModelHandle handle = rml_data_model_get_handle(fetched);
	CHECK_FALSE(rml_data_model_handle_is_variable_dirty(handle, rml_sv("value")));
	value = 7;
	rml_data_model_handle_dirty_all_variables(handle);
	CHECK(rml_data_model_handle_is_variable_dirty(handle, rml_sv("value")));
	session.Update();
	CHECK_FALSE(rml_data_model_handle_is_variable_dirty(handle, rml_sv("value")));
	CHECK(Take(rml_element_get_inner_rml(out)) == "7");

	rml_context_unload_all_documents(ctx);
	session.Update();
	CHECK(rml_context_remove_data_model(ctx, rml_sv("stats")));
	CHECK_FALSE(rml_context_remove_data_model(ctx, rml_sv("stats")));
}
