//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================
#ifndef WEAPON_CITIZENPACKAGE_H
#define WEAPON_CITIZENPACKAGE_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifdef CLIENT_DLL
	#define CWeaponCitizenPackage C_WeaponCitizenPackage
	#define CWeaponCitizenSuitcase C_WeaponCitizenSuitcase
#endif

//-----------------------------------------------------------------------------
// Purpose: Citizen Package
//-----------------------------------------------------------------------------
class CWeaponCitizenPackage : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponCitizenPackage, CBaseHL2MPCombatWeapon );
public:

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	virtual void ItemPostFrame( void ) { /* Do Nothing */ }
};

//-----------------------------------------------------------------------------
// Purpose: Citizen suitcase
//-----------------------------------------------------------------------------
class CWeaponCitizenSuitcase : public CWeaponCitizenPackage
{
	DECLARE_CLASS( CWeaponCitizenSuitcase, CWeaponCitizenPackage );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();
};

#endif // WEAPON_CITIZENPACKAGE_H