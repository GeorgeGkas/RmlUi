#include "Harness.h"
#include <cstdint>
#include <doctest.h>
#include <vector>

using namespace CApiTest;

namespace {

std::string InnerRml(Session& session, const char* id)
{
	RmlElement* element = session.ById(id);
	REQUIRE(element);
	return Take(rml_element_get_inner_rml(element));
}

// --- Custom types: a struct with a string and an int member, and an array of ints ---

struct Player {
	std::string name;
	int score = 0;
};

struct TypeSet {
	RmlDataTypeDefinition* int_type = nullptr;
	RmlDataTypeDefinition* string_type = nullptr;
	RmlDataTypeDefinition* player_type = nullptr;
	RmlDataTypeDefinition* int_array_type = nullptr;
};

bool GetInt(void*, void* ptr, RmlVariant* out)
{
	rml_variant_set_int(out, *static_cast<int*>(ptr));
	return true;
}
bool SetInt(void*, void* ptr, const RmlVariant* value)
{
	return rml_variant_get_int(value, static_cast<int*>(ptr));
}
bool GetStdString(void*, void* ptr, RmlVariant* out)
{
	const std::string& value = *static_cast<std::string*>(ptr);
	rml_variant_set_string(out, RmlStringView{value.data(), value.size()});
	return true;
}
RmlDataVariable PlayerChild(void* user_data, void* ptr, RmlStringView name)
{
	const TypeSet& types = *static_cast<TypeSet*>(user_data);
	Player& player = *static_cast<Player*>(ptr);
	const std::string member = ToString(name);
	if (member == "name")
		return RmlDataVariable{types.string_type, &player.name};
	if (member == "score")
		return RmlDataVariable{types.int_type, &player.score};
	return RmlDataVariable{nullptr, nullptr};
}
void PlayerMembers(void*, RmlStringList* out_names)
{
	rml_string_list_push(out_names, rml_sv("name"));
	rml_string_list_push(out_names, rml_sv("score"));
}
int IntArraySize(void*, void* ptr)
{
	return static_cast<int>(static_cast<std::vector<int>*>(ptr)->size());
}
RmlDataVariable IntArrayChild(void* user_data, void* ptr, int index)
{
	const TypeSet& types = *static_cast<TypeSet*>(user_data);
	std::vector<int>& values = *static_cast<std::vector<int>*>(ptr);
	if (index < 0 || index >= static_cast<int>(values.size()))
		return RmlDataVariable{nullptr, nullptr};
	return RmlDataVariable{types.int_type, &values[static_cast<size_t>(index)]};
}

void CreateTypes(TypeSet& storage)
{
	RmlDataTypeDefinitionCallbacks int_callbacks = {};
	int_callbacks.get = &GetInt;
	int_callbacks.set = &SetInt;
	storage.int_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_SCALAR, &int_callbacks, nullptr);

	RmlDataTypeDefinitionCallbacks string_callbacks = {};
	string_callbacks.get = &GetStdString;
	storage.string_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_SCALAR, &string_callbacks, nullptr);

	RmlDataTypeDefinitionCallbacks player_callbacks = {};
	player_callbacks.child_by_name = &PlayerChild;
	player_callbacks.member_names = &PlayerMembers;
	storage.player_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_STRUCT, &player_callbacks, &storage);

	RmlDataTypeDefinitionCallbacks array_callbacks = {};
	array_callbacks.size = &IntArraySize;
	array_callbacks.child_by_index = &IntArrayChild;
	storage.int_array_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_ARRAY, &array_callbacks, &storage);
}

void DestroyTypes(TypeSet& types)
{
	rml_data_type_definition_destroy(types.int_type);
	rml_data_type_definition_destroy(types.string_type);
	rml_data_type_definition_destroy(types.player_type);
	rml_data_type_definition_destroy(types.int_array_type);
}

// --- Event arguments ---

struct EventArgs {
	int calls = 0;
	size_t size = 0;
	int first = 0;
	std::string second;
	bool out_of_range_is_null = false;
	bool has_event = false;
};

void Pick(void* user_data, RmlDataModelHandle handle, RmlEvent* event, const RmlVariantList* arguments)
{
	EventArgs& args = *static_cast<EventArgs*>(user_data);
	args.calls += 1;
	args.has_event = event != nullptr;
	args.size = rml_variant_list_size(arguments);
	if (args.size >= 2)
	{
		rml_variant_get_int(rml_variant_list_get(arguments, 0), &args.first);
		args.second = ToString(rml_variant_get_string(rml_variant_list_get(arguments, 1)));
	}
	args.out_of_range_is_null = rml_variant_list_get(arguments, args.size) == nullptr;
	rml_data_model_handle_dirty_variable(handle, rml_sv("picked"));
}

