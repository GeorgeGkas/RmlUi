#include "Harness.h"
#include <doctest.h>

using namespace CApiTest;

namespace {

const char* tree_document = R"(
<rml>
<head>
<style>
	body { width: 400px; height: 300px; margin: 0; padding: 0; color: #ff0000; font-family: LatoLatin; font-size: 16px; }
	div, p { display: block; }
	#box { position: absolute; left: 20px; top: 30px; width: 100px; height: 50px; padding: 5px; border: 2px #000; z-index: 3; }
	#scroller { width: 100px; height: 60px; overflow: auto; }
	#scroller p { height: 40px; }
	img { display: inline-block; }
</style>
</head>
<body id="tree">
	<div id="box" class="alpha beta"><span id="label">Hello</span></div>
	<div id="list">
		<p id="p1" class="item">One</p>
		<p id="p2" class="item odd">Two</p>
		<p id="p3" class="item">Three</p>
	</div>
	<div id="scroller"><p id="s1">a</p><p id="s2">b</p><p id="s3">c</p><p id="s4">d</p></div>
	<img id="picture" src="picture.png"/>
	<input id="first_input" type="text"/>
	<input id="second_input" type="text"/>
</body>
</rml>
)";

struct AttachLog {
	int processed = 0;
	int attached = 0;
	int detached = 0;
	RmlEventListener* self = nullptr;
	bool destroy_on_detach = false;
};

void AttachProcess(void* user_data, RmlEvent*)
{
	static_cast<AttachLog*>(user_data)->processed += 1;
}
void AttachOnAttach(void* user_data, RmlElement*)
{
	static_cast<AttachLog*>(user_data)->attached += 1;
}
void AttachOnDetach(void* user_data, RmlElement*)
{
	AttachLog* log = static_cast<AttachLog*>(user_data);
	log->detached += 1;
	if (log->destroy_on_detach && log->self)
	{
		rml_event_listener_destroy(log->self);
		log->self = nullptr;
	}
}

bool ListContains(const RmlStringList* list, const char* value)
{
	for (size_t i = 0; i < rml_string_list_size(list); i++)
		if (ToString(rml_string_list_get(list, i)) == value)
			return true;
	return false;
}

} // namespace

TEST_CASE("capi.element.classes_attributes_and_identity")
{
	Session session("el_attr");
	session.Load(tree_document);
	RmlElement* box = session.ById("box");
	REQUIRE(box);

	CHECK(ToString(rml_element_get_tag_name(box)) == "div");
	CHECK(ToString(rml_element_get_id(box)) == "box");

	CHECK(rml_element_is_class_set(box, rml_sv("alpha")));
	CHECK(rml_element_is_class_set(box, rml_sv("beta")));
	rml_element_set_class(box, rml_sv("alpha"), false);
	rml_element_set_class(box, rml_sv("gamma"), true);
	CHECK_FALSE(rml_element_is_class_set(box, rml_sv("alpha")));
	CHECK(Take(rml_element_get_class_names(box)) == "beta gamma");
	rml_element_set_class_names(box, rml_sv("one two"));
	CHECK(Take(rml_element_get_class_names(box)) == "one two");
	CHECK(rml_element_is_class_set(box, rml_sv("two")));

	const int base_attributes = rml_element_get_num_attributes(box);
	rml_element_set_attribute_string(box, rml_sv("data-name"), rml_sv("widget"));
	CHECK(rml_element_has_attribute(box, rml_sv("data-name")));
	CHECK(Take(rml_element_get_attribute_string(box, rml_sv("data-name"))) == "widget");

	RmlVariant* number = rml_variant_create();
	rml_variant_set_int(number, 42);
	rml_element_set_attribute_variant(box, rml_sv("data-count"), number);
	rml_variant_destroy(number);
	CHECK(rml_element_get_num_attributes(box) == base_attributes + 2);

	const RmlVariant* count = rml_element_get_attribute(box, rml_sv("data-count"));
	REQUIRE(count);
	CHECK(rml_variant_get_type(count) == RML_VARIANT_TYPE_INT);
	int count_value = 0;
	CHECK(rml_variant_get_int(count, &count_value));
	CHECK(count_value == 42);
	CHECK(rml_element_get_attribute_const(box, rml_sv("data-count")) == count);
	CHECK(Take(rml_element_get_attribute_string(box, rml_sv("data-count"))) == "42");
	CHECK(rml_element_get_attribute(box, rml_sv("missing")) == nullptr);
	CHECK(rml_element_get_attribute_string(box, rml_sv("missing")).data == nullptr);

	rml_element_remove_attribute(box, rml_sv("data-name"));
	CHECK_FALSE(rml_element_has_attribute(box, rml_sv("data-name")));
	CHECK(rml_element_get_num_attributes(box) == base_attributes + 1);

	rml_element_set_id(box, rml_sv("renamed"));
	CHECK(ToString(rml_element_get_id(box)) == "renamed");
	CHECK(session.ById("renamed") == box);
	CHECK(session.ById("box") == nullptr);

	const std::string address = Take(rml_element_get_address(box, false, false));
	CHECK(address.find("div#renamed") == 0);
	const std::string full_address = Take(rml_element_get_address(box, false, true));
	CHECK(full_address.find("body#tree") != std::string::npos);
}

