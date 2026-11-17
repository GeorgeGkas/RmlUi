#include "Conversions.h"
#include <RmlUi/CApi/Factory.h>
#include <RmlUi/CApi/Interfaces.h>
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/Decorator.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementInstancer.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Event.h>
#include <RmlUi/Core/EventInstancer.h>
#include <RmlUi/Core/EventListenerInstancer.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/Filter.h>
#include <RmlUi/Core/FontEffect.h>
#include <RmlUi/Core/FontEffectInstancer.h>
#include <RmlUi/Core/FontEngineInterface.h>
#include <RmlUi/Core/FontGlyph.h>
#include <RmlUi/Core/Mesh.h>
#include <RmlUi/Core/Plugin.h>
#include <RmlUi/Core/PropertyDictionary.h>
#include <RmlUi/Core/RenderManager.h>
#include <RmlUi/Core/Spritesheet.h>
#include <RmlUi/Core/TextShapingContext.h>
#include <algorithm>
#include <map>

namespace {

class PluginAdapter final : public Rml::Plugin {
public:
	// RmlUi queries the event classes on both registration and unregistration, so they are read once to stay constant.
	PluginAdapter(const RmlPluginCallbacks& callbacks, void* user_data) :
		callbacks(callbacks), user_data(user_data), event_classes(callbacks.get_event_classes ? callbacks.get_event_classes(user_data) : EVT_ALL)
	{}

	int GetEventClasses() override { return event_classes; }
	void OnInitialise() override
	{
		if (callbacks.on_initialise)
			callbacks.on_initialise(user_data);
	}
	void SetRegistered(bool value) { is_registered = value; }
	bool IsRegistered() const { return is_registered; }

	void OnShutdown() override
	{
		if (callbacks.on_shutdown)
			callbacks.on_shutdown(user_data);
	}
	void OnContextCreate(Rml::Context* context) override
	{
		if (callbacks.on_context_create)
			callbacks.on_context_create(user_data, Rml::CApi::ToC(context));
	}
	void OnContextDestroy(Rml::Context* context) override
	{
		if (callbacks.on_context_destroy)
			callbacks.on_context_destroy(user_data, Rml::CApi::ToC(context));
	}
	void OnDocumentOpen(Rml::Context* context, const Rml::String& document_path) override
	{
		if (callbacks.on_document_open)
			callbacks.on_document_open(user_data, Rml::CApi::ToC(context), Rml::CApi::ToStringView(document_path));
	}
	void OnDocumentLoad(Rml::ElementDocument* document) override
	{
		if (callbacks.on_document_load)
			callbacks.on_document_load(user_data, Rml::CApi::ToC(document));
	}
	void OnDocumentUnload(Rml::ElementDocument* document) override
	{
		if (callbacks.on_document_unload)
			callbacks.on_document_unload(user_data, Rml::CApi::ToC(document));
	}
	void OnElementCreate(Rml::Element* element) override
	{
		if (callbacks.on_element_create)
			callbacks.on_element_create(user_data, Rml::CApi::ToC(element));
	}
	void OnElementDestroy(Rml::Element* element) override
	{
		if (callbacks.on_element_destroy)
			callbacks.on_element_destroy(user_data, Rml::CApi::ToC(element));
	}
	void OnDataModelCreate(Rml::Context* context, const Rml::String& name) override
	{
		if (callbacks.on_data_model_create)
			callbacks.on_data_model_create(user_data, Rml::CApi::ToC(context), Rml::CApi::ToStringView(name));
	}
	void OnDataModelDestroy(Rml::Context* context, const Rml::String& name) override
	{
		if (callbacks.on_data_model_destroy)
			callbacks.on_data_model_destroy(user_data, Rml::CApi::ToC(context), Rml::CApi::ToStringView(name));
	}

private:
	RmlPluginCallbacks callbacks;
	void* user_data;
	int event_classes;
	bool is_registered = false;
};

// Plugins registered through the C API. Rml::Shutdown clears the plugin registry, so registrations end there.
Rml::Vector<PluginAdapter*>& RegisteredPlugins()
{
	static Rml::Vector<PluginAdapter*> plugins;
	return plugins;
}

class CustomElement final : public Rml::Element {
public:
	CustomElement(const Rml::String& tag, const RmlElementCallbacks& callbacks, void* user_data) :
		Rml::Element(tag), callbacks(callbacks), user_data(user_data)
	{}

	~CustomElement() override
	{
		if (callbacks.on_destroy)
			callbacks.on_destroy(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)));
	}

