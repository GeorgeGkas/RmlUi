#ifndef RMLUI_CAPI_DEBUGGER_H
#define RMLUI_CAPI_DEBUGGER_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API bool rml_debugger_initialise(RmlContext* ctx);
RMLUI_CAPI_API void rml_debugger_shutdown(void);
RMLUI_CAPI_API bool rml_debugger_set_context(RmlContext* ctx);
RMLUI_CAPI_API void rml_debugger_set_visible(bool visible);
RMLUI_CAPI_API bool rml_debugger_is_visible(void);

#ifdef __cplusplus
}
#endif

#endif