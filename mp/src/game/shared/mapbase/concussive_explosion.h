//========= Mapbase MP - https://github.com/FriskTheFallenHuman/mapbase-mp ============//
//
// Purpose: This is basically weapon_cguard but separated has its own entity
//
//=============================================================================//
#ifndef CONCUSSIVE_EXPLOSION_H
#define CONCUSSIVE_EXPLOSION_H
#ifdef _WIN32
#pragma once
#endif

#include "te_particlesystem.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CTEConcussiveExplosion : public CTEParticleSystem
{
public:
	DECLARE_CLASS( CTEConcussiveExplosion, CTEParticleSystem );
	DECLARE_SERVERCLASS();

	CTEConcussiveExplosion( const char *name );
	virtual	~CTEConcussiveExplosion( void );

	CNetworkVector( m_vecNormal );
	CNetworkVar( float, m_flScale );
	CNetworkVar( int, m_nRadius );
	CNetworkVar( int, m_nMagnitude );
};

#ifdef MAPBASE
#define SF_CONCUSSIVEBLAST_REPEATABLE 0x00000001
#endif

//-----------------------------------------------------------------------------
// Purpose: Temp ent for the blast
//-----------------------------------------------------------------------------
class CConcussiveBlast : public CBaseEntity
{
	DECLARE_DATADESC();
public:
	DECLARE_CLASS( CConcussiveBlast, CBaseEntity );

	int		m_spriteTexture;

#ifdef MAPBASE
	float	m_flDamage = 200;
	float	m_flRadius = 256;
	float	m_flMagnitude = 1.0;
	string_t	m_iszSoundName;
#endif

	CConcussiveBlast( void ) {}

	virtual void Precache( void );
	virtual void Explode( float magnitude );
#ifdef MAPBASE
	virtual void InputExplode( inputdata_t &inputdata );
	virtual void InputExplodeWithMagnitude( inputdata_t &inputdata );
#endif

};

void CreateConcussiveBlast( const Vector& origin, const Vector& surfaceNormal, CBaseEntity* pOwner, float magnitude );

#endif // CONCUSSIVE_EXPLOSION_H