TEST_CASE("capi.element.properties_and_pseudo_classes")
{
	Session session("el_props");
	session.Load(tree_document);
	RmlElement* box = session.ById("box");
	RmlElement* label = session.ById("label");
	REQUIRE(box);
	REQUIRE(label);

	CHECK(rml_element_set_property(box, rml_sv("width"), rml_sv("120px")));
	CHECK_FALSE(rml_element_set_property(box, rml_sv("width"), rml_sv("not-a-length")));
	CHECK_FALSE(rml_element_set_property(box, rml_sv("not-a-property"), rml_sv("1px")));
	CHECK(rml_element_has_local_property(box, rml_sv("width")));
	CHECK(Take(rml_element_get_local_property_string(box, rml_sv("width"))) == "120px");
	session.Update();
	CHECK(rml_element_get_client_width(box) == doctest::Approx(130.f));

	// Inherited from the body rule.
	CHECK_FALSE(rml_element_has_local_property(label, rml_sv("color")));
	CHECK(rml_element_has_property(label, rml_sv("color")));
	const std::string color = Take(rml_element_get_property_string(label, rml_sv("color")));
	CHECK(color == "#ff0000");
	CHECK(rml_element_get_local_property_string(label, rml_sv("color")).data == nullptr);
	CHECK_FALSE(rml_element_has_property(box, rml_sv("not-a-property")));

	// Removing the inline declaration falls back to the stylesheet rule, which still counts as local.
	rml_element_remove_property(box, rml_sv("width"));
	CHECK(Take(rml_element_get_local_property_string(box, rml_sv("width"))) == "100px");
	session.Update();
	CHECK(rml_element_get_client_width(box) == doctest::Approx(110.f));

	rml_element_set_pseudo_class(box, rml_sv("checked"), true);
	CHECK(rml_element_is_pseudo_class_set(box, rml_sv("checked")));
	RmlStringList* active = rml_string_list_create();
	rml_element_get_active_pseudo_classes(box, active);
	CHECK(ListContains(active, "checked"));
	CHECK(rml_element_are_pseudo_classes_set(box, active));
	rml_string_list_push(active, rml_sv("disabled"));
	CHECK_FALSE(rml_element_are_pseudo_classes_set(box, active));
	rml_string_list_destroy(active);
	rml_element_set_pseudo_class(box, rml_sv("checked"), false);
	CHECK_FALSE(rml_element_is_pseudo_class_set(box, rml_sv("checked")));
}

