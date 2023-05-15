//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		TRIPWIRE
//
//=============================================================================//

#ifndef	WEAPONTRIPWIRE_H
#define	WEAPONTRIPWIRE_H
#ifdef _WIN32
	#pragma once
#endif // _WIN32

#include "basegrenade_shared.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"

enum eTripwireState_t
{
	TRIPWIRE_TRIPMINE_READY,
	TRIPWIRE_SATCHEL_THROW,
	TRIPWIRE_SATCHEL_ATTACH,
};

#ifdef CLIENT_DLL
	#define CWeaponTripwire C_WeaponTripwire
#endif

class CWeaponTripwire : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponTripwire, CBaseHL2MPCombatWeapon );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponTripwire();

	virtual void Spawn( void );
	virtual void Precache( void );

#ifdef GAME_DLL
	virtual int CapabilitiesGet( void )
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1;
	}
#endif // GAME_DLL
	virtual void PrimaryAttack( void );
	virtual void WeaponIdle( void );

	virtual void ItemPostFrame( void );
	virtual bool Reload( void );
	virtual bool CanAttachTripwire( void );		// In position where can attach TRIPWIRE?
	virtual void StartTripwireAttach( void );
	virtual void TripwireAttach( void );

	virtual bool Deploy( void );
	virtual bool Holster( CBaseCombatWeapon* pSwitchingTo = NULL );

	DECLARE_ACTTABLE();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif // GAME_DLL

protected:
	CNetworkVar( bool, m_bNeedReload );
	CNetworkVar( bool, m_bClearReload );
	CNetworkVar( bool, m_bAttachTripwire );

private:
	CWeaponTripwire( const CWeaponTripwire& );
};

#endif	//WEAPONTRIPWIRE_H