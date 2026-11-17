#include "Harness.h"
#include <cstring>
#include <doctest.h>
#include <vector>

using namespace CApiTest;

namespace {

int destroyed_custom_elements = 0;

void CountDestroy(void*, RmlElement*)
{
	destroyed_custom_elements += 1;
}

RmlElement* MakeCountedElement(const char* tag)
{
	RmlElementCallbacks callbacks = {};
	callbacks.on_destroy = &CountDestroy;
	return rml_element_create_custom(rml_sv(tag), &callbacks, nullptr);
}

RmlDecorator* MakeNullDecorator()
{
	RmlDecoratorCallbacks callbacks = {};
	callbacks.generate_element_data = [](void*, RmlElement*, RmlBoxArea) -> RmlDecoratorDataHandle { return 1; };
	callbacks.release_element_data = [](void*, RmlDecoratorDataHandle) {};
	callbacks.render_element = [](void*, RmlElement*, RmlDecoratorDataHandle) {};
	return rml_decorator_create(&callbacks, nullptr);
}

struct DecoratorProbe {
	int instances = 0;
	std::string size;
};

RmlDecorator* ProbeDecorator(void* user_data, RmlStringView, const RmlPropertyDictionary* properties)
{
	DecoratorProbe& probe = *static_cast<DecoratorProbe*>(user_data);
	probe.instances += 1;
	probe.size = Take(rml_property_dictionary_get_string(properties, rml_sv("probe-size")));
	return MakeNullDecorator();
}

struct FilterProbe {
	int instances = 0;
	std::string amount;
};

RmlFilter* ProbeFilter(void* user_data, RmlStringView, const RmlPropertyDictionary* properties)
{
	FilterProbe& probe = *static_cast<FilterProbe*>(user_data);
	probe.instances += 1;
	probe.amount = Take(rml_property_dictionary_get_string(properties, rml_sv("probe-amount")));
	return rml_filter_create(nullptr, nullptr);
}

struct EffectProbe {
	int instances = 0;
	std::string width;
	int metrics = 0;
	int generated = 0;
	int destroyed = 0;
	RmlFontEffectLayer layer = RML_FONT_EFFECT_LAYER_BACK;
	RmlColourb colour = {};
};

RmlFontEffect* MakeProbeEffect(EffectProbe* probe)
{
	RmlFontEffectCallbacks callbacks = {};
	callbacks.has_unique_texture = [](void*) { return true; };
	callbacks.get_glyph_metrics = [](void* user_data, RmlVector2i* origin, RmlVector2i* dimensions, const RmlFontGlyph* glyph) {
		static_cast<EffectProbe*>(user_data)->metrics += 1;
		*origin = RmlVector2i{0, 0};
		*dimensions = glyph->bitmap_dimensions;
		return true;
	};
	callbacks.generate_glyph_texture = [](void* user_data, unsigned char* destination, RmlVector2i dimensions, int stride, const RmlFontGlyph*) {
		static_cast<EffectProbe*>(user_data)->generated += 1;
		for (int y = 0; y < dimensions.y; y++)
			std::memset(destination + y * stride, 0xff, static_cast<size_t>(dimensions.x) * 4);
	};
	callbacks.on_destroy = [](void* user_data) { static_cast<EffectProbe*>(user_data)->destroyed += 1; };
	return rml_font_effect_create_custom(&callbacks, probe);
}

RmlFontEffect* ProbeFontEffect(void* user_data, RmlStringView, const RmlPropertyDictionary* properties)
{
	EffectProbe& probe = *static_cast<EffectProbe*>(user_data);
	probe.instances += 1;
	probe.width = Take(rml_property_dictionary_get_string(properties, rml_sv("probe-width")));
	RmlFontEffect* effect = MakeProbeEffect(&probe);
	rml_font_effect_set_layer(effect, RML_FONT_EFFECT_LAYER_FRONT);
	rml_font_effect_set_colour(effect, RmlColourb{10, 20, 30, 255});
	probe.layer = rml_font_effect_get_layer(effect);
	probe.colour = rml_font_effect_get_colour(effect);
	return effect;
}

struct ShutdownCounter {
	int initialised = 0;
	int shut_down = 0;
};

RmlPlugin* MakeCountingPlugin(ShutdownCounter* counter)
{
	RmlPluginCallbacks callbacks = {};
	callbacks.on_initialise = [](void* user_data) { static_cast<ShutdownCounter*>(user_data)->initialised += 1; };
	callbacks.on_shutdown = [](void* user_data) { static_cast<ShutdownCounter*>(user_data)->shut_down += 1; };
	return rml_plugin_create(&callbacks, counter);
}

struct EventClassProbe {
	int queries = 0;
	int document_loads = 0;
	int element_creates = 0;
};

int ArraySize(void*, void* ptr)
{
	return static_cast<int>(static_cast<std::vector<int>*>(ptr)->size());
}

RmlDataVariable ArrayChild(void* user_data, void* ptr, int index)
{
	std::vector<int>& values = *static_cast<std::vector<int>*>(ptr);
	if (index < 0 || index >= static_cast<int>(values.size()))
		return RmlDataVariable{nullptr, nullptr};
	return RmlDataVariable{static_cast<RmlDataTypeDefinition*>(user_data), &values[static_cast<size_t>(index)]};
}

bool GetIntValue(void*, void* ptr, RmlVariant* out)
{
	rml_variant_set_int(out, *static_cast<int*>(ptr));
	return true;
}

bool HasAssert(const SystemLog& log)
{
	for (const auto& message : log.messages)
		if (message.first == RML_LOG_TYPE_ASSERT)
			return true;
	return false;
}

} // namespace