TEST_CASE("capi.element.dom_traversal_and_queries")
{
	Session session("el_dom");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* list = session.ById("list");
	RmlElement* p1 = session.ById("p1");
	RmlElement* p2 = session.ById("p2");
	RmlElement* p3 = session.ById("p3");
	REQUIRE(list);
	REQUIRE(p1);
	REQUIRE(p2);
	REQUIRE(p3);

	CHECK(rml_element_get_parent_node(p2) == list);
	CHECK(rml_element_get_first_child(list) == p1);
	CHECK(rml_element_get_last_child(list) == p3);
	CHECK(rml_element_get_next_sibling(p1) == p2);
	CHECK(rml_element_get_previous_sibling(p3) == p2);
	CHECK(rml_element_get_previous_sibling(p1) == nullptr);
	CHECK(rml_element_get_child(list, 2) == p3);
	CHECK(rml_element_get_child(list, 9) == nullptr);
	CHECK(rml_element_get_num_children(list, false) == 3);
	CHECK(rml_element_has_child_nodes(list));
	CHECK(rml_element_get_owner_document(p1) == document);
	CHECK(rml_element_closest(p1, rml_sv("body")) == body);
	CHECK(rml_element_closest(p1, rml_sv("#list")) == list);
	CHECK(rml_element_closest(p1, rml_sv("span")) == nullptr);
	CHECK(rml_element_contains(list, p2));
	CHECK_FALSE(rml_element_contains(p2, list));

	CHECK(rml_element_matches(p2, rml_sv(".odd")));
	CHECK_FALSE(rml_element_matches(p1, rml_sv(".odd")));
	CHECK(rml_element_query_selector(body, rml_sv("#list .odd")) == p2);
	CHECK(rml_element_query_selector(body, rml_sv(".nothing")) == nullptr);

	RmlElementList* elements = rml_element_list_create();
	rml_element_query_selector_all(body, elements, rml_sv("p.item"));
	REQUIRE(rml_element_list_size(elements) == 3);
	CHECK(rml_element_list_get(elements, 0) == p1);
	CHECK(rml_element_list_get(elements, 2) == p3);

	rml_element_get_elements_by_class_name(body, elements, rml_sv("odd"));
	REQUIRE(rml_element_list_size(elements) == 1);
	CHECK(rml_element_list_get(elements, 0) == p2);

	rml_element_get_elements_by_tag_name(list, elements, rml_sv("p"));
	CHECK(rml_element_list_size(elements) == 3);
	rml_element_list_clear(elements);
	CHECK(rml_element_list_size(elements) == 0);

	CHECK(rml_element_utilities_get_element_by_id(body, rml_sv("p3")) == p3);
	rml_element_utilities_get_elements_by_tag_name(elements, body, rml_sv("p"), rml_sv(""));
	CHECK(rml_element_list_size(elements) >= 3);
	rml_element_list_clear(elements);
	rml_element_utilities_get_elements_by_tag_name(elements, body, rml_sv("p"), rml_sv("div"));
	CHECK(rml_element_list_size(elements) == 0);
	rml_element_list_clear(elements);
	rml_element_utilities_get_elements_by_class_name(elements, body, rml_sv("item"));
	CHECK(rml_element_list_size(elements) == 3);
	rml_element_list_destroy(elements);

	CHECK(Take(rml_element_get_inner_rml(p1)) == "One");
	rml_element_set_inner_rml(p1, rml_sv("<span id='inner'>Uno</span>"));
	CHECK(session.ById("inner") != nullptr);
	CHECK(rml_element_get_num_children(p1, false) == 1);
}

