#ifndef RMLUI_CAPI_ELEMENT_DOCUMENT_H
#define RMLUI_CAPI_ELEMENT_DOCUMENT_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API RmlElement* rml_document_as_element(RmlElementDocument* document);
RMLUI_CAPI_API RmlElementDocument* rml_element_as_document(RmlElement* element);

RMLUI_CAPI_API void rml_document_show(RmlElementDocument* document, RmlModalFlag modal, RmlFocusFlag focus, RmlScrollFlag scroll);
RMLUI_CAPI_API void rml_document_hide(RmlElementDocument* document);
RMLUI_CAPI_API void rml_document_close(RmlElementDocument* document);

RMLUI_CAPI_API void rml_document_set_title(RmlElementDocument* document, RmlStringView title);
RMLUI_CAPI_API RmlStringView rml_document_get_title(const RmlElementDocument* document);
RMLUI_CAPI_API RmlStringView rml_document_get_source_url(const RmlElementDocument* document);

RMLUI_CAPI_API RmlContext* rml_document_get_context(RmlElementDocument* document);

RMLUI_CAPI_API RmlElement* rml_document_create_element(RmlElementDocument* document, RmlStringView name);
RMLUI_CAPI_API RmlElement* rml_document_create_text_node(RmlElementDocument* document, RmlStringView text);

RMLUI_CAPI_API void rml_document_pull_to_front(RmlElementDocument* document);
RMLUI_CAPI_API void rml_document_push_to_back(RmlElementDocument* document);

RMLUI_CAPI_API bool rml_document_is_modal(const RmlElementDocument* document);

RMLUI_CAPI_API void rml_document_update_document(RmlElementDocument* document);
RMLUI_CAPI_API void rml_document_reload_style_sheet(RmlElementDocument* document);

RMLUI_CAPI_API RmlElement* rml_document_find_next_tab_element(RmlElementDocument* document, RmlElement* current_element, bool forward,
	bool wrap_around);

#ifdef __cplusplus
}
#endif

#endif