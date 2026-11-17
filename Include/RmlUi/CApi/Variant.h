#ifndef RMLUI_CAPI_VARIANT_H
#define RMLUI_CAPI_VARIANT_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

RMLUI_CAPI_API RmlVariant* rml_variant_create(void);
RMLUI_CAPI_API void rml_variant_destroy(RmlVariant* variant);
RMLUI_CAPI_API RmlVariant* rml_variant_copy(const RmlVariant* variant);
RMLUI_CAPI_API void rml_variant_clear(RmlVariant* variant);
RMLUI_CAPI_API RmlVariantType rml_variant_get_type(const RmlVariant* variant);

RMLUI_CAPI_API void rml_variant_set_bool(RmlVariant* variant, bool value);
RMLUI_CAPI_API bool rml_variant_get_bool(const RmlVariant* variant, bool* out);

RMLUI_CAPI_API void rml_variant_set_int(RmlVariant* variant, int value);
RMLUI_CAPI_API bool rml_variant_get_int(const RmlVariant* variant, int* out);

RMLUI_CAPI_API void rml_variant_set_int64(RmlVariant* variant, int64_t value);
RMLUI_CAPI_API bool rml_variant_get_int64(const RmlVariant* variant, int64_t* out);

RMLUI_CAPI_API void rml_variant_set_uint(RmlVariant* variant, unsigned int value);
RMLUI_CAPI_API bool rml_variant_get_uint(const RmlVariant* variant, unsigned int* out);

RMLUI_CAPI_API void rml_variant_set_uint64(RmlVariant* variant, uint64_t value);
RMLUI_CAPI_API bool rml_variant_get_uint64(const RmlVariant* variant, uint64_t* out);

RMLUI_CAPI_API void rml_variant_set_float(RmlVariant* variant, float value);
RMLUI_CAPI_API bool rml_variant_get_float(const RmlVariant* variant, float* out);

RMLUI_CAPI_API void rml_variant_set_double(RmlVariant* variant, double value);
RMLUI_CAPI_API bool rml_variant_get_double(const RmlVariant* variant, double* out);

RMLUI_CAPI_API void rml_variant_set_string(RmlVariant* variant, RmlStringView value);
RMLUI_CAPI_API RmlStringView rml_variant_get_string(const RmlVariant* variant);

RMLUI_CAPI_API void rml_variant_set_vector2f(RmlVariant* variant, RmlVector2f value);
RMLUI_CAPI_API bool rml_variant_get_vector2f(const RmlVariant* variant, RmlVector2f* out);

RMLUI_CAPI_API void rml_variant_set_vector2i(RmlVariant* variant, RmlVector2i value);
RMLUI_CAPI_API bool rml_variant_get_vector2i(const RmlVariant* variant, RmlVector2i* out);

RMLUI_CAPI_API void rml_variant_set_vector3f(RmlVariant* variant, RmlVector3f value);
RMLUI_CAPI_API bool rml_variant_get_vector3f(const RmlVariant* variant, RmlVector3f* out);

RMLUI_CAPI_API void rml_variant_set_vector4f(RmlVariant* variant, RmlVector4f value);
RMLUI_CAPI_API bool rml_variant_get_vector4f(const RmlVariant* variant, RmlVector4f* out);

RMLUI_CAPI_API void rml_variant_set_colourb(RmlVariant* variant, RmlColourb value);
RMLUI_CAPI_API bool rml_variant_get_colourb(const RmlVariant* variant, RmlColourb* out);

RMLUI_CAPI_API void rml_variant_set_colourf(RmlVariant* variant, RmlColourf value);
RMLUI_CAPI_API bool rml_variant_get_colourf(const RmlVariant* variant, RmlColourf* out);

RMLUI_CAPI_API RmlDictionary* rml_dictionary_create(void);
RMLUI_CAPI_API void rml_dictionary_destroy(RmlDictionary* dictionary);
RMLUI_CAPI_API void rml_dictionary_set(RmlDictionary* dictionary, RmlStringView key, const RmlVariant* value);
RMLUI_CAPI_API const RmlVariant* rml_dictionary_get(const RmlDictionary* dictionary, RmlStringView key);
RMLUI_CAPI_API size_t rml_dictionary_size(const RmlDictionary* dictionary);

typedef void (*RmlDictionaryForEachFunc)(void* user_data, RmlStringView key, const RmlVariant* value);
RMLUI_CAPI_API void rml_dictionary_for_each(const RmlDictionary* dictionary, RmlDictionaryForEachFunc callback, void* user_data);

RMLUI_CAPI_API size_t rml_variant_list_size(const RmlVariantList* list);
RMLUI_CAPI_API const RmlVariant* rml_variant_list_get(const RmlVariantList* list, size_t index);

RMLUI_CAPI_API RmlElementList* rml_element_list_create(void);
RMLUI_CAPI_API void rml_element_list_destroy(RmlElementList* list);
RMLUI_CAPI_API void rml_element_list_clear(RmlElementList* list);
RMLUI_CAPI_API size_t rml_element_list_size(const RmlElementList* list);
RMLUI_CAPI_API RmlElement* rml_element_list_get(RmlElementList* list, size_t index);

RMLUI_CAPI_API RmlStringList* rml_string_list_create(void);
RMLUI_CAPI_API void rml_string_list_destroy(RmlStringList* list);
RMLUI_CAPI_API size_t rml_string_list_size(const RmlStringList* list);
RMLUI_CAPI_API RmlStringView rml_string_list_get(const RmlStringList* list, size_t index);
RMLUI_CAPI_API void rml_string_list_push(RmlStringList* list, RmlStringView value);

#ifdef __cplusplus
}
#endif

#endif