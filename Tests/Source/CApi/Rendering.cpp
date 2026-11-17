#include "Harness.h"
#include <algorithm>
#include <cstring>
#include <doctest.h>
#include <vector>

using namespace CApiTest;

namespace {

void MakeQuad(RmlVertex (&vertices)[4], int (&indices)[6], float size)
{
	const RmlVector2f corners[4] = {{0, 0}, {size, 0}, {size, size}, {0, size}};
	for (int i = 0; i < 4; i++)
	{
		vertices[i] = RmlVertex{};
		vertices[i].position = corners[i];
		vertices[i].colour = RmlColourbPremultiplied{255, 255, 255, 255};
		vertices[i].tex_coord = RmlVector2f{corners[i].x / size, corners[i].y / size};
	}
	const int quad[6] = {0, 1, 2, 0, 2, 3};
	std::copy(quad, quad + 6, indices);
}

bool GenerateWhite(void* user_data, const RmlTextureGenerator* generator)
{
	const RmlVector2i dimensions = *static_cast<const RmlVector2i*>(user_data);
	std::vector<unsigned char> pixels(static_cast<size_t>(dimensions.x * dimensions.y * 4), 0xff);
	return rml_texture_generator_generate(generator, pixels.data(), pixels.size(), dimensions);
}

bool SubmitHandle(void*, const RmlTextureGenerator* generator)
{
	rml_texture_generator_set_handle(generator, 4242, RmlVector2i{3, 5});
	return true;
}

bool Contains(const std::vector<RmlTextureHandle>& handles, RmlTextureHandle low, RmlTextureHandle high)
{
	return std::any_of(handles.begin(), handles.end(), [&](RmlTextureHandle handle) { return handle >= low && handle < high; });
}

bool Contains(const std::vector<std::string>& values, const std::string& value)
{
	return std::find(values.begin(), values.end(), value) != values.end();
}

bool HasDimensions(const std::vector<RmlVector2i>& values, int x, int y)
{
	return std::any_of(values.begin(), values.end(), [&](RmlVector2i value) { return value.x == x && value.y == y; });
}

// --- Custom element drawing through the geometry, texture and shader API ---

struct Canvas {
	RmlTextureSource* file_source = nullptr;
	RmlTextureSource* generated_source = nullptr;
	RmlTextureSource* handle_source = nullptr;
	RmlVector2i generated_size = {2, 2};
	RmlGeometry* geometry = nullptr;
	RmlCompiledShader* shader = nullptr;
	RmlTexture file_texture = {};
	RmlTexture generated_texture = {};
	RmlTexture handle_texture = {};
	int renders = 0;
};

void RenderCanvas(void* user_data, RmlElement* element)
{
	Canvas& canvas = *static_cast<Canvas*>(user_data);
	canvas.renders += 1;
	if (!canvas.geometry)
	{
		RmlVertex vertices[4];
		int indices[6];
		MakeQuad(vertices, indices, 16.f);
		canvas.geometry = rml_geometry_create(element, vertices, 4, indices, 6);
		canvas.file_texture = rml_texture_source_get_texture(canvas.file_source, element);
		canvas.generated_texture = rml_texture_source_get_texture(canvas.generated_source, element);
		canvas.handle_texture = rml_texture_source_get_texture(canvas.handle_source, element);

		RmlDictionary* parameters = rml_dictionary_create();
		RmlVariant* value = rml_variant_create();
		rml_variant_set_float(value, 0.25f);
		rml_dictionary_set(parameters, rml_sv("strength"), value);
		canvas.shader = rml_compiled_shader_create(element, rml_sv("probe-shader"), parameters);
		rml_variant_destroy(value);
		rml_dictionary_destroy(parameters);
	}

	const RmlVector2f offset = rml_element_get_absolute_offset(element, RML_BOX_AREA_BORDER);
	rml_geometry_render(canvas.geometry, offset, canvas.file_texture, nullptr);
	rml_geometry_render(canvas.geometry, offset, canvas.generated_texture, nullptr);
	rml_geometry_render(canvas.geometry, offset, canvas.handle_texture, nullptr);
	rml_geometry_render(canvas.geometry, offset, RmlTexture{}, canvas.shader);
}

// --- Decorator that loads a texture and a sprite through the instancer interface ---

struct ImageDecorator {
	int instances = 0;
	std::string sprite_name;
	RmlTexture texture = {};
	RmlTexture sprite = {};
	RmlTexture missing_sprite = {};
	RmlRectanglef sprite_rectangle = {};
	float display_scale = 0.f;
	int generated = 0;
	int released = 0;
	int renders = 0;
};

RmlDecoratorDataHandle GenerateImageData(void* user_data, RmlElement* element, RmlBoxArea)
{
	static_cast<ImageDecorator*>(user_data)->generated += 1;
	RmlVertex vertices[4];
	int indices[6];
	MakeQuad(vertices, indices, 8.f);
	return reinterpret_cast<RmlDecoratorDataHandle>(rml_geometry_create(element, vertices, 4, indices, 6));
}

void ReleaseImageData(void* user_data, RmlDecoratorDataHandle data)
{
	static_cast<ImageDecorator*>(user_data)->released += 1;
	rml_geometry_destroy(reinterpret_cast<RmlGeometry*>(data));
}

void RenderImage(void* user_data, RmlElement* element, RmlDecoratorDataHandle data)
{
	ImageDecorator& probe = *static_cast<ImageDecorator*>(user_data);
	probe.renders += 1;
	const RmlVector2f offset = rml_element_get_absolute_offset(element, RML_BOX_AREA_BORDER);
	rml_geometry_render(reinterpret_cast<RmlGeometry*>(data), offset, probe.texture, nullptr);
	rml_geometry_render(reinterpret_cast<RmlGeometry*>(data), offset, probe.sprite, nullptr);
}

RmlDecorator* InstanceImageDecorator(void* user_data, RmlStringView, const RmlPropertyDictionary* properties,
	const RmlDecoratorInstancerInterface* instancer_interface)
{
	ImageDecorator& probe = *static_cast<ImageDecorator*>(user_data);
	probe.instances += 1;
	probe.sprite_name = Take(rml_property_dictionary_get_string(properties, rml_sv("probe-image")));
	probe.texture = rml_decorator_instancer_interface_get_texture(instancer_interface, rml_sv("tex.png"));
	probe.sprite = rml_decorator_instancer_interface_get_sprite(instancer_interface, rml_sv(probe.sprite_name.c_str()), &probe.sprite_rectangle,
		&probe.display_scale);
	probe.missing_sprite = rml_decorator_instancer_interface_get_sprite(instancer_interface, rml_sv("no-such-sprite"), nullptr, nullptr);

	RmlDecoratorCallbacks callbacks = {};
	callbacks.generate_element_data = &GenerateImageData;
	callbacks.release_element_data = &ReleaseImageData;
	callbacks.render_element = &RenderImage;
	return rml_decorator_create(&callbacks, &probe);
}

// --- Font engine in C that renders glyphs from a generated atlas ---

struct AtlasEngine {
	RmlTextureSource* atlas = nullptr;
	RmlVector2i atlas_size = {4, 4};
	int strings = 0;
	int shutdowns = 0;
};

AtlasEngine& Ae(void* user_data)
{
	return *static_cast<AtlasEngine*>(user_data);
}

RmlFontEngineInterface* MakeAtlasEngine(AtlasEngine* engine)
{
	RmlFontEngineInterfaceCallbacks callbacks = {};
	callbacks.initialize = [](void* user_data) {
		AtlasEngine& engine = Ae(user_data);
		engine.atlas = rml_texture_source_create_from_callback(&GenerateWhite, &engine.atlas_size);
	};
	callbacks.shutdown = [](void* user_data) {
		AtlasEngine& engine = Ae(user_data);
		engine.shutdowns += 1;
		rml_texture_source_destroy(engine.atlas);
		engine.atlas = nullptr;
	};
	callbacks.load_font_face = [](void*, RmlStringView, int, bool, RmlFontWeight) { return true; };
	callbacks.get_font_face_handle = [](void*, RmlStringView, RmlFontStyle, RmlFontWeight, int) -> RmlFontFaceHandle { return 1; };
	callbacks.get_font_metrics = [](void*, RmlFontFaceHandle, RmlFontMetrics* out) {
		out->size = 16;
		out->ascent = 12.f;
		out->descent = 4.f;
		out->line_spacing = 20.f;
		out->x_height = 8.f;
		return true;
	};
	callbacks.get_string_width = [](void*, RmlFontFaceHandle, RmlStringView string, const RmlTextShapingContext*, uint32_t) {
		return static_cast<int>(string.size) * 8;
	};
	callbacks.generate_string = [](void* user_data, RmlFontFaceHandle, RmlFontEffectsHandle, RmlStringView string, RmlVector2f,
									RmlColourbPremultiplied, float, const RmlTextShapingContext*, RmlTexturedMeshList* out_meshes) {
		AtlasEngine& engine = Ae(user_data);
		engine.strings += 1;
		RmlVertex vertices[4];
		int indices[6];
		MakeQuad(vertices, indices, 8.f);
		rml_textured_mesh_list_push_textured(out_meshes, vertices, 4, indices, 6, engine.atlas);
		return static_cast<int>(string.size) * 8;
	};
	return rml_font_engine_interface_create(&callbacks, engine);
}

// --- Minimal render interface for a second render manager ---

RmlRenderInterface* MakeMinimalRenderInterface()
{
	RmlRenderInterfaceCallbacks callbacks = {};
	callbacks.compile_geometry = [](void*, const RmlVertex*, size_t, const int*, size_t) -> RmlCompiledGeometryHandle { return 1; };
	callbacks.render_geometry = [](void*, RmlCompiledGeometryHandle, RmlVector2f, RmlTextureHandle) {};
	callbacks.release_geometry = [](void*, RmlCompiledGeometryHandle) {};
	callbacks.load_texture = [](void*, RmlVector2i* out_dimensions, RmlStringView) -> RmlTextureHandle {
		*out_dimensions = RmlVector2i{1, 1};
		return 77;
	};
	callbacks.generate_texture = [](void*, const unsigned char*, size_t, RmlVector2i) -> RmlTextureHandle { return 78; };
	callbacks.release_texture = [](void*, RmlTextureHandle) {};
	callbacks.enable_scissor_region = [](void*, bool) {};
	callbacks.set_scissor_region = [](void*, RmlRectanglei) {};
	return rml_render_interface_create(&callbacks, nullptr);
}

// --- Text input handler that keeps the active context for IME calls ---

struct ImeProbe {
	RmlTextInputContext* context = nullptr;
};

RmlTextInputHandler* MakeImeHandler(ImeProbe* probe)
{
	RmlTextInputHandlerCallbacks callbacks = {};
	callbacks.on_activate = [](void* user_data, RmlTextInputContext* context) { static_cast<ImeProbe*>(user_data)->context = context; };
	callbacks.on_deactivate = [](void* user_data, RmlTextInputContext*) { static_cast<ImeProbe*>(user_data)->context = nullptr; };
	callbacks.on_destroy = [](void* user_data, RmlTextInputContext*) { static_cast<ImeProbe*>(user_data)->context = nullptr; };
	return rml_text_input_handler_create(&callbacks, probe);
}

std::string InputValue(RmlElement* element)
{
	return Take(rml_form_control_get_value(rml_element_as_form_control(element)));
}

} // namespace

