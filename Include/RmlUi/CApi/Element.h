#ifndef RMLUI_CAPI_ELEMENT_H
#define RMLUI_CAPI_ELEMENT_H

#include "RmlUi/CApi/Event.h"
#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* --- Element ownership ---
   An element is caller-owned when RmlUi holds no reference to it: it has no parent, is not a document attached to a
   context (loaded or pending unload), and is not the root element of a context. Elements returned by
   rml_element_clone, rml_element_remove_child, rml_element_replace_child, rml_document_create_element,
   rml_document_create_text_node and rml_element_create_custom are caller-owned. Functions that take ownership leave
   elements owned by RmlUi untouched. */

RMLUI_CAPI_API RmlElement* rml_element_clone(const RmlElement* element);
/* Destroys a caller-owned element. Logs a warning and does nothing for elements owned by RmlUi. */
RMLUI_CAPI_API void rml_element_release(RmlElement* element);

/* --- Classes --- */

RMLUI_CAPI_API void rml_element_set_class(RmlElement* element, RmlStringView class_name, bool activate);
RMLUI_CAPI_API bool rml_element_is_class_set(const RmlElement* element, RmlStringView class_name);
RMLUI_CAPI_API void rml_element_set_class_names(RmlElement* element, RmlStringView class_names);
RMLUI_CAPI_API RmlOwnedString rml_element_get_class_names(const RmlElement* element);

/* --- Address and geometry --- */

RMLUI_CAPI_API RmlOwnedString rml_element_get_address(const RmlElement* element, bool include_pseudo_classes, bool include_parents);
RMLUI_CAPI_API RmlVector2f rml_element_get_relative_offset(RmlElement* element, RmlBoxArea area);
RMLUI_CAPI_API RmlVector2f rml_element_get_absolute_offset(RmlElement* element, RmlBoxArea area);
RMLUI_CAPI_API RmlBoxArea rml_element_get_clip_area(const RmlElement* element);
RMLUI_CAPI_API float rml_element_get_baseline(const RmlElement* element);
RMLUI_CAPI_API bool rml_element_get_intrinsic_dimensions(RmlElement* element, RmlVector2f* dimensions, float* ratio);
RMLUI_CAPI_API bool rml_element_is_replaced(RmlElement* element);
RMLUI_CAPI_API bool rml_element_is_point_within_element(RmlElement* element, RmlVector2f point);
RMLUI_CAPI_API bool rml_element_is_visible(const RmlElement* element, bool include_ancestors);
RMLUI_CAPI_API float rml_element_get_z_index(const RmlElement* element);
RMLUI_CAPI_API RmlFontFaceHandle rml_element_get_font_face_handle(const RmlElement* element);

/* --- Properties --- */

RMLUI_CAPI_API bool rml_element_set_property(RmlElement* element, RmlStringView name, RmlStringView value);
RMLUI_CAPI_API void rml_element_remove_property(RmlElement* element, RmlStringView name);
/* Like Element::GetProperty, returns true for every registered property name, since every element has a computed
   value for each property. Use rml_element_has_local_property to test whether the element itself declares it. */
RMLUI_CAPI_API bool rml_element_has_property(RmlElement* element, RmlStringView name);
RMLUI_CAPI_API RmlOwnedString rml_element_get_property_string(RmlElement* element, RmlStringView name);
RMLUI_CAPI_API bool rml_element_has_local_property(RmlElement* element, RmlStringView name);
RMLUI_CAPI_API RmlOwnedString rml_element_get_local_property_string(RmlElement* element, RmlStringView name);