protected:
	void OnUpdate() override
	{
		if (callbacks.on_update)
			callbacks.on_update(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)));
		else
			Element::OnUpdate();
	}
	void OnRender() override
	{
		if (callbacks.on_render)
			callbacks.on_render(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)));
		else
			Element::OnRender();
	}
	void OnLayout() override
	{
		if (callbacks.on_layout)
			callbacks.on_layout(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)));
		else
			Element::OnLayout();
	}
	void OnResize() override
	{
		if (callbacks.on_resize)
			callbacks.on_resize(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)));
		else
			Element::OnResize();
	}
	void OnAttributeChange(const Rml::ElementAttributes& changed_attributes) override
	{
		Element::OnAttributeChange(changed_attributes);
		if (callbacks.on_attribute_change)
			callbacks.on_attribute_change(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)),
				reinterpret_cast<const RmlDictionary*>(&changed_attributes));
	}
	void OnPropertyChange(const Rml::PropertyIdSet& changed_properties) override
	{
		Element::OnPropertyChange(changed_properties);
		if (callbacks.on_property_change)
			callbacks.on_property_change(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)),
				reinterpret_cast<const RmlPropertyIdSet*>(&changed_properties));
	}
	void OnChildAdd(Rml::Element* child) override
	{
		Element::OnChildAdd(child);
		if (callbacks.on_child_add)
			callbacks.on_child_add(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)), Rml::CApi::ToC(child));
	}
	void OnChildRemove(Rml::Element* child) override
	{
		if (callbacks.on_child_remove)
			callbacks.on_child_remove(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)), Rml::CApi::ToC(child));
		Element::OnChildRemove(child);
	}
	void ProcessDefaultAction(Rml::Event& event) override
	{
		Element::ProcessDefaultAction(event);
		if (callbacks.process_default_action)
			callbacks.process_default_action(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)), Rml::CApi::ToC(&event));
	}
	bool GetIntrinsicDimensions(Rml::Vector2f& dimensions, float& ratio) override
	{
		if (!callbacks.get_intrinsic_dimensions)
			return Element::GetIntrinsicDimensions(dimensions, ratio);
		RmlVector2f c_dimensions = Rml::CApi::ToC(dimensions);
		if (!callbacks.get_intrinsic_dimensions(user_data, Rml::CApi::ToC(static_cast<Rml::Element*>(this)), &c_dimensions, &ratio))
			return false;
		dimensions = Rml::CApi::FromC(c_dimensions);
		return true;
	}

private:
	RmlElementCallbacks callbacks;
	void* user_data;
};

class ElementInstancerAdapter final : public Rml::ElementInstancer {
public:
	ElementInstancerAdapter(const RmlElementInstancerCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}

	Rml::ElementPtr InstanceElement(Rml::Element* parent, const Rml::String& tag, const Rml::XMLAttributes& attributes) override
	{
		if (!callbacks.instance)
			return nullptr;
		RmlElement* element = callbacks.instance(user_data, Rml::CApi::ToC(parent), Rml::CApi::ToStringView(tag),
			reinterpret_cast<const RmlDictionary*>(&attributes));
		if (!element)
			return nullptr;
		return Rml::ElementPtr(Rml::CApi::FromC(element));
	}

	void ReleaseElement(Rml::Element* element) override
	{
		if (!element)
			return;
		if (callbacks.release)
			callbacks.release(user_data, Rml::CApi::ToC(element));
		delete element;
	}

private:
	RmlElementInstancerCallbacks callbacks;
	void* user_data;
};

class DecoratorAdapter final : public Rml::Decorator {
public:
	DecoratorAdapter(const RmlDecoratorCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}

	Rml::DecoratorDataHandle GenerateElementData(Rml::Element* element, Rml::BoxArea paint_area) const override
	{
		if (!callbacks.generate_element_data)
			return 0;
		return callbacks.generate_element_data(user_data, Rml::CApi::ToC(element), static_cast<RmlBoxArea>(paint_area));
	}
	void ReleaseElementData(Rml::DecoratorDataHandle element_data) const override
	{
		if (callbacks.release_element_data)
			callbacks.release_element_data(user_data, element_data);
	}
	void RenderElement(Rml::Element* element, Rml::DecoratorDataHandle element_data) const override
	{
		if (callbacks.render_element)
			callbacks.render_element(user_data, Rml::CApi::ToC(element), element_data);
	}

private:
	RmlDecoratorCallbacks callbacks;
	void* user_data;
};

template <typename Instancer>
Rml::CApi::PropertyDictionaryView MakePropertyView(const Rml::PropertyDictionary& properties, const Instancer& instancer)
{
	return Rml::CApi::PropertyDictionaryView{&properties, &instancer.GetPropertySpecification()};
}

const RmlPropertyDictionary* ToC(const Rml::CApi::PropertyDictionaryView& view)
{
	return reinterpret_cast<const RmlPropertyDictionary*>(&view);
}

class DecoratorInstancerAdapter final : public Rml::DecoratorInstancer {
public:
	DecoratorInstancerAdapter(RmlDecoratorInstanceFunc instance, RmlDecoratorInstanceWithInterfaceFunc instance_with_interface, void* user_data) :
		instance(instance), instance_with_interface(instance_with_interface), user_data(user_data)
	{}

	Rml::SharedPtr<Rml::Decorator> InstanceDecorator(const Rml::String& name, const Rml::PropertyDictionary& properties,
		const Rml::DecoratorInstancerInterface& instancer_interface) override
	{
		const Rml::CApi::PropertyDictionaryView view = MakePropertyView(properties, *this);
		RmlDecorator* decorator = nullptr;
		if (instance_with_interface)
			decorator = instance_with_interface(user_data, Rml::CApi::ToStringView(name), ToC(view),
				reinterpret_cast<const RmlDecoratorInstancerInterface*>(&instancer_interface));
		else if (instance)
			decorator = instance(user_data, Rml::CApi::ToStringView(name), ToC(view));
		if (!decorator)
			return nullptr;
		return Rml::SharedPtr<Rml::Decorator>(reinterpret_cast<Rml::Decorator*>(decorator));
	}

	Rml::PropertyDefinition* Register(const Rml::String& name, const Rml::String& default_value)
	{
		return &RegisterProperty(name, default_value);
	}

	Rml::ShorthandId RegisterShorthandPublic(const Rml::String& shorthand_name, const Rml::String& property_names, Rml::ShorthandType type)
	{
		return RegisterShorthand(shorthand_name, property_names, type);
	}

private:
	RmlDecoratorInstanceFunc instance;
	RmlDecoratorInstanceWithInterfaceFunc instance_with_interface;
	void* user_data;
};

