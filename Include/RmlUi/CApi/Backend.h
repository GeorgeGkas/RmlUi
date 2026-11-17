#ifndef RMLUI_CAPI_BACKEND_H
#define RMLUI_CAPI_BACKEND_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RMLUI_CAPI_BACKEND_API
	#if defined(RMLUI_STATIC_LIB)
		#define RMLUI_CAPI_BACKEND_API
	#elif defined(_WIN32)
		#if defined(RMLUI_CAPI_BACKEND_EXPORTS)
			#define RMLUI_CAPI_BACKEND_API __declspec(dllexport)
		#else
			#define RMLUI_CAPI_BACKEND_API __declspec(dllimport)
		#endif
	#else
		#define RMLUI_CAPI_BACKEND_API __attribute__((visibility("default")))
	#endif
#endif

typedef bool (*RmlBackendKeyDownFunc)(void* user_data, RmlContext* ctx, RmlKeyIdentifier key, int key_modifier, float native_dp_ratio, bool priority);

RMLUI_CAPI_BACKEND_API bool rml_backend_initialize(RmlStringView window_name, int width, int height, bool allow_resize);
RMLUI_CAPI_BACKEND_API void rml_backend_shutdown(void);
RMLUI_CAPI_BACKEND_API RmlSystemInterface* rml_backend_get_system_interface(void);
RMLUI_CAPI_BACKEND_API RmlRenderInterface* rml_backend_get_render_interface(void);
RMLUI_CAPI_BACKEND_API bool rml_backend_process_events(RmlContext* ctx, RmlBackendKeyDownFunc key_down_callback, void* user_data, bool power_save);
RMLUI_CAPI_BACKEND_API void rml_backend_request_exit(void);
RMLUI_CAPI_BACKEND_API void rml_backend_begin_frame(void);
RMLUI_CAPI_BACKEND_API void rml_backend_present_frame(void);
RMLUI_CAPI_BACKEND_API RmlStringView rml_backend_get_name(void);

#ifdef __cplusplus
}
#endif

#endif
