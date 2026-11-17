set(TARGET_NAME "rmlui_capi_tests")

add_executable(${TARGET_NAME}
	main.cpp
	Foundation.cpp
	DomEvents.cpp
	DataModel.cpp
	Extensions.cpp
	ReviewFixes.cpp
)

# Pure C compilation check: CApi headers must be valid C99.
add_library(rmlui_capi_header_compile OBJECT HeaderCompile.c)
set_target_properties(rmlui_capi_header_compile PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)
target_include_directories(rmlui_capi_header_compile PRIVATE "${PROJECT_SOURCE_DIR}/Include")
if(MSVC)
	target_compile_options(rmlui_capi_header_compile PRIVATE /W4 /WX)
else()
	target_compile_options(rmlui_capi_header_compile PRIVATE -Wall -Wextra -Werror)
endif()
add_dependencies(${TARGET_NAME} rmlui_capi_header_compile)

set_common_target_options(${TARGET_NAME})

target_link_libraries(${TARGET_NAME} PRIVATE
	rmlui_capi
	doctest::doctest
)

if(RMLUI_CAPI_BACKEND)
	target_link_libraries(${TARGET_NAME} PRIVATE rmlui_capi_backend)
	target_sources(${TARGET_NAME} PRIVATE BackendName.cpp)
	target_compile_definitions(${TARGET_NAME} PRIVATE "RMLUI_CAPI_BACKEND_EXPECTED=\"${RMLUI_BACKEND}\"")
endif()

target_compile_definitions(${TARGET_NAME} PRIVATE
	"RMLUI_CAPI_TESTS_SAMPLES_ASSETS=\"${PROJECT_SOURCE_DIR}/Samples/assets\""
)

if(NOT EMSCRIPTEN)
	doctest_discover_tests(${TARGET_NAME})
endif()