class FilterAdapter final : public Rml::Filter {
public:
	explicit FilterAdapter(const RmlFilterCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}

	Rml::CompiledFilter CompileFilter(Rml::Element* element) const override
	{
		if (!callbacks.compile_filter || !element || !element->GetRenderManager())
			return {};

		RmlOwnedString name = {};
		Rml::Dictionary parameters;
		const bool compile = callbacks.compile_filter(user_data, Rml::CApi::ToC(element), &name, Rml::CApi::ToC(&parameters));
		Rml::String filter_name;
		if (name.data)
			filter_name.assign(name.data, name.size);
		rml_owned_string_free(name);
		if (!compile)
			return {};
		return element->GetRenderManager()->CompileFilter(filter_name, parameters);
	}

	void ExtendInkOverflow(Rml::Element* element, Rml::Rectanglef& overflow) const override
	{
		if (!callbacks.extend_ink_overflow)
			return;
		RmlRectanglef c_overflow = Rml::CApi::ToC(overflow);
		callbacks.extend_ink_overflow(user_data, Rml::CApi::ToC(element), &c_overflow);
		overflow = Rml::CApi::FromC(c_overflow);
	}

private:
	RmlFilterCallbacks callbacks;
	void* user_data;
};

class FilterInstancerAdapter final : public Rml::FilterInstancer {
public:
	FilterInstancerAdapter(RmlFilterInstanceFunc instance, void* user_data) : instance(instance), user_data(user_data) {}

	Rml::SharedPtr<Rml::Filter> InstanceFilter(const Rml::String& name, const Rml::PropertyDictionary& properties) override
	{
		if (!instance)
			return nullptr;
		const Rml::CApi::PropertyDictionaryView view = MakePropertyView(properties, *this);
		RmlFilter* filter = instance(user_data, Rml::CApi::ToStringView(name), ToC(view));
		if (!filter)
			return nullptr;
		return Rml::SharedPtr<Rml::Filter>(reinterpret_cast<Rml::Filter*>(filter));
	}

	Rml::PropertyDefinition* Register(const Rml::String& name, const Rml::String& default_value)
	{
		return &RegisterProperty(name, default_value);
	}

	Rml::ShorthandId RegisterShorthandPublic(const Rml::String& shorthand_name, const Rml::String& property_names, Rml::ShorthandType type)
	{
		return RegisterShorthand(shorthand_name, property_names, type);
	}

private:
	RmlFilterInstanceFunc instance;
	void* user_data;
};

class FontEffectInstancerAdapter final : public Rml::FontEffectInstancer {
public:
	FontEffectInstancerAdapter(RmlFontEffectInstanceFunc instance, void* user_data) : instance(instance), user_data(user_data) {}

	Rml::SharedPtr<Rml::FontEffect> InstanceFontEffect(const Rml::String& name, const Rml::PropertyDictionary& properties) override
	{
		if (!instance)
			return nullptr;
		const Rml::CApi::PropertyDictionaryView view = MakePropertyView(properties, *this);
		RmlFontEffect* effect = instance(user_data, Rml::CApi::ToStringView(name), ToC(view));
		if (!effect)
			return nullptr;
		return Rml::SharedPtr<Rml::FontEffect>(reinterpret_cast<Rml::FontEffect*>(effect));
	}

	Rml::PropertyDefinition* Register(const Rml::String& name, const Rml::String& default_value, bool affects_generation)
	{
		return &RegisterProperty(name, default_value, affects_generation);
	}

	Rml::ShorthandId RegisterShorthandPublic(const Rml::String& shorthand_name, const Rml::String& property_names, Rml::ShorthandType type)
	{
		return RegisterShorthand(shorthand_name, property_names, type);
	}

private:
	RmlFontEffectInstanceFunc instance;
	void* user_data;
};

Rml::FontGlyph FromC(const RmlFontGlyph& glyph)
{
	Rml::FontGlyph result;
	result.bearing = Rml::CApi::FromC(glyph.bearing);
	result.advance = glyph.advance;
	result.bitmap_data = glyph.bitmap_data;
	result.bitmap_dimensions = Rml::CApi::FromC(glyph.bitmap_dimensions);
	result.color_format = glyph.color_format == RML_COLOR_FORMAT_RGBA8 ? Rml::ColorFormat::RGBA8 : Rml::ColorFormat::A8;
	return result;
}

RmlFontGlyph ToC(const Rml::FontGlyph& glyph)
{
	RmlFontGlyph result = {};
	result.bearing = Rml::CApi::ToC(glyph.bearing);
	result.advance = glyph.advance;
	result.bitmap_data = glyph.bitmap_data;
	result.bitmap_dimensions = Rml::CApi::ToC(glyph.bitmap_dimensions);
	result.color_format = glyph.color_format == Rml::ColorFormat::RGBA8 ? RML_COLOR_FORMAT_RGBA8 : RML_COLOR_FORMAT_A8;
	return result;
}

class FontEffectAdapter final : public Rml::FontEffect {
public:
	FontEffectAdapter(const RmlFontEffectCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}
	~FontEffectAdapter() override
	{
		if (callbacks.on_destroy)
			callbacks.on_destroy(user_data);
	}