// --- Plugins ---

struct PluginLog {
	int event_classes = RML_PLUGIN_EVENT_ALL;
	int initialised = 0;
	int shut_down = 0;
	int contexts_created = 0;
	int contexts_destroyed = 0;
	std::vector<std::string> documents_opened;
	int documents_loaded = 0;
	int documents_unloaded = 0;
	int elements_created = 0;
	int elements_destroyed = 0;
	std::vector<std::string> models_created;
	std::vector<std::string> models_destroyed;
};

PluginLog& Pl(void* user_data)
{
	return *static_cast<PluginLog*>(user_data);
}

RmlPlugin* MakePlugin(PluginLog* log)
{
	RmlPluginCallbacks callbacks = {};
	callbacks.get_event_classes = [](void* user_data) { return Pl(user_data).event_classes; };
	callbacks.on_initialise = [](void* user_data) { Pl(user_data).initialised += 1; };
	callbacks.on_shutdown = [](void* user_data) { Pl(user_data).shut_down += 1; };
	callbacks.on_context_create = [](void* user_data, RmlContext*) { Pl(user_data).contexts_created += 1; };
	callbacks.on_context_destroy = [](void* user_data, RmlContext*) { Pl(user_data).contexts_destroyed += 1; };
	callbacks.on_document_open = [](void* user_data, RmlContext*, RmlStringView path) { Pl(user_data).documents_opened.push_back(ToString(path)); };
	callbacks.on_document_load = [](void* user_data, RmlElementDocument*) { Pl(user_data).documents_loaded += 1; };
	callbacks.on_document_unload = [](void* user_data, RmlElementDocument*) { Pl(user_data).documents_unloaded += 1; };
	callbacks.on_element_create = [](void* user_data, RmlElement*) { Pl(user_data).elements_created += 1; };
	callbacks.on_element_destroy = [](void* user_data, RmlElement*) { Pl(user_data).elements_destroyed += 1; };
	callbacks.on_data_model_create = [](void* user_data, RmlContext*, RmlStringView name) { Pl(user_data).models_created.push_back(ToString(name)); };
	callbacks.on_data_model_destroy = [](void* user_data, RmlContext*, RmlStringView name) {
		Pl(user_data).models_destroyed.push_back(ToString(name));
	};
	return rml_plugin_create(&callbacks, log);
}

} // namespace

TEST_CASE("capi.data_model.all_scalar_bindings_read_and_write")
{
	Session session("data_scalars");

	bool flag = false;
	int64_t big = -5000000000LL;
	uint64_t ubig = 10000000000ULL;
	float ratio = 1.5f;
	double precise = 0.25;
	int count = 3;

	RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("scalars"), false);
	REQUIRE(rml_data_model_bind_bool(constructor, rml_sv("flag"), &flag));
	REQUIRE(rml_data_model_bind_int64(constructor, rml_sv("big"), &big));
	REQUIRE(rml_data_model_bind_uint64(constructor, rml_sv("ubig"), &ubig));
	REQUIRE(rml_data_model_bind_float(constructor, rml_sv("ratio"), &ratio));
	REQUIRE(rml_data_model_bind_double(constructor, rml_sv("precise"), &precise));
	REQUIRE(rml_data_model_bind_int(constructor, rml_sv("count"), &count));

	// Names must be unique and pointers non-null.
	CHECK_FALSE(rml_data_model_bind_int(constructor, rml_sv("count"), &count));
	CHECK_FALSE(rml_data_model_bind_int(constructor, rml_sv("null_int"), nullptr));

	session.Load(R"(
<rml>
<head><style>body { font-family: LatoLatin; } input { display: block; width: 100px; height: 20px; }</style></head>
<body data-model="scalars">
	<input id="flag_box" type="checkbox" data-checked="flag"/>
	<p id="flag">{{ flag }}</p>
	<p id="big">{{ big }}</p>
	<p id="ubig">{{ ubig }}</p>
	<p id="ratio">{{ ratio }}</p>
	<p id="precise">{{ precise }}</p>
	<p id="sum">{{ count + 1 }}</p>
	<input id="ratio_input" type="range" min="0" max="10" step="0.5" data-value="ratio"/>
	<input id="count_input" type="range" min="0" max="10" step="1" data-value="count"/>
</body>
</rml>
)");

	CHECK(InnerRml(session, "flag") == "0");
	CHECK(InnerRml(session, "big") == "-5000000000");
	CHECK(InnerRml(session, "ubig") == "10000000000");
	CHECK(InnerRml(session, "ratio") == "1.5");
	CHECK(InnerRml(session, "precise") == "0.25");
	CHECK(InnerRml(session, "sum") == "4");

	// Two-way bindings write back into the bound memory.
	rml_element_click(session.ById("flag_box"));
	session.Update();
	CHECK(flag);
	CHECK(InnerRml(session, "flag") == "1");

	rml_form_control_set_value(rml_element_as_form_control(session.ById("ratio_input")), rml_sv("2.5"));
	session.Update();
	CHECK(ratio == doctest::Approx(2.5f));
	CHECK(InnerRml(session, "ratio") == "2.5");

	rml_form_control_set_value(rml_element_as_form_control(session.ById("count_input")), rml_sv("7"));
	session.Update();
	CHECK(count == 7);

	// Changes made by the host appear after dirtying.
	big = 42;
	precise = 0.5;
	RmlDataModelHandle handle = rml_data_model_get_handle(constructor);
	rml_data_model_handle_dirty_variable(handle, rml_sv("big"));
	rml_data_model_handle_dirty_variable(handle, rml_sv("precise"));
	session.Update();
	CHECK(InnerRml(session, "big") == "42");
	CHECK(InnerRml(session, "precise") == "0.5");
}

