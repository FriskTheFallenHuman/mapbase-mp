# server_mapbase.cmake

include_guard(GLOBAL)

set(SERVER_MAPBASE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	SERVER_MAPBASE_SOURCE_FILES

	# Source Files
	"${SERVER_MAPBASE_DIR}/logic_random_outputs.cpp"
	"${SERVER_MAPBASE_DIR}/point_entity_finder.cpp"
	"${SERVER_MAPBASE_DIR}/env_projectedtexture.h"
	"${SERVER_MAPBASE_DIR}/env_global_light.cpp"
	"${SERVER_MAPBASE_DIR}/skyboxswapper.cpp"
	"${SERVER_MAPBASE_DIR}/env_instructor_hint.cpp"
	"${SERVER_MAPBASE_DIR}/postprocesscontroller.cpp"
	"${SERVER_MAPBASE_DIR}/postprocesscontroller.h"
	"${SERVER_MAPBASE_DIR}/env_dof_controller.cpp"
	"${SERVER_MAPBASE_DIR}/env_dof_controller.h"
	"${SERVER_MAPBASE_DIR}/logic_playmovie.cpp"
	"${SERVER_MAPBASE_DIR}/movie_display.cpp"
	"${SERVER_MAPBASE_DIR}/fogvolume.cpp"
	"${SERVER_MAPBASE_DIR}/fogvolume.h"
	"$<$<BOOL:${MAPBASE_RESPONSE_SYSTEM}>:${SERVER_MAPBASE_DIR}/ai_expresserfollowup.cpp>"
	"$<$<BOOL:${MAPBASE_RESPONSE_SYSTEM}>:${SERVER_MAPBASE_DIR}/ai_speechqueue.cpp>"
	"$<$<BOOL:${MAPBASE_RESPONSE_SYSTEM}>:${SERVER_MAPBASE_DIR}/ai_speechqueue.h>"

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
	"${SERVER_MAPBASE_DIR}/mapbase/ai_grenade.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/ai_grenade.h"
	"${SERVER_MAPBASE_DIR}/mapbase/ai_monitor.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/ai_weaponmodifier.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/closecaption_entity.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/datadesc_mod.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/datadesc_mod.h"
	"${SERVER_MAPBASE_DIR}/mapbase/expandedrs_combine.h"
	"${SERVER_MAPBASE_DIR}/mapbase/func_clientclip.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/func_fake_worldportal.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/GlobalStrings.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/GlobalStrings.h"
	"${SERVER_MAPBASE_DIR}/mapbase/logic_externaldata.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/logic_skill.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/logic_substring.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_advanced_finder.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_copy_size.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_damageinfo.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_entity_replace.cpp"
	#"${SERVER_MAPBASE_DIR}/mapbase/point_physics_control.cpp" # Backlogged
	"${SERVER_MAPBASE_DIR}/mapbase/point_projectile.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_radiation_source.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/point_glow.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/SystemConvarMod.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/SystemConvarMod.h"
	"${SERVER_MAPBASE_DIR}/mapbase/variant_tools.h"
	"${SERVER_MAPBASE_DIR}/mapbase/vgui_text_display.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/logic_eventlistener.cpp"
	"${SERVER_MAPBASE_DIR}/mapbase/logic_register_activator.cpp"
	
	# HL2->Point Entities
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/env_flare.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/env_flare.h>"

	# HL2->Weapons
	"$<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2/grenade_molotov.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2/grenade_molotov.h>"
	"$<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2/grenade_tripwire.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2/grenade_tripwire.h>"

	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/concussive_explosion.cpp>"
	"$<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/concussive_explosion.h>"

	# Original stunstick files are conditional'd out in the HL2 VPCs
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_stunstick.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SRCDIR}/game/shared/mapbase/weapon_stunstick.h>"

	# This guns where ported from SP to MP
	# HL2MP->Weapons
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2mp/grenade_satchel.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2mp/grenade_satchel.h>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2mp/grenade_tripmine.cpp>"
	"$<$<NOT:<$<BOOL:${MAPBASE_MP}>:${SERVER_MAPBASE_DIR}/hl2mp/grenade_tripmine.h>"
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

function(target_use_server_mapbase_features target)
	target_sources(
		${target} PRIVATE
		${SERVER_MAPBASE_SOURCE_FILES}
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
		responserules
	)
endfunction()