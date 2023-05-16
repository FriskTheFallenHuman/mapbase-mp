#include "cbase.h"
#include "mapbase_bot.h"
#include "bot_manager.h"
#include "in_utils.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar bot_closestdistancetoplayer( "bot_closestdistancetoplayer", "1800", FCVAR_CHEAT | FCVAR_NOTIFY, "Distance that this bot would get closest to." );
ConVar bot_respawn( "bot_respawn", "1", FCVAR_CHEAT | FCVAR_NOTIFY, "Should this bot respawn after death." );
ConVar bot_ai( "bot_ai", "1", FCVAR_NOTIFY, "Spawns this bot with no ai." );
ConVar bot_tag( "bot_tag", "0", FCVAR_NOTIFY, "Should bots use its respective BOT tag?" );

extern void respawn( CBaseEntity* pEdict, bool fCopyCorpse );

CUtlVector<string_t> m_botScriptNames;

//-----------------------------------------------------------------------------
// Purpose: Load the bot names from the .txt file
//-----------------------------------------------------------------------------
void LoadBotNames( void )
{
	m_botScriptNames.RemoveAll();

	KeyValues* pKV = new KeyValues( "BotNames" );
	if( pKV->LoadFromFile( filesystem, "scripts/bot_names.txt", "GAME" ) )
	{
		FOR_EACH_VALUE( pKV, pSubData )
		{
			if( FStrEq( pSubData->GetString(), "" ) )
			{
				continue;
			}

			string_t iName = AllocPooledString( pSubData->GetString() );
			if( m_botScriptNames.Find( iName ) == m_botScriptNames.InvalidIndex() )
			{
				m_botScriptNames[m_botScriptNames.AddToTail()] = iName;
			}
		}
	}

	pKV->deleteThis();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char* NewNameSelection( void )
{
	if( m_botScriptNames.Count() == 0 )
	{
		return "MISSINGNO";
	}

	int nPoneNames = m_botScriptNames.Count();
	int randomChoice = rand() % nPoneNames;
	string_t iszName = m_botScriptNames[randomChoice];
	const char* pszName = STRING( iszName );

	return pszName;
}

//-----------------------------------------------------------------------------
// Purpose: It allows to create a bot with the name and position specified.
//			Uses our custom name algorithm.
//-----------------------------------------------------------------------------
CPlayer* CreateGameBot( const char* pPlayername, const Vector* vecPosition, const QAngle* angles )
{
	LoadBotNames();

	if( !pPlayername )
	{
		const char* pPlayername1 = NewNameSelection();
		const char* pBotNameFormat = bot_tag.GetBool() ? "[BOT] %s" : "%s";
		char combinedName[MAX_PLAYER_NAME_LENGTH];
		Q_snprintf( combinedName, sizeof( combinedName ), pBotNameFormat, pPlayername1 );
		pPlayername = combinedName;
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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CMapBaseBot::CMapBaseBot( CBasePlayer* parent ) : BaseClass( parent )
{
}

//-----------------------------------------------------------------------------
// Purpose: Create the components that the bot will have
//-----------------------------------------------------------------------------
void CMapBaseBot::SetUpComponents()
{
	ADD_COMPONENT( CBotVision );
	ADD_COMPONENT( CBotFollow );
	ADD_COMPONENT( CBotLocomotion );
	ADD_COMPONENT( CBotMemory );
	ADD_COMPONENT( CBotAttack );
	ADD_COMPONENT( CBotDecision ); // This component is mandatory!
}

//-----------------------------------------------------------------------------
// Purpose: Create the schedules that the bot will have
//-----------------------------------------------------------------------------
void CMapBaseBot::SetUpSchedules()
{
	ADD_COMPONENT( CHuntEnemySchedule );
	ADD_COMPONENT( CReloadSchedule );
	ADD_COMPONENT( CCoverSchedule );
	ADD_COMPONENT( CHideSchedule );
	ADD_COMPONENT( CChangeWeaponSchedule );
	ADD_COMPONENT( CHideAndHealSchedule );
	ADD_COMPONENT( CHideAndReloadSchedule );
	ADD_COMPONENT( CMoveAsideSchedule );
	ADD_COMPONENT( CCallBackupSchedule );
	ADD_COMPONENT( CDefendSpawnSchedule );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CMapBaseBot::Spawn()
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CMapBaseBot::Update()
{
	VPROF_BUDGET( "CMapBaseBot::Update", VPROF_BUDGETGROUP_BOTS );

	//i should have made this a schedule but when i did it just didn't function so i'm doing this.

	if( GetHost()->IsAlive() )
	{
		if( bot_ai.GetBool() )
		{
			BaseClass::Update();

			if( TheNavAreas.Count() > 0 )
			{
				//wander around.
				Vector vecFrom( GetAbsOrigin() );
				float dist = bot_closestdistancetoplayer.GetFloat();
				CPlayer* closestPlayer = Utils::GetClosestPlayer( GetAbsOrigin(), &dist );

				CNavArea* pArea = closestPlayer->GetLastKnownArea();

				if( pArea == NULL )
				{
					pArea = TheNavAreas[RandomInt( 0, TheNavAreas.Count() - 1 )];

					if( pArea == NULL )
					{
						return;
					}
				}

				Vector vecGoal( pArea->GetCenter() );

				if( !GetLocomotion() || !GetLocomotion()->IsTraversable( vecFrom, vecGoal ) )
				{
					return;
				}

				GetLocomotion()->DriveTo( "Move to nearest player or randomly roam.", pArea );
			}
			else
			{
				Warning( "No nav mesh detected. Kicking %s...\n", GetHost()->GetPlayerName() );
				Kick();
				return;
			}
		}
	}
	//respawn if we die.
	else
	{
		if( bot_respawn.GetBool() )
		{
			respawn( GetHost(), !IsObserver() );
			Spawn();
		}
	}
}

CON_COMMAND_F( bot_add, "Adds a specified number of bots", FCVAR_SERVER )
{
	SpawnGameBots( Q_atoi( args.Arg( 1 ) ) );
}

void SpawnGameBots( int botCount )
{
	// Look at -count. Don't include ourselves.
	int count = Clamp( botCount, 1, gpGlobals->maxClients - 1 );
	// Ok, spawn all the bots.
	while( --count >= 0 )
	{
		CPlayer* pPlayer = CreateGameBot( NULL, NULL, NULL );
		Assert( pPlayer );

		if( !pPlayer || pPlayer == nullptr )
		{
			return;
		}

		if( pPlayer )
		{
			//everything is handled in spawn
		}
	}
}