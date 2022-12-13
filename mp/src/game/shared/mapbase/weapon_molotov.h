//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Molotov weapon
//
//=============================================================================//
#ifndef	WEAPON_MOLOTOV_H
#define	WEAPON_MOLOTOV_H
#if _WIN32
#pragma once
#endif // _WIN32

#include "weapon_hl2mpbasehlmpcombatweapon.h"

class CGrenade_Molotov;

#ifdef CLIENT_DLL
	#define CWeaponMolotov C_WeaponMolotov
#endif

//-----------------------------------------------------------------------------
// Incendiary grenades
//-----------------------------------------------------------------------------
class CWeaponMolotov : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponMolotov, CBaseHL2MPCombatWeapon );

public:
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

public:

	CWeaponMolotov( void );

	virtual void Precache( void );
#ifdef GAME_DLL
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif
	virtual void PrimaryAttack( void );
	virtual void DecrementAmmo( CBaseCombatCharacter *pOwner );
	virtual void ItemPostFrame( void );

	virtual bool Deploy( void );
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo = NULL );
	
	virtual bool Reload( void );

	virtual bool ShouldDisplayHUDHint() { return true; }

	virtual void ThrowMolotov( CBasePlayer *pPlayer );
	virtual bool IsPrimed( bool ) { return ( m_AttackPaused != 0 );	}

	// check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
	virtual void	CheckThrowPosition( CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc,
		const QAngle& angles = vec3_angle );

private:

	CWeaponMolotov( const CWeaponMolotov & );

	CNetworkVar( bool,	m_bRedraw );	//Draw the weapon again after throwing a grenade
	
	CNetworkVar( int,	m_AttackPaused );
	CNetworkVar( bool,	m_fDrawbackFinished );

	DECLARE_ACTTABLE();
};

#endif	//WEAPON_MOLOTOV_H