TEST_CASE("capi.element.mutation_transfers_ownership")
{
	Session session("el_mut");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* list = session.ById("list");
	RmlElement* p1 = session.ById("p1");
	RmlElement* p2 = session.ById("p2");
	REQUIRE(list);

	RmlElement* fresh = rml_document_create_element(document, rml_sv("p"));
	REQUIRE(fresh);
	CHECK(rml_element_get_parent_node(fresh) == nullptr);
	rml_element_set_id(fresh, rml_sv("fresh"));
	CHECK(rml_element_append_child(list, fresh) == fresh);
	CHECK(rml_element_get_last_child(list) == fresh);

	// Parented elements are refused and stay where they are.
	CHECK(rml_element_append_child(list, p1) == nullptr);
	CHECK(rml_element_get_parent_node(p1) == list);
	rml_element_release(p1);
	CHECK(session.ById("p1") == p1);
	CHECK(session.system_log.HasMessage("owned by RmlUi"));

	RmlElement* first = rml_document_create_element(document, rml_sv("p"));
	CHECK(rml_element_insert_before(list, first, p1) == first);
	CHECK(rml_element_get_first_child(list) == first);

	RmlElement* replacement = rml_document_create_element(document, rml_sv("p"));
	RmlElement* replaced = rml_element_replace_child(list, replacement, p2);
	CHECK(replaced == p2);
	CHECK(rml_element_get_parent_node(p2) == nullptr);
	CHECK(rml_element_get_next_sibling(p1) == replacement);

	// A removed child is handed back to the caller and must be released.
	RmlElement* removed = rml_element_remove_child(list, first);
	CHECK(removed == first);
	CHECK(rml_element_get_parent_node(first) == nullptr);
	CHECK(rml_element_remove_child(list, first) == nullptr);

	RmlElement* clone = rml_element_clone(p1);
	REQUIRE(clone);
	CHECK(clone != p1);
	CHECK(rml_element_get_parent_node(clone) == nullptr);
	CHECK(Take(rml_element_get_inner_rml(clone)) == "One");

	// A detached subtree can be re-attached.
	CHECK(rml_element_append_child(list, removed) == removed);

	rml_element_release(replaced);
	rml_element_release(clone);
	session.Update();
}

TEST_CASE("capi.element.text_nodes")
{
	Session session("el_text");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* label = session.ById("label");
	REQUIRE(label);

	RmlElement* text_element = rml_element_get_first_child(label);
	REQUIRE(text_element);
	RmlElementText* text = rml_element_as_text(text_element);
	REQUIRE(text);
	CHECK(rml_text_as_element(text) == text_element);
	CHECK(ToString(rml_text_get_text(text)) == "Hello");
	CHECK(rml_element_as_text(label) == nullptr);

	rml_text_set_text(text, rml_sv("World"));
	CHECK(ToString(rml_text_get_text(text)) == "World");
	rml_text_suppress_auto_layout(text);

	RmlElement* node = rml_document_create_text_node(document, rml_sv("Appended"));
	REQUIRE(node);
	REQUIRE(rml_element_as_text(node));
	CHECK(ToString(rml_text_get_text(rml_element_as_text(node))) == "Appended");
	CHECK(rml_element_append_child(label, node) == node);
	session.Update();
	CHECK(rml_element_utilities_get_string_width(label, rml_sv("World"), 0) > 0);
}

