#include "Conversions.h"
#include <RmlUi/CApi/Element.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Log.h>
#include <RmlUi/Core/ElementText.h>
#include <RmlUi/Core/ElementUtilities.h>
#include <RmlUi/Core/Property.h>
#include <RmlUi/Core/ScrollTypes.h>
#include <RmlUi/Core/Traits.h> // rmlui_dynamic_cast
#include <RmlUi/Core/Types.h>

namespace {

RmlElement* ReleaseElement(Rml::ElementPtr element_ptr)
{
	return Rml::CApi::ToC(element_ptr.release());
}

bool IsCallerOwned(RmlElement* element)
{
	return element && Rml::CApi::FromC(element)->GetParentNode() == nullptr;
}

Rml::ScrollIntoViewOptions ToScrollIntoViewOptions(RmlScrollIntoViewOptions options)
{
	return Rml::ScrollIntoViewOptions(static_cast<Rml::ScrollAlignment>(options.vertical),
		static_cast<Rml::ScrollAlignment>(options.horizontal), static_cast<Rml::ScrollBehavior>(options.behavior),
		static_cast<Rml::ScrollParentage>(options.parentage));
}

Rml::Dictionary ToDictionary(const RmlDictionary* parameters)
{
	if (!parameters)
		return {};
	return *Rml::CApi::FromC(parameters);
}

} // namespace

RmlElement* rml_element_clone(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return ReleaseElement(Rml::CApi::FromC(element)->Clone());
}

void rml_element_release(RmlElement* element)
{
	RMLUI_ASSERT(element);
	if (!IsCallerOwned(element))
	{
		Rml::Log::Message(Rml::Log::LT_WARNING, "rml_element_release ignored an element that still has a parent.");
		return;
	}
	Rml::ElementPtr ptr(Rml::CApi::FromC(element));
}

void rml_element_set_class(RmlElement* element, RmlStringView class_name, bool activate)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetClass(Rml::CApi::ToString(class_name), activate);
}

bool rml_element_is_class_set(const RmlElement* element, RmlStringView class_name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->IsClassSet(Rml::CApi::ToString(class_name));
}

void rml_element_set_class_names(RmlElement* element, RmlStringView class_names)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetClassNames(Rml::CApi::ToString(class_names));
}

RmlOwnedString rml_element_get_class_names(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToOwnedString(Rml::CApi::FromC(element)->GetClassNames());
}

RmlOwnedString rml_element_get_address(const RmlElement* element, bool include_pseudo_classes, bool include_parents)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToOwnedString(Rml::CApi::FromC(element)->GetAddress(include_pseudo_classes, include_parents));
}

RmlVector2f rml_element_get_relative_offset(RmlElement* element, RmlBoxArea area)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetRelativeOffset(static_cast<Rml::BoxArea>(area)));
}

RmlVector2f rml_element_get_absolute_offset(RmlElement* element, RmlBoxArea area)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetAbsoluteOffset(static_cast<Rml::BoxArea>(area)));
}

RmlBoxArea rml_element_get_clip_area(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return static_cast<RmlBoxArea>(Rml::CApi::FromC(element)->GetClipArea());
}

float rml_element_get_baseline(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetBaseline();
}

bool rml_element_get_intrinsic_dimensions(RmlElement* element, RmlVector2f* dimensions, float* ratio)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(dimensions);
	RMLUI_ASSERT(ratio);
	Rml::Vector2f size;
	float aspect_ratio = 0.f;
	const bool result = Rml::CApi::FromC(element)->GetIntrinsicDimensions(size, aspect_ratio);
	if (result)
	{
		*dimensions = Rml::CApi::ToC(size);
		*ratio = aspect_ratio;
	}
	return result;
}

bool rml_element_is_replaced(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->IsReplaced();
}

bool rml_element_is_point_within_element(RmlElement* element, RmlVector2f point)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->IsPointWithinElement(Rml::CApi::FromC(point));
}

bool rml_element_is_visible(const RmlElement* element, bool include_ancestors)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->IsVisible(include_ancestors);
}

float rml_element_get_z_index(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetZIndex();
}