TEST_CASE("capi.render.custom_element_draws_geometry_with_textures_and_shaders")
{
	SessionOptions options;
	options.full_render_interface = true;
	Session session("render_canvas", options);
	RmlElementDocument* document = session.Load(
		"<rml><head><style>capi-canvas { display: block; width: 16px; height: 16px; }</style></head><body/></rml>", true, "ui/canvas.rml");

	Canvas canvas;
	canvas.file_source = rml_texture_source_create_from_file(rml_sv("image.png"), rml_sv("ui/canvas.rml"));
	canvas.generated_source = rml_texture_source_create_from_callback(&GenerateWhite, &canvas.generated_size);
	canvas.handle_source = rml_texture_source_create_from_callback(&SubmitHandle, nullptr);
	REQUIRE(canvas.file_source);
	REQUIRE(canvas.generated_source);
	REQUIRE(canvas.handle_source);

	RmlElementCallbacks callbacks = {};
	callbacks.on_render = &RenderCanvas;
	RmlElement* element = rml_element_create_custom(rml_sv("capi-canvas"), &callbacks, &canvas);
	CHECK(rml_geometry_create(element, nullptr, 0, nullptr, 0) == nullptr);
	CHECK(rml_compiled_shader_create(element, rml_sv("probe-shader"), nullptr) == nullptr);
	CHECK_FALSE(rml_texture_is_valid(rml_texture_source_get_texture(canvas.file_source, element)));
	REQUIRE(rml_element_append_child(rml_document_as_element(document), element));

	session.Update();
	session.Render();
	REQUIRE(canvas.renders > 0);
	REQUIRE(canvas.geometry);
	REQUIRE(canvas.shader);

	CHECK(rml_texture_is_valid(canvas.file_texture));
	CHECK(rml_texture_is_valid(canvas.generated_texture));
	CHECK_FALSE(rml_texture_is_valid(RmlTexture{}));
	CHECK(rml_texture_get_dimensions(canvas.file_texture).x == 32);
	CHECK(rml_texture_get_dimensions(canvas.generated_texture).y == 2);
	CHECK(rml_texture_get_dimensions(RmlTexture{}).x == 0);

	CHECK(Contains(session.render_log.texture_sources, "ui/image.png"));
	CHECK(HasDimensions(session.render_log.generated_dimensions, 2, 2));
	CHECK(Contains(session.render_log.rendered_textures, 1000, 2000));
	CHECK(Contains(session.render_log.rendered_textures, 2000, 3000));
	CHECK(Contains(session.render_log.rendered_textures, 4242, 4243));
	CHECK(rml_texture_get_dimensions(canvas.handle_texture).y == 5);
	CHECK(Contains(session.render_log.compiled_shaders, "probe-shader"));
	CHECK(Contains(session.render_log.shader_parameter_keys, "strength"));
	CHECK(session.render_log.rendered_shaders > 0);

	RmlStringList* sources = rml_string_list_create();
	rml_get_texture_source_list(sources);
	bool listed = false;
	for (size_t i = 0; i < rml_string_list_size(sources); i++)
		listed |= ToString(rml_string_list_get(sources, i)) == "ui/image.png";
	CHECK(listed);
	rml_string_list_destroy(sources);

	const int released_geometry = session.render_log.released_geometry;
	rml_geometry_destroy(canvas.geometry);
	CHECK(session.render_log.released_geometry == released_geometry + 1);
	rml_compiled_shader_destroy(canvas.shader);
	CHECK(session.render_log.released_shaders == 1);

	rml_context_unload_all_documents(session.ctx);
	session.Update();
	const int released_textures = session.render_log.released_textures;
	rml_texture_source_destroy(canvas.handle_source);
	CHECK(session.render_log.released_textures == released_textures + 1);
	rml_texture_source_destroy(canvas.file_source);
	rml_texture_source_destroy(canvas.generated_source);
}

