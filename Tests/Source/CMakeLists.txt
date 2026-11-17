add_subdirectory("Common")

add_subdirectory("Benchmarks")
add_subdirectory("UnitTests")
add_subdirectory("VisualTests")

if(RMLUI_CAPI)
	add_subdirectory("CApi")
endif()
