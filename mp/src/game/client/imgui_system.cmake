# imgui_system.cmake

include_guard(GLOBAL)

set(IMGUI_DIR ${CMAKE_CURRENT_LIST_DIR})

set(
	IMGUI_SOURCE_FILES

	# Source Files
	"${IMGUI_DIR}/imgui/imgui_impl_source.cpp"
	"${IMGUI_DIR}/imgui/imgui_system.cpp"
	
	# Header Files
	"${IMGUI_DIR}/imgui/imconfig_source.h"
	"${IMGUI_DIR}/imgui/imgui_impl_source.h"
	"${IMGUI_DIR}/imgui/imgui_system.h"
	"${IMGUI_DIR}/imgui/imgui_window.h"
	
	# Source Files
	"${SRCDIR}/thirdparty/imgui/imgui.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_demo.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_draw.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_tables.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_widgets.cpp"
)

set_source_files_properties(
	"${SRCDIR}/thirdparty/imgui/imgui.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_demo.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_draw.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_tables.cpp"
	"${SRCDIR}/thirdparty/imgui/imgui_widgets.cpp"

	PROPERTIES SKIP_PRECOMPILE_HEADERS ON
)

function(target_use_imgui target)

	target_sources(
		${target} PRIVATE
		${IMGUI_SOURCE_FILES}
	)

	target_include_directories(
		${target} PRIVATE
		"${IMGUI_DIR}/imgui"
		"${SRCDIR}/thirdparty/imgui"
	)

	target_compile_definitions(
		${target} PRIVATE
		IMGUI_DISABLE_INCLUDE_IMCONFIG_H
		IMGUI_USER_CONFIG="${IMGUI_DIR}/imgui/imconfig_source.h"
	)
endfunction()