	bool HasUniqueTexture() const override
	{
		if (!callbacks.has_unique_texture)
			return FontEffect::HasUniqueTexture();
		return callbacks.has_unique_texture(user_data);
	}
	bool GetGlyphMetrics(Rml::Vector2i& origin, Rml::Vector2i& dimensions, const Rml::FontGlyph& glyph) const override
	{
		if (!callbacks.get_glyph_metrics)
			return FontEffect::GetGlyphMetrics(origin, dimensions, glyph);
		RmlVector2i c_origin = Rml::CApi::ToC(origin);
		RmlVector2i c_dimensions = Rml::CApi::ToC(dimensions);
		const RmlFontGlyph c_glyph = ToC(glyph);
		if (!callbacks.get_glyph_metrics(user_data, &c_origin, &c_dimensions, &c_glyph))
			return false;
		origin = Rml::CApi::FromC(c_origin);
		dimensions = Rml::CApi::FromC(c_dimensions);
		return true;
	}
	void GenerateGlyphTexture(Rml::byte* destination_data, Rml::Vector2i destination_dimensions, int destination_stride,
		const Rml::FontGlyph& glyph) const override
	{
		if (!callbacks.generate_glyph_texture)
			return FontEffect::GenerateGlyphTexture(destination_data, destination_dimensions, destination_stride, glyph);
		const RmlFontGlyph c_glyph = ToC(glyph);
		callbacks.generate_glyph_texture(user_data, destination_data, Rml::CApi::ToC(destination_dimensions), destination_stride, &c_glyph);
	}

private:
	RmlFontEffectCallbacks callbacks;
	void* user_data;
};

// What an RmlTexturedMeshList handle points to during generate_string.
struct TexturedMeshListView {
	Rml::TexturedMeshList* list;
	Rml::RenderManager* render_manager;
};

class EventInstancerAdapter final : public Rml::EventInstancer {
public:
	EventInstancerAdapter(const RmlEventInstancerCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}

	Rml::EventPtr InstanceEvent(Rml::Element* target, Rml::EventId id, const Rml::String& type, const Rml::Dictionary& parameters,
		bool interruptible) override
	{
		if (callbacks.instance)
		{
			if (RmlEvent* event = callbacks.instance(user_data, Rml::CApi::ToC(target), static_cast<uint16_t>(id), Rml::CApi::ToStringView(type),
					reinterpret_cast<const RmlDictionary*>(&parameters), interruptible))
				return Rml::EventPtr(Rml::CApi::FromC(event));
		}
		return Rml::EventPtr(new Rml::Event(target, id, type, parameters, interruptible));
	}

	void ReleaseEvent(Rml::Event* event) override
	{
		if (!event)
			return;
		if (callbacks.release)
			callbacks.release(user_data, Rml::CApi::ToC(event));
		delete event;
	}

	void Release() override {}

private:
	RmlEventInstancerCallbacks callbacks;
	void* user_data;
};

class EventListenerInstancerAdapter final : public Rml::EventListenerInstancer {
public:
	EventListenerInstancerAdapter(RmlEventListenerInstancerFunc instance, void* user_data) : instance(instance), user_data(user_data) {}

	Rml::EventListener* InstanceEventListener(const Rml::String& value, Rml::Element* element) override
	{
		if (!instance)
			return nullptr;
		return Rml::CApi::FromC(instance(user_data, Rml::CApi::ToStringView(value), Rml::CApi::ToC(element)));
	}

private:
	RmlEventListenerInstancerFunc instance;
	void* user_data;
};

RmlTextShapingContext ToShapingContext(const Rml::TextShapingContext& context)
{
	RmlTextShapingContext result = {};
	result.language = Rml::CApi::ToStringView(context.language);
	result.text_direction = static_cast<RmlTextDirection>(static_cast<int>(context.text_direction));
	result.font_kerning = static_cast<RmlFontKerning>(static_cast<int>(context.font_kerning));
	result.letter_spacing = context.letter_spacing;
	return result;
}

class FontEngineAdapter final : public Rml::FontEngineInterface {
public:
	FontEngineAdapter(const RmlFontEngineInterfaceCallbacks& callbacks, void* user_data) : callbacks(callbacks), user_data(user_data) {}