TEST_CASE("capi.data_model.event_arguments_arrive_as_variant_list")
{
	Session session("data_events");
	EventArgs args;
	int picked = 0;
	RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("events"), false);
	REQUIRE(rml_data_model_bind_int(constructor, rml_sv("picked"), &picked));
	REQUIRE(rml_data_model_bind_event_callback(constructor, rml_sv("pick"), &Pick, &args));
	CHECK_FALSE(rml_data_model_bind_event_callback(constructor, rml_sv("null_event"), nullptr, nullptr));

	session.Load(R"RML(
<rml>
<head><style>body { font-family: LatoLatin; } button { display: block; width: 50px; height: 20px; }</style></head>
<body data-model="events">
	<button id="go" data-event-click="pick(3, 'x')">Go</button>
	<button id="bare" data-event-click="pick">Bare</button>
</body>
</rml>
)RML");

	rml_element_click(session.ById("go"));
	CHECK(args.calls == 1);
	CHECK(args.has_event);
	CHECK(args.size == 2);
	CHECK(args.first == 3);
	CHECK(args.second == "x");
	CHECK(args.out_of_range_is_null);
	CHECK(rml_data_model_handle_is_variable_dirty(rml_data_model_get_handle(constructor), rml_sv("picked")));

	rml_element_click(session.ById("bare"));
	CHECK(args.calls == 2);
	CHECK(args.size == 0);
}

TEST_CASE("capi.data_model.struct_and_array_types_defined_in_c")
{
	Session session("data_types");
	TypeSet types;
	CreateTypes(types);
	REQUIRE(types.int_type);
	REQUIRE(types.string_type);
	REQUIRE(types.player_type);
	REQUIRE(types.int_array_type);

	Player player{"Ada", 10};
	std::vector<int> values = {1, 2, 3};

	{
		RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("types"), false);
		REQUIRE(rml_data_model_bind_custom(constructor, rml_sv("player"), types.player_type, &player));
		REQUIRE(rml_data_model_bind_custom(constructor, rml_sv("values"), types.int_array_type, &values));
		CHECK_FALSE(rml_data_model_bind_custom(constructor, rml_sv("null_type"), nullptr, &player));

		session.Load(R"(
<rml>
<head><style>body { font-family: LatoLatin; } p, div { display: block; } input { display: block; width: 50px; height: 20px; }</style></head>
<body data-model="types">
	<p id="name">{{ player.name }}</p>
	<p id="score">{{ player.score }}</p>
	<div id="list"><span data-for="value : values">{{ value }}</span></div>
	<p id="second">{{ values[1] }}</p>
	<input id="score_input" type="range" min="0" max="100" step="1" data-value="player.score"/>
</body>
</rml>
)");

		CHECK(InnerRml(session, "name") == "Ada");
		CHECK(InnerRml(session, "score") == "10");
		CHECK(InnerRml(session, "second") == "2");
		// data-for keeps its hidden template element as the list's last child.
		RmlElement* list = session.ById("list");
		CHECK(rml_element_get_num_children(list, false) == 3 + 1);
		CHECK(Take(rml_element_get_inner_rml(rml_element_get_child(list, 2))) == "3");

		values.push_back(4);
		player.name = "Grace";
		RmlDataModelHandle handle = rml_data_model_get_handle(constructor);
		rml_data_model_handle_dirty_variable(handle, rml_sv("values"));
		rml_data_model_handle_dirty_variable(handle, rml_sv("player"));
		session.Update();
		CHECK(rml_element_get_num_children(list, false) == 4 + 1);
		CHECK(InnerRml(session, "name") == "Grace");

		// Writes through a struct member reach the C set callback.
		rml_form_control_set_value(rml_element_as_form_control(session.ById("score_input")), rml_sv("99"));
		session.Update();
		CHECK(player.score == 99);

		rml_context_unload_all_documents(session.ctx);
		session.Update();
		CHECK(rml_context_remove_data_model(session.ctx, rml_sv("types")));
	}

	DestroyTypes(types);
}

