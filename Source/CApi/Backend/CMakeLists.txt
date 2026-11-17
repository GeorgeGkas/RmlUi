add_library(rmlui_capi_backend
	Backend.cpp
)

target_sources(rmlui_capi_backend PRIVATE
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Backend.h"
)

set_common_target_options(rmlui_capi_backend)

target_include_directories(rmlui_capi_backend PRIVATE "${PROJECT_SOURCE_DIR}/Include")
target_include_directories(rmlui_capi_backend INTERFACE
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_library(RmlUi::CApiBackend ALIAS rmlui_capi_backend)

target_link_libraries(rmlui_capi_backend
	PUBLIC rmlui_capi
	PRIVATE rmlui_core rmlui_backend_${RMLUI_BACKEND}
)

target_compile_definitions(rmlui_capi_backend PRIVATE "RMLUI_CAPI_BACKEND_NAME=\"${RMLUI_BACKEND}\"")

get_target_property(rmlui_capi_backend_TYPE rmlui_capi_backend "TYPE")
if(rmlui_capi_backend_TYPE STREQUAL "SHARED_LIBRARY")
	target_compile_definitions(rmlui_capi_backend PRIVATE "RMLUI_CAPI_BACKEND_EXPORTS")
endif()

set_target_properties(rmlui_capi_backend PROPERTIES
	EXPORT_NAME "CApiBackend"
)