TEST_CASE("capi.regressions.custom_element_without_instancer_is_destroyed")
{
	destroyed_custom_elements = 0;
	{
		Session session("regression_custom_element");
		RmlElementDocument* document = session.Load("<rml><body/></rml>");

		rml_element_release(MakeCountedElement("capi-direct"));

		RmlElement* appended = MakeCountedElement("capi-direct");
		REQUIRE(rml_element_append_child(rml_document_as_element(document), appended));
		rml_context_unload_all_documents(session.ctx);
		session.Update();
	}
	CHECK(destroyed_custom_elements == 2);
}

TEST_CASE("capi.regressions.custom_elements_given_to_select_and_tab_set_are_destroyed")
{
	destroyed_custom_elements = 0;
	{
		Session session("regression_custom_adoption");
		session.Load("<rml><head><style>select, tabset { display: block; width: 100px; height: 20px; }</style></head><body>"
					 "<select id='sel'><option value='a'>A</option></select>"
					 "<tabset id='tabs'><tab>One</tab><panel>First</panel></tabset></body></rml>");
		RmlElementFormControlSelect* select = rml_element_as_form_control_select(session.ById("sel"));
		RmlElementTabSet* tabs = rml_element_as_tab_set(session.ById("tabs"));
		REQUIRE(select);
		REQUIRE(tabs);

		CHECK(rml_form_control_select_add_element(select, MakeCountedElement("option"), -1) == 1);
		rml_tab_set_set_tab_element(tabs, 0, MakeCountedElement("tab"));
		rml_tab_set_set_panel_element(tabs, 0, MakeCountedElement("panel"));
		session.Update();
		CHECK(destroyed_custom_elements == 0);

		rml_context_unload_all_documents(session.ctx);
		session.Update();
	}
	CHECK(destroyed_custom_elements == 3);
}

TEST_CASE("capi.regressions.context_roots_and_pending_documents_are_not_caller_owned")
{
	Session session("regression_not_caller_owned");
	session.Load("<rml><body><div id='holder'/></body></rml>");
	RmlElement* holder = session.ById("holder");
	REQUIRE(holder);
	RmlElementDocument* closing = session.Load("<rml><body/></rml>");

	RmlElement* root = rml_context_get_root_element(session.ctx);
	rml_element_release(root);
	CHECK(session.system_log.HasMessage("owned by RmlUi"));
	CHECK(rml_element_append_child(holder, root) == nullptr);
	CHECK(rml_context_get_root_element(session.ctx) == root);

	rml_document_close(closing);
	RmlElement* closing_element = rml_document_as_element(closing);
	CHECK(rml_element_append_child(holder, closing_element) == nullptr);
	rml_element_release(closing_element);
	CHECK(rml_element_get_num_children(holder, false) == 0);

	session.Update();
	session.Render();
	CHECK(rml_context_get_num_documents(session.ctx) == 1);
}

TEST_CASE("capi.regressions.replace_child_leaves_new_child_when_old_child_is_not_a_child")
{
	Session session("regression_replace_child");
	RmlElementDocument* document = session.Load("<rml><body><div id='a'/><div id='b'><div id='nested'/></div></body></rml>");
	RmlElement* a = session.ById("a");
	RmlElement* new_child = rml_document_create_element(document, rml_sv("span"));
	REQUIRE(new_child);

	CHECK(rml_element_replace_child(a, new_child, session.ById("nested")) == nullptr);
	CHECK(rml_element_get_parent_node(new_child) == nullptr);
	rml_element_release(new_child);
	CHECK(rml_element_get_num_children(a, false) == 0);
}

