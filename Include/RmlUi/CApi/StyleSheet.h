#ifndef RMLUI_CAPI_STYLE_SHEET_H
#define RMLUI_CAPI_STYLE_SHEET_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API RmlStyleSheetContainer* rml_style_sheet_container_create_from_string(RmlStringView sheet);
RMLUI_CAPI_API RmlStyleSheetContainer* rml_style_sheet_container_create_from_file(RmlStringView path);
RMLUI_CAPI_API void rml_style_sheet_container_destroy(RmlStyleSheetContainer* container);

RMLUI_CAPI_API void rml_document_set_style_sheet_container(RmlElementDocument* document, RmlStyleSheetContainer* container);
RMLUI_CAPI_API void rml_clear_style_sheet_cache(void);
RMLUI_CAPI_API void rml_clear_template_cache(void);

RMLUI_CAPI_API RmlPropertyDefinition* rml_style_sheet_register_property(RmlStringView name, RmlStringView default_value, bool inherited,
	bool forces_layout);
RMLUI_CAPI_API void rml_property_definition_add_parser(RmlPropertyDefinition* definition, RmlStringView parser_name, RmlStringView parser_parameters);
RMLUI_CAPI_API uint8_t rml_style_sheet_register_shorthand(RmlStringView shorthand_name, RmlStringView property_names, RmlShorthandType type);

RMLUI_CAPI_API RmlPropertyDictionary* rml_property_dictionary_create(void);
RMLUI_CAPI_API void rml_property_dictionary_destroy(RmlPropertyDictionary* dictionary);
RMLUI_CAPI_API bool rml_style_sheet_parse_property_declaration(RmlPropertyDictionary* dictionary, RmlStringView property_name,
	RmlStringView property_value);
RMLUI_CAPI_API RmlOwnedString rml_property_dictionary_get_string(const RmlPropertyDictionary* dictionary, RmlStringView property_name);

/* Returns 0 when the name is not a registered property. */
RMLUI_CAPI_API uint8_t rml_style_sheet_get_property_id(RmlStringView name);
RMLUI_CAPI_API bool rml_property_id_set_contains(const RmlPropertyIdSet* set, uint8_t property_id);
RMLUI_CAPI_API size_t rml_property_id_set_size(const RmlPropertyIdSet* set);

#ifdef __cplusplus
}
#endif

#endif