RmlFontFaceHandle rml_element_get_font_face_handle(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetFontFaceHandle();
}

bool rml_element_set_property(RmlElement* element, RmlStringView name, RmlStringView value)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->SetProperty(Rml::CApi::ToString(name), Rml::CApi::ToString(value));
}

void rml_element_remove_property(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->RemoveProperty(Rml::CApi::ToString(name));
}

bool rml_element_has_property(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetProperty(Rml::CApi::ToString(name)) != nullptr;
}

RmlOwnedString rml_element_get_property_string(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	const Rml::Property* property = Rml::CApi::FromC(element)->GetProperty(Rml::CApi::ToString(name));
	if (!property)
		return {};
	return Rml::CApi::ToOwnedString(property->ToString());
}

bool rml_element_has_local_property(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetLocalProperty(Rml::CApi::ToString(name)) != nullptr;
}

RmlOwnedString rml_element_get_local_property_string(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	const Rml::Property* property = Rml::CApi::FromC(element)->GetLocalProperty(Rml::CApi::ToString(name));
	if (!property)
		return {};
	return Rml::CApi::ToOwnedString(property->ToString());
}

RmlVector2f rml_element_get_containing_block(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetContainingBlock());
}

float rml_element_get_line_height(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetLineHeight();
}

bool rml_element_project(RmlElement* element, RmlVector2f* point)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(point);
	Rml::Vector2f projected = Rml::CApi::FromC(*point);
	const bool success = Rml::CApi::FromC(element)->Project(projected);
	if (success)
		*point = Rml::CApi::ToC(projected);
	return success;
}

void rml_element_set_pseudo_class(RmlElement* element, RmlStringView pseudo_class, bool activate)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetPseudoClass(Rml::CApi::ToString(pseudo_class), activate);
}

bool rml_element_is_pseudo_class_set(const RmlElement* element, RmlStringView pseudo_class)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->IsPseudoClassSet(Rml::CApi::ToString(pseudo_class));
}

void rml_element_get_active_pseudo_classes(const RmlElement* element, RmlStringList* out_list)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(out_list);
	Rml::StringList& list = *Rml::CApi::FromC(out_list);
	list = Rml::CApi::FromC(element)->GetActivePseudoClasses();
}

bool rml_element_are_pseudo_classes_set(const RmlElement* element, const RmlStringList* pseudo_classes)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(pseudo_classes);
	return Rml::CApi::FromC(element)->ArePseudoClassesSet(*Rml::CApi::FromC(pseudo_classes));
}

void rml_element_set_attribute_string(RmlElement* element, RmlStringView name, RmlStringView value)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetAttribute(Rml::CApi::ToString(name), Rml::CApi::ToString(value));
}

void rml_element_set_attribute_variant(RmlElement* element, RmlStringView name, const RmlVariant* value)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(value);
	Rml::CApi::FromC(element)->SetAttribute(Rml::CApi::ToString(name), *Rml::CApi::FromC(value));
}

const RmlVariant* rml_element_get_attribute(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetAttribute(Rml::CApi::ToString(name)));
}

const RmlVariant* rml_element_get_attribute_const(const RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetAttribute(Rml::CApi::ToString(name)));
}

RmlOwnedString rml_element_get_attribute_string(const RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	const Rml::Variant* attribute = Rml::CApi::FromC(element)->GetAttribute(Rml::CApi::ToString(name));
	if (!attribute)
		return {};
	Rml::String value;
	if (!attribute->GetInto(value))
		return {};
	return Rml::CApi::ToOwnedString(value);
}

bool rml_element_has_attribute(const RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->HasAttribute(Rml::CApi::ToString(name));
}

void rml_element_remove_attribute(RmlElement* element, RmlStringView name)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->RemoveAttribute(Rml::CApi::ToString(name));
}

int rml_element_get_num_attributes(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetNumAttributes();
}

RmlElement* rml_element_get_focus_leaf_node(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetFocusLeafNode());
}

RmlContext* rml_element_get_context(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetContext());
}