TEST_CASE("capi.element.layout_metrics_and_geometry")
{
	Session session("el_metrics");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* box = session.ById("box");
	RmlElement* label = session.ById("label");
	RmlElement* picture = session.ById("picture");
	REQUIRE(box);
	REQUIRE(picture);

	CHECK(rml_element_get_offset_left(box) == doctest::Approx(20.f));
	CHECK(rml_element_get_offset_top(box) == doctest::Approx(30.f));
	CHECK(rml_element_get_offset_width(box) == doctest::Approx(114.f));
	CHECK(rml_element_get_offset_height(box) == doctest::Approx(64.f));
	CHECK(rml_element_get_offset_parent(box) == body);
	CHECK(rml_element_get_client_left(box) == doctest::Approx(2.f));
	CHECK(rml_element_get_client_top(box) == doctest::Approx(2.f));
	CHECK(rml_element_get_client_width(box) == doctest::Approx(110.f));
	CHECK(rml_element_get_client_height(box) == doctest::Approx(60.f));
	CHECK(rml_element_get_absolute_left(box) == doctest::Approx(20.f));
	CHECK(rml_element_get_absolute_top(box) == doctest::Approx(30.f));

	const RmlVector2f border_offset = rml_element_get_absolute_offset(box, RML_BOX_AREA_BORDER);
	CHECK(border_offset.x == doctest::Approx(20.f));
	CHECK(border_offset.y == doctest::Approx(30.f));
	const RmlVector2f content_offset = rml_element_get_absolute_offset(box, RML_BOX_AREA_CONTENT);
	CHECK(content_offset.x == doctest::Approx(27.f));
	const RmlVector2f relative = rml_element_get_relative_offset(box, RML_BOX_AREA_BORDER);
	CHECK(relative.x == doctest::Approx(20.f));

	const RmlVector2f containing_block = rml_element_get_containing_block(label);
	CHECK(containing_block.x == doctest::Approx(100.f));
	CHECK(rml_element_get_line_height(label) > 0.f);
	CHECK(rml_element_get_baseline(label) >= 0.f);
	CHECK(rml_element_get_z_index(box) == doctest::Approx(3.f));
	CHECK(rml_element_get_clip_area(box) == RML_BOX_AREA_PADDING);
	CHECK(rml_element_get_font_face_handle(label) != 0);
	CHECK(rml_element_is_visible(box, true));

	CHECK(rml_element_is_point_within_element(box, RmlVector2f{60, 60}));
	CHECK_FALSE(rml_element_is_point_within_element(box, RmlVector2f{5, 5}));

	RmlVector2f point = {60, 60};
	CHECK(rml_element_project(box, &point));
	CHECK(point.x == doctest::Approx(60.f));
	CHECK(point.y == doctest::Approx(60.f));

	RmlRectanglef bounds = {};
	CHECK(rml_element_utilities_get_bounding_box(box, &bounds, RML_BOX_AREA_BORDER));
	CHECK(bounds.p0.x == doctest::Approx(20.f));
	CHECK(bounds.p1.x == doctest::Approx(134.f));
	CHECK(rml_element_utilities_get_dp_ratio(box) == doctest::Approx(1.f));
	rml_element_utilities_format_element(box, RmlVector2f{400, 300});

	// The image is a replaced element whose size comes from the render interface's load_texture.
	CHECK(rml_element_is_replaced(picture));
	session.Render();
	RmlVector2f intrinsic = {};
	float ratio = 0.f;
	REQUIRE(rml_element_get_intrinsic_dimensions(picture, &intrinsic, &ratio));
	CHECK(intrinsic.x == doctest::Approx(32.f));
	CHECK(intrinsic.y == doctest::Approx(32.f));
	CHECK(ratio == doctest::Approx(1.f));
	CHECK_FALSE(rml_element_get_intrinsic_dimensions(box, &intrinsic, &ratio));
	REQUIRE_FALSE(session.render_log.texture_sources.empty());
	CHECK(session.render_log.texture_sources[0].find("picture.png") != std::string::npos);
}

TEST_CASE("capi.element.scrolling")
{
	Session session("el_scroll");
	session.Load(tree_document);
	RmlElement* scroller = session.ById("scroller");
	RmlElement* s4 = session.ById("s4");
	REQUIRE(scroller);
	REQUIRE(s4);

	CHECK(rml_element_get_scroll_height(scroller) == doctest::Approx(160.f));
	CHECK(rml_element_get_scroll_width(scroller) <= 100.f);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(0.f));

	rml_element_set_scroll_top(scroller, 50.f);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(50.f));
	rml_element_set_scroll_left(scroller, 0.f);
	CHECK(rml_element_get_scroll_left(scroller) == doctest::Approx(0.f));

	rml_element_scroll_to(scroller, RmlVector2f{0.f, 20.f}, RML_SCROLL_BEHAVIOR_INSTANT);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(20.f));

	rml_element_scroll_into_view(s4, true);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(100.f));

	RmlScrollIntoViewOptions options = {RML_SCROLL_ALIGNMENT_START, RML_SCROLL_ALIGNMENT_NEAREST, RML_SCROLL_BEHAVIOR_INSTANT,
		RML_SCROLL_PARENTAGE_CLOSEST};
	rml_element_scroll_into_view_with_options(session.ById("s1"), options);
	CHECK(rml_element_get_scroll_top(scroller) == doctest::Approx(0.f));
}

