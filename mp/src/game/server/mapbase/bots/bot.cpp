//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors:
// Iván Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bot.h"

#include "in_utils.h"

#include "bot_defs.h"
#include "squad_manager.h"
#include "bot_manager.h"

#include "nav.h"
#include "nav_mesh.h"
#include "nav_area.h"

#include "fmtstr.h"
#include "in_buttons.h"

#include "ai_hint.h"
#include "movehelper_server.h"

#include "datacache/imdlcache.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Commands
//-----------------------------------------------------------------------------

ConVar bot_frozen( "bot_frozen", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_crouch( "bot_crouch", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_jump( "bot_jump", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_flashlight( "bot_flashlight", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_mimic( "bot_mimic", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_aim_player( "bot_aim_player", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_primary_attack( "bot_primary_attack", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_secondary_attack( "bot_secondary_attack", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_tertiary_attack( "bot_tertiary_attack", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_sendcmd( "bot_sendcmd", "", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "Forces bots to send the specified command." );
ConVar bot_team( "bot_team", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "Force all bots created with bot_add to change to the specified team." );

ConVar bot_notarget( "bot_notarget", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_god( "bot_god", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_buddha( "bot_buddha", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_dont_attack( "bot_dont_attack", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_debug( "bot_debug", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_locomotion( "bot_debug_locomotion", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_jump( "bot_debug_jump", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_memory( "bot_debug_memory", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_debug_cmd( "bot_debug_cmd", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_conditions( "bot_debug_conditions", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_desires( "bot_debug_desires", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );
ConVar bot_debug_max_msgs( "bot_debug_max_msgs", "10", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_optimize( "bot_optimize", "0", FCVAR_ADMIN_ONLY | FCVAR_CHEAT | FCVAR_NOTIFY, "" );

ConVar bot_far_distance( "bot_far_distance", "2500", FCVAR_SERVER | FCVAR_NOTIFY, "" );

//-----------------------------------------------------------------------------
// Macros
//-----------------------------------------------------------------------------

// Only a utility macro to check if the current state is finished
#define STATE_FINISHED m_iStateTimer.HasStarted() && m_iStateTimer.IsElapsed()

//-----------------------------------------------------------------------------
// Purpose: It allows to create a bot with the name and position specified.
//			This is a sniped on how any subclass's implementation should
//			looks like.
//-----------------------------------------------------------------------------

#ifndef MAPBASE_MP
CPlayer* CreateBot( const char* pPlayername, const Vector* vecPosition, const QAngle* angles )
{
	if( !pPlayername )
	{
		pPlayername = m_botNames[RandomInt( 0, ARRAYSIZE( m_botNames ) - 1 )];
		pPlayername = UTIL_VarArgs( "%s Bot", pPlayername );
	}

	edict_t* pSoul = engine->CreateFakeClient( pPlayername );
	Assert( pSoul );

	if( !pSoul )
	{
		Warning( "There was a problem creating a bot. Maybe there is no more space for players on the server." );
		return NULL;
	}

	CPlayer* pPlayer = ( CPlayer* )CBaseEntity::Instance( pSoul );
	Assert( pPlayer );

	pPlayer->ClearFlags();
	pPlayer->AddFlag( FL_CLIENT | FL_FAKECLIENT );

	// This is where we implement the Artificial Intelligence.
	pPlayer->SetUpBot();
	Assert( pPlayer->GetBotController() );

	if( !pPlayer->GetBotController() )
	{
		Warning( "There was a problem creating a bot. The player was created but the controller could not be created." );
		return NULL;
	}

	pPlayer->Spawn();

	if( vecPosition )
	{
		pPlayer->Teleport( vecPosition, angles, NULL );
	}

	++g_botID;
	return pPlayer;
}
#endif // !MAPBASE_MP

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBot::CBot( CBasePlayer* parent ) : BaseClass( parent )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::Spawn()
{
	Assert( GetProfile() );

	m_nComponents.Purge();
	m_nSchedules.Purge();

	SetUpComponents();
	SetUpSchedules();

	FOR_EACH_COMPONENT
	{
		m_nComponents[it]->Reset();
	}

	m_cmd = NULL;
	m_lastCmd = NULL;

	m_iState = STATE_IDLE;
	m_iTacticalMode = TACTICAL_MODE_NONE;
	m_iStateTimer.Invalidate();

	m_nActiveSchedule = NULL;
	m_nConditions.ClearAll();

	m_iRepeatedDamageTimes = 0;
	m_flDamageAccumulated = 0.0f;

	if( GetMemory() )
	{
		GetMemory()->UpdateDataMemory( MEMORY_SPAWN_POSITION, GetAbsOrigin() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called every frame
//-----------------------------------------------------------------------------
void CBot::Update()
{
	VPROF_BUDGET( "Update", VPROF_BUDGETGROUP_BOTS );

	if( STATE_FINISHED )
	{
		CleanState();
	}

	// TODO: FIXME
	if( bot_mimic.GetInt() > 0 )
	{
		MimicThink( bot_mimic.GetInt() );
		return;
	}

	m_cmd = new CUserCmd();
	m_cmd->viewangles = GetHost()->EyeAngles();

	Upkeep();

	if( CanRunAI() )
	{
		RunAI();
	}

	PlayerMove( m_cmd );
}

//-----------------------------------------------------------------------------
// Purpose: Simulates all input as if it were a player
//-----------------------------------------------------------------------------
void CBot::PlayerMove( CUserCmd* cmd )
{
	VPROF_BUDGET( "PlayerMove", VPROF_BUDGETGROUP_BOTS );

	m_lastCmd = m_cmd;

	// This is not necessary if the player is a human
	if( !GetHost()->IsBot() )
	{
		return;
	}

	GetHost()->RemoveEffects( EF_NOINTERP );

	// Save off the CUserCmd to execute later
	GetHost()->ProcessUsercmds( cmd, 1, 1, 0, false );
}

//-----------------------------------------------------------------------------
// Purpose: Returns if we can process the AI
//-----------------------------------------------------------------------------
bool CBot::CanRunAI()
{
	if( bot_frozen.GetBool() )
	{
		return false;
	}

	if( !GetHost()->IsAlive() )
	{
		return false;
	}

	if( GetHost()->IsMarkedForDeletion() )
	{
		return false;
	}

	if( IsPanicked() )
	{
		return false;
	}

	AssertMsg( GetDecision(), "Bot without decision component!" );

	if( !GetDecision() )
	{
		return false;
	}

	if( GetLocomotion() && TheNavMesh->GetNavAreaCount() == 0 )
	{
		return false;
	}

	if( GetFollow() && GetFollow()->IsFollowingBot() )
	{
		CPlayer* pLeader = ToInPlayer( GetFollow()->GetEntity() );

		if( pLeader && pLeader->GetBotController() )
		{
			return pLeader->GetBotController()->CanRunAI();
		}
	}

	if( ( ( gpGlobals->tickcount + GetHost()->entindex() ) % 2 ) == 0 )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: All the processing that is required and preferably light for the engine.
//-----------------------------------------------------------------------------
void CBot::Upkeep()
{
}

//-----------------------------------------------------------------------------
// Purpose: All the processing that can be heavy for the engine.
//-----------------------------------------------------------------------------
void CBot::RunAI()
{
	m_RunTimer.Start();

	BlockConditions();

	ApplyDebugCommands();

	UpdateComponents( true );

	GatherConditions();

	UnblockConditions();

	UpdateComponents( false );

	UpdateSchedule();

	m_RunTimer.End();

	DebugDisplay();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::UpdateComponents( bool important )
{
	CFastTimer timer;

	FOR_EACH_COMPONENT
	{
		if( important && !m_nComponents[it]->ItsImportant() )
		{
			continue;
		}
		else if( !important && m_nComponents[it]->ItsImportant() )
		{
			continue;
		}

		timer.Start();
		m_nComponents[it]->Update();
		timer.End();
		m_nComponents[it]->SetUpdateCost( timer.GetDuration().GetMillisecondsF() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Applies actions for debugging
//-----------------------------------------------------------------------------
void CBot::ApplyDebugCommands()
{
	// God mode
	if( bot_god.GetBool() )
	{
		GetHost()->AddFlag( FL_GODMODE );
	}
	else
	{
		GetHost()->RemoveFlag( FL_GODMODE );
	}

	// Enemies can not see us
	if( bot_notarget.GetBool() )
	{
		GetHost()->AddFlag( FL_NOTARGET );
	}
	else
	{
		GetHost()->RemoveFlag( FL_NOTARGET );
	}

	// Buddha
	if( bot_buddha.GetBool() )
	{
		GetHost()->m_debugOverlays = GetHost()->m_debugOverlays | OVERLAY_BUDDHA_MODE;
	}
	else
	{
		GetHost()->m_debugOverlays = GetHost()->m_debugOverlays & ~OVERLAY_BUDDHA_MODE;
	}

	// Forced Crouch
	if( bot_crouch.GetBool() )
	{
		InjectButton( IN_DUCK );
	}

	// Forced Jumping
	if( bot_jump.GetBool() )
	{
		InjectButton( IN_JUMP );
	}

	// Forced primary attack
	if( bot_primary_attack.GetBool() )
	{
		InjectButton( IN_ATTACK );
	}

	// Forced secondary attack
	if( bot_secondary_attack.GetBool() )
	{
		InjectButton( IN_ATTACK2 );
	}

	// Forced tertiary attack
	if( bot_tertiary_attack.GetBool() )
	{
		InjectButton( IN_ATTACK3 );
	}

	// Forced flashlight
	if( bot_flashlight.GetBool() )
	{
		if( !GetHost()->FlashlightIsOn() )
		{
			GetHost()->FlashlightTurnOn();
		}
	}
	else
	{
		if( GetHost()->FlashlightIsOn() )
		{
			GetHost()->FlashlightTurnOff();
		}
	}

	if( GetVision() )
	{
		// We aim at the host
		if( bot_aim_player.GetBool() )
		{
			CBasePlayer* pPlayer = UTIL_GetListenServerHost();

			if( pPlayer )
			{
				GetVision()->LookAt( "bot_aim_player", pPlayer->EyePosition(), PRIORITY_UNINTERRUPTABLE, 1.0f );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Imitates all inputs of the specified player.
//			TODO: It does not work! It is easy to fix,
//			but I want to give priority to other parts of the code
//-----------------------------------------------------------------------------
void CBot::MimicThink( int playerIndex )
{
	CBasePlayer* pPlayer = UTIL_PlayerByIndex( playerIndex );

	// Ensure the player is valid.
	if( !pPlayer )
	{
		return;
	}

	// No Input = No Mimic
	if( !pPlayer->GetLastUserCommand() )
	{
		return;
	}

	m_flDebugYPosition = 0.34f;

	DebugDisplay();

	const CUserCmd* playercmd = pPlayer->GetLastUserCommand();
	m_cmd = new CUserCmd();

	m_cmd->command_number = playercmd->command_number;
	m_cmd->tick_count = playercmd->tick_count;
	m_cmd->viewangles = playercmd->viewangles;
	m_cmd->forwardmove = playercmd->forwardmove;
	m_cmd->sidemove = playercmd->sidemove;
	m_cmd->upmove = playercmd->upmove;
	m_cmd->buttons = playercmd->buttons;
	m_cmd->impulse = playercmd->impulse;
	m_cmd->weaponselect = playercmd->weaponselect;
	m_cmd->weaponsubtype = playercmd->weaponsubtype;
	m_cmd->random_seed = playercmd->random_seed;
	m_cmd->mousedx = playercmd->mousedx;
	m_cmd->mousedy = playercmd->mousedy;

	CFmtStr msg;
	DebugScreenText( msg.sprintf( "command_number: %i", GetUserCommand()->command_number ) );
	DebugScreenText( msg.sprintf( "tick_count: %i", GetUserCommand()->tick_count ) );
	DebugScreenText( msg.sprintf( "viewangles: %.2f, %.2f", GetUserCommand()->viewangles.x, GetUserCommand()->viewangles.y ) );
	DebugScreenText( msg.sprintf( "forwardmove: %.2f", GetUserCommand()->forwardmove ) );
	DebugScreenText( msg.sprintf( "sidemove: %.2f", GetUserCommand()->sidemove ) );
	DebugScreenText( msg.sprintf( "upmove: %.2f", GetUserCommand()->upmove ) );
	DebugScreenText( msg.sprintf( "buttons:%i", ( int )GetUserCommand()->buttons ) );
	DebugScreenText( msg.sprintf( "impulse: %i", ( int )GetUserCommand()->impulse ) );
	DebugScreenText( msg.sprintf( "weaponselect: %i", GetUserCommand()->weaponselect ) );
	DebugScreenText( msg.sprintf( "weaponsubtype: %i", GetUserCommand()->weaponsubtype ) );
	DebugScreenText( msg.sprintf( "random_seed: %i", GetUserCommand()->random_seed ) );
	DebugScreenText( msg.sprintf( "mousedx: %i", ( int )GetUserCommand()->mousedx ) );
	DebugScreenText( msg.sprintf( "mousedy: %i", ( int )GetUserCommand()->mousedy ) );
	DebugScreenText( msg.sprintf( "hasbeenpredicted: %i", ( int )GetUserCommand()->hasbeenpredicted ) );

	//RunPlayerMove( m_cmd );
}

//-----------------------------------------------------------------------------
// Purpose: Kick the player
//-----------------------------------------------------------------------------
void CBot::Kick()
{
	engine->ServerCommand( UTIL_VarArgs( "kickid %i\n", GetHost()->GetPlayerInfo()->GetUserID() ) );
}

//-----------------------------------------------------------------------------
// Purpose: Move the bot to the desired direction by
//			injecting the command during the current frame.
//-----------------------------------------------------------------------------
void CBot::InjectMovement( NavRelativeDirType direction )
{
	if( !GetUserCommand() )
	{
		return;
	}

	switch( direction )
	{
		case FORWARD:
		default:
			GetUserCommand()->forwardmove = 450.0f;
			InjectButton( IN_FORWARD );
			break;

		case UP:
			GetUserCommand()->upmove = 450.0f;
			break;

		case DOWN:
			GetUserCommand()->upmove = -450.0f;
			break;

		case BACKWARD:
			GetUserCommand()->forwardmove = -450.0f;
			InjectButton( IN_BACK );
			break;

		case LEFT:
			GetUserCommand()->sidemove = -450.0f;
			InjectButton( IN_LEFT );
			break;

		case RIGHT:
			GetUserCommand()->sidemove = 450.0f;
			InjectButton( IN_RIGHT );
			break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Injects the command to have pressed a button during the current frame.
//-----------------------------------------------------------------------------
void CBot::InjectButton( int btn )
{
	if( !GetUserCommand() )
	{
		return;
	}

	GetUserCommand()->buttons |= btn;
}

//-----------------------------------------------------------------------------
// Purpose: It allows the Bot to own another player.
//			That is, the control is exchanged.
//			TODO: Implement it.
//-----------------------------------------------------------------------------
void CBot::Possess( CPlayer* pOther )
{
	Assert( "Possess() Isn't implemented yet!" );
}

//-----------------------------------------------------------------------------
// Purpose: Returns if the host is watching us in spectator mode.
//			It should only be used to debug.
//-----------------------------------------------------------------------------
bool CBot::IsLocalPlayerWatchingMe()
{
	if( engine->IsDedicatedServer() )
	{
		return false;
	}

	CBasePlayer* pPlayer = UTIL_GetListenServerHost();

	if( !pPlayer )
	{
		return false;
	}

	if( pPlayer->IsAlive() || !pPlayer->IsObserver() )
	{
		return false;
	}

	if( pPlayer->GetObserverMode() != OBS_MODE_IN_EYE && pPlayer->GetObserverMode() != OBS_MODE_CHASE )
	{
		return false;
	}

	if( pPlayer->GetObserverTarget() != GetHost() )
	{
		return false;
	}

	return true;
}

//================================================================================
#ifndef MAPBASE_MP
CON_COMMAND_F( bot_add, "Adds a specified number of generic bots", FCVAR_SERVER )
{
	// Look at -count.
	int count = args.FindArgInt( "-count", 1 );
	count = clamp( count, 1, 16 );

	// Ok, spawn all the bots.
	while( --count >= 0 )
	{
		CPlayer* pPlayer = CreateBot( NULL, NULL, NULL );
		Assert( pPlayer );

		if( pPlayer )
		{
			if( bot_team.GetInt() > 0 )
			{
				pPlayer->ChangeTeam( bot_team.GetInt() );
			}
		}
	}
}
#endif // !MAPBASE_MP

CON_COMMAND_F( bot_kick, "Kick all bots on the server", FCVAR_SERVER )
{
	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer || !pPlayer->IsAlive() )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		pPlayer->GetBotController()->Kick();
	}
}

CON_COMMAND_F( bot_debug_follow, "It causes all Bots to start following the host", FCVAR_SERVER )
{
	CPlayer* pOwner = ToInPlayer( CBasePlayer::Instance( UTIL_GetCommandClientIndex() ) );

	if( !pOwner )
	{
		return;
	}

	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer || !pPlayer->IsAlive() )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		IBot* pBot = pPlayer->GetBotController();

		if( pBot->GetFollow() )
		{
			pBot->GetFollow()->Start( pOwner );
		}
	}
}

CON_COMMAND_F( bot_debug_stop_follow, "Causes all Bots to stop following", FCVAR_SERVER )
{
	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer || !pPlayer->IsAlive() )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		IBot* pBot = pPlayer->GetBotController();

		if( pBot->GetFollow() )
		{
			pBot->GetFollow()->Stop();
		}
	}
}

CON_COMMAND_F( bot_debug_drive_random, "Orders all bots to move at random sites", FCVAR_SERVER )
{
	if( TheNavAreas.Count() == 0 )
	{
		return;
	}

	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer || !pPlayer->IsAlive() )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		IBot* pBot = pPlayer->GetBotController();

		if( !pBot->GetLocomotion() )
		{
			continue;
		}

		Vector vecFrom( pPlayer->GetAbsOrigin() );
		CNavArea* pArea = NULL;

		while( true )
		{
			pArea = TheNavAreas[RandomInt( 0, TheNavAreas.Count() - 1 )];

			if( pArea == NULL )
			{
				continue;
			}

			Vector vecGoal( pArea->GetCenter() );

			if( !pBot->GetLocomotion()->IsTraversable( vecFrom, vecGoal ) )
			{
				continue;
			}

			pBot->GetLocomotion()->DriveTo( "bot_debug_drive_random", pArea );
			break;
		}
	}
}

CON_COMMAND_F( bot_debug_drive_player, "Command all bots to move to host location", FCVAR_SERVER )
{
	CPlayer* pOwner = ToInPlayer( CBasePlayer::Instance( UTIL_GetCommandClientIndex() ) );

	if( !pOwner )
	{
		return;
	}

	CNavArea* pArea = pOwner->GetLastKnownArea();

	if( !pArea )
	{
		return;
	}

	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer || !pPlayer->IsAlive() )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		IBot* pBot = pPlayer->GetBotController();

		if( !pBot->GetLocomotion() )
		{
			continue;
		}

		pBot->GetLocomotion()->DriveTo( "bot_debug_drive_player", pArea );
	}
}

/*
CON_COMMAND_F( bot_possess, "", FCVAR_SERVER )
{
	CPlayer *pOwner = ToInPlayer(CBasePlayer::Instance(UTIL_GetCommandClientIndex()));

	if ( !pOwner )
		return;

	if ( args.ArgC() != 2 )
	{
		Warning( "bot_possess <client index>\n" );
		return;
	}

	int iBotClient = atoi( args[1] );
	int iBotEnt = iBotClient + 1;

	if ( iBotClient < 0 ||
		iBotClient >= gpGlobals->maxClients ||
		pOwner->entindex() == iBotEnt )
	{
		Warning( "bot_possess <client index>\n" );
		return;
	}

	CPlayer *pBot = ToInPlayer(CBasePlayer::Instance( iBotEnt ));

	if ( !pBot )
		return;

	if ( !pBot->GetAI() )
		return;

	DevWarning("Possesing %s!! \n", pBot->GetPlayerName());
	pBot->GetAI()->Possess( pOwner );
}*/