RmlStringView rml_element_get_tag_name(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToStringView(Rml::CApi::FromC(element)->GetTagName());
}

RmlStringView rml_element_get_id(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToStringView(Rml::CApi::FromC(element)->GetId());
}

void rml_element_set_id(RmlElement* element, RmlStringView id)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetId(Rml::CApi::ToString(id));
}

float rml_element_get_absolute_left(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetAbsoluteLeft();
}

float rml_element_get_absolute_top(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetAbsoluteTop();
}

float rml_element_get_client_left(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetClientLeft();
}

float rml_element_get_client_top(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetClientTop();
}

float rml_element_get_client_width(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetClientWidth();
}

float rml_element_get_client_height(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetClientHeight();
}

RmlElement* rml_element_get_offset_parent(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetOffsetParent());
}

float rml_element_get_offset_left(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetOffsetLeft();
}

float rml_element_get_offset_top(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetOffsetTop();
}

float rml_element_get_offset_width(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetOffsetWidth();
}

float rml_element_get_offset_height(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetOffsetHeight();
}

float rml_element_get_scroll_left(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetScrollLeft();
}

void rml_element_set_scroll_left(RmlElement* element, float scroll_left)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetScrollLeft(scroll_left);
}

float rml_element_get_scroll_top(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetScrollTop();
}

void rml_element_set_scroll_top(RmlElement* element, float scroll_top)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetScrollTop(scroll_top);
}

float rml_element_get_scroll_width(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetScrollWidth();
}

float rml_element_get_scroll_height(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetScrollHeight();
}

RmlElementDocument* rml_element_get_owner_document(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetOwnerDocument());
}

RmlElement* rml_element_get_parent_node(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetParentNode());
}

RmlElement* rml_element_closest(const RmlElement* element, RmlStringView selectors)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->Closest(Rml::CApi::ToString(selectors)));
}

RmlElement* rml_element_get_next_sibling(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetNextSibling());
}

RmlElement* rml_element_get_previous_sibling(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetPreviousSibling());
}

RmlElement* rml_element_get_first_child(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetFirstChild());
}

RmlElement* rml_element_get_last_child(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetLastChild());
}

RmlElement* rml_element_get_child(const RmlElement* element, int index)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetChild(index));
}

int rml_element_get_num_children(const RmlElement* element, bool include_non_dom_elements)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->GetNumChildren(include_non_dom_elements);
}

bool rml_element_has_child_nodes(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->HasChildNodes();
}

RmlOwnedString rml_element_get_inner_rml(const RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToOwnedString(Rml::CApi::FromC(element)->GetInnerRML());
}

void rml_element_set_inner_rml(RmlElement* element, RmlStringView rml)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->SetInnerRML(Rml::CApi::ToString(rml));
}

bool rml_element_focus(RmlElement* element, bool focus_visible)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->Focus(focus_visible);
}

void rml_element_blur(RmlElement* element)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->Blur();
}

void rml_element_click(RmlElement* element)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->Click();
}

void rml_element_add_event_listener(RmlElement* element, RmlStringView event, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(listener);
	Rml::CApi::FromC(element)->AddEventListener(Rml::CApi::ToString(event), Rml::CApi::FromC(listener), in_capture_phase);
}

void rml_element_add_event_listener_by_id(RmlElement* element, uint16_t event_id, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(listener);
	Rml::CApi::FromC(element)->AddEventListener(static_cast<Rml::EventId>(event_id), Rml::CApi::FromC(listener), in_capture_phase);
}

void rml_element_remove_event_listener(RmlElement* element, RmlStringView event, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(listener);
	Rml::CApi::FromC(element)->RemoveEventListener(Rml::CApi::ToString(event), Rml::CApi::FromC(listener), in_capture_phase);
}

void rml_element_remove_event_listener_by_id(RmlElement* element, uint16_t event_id, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(listener);
	Rml::CApi::FromC(element)->RemoveEventListener(static_cast<Rml::EventId>(event_id), Rml::CApi::FromC(listener), in_capture_phase);
}

bool rml_element_dispatch_event(RmlElement* element, RmlStringView type, const RmlDictionary* parameters)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->DispatchEvent(Rml::CApi::ToString(type), ToDictionary(parameters));
}