TEST_CASE("capi.element.focus_and_tab_navigation")
{
	Session session("el_focus");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* first = session.ById("first_input");
	RmlElement* second = session.ById("second_input");
	REQUIRE(first);
	REQUIRE(second);

	CHECK(rml_element_focus(first, false));
	CHECK(rml_context_get_focus_element(session.ctx) == first);
	CHECK(rml_element_get_focus_leaf_node(rml_document_as_element(document)) == first);
	CHECK(rml_document_find_next_tab_element(document, first, true, false) == second);
	CHECK(rml_document_find_next_tab_element(document, second, false, false) == first);
	CHECK(rml_document_find_next_tab_element(document, second, true, true) == first);

	rml_context_process_key_down(session.ctx, RML_KEY_TAB, 0);
	CHECK(rml_context_get_focus_element(session.ctx) == second);
	CHECK(rml_element_is_pseudo_class_set(second, rml_sv("focus-visible")));
}

TEST_CASE("capi.element.event_dispatch_phases_and_propagation")
{
	Session session("el_events");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* list = session.ById("list");
	RmlElement* p1 = session.ById("p1");
	REQUIRE(p1);

	ListenerLog target_log;
	ListenerLog parent_log;
	ListenerLog capture_log;
	RmlEventListener* target_listener = MakeListener(&target_log);
	RmlEventListener* parent_listener = MakeListener(&parent_log);
	RmlEventListener* capture_listener = MakeListener(&capture_log);
	rml_element_add_event_listener(p1, rml_sv("custom"), target_listener, false);
	rml_element_add_event_listener(list, rml_sv("custom"), parent_listener, false);
	rml_element_add_event_listener(body, rml_sv("custom"), capture_listener, true);

	RmlDictionary* parameters = rml_dictionary_create();
	RmlVariant* value = rml_variant_create();
	rml_variant_set_string(value, rml_sv("payload"));
	rml_dictionary_set(parameters, rml_sv("message"), value);
	rml_variant_destroy(value);

	CHECK(rml_element_dispatch_event(p1, rml_sv("custom"), parameters));
	CHECK(capture_log.calls == 1);
	CHECK(target_log.calls == 1);
	CHECK(parent_log.calls == 1);
	CHECK(capture_log.phases[0] == RML_EVENT_PHASE_CAPTURE);
	CHECK(target_log.phases[0] == RML_EVENT_PHASE_TARGET);
	CHECK(parent_log.phases[0] == RML_EVENT_PHASE_BUBBLE);
	CHECK(parent_log.targets[0] == p1);
	CHECK(parent_log.current[0] == list);

	// Non-bubbling events stop at the target.
	CHECK(rml_element_dispatch_event_ex(p1, rml_sv("custom"), parameters, true, false));
	CHECK(target_log.calls == 2);
	CHECK(parent_log.calls == 1);

	// Stopping propagation at the target keeps the event from the parent and reports it as consumed.
	target_log.stop_propagation = true;
	CHECK_FALSE(rml_element_dispatch_event(p1, rml_sv("custom"), parameters));
	CHECK(target_log.calls == 3);
	CHECK(parent_log.calls == 1);

	// Non-interruptible events ignore stop requests.
	CHECK(rml_element_dispatch_event_ex(p1, rml_sv("custom"), parameters, false, true));
	CHECK(parent_log.calls == 2);
	target_log.stop_propagation = false;

	// Stopping immediate propagation also skips later listeners on the same element.
	ListenerLog second_log;
	RmlEventListener* second_listener = MakeListener(&second_log);
	rml_element_add_event_listener(p1, rml_sv("custom"), second_listener, false);
	target_log.stop_immediate = true;
	rml_element_dispatch_event(p1, rml_sv("custom"), parameters);
	CHECK(second_log.calls == 0);
	target_log.stop_immediate = false;
	rml_element_dispatch_event(p1, rml_sv("custom"), parameters);
	CHECK(second_log.calls == 1);

	rml_element_remove_event_listener(p1, rml_sv("custom"), target_listener, false);
	rml_element_remove_event_listener(p1, rml_sv("custom"), second_listener, false);
	rml_element_remove_event_listener(list, rml_sv("custom"), parent_listener, false);
	rml_element_remove_event_listener(body, rml_sv("custom"), capture_listener, true);
	rml_element_dispatch_event(p1, rml_sv("custom"), parameters);
	CHECK(target_log.calls == 6);

	rml_dictionary_destroy(parameters);
	rml_event_listener_destroy(target_listener);
	rml_event_listener_destroy(second_listener);
	rml_event_listener_destroy(parent_listener);
	rml_event_listener_destroy(capture_listener);
}

