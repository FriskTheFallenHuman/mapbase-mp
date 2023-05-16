//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors:
// Iv�n Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "squad_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

CSquadManager g_SquadManager;
CSquadManager* TheSquads = &g_SquadManager;

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CSquadManager::CSquadManager() : CAutoGameSystemPerFrame( "SquadManager" )
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CSquadManager::LevelShutdownPostEntity()
{
	// Kill all squads
	m_nSquads.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CSquadManager::FrameUpdatePostEntityThink()
{
	// There's not any squad
	if( m_nSquads.Count() == 0 )
	{
		return;
	}

	// Every squad need to be verified
	FOR_EACH_VEC( m_nSquads, it )
	{
		CSquad* pSquad = m_nSquads.Element( it );
		pSquad->Think();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CSquad* CSquadManager::GetSquad( const char* name )
{
	// There's not any squad
	if( m_nSquads.Count() == 0 )
	{
		return NULL;
	}

	FOR_EACH_VEC( m_nSquads, it )
	{
		CSquad* pSquad = m_nSquads.Element( it );

		// Not what we find
		if( !pSquad->IsNamed( name ) )
		{
			continue;
		}

		return pSquad;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CSquad* CSquadManager::GetOrCreateSquad( const char* name )
{
	// Check if exist
	CSquad* pSquad = GetSquad( name );

	// yep
	if( pSquad )
	{
		return pSquad;
	}

	// Create the squad
	pSquad = new CSquad();
	pSquad->SetName( name );

	return pSquad;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CSquadManager::AddSquad( CSquad* pSquad )
{
	int index = m_nSquads.Find( pSquad );

	// Is on our list
	if( index > -1 )
	{
		return;
	}

	// Add it
	m_nSquads.AddToTail( pSquad );
}