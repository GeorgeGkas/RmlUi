#include "Conversions.h"
#include <RmlUi/CApi/Context.h>
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/DataModelHandle.h>
#include <RmlUi/Core/Debug.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/ScrollTypes.h>
#include <RmlUi/Core/Types.h>

namespace {

Rml::TouchList BuildTouchList(const RmlTouch* touches, size_t num_touches)
{
	Rml::TouchList result;
	if (!touches || num_touches == 0)
		return result;

	result.reserve(num_touches);
	for (size_t i = 0; i < num_touches; ++i)
		result.push_back(Rml::CApi::FromC(touches[i]));
	return result;
}

} // namespace

RmlStringView rml_context_get_name(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToStringView(Rml::CApi::FromC(ctx)->GetName());
}

void rml_context_set_dimensions(RmlContext* ctx, RmlVector2i dimensions)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->SetDimensions(Rml::CApi::FromC(dimensions));
}

RmlVector2i rml_context_get_dimensions(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetDimensions());
}

void rml_context_set_dp_ratio(RmlContext* ctx, float dp_ratio)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->SetDensityIndependentPixelRatio(dp_ratio);
}

float rml_context_get_dp_ratio(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->GetDensityIndependentPixelRatio();
}

bool rml_context_update(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->Update();
}

bool rml_context_render(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->Render();
}

RmlElementDocument* rml_context_create_document(RmlContext* ctx, RmlStringView instancer_name)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->CreateDocument(Rml::CApi::ToString(instancer_name)));
}

RmlElementDocument* rml_context_load_document(RmlContext* ctx, RmlStringView path)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->LoadDocument(Rml::CApi::ToString(path)));
}

RmlElementDocument* rml_context_load_document_from_memory(RmlContext* ctx, RmlStringView rml, RmlStringView source_url)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(
		Rml::CApi::FromC(ctx)->LoadDocumentFromMemory(Rml::CApi::ToString(rml), Rml::CApi::ToString(source_url)));
}

void rml_context_unload_document(RmlContext* ctx, RmlElementDocument* document)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(document);
	Rml::CApi::FromC(ctx)->UnloadDocument(Rml::CApi::FromC(document));
}

void rml_context_unload_all_documents(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->UnloadAllDocuments();
}

void rml_context_enable_mouse_cursor(RmlContext* ctx, bool enable)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->EnableMouseCursor(enable);
}

void rml_context_activate_theme(RmlContext* ctx, RmlStringView theme_name, bool activate)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->ActivateTheme(Rml::CApi::ToString(theme_name), activate);
}

bool rml_context_is_theme_active(const RmlContext* ctx, RmlStringView theme_name)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->IsThemeActive(Rml::CApi::ToString(theme_name));
}

RmlElementDocument* rml_context_get_document_by_id(RmlContext* ctx, RmlStringView id)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetDocument(Rml::CApi::ToString(id)));
}

RmlElementDocument* rml_context_get_document_by_index(RmlContext* ctx, int index)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetDocument(index));
}

int rml_context_get_num_documents(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->GetNumDocuments();
}

RmlElement* rml_context_get_hover_element(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetHoverElement());
}

RmlElement* rml_context_get_focus_element(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetFocusElement());
}

RmlElement* rml_context_get_root_element(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetRootElement());
}

RmlElement* rml_context_get_element_at_point(RmlContext* ctx, RmlVector2f point, const RmlElement* ignore_element)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(
		Rml::CApi::FromC(ctx)->GetElementAtPoint(Rml::CApi::FromC(point), Rml::CApi::FromC(ignore_element)));
}

RmlElement* rml_context_get_element_at_point_within(RmlContext* ctx, RmlVector2f point, const RmlElement* ignore_element, RmlElement* element)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(
		Rml::CApi::FromC(ctx)->GetElementAtPoint(Rml::CApi::FromC(point), Rml::CApi::FromC(ignore_element), Rml::CApi::FromC(element)));
}

void rml_context_pull_document_to_front(RmlContext* ctx, RmlElementDocument* document)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(document);
	Rml::CApi::FromC(ctx)->PullDocumentToFront(Rml::CApi::FromC(document));
}

void rml_context_push_document_to_back(RmlContext* ctx, RmlElementDocument* document)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(document);
	Rml::CApi::FromC(ctx)->PushDocumentToBack(Rml::CApi::FromC(document));
}

void rml_context_unfocus_document(RmlContext* ctx, RmlElementDocument* document)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(document);
	Rml::CApi::FromC(ctx)->UnfocusDocument(Rml::CApi::FromC(document));
}

void rml_context_add_event_listener(RmlContext* ctx, RmlStringView event, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(listener);
	Rml::CApi::FromC(ctx)->AddEventListener(Rml::CApi::ToString(event), Rml::CApi::FromC(listener), in_capture_phase);
}

void rml_context_remove_event_listener(RmlContext* ctx, RmlStringView event, RmlEventListener* listener, bool in_capture_phase)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	RMLUI_CAPI_REQUIRE_VOID(listener);
	Rml::CApi::FromC(ctx)->RemoveEventListener(Rml::CApi::ToString(event), Rml::CApi::FromC(listener), in_capture_phase);
}

bool rml_context_process_key_down(RmlContext* ctx, RmlKeyIdentifier key, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessKeyDown(static_cast<Rml::Input::KeyIdentifier>(key), key_modifier);
}

