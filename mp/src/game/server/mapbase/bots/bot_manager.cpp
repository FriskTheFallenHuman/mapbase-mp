//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors:
// Iv�n Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bot_manager.h"
#include "bot.h"
#include "in_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CBotManager g_BotManager;
CBotManager* TheBots = &g_BotManager;

void Bot_RunAll()
{
	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		pPlayer->GetBotController()->Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBotManager::CBotManager() : CAutoGameSystemPerFrame( "BotManager" )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CBotManager::Init()
{
	Utils::InitBotTrig();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBotManager::LevelInitPostEntity()
{

}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBotManager::LevelShutdownPreEntity()
{
	for( int it = 0; it <= gpGlobals->maxClients; ++it )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

		if( !pPlayer )
		{
			continue;
		}

		if( !pPlayer->IsBot() )
		{
			continue;
		}

		engine->ServerCommand( UTIL_VarArgs( "kickid %i\n", pPlayer->GetPlayerInfo()->GetUserID() ) );
	}

	engine->ServerExecute();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBotManager::FrameUpdatePreEntityThink()
{
	//Bot_RunAll();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBotManager::FrameUpdatePostEntityThink()
{

}