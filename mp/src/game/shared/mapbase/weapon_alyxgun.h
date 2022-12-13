//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//
#ifndef WEAPON_ALYXGUN_H
#define WEAPON_ALYXGUN_H
#if defined( _WIN32 )
#pragma once
#endif

#include "weapon_hl2mpbase_selectfiremachinegun.h"

#ifdef CLIENT_DLL
	#define CWeaponAlyxGun C_WeaponAlyxGun
#endif

#ifdef MAPBASE
extern acttable_t *GetPistolActtable();
extern int GetPistolActtableCount();
#endif

class CWeaponAlyxGun : public CHL2MPSelectFireMachineGun
{
public:
	DECLARE_CLASS( CWeaponAlyxGun, CHL2MPSelectFireMachineGun );

	CWeaponAlyxGun();
	~CWeaponAlyxGun();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	virtual int		GetMinBurst( void ) { return 4; }
	virtual int		GetMaxBurst( void ) { return 7; }
	virtual float	GetMinRestTime( void );
	virtual float	GetMaxRestTime( void );

	float	GetFireRate( void ) { return 0.1f; }
#ifdef GAME_DLL
	int		CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
	int		WeaponRangeAttack1Condition( float flDot, float flDist );
	int		WeaponRangeAttack2Condition( float flDot, float flDist );
#endif

	virtual const Vector& GetBulletSpread( void );

#ifdef GAME_DLL
	void FireNPCPrimaryAttack( CBaseCombatCharacter *pOperator, bool bUseWeaponAngles );

	void Operator_ForceNPCFire( CBaseCombatCharacter  *pOperator, bool bSecondary );
	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif

#ifdef MAPBASE
	virtual acttable_t		*GetBackupActivityList() { return GetPistolActtable(); }
	virtual int				GetBackupActivityListCount() { return GetPistolActtableCount(); }
#endif

	float m_flTooCloseTimer;

	DECLARE_ACTTABLE();

};

#endif // WEAPON_ALYXGUN_H
