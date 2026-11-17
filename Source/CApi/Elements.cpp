#include "Conversions.h"
#include <RmlUi/CApi/Elements.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/Elements/ElementForm.h>
#include <RmlUi/Core/Elements/ElementFormControl.h>
#include <RmlUi/Core/Elements/ElementFormControlInput.h>
#include <RmlUi/Core/Elements/ElementFormControlSelect.h>
#include <RmlUi/Core/Elements/ElementFormControlTextArea.h>
#include <RmlUi/Core/Elements/ElementProgress.h>
#include <RmlUi/Core/Elements/ElementTabSet.h>
#include <RmlUi/Core/Traits.h>
#include <RmlUi/Core/Types.h>

namespace {

void SetOwnedSelectionText(Rml::String selected_text, RmlOwnedString* out)
{
	if (!out)
		return;
	*out = Rml::CApi::ToOwnedString(selected_text);
}

} // namespace

RmlElement* rml_form_as_element(RmlElementForm* form)
{
	RMLUI_ASSERT(form);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(form)));
}

RmlElementForm* rml_element_as_form(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementForm*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_form_control_as_element(RmlElementFormControl* control)
{
	RMLUI_ASSERT(control);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(control)));
}

RmlElementFormControl* rml_element_as_form_control(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementFormControl*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_form_control_input_as_element(RmlElementFormControlInput* input)
{
	RMLUI_ASSERT(input);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(input)));
}

RmlElementFormControlInput* rml_element_as_form_control_input(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementFormControlInput*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_form_control_select_as_element(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(select)));
}

RmlElementFormControlSelect* rml_element_as_form_control_select(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementFormControlSelect*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_form_control_text_area_as_element(RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(text_area)));
}

RmlElementFormControlTextArea* rml_element_as_form_control_text_area(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementFormControlTextArea*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_progress_as_element(RmlElementProgress* progress)
{
	RMLUI_ASSERT(progress);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(progress)));
}

RmlElementProgress* rml_element_as_progress(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementProgress*>(Rml::CApi::FromC(element)));
}

RmlElement* rml_tab_set_as_element(RmlElementTabSet* tab_set)
{
	RMLUI_ASSERT(tab_set);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(tab_set)));
}

RmlElementTabSet* rml_element_as_tab_set(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementTabSet*>(Rml::CApi::FromC(element)));
}

void rml_form_submit(RmlElementForm* form, RmlStringView name, RmlStringView submit_value)
{
	RMLUI_ASSERT(form);
	Rml::CApi::FromC(form)->Submit(Rml::CApi::ToString(name), Rml::CApi::ToString(submit_value));
}

RmlOwnedString rml_form_control_get_name(const RmlElementFormControl* control)
{
	RMLUI_ASSERT(control);
	return Rml::CApi::ToOwnedString(Rml::CApi::FromC(control)->GetName());
}

void rml_form_control_set_name(RmlElementFormControl* control, RmlStringView name)
{
	RMLUI_ASSERT(control);
	Rml::CApi::FromC(control)->SetName(Rml::CApi::ToString(name));
}

RmlOwnedString rml_form_control_get_value(RmlElementFormControl* control)
{
	RMLUI_ASSERT(control);
	return Rml::CApi::ToOwnedString(Rml::CApi::FromC(control)->GetValue());
}

void rml_form_control_set_value(RmlElementFormControl* control, RmlStringView value)
{
	RMLUI_ASSERT(control);
	Rml::CApi::FromC(control)->SetValue(Rml::CApi::ToString(value));
}

bool rml_form_control_is_submitted(RmlElementFormControl* control)
{
	RMLUI_ASSERT(control);
	return Rml::CApi::FromC(control)->IsSubmitted();
}

bool rml_form_control_is_disabled(const RmlElementFormControl* control)
{
	RMLUI_ASSERT(control);
	return Rml::CApi::FromC(control)->IsDisabled();
}

void rml_form_control_set_disabled(RmlElementFormControl* control, bool disable)
{
	RMLUI_ASSERT(control);
	Rml::CApi::FromC(control)->SetDisabled(disable);
}

void rml_form_control_input_select(RmlElementFormControlInput* input)
{
	RMLUI_ASSERT(input);
	Rml::CApi::FromC(input)->Select();
}