TEST_CASE("capi.data_model.plugins_receive_events_for_their_classes")
{
	PluginLog all_log;
	PluginLog filtered_log;
	filtered_log.event_classes = RML_PLUGIN_EVENT_DOCUMENT | RML_PLUGIN_EVENT_DATA_MODEL;
	RmlPlugin* all_plugin = MakePlugin(&all_log);
	RmlPlugin* filtered_plugin = MakePlugin(&filtered_log);
	REQUIRE(all_plugin);
	REQUIRE(filtered_plugin);

	{
		Session session("plugins");
		rml_register_plugin(all_plugin);
		rml_register_plugin(filtered_plugin);
		// Registering after initialisation still delivers on_initialise.
		CHECK(all_log.initialised == 1);
		CHECK(filtered_log.initialised == 1);

		RmlContext* second = rml_create_context(rml_sv("plugins_second"), RmlVector2i{100, 100}, nullptr, nullptr);
		REQUIRE(second);
		CHECK(all_log.contexts_created == 1);
		CHECK(filtered_log.contexts_created == 0);

		int value = 0;
		RmlDataModelConstructor constructor = rml_context_create_data_model(session.ctx, rml_sv("observed"), false);
		REQUIRE(rml_data_model_bind_int(constructor, rml_sv("value"), &value));
		REQUIRE(all_log.models_created.size() == 1);
		CHECK(all_log.models_created[0] == "observed");
		REQUIRE(filtered_log.models_created.size() == 1);

		RmlElementDocument* document = session.Load("<rml><body data-model='observed'><p>{{ value }}</p></body></rml>", true, "observed.rml");
		REQUIRE(all_log.documents_opened.size() == 1);
		CHECK(all_log.documents_opened[0] == "observed.rml");
		CHECK(all_log.documents_loaded == 1);
		CHECK(filtered_log.documents_loaded == 1);
		CHECK(all_log.elements_created > 0);
		CHECK(filtered_log.elements_created == 0);

		rml_document_close(document);
		session.Update();
		CHECK(all_log.documents_unloaded == 1);
		CHECK(filtered_log.documents_unloaded == 1);
		CHECK(all_log.elements_destroyed > 0);
		CHECK(filtered_log.elements_destroyed == 0);

		CHECK(rml_context_remove_data_model(session.ctx, rml_sv("observed")));
		REQUIRE(all_log.models_destroyed.size() == 1);
		CHECK(all_log.models_destroyed[0] == "observed");
		CHECK(filtered_log.models_destroyed.size() == 1);

		CHECK(rml_remove_context(rml_sv("plugins_second")));
		CHECK(all_log.contexts_destroyed == 1);
		CHECK(filtered_log.contexts_destroyed == 0);

		// Unregistering while initialised delivers on_shutdown and stops further events.
		rml_unregister_plugin(all_plugin);
		CHECK(all_log.shut_down == 1);
		const int created = all_log.contexts_created;
		RmlContext* third = rml_create_context(rml_sv("plugins_third"), RmlVector2i{100, 100}, nullptr, nullptr);
		REQUIRE(third);
		CHECK(all_log.contexts_created == created);
		CHECK(rml_remove_context(rml_sv("plugins_third")));
	}
	CHECK(all_log.shut_down == 1);

	rml_plugin_destroy(all_plugin);
	rml_plugin_destroy(filtered_plugin);
	rml_plugin_destroy(nullptr);
}
