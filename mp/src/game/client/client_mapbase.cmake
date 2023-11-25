# client_mapbase.cmake

include_guard(GLOBAL)

set(CLIENT_MAPBASE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	CLIENT_MAPBASE_SOURCE_FILES

	# Source Files
	"${CLIENT_MAPBASE_DIR}/c_env_global_light.cpp"
	"${CLIENT_MAPBASE_DIR}/worldlight.cpp"
	"${CLIENT_MAPBASE_DIR}/worldlight.h"
	"${CLIENT_MAPBASE_DIR}/c_baselesson.cpp"
	"${CLIENT_MAPBASE_DIR}/c_baselesson.h"
	"${CLIENT_MAPBASE_DIR}/c_gameinstructor.cpp"
	"${CLIENT_MAPBASE_DIR}/c_gameinstructor.h"
	"${CLIENT_MAPBASE_DIR}/hud_locator_target.cpp"
	"${CLIENT_MAPBASE_DIR}/hud_locator_target.h"
	"${CLIENT_MAPBASE_DIR}/c_postprocesscontroller.cpp"
	"${CLIENT_MAPBASE_DIR}/c_postprocesscontroller.h"
	"${CLIENT_MAPBASE_DIR}/c_env_dof_controller.cpp"
	"${CLIENT_MAPBASE_DIR}/c_movie_display.cpp"
	"${CLIENT_MAPBASE_DIR}/c_movie_display.h"
	"${CLIENT_MAPBASE_DIR}/vgui_movie_display.cpp"
	"${CLIENT_MAPBASE_DIR}/convarproxy.cpp"

	
	# Mapbase
	"${SRCDIR}/game/shared/mapbase/mapbase_shared.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_mountcontent.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_mountcontent.h"
	"${SRCDIR}/game/shared/mapbase/mapbase_usermessages.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_rpc.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_game_log.cpp"
	"${SRCDIR}/game/shared/mapbase/MapEdit.cpp"
	"${SRCDIR}/game/shared/mapbase/MapEdit.h"
	"${SRCDIR}/game/shared/mapbase/matchers.cpp"
	"${SRCDIR}/game/shared/mapbase/matchers.h"
	"${CLIENT_MAPBASE_DIR}/mapbase/languagepreferenceproxy.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_clientclip.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_fake_worldportal.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_fake_worldportal.h"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_point_glow.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_vgui_text_display.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/mapbase_autocubemap.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/shaderapihack.h"
)

function(target_use_client_mapbase_features target)

	# Don't append this files if we are running in MP
	if (NOT ${MAPBASE_MP})
		list(
			APPEND 
			CLIENT_MAPBASE_SOURCE_FILES
			"${SRCDIR}/game/shared/mapbase/singleplayer_animstate.cpp"
			"${SRCDIR}/game/shared/mapbase/singleplayer_animstate.h"

			# Original stunstick files are conditional'd out in the HL2 VPCs
			"${SRCDIR}/game/shared/mapbase/weapon_stunstick.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_stunstick.h"

			"${SRCDIR}/game/shared/mapbase/weapon_slam.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_slam.h"
		)
	endif()

	# If VSCRIPT is enable, we need to include our implementation
	if (${MAPBASE_VSCRIPT})
		list(
			APPEND 
			CLIENT_MAPBASE_SOURCE_FILES
			"${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.cpp"
			"${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.h"
			"${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.nut"
			"${CLIENT_MAPBASE_DIR}/vscript_client.cpp"
			"${CLIENT_MAPBASE_DIR}/vscript_client.h"
			"${CLIENT_MAPBASE_DIR}/vscript_client.h"
			"${SRCDIR}/game/shared/vscript_shared.cpp"
			"${SRCDIR}/game/shared/vscript_shared.h"
			"${SRCDIR}/game/shared/mapbase/vscript_funcs_shared.cpp"
			"${SRCDIR}/game/shared/mapbase/vscript_funcs_shared.h"
			"${SRCDIR}/game/shared/mapbase/vscript_singletons.cpp"
			"${SRCDIR}/game/shared/mapbase/vscript_singletons.h"
			"${SRCDIR}/game/shared/mapbase/vscript_funcs_hl2.cpp"
			"${SRCDIR}/game/shared/mapbase/vscript_consts_shared.cpp"
			"${SRCDIR}/game/shared/mapbase/vscript_consts_weapons.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_custom_scripted.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_custom_scripted.h"
			"${SRCDIR}/game/shared/mapbase/logic_script_client.cpp"
		)
	endif()

	# On Multiplayer we may want to use these instead
	if (${MAPBASE_MP})
		list(
			APPEND 
			CLIENT_MAPBASE_SOURCE_FILES
			"${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.cpp"
			"${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.h"

			# This guns where ported from SP to MP
			# HL2MP->Weapons
			"${SRCDIR}/game/shared/mapbase/weapon_alyxgun.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_alyxgun.h"
			"${SRCDIR}/game/shared/mapbase/weapon_annabelle.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_bugbait.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.h"
		)
	endif()

	target_sources(
		${target} PRIVATE
		${CLIENT_MAPBASE_SOURCE_FILES}
	)

	target_include_directories(
		${target} PRIVATE
		"${CLIENT_MAPBASE_DIR}/mapbase"
		"${SRCDIR}/game/shared/mapbase"
	)

	target_compile_definitions(
		${target} PRIVATE
		ASW_PROJECTED_TEXTURES
		DYNAMIC_RTT_SHADOWS
		GLOWS_ENABLE
		$<$<BOOL:${MAPBASE_RPC}>:MAPBASE_RPC>
		$<$<BOOL:${MAPBASE_RPC}>:DISCORD_RPC>
		$<$<BOOL:${MAPBASE_RPC}>:STEAM_RPC>
		$<$<BOOL:${MAPBASE_SUBTITLES}>:MAPBASE_SUBTITLES>
		$<$<BOOL:${MAPBASE_VSCRIPT}>:MAPBASE_VSCRIPT>
		$<$<BOOL:${MAPBASE_RESPONSE_SYSTEM}>:NEW_RESPONSE_SYSTEM>
	)

	target_link_libraries(
		${target} PRIVATE

		# Discord integration
		"$<$<BOOL:${MAPBASE_RPC}>:${LIBPUBLIC}/discord-rpc${STATIC_LIB_EXT}>"

		vscript
		raytrace
		responserules
	)
endfunction()