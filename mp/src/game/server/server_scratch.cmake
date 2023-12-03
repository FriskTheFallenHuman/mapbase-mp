# server_scratch.cmake

include("${CMAKE_CURRENT_LIST_DIR}/server_base.cmake")

if (${IS_SOURCESDK})
	include("${CMAKE_CURRENT_LIST_DIR}/nav_mesh.cmake")
endif()

set(SERVER_SCRATCH_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	SERVER_SCRATCH_SOURCE_FILES

	"${SERVER_SCRATCH_DIR}/ai_relationship.cpp"
	"${SERVER_SCRATCH_DIR}/basegrenade_concussion.cpp"
	"${SERVER_SCRATCH_DIR}/basegrenade_contact.cpp"
	"${SERVER_SCRATCH_DIR}/basegrenade_timed.cpp"
	"${SERVER_SCRATCH_DIR}/EntityFlame.h"
	"${SERVER_SCRATCH_DIR}/hl2/Func_Monitor.cpp"
	"${SERVER_SCRATCH_DIR}/GrenadeThrown.cpp"
	"${SERVER_SCRATCH_DIR}/GrenadeThrown.h"
	"${SERVER_SCRATCH_DIR}/h_cycler.cpp"
	"${SERVER_SCRATCH_DIR}/h_cycler.h"
	"${SERVER_SCRATCH_DIR}/monstermaker.cpp"
	"${SERVER_SCRATCH_DIR}/monstermaker.h"
	"${SERVER_SCRATCH_DIR}/physics_bone_follower.h"
	"${SRCDIR}/game/shared/predicted_viewmodel.cpp"
	"${SRCDIR}/game/shared/predicted_viewmodel.h"
	"${SRCDIR}/game/shared/playerclass_info_parse.cpp"
	"${SRCDIR}/game/shared/playerclass_info_parse.h"
	"${SRCDIR}/game/shared/ragdoll_shared.h"
	"${SRCDIR}/game/shared/solidsetdefaults.h"
	#"${SERVER_SCRATCH_DIR}/team_control_point.cpp"
	#"${SERVER_SCRATCH_DIR}/team_control_point.h"
	#"${SERVER_SCRATCH_DIR}/team_control_point_master.cpp"
	#"${SERVER_SCRATCH_DIR}/team_control_point_master.h"
	#"${SERVER_SCRATCH_DIR}/team_control_point_round.cpp"
	#"${SERVER_SCRATCH_DIR}/team_control_point_round.h"
	#"${SERVER_SCRATCH_DIR}/team_train_watcher.cpp"
	#"${SERVER_SCRATCH_DIR}/team_train_watcher.h"
	"${SERVER_SCRATCH_DIR}/team_objectiveresource.cpp"
	"${SERVER_SCRATCH_DIR}/team_objectiveresource.h"
	#"${SERVER_SCRATCH_DIR}/team_spawnpoint.cpp"
	#"${SERVER_SCRATCH_DIR}/team_spawnpoint.h"
	#"${SRCDIR}/game/shared/teamplay_round_timer.cpp"
	"${SRCDIR}/game/shared/teamplay_round_timer.h"
	"${SRCDIR}/game/shared/teamplayroundbased_gamerules.cpp"
	"${SRCDIR}/game/shared/teamplayroundbased_gamerules.h"
	"${SRCDIR}/game/shared/touchlink.h"
	#"${SERVER_SCRATCH_DIR}/trigger_area_capture.cpp"
	#"${SERVER_SCRATCH_DIR}/trigger_area_capture.h"
	
	# SDK
	"${SERVER_SCRATCH_DIR}/sdk/sdk_client.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_eventlog.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_gameinterface.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_gamemovement.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_shareddefs.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_shareddefs.h"
	"${SRCDIR}/game/shared/sdk/sdk_usermessages.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_vehicle_jeep.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/te_firebullets.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/te_firebullets.h"
	"${SRCDIR}/game/shared/sdk/sdk_player_shared.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.h"	

	# SDK->PerfBots
	#"${SERVER_SCRATCH_DIR}/sdk/sdk_bot_temp.cpp"
	#"${SERVER_SCRATCH_DIR}/sdk/sdk_bot_temp.h"

	# SDK->Player
	"${SERVER_SCRATCH_DIR}/sdk/sdk_player.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_player.h"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_player_resource.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_player_resource.h"
	"${SRCDIR}/game/shared/sdk/sdk_player_shared.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playeranimstate.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playeranimstate.h"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.h"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_playermove.cpp"

	# SDK->Rules
	"${SRCDIR}/game/shared/sdk/sdk_gamerules.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_gamerules.h"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_team.cpp"
	"${SERVER_SCRATCH_DIR}/sdk/sdk_team.h"

	# SDK->Weapons
	"${SRCDIR}/game/shared/sdk/sdk_basegrenade_projectile.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_basegrenade_projectile.h"
	"${SRCDIR}/game/shared/sdk/sdk_fx_shared.cpp"		
	"${SRCDIR}/game/shared/sdk/sdk_fx_shared.h"
	"${SRCDIR}/game/shared/sdk/sdk_weapon_melee.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_weapon_melee.h"
	"${SRCDIR}/game/shared/sdk/sdk_weapon_parse.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_weapon_parse.h"	
	"${SRCDIR}/game/shared/sdk/weapon_basesdkgrenade.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_basesdkgrenade.h"
	"${SRCDIR}/game/shared/sdk/weapon_crowbar.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_grenade.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_grenade.h"
	"${SRCDIR}/game/shared/sdk/weapon_mp5.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_pistol.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_sdkbase.cpp"
	"${SRCDIR}/game/shared/sdk/weapon_sdkbase.h"
	"${SRCDIR}/game/shared/sdk/weapon_shotgun.cpp"

	# Multiplayer
	"${SRCDIR}/game/shared/Multiplayer/multiplayer_animstate.cpp"
	"${SRCDIR}/game/shared/Multiplayer/multiplayer_animstate.h"
)

set(
	SERVER_SCRATCH_EXCLUDE_SOURCES
)

add_library(server_scratch MODULE ${SERVER_SCRATCH_SOURCE_FILES})

set_target_properties(
	server_scratch PROPERTIES
	OUTPUT_NAME "server"
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)

target_use_server_base(server_scratch SERVER_SCRATCH_EXCLUDE_SOURCES)

target_include_directories(
	server_scratch PRIVATE
	"${SRCDIR}/game/shared/sdk"
	"${SERVER_SCRATCH_DIR}/sdk"
	"${SRCDIR}/game/shared/Multiplayer"
)

target_compile_definitions(
	server_scratch PRIVATE
	SDK_DLL
)

if (${IS_SOURCESDK})
	target_use_nav_mesh(server_scratch)
endif()