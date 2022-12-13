//========= Mapbase MP - https://github.com/FriskTheFallenHuman/mapbase-mp ============//
//
// Purpose: Flare gun (fffsssssssssss!!)
//
//=============================================================================//
#ifndef WEAPON_FLAREGUN_H
#define WEAPON_FLAREGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"

#ifdef CLIENT_DLL
#define CWeaponFlaregun C_WeaponFlaregun
#endif

//---------------------
// Purpose: Flaregun
//---------------------
class CWeaponFlaregun : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponFlaregun, CBaseHL2MPCombatWeapon );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

	virtual void Precache( void );
	virtual void PrimaryAttack( void );
};

#endif // WEAPON_FLAREGUN_H