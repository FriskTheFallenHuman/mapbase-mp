//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $Workfile:     $
// $Date:         $
//
//-----------------------------------------------------------------------------
// $Log: $
//
// $NoKeywords: $
//=============================================================================//

#ifndef HL2MP_WEAPON_CROWBAR_H
#define HL2MP_WEAPON_CROWBAR_H
#pragma once


#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "weapon_hl2mpbasebasebludgeon.h"


#ifdef CLIENT_DLL
	#define CWeaponCrowbar C_WeaponCrowbar
	#define CWeaponPipe C_WeaponPipe
#endif

#define	CROWBAR_RANGE	75.0f
#define	CROWBAR_REFIRE	0.4f

//-----------------------------------------------------------------------------
// Purpose: Gordon Freeman's Iconic Weapon
//-----------------------------------------------------------------------------
class CWeaponCrowbar : public CBaseHL2MPBludgeonWeapon
{
public:
	DECLARE_CLASS( CWeaponCrowbar, CBaseHL2MPBludgeonWeapon );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	CWeaponCrowbar();

	float		GetRange( void )
	{
		return CROWBAR_RANGE;
	}
	float		GetFireRate( void )
	{
		return CROWBAR_REFIRE;
	}

	void		AddViewKick( void );
	float		GetDamageForActivity( Activity hitActivity );

#ifndef CLIENT_DLL
	virtual int WeaponMeleeAttack1Condition( float flDot, float flDist );
#endif
	void		SecondaryAttack( void )
	{
		return;
	}

	// Animation event
#ifndef CLIENT_DLL
	virtual void Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator );
#endif

#ifdef MAPBASE
	// Don't use backup activities
	acttable_t*		GetBackupActivityList()
	{
		return NULL;
	}
	int				GetBackupActivityListCount()
	{
		return 0;
	}
#endif

#ifndef CLIENT_DLL
	// Animation event handlers
	void HandleAnimEventMeleeHit( animevent_t* pEvent, CBaseCombatCharacter* pOperator );
#endif

private:

	CWeaponCrowbar( const CWeaponCrowbar& );
};

//-----------------------------------------------------------------------------
// Purpose: Matt's Pipe weapon
//-----------------------------------------------------------------------------
class CWeaponPipe : public CWeaponCrowbar
{
	DECLARE_CLASS( CWeaponPipe, CWeaponCrowbar );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
};

#endif // HL2MP_WEAPON_CROWBAR_H

