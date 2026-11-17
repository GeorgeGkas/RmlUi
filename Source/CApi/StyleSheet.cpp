#include "Conversions.h"
#include <RmlUi/CApi/StyleSheet.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Factory.h>
#include <RmlUi/Core/Property.h>
#include <RmlUi/Core/PropertyDictionary.h>
#include <RmlUi/Core/PropertyDefinition.h>
#include <RmlUi/Core/StyleSheetContainer.h>
#include <RmlUi/Core/PropertyIdSet.h>
#include <RmlUi/Core/StyleSheetSpecification.h>

namespace {

struct StyleSheetBox {
	Rml::SharedPtr<Rml::StyleSheetContainer> container;
};

} // namespace

RmlStyleSheetContainer* rml_style_sheet_container_create_from_string(RmlStringView sheet)
{
	Rml::SharedPtr<Rml::StyleSheetContainer> container = Rml::Factory::InstanceStyleSheetString(Rml::CApi::ToString(sheet));
	if (!container)
		return nullptr;
	return reinterpret_cast<RmlStyleSheetContainer*>(new StyleSheetBox{std::move(container)});
}

RmlStyleSheetContainer* rml_style_sheet_container_create_from_file(RmlStringView path)
{
	Rml::SharedPtr<Rml::StyleSheetContainer> container = Rml::Factory::InstanceStyleSheetFile(Rml::CApi::ToString(path));
	if (!container)
		return nullptr;
	return reinterpret_cast<RmlStyleSheetContainer*>(new StyleSheetBox{std::move(container)});
}

void rml_style_sheet_container_destroy(RmlStyleSheetContainer* container)
{
	delete reinterpret_cast<StyleSheetBox*>(container);
}

void rml_document_set_style_sheet_container(RmlElementDocument* document, RmlStyleSheetContainer* container)
{
	RMLUI_ASSERT(document);
	RMLUI_ASSERT(container);
	Rml::CApi::FromC(document)->SetStyleSheetContainer(reinterpret_cast<StyleSheetBox*>(container)->container);
}

void rml_clear_style_sheet_cache(void)
{
	Rml::Factory::ClearStyleSheetCache();
}

void rml_clear_template_cache(void)
{
	Rml::Factory::ClearTemplateCache();
}

RmlPropertyDefinition* rml_style_sheet_register_property(RmlStringView name, RmlStringView default_value, bool inherited, bool forces_layout)
{
	Rml::PropertyDefinition& definition =
		Rml::StyleSheetSpecification::RegisterProperty(Rml::CApi::ToString(name), Rml::CApi::ToString(default_value), inherited, forces_layout);
	return reinterpret_cast<RmlPropertyDefinition*>(&definition);
}

void rml_property_definition_add_parser(RmlPropertyDefinition* definition, RmlStringView parser_name, RmlStringView parser_parameters)
{
	RMLUI_ASSERT(definition);
	reinterpret_cast<Rml::PropertyDefinition*>(definition)->AddParser(Rml::CApi::ToString(parser_name), Rml::CApi::ToString(parser_parameters));
}

uint8_t rml_style_sheet_register_shorthand(RmlStringView shorthand_name, RmlStringView property_names, RmlShorthandType type)
{
	return static_cast<uint8_t>(Rml::StyleSheetSpecification::RegisterShorthand(Rml::CApi::ToString(shorthand_name),
		Rml::CApi::ToString(property_names), static_cast<Rml::ShorthandType>(type)));
}

RmlPropertyDictionary* rml_property_dictionary_create(void)
{
	return reinterpret_cast<RmlPropertyDictionary*>(new Rml::PropertyDictionary());
}

void rml_property_dictionary_destroy(RmlPropertyDictionary* dictionary)
{
	delete reinterpret_cast<Rml::PropertyDictionary*>(dictionary);
}

bool rml_style_sheet_parse_property_declaration(RmlPropertyDictionary* dictionary, RmlStringView property_name, RmlStringView property_value)
{
	RMLUI_ASSERT(dictionary);
	return Rml::StyleSheetSpecification::ParsePropertyDeclaration(*reinterpret_cast<Rml::PropertyDictionary*>(dictionary),
		Rml::CApi::ToString(property_name), Rml::CApi::ToString(property_value));
}

uint8_t rml_style_sheet_get_property_id(RmlStringView name)
{
	return static_cast<uint8_t>(Rml::StyleSheetSpecification::GetPropertyId(Rml::CApi::ToString(name)));
}

bool rml_property_id_set_contains(const RmlPropertyIdSet* set, uint8_t property_id)
{
	if (!set)
		return false;
	return reinterpret_cast<const Rml::PropertyIdSet*>(set)->Contains(static_cast<Rml::PropertyId>(property_id));
}

size_t rml_property_id_set_size(const RmlPropertyIdSet* set)
{
	if (!set)
		return 0;
	return reinterpret_cast<const Rml::PropertyIdSet*>(set)->Size();
}

RmlOwnedString rml_property_dictionary_get_string(const RmlPropertyDictionary* dictionary, RmlStringView property_name)
{
	RMLUI_ASSERT(dictionary);
	const Rml::PropertyDictionary& properties = *reinterpret_cast<const Rml::PropertyDictionary*>(dictionary);
	const Rml::String name = Rml::CApi::ToString(property_name);
	const Rml::Property* property = properties.GetProperty(Rml::StyleSheetSpecification::GetPropertyId(name));
	if (!property)
		property = properties.GetCustomProperty(name);
	if (!property)
		return {};
	return Rml::CApi::ToOwnedString(property->ToString());
}
