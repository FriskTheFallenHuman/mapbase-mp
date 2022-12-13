//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef WEAPON_HL2MPBASE_SELECTFIREMACHINEGUN_H
#define WEAPON_HL2MPBASE_SELECTFIREMACHINEGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "weapon_hl2mpbase_machinegun.h"

#if defined( CLIENT_DLL )
	#define CHL2MPSelectFireMachineGun C_HL2MPSelectFireMachineGun
#endif

//=========================================================
// Machine guns capable of switching between full auto and 
// burst fire modes.
//=========================================================
// Mode settings for select fire weapons
enum eHL2FireTypes
{
	FIREMODE_FULLAUTO = 1,
	FIREMODE_SEMI,
	FIREMODE_3RNDBURST,
};

//=========================================================
//	>> CHL2MPSelectFireMachineGun
//=========================================================
class CHL2MPSelectFireMachineGun : public CHL2MPMachineGun
{
	DECLARE_CLASS( CHL2MPSelectFireMachineGun, CHL2MPMachineGun );
public:

	CHL2MPSelectFireMachineGun( void );

	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	virtual float	GetBurstCycleRate( void );
	virtual float	GetFireRate( void );

	virtual bool	Deploy( void );
	virtual void	WeaponSound( WeaponSound_t shoot_type, float soundtime = 0.0f );

	virtual int		GetBurstSize( void ) { return 3; };

	void			BurstThink( void );

	virtual void	PrimaryAttack( void );
	virtual void	SecondaryAttack( void );

#ifdef GAME_DLL
	virtual int		WeaponRangeAttack1Condition( float flDot, float flDist );
	virtual int		WeaponRangeAttack2Condition( float flDot, float flDist );
#endif // GAME_DLL

private:
	
	CHL2MPSelectFireMachineGun( const CHL2MPSelectFireMachineGun & );

protected:
	CNetworkVar( int, m_iBurstSize );
	CNetworkVar( int, m_iFireMode );
};

#endif // WEAPON_HL2MPBASE_SELECTFIREMACHINEGUN_H