bool rml_element_dispatch_event_ex(RmlElement* element, RmlStringView type, const RmlDictionary* parameters, bool interruptible, bool bubbles)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->DispatchEvent(Rml::CApi::ToString(type), ToDictionary(parameters), interruptible, bubbles);
}

bool rml_element_dispatch_event_by_id(RmlElement* element, uint16_t event_id, const RmlDictionary* parameters)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->DispatchEvent(static_cast<Rml::EventId>(event_id), ToDictionary(parameters));
}

void rml_element_scroll_into_view(RmlElement* element, bool align_with_top)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->ScrollIntoView(align_with_top);
}

void rml_element_scroll_into_view_with_options(RmlElement* element, RmlScrollIntoViewOptions options)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->ScrollIntoView(ToScrollIntoViewOptions(options));
}

void rml_element_scroll_to(RmlElement* element, RmlVector2f offset, RmlScrollBehavior behavior)
{
	RMLUI_ASSERT(element);
	Rml::CApi::FromC(element)->ScrollTo(Rml::CApi::FromC(offset), static_cast<Rml::ScrollBehavior>(behavior));
}

RmlElement* rml_element_append_child(RmlElement* element, RmlElement* child)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(child);
	if (!IsCallerOwned(child))
		return nullptr;
	Rml::ElementPtr child_ptr(Rml::CApi::FromC(child));
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->AppendChild(std::move(child_ptr)));
}

RmlElement* rml_element_insert_before(RmlElement* element, RmlElement* child, RmlElement* adjacent_element)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(child);
	RMLUI_ASSERT(adjacent_element);
	if (!IsCallerOwned(child))
		return nullptr;
	Rml::ElementPtr child_ptr(Rml::CApi::FromC(child));
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->InsertBefore(std::move(child_ptr), Rml::CApi::FromC(adjacent_element)));
}

RmlElement* rml_element_replace_child(RmlElement* element, RmlElement* new_child, RmlElement* old_child)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(new_child);
	RMLUI_ASSERT(old_child);
	if (!IsCallerOwned(new_child))
		return nullptr;
	Rml::ElementPtr new_child_ptr(Rml::CApi::FromC(new_child));
	return ReleaseElement(Rml::CApi::FromC(element)->ReplaceChild(std::move(new_child_ptr), Rml::CApi::FromC(old_child)));
}

RmlElement* rml_element_remove_child(RmlElement* element, RmlElement* child)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(child);
	return ReleaseElement(Rml::CApi::FromC(element)->RemoveChild(Rml::CApi::FromC(child)));
}

RmlElement* rml_element_get_element_by_id(RmlElement* element, RmlStringView id)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->GetElementById(Rml::CApi::ToString(id)));
}

void rml_element_get_elements_by_tag_name(RmlElement* element, RmlElementList* out_elements, RmlStringView tag)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(out_elements);
	Rml::CApi::FromC(out_elements)->clear();
	Rml::CApi::FromC(element)->GetElementsByTagName(*Rml::CApi::FromC(out_elements), Rml::CApi::ToString(tag));
}

void rml_element_get_elements_by_class_name(RmlElement* element, RmlElementList* out_elements, RmlStringView class_name)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(out_elements);
	Rml::CApi::FromC(out_elements)->clear();
	Rml::CApi::FromC(element)->GetElementsByClassName(*Rml::CApi::FromC(out_elements), Rml::CApi::ToString(class_name));
}

RmlElement* rml_element_query_selector(RmlElement* element, RmlStringView selector)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(Rml::CApi::FromC(element)->QuerySelector(Rml::CApi::ToString(selector)));
}

void rml_element_query_selector_all(RmlElement* element, RmlElementList* out_elements, RmlStringView selector)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(out_elements);
	Rml::CApi::FromC(out_elements)->clear();
	Rml::CApi::FromC(element)->QuerySelectorAll(*Rml::CApi::FromC(out_elements), Rml::CApi::ToString(selector));
}

