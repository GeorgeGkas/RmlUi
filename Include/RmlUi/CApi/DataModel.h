#ifndef RMLUI_CAPI_DATA_MODEL_H
#define RMLUI_CAPI_DATA_MODEL_H

#include "RmlUi/CApi/Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
    Direct scalar bindings. RmlUi reads and writes the caller's memory.
    The pointed-to value must stay valid until the data model is removed.
 */
RMLUI_CAPI_API bool rml_data_model_bind_bool(RmlDataModelConstructor constructor, RmlStringView name, bool* ptr);
RMLUI_CAPI_API bool rml_data_model_bind_int(RmlDataModelConstructor constructor, RmlStringView name, int* ptr);
RMLUI_CAPI_API bool rml_data_model_bind_int64(RmlDataModelConstructor constructor, RmlStringView name, int64_t* ptr);
RMLUI_CAPI_API bool rml_data_model_bind_uint64(RmlDataModelConstructor constructor, RmlStringView name, uint64_t* ptr);
RMLUI_CAPI_API bool rml_data_model_bind_float(RmlDataModelConstructor constructor, RmlStringView name, float* ptr);
RMLUI_CAPI_API bool rml_data_model_bind_double(RmlDataModelConstructor constructor, RmlStringView name, double* ptr);

typedef void (*RmlDataModelGetFunc)(void* user_data, RmlVariant* out_value);
typedef void (*RmlDataModelSetFunc)(void* user_data, const RmlVariant* value);

/* set may be NULL for a read-only variable. This is how strings are bound. */
RMLUI_CAPI_API bool rml_data_model_bind_func(RmlDataModelConstructor constructor, RmlStringView name, RmlDataModelGetFunc get,
	RmlDataModelSetFunc set, void* user_data);

typedef bool (*RmlDataTypeGetFunc)(void* user_data, void* ptr, RmlVariant* out_value);
typedef bool (*RmlDataTypeSetFunc)(void* user_data, void* ptr, const RmlVariant* value);
typedef int (*RmlDataTypeSizeFunc)(void* user_data, void* ptr);
typedef RmlDataVariable (*RmlDataTypeChildByIndexFunc)(void* user_data, void* ptr, int index);
typedef RmlDataVariable (*RmlDataTypeChildByNameFunc)(void* user_data, void* ptr, RmlStringView name);
typedef void (*RmlDataTypeMemberNamesFunc)(void* user_data, RmlStringList* out_names);

typedef struct RmlDataTypeDefinitionCallbacks {
	RmlDataTypeGetFunc get;
	RmlDataTypeSetFunc set;
	RmlDataTypeSizeFunc size;
	RmlDataTypeChildByIndexFunc child_by_index;
	RmlDataTypeChildByNameFunc child_by_name;
	RmlDataTypeMemberNamesFunc member_names;
} RmlDataTypeDefinitionCallbacks;

/* Caller owns the definition and must destroy it after every data model that uses it is gone. */
RMLUI_CAPI_API RmlDataTypeDefinition* rml_data_type_definition_create(RmlDataVariableType type,
	const RmlDataTypeDefinitionCallbacks* callbacks, void* user_data);
RMLUI_CAPI_API void rml_data_type_definition_destroy(RmlDataTypeDefinition* definition);

RMLUI_CAPI_API bool rml_data_model_bind_custom(RmlDataModelConstructor constructor, RmlStringView name, RmlDataTypeDefinition* definition,
	void* ptr);

typedef void (*RmlDataModelEventFunc)(void* user_data, RmlDataModelHandle handle, RmlEvent* event, const RmlVariantList* arguments);
RMLUI_CAPI_API bool rml_data_model_bind_event_callback(RmlDataModelConstructor constructor, RmlStringView name, RmlDataModelEventFunc callback,
	void* user_data);

typedef void (*RmlDataModelTransformFunc)(void* user_data, const RmlVariantList* arguments, RmlVariant* out_result);
RMLUI_CAPI_API void rml_data_model_register_transform_func(RmlDataModelConstructor constructor, RmlStringView name,
	RmlDataModelTransformFunc callback, void* user_data);

#ifdef __cplusplus
}
#endif

#endif
