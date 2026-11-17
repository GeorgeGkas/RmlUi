#ifndef RMLUI_CAPI_LOG_H
#define RMLUI_CAPI_LOG_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API void rml_log_message(RmlLogType type, RmlStringView message);

#ifdef __cplusplus
}
#endif

#endif