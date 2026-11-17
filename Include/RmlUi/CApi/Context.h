#ifndef RMLUI_CAPI_CONTEXT_H
#define RMLUI_CAPI_CONTEXT_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API RmlStringView rml_context_get_name(const RmlContext* ctx);
RMLUI_CAPI_API void rml_context_set_dimensions(RmlContext* ctx, RmlVector2i dimensions);
RMLUI_CAPI_API RmlVector2i rml_context_get_dimensions(const RmlContext* ctx);
RMLUI_CAPI_API void rml_context_set_dp_ratio(RmlContext* ctx, float dp_ratio);
RMLUI_CAPI_API float rml_context_get_dp_ratio(const RmlContext* ctx);

RMLUI_CAPI_API bool rml_context_update(RmlContext* ctx);
RMLUI_CAPI_API bool rml_context_render(RmlContext* ctx);

RMLUI_CAPI_API RmlElementDocument* rml_context_create_document(RmlContext* ctx, RmlStringView instancer_name);
RMLUI_CAPI_API RmlElementDocument* rml_context_load_document(RmlContext* ctx, RmlStringView path);
RMLUI_CAPI_API RmlElementDocument* rml_context_load_document_from_memory(RmlContext* ctx, RmlStringView rml, RmlStringView source_url);
RMLUI_CAPI_API void rml_context_unload_document(RmlContext* ctx, RmlElementDocument* document);
RMLUI_CAPI_API void rml_context_unload_all_documents(RmlContext* ctx);

RMLUI_CAPI_API void rml_context_enable_mouse_cursor(RmlContext* ctx, bool enable);
RMLUI_CAPI_API void rml_context_activate_theme(RmlContext* ctx, RmlStringView theme_name, bool activate);
RMLUI_CAPI_API bool rml_context_is_theme_active(const RmlContext* ctx, RmlStringView theme_name);

RMLUI_CAPI_API RmlElementDocument* rml_context_get_document_by_id(RmlContext* ctx, RmlStringView id);
RMLUI_CAPI_API RmlElementDocument* rml_context_get_document_by_index(RmlContext* ctx, int index);
RMLUI_CAPI_API int rml_context_get_num_documents(const RmlContext* ctx);

RMLUI_CAPI_API RmlElement* rml_context_get_hover_element(RmlContext* ctx);
RMLUI_CAPI_API RmlElement* rml_context_get_focus_element(RmlContext* ctx);
RMLUI_CAPI_API RmlElement* rml_context_get_root_element(RmlContext* ctx);
RMLUI_CAPI_API RmlElement* rml_context_get_element_at_point(RmlContext* ctx, RmlVector2f point, const RmlElement* ignore_element);

RMLUI_CAPI_API void rml_context_pull_document_to_front(RmlContext* ctx, RmlElementDocument* document);
RMLUI_CAPI_API void rml_context_push_document_to_back(RmlContext* ctx, RmlElementDocument* document);
RMLUI_CAPI_API void rml_context_unfocus_document(RmlContext* ctx, RmlElementDocument* document);

RMLUI_CAPI_API void rml_context_add_event_listener(RmlContext* ctx, RmlStringView event, RmlEventListener* listener, bool in_capture_phase);
RMLUI_CAPI_API void rml_context_remove_event_listener(RmlContext* ctx, RmlStringView event, RmlEventListener* listener, bool in_capture_phase);

RMLUI_CAPI_API bool rml_context_process_key_down(RmlContext* ctx, RmlKeyIdentifier key, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_key_up(RmlContext* ctx, RmlKeyIdentifier key, int key_modifier);

RMLUI_CAPI_API bool rml_context_process_text_input_char(RmlContext* ctx, uint32_t character);
RMLUI_CAPI_API bool rml_context_process_text_input_ascii(RmlContext* ctx, char character);
RMLUI_CAPI_API bool rml_context_process_text_input_string(RmlContext* ctx, RmlStringView string);

RMLUI_CAPI_API bool rml_context_process_mouse_move(RmlContext* ctx, int x, int y, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_mouse_button_down(RmlContext* ctx, int button_index, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_mouse_button_up(RmlContext* ctx, int button_index, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_mouse_wheel(RmlContext* ctx, RmlVector2f wheel_delta, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_mouse_leave(RmlContext* ctx);

RMLUI_CAPI_API bool rml_context_process_touch_move(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_touch_start(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_touch_end(RmlContext* ctx, const RmlTouch* touches, size_t num_touches, int key_modifier);
RMLUI_CAPI_API bool rml_context_process_touch_cancel(RmlContext* ctx, const RmlTouch* touches, size_t num_touches);

RMLUI_CAPI_API bool rml_context_is_mouse_interacting(const RmlContext* ctx);

RMLUI_CAPI_API void rml_context_set_default_scroll_behavior(RmlContext* ctx, RmlScrollBehavior behavior, float speed_factor);

RMLUI_CAPI_API RmlTextInputHandler* rml_context_get_text_input_handler(RmlContext* ctx);

RMLUI_CAPI_API RmlDataModelConstructor rml_context_create_data_model(RmlContext* ctx, RmlStringView name, bool allow_missing_variables);
RMLUI_CAPI_API RmlDataModelConstructor rml_context_get_data_model(RmlContext* ctx, RmlStringView name);
RMLUI_CAPI_API bool rml_context_remove_data_model(RmlContext* ctx, RmlStringView name);

RMLUI_CAPI_API bool rml_data_model_constructor_is_valid(RmlDataModelConstructor constructor);
RMLUI_CAPI_API RmlDataModelHandle rml_data_model_get_handle(RmlDataModelConstructor constructor);

RMLUI_CAPI_API bool rml_data_model_handle_is_variable_dirty(RmlDataModelHandle handle, RmlStringView variable_name);
RMLUI_CAPI_API void rml_data_model_handle_dirty_variable(RmlDataModelHandle handle, RmlStringView variable_name);
RMLUI_CAPI_API void rml_data_model_handle_dirty_all_variables(RmlDataModelHandle handle);

RMLUI_CAPI_API void rml_context_set_documents_base_tag(RmlContext* ctx, RmlStringView tag);
RMLUI_CAPI_API RmlStringView rml_context_get_documents_base_tag(const RmlContext* ctx);

RMLUI_CAPI_API void rml_context_request_next_update(RmlContext* ctx, double delay);
RMLUI_CAPI_API double rml_context_get_next_update_delay(const RmlContext* ctx);

#ifdef __cplusplus
}
#endif

#endif