RMLUI_CAPI_API RmlVector2f rml_element_get_containing_block(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_line_height(RmlElement* element);
RMLUI_CAPI_API bool rml_element_project(RmlElement* element, RmlVector2f* point);

/* --- Pseudo-classes --- */

RMLUI_CAPI_API void rml_element_set_pseudo_class(RmlElement* element, RmlStringView pseudo_class, bool activate);
RMLUI_CAPI_API bool rml_element_is_pseudo_class_set(const RmlElement* element, RmlStringView pseudo_class);
RMLUI_CAPI_API void rml_element_get_active_pseudo_classes(const RmlElement* element, RmlStringList* out_list);
RMLUI_CAPI_API bool rml_element_are_pseudo_classes_set(const RmlElement* element, const RmlStringList* pseudo_classes);

/* --- Attributes --- */

RMLUI_CAPI_API void rml_element_set_attribute_string(RmlElement* element, RmlStringView name, RmlStringView value);
RMLUI_CAPI_API void rml_element_set_attribute_variant(RmlElement* element, RmlStringView name, const RmlVariant* value);
RMLUI_CAPI_API const RmlVariant* rml_element_get_attribute(RmlElement* element, RmlStringView name);
RMLUI_CAPI_API const RmlVariant* rml_element_get_attribute_const(const RmlElement* element, RmlStringView name);
RMLUI_CAPI_API RmlOwnedString rml_element_get_attribute_string(const RmlElement* element, RmlStringView name);
RMLUI_CAPI_API bool rml_element_has_attribute(const RmlElement* element, RmlStringView name);
RMLUI_CAPI_API void rml_element_remove_attribute(RmlElement* element, RmlStringView name);
RMLUI_CAPI_API int rml_element_get_num_attributes(const RmlElement* element);

/* --- Context and focus --- */

RMLUI_CAPI_API RmlElement* rml_element_get_focus_leaf_node(RmlElement* element);
RMLUI_CAPI_API RmlContext* rml_element_get_context(const RmlElement* element);

/* --- DOM identity --- */

RMLUI_CAPI_API RmlStringView rml_element_get_tag_name(const RmlElement* element);
RMLUI_CAPI_API RmlStringView rml_element_get_id(const RmlElement* element);
RMLUI_CAPI_API void rml_element_set_id(RmlElement* element, RmlStringView id);

/* --- Absolute / client / offset / scroll metrics --- */

RMLUI_CAPI_API float rml_element_get_absolute_left(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_absolute_top(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_client_left(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_client_top(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_client_width(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_client_height(RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_offset_parent(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_offset_left(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_offset_top(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_offset_width(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_offset_height(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_scroll_left(RmlElement* element);
RMLUI_CAPI_API void rml_element_set_scroll_left(RmlElement* element, float scroll_left);
RMLUI_CAPI_API float rml_element_get_scroll_top(RmlElement* element);
RMLUI_CAPI_API void rml_element_set_scroll_top(RmlElement* element, float scroll_top);
RMLUI_CAPI_API float rml_element_get_scroll_width(RmlElement* element);
RMLUI_CAPI_API float rml_element_get_scroll_height(RmlElement* element);

/* --- DOM tree --- */

RMLUI_CAPI_API RmlElementDocument* rml_element_get_owner_document(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_parent_node(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_closest(const RmlElement* element, RmlStringView selectors);
RMLUI_CAPI_API RmlElement* rml_element_get_next_sibling(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_previous_sibling(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_first_child(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_last_child(const RmlElement* element);
RMLUI_CAPI_API RmlElement* rml_element_get_child(const RmlElement* element, int index);
RMLUI_CAPI_API int rml_element_get_num_children(const RmlElement* element, bool include_non_dom_elements);
RMLUI_CAPI_API bool rml_element_has_child_nodes(const RmlElement* element);
RMLUI_CAPI_API RmlOwnedString rml_element_get_inner_rml(const RmlElement* element);
RMLUI_CAPI_API void rml_element_set_inner_rml(RmlElement* element, RmlStringView rml);

/* --- DOM interaction --- */

RMLUI_CAPI_API bool rml_element_focus(RmlElement* element, bool focus_visible);
RMLUI_CAPI_API void rml_element_blur(RmlElement* element);
RMLUI_CAPI_API void rml_element_click(RmlElement* element);

RMLUI_CAPI_API void rml_element_add_event_listener(RmlElement* element, RmlStringView event, RmlEventListener* listener,
	bool in_capture_phase);
RMLUI_CAPI_API void rml_element_add_event_listener_by_id(RmlElement* element, uint16_t event_id, RmlEventListener* listener,
	bool in_capture_phase);
RMLUI_CAPI_API void rml_element_remove_event_listener(RmlElement* element, RmlStringView event, RmlEventListener* listener,
	bool in_capture_phase);
RMLUI_CAPI_API void rml_element_remove_event_listener_by_id(RmlElement* element, uint16_t event_id, RmlEventListener* listener,
	bool in_capture_phase);

RMLUI_CAPI_API bool rml_element_dispatch_event(RmlElement* element, RmlStringView type, const RmlDictionary* parameters);
RMLUI_CAPI_API bool rml_element_dispatch_event_ex(RmlElement* element, RmlStringView type, const RmlDictionary* parameters,
	bool interruptible, bool bubbles);
RMLUI_CAPI_API bool rml_element_dispatch_event_by_id(RmlElement* element, uint16_t event_id, const RmlDictionary* parameters);

RMLUI_CAPI_API void rml_element_scroll_into_view(RmlElement* element, bool align_with_top);
RMLUI_CAPI_API void rml_element_scroll_into_view_with_options(RmlElement* element, RmlScrollIntoViewOptions options);
RMLUI_CAPI_API void rml_element_scroll_to(RmlElement* element, RmlVector2f offset, RmlScrollBehavior behavior);

/* --- DOM mutation (child ownership) --- */

/* Takes ownership of a caller-owned child and returns it. Returns null and leaves the child untouched when RmlUi
   already owns it. */
RMLUI_CAPI_API RmlElement* rml_element_append_child(RmlElement* element, RmlElement* child);
/* Like rml_element_append_child, but inserts before adjacent_element. Appends when adjacent_element is null or not a
   child of element. */
RMLUI_CAPI_API RmlElement* rml_element_insert_before(RmlElement* element, RmlElement* child, RmlElement* adjacent_element);
/* Replaces old_child with the caller-owned new_child and returns old_child, which becomes caller-owned. Returns null
   and takes nothing when new_child is owned by RmlUi or old_child is not a child of element. */
RMLUI_CAPI_API RmlElement* rml_element_replace_child(RmlElement* element, RmlElement* new_child, RmlElement* old_child);
/* Detaches child and returns it as caller-owned. Returns null when child is not a child of element. */
RMLUI_CAPI_API RmlElement* rml_element_remove_child(RmlElement* element, RmlElement* child);

/* --- DOM query --- */

RMLUI_CAPI_API RmlElement* rml_element_get_element_by_id(RmlElement* element, RmlStringView id);
RMLUI_CAPI_API void rml_element_get_elements_by_tag_name(RmlElement* element, RmlElementList* out_elements, RmlStringView tag);
RMLUI_CAPI_API void rml_element_get_elements_by_class_name(RmlElement* element, RmlElementList* out_elements, RmlStringView class_name);
RMLUI_CAPI_API RmlElement* rml_element_query_selector(RmlElement* element, RmlStringView selector);
RMLUI_CAPI_API void rml_element_query_selector_all(RmlElement* element, RmlElementList* out_elements, RmlStringView selector);
RMLUI_CAPI_API bool rml_element_matches(RmlElement* element, RmlStringView selector);
RMLUI_CAPI_API bool rml_element_contains(const RmlElement* element, RmlElement* other);

/* --- ElementText --- */

RMLUI_CAPI_API RmlElement* rml_text_as_element(RmlElementText* text);
RMLUI_CAPI_API RmlElementText* rml_element_as_text(RmlElement* element);
RMLUI_CAPI_API void rml_text_set_text(RmlElementText* text, RmlStringView text_content);
RMLUI_CAPI_API RmlStringView rml_text_get_text(const RmlElementText* text);
RMLUI_CAPI_API void rml_text_suppress_auto_layout(RmlElementText* text);

/* --- ElementUtilities --- */

RMLUI_CAPI_API RmlElement* rml_element_utilities_get_element_by_id(RmlElement* root_element, RmlStringView id);
RMLUI_CAPI_API void rml_element_utilities_get_elements_by_tag_name(RmlElementList* elements, RmlElement* root_element, RmlStringView tag,
	RmlStringView stop_tag);
RMLUI_CAPI_API void rml_element_utilities_get_elements_by_class_name(RmlElementList* elements, RmlElement* root_element,
	RmlStringView class_name);
RMLUI_CAPI_API float rml_element_utilities_get_dp_ratio(RmlElement* element);
RMLUI_CAPI_API int rml_element_utilities_get_string_width(RmlElement* element, RmlStringView string, uint32_t prior_character);
RMLUI_CAPI_API bool rml_element_utilities_get_bounding_box(RmlElement* element, RmlRectanglef* out_rectangle, RmlBoxArea area);
RMLUI_CAPI_API void rml_element_utilities_format_element(RmlElement* element, RmlVector2f containing_block);

#ifdef __cplusplus
}
#endif

#endif