TEST_CASE("capi.regressions.insert_before_null_adjacent_appends_like_cpp")
{
	Session session("regression_insert_before");
	RmlElementDocument* document = session.Load("<rml><body><div id='a'/></body></rml>");
	RmlElement* body = rml_document_as_element(document);
	RmlElement* child = rml_document_create_element(document, rml_sv("span"));
	REQUIRE(child);

	CHECK(rml_element_insert_before(body, child, nullptr) == child);
	CHECK(rml_element_get_child(body, rml_element_get_num_children(body, false) - 1) == child);
	CHECK_FALSE(HasAssert(session.system_log));
}

TEST_CASE("capi.regressions.decorator_instancer_properties_are_readable")
{
	DecoratorProbe probe;
	RmlDecoratorInstancer* instancer = rml_decorator_instancer_create(&ProbeDecorator, &probe);
	REQUIRE(instancer);
	{
		Session session("regression_decorator");
		rml_register_decorator_instancer(rml_sv("probe"), instancer);
		RmlPropertyDefinition* size = rml_decorator_instancer_register_property(instancer, rml_sv("probe-size"), rml_sv("4px"));
		rml_property_definition_add_parser(size, rml_sv("length"), rml_sv(""));
		rml_decorator_instancer_register_shorthand(instancer, rml_sv("decorator"), rml_sv("probe-size"), RML_SHORTHAND_TYPE_FALL_THROUGH);

		session.Load("<rml><head><style>body { decorator: probe(9px); }</style></head><body/></rml>");
		session.Render();
	}
	rml_decorator_instancer_destroy(instancer);

	REQUIRE(probe.instances > 0);
	CHECK(probe.size == "9px");
}

TEST_CASE("capi.regressions.filter_instancer_properties_are_readable")
{
	FilterProbe probe;
	RmlFilterInstancer* instancer = rml_filter_instancer_create(&ProbeFilter, &probe);
	REQUIRE(instancer);
	{
		SessionOptions options;
		options.full_render_interface = true;
		Session session("regression_filter", options);
		rml_register_filter_instancer(rml_sv("probe-filter"), instancer);
		RmlPropertyDefinition* amount = rml_filter_instancer_register_property(instancer, rml_sv("probe-amount"), rml_sv("1"));
		rml_property_definition_add_parser(amount, rml_sv("number"), rml_sv(""));
		rml_filter_instancer_register_shorthand(instancer, rml_sv("filter"), rml_sv("probe-amount"), RML_SHORTHAND_TYPE_FALL_THROUGH);

		session.Load("<rml><head><style>body { filter: probe-filter(0.5); }</style></head><body/></rml>");
		session.Render();
	}
	rml_filter_instancer_destroy(instancer);

	REQUIRE(probe.instances > 0);
	CHECK(probe.amount == "0.5");
}

TEST_CASE("capi.regressions.custom_font_effect_with_default_font_engine")
{
	EffectProbe probe;
	RmlFontEffectInstancer* instancer = rml_font_effect_instancer_create(&ProbeFontEffect, &probe);
	REQUIRE(instancer);
	{
		Session session("regression_font_effect");
		rml_register_font_effect_instancer(rml_sv("probe-effect"), instancer);
		RmlPropertyDefinition* width = rml_font_effect_instancer_register_property(instancer, rml_sv("probe-width"), rml_sv("1px"), true);
		rml_property_definition_add_parser(width, rml_sv("length"), rml_sv(""));
		rml_font_effect_instancer_register_shorthand(instancer, rml_sv("font-effect"), rml_sv("probe-width"), RML_SHORTHAND_TYPE_FALL_THROUGH);

		session.Load("<rml><head><style>body { font-family: LatoLatin; font-size: 16px; font-effect: probe-effect(3px); }</style></head>"
					 "<body>Hi</body></rml>");
		session.Render();
		CHECK(probe.metrics > 0);
		CHECK(probe.generated > 0);
	}
	rml_font_effect_instancer_destroy(instancer);

	REQUIRE(probe.instances > 0);
	CHECK(probe.width == "3px");
	CHECK(probe.layer == RML_FONT_EFFECT_LAYER_FRONT);
	CHECK(probe.colour.red == 10);
	CHECK(probe.colour.alpha == 255);
	CHECK(probe.destroyed == probe.instances);
}

