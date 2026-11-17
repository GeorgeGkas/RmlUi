#include "Conversions.h"
#include <RmlUi/CApi/ElementDocument.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/ElementDocument.h>
#include <RmlUi/Core/Traits.h>
#include <RmlUi/Core/Types.h>

RmlElement* rml_document_as_element(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(document)));
}

RmlElementDocument* rml_element_as_document(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementDocument*>(Rml::CApi::FromC(element)));
}

void rml_document_show(RmlElementDocument* document, RmlModalFlag modal, RmlFocusFlag focus, RmlScrollFlag scroll)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->Show(static_cast<Rml::ModalFlag>(modal), static_cast<Rml::FocusFlag>(focus),
		static_cast<Rml::ScrollFlag>(scroll));
}

void rml_document_hide(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->Hide();
}

void rml_document_close(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->Close();
}

void rml_document_set_title(RmlElementDocument* document, RmlStringView title)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->SetTitle(Rml::CApi::ToString(title));
}

RmlStringView rml_document_get_title(const RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	return Rml::CApi::ToStringView(Rml::CApi::FromC(document)->GetTitle());
}

RmlStringView rml_document_get_source_url(const RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	return Rml::CApi::ToStringView(Rml::CApi::FromC(document)->GetSourceURL());
}

RmlContext* rml_document_get_context(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	return Rml::CApi::ToC(Rml::CApi::FromC(document)->GetContext());
}

RmlElement* rml_document_create_element(RmlElementDocument* document, RmlStringView name)
{
	RMLUI_ASSERT(document);
	Rml::ElementPtr element = Rml::CApi::FromC(document)->CreateElement(Rml::CApi::ToString(name));
	return Rml::CApi::ToC(element.release());
}

RmlElement* rml_document_create_text_node(RmlElementDocument* document, RmlStringView text)
{
	RMLUI_ASSERT(document);
	Rml::ElementPtr element = Rml::CApi::FromC(document)->CreateTextNode(Rml::CApi::ToString(text));
	return Rml::CApi::ToC(element.release());
}

void rml_document_pull_to_front(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->PullToFront();
}

void rml_document_push_to_back(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->PushToBack();
}

bool rml_document_is_modal(const RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	return Rml::CApi::FromC(document)->IsModal();
}

void rml_document_update_document(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->UpdateDocument();
}

void rml_document_reload_style_sheet(RmlElementDocument* document)
{
	RMLUI_ASSERT(document);
	Rml::CApi::FromC(document)->ReloadStyleSheet();
}

RmlElement* rml_document_find_next_tab_element(RmlElementDocument* document, RmlElement* current_element, bool forward, bool wrap_around)
{
	RMLUI_ASSERT(document);
	RMLUI_ASSERT(current_element);
	return Rml::CApi::ToC(
		Rml::CApi::FromC(document)->FindNextTabElement(Rml::CApi::FromC(current_element), forward, wrap_around));
}