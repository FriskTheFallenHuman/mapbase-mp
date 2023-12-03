# client_scratch.cmake

include("${CMAKE_CURRENT_LIST_DIR}/client_base.cmake")

set(CLIENT_SCRATCH_DIR ${CMAKE_CURRENT_LIST_DIR})

set(
	CLIENT_SCRATCH_SOURCE_FILES

	# Source Files
	"${SRCDIR}/game/shared/basecombatweapon_shared.h"
	"${CLIENT_SCRATCH_DIR}/c_team_objectiveresource.cpp"
	"${CLIENT_SCRATCH_DIR}/c_team_objectiveresource.h"
	#"${CLIENT_SCRATCH_DIR}/c_team_train_watcher.cpp"
	#"${CLIENT_SCRATCH_DIR}/c_team_train_watcher.h"
	#"${CLIENT_SCRATCH_DIR}/hud_base_account.cpp"
	#"${CLIENT_SCRATCH_DIR}/hud_base_account.h"
	"${CLIENT_SCRATCH_DIR}/hud_basedeathnotice.cpp"
	"${CLIENT_SCRATCH_DIR}/hud_basedeathnotice.h"
	#"${CLIENT_SCRATCH_DIR}/hud_controlpointicons.cpp"
	"${CLIENT_SCRATCH_DIR}/hud_voicestatus.cpp"
	"${SRCDIR}/game/shared/predicted_viewmodel.cpp"
	"${SRCDIR}/game/shared/predicted_viewmodel.h"
	#"${CLIENT_SCRATCH_DIR}/hud_vguiscreencursor.cpp"
	"${SRCDIR}/game/shared/playerclass_info_parse.cpp"
	"${SRCDIR}/game/shared/playerclass_info_parse.h"
	#"${SRCDIR}/game/shared/teamplay_round_timer.cpp"
	#"${SRCDIR}/game/shared/teamplay_round_timer.h"
	#"${CLIENT_SCRATCH_DIR}/TeamBitmapImage.cpp"
	#"${CLIENT_SCRATCH_DIR}/voice_menu.cpp"

	# game_controls
	"${CLIENT_SCRATCH_DIR}/game_controls/ClassMenu.cpp"
	
	# SDK
	"${CLIENT_SCRATCH_DIR}/sdk/c_te_firebullets.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/clientmode_sdk.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/clientmode_sdk.h"
	"${CLIENT_SCRATCH_DIR}/sdk/sdk_in_main.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/sdk_prediction.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_shareddefs.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_shareddefs.h"
	"${SRCDIR}/game/shared/sdk/sdk_usermessages.cpp"

	# SDK->GUI
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_backgroundpanel.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_backgroundpanel.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_classmenu.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_classmenu.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_scoreboard.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_scoreboard.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_spectatorgui.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_spectatorgui.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_teammenu.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_teammenu.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_textwindow.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdk_textwindow.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdkviewport.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/sdkviewport.h"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/vgui_rootpanel_sdk.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui/vgui_rootpanel_sdk.h"

	# SDK->Hud Elements
	"${CLIENT_SCRATCH_DIR}/sdk/hud/hud_sdkdeathnotice.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_ammo.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_armor.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_chat.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_chat.h"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_damage.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_health.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_stamina.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_stamina.h"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_targetid.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/hud/sdk_hud_weaponselection.cpp"				

	# SDK->Player
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_player.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_player.h"
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_player_resource.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_player_resource.h"
	"${SRCDIR}/game/shared/sdk/sdk_gamemovement.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_player_shared.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playeranimstate.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playeranimstate.h"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_playerclass_info_parse.h"					

	# SDK->Rules
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_team.cpp"
	"${CLIENT_SCRATCH_DIR}/sdk/c_sdk_team.h"
	"${SRCDIR}/game/shared/sdk/sdk_gamerules.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_gamerules.h"			

	# SDK->Weapons
	"${SRCDIR}/game/shared/sdk/sdk_basegrenade_projectile.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_basegrenade_projectile.h"
	"${CLIENT_SCRATCH_DIR}/sdk/sdk_fx_impacts.cpp"
	"${SRCDIR}/game/shared/sdk/sdk_fx_shared.cpp"		
	"${SRCDIR}/game/shared/sdk/sdk_fx_shared.h"
	"${CLIENT_SCRATCH_DIR}/sdk/sdk_fx_tracers.cpp"	
	"${CLIENT_SCRATCH_DIR}/sdk/sdk_fx_weaponfx.cpp"	
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
	CLIENT_SCRATCH_EXCLUDE_SOURCES
	"${SRCDIR}/game/shared/weapon_parse_default.cpp"
)

add_library(client_scratch MODULE ${CLIENT_SCRATCH_SOURCE_FILES})

set_target_properties(
	client_scratch PROPERTIES
	OUTPUT_NAME "client"
	PREFIX ""
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)

target_use_client_base(client_scratch CLIENT_SCRATCH_EXCLUDE_SOURCES)

target_include_directories(
	client_scratch PRIVATE
	"${CLIENT_SCRATCH_DIR}/sdk/hud"
	"${CLIENT_SCRATCH_DIR}/sdk/vgui"
	"${CLIENT_SCRATCH_DIR}/sdk"
	"${SRCDIR}/game/shared/sdk"
	"${SRCDIR}/game/shared/Multiplayer"
)

target_compile_definitions(
	client_scratch PRIVATE
	SDK_DLL
)