TEST_CASE("capi.regressions.font_effect_accessors_and_destroy")
{
	RmlFontEffect* base = rml_font_effect_create();
	REQUIRE(base);
	rml_font_effect_set_fingerprint(base, 42);
	CHECK(rml_font_effect_get_fingerprint(base) == 42);
	CHECK(rml_font_effect_get_layer(base) == RML_FONT_EFFECT_LAYER_BACK);
	CHECK_FALSE(rml_font_effect_has_unique_texture(base));
	RmlFontGlyph glyph = {};
	RmlVector2i origin = {}, dimensions = {};
	CHECK_FALSE(rml_font_effect_get_glyph_metrics(base, &origin, &dimensions, &glyph));
	rml_font_effect_destroy(base);

	EffectProbe probe;
	RmlFontEffect* custom = MakeProbeEffect(&probe);
	CHECK(rml_font_effect_has_unique_texture(custom));
	unsigned char bitmap[4] = {};
	glyph.bitmap_data = bitmap;
	glyph.bitmap_dimensions = RmlVector2i{2, 2};
	glyph.color_format = RML_COLOR_FORMAT_A8;
	CHECK(rml_font_effect_get_glyph_metrics(custom, &origin, &dimensions, &glyph));
	CHECK(dimensions.x == 2);
	unsigned char destination[16] = {};
	rml_font_effect_generate_glyph_texture(custom, destination, RmlVector2i{2, 2}, 8, &glyph);
	CHECK(destination[15] == 0xff);
	CHECK(probe.metrics == 1);
	CHECK(probe.generated == 1);
	rml_font_effect_destroy(custom);
	CHECK(probe.destroyed == 1);

	rml_decorator_destroy(MakeNullDecorator());
	rml_filter_destroy(rml_filter_create(nullptr, nullptr));
}

TEST_CASE("capi.regressions.plugin_shutdown_is_delivered_after_reinitialise")
{
	ShutdownCounter counter;
	RmlPlugin* plugin = MakeCountingPlugin(&counter);
	REQUIRE(plugin);

	for (int round = 0; round < 2; round++)
	{
		Session session("regression_plugin");
		rml_register_plugin(plugin);
	}
	CHECK(counter.initialised == 2);

	{
		Session session("regression_plugin");
		rml_register_plugin(plugin);
		rml_unregister_plugin(plugin);
	}
	CHECK(counter.initialised == 3);
	CHECK(counter.shut_down == 3);
	rml_plugin_destroy(plugin);
}

TEST_CASE("capi.regressions.plugin_event_classes_are_read_once")
{
	EventClassProbe probe;
	RmlPluginCallbacks callbacks = {};
	callbacks.get_event_classes = [](void* user_data) {
		static_cast<EventClassProbe*>(user_data)->queries += 1;
		return RML_PLUGIN_EVENT_DOCUMENT;
	};
	callbacks.on_document_load = [](void* user_data, RmlElementDocument*) { static_cast<EventClassProbe*>(user_data)->document_loads += 1; };
	callbacks.on_element_create = [](void* user_data, RmlElement*) { static_cast<EventClassProbe*>(user_data)->element_creates += 1; };
	RmlPlugin* plugin = rml_plugin_create(&callbacks, &probe);
	REQUIRE(plugin);
	CHECK(probe.queries == 1);
	{
		Session session("regression_plugin_classes");
		rml_register_plugin(plugin);
		session.Load("<rml><body><div/></body></rml>");
		rml_unregister_plugin(plugin);
	}
	rml_plugin_destroy(plugin);

	CHECK(probe.queries == 1);
	CHECK(probe.document_loads == 1);
	CHECK(probe.element_creates == 0);
}

TEST_CASE("capi.regressions.array_types_defined_in_c_expose_size")
{
	Session session("regression_array_size");
	RmlDataTypeDefinitionCallbacks int_callbacks = {};
	int_callbacks.get = &GetIntValue;
	RmlDataTypeDefinition* int_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_SCALAR, &int_callbacks, nullptr);
	RmlDataTypeDefinitionCallbacks array_callbacks = {};
	array_callbacks.size = &ArraySize;
	array_callbacks.child_by_index = &ArrayChild;
	RmlDataTypeDefinition* array_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_ARRAY, &array_callbacks, int_type);

	std::vector<int> values = {1, 2, 3};
	RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("regression_array"), false);
	REQUIRE(rml_data_model_bind_custom(constructor, rml_sv("values"), array_type, &values));
	session.Load("<rml><body data-model='regression_array'><p id='size'>{{ values.size }}</p></body></rml>");
	CHECK(Take(rml_element_get_inner_rml(session.ById("size"))) == "3");

	rml_context_unload_all_documents(session.ctx);
	session.Update();
	rml_context_remove_data_model(session.ctx, rml_sv("regression_array"));
	rml_data_type_definition_destroy(array_type);
	rml_data_type_definition_destroy(int_type);
}

