add_library(rmlui_capi
	Adapters.cpp
	Conversions.cpp
	Core.cpp
	Interfaces.cpp
	Context.cpp
	Element.cpp
	ElementDocument.cpp
	Elements.cpp
	Variant.cpp
	Event.cpp
	DataModel.cpp
	Factory.cpp
	Render.cpp
	StyleSheet.cpp
	Debugger.cpp
	Log.cpp
	Types.cpp
)

target_sources(rmlui_capi PRIVATE
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Header.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Types.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Core.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Interfaces.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Context.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Element.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/ElementDocument.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Elements.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Variant.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Event.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/DataModel.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Factory.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Render.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/StyleSheet.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Debugger.h"
	"${PROJECT_SOURCE_DIR}/Include/RmlUi/CApi/Log.h"
)

set_common_target_options(rmlui_capi)

target_include_directories(rmlui_capi PRIVATE "${PROJECT_SOURCE_DIR}/Include")
target_include_directories(rmlui_capi INTERFACE
	"$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/Include>"
	"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_library(RmlUi::CApi ALIAS rmlui_capi)

target_link_libraries(rmlui_capi PRIVATE rmlui_core rmlui_debugger)

get_target_property(rmlui_capi_TYPE rmlui_capi "TYPE")
if(rmlui_capi_TYPE STREQUAL "SHARED_LIBRARY")
	target_compile_definitions(rmlui_capi PRIVATE "RMLUI_CAPI_EXPORTS")
elseif(rmlui_capi_TYPE STREQUAL "STATIC_LIBRARY")
	# Consumers include CApi headers; without this, RMLUI_CAPI_API becomes dllimport on Windows.
	target_compile_definitions(rmlui_capi PUBLIC "RMLUI_STATIC_LIB")
endif()

set_target_properties(rmlui_capi PROPERTIES
	EXPORT_NAME "CApi"
)

install(TARGETS rmlui_capi
	EXPORT RmlUiTargets
	${RMLUI_RUNTIME_DEPENDENCY_SET_ARG}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install_target_pdb(rmlui_capi)
