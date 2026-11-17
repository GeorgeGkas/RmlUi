#include "Harness.h"
#include <algorithm>
#include <doctest.h>
#include <vector>

using namespace CApiTest;

namespace {

struct WidgetLog {
	RmlElementCallbacks callbacks = {};
	std::vector<RmlElement*> created;
	std::vector<RmlElement*> released;
	std::vector<RmlElement*> destroyed;
	std::vector<std::string> instanced_tags;
	std::vector<std::string> instance_attribute_ids;
	int updates = 0;
	int renders = 0;
	int layouts = 0;
	int resizes = 0;
	std::vector<std::string> changed_attributes;
	std::vector<size_t> property_change_sizes;
	bool saw_width_change = false;
	std::vector<std::string> added_children;
	std::vector<std::string> removed_children;
	std::vector<std::string> default_actions;
	int intrinsic_queries = 0;
};

WidgetLog& Wl(void* user_data)
{
	return *static_cast<WidgetLog*>(user_data);
}

std::string IdOf(RmlElement* element)
{
	return ToString(rml_element_get_id(element));
}

void CollectKey(void* user_data, RmlStringView key, const RmlVariant*)
{
	static_cast<std::vector<std::string>*>(user_data)->push_back(ToString(key));
}

void InitWidgetCallbacks(WidgetLog& log)
{
	RmlElementCallbacks& c = log.callbacks;
	c.on_update = [](void* user_data, RmlElement*) { Wl(user_data).updates += 1; };
	c.on_render = [](void* user_data, RmlElement*) { Wl(user_data).renders += 1; };
	c.on_layout = [](void* user_data, RmlElement*) { Wl(user_data).layouts += 1; };
	c.on_resize = [](void* user_data, RmlElement*) { Wl(user_data).resizes += 1; };
	c.on_attribute_change = [](void* user_data, RmlElement*, const RmlDictionary* changed) {
		rml_dictionary_for_each(changed, &CollectKey, &Wl(user_data).changed_attributes);
	};
	c.on_property_change = [](void* user_data, RmlElement*, const RmlPropertyIdSet* changed) {
		WidgetLog& log = Wl(user_data);
		log.property_change_sizes.push_back(rml_property_id_set_size(changed));
		if (rml_property_id_set_contains(changed, rml_style_sheet_get_property_id(rml_sv("width"))))
			log.saw_width_change = true;
	};
	c.on_child_add = [](void* user_data, RmlElement*, RmlElement* child) { Wl(user_data).added_children.push_back(IdOf(child)); };
	c.on_child_remove = [](void* user_data, RmlElement*, RmlElement* child) { Wl(user_data).removed_children.push_back(IdOf(child)); };
	c.process_default_action = [](void* user_data, RmlElement*, RmlEvent* event) {
		Wl(user_data).default_actions.push_back(ToString(rml_event_get_type(event)));
	};
	c.get_intrinsic_dimensions = [](void* user_data, RmlElement* element, RmlVector2f* dimensions, float* ratio) {
		Wl(user_data).intrinsic_queries += 1;
		if (!rml_element_has_attribute(element, rml_sv("sized")))
			return false;
		*dimensions = RmlVector2f{40.f, 20.f};
		*ratio = 2.f;
		return true;
	};
	c.on_destroy = [](void* user_data, RmlElement* element) { Wl(user_data).destroyed.push_back(element); };
}

RmlElement* InstanceWidget(void* user_data, RmlElement*, RmlStringView tag, const RmlDictionary* attributes)
{
	WidgetLog& log = Wl(user_data);
	log.instanced_tags.push_back(ToString(tag));
	if (const RmlVariant* id = rml_dictionary_get(attributes, rml_sv("id")))
		log.instance_attribute_ids.push_back(ToString(rml_variant_get_string(id)));
	RmlElement* element = rml_element_create_custom(tag, &log.callbacks, user_data);
	log.created.push_back(element);
	return element;
}

void ReleaseWidget(void* user_data, RmlElement* element)
{
	Wl(user_data).released.push_back(element);
}

bool Contains(const std::vector<std::string>& values, const std::string& value)
{
	return std::find(values.begin(), values.end(), value) != values.end();
}

} // namespace