TEST_CASE("capi.regressions.data_model_handles_and_bind_uint")
{
	Session session("regression_data_model_handles");
	RmlDataModelHandle invalid = {};
	CHECK_FALSE(rml_data_model_handle_is_valid(invalid));
	CHECK_FALSE(rml_data_model_handle_is_variable_dirty(invalid, rml_sv("big")));
	rml_data_model_handle_dirty_variable(invalid, rml_sv("big"));
	rml_data_model_handle_dirty_all_variables(invalid);
	CHECK(session.system_log.HasMessage("rml_data_model_handle_dirty_variable: invalid argument"));

	unsigned int big = 4000000000u;
	RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("regression_uint"), false);
	REQUIRE(rml_data_model_bind_uint(constructor, rml_sv("big"), &big));
	RmlDataModelHandle handle = rml_data_model_get_handle(constructor);
	CHECK(rml_data_model_handle_is_valid(handle));

	session.Load("<rml><body data-model='regression_uint'><p id='big'>{{ big }}</p></body></rml>");
	CHECK(Take(rml_element_get_inner_rml(session.ById("big"))) == "4000000000");
	big = 7;
	rml_data_model_handle_dirty_variable(handle, rml_sv("big"));
	CHECK(rml_data_model_handle_is_variable_dirty(handle, rml_sv("big")));
	session.Update();
	CHECK(Take(rml_element_get_inner_rml(session.ById("big"))) == "7");

	rml_context_unload_all_documents(session.ctx);
	session.Update();
	rml_context_remove_data_model(session.ctx, rml_sv("regression_uint"));
}

TEST_CASE("capi.regressions.null_arguments_are_rejected_in_every_build")
{
	Session session("regression_null_arguments");
	CHECK(rml_element_get_tag_name(nullptr).size == 0);
	rml_element_set_class(nullptr, rml_sv("x"), true);
	CHECK_FALSE(rml_context_update(nullptr));
	int value = 7;
	CHECK_FALSE(rml_variant_get_int(nullptr, &value));
	CHECK(value == 7);
	CHECK(rml_context_get_element_at_point(nullptr, RmlVector2f{0, 0}, nullptr) == nullptr);
	rml_owned_string_set(nullptr, rml_sv("x"));
	CHECK(session.system_log.HasMessage("rml_element_set_class: invalid argument, expected 'element'"));
	CHECK(session.system_log.HasMessage("rml_context_update: invalid argument"));
	CHECK_FALSE(HasAssert(session.system_log));
}

TEST_CASE("capi.regressions.owned_string_set_accepts_its_own_contents")
{
	RmlOwnedString owned = {};
	rml_owned_string_set(&owned, rml_sv("hello world"));
	rml_owned_string_set(&owned, RmlStringView{owned.data + 6, 5});
	CHECK(ToString(RmlStringView{owned.data, owned.size}) == "world");
	rml_owned_string_set(&owned, RmlStringView{owned.data, owned.size});
	CHECK(Take(owned) == "world");
}

TEST_CASE("capi.regressions.element_at_point_within_searches_only_the_given_subtree")
{
	Session session("regression_element_at_point");
	session.Load("<rml><head><style>div { display: block; width: 50px; height: 50px; }</style></head>"
				 "<body><div id='a'/><div id='b'/></body></rml>");
	RmlElement* a = session.ById("a");
	RmlElement* b = session.ById("b");
	RmlVector2f inside_a = {10, 10};

	CHECK(rml_context_get_element_at_point_within(session.ctx, inside_a, nullptr, nullptr) == a);
	CHECK(rml_context_get_element_at_point_within(session.ctx, inside_a, nullptr, a) == a);
	CHECK(rml_context_get_element_at_point_within(session.ctx, inside_a, nullptr, b) == nullptr);
	RmlElement* behind_a = rml_context_get_element_at_point_within(session.ctx, inside_a, a, nullptr);
	CHECK(behind_a != nullptr);
	CHECK(behind_a != a);
}
