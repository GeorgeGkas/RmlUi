#pragma once

#include <RmlUi/CApi/Types.h>
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Element.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/ElementText.h>
#include <RmlUi/Core/ElementUtilities.h>
#include <RmlUi/Core/Elements/ElementForm.h>
#include <RmlUi/Core/Elements/ElementFormControl.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/Elements/ElementFormControlSelect.h>
#include <RmlUi/Core/Elements/ElementFormControlTextArea.h>
#include <RmlUi/Core/Elements/ElementProgress.h>
#include <RmlUi/Core/Elements/ElementTabSet.h>
#include <RmlUi/Core/EventListener.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Plugin.h>
#include <RmlUi/Core/RenderInterface.h>
#include <RmlUi/Core/SystemInterface.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/FontEngineInterface.h>
#include <RmlUi/Core/TextInputContext.h>
#include <RmlUi/Core/TextInputHandler.h>
#include <RmlUi/Core/Types.h>
#include <RmlUi/Core/Variant.h>
#include <RmlUi/Core/Vertex.h>
#include <cstring>
namespace Rml {
namespace CApi {

Rml::String ToString(RmlStringView view);
RmlStringView ToStringView(const Rml::String& string);
RmlOwnedString ToOwnedString(const Rml::String& string);

inline Context* FromC(RmlContext* context)
{
	return reinterpret_cast<Context*>(context);
}

inline const Context* FromC(const RmlContext* context)
{
	return reinterpret_cast<const Context*>(context);
}

inline RmlContext* ToC(Context* context)
{
	return reinterpret_cast<RmlContext*>(context);
}

inline Element* FromC(RmlElement* element)
{
	return reinterpret_cast<Element*>(element);
}

inline const Element* FromC(const RmlElement* element)
{
	return reinterpret_cast<const Element*>(element);
}

inline RmlElement* ToC(Element* element)
{
	return reinterpret_cast<RmlElement*>(element);
}

inline ElementDocument* FromC(RmlElementDocument* document)
{
	return reinterpret_cast<ElementDocument*>(document);
}

inline const ElementDocument* FromC(const RmlElementDocument* document)
{
	return reinterpret_cast<const ElementDocument*>(document);
}

inline RmlElementDocument* ToC(ElementDocument* document)
{
	return reinterpret_cast<RmlElementDocument*>(document);
}

inline ElementText* FromC(RmlElementText* text)
{
	return reinterpret_cast<ElementText*>(text);
}

inline const ElementText* FromC(const RmlElementText* text)
{
	return reinterpret_cast<const ElementText*>(text);
}

inline RmlElementText* ToC(ElementText* text)
{
	return reinterpret_cast<RmlElementText*>(text);
}

inline ElementForm* FromC(RmlElementForm* form)
{
	return reinterpret_cast<ElementForm*>(form);
}

inline const ElementForm* FromC(const RmlElementForm* form)
{
	return reinterpret_cast<const ElementForm*>(form);
}

inline RmlElementForm* ToC(ElementForm* form)
{
	return reinterpret_cast<RmlElementForm*>(form);
}

inline ElementFormControl* FromC(RmlElementFormControl* control)
{
	return reinterpret_cast<ElementFormControl*>(control);
}

inline const ElementFormControl* FromC(const RmlElementFormControl* control)
{
	return reinterpret_cast<const ElementFormControl*>(control);
}

inline RmlElementFormControl* ToC(ElementFormControl* control)
{
	return reinterpret_cast<RmlElementFormControl*>(control);
}

inline ElementFormControlInput* FromC(RmlElementFormControlInput* input)
{
	return reinterpret_cast<ElementFormControlInput*>(input);
}

inline const ElementFormControlInput* FromC(const RmlElementFormControlInput* input)
{
	return reinterpret_cast<const ElementFormControlInput*>(input);
}

inline RmlElementFormControlInput* ToC(ElementFormControlInput* input)
{
	return reinterpret_cast<RmlElementFormControlInput*>(input);
}

inline ElementFormControlSelect* FromC(RmlElementFormControlSelect* select)
{
	return reinterpret_cast<ElementFormControlSelect*>(select);
}

inline const ElementFormControlSelect* FromC(const RmlElementFormControlSelect* select)
{
	return reinterpret_cast<const ElementFormControlSelect*>(select);
}

inline RmlElementFormControlSelect* ToC(ElementFormControlSelect* select)
{
	return reinterpret_cast<RmlElementFormControlSelect*>(select);
}

inline ElementFormControlTextArea* FromC(RmlElementFormControlTextArea* text_area)
{
	return reinterpret_cast<ElementFormControlTextArea*>(text_area);
}

inline const ElementFormControlTextArea* FromC(const RmlElementFormControlTextArea* text_area)
{
	return reinterpret_cast<const ElementFormControlTextArea*>(text_area);
}

inline RmlElementFormControlTextArea* ToC(ElementFormControlTextArea* text_area)
{
	return reinterpret_cast<RmlElementFormControlTextArea*>(text_area);
}

inline ElementProgress* FromC(RmlElementProgress* progress)
{
	return reinterpret_cast<ElementProgress*>(progress);
}

inline const ElementProgress* FromC(const RmlElementProgress* progress)
{
	return reinterpret_cast<const ElementProgress*>(progress);
}

inline RmlElementProgress* ToC(ElementProgress* progress)
{
	return reinterpret_cast<RmlElementProgress*>(progress);
}

inline ElementTabSet* FromC(RmlElementTabSet* tab_set)
{
	return reinterpret_cast<ElementTabSet*>(tab_set);
}

inline const ElementTabSet* FromC(const RmlElementTabSet* tab_set)
{
	return reinterpret_cast<const ElementTabSet*>(tab_set);
}

inline RmlElementTabSet* ToC(ElementTabSet* tab_set)
{
	return reinterpret_cast<RmlElementTabSet*>(tab_set);
}

inline SystemInterface* FromC(RmlSystemInterface* system_interface)
{
	return reinterpret_cast<SystemInterface*>(system_interface);
}

inline RmlSystemInterface* ToC(SystemInterface* system_interface)
{
	return reinterpret_cast<RmlSystemInterface*>(system_interface);
}

inline FileInterface* FromC(RmlFileInterface* file_interface)
{
	return reinterpret_cast<FileInterface*>(file_interface);
}

inline RmlFileInterface* ToC(FileInterface* file_interface)
{
	return reinterpret_cast<RmlFileInterface*>(file_interface);
}

inline RenderInterface* FromC(RmlRenderInterface* render_interface)
{
	return reinterpret_cast<RenderInterface*>(render_interface);
}

inline RmlRenderInterface* ToC(RenderInterface* render_interface)
{
	return reinterpret_cast<RmlRenderInterface*>(render_interface);
}

inline TextInputHandler* FromC(RmlTextInputHandler* text_input_handler)
{
	return reinterpret_cast<TextInputHandler*>(text_input_handler);
}

inline RmlTextInputHandler* ToC(TextInputHandler* text_input_handler)
{
	return reinterpret_cast<RmlTextInputHandler*>(text_input_handler);
}

inline FontEngineInterface* FromC(RmlFontEngineInterface* font_engine_interface)
{
	return reinterpret_cast<FontEngineInterface*>(font_engine_interface);
}

inline RmlFontEngineInterface* ToC(FontEngineInterface* font_engine_interface)
{
	return reinterpret_cast<RmlFontEngineInterface*>(font_engine_interface);
}

inline TextInputContext* FromC(RmlTextInputContext* text_input_context)
{
	return reinterpret_cast<TextInputContext*>(text_input_context);
}

inline RmlTextInputContext* ToC(TextInputContext* text_input_context)
{
	return reinterpret_cast<RmlTextInputContext*>(text_input_context);
}

inline DataModelConstructor FromC(RmlDataModelConstructor value)
{
	DataModelConstructor result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline RmlDataModelConstructor ToC(const DataModelConstructor& value)
{
	RmlDataModelConstructor result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline DataModelHandle FromC(RmlDataModelHandle value)
{
	DataModelHandle result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline RmlDataModelHandle ToC(const DataModelHandle& value)
{
	RmlDataModelHandle result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline EventListener* FromC(RmlEventListener* event_listener)
{
	return reinterpret_cast<EventListener*>(event_listener);
}

inline RmlEventListener* ToC(EventListener* event_listener)
{
	return reinterpret_cast<RmlEventListener*>(event_listener);
}

inline Plugin* FromC(RmlPlugin* plugin)
{
	return reinterpret_cast<Plugin*>(plugin);
}

inline RmlPlugin* ToC(Plugin* plugin)
{
	return reinterpret_cast<RmlPlugin*>(plugin);
}

inline const Vector2i& FromC(const RmlVector2i& value)
{
	return *reinterpret_cast<const Vector2i*>(&value);
}

inline RmlVector2i ToC(const Vector2i& value)
{
	RmlVector2i result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Vector2f& FromC(const RmlVector2f& value)
{
	return *reinterpret_cast<const Vector2f*>(&value);
}

inline RmlVector2f ToC(const Vector2f& value)
{
	RmlVector2f result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Vector3i& FromC(const RmlVector3i& value)
{
	return *reinterpret_cast<const Vector3i*>(&value);
}

inline RmlVector3i ToC(const Vector3i& value)
{
	RmlVector3i result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Vector3f& FromC(const RmlVector3f& value)
{
	return *reinterpret_cast<const Vector3f*>(&value);
}

inline RmlVector3f ToC(const Vector3f& value)
{
	RmlVector3f result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Vector4i& FromC(const RmlVector4i& value)
{
	return *reinterpret_cast<const Vector4i*>(&value);
}

inline RmlVector4i ToC(const Vector4i& value)
{
	RmlVector4i result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Vector4f& FromC(const RmlVector4f& value)
{
	return *reinterpret_cast<const Vector4f*>(&value);
}

inline RmlVector4f ToC(const Vector4f& value)
{
	RmlVector4f result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Colourb& FromC(const RmlColourb& value)
{
	return *reinterpret_cast<const Colourb*>(&value);
}

inline RmlColourb ToC(const Colourb& value)
{
	RmlColourb result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const ColourbPremultiplied& FromC(const RmlColourbPremultiplied& value)
{
	return *reinterpret_cast<const ColourbPremultiplied*>(&value);
}

inline RmlColourbPremultiplied ToC(const ColourbPremultiplied& value)
{
	RmlColourbPremultiplied result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Colourf& FromC(const RmlColourf& value)
{
	return *reinterpret_cast<const Colourf*>(&value);
}

inline RmlColourf ToC(const Colourf& value)
{
	RmlColourf result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Rectanglei& FromC(const RmlRectanglei& value)
{
	return *reinterpret_cast<const Rectanglei*>(&value);
}

inline RmlRectanglei ToC(const Rectanglei& value)
{
	RmlRectanglei result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Rectanglef& FromC(const RmlRectanglef& value)
{
	return *reinterpret_cast<const Rectanglef*>(&value);
}

inline RmlRectanglef ToC(const Rectanglef& value)
{
	RmlRectanglef result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Matrix4f& FromC(const RmlMatrix4f& value)
{
	return *reinterpret_cast<const Matrix4f*>(&value);
}

inline RmlMatrix4f ToC(const Matrix4f& value)
{
	RmlMatrix4f result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, value.data(), sizeof(result));
	return result;
}

inline const Vertex& FromC(const RmlVertex& value)
{
	return *reinterpret_cast<const Vertex*>(&value);
}

inline RmlVertex ToC(const Vertex& value)
{
	RmlVertex result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline const Touch& FromC(const RmlTouch& value)
{
	return *reinterpret_cast<const Touch*>(&value);
}

inline RmlTouch ToC(const Touch& value)
{
	RmlTouch result;
	static_assert(sizeof(result) == sizeof(value));
	std::memcpy(&result, &value, sizeof(result));
	return result;
}

inline Variant* FromC(RmlVariant* variant)
{
	return reinterpret_cast<Variant*>(variant);
}

inline const Variant* FromC(const RmlVariant* variant)
{
	return reinterpret_cast<const Variant*>(variant);
}

inline RmlVariant* ToC(Variant* variant)
{
	return reinterpret_cast<RmlVariant*>(variant);
}

inline const RmlVariant* ToC(const Variant* variant)
{
	return reinterpret_cast<const RmlVariant*>(variant);
}

inline Dictionary* FromC(RmlDictionary* dictionary)
{
	return reinterpret_cast<Dictionary*>(dictionary);
}

inline const Dictionary* FromC(const RmlDictionary* dictionary)
{
	return reinterpret_cast<const Dictionary*>(dictionary);
}

inline RmlDictionary* ToC(Dictionary* dictionary)
{
	return reinterpret_cast<RmlDictionary*>(dictionary);
}

inline const RmlDictionary* ToC(const Dictionary* dictionary)
{
	return reinterpret_cast<const RmlDictionary*>(dictionary);
}

inline VariantList* FromC(RmlVariantList* list)
{
	return reinterpret_cast<VariantList*>(list);
}

inline const VariantList* FromC(const RmlVariantList* list)
{
	return reinterpret_cast<const VariantList*>(list);
}

inline RmlVariantList* ToC(VariantList* list)
{
	return reinterpret_cast<RmlVariantList*>(list);
}

inline const RmlVariantList* ToC(const VariantList* list)
{
	return reinterpret_cast<const RmlVariantList*>(list);
}

inline ElementList* FromC(RmlElementList* list)
{
	return reinterpret_cast<ElementList*>(list);
}

inline const ElementList* FromC(const RmlElementList* list)
{
	return reinterpret_cast<const ElementList*>(list);
}

inline RmlElementList* ToC(ElementList* list)
{
	return reinterpret_cast<RmlElementList*>(list);
}

inline const RmlElementList* ToC(const ElementList* list)
{
	return reinterpret_cast<const RmlElementList*>(list);
}

inline StringList* FromC(RmlStringList* list)
{
	return reinterpret_cast<StringList*>(list);
}

inline const StringList* FromC(const RmlStringList* list)
{
	return reinterpret_cast<const StringList*>(list);
}

inline RmlStringList* ToC(StringList* list)
{
	return reinterpret_cast<RmlStringList*>(list);
}

inline const RmlStringList* ToC(const StringList* list)
{
	return reinterpret_cast<const RmlStringList*>(list);
}

inline Event* FromC(RmlEvent* event)
{
	return reinterpret_cast<Event*>(event);
}

inline const Event* FromC(const RmlEvent* event)
{
	return reinterpret_cast<const Event*>(event);
}

inline RmlEvent* ToC(Event* event)
{
	return reinterpret_cast<RmlEvent*>(event);
}

inline const RmlEvent* ToC(const Event* event)
{
	return reinterpret_cast<const RmlEvent*>(event);
}

} // namespace CApi
} // namespace Rml