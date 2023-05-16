# game_shader_dx9_episodic.cmake

include( "${CMAKE_CURRENT_LIST_DIR}/game_shader_dx9_base.cmake")

add_library(game_shader_dx9_episodic MODULE)

set_target_properties(
	game_shader_dx9_episodic PROPERTIES
	OUTPUT_NAME "game_shader_dx9"
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)

target_use_game_shader_dx9_base(game_shader_dx9_episodic)