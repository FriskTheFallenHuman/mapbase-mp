# gamepadui_hl2.cmake

include( "${CMAKE_CURRENT_LIST_DIR}/gamepadui_base.cmake")

add_library(gamepadui MODULE)

set_target_properties(
	gamepadui PROPERTIES
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)

target_use_gamepadui_base(gamepadui)