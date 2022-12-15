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
)

function(target_use_server_mapbase_features target)
	# Do NOT append these files if we use the new system
	if (NOT ${MAPBASE_RESPONSE_SYSTEM})
		list(
			APPEND 
			SERVER_MAPBASE_SOURCE_FILES
			"${SERVER_BASE_DIR}/AI_Criteria.cpp"
			"${SERVER_BASE_DIR}/AI_ResponseSystem.cpp"
			"${SERVER_BASE_DIR}/ai_speech.cpp"
		)
	endif()

	# If we use the New Response System WE need these files
	if (${MAPBASE_RESPONSE_SYSTEM})
		list(
			APPEND 
			SERVER_MAPBASE_SOURCE_FILES
			"${SRCDIR}/game/shared/ai_responsesystem_new.cpp"
			"${SRCDIR}/game/shared/ai_responsesystem_new.h"
			"${SERVER_BASE_DIR}/ai_speech_new.cpp"
			"${SERVER_BASE_DIR}/ai_speech_new.h"
			"${SRCDIR}/game/shared/ai_criteria_new.cpp"
			"${SRCDIR}/game/shared/ai_criteria_new.h"
			"${SERVER_MAPBASE_DIR}/ai_expresserfollowup.cpp"
			"${SERVER_MAPBASE_DIR}/ai_speechqueue.cpp"
			"${SERVER_MAPBASE_DIR}/ai_speechqueue.h"
		)
	endif()

	# Don't append this files if we are running in MP
	if (NOT ${MAPBASE_MP})
		list(
			APPEND 
			SERVER_MAPBASE_SOURCE_FILES
			"${SRCDIR}/game/shared/mapbase/singleplayer_animstate.cpp"
			"${SRCDIR}/game/shared/mapbase/singleplayer_animstate.h"

			"${SERVER_MAPBASE_DIR}/hl2mp/grenade_satchel.cpp"
			"${SERVER_MAPBASE_DIR}/hl2mp/grenade_satchel.h"
			"${SERVER_MAPBASE_DIR}/hl2mp/grenade_tripmine.cpp"
			"${SERVER_MAPBASE_DIR}/hl2mp/grenade_tripmine.h"

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
			SERVER_MAPBASE_SOURCE_FILES
			"${SERVER_MAPBASE_DIR}/vscript_server.cpp"
			"${SERVER_MAPBASE_DIR}/vscript_server.h"
			"${SERVER_MAPBASE_DIR}/vscript_server.h"
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
			SERVER_MAPBASE_SOURCE_FILES
			"${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.cpp"
			"${SRCDIR}/game/shared/mapbase/mapbase_viewmodel.h"

			# HL2->Point Entities
			"${SRCDIR}/game/shared/mapbase/env_flare.cpp"
			"${SRCDIR}/game/shared/mapbase/env_flare.h"

			# HL2->Weapons
			"${SERVER_MAPBASE_DIR}/hl2/grenade_molotov.cpp"
			"${SERVER_MAPBASE_DIR}/hl2/grenade_molotov.h"
			"${SERVER_MAPBASE_DIR}/hl2/grenade_tripwire.cpp"
			"${SERVER_MAPBASE_DIR}/hl2/grenade_tripwire.h"

			"${SRCDIR}/game/shared/mapbase/concussive_explosion.cpp"
			"${SRCDIR}/game/shared/mapbase/concussive_explosion.h"

			# This guns where ported from SP to MP
			# HL2MP->Weapons
			"${SRCDIR}/game/shared/mapbase/weapon_alyxgun.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_alyxgun.h"
			"${SRCDIR}/game/shared/mapbase/weapon_annabelle.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_ar1.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_bugbait.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_cguard.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_citizenpackage.h"
			"${SRCDIR}/game/shared/mapbase/weapon_flaregun.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_flaregun.h"
			"${SRCDIR}/game/shared/mapbase/weapon_immolator.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_molotov.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_molotov.h"
			"${SRCDIR}/game/shared/mapbase/weapon_smg2.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_sniperrifle.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_tripwire.cpp"
			"${SRCDIR}/game/shared/mapbase/weapon_tripwire.h"
		)
	endif()

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