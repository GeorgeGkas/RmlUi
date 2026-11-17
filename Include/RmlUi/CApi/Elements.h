#ifndef RMLUI_CAPI_ELEMENTS_H
#define RMLUI_CAPI_ELEMENTS_H

#include "RmlUi/CApi/Element.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Casts --- */

RMLUI_CAPI_API RmlElement* rml_form_as_element(RmlElementForm* form);
RMLUI_CAPI_API RmlElementForm* rml_element_as_form(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_form_control_as_element(RmlElementFormControl* control);
RMLUI_CAPI_API RmlElementFormControl* rml_element_as_form_control(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_form_control_input_as_element(RmlElementFormControlInput* input);
RMLUI_CAPI_API RmlElementFormControlInput* rml_element_as_form_control_input(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_form_control_select_as_element(RmlElementFormControlSelect* select);
RMLUI_CAPI_API RmlElementFormControlSelect* rml_element_as_form_control_select(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_form_control_text_area_as_element(RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API RmlElementFormControlTextArea* rml_element_as_form_control_text_area(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_progress_as_element(RmlElementProgress* progress);
RMLUI_CAPI_API RmlElementProgress* rml_element_as_progress(RmlElement* element);

RMLUI_CAPI_API RmlElement* rml_tab_set_as_element(RmlElementTabSet* tab_set);
RMLUI_CAPI_API RmlElementTabSet* rml_element_as_tab_set(RmlElement* element);

/* --- ElementForm --- */

RMLUI_CAPI_API void rml_form_submit(RmlElementForm* form, RmlStringView name, RmlStringView submit_value);

/* --- ElementFormControl --- */

RMLUI_CAPI_API RmlOwnedString rml_form_control_get_name(const RmlElementFormControl* control);
RMLUI_CAPI_API void rml_form_control_set_name(RmlElementFormControl* control, RmlStringView name);
RMLUI_CAPI_API RmlOwnedString rml_form_control_get_value(RmlElementFormControl* control);
RMLUI_CAPI_API void rml_form_control_set_value(RmlElementFormControl* control, RmlStringView value);
RMLUI_CAPI_API bool rml_form_control_is_submitted(RmlElementFormControl* control);
RMLUI_CAPI_API bool rml_form_control_is_disabled(const RmlElementFormControl* control);
RMLUI_CAPI_API void rml_form_control_set_disabled(RmlElementFormControl* control, bool disable);

/* --- ElementFormControlInput --- */

RMLUI_CAPI_API void rml_form_control_input_select(RmlElementFormControlInput* input);
RMLUI_CAPI_API void rml_form_control_input_set_selection_range(RmlElementFormControlInput* input, int selection_start, int selection_end);
RMLUI_CAPI_API void rml_form_control_input_get_selection(const RmlElementFormControlInput* input, int* selection_start, int* selection_end,
	RmlOwnedString* selected_text);
RMLUI_CAPI_API void rml_form_control_input_set_composition_range(RmlElementFormControlInput* input, int range_start, int range_end);

/* --- ElementFormControlSelect --- */

RMLUI_CAPI_API void rml_form_control_select_set_selection(RmlElementFormControlSelect* select, int selection);
RMLUI_CAPI_API int rml_form_control_select_get_selection(const RmlElementFormControlSelect* select);
RMLUI_CAPI_API RmlElement* rml_form_control_select_get_option(RmlElementFormControlSelect* select, int index);
RMLUI_CAPI_API int rml_form_control_select_get_num_options(RmlElementFormControlSelect* select);
RMLUI_CAPI_API int rml_form_control_select_add_rml(RmlElementFormControlSelect* select, RmlStringView rml, RmlStringView value, int before,
	bool selectable);
/* Takes ownership of a caller-owned option (see Element.h). Returns -1 and leaves the element untouched when RmlUi
   already owns it. */
RMLUI_CAPI_API int rml_form_control_select_add_element(RmlElementFormControlSelect* select, RmlElement* option_element, int before);
RMLUI_CAPI_API void rml_form_control_select_remove(RmlElementFormControlSelect* select, int index);
RMLUI_CAPI_API void rml_form_control_select_remove_all(RmlElementFormControlSelect* select);
RMLUI_CAPI_API void rml_form_control_select_show_select_box(RmlElementFormControlSelect* select);
RMLUI_CAPI_API void rml_form_control_select_hide_select_box(RmlElementFormControlSelect* select);
RMLUI_CAPI_API void rml_form_control_select_cancel_select_box(RmlElementFormControlSelect* select);
RMLUI_CAPI_API bool rml_form_control_select_is_select_box_visible(RmlElementFormControlSelect* select);

/* --- ElementFormControlTextArea --- */

RMLUI_CAPI_API void rml_form_control_text_area_set_num_columns(RmlElementFormControlTextArea* text_area, int num_columns);
RMLUI_CAPI_API int rml_form_control_text_area_get_num_columns(const RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API void rml_form_control_text_area_set_num_rows(RmlElementFormControlTextArea* text_area, int num_rows);
RMLUI_CAPI_API int rml_form_control_text_area_get_num_rows(const RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API void rml_form_control_text_area_set_max_length(RmlElementFormControlTextArea* text_area, int max_length);
RMLUI_CAPI_API int rml_form_control_text_area_get_max_length(const RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API void rml_form_control_text_area_set_word_wrap(RmlElementFormControlTextArea* text_area, bool word_wrap);
RMLUI_CAPI_API bool rml_form_control_text_area_get_word_wrap(RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API void rml_form_control_text_area_select(RmlElementFormControlTextArea* text_area);
RMLUI_CAPI_API void rml_form_control_text_area_set_selection_range(RmlElementFormControlTextArea* text_area, int selection_start,
	int selection_end);
RMLUI_CAPI_API void rml_form_control_text_area_get_selection(const RmlElementFormControlTextArea* text_area, int* selection_start,
	int* selection_end, RmlOwnedString* selected_text);
RMLUI_CAPI_API void rml_form_control_text_area_set_composition_range(RmlElementFormControlTextArea* text_area, int range_start, int range_end);

/* --- ElementProgress --- */

RMLUI_CAPI_API float rml_progress_get_value(const RmlElementProgress* progress);
RMLUI_CAPI_API void rml_progress_set_value(RmlElementProgress* progress, float value);
RMLUI_CAPI_API float rml_progress_get_max(const RmlElementProgress* progress);
RMLUI_CAPI_API void rml_progress_set_max(RmlElementProgress* progress, float max_value);

/* --- ElementTabSet --- */

RMLUI_CAPI_API void rml_tab_set_set_tab_rml(RmlElementTabSet* tab_set, int tab_index, RmlStringView rml);
RMLUI_CAPI_API void rml_tab_set_set_panel_rml(RmlElementTabSet* tab_set, int tab_index, RmlStringView rml);
/* Takes ownership of a caller-owned element (see Element.h). Does nothing when RmlUi already owns it. */
RMLUI_CAPI_API void rml_tab_set_set_tab_element(RmlElementTabSet* tab_set, int tab_index, RmlElement* element);
/* Takes ownership of a caller-owned element (see Element.h). Does nothing when RmlUi already owns it. */
RMLUI_CAPI_API void rml_tab_set_set_panel_element(RmlElementTabSet* tab_set, int tab_index, RmlElement* element);
RMLUI_CAPI_API void rml_tab_set_remove_tab(RmlElementTabSet* tab_set, int tab_index);
RMLUI_CAPI_API int rml_tab_set_get_num_tabs(RmlElementTabSet* tab_set);
RMLUI_CAPI_API void rml_tab_set_set_active_tab(RmlElementTabSet* tab_set, int tab_index);
RMLUI_CAPI_API int rml_tab_set_get_active_tab(const RmlElementTabSet* tab_set);

#ifdef __cplusplus
}
#endif

#endif