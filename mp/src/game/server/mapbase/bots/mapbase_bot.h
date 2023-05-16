#ifndef MAPBASE_BOT_H
#define MAPBASE_BOT_H

#ifdef _WIN32
	#pragma once
#endif

#include "bot.h"
#include "ai_hint.h"

//-----------------------------------------------------------------------------
// Purpose: Advanced Non-Player Controller Bot
//-----------------------------------------------------------------------------
class CMapBaseBot : public CBot
{
public:
	DECLARE_CLASS_GAMEROOT( CMapBaseBot, CBot );

	CMapBaseBot( CBasePlayer* parent );

public:
	virtual void Spawn();
	virtual void Update();
	virtual void SetUpComponents();
	virtual void SetUpSchedules();
};

extern CPlayer* CreateGameBot( const char* pPlayername, const Vector* vecPosition, const QAngle* angles );
void SpawnGameBots( int botCount );

#endif // MAPBASE_BOT_H