TEST_CASE("capi.render.textures_from_another_render_manager_are_rejected")
{
	RmlRenderInterface* other_render = MakeMinimalRenderInterface();
	REQUIRE(other_render);
	{
		Session session("render_mismatch");
		session.Load("<rml><head><style>div { display: block; width: 10px; height: 10px; }</style></head><body><div id='box'/></body></rml>");
		RmlContext* other = rml_create_context(rml_sv("render_mismatch_other"), RmlVector2i{100, 100}, other_render, nullptr);
		REQUIRE(other);

		RmlElementDocument* other_document = rml_context_load_document_from_memory(other, rml_sv("<rml><body/></rml>"), rml_sv("other.rml"));
		REQUIRE(other_document);

		RmlTextureSource* source = rml_texture_source_create_from_file(rml_sv("other.png"), rml_sv(""));
		CHECK_FALSE(rml_texture_is_valid(rml_texture_source_get_texture(source, rml_context_get_root_element(other))));
		RmlTexture foreign = rml_texture_source_get_texture(source, rml_document_as_element(other_document));
		CHECK(rml_texture_is_valid(foreign));

		RmlVertex vertices[4];
		int indices[6];
		MakeQuad(vertices, indices, 4.f);
		RmlGeometry* geometry = rml_geometry_create(session.ById("box"), vertices, 4, indices, 6);
		REQUIRE(geometry);
		const int rendered = session.render_log.rendered_geometry;
		rml_geometry_render(geometry, RmlVector2f{0, 0}, foreign, nullptr);
		CHECK(session.render_log.rendered_geometry == rendered);
		CHECK(session.system_log.HasMessage("different render manager"));

		rml_geometry_destroy(geometry);
		rml_texture_source_destroy(source);
		CHECK(rml_remove_context(rml_sv("render_mismatch_other")));
	}
	rml_render_interface_destroy(other_render);
}