TEST_CASE("capi.extensions.custom_element_callbacks_via_instancer")
{
	WidgetLog log;
	InitWidgetCallbacks(log);
	RmlElementInstancerCallbacks instancer_callbacks = {};
	instancer_callbacks.instance = &InstanceWidget;
	instancer_callbacks.release = &ReleaseWidget;
	RmlElementInstancer* instancer = rml_element_instancer_create(&instancer_callbacks, &log);
	REQUIRE(instancer);

	{
		Session session("custom_elements");
		rml_register_element_instancer(rml_sv("capi-box"), instancer);

		session.Load(R"(
<rml>
<head><style>body { font-family: LatoLatin; } capi-box { display: block; width: 50px; height: 30px; }</style></head>
<body>
	<capi-box id="box"><span id="kid">x</span></capi-box>
	<capi-box id="sized" sized="" style="display: inline-block; width: auto; height: auto;"/>
</body>
</rml>
)");
		session.Render();

		REQUIRE(log.created.size() == 2);
		CHECK(log.instanced_tags == std::vector<std::string>{"capi-box", "capi-box"});
		CHECK(log.instance_attribute_ids == std::vector<std::string>{"box", "sized"});
		RmlElement* box = session.ById("box");
		RmlElement* sized = session.ById("sized");
		CHECK(box == log.created[0]);
		CHECK(sized == log.created[1]);

		CHECK(log.updates > 0);
		CHECK(log.renders > 0);
		CHECK(log.layouts > 0);
		CHECK(Contains(log.added_children, "kid"));
		CHECK_FALSE(log.property_change_sizes.empty());

		// Intrinsic dimensions from the callback size a replaced element.
		RmlVector2f dimensions = {};
		float ratio = 0.f;
		CHECK(rml_element_get_intrinsic_dimensions(sized, &dimensions, &ratio));
		CHECK(dimensions.x == doctest::Approx(40.f));
		CHECK(dimensions.y == doctest::Approx(20.f));
		CHECK(ratio == doctest::Approx(2.f));
		CHECK(rml_element_get_client_width(sized) == doctest::Approx(40.f));
		CHECK_FALSE(rml_element_get_intrinsic_dimensions(box, &dimensions, &ratio));
		CHECK(log.intrinsic_queries > 0);

		log.changed_attributes.clear();
		rml_element_set_attribute_string(box, rml_sv("data-x"), rml_sv("1"));
		CHECK(Contains(log.changed_attributes, "data-x"));

		log.saw_width_change = false;
		const int resizes = log.resizes;
		CHECK(rml_element_set_property(box, rml_sv("width"), rml_sv("70px")));
		session.Update();
		CHECK(log.saw_width_change);
		CHECK(log.resizes > resizes);

		rml_element_click(box);
		CHECK(Contains(log.default_actions, "click"));

		RmlElement* kid = rml_element_remove_child(box, session.ById("kid"));
		REQUIRE(kid);
		CHECK(Contains(log.removed_children, "kid"));
		rml_element_release(kid);

		CHECK(log.destroyed.empty());
		rml_context_unload_all_documents(session.ctx);
		session.Update();
		CHECK(log.released.size() == log.created.size());
		CHECK(log.destroyed.size() == log.created.size());
		std::sort(log.created.begin(), log.created.end());
		std::sort(log.released.begin(), log.released.end());
		std::sort(log.destroyed.begin(), log.destroyed.end());
		CHECK(log.released == log.created);
		CHECK(log.destroyed == log.created);
	}

	rml_element_instancer_destroy(instancer);
}