TEST_CASE("capi.element.event_accessors_inside_listener")
{
	Session session("el_event_access");
	session.Load(tree_document);
	RmlElement* p1 = session.ById("p1");
	REQUIRE(p1);

	struct Seen {
		std::string message;
		bool interruptible = false;
		bool propagating = false;
		bool immediate = false;
		bool propagating_after_stop = true;
		RmlVector2f mouse = {};
	} seen;

	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = [](void* user_data, RmlEvent* event) {
		Seen& s = *static_cast<Seen*>(user_data);
		const RmlVariant* message = rml_dictionary_get(rml_event_get_parameters(event), rml_sv("message"));
		if (message)
			s.message = ToString(rml_variant_get_string(message));
		s.interruptible = rml_event_is_interruptible(event);
		s.propagating = rml_event_is_propagating(event);
		s.immediate = rml_event_is_immediate_propagating(event);
		s.mouse = rml_event_get_unprojected_mouse_screen_pos(event);
		rml_event_stop_propagation(event);
		s.propagating_after_stop = rml_event_is_propagating(event);
	};
	RmlEventListener* listener = rml_event_listener_create(&callbacks, &seen);
	rml_element_add_event_listener(p1, rml_sv("probe"), listener, false);

	RmlDictionary* parameters = rml_dictionary_create();
	RmlVariant* value = rml_variant_create();
	rml_variant_set_string(value, rml_sv("hello"));
	rml_dictionary_set(parameters, rml_sv("message"), value);
	rml_variant_set_int(value, 12);
	rml_dictionary_set(parameters, rml_sv("mouse_x"), value);
	rml_variant_set_int(value, 34);
	rml_dictionary_set(parameters, rml_sv("mouse_y"), value);
	rml_variant_destroy(value);

	rml_element_dispatch_event(p1, rml_sv("probe"), parameters);
	CHECK(seen.message == "hello");
	CHECK(seen.interruptible);
	CHECK(seen.propagating);
	CHECK(seen.immediate);
	CHECK_FALSE(seen.propagating_after_stop);
	CHECK(seen.mouse.x == doctest::Approx(12.f));
	CHECK(seen.mouse.y == doctest::Approx(34.f));

	rml_dictionary_destroy(parameters);
	rml_element_remove_event_listener(p1, rml_sv("probe"), listener, false);
	rml_event_listener_destroy(listener);
}