	void Initialize() override
	{
		if (callbacks.initialize)
			callbacks.initialize(user_data);
		else
			FontEngineInterface::Initialize();
	}
	void Shutdown() override
	{
		if (callbacks.shutdown)
			callbacks.shutdown(user_data);
		else
			FontEngineInterface::Shutdown();
	}
	bool LoadFontFace(const Rml::String& file_name, int face_index, bool fallback_face, Rml::Style::FontWeight weight) override
	{
		if (!callbacks.load_font_face)
			return FontEngineInterface::LoadFontFace(file_name, face_index, fallback_face, weight);
		return callbacks.load_font_face(user_data, Rml::CApi::ToStringView(file_name), face_index, fallback_face, static_cast<RmlFontWeight>(weight));
	}
	bool LoadFontFace(const Rml::String& file_name, int face_index, const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight,
		bool fallback_face) override
	{
		if (!callbacks.load_font_face_with_family)
			return FontEngineInterface::LoadFontFace(file_name, face_index, family, style, weight, fallback_face);
		return callbacks.load_font_face_with_family(user_data, Rml::CApi::ToStringView(file_name), face_index, Rml::CApi::ToStringView(family),
			static_cast<RmlFontStyle>(style), static_cast<RmlFontWeight>(weight), fallback_face);
	}
	bool LoadFontFace(Rml::Span<const Rml::byte> data, int face_index, const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight,
		bool fallback_face) override
	{
		if (!callbacks.load_font_face_from_memory)
			return FontEngineInterface::LoadFontFace(data, face_index, family, style, weight, fallback_face);
		return callbacks.load_font_face_from_memory(user_data, data.data(), data.size(), face_index, Rml::CApi::ToStringView(family),
			static_cast<RmlFontStyle>(style), static_cast<RmlFontWeight>(weight), fallback_face);
	}
	Rml::FontFaceHandle GetFontFaceHandle(const Rml::String& family, Rml::Style::FontStyle style, Rml::Style::FontWeight weight, int size) override
	{
		if (!callbacks.get_font_face_handle)
			return FontEngineInterface::GetFontFaceHandle(family, style, weight, size);
		return callbacks.get_font_face_handle(user_data, Rml::CApi::ToStringView(family), static_cast<RmlFontStyle>(style),
			static_cast<RmlFontWeight>(weight), size);
	}
	const Rml::FontMetrics& GetFontMetrics(Rml::FontFaceHandle handle) override
	{
		if (!callbacks.get_font_metrics)
			return FontEngineInterface::GetFontMetrics(handle);

		RmlFontMetrics metrics = {};
		if (!callbacks.get_font_metrics(user_data, handle, &metrics))
			return FontEngineInterface::GetFontMetrics(handle);

		Rml::FontMetrics& stored = metrics_by_handle[handle];
		stored.size = metrics.size;
		stored.ascent = metrics.ascent;
		stored.descent = metrics.descent;
		stored.line_spacing = metrics.line_spacing;
		stored.x_height = metrics.x_height;
		stored.underline_position = metrics.underline_position;
		stored.underline_thickness = metrics.underline_thickness;
		stored.has_ellipsis = metrics.has_ellipsis;
		return stored;
	}
	int GetStringWidth(Rml::FontFaceHandle handle, Rml::StringView string, const Rml::TextShapingContext& text_shaping_context,
		Rml::Character prior_character) override
	{
		if (!callbacks.get_string_width)
			return FontEngineInterface::GetStringWidth(handle, string, text_shaping_context, prior_character);
		const RmlTextShapingContext shaping = ToShapingContext(text_shaping_context);
		return callbacks.get_string_width(user_data, handle, RmlStringView{string.begin(), string.size()}, &shaping,
			static_cast<uint32_t>(prior_character));
	}
	Rml::FontEffectsHandle PrepareFontEffects(Rml::FontFaceHandle handle, const Rml::FontEffectList& font_effects) override
	{
		if (!callbacks.prepare_font_effects)
			return FontEngineInterface::PrepareFontEffects(handle, font_effects);
		Rml::Vector<const RmlFontEffect*> effects;
		effects.reserve(font_effects.size());
		for (const Rml::SharedPtr<const Rml::FontEffect>& effect : font_effects)
			effects.push_back(reinterpret_cast<const RmlFontEffect*>(effect.get()));
		return callbacks.prepare_font_effects(user_data, handle, effects.empty() ? nullptr : effects.data(), effects.size());
	}
	int GenerateString(Rml::RenderManager& render_manager, Rml::FontFaceHandle face_handle, Rml::FontEffectsHandle font_effects_handle,
		Rml::StringView string, Rml::Vector2f position, Rml::ColourbPremultiplied colour, float opacity,
		const Rml::TextShapingContext& text_shaping_context, Rml::TexturedMeshList& mesh_list) override
	{
		if (!callbacks.generate_string)
			return FontEngineInterface::GenerateString(render_manager, face_handle, font_effects_handle, string, position, colour, opacity,
				text_shaping_context, mesh_list);
		const RmlTextShapingContext shaping = ToShapingContext(text_shaping_context);
		TexturedMeshListView meshes{&mesh_list, &render_manager};
		return callbacks.generate_string(user_data, face_handle, font_effects_handle, RmlStringView{string.begin(), string.size()},
			Rml::CApi::ToC(position), Rml::CApi::ToC(colour), opacity, &shaping, reinterpret_cast<RmlTexturedMeshList*>(&meshes));
	}
	int GetVersion(Rml::FontFaceHandle handle) override
	{
		if (!callbacks.get_version)
			return FontEngineInterface::GetVersion(handle);
		return callbacks.get_version(user_data, handle);
	}
	void ReleaseFontResources() override
	{
		metrics_by_handle.clear();
		if (callbacks.release_font_resources)
			callbacks.release_font_resources(user_data);
		else
			FontEngineInterface::ReleaseFontResources();
	}

private:
	RmlFontEngineInterfaceCallbacks callbacks;
	void* user_data;
	std::map<Rml::FontFaceHandle, Rml::FontMetrics> metrics_by_handle;
};

} // namespace

RmlPlugin* rml_plugin_create(const RmlPluginCallbacks* callbacks, void* user_data)
{
	if (!callbacks)
		return nullptr;
	return reinterpret_cast<RmlPlugin*>(new PluginAdapter(*callbacks, user_data));
}

void rml_plugin_destroy(RmlPlugin* plugin)
{
	if (!plugin)
		return;
	auto* adapter = reinterpret_cast<PluginAdapter*>(plugin);
	if (adapter->IsRegistered())
	{
		Rml::CApi::NotePluginRegistered(plugin, false);
		Rml::UnregisterPlugin(adapter);
	}
	delete adapter;
}