TEST_CASE("capi.render.decorator_loads_textures_and_sprites_through_its_instancer_interface")
{
	ImageDecorator probe;
	RmlDecoratorInstancer* instancer = rml_decorator_instancer_create_with_interface(&InstanceImageDecorator, &probe);
	REQUIRE(instancer);
	{
		Session session("render_decorator");
		rml_register_decorator_instancer(rml_sv("probe-image"), instancer);
		RmlPropertyDefinition* image = rml_decorator_instancer_register_property(instancer, rml_sv("probe-image"), rml_sv(""));
		rml_property_definition_add_parser(image, rml_sv("string"), rml_sv(""));
		rml_decorator_instancer_register_shorthand(instancer, rml_sv("decorator"), rml_sv("probe-image"), RML_SHORTHAND_TYPE_FALL_THROUGH);

		session.Load(R"RML(
<rml><head><style>
	@spritesheet probe-sheet { src: sheet.png; resolution: 2x; probe-icon: 2px 4px 8px 6px; }
	div { display: block; width: 20px; height: 20px; decorator: probe-image(probe-icon); }
</style></head><body><div/></body></rml>
)RML",
			true, "ui/decorated.rml");
		session.Render();

		REQUIRE(probe.instances > 0);
		CHECK(probe.sprite_name == "probe-icon");
		CHECK(rml_texture_is_valid(probe.texture));
		CHECK(rml_texture_is_valid(probe.sprite));
		CHECK_FALSE(rml_texture_is_valid(probe.missing_sprite));
		CHECK(probe.sprite_rectangle.p0.x == doctest::Approx(2.f));
		CHECK(probe.sprite_rectangle.p0.y == doctest::Approx(4.f));
		CHECK(probe.sprite_rectangle.p1.x == doctest::Approx(10.f));
		CHECK(probe.sprite_rectangle.p1.y == doctest::Approx(10.f));
		CHECK(probe.display_scale == doctest::Approx(0.5f));
		CHECK(probe.generated > 0);
		CHECK(probe.renders > 0);
		CHECK(Contains(session.render_log.texture_sources, "ui/tex.png"));
		CHECK(Contains(session.render_log.texture_sources, "ui/sheet.png"));

		rml_context_unload_all_documents(session.ctx);
		session.Update();
		CHECK(probe.released == probe.generated);
	}
	rml_decorator_instancer_destroy(instancer);
}

