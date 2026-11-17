#ifndef RMLUI_CAPI_HEADER_H
#define RMLUI_CAPI_HEADER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef RMLUI_CAPI_API
	#if defined(RMLUI_STATIC_LIB)
		#define RMLUI_CAPI_API
	#elif defined(_WIN32)
		#if defined(RMLUI_CAPI_EXPORTS)
			#define RMLUI_CAPI_API __declspec(dllexport)
		#else
			#define RMLUI_CAPI_API __declspec(dllimport)
		#endif
	#else
		#define RMLUI_CAPI_API __attribute__((visibility("default")))
	#endif
#endif

#ifdef __cplusplus
}
#endif

#endif