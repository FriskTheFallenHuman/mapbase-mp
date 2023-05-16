# vvis_launcher.cmake

set(VVIS_LAUNCHER_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	VVIS_LAUNCHER_SOURCE_FILES

	"${VVIS_LAUNCHER_DIR}/vvis_launcher.cpp"
	"$<${IS_WINDOWS}:${VVIS_LAUNCHER_DIR}/StdAfx.cpp>"

	# Header Files
	"${SRCDIR}/public/tier1/interface.h"
	"${VVIS_LAUNCHER_DIR}/StdAfx.h"
)

add_executable(vvis_launcher ${VVIS_LAUNCHER_SOURCE_FILES})

set_property(TARGET vvis_launcher PROPERTY FOLDER "Tools//Map Compilers")

set_target_properties(
	vvis_launcher PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${GAMEDIR}/bin"
)

target_precompile_headers(
	vvis_launcher PRIVATE
	"${VVIS_LAUNCHER_DIR}/StdAfx.h"
)

target_include_directories(
	vvis_launcher PRIVATE
	"${SRCDIR}/utils/common"
)

if( ${IS_WINDOWS} )
target_link_options(
	vvis_launcher PRIVATE
	/LARGEADDRESSAWARE
)
endif()

target_link_libraries(
	vvis_launcher PRIVATE
)