TEST_CASE("capi.extensions.custom_properties_and_shorthands")
{
	Session session("custom_properties");

	RmlPropertyDefinition* length = rml_style_sheet_register_property(rml_sv("capi-length"), rml_sv("0px"), false, false);
	REQUIRE(length);
	rml_property_definition_add_parser(length, rml_sv("length"), rml_sv(""));
	RmlPropertyDefinition* tint = rml_style_sheet_register_property(rml_sv("capi-tint"), rml_sv("black"), true, false);
	REQUIRE(tint);
	rml_property_definition_add_parser(tint, rml_sv("color"), rml_sv(""));
	CHECK(rml_style_sheet_register_shorthand(rml_sv("capi-both"), rml_sv("capi-length, capi-tint"), RML_SHORTHAND_TYPE_FALL_THROUGH) != 0);

	CHECK(rml_style_sheet_get_property_id(rml_sv("capi-length")) != 0);
	CHECK(rml_style_sheet_get_property_id(rml_sv("capi-tint")) != 0);
	CHECK(rml_style_sheet_get_property_id(rml_sv("capi-unknown")) == 0);

	session.Load(R"(
<rml>
<head><style>
	body { font-family: LatoLatin; }
	#parent { display: block; capi-both: 12px #00ff00; }
</style></head>
<body>
	<div id="parent"><div id="child"/></div>
	<div id="plain"/>
</body>
</rml>
)");

	RmlElement* parent = session.ById("parent");
	CHECK(Take(rml_element_get_property_string(parent, rml_sv("capi-length"))) == "12px");
	CHECK(Take(rml_element_get_property_string(parent, rml_sv("capi-tint"))) == "#00ff00");

	// Only the inherited property reaches the child; the other uses its default.
	RmlElement* child = session.ById("child");
	CHECK(Take(rml_element_get_property_string(child, rml_sv("capi-tint"))) == "#00ff00");
	CHECK(Take(rml_element_get_property_string(child, rml_sv("capi-length"))) == "0px");
	CHECK(Take(rml_element_get_property_string(session.ById("plain"), rml_sv("capi-tint"))) == "#000000");

	// Declarations are validated by the registered parsers.
	RmlPropertyDictionary* dictionary = rml_property_dictionary_create();
	CHECK(rml_style_sheet_parse_property_declaration(dictionary, rml_sv("capi-both"), rml_sv("3px red")));
	CHECK(Take(rml_property_dictionary_get_string(dictionary, rml_sv("capi-length"))) == "3px");
	CHECK_FALSE(rml_style_sheet_parse_property_declaration(dictionary, rml_sv("capi-length"), rml_sv("not-a-length")));
	rml_property_dictionary_destroy(dictionary);
}

TEST_CASE("capi.events.standalone_event_create_and_destroy")
{
	Session session("standalone_events");
	session.Load("<rml><body><div id='target'/></body></rml>");
	RmlElement* target = session.ById("target");
	CHECK(rml_element_get_context(target) == session.ctx);

	const uint16_t id = rml_register_event_type(rml_sv("capi-standalone"), true, true, RML_DEFAULT_ACTION_PHASE_NONE);
	RmlDictionary* parameters = rml_dictionary_create();
	RmlVariant* value = rml_variant_create();
	rml_variant_set_int(value, 2);
	rml_dictionary_set(parameters, rml_sv("button"), value);
	rml_variant_destroy(value);

	RmlEvent* event = rml_event_create(target, id, rml_sv("capi-standalone"), parameters, true);
	REQUIRE(event);
	// The event copies its parameters.
	rml_dictionary_destroy(parameters);

	CHECK(rml_event_get_id(event) == id);
	CHECK(ToString(rml_event_get_type(event)) == "capi-standalone");
	CHECK(rml_event_get_target_element(event) == target);
	CHECK(rml_event_is_interruptible(event));
	CHECK(rml_event_is_propagating(event));
	int button = 0;
	REQUIRE(rml_dictionary_get(rml_event_get_parameters(event), rml_sv("button")));
	CHECK(rml_variant_get_int(rml_dictionary_get(rml_event_get_parameters(event), rml_sv("button")), &button));
	CHECK(button == 2);
	rml_event_stop_propagation(event);
	CHECK_FALSE(rml_event_is_propagating(event));
	rml_event_destroy(event);

	RmlEvent* bare = rml_event_create(nullptr, 0, rml_sv("bare"), nullptr, false);
	REQUIRE(bare);
	CHECK(rml_event_get_target_element(bare) == nullptr);
	CHECK_FALSE(rml_event_is_interruptible(bare));
	CHECK(rml_dictionary_size(rml_event_get_parameters(bare)) == 0);
	rml_event_destroy(bare);
}
