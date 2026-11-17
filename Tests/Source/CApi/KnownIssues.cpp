// Each test asserts the behavior the C API should have and is marked should_fail while the defect exists.
// When a defect is fixed its test starts passing, doctest reports that as a failure, and the marker must be removed.

#include "Harness.h"
#include <doctest.h>
#include <vector>

using namespace CApiTest;

namespace {

int destroyed_custom_elements = 0;

void CountDestroy(void*, RmlElement*)
{
	destroyed_custom_elements += 1;
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
	RmlDecoratorCallbacks callbacks = {};
	callbacks.generate_element_data = [](void*, RmlElement*, RmlBoxArea) -> RmlDecoratorDataHandle { return 1; };
	callbacks.release_element_data = [](void*, RmlDecoratorDataHandle) {};
	callbacks.render_element = [](void*, RmlElement*, RmlDecoratorDataHandle) {};
	return rml_decorator_create(&callbacks, nullptr);
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

TEST_CASE("capi.known_issues.custom_element_without_instancer_is_destroyed" * doctest::should_fail())
{
	destroyed_custom_elements = 0;
	RmlElementCallbacks callbacks = {};
	callbacks.on_destroy = &CountDestroy;
	{
		Session session("known_custom_element");
		RmlElementDocument* document = session.Load("<rml><body/></rml>");

		RmlElement* released = rml_element_create_custom(rml_sv("capi-direct"), &callbacks, nullptr);
		rml_element_release(released);

		RmlElement* appended = rml_element_create_custom(rml_sv("capi-direct"), &callbacks, nullptr);
		REQUIRE(rml_element_append_child(rml_document_as_element(document), appended));
		rml_context_unload_all_documents(session.ctx);
		session.Update();
	}
	CHECK(destroyed_custom_elements == 2);
}

TEST_CASE("capi.known_issues.decorator_instancer_properties_are_readable" * doctest::should_fail())
{
	DecoratorProbe probe;
	RmlDecoratorInstancer* instancer = rml_decorator_instancer_create(&ProbeDecorator, &probe);
	REQUIRE(instancer);
	{
		Session session("known_decorator");
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

TEST_CASE("capi.known_issues.plugin_shutdown_is_delivered_after_reinitialise" * doctest::should_fail())
{
	ShutdownCounter counter;
	RmlPlugin* plugin = MakeCountingPlugin(&counter);
	REQUIRE(plugin);

	for (int round = 0; round < 2; round++)
	{
		Session session("known_plugin");
		rml_register_plugin(plugin);
	}
	CHECK(counter.initialised == 2);

	{
		Session session("known_plugin");
		rml_register_plugin(plugin);
		rml_unregister_plugin(plugin);
	}
	CHECK(counter.initialised == 3);
	CHECK(counter.shut_down == 3);
	rml_plugin_destroy(plugin);
}

TEST_CASE("capi.known_issues.style_sheet_from_missing_file_returns_null" * doctest::should_fail())
{
	MemoryFileSystem fs;
	RmlFileInterface* file_interface = MakeFileInterface(&fs);
	SessionOptions options;
	options.file_interface = file_interface;
	options.load_default_font = false;
	{
		Session session("known_missing_sheet", options);
		RmlStyleSheetContainer* container = rml_style_sheet_container_create_from_file(rml_sv("missing.rcss"));
		CHECK(container == nullptr);
		rml_style_sheet_container_destroy(container);
		// Closing, reading, or seeking a handle that open never returned.
		CHECK(fs.invalid_handle_calls == 0);
	}
	rml_file_interface_destroy(file_interface);
}

TEST_CASE("capi.known_issues.array_types_defined_in_c_expose_size" * doctest::should_fail())
{
	Session session("known_array_size");
	RmlDataTypeDefinitionCallbacks int_callbacks = {};
	int_callbacks.get = &GetIntValue;
	RmlDataTypeDefinition* int_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_SCALAR, &int_callbacks, nullptr);
	RmlDataTypeDefinitionCallbacks array_callbacks = {};
	array_callbacks.size = &ArraySize;
	array_callbacks.child_by_index = &ArrayChild;
	RmlDataTypeDefinition* array_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_ARRAY, &array_callbacks, int_type);

	std::vector<int> values = {1, 2, 3};
	RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("known_array"), false);
	REQUIRE(rml_data_model_bind_custom(constructor, rml_sv("values"), array_type, &values));
	session.Load("<rml><body data-model='known_array'><p id='size'>{{ values.size }}</p></body></rml>");
	CHECK(Take(rml_element_get_inner_rml(session.ById("size"))) == "3");

	rml_context_unload_all_documents(session.ctx);
	session.Update();
	rml_context_remove_data_model(session.ctx, rml_sv("known_array"));
	rml_data_type_definition_destroy(array_type);
	rml_data_type_definition_destroy(int_type);
}

TEST_CASE("capi.known_issues.replace_child_leaves_new_child_when_old_child_is_not_a_child" * doctest::should_fail())
{
	Session session("known_replace_child");
	RmlElementDocument* document = session.Load("<rml><body><div id='a'/><div id='b'><div id='nested'/></div></body></rml>");
	RmlElement* a = session.ById("a");
	RmlElement* new_child = rml_document_create_element(document, rml_sv("span"));
	REQUIRE(new_child);

	// 'nested' is not a child of 'a'. Null is also returned when new_child is rejected, so the caller cannot tell
	// whether new_child was consumed.
	CHECK(rml_element_replace_child(a, new_child, session.ById("nested")) == nullptr);
	CHECK(rml_element_get_parent_node(new_child) == nullptr);
	if (!rml_element_get_parent_node(new_child))
		rml_element_release(new_child);
	CHECK(rml_element_get_num_children(a, false) == 0);
}

TEST_CASE("capi.known_issues.insert_before_null_adjacent_appends_like_cpp" * doctest::should_fail())
{
	Session session("known_insert_before");
	RmlElementDocument* document = session.Load("<rml><body><div id='a'/></body></rml>");
	RmlElement* body = rml_document_as_element(document);
	RmlElement* child = rml_document_create_element(document, rml_sv("span"));
	REQUIRE(child);

	CHECK(rml_element_insert_before(body, child, nullptr) == child);
	CHECK(rml_element_get_child(body, rml_element_get_num_children(body, false) - 1) == child);
	CHECK_FALSE(HasAssert(session.system_log));
}