TEST_CASE("capi.element.registered_event_types_and_listeners_by_id")
{
	Session session("el_event_ids");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* body = rml_document_as_element(document);
	RmlElement* p1 = session.ById("p1");
	REQUIRE(p1);

	const uint16_t bubbling = rml_register_event_type(rml_sv("capi-bubbles"), true, true, RML_DEFAULT_ACTION_PHASE_NONE);
	const uint16_t local = rml_register_event_type(rml_sv("capi-local"), false, false, RML_DEFAULT_ACTION_PHASE_TARGET);
	CHECK(bubbling != 0);
	CHECK(local != 0);
	CHECK(bubbling != local);
	CHECK(rml_register_event_type(rml_sv("capi-bubbles"), true, true, RML_DEFAULT_ACTION_PHASE_NONE) == bubbling);

	struct Seen {
		uint16_t id = 0;
		int calls = 0;
	} target_seen, body_seen;
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = [](void* user_data, RmlEvent* event) {
		Seen& s = *static_cast<Seen*>(user_data);
		s.id = rml_event_get_id(event);
		s.calls += 1;
	};
	RmlEventListener* target_listener = rml_event_listener_create(&callbacks, &target_seen);
	RmlEventListener* body_listener = rml_event_listener_create(&callbacks, &body_seen);

	rml_element_add_event_listener_by_id(p1, bubbling, target_listener, false);
	rml_element_add_event_listener_by_id(p1, local, target_listener, false);
	rml_element_add_event_listener_by_id(body, bubbling, body_listener, false);
	rml_element_add_event_listener_by_id(body, local, body_listener, false);

	rml_element_dispatch_event_by_id(p1, bubbling, nullptr);
	CHECK(target_seen.id == bubbling);
	CHECK(body_seen.calls == 1);

	rml_element_dispatch_event(p1, rml_sv("capi-local"), nullptr);
	CHECK(target_seen.id == local);
	CHECK(target_seen.calls == 2);
	CHECK(body_seen.calls == 1);

	rml_element_remove_event_listener_by_id(p1, bubbling, target_listener, false);
	rml_element_remove_event_listener_by_id(p1, local, target_listener, false);
	rml_element_remove_event_listener_by_id(body, bubbling, body_listener, false);
	rml_element_remove_event_listener_by_id(body, local, body_listener, false);
	rml_element_dispatch_event_by_id(p1, bubbling, nullptr);
	CHECK(target_seen.calls == 2);

	rml_event_listener_destroy(target_listener);
	rml_event_listener_destroy(body_listener);
}

TEST_CASE("capi.element.listener_attach_detach_and_self_destroy")
{
	Session session("el_attach");
	RmlElementDocument* document = session.Load(tree_document);
	RmlElement* list = session.ById("list");
	REQUIRE(list);

	AttachLog log;
	RmlEventListenerCallbacks callbacks = {};
	callbacks.process_event = &AttachProcess;
	callbacks.on_attach = &AttachOnAttach;
	callbacks.on_detach = &AttachOnDetach;
	RmlEventListener* listener = rml_event_listener_create(&callbacks, &log);
	REQUIRE(listener);
	log.self = listener;

	RmlElement* child = rml_document_create_element(document, rml_sv("div"));
	rml_element_append_child(list, child);
	rml_element_add_event_listener(child, rml_sv("click"), listener, false);
	CHECK(log.attached == 1);
	rml_element_click(child);
	CHECK(log.processed == 1);

	// Destroying the element detaches the listener, which may destroy itself from on_detach.
	log.destroy_on_detach = true;
	RmlElement* removed = rml_element_remove_child(list, child);
	rml_element_release(removed);
	CHECK(log.detached == 1);
	CHECK(log.self == nullptr);

	CHECK(rml_event_listener_create(nullptr, nullptr) == nullptr);
	RmlEventListenerCallbacks empty = {};
	CHECK(rml_event_listener_create(&empty, nullptr) == nullptr);
}
