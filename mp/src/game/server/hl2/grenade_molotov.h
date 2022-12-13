//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Molotov grenades
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef	GRENADEMOLOTOV_H
#define	GRENADEMOLOTOV_H
#ifdef _WIN32
#pragma once
#endif // _WIN32


#include "basegrenade_shared.h"

#define	MAX_MOLOTOV_NO_COLLIDE_TIME 0.2

class SmokeTrail;
class CFireTrail;

class CGrenade_Molotov : public CBaseGrenade
{
public:
	DECLARE_CLASS( CGrenade_Molotov, CBaseGrenade );

	CHandle< SmokeTrail > m_hSmokeTrail;
#ifdef MAPBASE_MP
	CHandle< CFireTrail > m_hFireTrail;
#endif
	float				 m_fSpawnTime;
	float				m_fDangerRadius;

	virtual void Spawn( void );
	virtual void Precache( void );
	virtual void MolotovTouch( CBaseEntity *pOther );
	virtual void MolotovThink( void );
	virtual void Event_Killed( const CTakeDamageInfo &info );

public:
	void EXPORT				Detonate(void);
	CGrenade_Molotov(void);

	DECLARE_DATADESC();
};

#endif	//GRENADEMOLOTOV_H