bool rml_element_matches(RmlElement* element, RmlStringView selector)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::FromC(element)->Matches(Rml::CApi::ToString(selector));
}

bool rml_element_contains(const RmlElement* element, RmlElement* other)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(other);
	return Rml::CApi::FromC(element)->Contains(Rml::CApi::FromC(other));
}

RmlElement* rml_text_as_element(RmlElementText* text)
{
	RMLUI_ASSERT(text);
	return Rml::CApi::ToC(static_cast<Rml::Element*>(Rml::CApi::FromC(text)));
}

RmlElementText* rml_element_as_text(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::CApi::ToC(rmlui_dynamic_cast<Rml::ElementText*>(Rml::CApi::FromC(element)));
}

void rml_text_set_text(RmlElementText* text, RmlStringView text_content)
{
	RMLUI_ASSERT(text);
	Rml::CApi::FromC(text)->SetText(Rml::CApi::ToString(text_content));
}

RmlStringView rml_text_get_text(const RmlElementText* text)
{
	RMLUI_ASSERT(text);
	return Rml::CApi::ToStringView(Rml::CApi::FromC(text)->GetText());
}

void rml_text_suppress_auto_layout(RmlElementText* text)
{
	RMLUI_ASSERT(text);
	Rml::CApi::FromC(text)->SuppressAutoLayout();
}

RmlElement* rml_element_utilities_get_element_by_id(RmlElement* root_element, RmlStringView id)
{
	RMLUI_ASSERT(root_element);
	return Rml::CApi::ToC(Rml::ElementUtilities::GetElementById(Rml::CApi::FromC(root_element), Rml::CApi::ToString(id)));
}

void rml_element_utilities_get_elements_by_tag_name(RmlElementList* elements, RmlElement* root_element, RmlStringView tag,
	RmlStringView stop_tag)
{
	RMLUI_ASSERT(elements);
	RMLUI_ASSERT(root_element);
	Rml::CApi::FromC(elements)->clear();
	Rml::ElementUtilities::GetElementsByTagName(*Rml::CApi::FromC(elements), Rml::CApi::FromC(root_element), Rml::CApi::ToString(tag),
		Rml::CApi::ToString(stop_tag));
}

void rml_element_utilities_get_elements_by_class_name(RmlElementList* elements, RmlElement* root_element, RmlStringView class_name)
{
	RMLUI_ASSERT(elements);
	RMLUI_ASSERT(root_element);
	Rml::CApi::FromC(elements)->clear();
	Rml::ElementUtilities::GetElementsByClassName(*Rml::CApi::FromC(elements), Rml::CApi::FromC(root_element), Rml::CApi::ToString(class_name));
}

float rml_element_utilities_get_dp_ratio(RmlElement* element)
{
	RMLUI_ASSERT(element);
	return Rml::ElementUtilities::GetDensityIndependentPixelRatio(Rml::CApi::FromC(element));
}

int rml_element_utilities_get_string_width(RmlElement* element, RmlStringView string, uint32_t prior_character)
{
	RMLUI_ASSERT(element);
	const Rml::Character prior = prior_character == 0 ? Rml::Character::Null : static_cast<Rml::Character>(prior_character);
	return Rml::ElementUtilities::GetStringWidth(Rml::CApi::FromC(element), Rml::CApi::ToString(string), prior);
}

bool rml_element_utilities_get_bounding_box(RmlElement* element, RmlRectanglef* out_rectangle, RmlBoxArea area)
{
	RMLUI_ASSERT(element);
	RMLUI_ASSERT(out_rectangle);
	Rml::Rectanglef rectangle;
	const bool success = Rml::ElementUtilities::GetBoundingBox(rectangle, Rml::CApi::FromC(element), static_cast<Rml::BoxArea>(area));
	if (success)
		*out_rectangle = Rml::CApi::ToC(rectangle);
	return success;
}

void rml_element_utilities_format_element(RmlElement* element, RmlVector2f containing_block)
{
	RMLUI_ASSERT(element);
	Rml::ElementUtilities::FormatElement(Rml::CApi::FromC(element), Rml::CApi::FromC(containing_block));
}