void rml_form_control_input_set_selection_range(RmlElementFormControlInput* input, int selection_start, int selection_end)
{
	RMLUI_ASSERT(input);
	Rml::CApi::FromC(input)->SetSelectionRange(selection_start, selection_end);
}

void rml_form_control_input_get_selection(const RmlElementFormControlInput* input, int* selection_start, int* selection_end,
	RmlOwnedString* selected_text)
{
	RMLUI_ASSERT(input);
	int start = 0;
	int end = 0;
	Rml::String text;
	Rml::CApi::FromC(input)->GetSelection(&start, &end, &text);
	if (selection_start)
		*selection_start = start;
	if (selection_end)
		*selection_end = end;
	SetOwnedSelectionText(std::move(text), selected_text);
}

void rml_form_control_input_set_composition_range(RmlElementFormControlInput* input, int range_start, int range_end)
{
	RMLUI_ASSERT(input);
	Rml::CApi::FromC(input)->SetCompositionRange(range_start, range_end);
}

void rml_form_control_select_set_selection(RmlElementFormControlSelect* select, int selection)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->SetSelection(selection);
}

int rml_form_control_select_get_selection(const RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::FromC(select)->GetSelection();
}

RmlElement* rml_form_control_select_get_option(RmlElementFormControlSelect* select, int index)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::ToC(Rml::CApi::FromC(select)->GetOption(index));
}

int rml_form_control_select_get_num_options(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::FromC(select)->GetNumOptions();
}

int rml_form_control_select_add_rml(RmlElementFormControlSelect* select, RmlStringView rml, RmlStringView value, int before, bool selectable)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::FromC(select)->Add(Rml::CApi::ToString(rml), Rml::CApi::ToString(value), before, selectable);
}

int rml_form_control_select_add_element(RmlElementFormControlSelect* select, RmlElement* option_element, int before)
{
	RMLUI_ASSERT(select);
	RMLUI_ASSERT(option_element);
	if (Rml::CApi::FromC(option_element)->GetParentNode())
		return -1;
	Rml::ElementPtr option_ptr(Rml::CApi::FromC(option_element));
	return Rml::CApi::FromC(select)->Add(std::move(option_ptr), before);
}

void rml_form_control_select_remove(RmlElementFormControlSelect* select, int index)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->Remove(index);
}

void rml_form_control_select_remove_all(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->RemoveAll();
}

void rml_form_control_select_show_select_box(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->ShowSelectBox();
}

void rml_form_control_select_hide_select_box(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->HideSelectBox();
}

void rml_form_control_select_cancel_select_box(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	Rml::CApi::FromC(select)->CancelSelectBox();
}

bool rml_form_control_select_is_select_box_visible(RmlElementFormControlSelect* select)
{
	RMLUI_ASSERT(select);
	return Rml::CApi::FromC(select)->IsSelectBoxVisible();
}

void rml_form_control_text_area_set_num_columns(RmlElementFormControlTextArea* text_area, int num_columns)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetNumColumns(num_columns);
}

int rml_form_control_text_area_get_num_columns(const RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	return Rml::CApi::FromC(text_area)->GetNumColumns();
}

void rml_form_control_text_area_set_num_rows(RmlElementFormControlTextArea* text_area, int num_rows)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetNumRows(num_rows);
}

int rml_form_control_text_area_get_num_rows(const RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	return Rml::CApi::FromC(text_area)->GetNumRows();
}

void rml_form_control_text_area_set_max_length(RmlElementFormControlTextArea* text_area, int max_length)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetMaxLength(max_length);
}

int rml_form_control_text_area_get_max_length(const RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	return Rml::CApi::FromC(text_area)->GetMaxLength();
}

void rml_form_control_text_area_set_word_wrap(RmlElementFormControlTextArea* text_area, bool word_wrap)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetWordWrap(word_wrap);
}

bool rml_form_control_text_area_get_word_wrap(RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	return Rml::CApi::FromC(text_area)->GetWordWrap();
}

void rml_form_control_text_area_select(RmlElementFormControlTextArea* text_area)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->Select();
}

void rml_form_control_text_area_set_selection_range(RmlElementFormControlTextArea* text_area, int selection_start, int selection_end)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetSelectionRange(selection_start, selection_end);
}