namespace Rml {
namespace CApi {
void NotePluginRegistered(RmlPlugin* plugin, bool registered)
{
	if (!plugin)
		return;
	auto* adapter = reinterpret_cast<PluginAdapter*>(plugin);
	adapter->SetRegistered(registered);
	Rml::Vector<PluginAdapter*>& plugins = RegisteredPlugins();
	plugins.erase(std::remove(plugins.begin(), plugins.end(), adapter), plugins.end());
	if (registered)
		plugins.push_back(adapter);
}

void NotePluginsShutdown()
{
	for (PluginAdapter* adapter : RegisteredPlugins())
		adapter->SetRegistered(false);
	RegisteredPlugins().clear();
}
} // namespace CApi
} // namespace Rml

RmlElement* rml_element_create_custom(RmlStringView tag, const RmlElementCallbacks* callbacks, void* user_data)
{
	RmlElementCallbacks empty = {};
	const RmlElementCallbacks* used = callbacks ? callbacks : &empty;
	return Rml::CApi::ToC(new CustomElement(Rml::CApi::ToString(tag), *used, user_data));
}

RmlElementInstancer* rml_element_instancer_create(const RmlElementInstancerCallbacks* callbacks, void* user_data)
{
	if (!callbacks || !callbacks->instance || !callbacks->release)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Element instancer requires instance and release callbacks.");
		return nullptr;
	}
	return reinterpret_cast<RmlElementInstancer*>(new ElementInstancerAdapter(*callbacks, user_data));
}

void rml_element_instancer_destroy(RmlElementInstancer* instancer)
{
	delete reinterpret_cast<ElementInstancerAdapter*>(instancer);
}

void rml_register_element_instancer(RmlStringView tag, RmlElementInstancer* instancer)
{
	Rml::Factory::RegisterElementInstancer(Rml::CApi::ToString(tag), reinterpret_cast<Rml::ElementInstancer*>(instancer));
}

RmlDecorator* rml_decorator_create(const RmlDecoratorCallbacks* callbacks, void* user_data)
{
	if (!callbacks || !callbacks->generate_element_data || !callbacks->release_element_data || !callbacks->render_element)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Decorator requires generate, release, and render callbacks.");
		return nullptr;
	}
	return reinterpret_cast<RmlDecorator*>(new DecoratorAdapter(*callbacks, user_data));
}

void rml_decorator_destroy(RmlDecorator* decorator)
{
	delete reinterpret_cast<DecoratorAdapter*>(decorator);
}

RmlDecoratorInstancer* rml_decorator_instancer_create(RmlDecoratorInstanceFunc instance, void* user_data)
{
	if (!instance)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Decorator instancer requires an instance callback.");
		return nullptr;
	}
	return reinterpret_cast<RmlDecoratorInstancer*>(new DecoratorInstancerAdapter(instance, nullptr, user_data));
}

RmlDecoratorInstancer* rml_decorator_instancer_create_with_interface(RmlDecoratorInstanceWithInterfaceFunc instance, void* user_data)
{
	if (!instance)
	{
		Rml::Log::Message(Rml::Log::LT_ERROR, "Decorator instancer requires an instance callback.");
		return nullptr;
	}
	return reinterpret_cast<RmlDecoratorInstancer*>(new DecoratorInstancerAdapter(nullptr, instance, user_data));
}

RmlTexture rml_decorator_instancer_interface_get_texture(const RmlDecoratorInstancerInterface* instancer_interface, RmlStringView filename)
{
	RMLUI_CAPI_REQUIRE(instancer_interface, {});
	const auto& cpp_interface = *reinterpret_cast<const Rml::DecoratorInstancerInterface*>(instancer_interface);
	return Rml::CApi::ToC(cpp_interface.GetTexture(Rml::CApi::ToString(filename)));
}

RmlTexture rml_decorator_instancer_interface_get_sprite(const RmlDecoratorInstancerInterface* instancer_interface, RmlStringView name,
	RmlRectanglef* out_rectangle, float* out_display_scale)
{
	RMLUI_CAPI_REQUIRE(instancer_interface, {});
	const auto& cpp_interface = *reinterpret_cast<const Rml::DecoratorInstancerInterface*>(instancer_interface);
	const Rml::Sprite* sprite = cpp_interface.GetSprite(Rml::CApi::ToString(name));
	if (!sprite || !sprite->sprite_sheet)
		return {};
	if (out_rectangle)
		*out_rectangle = Rml::CApi::ToC(sprite->rectangle);
	if (out_display_scale)
		*out_display_scale = sprite->sprite_sheet->display_scale;
	return Rml::CApi::ToC(sprite->sprite_sheet->texture_source.GetTexture(cpp_interface.GetRenderManager()));
}

void rml_decorator_instancer_destroy(RmlDecoratorInstancer* instancer)
{
	delete reinterpret_cast<DecoratorInstancerAdapter*>(instancer);
}

void rml_register_decorator_instancer(RmlStringView name, RmlDecoratorInstancer* instancer)
{
	Rml::Factory::RegisterDecoratorInstancer(Rml::CApi::ToString(name), reinterpret_cast<Rml::DecoratorInstancer*>(instancer));
}

RmlPropertyDefinition* rml_decorator_instancer_register_property(RmlDecoratorInstancer* instancer, RmlStringView name, RmlStringView default_value)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<DecoratorInstancerAdapter*>(instancer);
	return reinterpret_cast<RmlPropertyDefinition*>(adapter->Register(Rml::CApi::ToString(name), Rml::CApi::ToString(default_value)));
}