TEST_CASE("capi.render.c_font_engine_renders_glyphs_from_a_callback_texture")
{
	AtlasEngine engine;
	RmlFontEngineInterface* font_engine = MakeAtlasEngine(&engine);
	REQUIRE(font_engine);
	{
		SessionOptions options;
		options.font_engine = font_engine;
		options.load_default_font = false;
		Session session("render_font_atlas", options);
		REQUIRE(engine.atlas);
		session.Load("<rml><head><style>body { font-family: atlas; font-size: 16px; }</style></head><body>abc</body></rml>");
		session.Render();

		CHECK(engine.strings > 0);
		CHECK(HasDimensions(session.render_log.generated_dimensions, 4, 4));
		CHECK(Contains(session.render_log.rendered_textures, 2000, 3000));
	}
	CHECK(engine.shutdowns == 1);
	CHECK(engine.atlas == nullptr);
	rml_font_engine_interface_destroy(font_engine);
}

TEST_CASE("capi.render.text_input_context_supports_ime_editing")
{
	ImeProbe probe;
	RmlTextInputHandler* handler = MakeImeHandler(&probe);
	REQUIRE(handler);
	{
		SessionOptions options;
		options.context_text_input_handler = handler;
		Session session("render_ime", options);
		session.Load("<rml><head><style>input { display: block; width: 100px; height: 20px; }</style></head>"
					 "<body><input id='field' type='text' value='hello'/></body></rml>");
		RmlElement* field = session.ById("field");
		rml_element_focus(field, false);
		REQUIRE(probe.context);
		RmlTextInputContext* context = probe.context;

		rml_text_input_context_set_selection_range(context, 1, 3);
		int start = -1, end = -1;
		rml_text_input_context_get_selection_range(context, &start, &end);
		CHECK(start == 1);
		CHECK(end == 3);

		rml_text_input_context_set_text(context, rml_sv("J"), 0, 1);
		CHECK(InputValue(field) == "Jello");

		rml_text_input_context_set_cursor_position(context, 5);
		rml_text_input_context_get_selection_range(context, &start, &end);
		CHECK(start == 5);
		CHECK(end == 5);

		rml_text_input_context_set_composition_range(context, 0, 1);
		rml_text_input_context_commit_composition(context, rml_sv("Y"));
		CHECK(InputValue(field) == "Yello");

		session.Update();
		RmlRectanglef box = {};
		CHECK(rml_text_input_context_get_bounding_box(context, &box));
		CHECK(box.p1.x - box.p0.x >= 100.f);

		rml_element_blur(field);
		CHECK(probe.context == nullptr);
	}
	rml_text_input_handler_destroy(handler);
}