bool rml_context_process_key_up(RmlContext* ctx, RmlKeyIdentifier key, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessKeyUp(static_cast<Rml::Input::KeyIdentifier>(key), key_modifier);
}

bool rml_context_process_text_input_char(RmlContext* ctx, uint32_t character)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTextInput(static_cast<Rml::Character>(character));
}

bool rml_context_process_text_input_ascii(RmlContext* ctx, char character)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTextInput(character);
}

bool rml_context_process_text_input_string(RmlContext* ctx, RmlStringView string)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTextInput(Rml::CApi::ToString(string));
}

bool rml_context_process_mouse_move(RmlContext* ctx, int x, int y, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessMouseMove(x, y, key_modifier);
}

bool rml_context_process_mouse_button_down(RmlContext* ctx, int button_index, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessMouseButtonDown(button_index, key_modifier);
}

bool rml_context_process_mouse_button_up(RmlContext* ctx, int button_index, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessMouseButtonUp(button_index, key_modifier);
}

bool rml_context_process_mouse_wheel(RmlContext* ctx, RmlVector2f wheel_delta, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessMouseWheel(Rml::CApi::FromC(wheel_delta), key_modifier);
}

bool rml_context_process_mouse_leave(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessMouseLeave();
}

bool rml_context_process_touch_move(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTouchMove(BuildTouchList(touches, num_touches), key_modifier);
}

bool rml_context_process_touch_start(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTouchStart(BuildTouchList(touches, num_touches), key_modifier);
}

bool rml_context_process_touch_end(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTouchEnd(BuildTouchList(touches, num_touches), key_modifier);
}

bool rml_context_process_touch_cancel(RmlContext* ctx, const RmlTouch* touches, size_t num_touches)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->ProcessTouchCancel(BuildTouchList(touches, num_touches));
}

bool rml_context_is_mouse_interacting(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->IsMouseInteracting();
}

void rml_context_set_default_scroll_behavior(RmlContext* ctx, RmlScrollBehavior behavior, float speed_factor)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->SetDefaultScrollBehavior(static_cast<Rml::ScrollBehavior>(behavior), speed_factor);
}

RmlTextInputHandler* rml_context_get_text_input_handler(RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetTextInputHandler());
}

RmlDataModelConstructor rml_context_create_data_model(RmlContext* ctx, RmlStringView name, bool allow_missing_variables)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(
		Rml::CApi::FromC(ctx)->CreateDataModel(Rml::CApi::ToString(name), nullptr, allow_missing_variables));
}

RmlDataModelConstructor rml_context_get_data_model(RmlContext* ctx, RmlStringView name)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::ToC(Rml::CApi::FromC(ctx)->GetDataModel(Rml::CApi::ToString(name)));
}

bool rml_context_remove_data_model(RmlContext* ctx, RmlStringView name)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->RemoveDataModel(Rml::CApi::ToString(name));
}

bool rml_data_model_constructor_is_valid(RmlDataModelConstructor constructor)
{
	return static_cast<bool>(Rml::CApi::FromC(constructor));
}

RmlDataModelHandle rml_data_model_get_handle(RmlDataModelConstructor constructor)
{
	const Rml::DataModelConstructor cpp_constructor = Rml::CApi::FromC(constructor);
	return Rml::CApi::ToC(cpp_constructor.GetModelHandle());
}

bool rml_data_model_handle_is_valid(RmlDataModelHandle handle)
{
	return static_cast<bool>(Rml::CApi::FromC(handle));
}

bool rml_data_model_handle_is_variable_dirty(RmlDataModelHandle handle, RmlStringView variable_name)
{
	Rml::DataModelHandle cpp_handle = Rml::CApi::FromC(handle);
	RMLUI_CAPI_REQUIRE(cpp_handle, false);
	return cpp_handle.IsVariableDirty(Rml::CApi::ToString(variable_name));
}

void rml_data_model_handle_dirty_variable(RmlDataModelHandle handle, RmlStringView variable_name)
{
	Rml::DataModelHandle cpp_handle = Rml::CApi::FromC(handle);
	RMLUI_CAPI_REQUIRE_VOID(cpp_handle);
	cpp_handle.DirtyVariable(Rml::CApi::ToString(variable_name));
}

void rml_data_model_handle_dirty_all_variables(RmlDataModelHandle handle)
{
	Rml::DataModelHandle cpp_handle = Rml::CApi::FromC(handle);
	RMLUI_CAPI_REQUIRE_VOID(cpp_handle);
	cpp_handle.DirtyAllVariables();
}

void rml_context_set_documents_base_tag(RmlContext* ctx, RmlStringView tag)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->SetDocumentsBaseTag(Rml::CApi::ToString(tag));
}

RmlStringView rml_context_get_documents_base_tag(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	// Context::GetDocumentsBaseTag is non-const in C++ even though it only reads state.
	return Rml::CApi::ToStringView(const_cast<Rml::Context*>(Rml::CApi::FromC(ctx))->GetDocumentsBaseTag());
}

void rml_context_request_next_update(RmlContext* ctx, double delay)
{
	RMLUI_CAPI_REQUIRE_VOID(ctx);
	Rml::CApi::FromC(ctx)->RequestNextUpdate(delay);
}

double rml_context_get_next_update_delay(const RmlContext* ctx)
{
	RMLUI_CAPI_REQUIRE(ctx, {});
	return Rml::CApi::FromC(ctx)->GetNextUpdateDelay();
}