uint8_t rml_decorator_instancer_register_shorthand(RmlDecoratorInstancer* instancer, RmlStringView shorthand_name, RmlStringView property_names,
	RmlShorthandType type)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<DecoratorInstancerAdapter*>(instancer);
	return static_cast<uint8_t>(adapter->RegisterShorthandPublic(Rml::CApi::ToString(shorthand_name), Rml::CApi::ToString(property_names),
		static_cast<Rml::ShorthandType>(type)));
}

RmlFilter* rml_filter_create(const RmlFilterCallbacks* callbacks, void* user_data)
{
	RmlFilterCallbacks empty = {};
	return reinterpret_cast<RmlFilter*>(new FilterAdapter(callbacks ? *callbacks : empty, user_data));
}

void rml_filter_destroy(RmlFilter* filter)
{
	delete reinterpret_cast<FilterAdapter*>(filter);
}

RmlPropertyDefinition* rml_filter_instancer_register_property(RmlFilterInstancer* instancer, RmlStringView name, RmlStringView default_value)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<FilterInstancerAdapter*>(instancer);
	return reinterpret_cast<RmlPropertyDefinition*>(adapter->Register(Rml::CApi::ToString(name), Rml::CApi::ToString(default_value)));
}

uint8_t rml_filter_instancer_register_shorthand(RmlFilterInstancer* instancer, RmlStringView shorthand_name, RmlStringView property_names,
	RmlShorthandType type)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<FilterInstancerAdapter*>(instancer);
	return static_cast<uint8_t>(adapter->RegisterShorthandPublic(Rml::CApi::ToString(shorthand_name), Rml::CApi::ToString(property_names),
		static_cast<Rml::ShorthandType>(type)));
}

RmlFilterInstancer* rml_filter_instancer_create(RmlFilterInstanceFunc instance, void* user_data)
{
	if (!instance)
		return nullptr;
	return reinterpret_cast<RmlFilterInstancer*>(new FilterInstancerAdapter(instance, user_data));
}

void rml_filter_instancer_destroy(RmlFilterInstancer* instancer)
{
	delete reinterpret_cast<FilterInstancerAdapter*>(instancer);
}

void rml_register_filter_instancer(RmlStringView name, RmlFilterInstancer* instancer)
{
	Rml::Factory::RegisterFilterInstancer(Rml::CApi::ToString(name), reinterpret_cast<Rml::FilterInstancer*>(instancer));
}

RmlFontEffect* rml_font_effect_create(void)
{
	return reinterpret_cast<RmlFontEffect*>(new Rml::FontEffect());
}

RmlFontEffect* rml_font_effect_create_custom(const RmlFontEffectCallbacks* callbacks, void* user_data)
{
	RmlFontEffectCallbacks empty = {};
	return reinterpret_cast<RmlFontEffect*>(static_cast<Rml::FontEffect*>(new FontEffectAdapter(callbacks ? *callbacks : empty, user_data)));
}

void rml_font_effect_destroy(RmlFontEffect* effect)
{
	delete reinterpret_cast<Rml::FontEffect*>(effect);
}

void rml_font_effect_set_layer(RmlFontEffect* effect, RmlFontEffectLayer layer)
{
	RMLUI_CAPI_REQUIRE_VOID(effect);
	reinterpret_cast<Rml::FontEffect*>(effect)->SetLayer(layer == RML_FONT_EFFECT_LAYER_FRONT ? Rml::FontEffect::Layer::Front
																							   : Rml::FontEffect::Layer::Back);
}

RmlFontEffectLayer rml_font_effect_get_layer(const RmlFontEffect* effect)
{
	RMLUI_CAPI_REQUIRE(effect, RML_FONT_EFFECT_LAYER_BACK);
	return reinterpret_cast<const Rml::FontEffect*>(effect)->GetLayer() == Rml::FontEffect::Layer::Front ? RML_FONT_EFFECT_LAYER_FRONT
																										 : RML_FONT_EFFECT_LAYER_BACK;
}

void rml_font_effect_set_colour(RmlFontEffect* effect, RmlColourb colour)
{
	RMLUI_CAPI_REQUIRE_VOID(effect);
	reinterpret_cast<Rml::FontEffect*>(effect)->SetColour(Rml::CApi::FromC(colour));
}

RmlColourb rml_font_effect_get_colour(const RmlFontEffect* effect)
{
	RMLUI_CAPI_REQUIRE(effect, {});
	return Rml::CApi::ToC(reinterpret_cast<const Rml::FontEffect*>(effect)->GetColour());
}

void rml_font_effect_set_fingerprint(RmlFontEffect* effect, size_t fingerprint)
{
	RMLUI_CAPI_REQUIRE_VOID(effect);
	reinterpret_cast<Rml::FontEffect*>(effect)->SetFingerprint(fingerprint);
}

size_t rml_font_effect_get_fingerprint(const RmlFontEffect* effect)
{
	RMLUI_CAPI_REQUIRE(effect, {});
	return reinterpret_cast<const Rml::FontEffect*>(effect)->GetFingerprint();
}

bool rml_font_effect_has_unique_texture(const RmlFontEffect* effect)
{
	RMLUI_CAPI_REQUIRE(effect, {});
	return reinterpret_cast<const Rml::FontEffect*>(effect)->HasUniqueTexture();
}

