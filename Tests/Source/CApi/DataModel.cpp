#include <RmlUi/CApi.h>
#include <doctest.h>
#include <string>

namespace {

struct SystemState {
	double elapsed = 0.0;
};

double SystemGetElapsedTime(void* user_data)
{
	return static_cast<SystemState*>(user_data)->elapsed;
}

bool SystemLogMessage(void*, RmlLogType, RmlStringView)
{
	return true;
}

RmlCompiledGeometryHandle RenderCompileGeometry(void*, const RmlVertex*, size_t, const int*, size_t)
{
	return 1;
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
RmlTextureHandle RenderGenerateTexture(void*, const unsigned char*, size_t, RmlVector2i)
{
	return 2;
}
void RenderReleaseTexture(void*, RmlTextureHandle) {}
void RenderEnableScissor(void*, bool) {}
void RenderSetScissor(void*, RmlRectanglei) {}

struct Harness {
	SystemState system_state;
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
		render_interface = rml_render_interface_create(&render_callbacks, nullptr);
		REQUIRE(system_interface);
		REQUIRE(render_interface);
		rml_set_system_interface(system_interface);
		rml_set_render_interface(render_interface);
		REQUIRE(rml_initialise());

		std::string font = std::string(RMLUI_CAPI_TESTS_SAMPLES_ASSETS) + "/LatoLatin-Regular.ttf";
		REQUIRE(rml_load_font_face(rml_sv(font.c_str()), false, RML_FONT_WEIGHT_AUTO, 0));
		ctx = rml_create_context(rml_sv("data"), RmlVector2i{800, 600}, nullptr, nullptr);
		REQUIRE(ctx);
	}

	void Teardown()
	{
		if (ctx)
			rml_remove_context(rml_sv("data"));
		rml_shutdown();
		if (render_interface)
			rml_render_interface_destroy(render_interface);
		if (system_interface)
			rml_system_interface_destroy(system_interface);
	}
};

void GetLabel(void* user_data, RmlVariant* out_value)
{
	const char* text = static_cast<const char*>(user_data);
	rml_variant_set_string(out_value, rml_sv(text));
}

bool GetCustomInt(void*, void* ptr, RmlVariant* out_value)
{
	rml_variant_set_int(out_value, *static_cast<int*>(ptr));
	return true;
}

bool SetCustomInt(void*, void* ptr, const RmlVariant* value)
{
	int out = 0;
	if (!rml_variant_get_int(value, &out))
		return false;
	*static_cast<int*>(ptr) = out;
	return true;
}

void OnGo(void* user_data, RmlDataModelHandle, RmlEvent*, const RmlVariantList*)
{
	*static_cast<int*>(user_data) += 1;
}

void TimesTwo(void*, const RmlVariantList* arguments, RmlVariant* out_result)
{
	int value = 0;
	const RmlVariant* argument = rml_variant_list_get(arguments, 0);
	if (argument)
		rml_variant_get_int(argument, &value);
	rml_variant_set_int(out_result, value * 2);
}

std::string OwnedToString(RmlOwnedString owned)
{
	std::string result;
	if (owned.data)
		result.assign(owned.data, owned.size);
	rml_owned_string_free(owned);
	return result;
}

} // namespace

TEST_CASE("capi.data_model.scalar_func_custom_and_event")
{
	Harness harness;
	harness.Setup();

	int counter = 7;
	int custom = 3;
	int clicks = 0;
	const char* label = "Ada";

	RmlDataModelConstructor constructor = rml_context_create_data_model(harness.ctx, rml_sv("hud"), false);
	REQUIRE(rml_data_model_constructor_is_valid(constructor));
	REQUIRE(rml_data_model_bind_int(constructor, rml_sv("counter"), &counter));
	REQUIRE(rml_data_model_bind_func(constructor, rml_sv("label"), &GetLabel, nullptr, const_cast<char*>(label)));

	RmlDataTypeDefinitionCallbacks type_callbacks = {};
	type_callbacks.get = &GetCustomInt;
	type_callbacks.set = &SetCustomInt;
	RmlDataTypeDefinition* custom_type = rml_data_type_definition_create(RML_DATA_VARIABLE_TYPE_SCALAR, &type_callbacks, nullptr);
	REQUIRE(custom_type);
	REQUIRE(rml_data_model_bind_custom(constructor, rml_sv("custom"), custom_type, &custom));
	REQUIRE(rml_data_model_bind_event_callback(constructor, rml_sv("on_go"), &OnGo, &clicks));
	rml_data_model_register_transform_func(constructor, rml_sv("times_two"), &TimesTwo, nullptr);

	const char* document_rml = R"(
		<rml>
		<head>
			<style>
				body { font-family: LatoLatin; font-size: 16dp; }
			</style>
		</head>
		<body data-model="hud">
			<p id="count">{{ counter }}</p>
			<p id="label">{{ label }}</p>
			<p id="custom">{{ custom }}</p>
			<p id="doubled">{{ counter | times_two }}</p>
			<button id="go" data-event-click="on_go">Go</button>
		</body>
		</rml>
	)";

	RmlElementDocument* document = rml_context_load_document_from_memory(harness.ctx, rml_sv(document_rml), rml_sv("[data]"));
	REQUIRE(document);
	rml_document_show(document, RML_MODAL_FLAG_NONE, RML_FOCUS_FLAG_AUTO, RML_SCROLL_FLAG_AUTO);
	REQUIRE(rml_context_update(harness.ctx));

	RmlElement* body = rml_document_as_element(document);
	CHECK(OwnedToString(rml_element_get_inner_rml(rml_element_get_element_by_id(body, rml_sv("count")))) == "7");
	CHECK(OwnedToString(rml_element_get_inner_rml(rml_element_get_element_by_id(body, rml_sv("label")))) == "Ada");
	CHECK(OwnedToString(rml_element_get_inner_rml(rml_element_get_element_by_id(body, rml_sv("custom")))) == "3");
	CHECK(OwnedToString(rml_element_get_inner_rml(rml_element_get_element_by_id(body, rml_sv("doubled")))) == "14");

	counter = 9;
	RmlDataModelHandle handle = rml_data_model_get_handle(constructor);
	rml_data_model_handle_dirty_variable(handle, rml_sv("counter"));
	REQUIRE(rml_context_update(harness.ctx));
	CHECK(OwnedToString(rml_element_get_inner_rml(rml_element_get_element_by_id(body, rml_sv("count")))) == "9");

	RmlElement* button = rml_element_get_element_by_id(body, rml_sv("go"));
	REQUIRE(button);
	rml_element_click(button);
	CHECK(clicks == 1);

	harness.Teardown();
	rml_data_type_definition_destroy(custom_type);
}
