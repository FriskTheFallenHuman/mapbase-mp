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
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.h>"
	"${SRCDIR}/game/shared/mapbase/mapbase_mountcontent.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_mountcontent.h"
	"${SRCDIR}/game/shared/mapbase/mapbase_usermessages.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_rpc.cpp"
	"${SRCDIR}/game/shared/mapbase/mapbase_game_log.cpp"
	"${SRCDIR}/game/shared/mapbase/MapEdit.cpp"
	"${SRCDIR}/game/shared/mapbase/MapEdit.h"
	"${SRCDIR}/game/shared/mapbase/matchers.cpp"
	"${SRCDIR}/game/shared/mapbase/matchers.h"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/singleplayer_animstate.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/singleplayer_animstate.h>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_funcs_shared.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_funcs_shared.h>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_singletons.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_singletons.h>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_funcs_hl2.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_consts_shared.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/vscript_consts_weapons.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/weapon_custom_scripted.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/weapon_custom_scripted.h>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${SRCDIR}/game/shared/mapbase/logic_script_client.cpp>"

	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.cpp>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.h>"
	"$<$<BOOL:${MAPBASE_VSCRIPT}>:${CLIENT_MAPBASE_DIR}/mapbase/vscript_vgui.nut>"

	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_clientclip.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_fake_worldportal.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_func_fake_worldportal.h"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_point_glow.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/c_vgui_text_display.cpp"
	"${CLIENT_MAPBASE_DIR}/mapbase/mapbase_autocubemap.cpp"

	# HL2 Client->Weapons
	# Original stunstick files are conditional'd out in the HL2 CMAKE files
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_stunstick.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_stunstick.h>"

	# This guns where ported from SP to MP
	# HL2MP->Weapons
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_alyxgun.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_alyxgun.h>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_annabelle.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_ar1.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_bugbait.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_cguard.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.h>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_flaregun.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_flaregun.h>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_immolator.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_molotov.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_molotov.h>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_smg2.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_sniperrifle.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_tripwire.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_tripwire.h>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_slam.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_slam.h>"
)

function(target_use_client_mapbase_features target)
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
		$<$<BOOL:${MAPBASE_VSCRIPT}>:MAPBASE_VSCRIPT>
		$<$<BOOL:${MAPBASE_RESPONSE_SYSTEM}>:NEW_RESPONSE_SYSTEM>
	)

	target_link_libraries(
		${target} PRIVATE
		vscript
		raytrace
		responserules
	)
endfunction()