bool rml_font_effect_get_glyph_metrics(const RmlFontEffect* effect, RmlVector2i* origin, RmlVector2i* dimensions, const RmlFontGlyph* glyph)
{
	RMLUI_CAPI_REQUIRE(effect && origin && dimensions && glyph, {});
	Rml::Vector2i cpp_origin = Rml::CApi::FromC(*origin);
	Rml::Vector2i cpp_dimensions = Rml::CApi::FromC(*dimensions);
	if (!reinterpret_cast<const Rml::FontEffect*>(effect)->GetGlyphMetrics(cpp_origin, cpp_dimensions, FromC(*glyph)))
		return false;
	*origin = Rml::CApi::ToC(cpp_origin);
	*dimensions = Rml::CApi::ToC(cpp_dimensions);
	return true;
}

void rml_font_effect_generate_glyph_texture(const RmlFontEffect* effect, unsigned char* destination, RmlVector2i destination_dimensions,
	int destination_stride, const RmlFontGlyph* glyph)
{
	RMLUI_CAPI_REQUIRE_VOID(effect && destination && glyph);
	reinterpret_cast<const Rml::FontEffect*>(effect)->GenerateGlyphTexture(destination, Rml::CApi::FromC(destination_dimensions), destination_stride,
		FromC(*glyph));
}

RmlPropertyDefinition* rml_font_effect_instancer_register_property(RmlFontEffectInstancer* instancer, RmlStringView name,
	RmlStringView default_value, bool affects_generation)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<FontEffectInstancerAdapter*>(instancer);
	return reinterpret_cast<RmlPropertyDefinition*>(
		adapter->Register(Rml::CApi::ToString(name), Rml::CApi::ToString(default_value), affects_generation));
}

uint8_t rml_font_effect_instancer_register_shorthand(RmlFontEffectInstancer* instancer, RmlStringView shorthand_name, RmlStringView property_names,
	RmlShorthandType type)
{
	RMLUI_CAPI_REQUIRE(instancer, {});
	auto* adapter = reinterpret_cast<FontEffectInstancerAdapter*>(instancer);
	return static_cast<uint8_t>(adapter->RegisterShorthandPublic(Rml::CApi::ToString(shorthand_name), Rml::CApi::ToString(property_names),
		static_cast<Rml::ShorthandType>(type)));
}

RmlFontEffectInstancer* rml_font_effect_instancer_create(RmlFontEffectInstanceFunc instance, void* user_data)
{
	if (!instance)
		return nullptr;
	return reinterpret_cast<RmlFontEffectInstancer*>(new FontEffectInstancerAdapter(instance, user_data));
}

void rml_font_effect_instancer_destroy(RmlFontEffectInstancer* instancer)
{
	delete reinterpret_cast<FontEffectInstancerAdapter*>(instancer);
}

void rml_register_font_effect_instancer(RmlStringView name, RmlFontEffectInstancer* instancer)
{
	Rml::Factory::RegisterFontEffectInstancer(Rml::CApi::ToString(name), reinterpret_cast<Rml::FontEffectInstancer*>(instancer));
}

RmlEventInstancer* rml_event_instancer_create(const RmlEventInstancerCallbacks* callbacks, void* user_data)
{
	RmlEventInstancerCallbacks empty = {};
	return reinterpret_cast<RmlEventInstancer*>(new EventInstancerAdapter(callbacks ? *callbacks : empty, user_data));
}

void rml_event_instancer_destroy(RmlEventInstancer* instancer)
{
	delete reinterpret_cast<EventInstancerAdapter*>(instancer);
}

void rml_register_event_instancer(RmlEventInstancer* instancer)
{
	Rml::Factory::RegisterEventInstancer(reinterpret_cast<Rml::EventInstancer*>(instancer));
}

RmlEventListenerInstancer* rml_event_listener_instancer_create(RmlEventListenerInstancerFunc instance, void* user_data)
{
	if (!instance)
		return nullptr;
	return reinterpret_cast<RmlEventListenerInstancer*>(new EventListenerInstancerAdapter(instance, user_data));
}

void rml_event_listener_instancer_destroy(RmlEventListenerInstancer* instancer)
{
	delete reinterpret_cast<EventListenerInstancerAdapter*>(instancer);
}

void rml_register_event_listener_instancer(RmlEventListenerInstancer* instancer)
{
	Rml::Factory::RegisterEventListenerInstancer(reinterpret_cast<Rml::EventListenerInstancer*>(instancer));
}

RmlFontEngineInterface* rml_font_engine_interface_create(const RmlFontEngineInterfaceCallbacks* callbacks, void* user_data)
{
	if (!callbacks)
		return nullptr;
	return reinterpret_cast<RmlFontEngineInterface*>(new FontEngineAdapter(*callbacks, user_data));
}

void rml_font_engine_interface_destroy(RmlFontEngineInterface* iface)
{
	delete reinterpret_cast<FontEngineAdapter*>(iface);
}

void rml_textured_mesh_list_push(RmlTexturedMeshList* list, const RmlVertex* vertices, size_t num_vertices, const int* indices,
	size_t num_indices)
{
	rml_textured_mesh_list_push_textured(list, vertices, num_vertices, indices, num_indices, nullptr);
}

void rml_textured_mesh_list_push_textured(RmlTexturedMeshList* list, const RmlVertex* vertices, size_t num_vertices, const int* indices,
	size_t num_indices, const RmlTextureSource* texture)
{
	RMLUI_CAPI_REQUIRE_VOID(list);
	const TexturedMeshListView& view = *reinterpret_cast<const TexturedMeshListView*>(list);

	Rml::TexturedMesh textured;
	textured.mesh = Rml::CApi::MakeMesh(vertices, num_vertices, indices, num_indices);
	if (texture)
		textured.texture = Rml::CApi::GetTexture(texture, *view.render_manager);
	view.list->push_back(std::move(textured));
}