void rml_form_control_text_area_get_selection(const RmlElementFormControlTextArea* text_area, int* selection_start, int* selection_end,
	RmlOwnedString* selected_text)
{
	RMLUI_ASSERT(text_area);
	int start = 0;
	int end = 0;
	Rml::String text;
	Rml::CApi::FromC(text_area)->GetSelection(&start, &end, &text);
	if (selection_start)
		*selection_start = start;
	if (selection_end)
		*selection_end = end;
	SetOwnedSelectionText(std::move(text), selected_text);
}

void rml_form_control_text_area_set_composition_range(RmlElementFormControlTextArea* text_area, int range_start, int range_end)
{
	RMLUI_ASSERT(text_area);
	Rml::CApi::FromC(text_area)->SetCompositionRange(range_start, range_end);
}

float rml_progress_get_value(const RmlElementProgress* progress)
{
	RMLUI_ASSERT(progress);
	return Rml::CApi::FromC(progress)->GetValue();
}

void rml_progress_set_value(RmlElementProgress* progress, float value)
{
	RMLUI_ASSERT(progress);
	Rml::CApi::FromC(progress)->SetValue(value);
}

float rml_progress_get_max(const RmlElementProgress* progress)
{
	RMLUI_ASSERT(progress);
	return Rml::CApi::FromC(progress)->GetMax();
}

void rml_progress_set_max(RmlElementProgress* progress, float max_value)
{
	RMLUI_ASSERT(progress);
	Rml::CApi::FromC(progress)->SetMax(max_value);
}

void rml_tab_set_set_tab_rml(RmlElementTabSet* tab_set, int tab_index, RmlStringView rml)
{
	RMLUI_ASSERT(tab_set);
	Rml::CApi::FromC(tab_set)->SetTab(tab_index, Rml::CApi::ToString(rml));
}

void rml_tab_set_set_panel_rml(RmlElementTabSet* tab_set, int tab_index, RmlStringView rml)
{
	RMLUI_ASSERT(tab_set);
	Rml::CApi::FromC(tab_set)->SetPanel(tab_index, Rml::CApi::ToString(rml));
}

void rml_tab_set_set_tab_element(RmlElementTabSet* tab_set, int tab_index, RmlElement* element)
{
	RMLUI_ASSERT(tab_set);
	RMLUI_ASSERT(element);
	if (Rml::CApi::FromC(element)->GetParentNode())
	{
		Rml::Log::Message(Rml::Log::LT_WARNING, "rml_tab_set_set_tab_element ignored an element that still has a parent.");
		return;
	}
	Rml::ElementPtr element_ptr(Rml::CApi::FromC(element));
	Rml::CApi::FromC(tab_set)->SetTab(tab_index, std::move(element_ptr));
}

void rml_tab_set_set_panel_element(RmlElementTabSet* tab_set, int tab_index, RmlElement* element)
{
	RMLUI_ASSERT(tab_set);
	RMLUI_ASSERT(element);
	if (Rml::CApi::FromC(element)->GetParentNode())
	{
		Rml::Log::Message(Rml::Log::LT_WARNING, "rml_tab_set_set_panel_element ignored an element that still has a parent.");
		return;
	}
	Rml::ElementPtr element_ptr(Rml::CApi::FromC(element));
	Rml::CApi::FromC(tab_set)->SetPanel(tab_index, std::move(element_ptr));
}

void rml_tab_set_remove_tab(RmlElementTabSet* tab_set, int tab_index)
{
	RMLUI_ASSERT(tab_set);
	Rml::CApi::FromC(tab_set)->RemoveTab(tab_index);
}

int rml_tab_set_get_num_tabs(RmlElementTabSet* tab_set)
{
	RMLUI_ASSERT(tab_set);
	return Rml::CApi::FromC(tab_set)->GetNumTabs();
}

void rml_tab_set_set_active_tab(RmlElementTabSet* tab_set, int tab_index)
{
	RMLUI_ASSERT(tab_set);
	Rml::CApi::FromC(tab_set)->SetActiveTab(tab_index);
}

int rml_tab_set_get_active_tab(const RmlElementTabSet* tab_set)
{
	RMLUI_ASSERT(tab_set);
	return Rml::CApi::FromC(tab_set)->GetActiveTab();
}