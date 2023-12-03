# game_shader_dx9_scratch.cmake

include( "${CMAKE_CURRENT_LIST_DIR}/game_shader_dx9_base.cmake")

add_library(game_shader_dx9_scratch MODULE)

set_target_properties(
	game_shader_dx9_scratch PROPERTIES
	OUTPUT_NAME "game_shader_dx9"
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)

target_use_game_shader_dx9_base(game_shader_dx9_scratch)