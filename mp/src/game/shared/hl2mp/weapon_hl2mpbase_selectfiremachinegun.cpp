//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbase_selectfiremachinegun.h"
#include "in_buttons.h"
#include "gamestats.h"

#if defined( CLIENT_DLL )
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
	#include "soundent.h"
	#include "ai_basenpc.h"
	#include "game.h"
	#include "ilagcompensationmanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=========================================================
//	>> CHL2MPSelectFireMachineGun
//=========================================================
IMPLEMENT_NETWORKCLASS_ALIASED( HL2MPSelectFireMachineGun, DT_HL2MPSelectFireMachineGun )

BEGIN_NETWORK_TABLE( CHL2MPSelectFireMachineGun, DT_HL2MPSelectFireMachineGun )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iBurstSize ) ),
	RecvPropInt( RECVINFO( m_iFireMode ) ),
#else
	SendPropInt( SENDINFO( m_iBurstSize ) ),
	SendPropInt( SENDINFO( m_iFireMode ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CHL2MPSelectFireMachineGun )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_iBurstSize, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iFireMode, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

BEGIN_DATADESC( CHL2MPSelectFireMachineGun )

	DEFINE_FIELD( m_iBurstSize,		FIELD_INTEGER ),
	DEFINE_FIELD( m_iFireMode,		FIELD_INTEGER ),

#ifdef GAME_DLL
	// Function pinters
	DEFINE_THINKFUNC( BurstThink ),
#endif // GAME_DLL

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CHL2MPSelectFireMachineGun::GetBurstCycleRate( void )
{
	// this is the time it takes to fire an entire 
	// burst, plus whatever amount of delay we want
	// to have between bursts.
	return 0.5f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CHL2MPSelectFireMachineGun::GetFireRate( void )
{
	switch( m_iFireMode )
	{
	case FIREMODE_FULLAUTO:
		// the time between rounds fired on full auto
		return 0.1f;	// 600 rounds per minute = 0.1 seconds per bullet
		break;

	case FIREMODE_3RNDBURST:
		// the time between rounds fired within a single burst
		return 0.1f;	// 600 rounds per minute = 0.1 seconds per bullet
		break;

	default:
		return 0.1f;
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHL2MPSelectFireMachineGun::Deploy( void )
{
	// Forget about any bursts this weapon was firing when holstered
	m_iBurstSize = 0;

	return BaseClass::Deploy();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPSelectFireMachineGun::PrimaryAttack( void )
{
	if ( m_bFireOnEmpty )
		return;

	switch( m_iFireMode )
	{
	case FIREMODE_FULLAUTO:
		BaseClass::PrimaryAttack();
		// Msg("%.3f\n", m_flNextPrimaryAttack.Get() );
		SetWeaponIdleTime( gpGlobals->curtime + 3.0f );
		break;

	case FIREMODE_3RNDBURST:
		m_iBurstSize = GetBurstSize();
		
		// Call the think function directly so that the first round gets fired immediately.
		BurstThink();
		SetThink( &CHL2MPSelectFireMachineGun::BurstThink );
		m_flNextSecondaryAttack = m_flNextPrimaryAttack = gpGlobals->curtime + GetBurstCycleRate();

		// Pick up the rest of the burst through the think function.
		SetNextThink( gpGlobals->curtime + GetFireRate() );
		break;
	}

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner )
	{
#ifdef GAME_DLL
		m_iPrimaryAttacks++;
		gamestats->Event_WeaponFired( pOwner, false, GetClassname() );
#endif // GAME_DLL
		ToHL2MPPlayer( pOwner )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPSelectFireMachineGun::SecondaryAttack( void )
{
	// change fire modes.

	switch( m_iFireMode )
	{
	case FIREMODE_FULLAUTO:
		//Msg( "Burst\n" );
		m_iFireMode = FIREMODE_3RNDBURST;
		WeaponSound( SPECIAL2 );
		break;

	case FIREMODE_3RNDBURST:
		//Msg( "Auto\n" );
		m_iFireMode = FIREMODE_FULLAUTO;
		WeaponSound( SPECIAL1 );
		break;
	}
	
	SendWeaponAnim( GetSecondaryAttackActivity() );

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.3;

	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner )
	{
#ifdef GAME_DLL
		m_iSecondaryAttacks++;
		gamestats->Event_WeaponFired( pOwner, false, GetClassname() );
#endif // GAME_DLL

#ifdef MAPBASE
		ToHL2MPPlayer( pOwner )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_SECONDARY );
#endif
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPSelectFireMachineGun::BurstThink( void )
{
	BaseClass::PrimaryAttack();

	m_iBurstSize--;

	if( m_iBurstSize == 0 )
	{
		// The burst is over!
		SetThink( NULL );

		// idle immediately to stop the firing animation
		SetWeaponIdleTime( gpGlobals->curtime );
		return;
	}

	SetNextThink( gpGlobals->curtime + GetFireRate() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MPSelectFireMachineGun::WeaponSound( WeaponSound_t shoot_type, float soundtime /*= 0.0f*/ )
{
	if ( shoot_type == SINGLE )
	{
		switch( m_iFireMode )
		{
		case FIREMODE_FULLAUTO:
			BaseClass::WeaponSound( SINGLE, soundtime );
			break;

		case FIREMODE_3RNDBURST:
			if( m_iBurstSize == GetBurstSize() && m_iClip1 >= m_iBurstSize )
			{
				// First round of a burst, and enough bullets remaining in the clip to fire the whole burst
				BaseClass::WeaponSound( BURST, soundtime );
			}
			else if( m_iClip1 < m_iBurstSize )
			{
				// Not enough rounds remaining in the magazine to fire a burst, so play the gunshot
				// sounds individually as each round is fired.
				BaseClass::WeaponSound( SINGLE, soundtime );
			}

			break;
		}
		return;
	}

	BaseClass::WeaponSound( shoot_type, soundtime );
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// BUGBUG: These need to be rethought
//-----------------------------------------------------------------------------
int CHL2MPSelectFireMachineGun::WeaponRangeAttack1Condition( float flDot, float flDist )
{
	if ( m_iClip1 <=0 )
		return COND_NO_PRIMARY_AMMO;
	else if ( flDist < m_fMinRange1 ) 
		return COND_TOO_CLOSE_TO_ATTACK;
	else if ( flDist > m_fMaxRange1 ) 
		return COND_TOO_FAR_TO_ATTACK;
	else if ( flDot < 0.5 )	// UNDONE: Why check this here? Isn't the AI checking this already?
		return COND_NOT_FACING_ATTACK;

	return COND_CAN_RANGE_ATTACK1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CHL2MPSelectFireMachineGun::WeaponRangeAttack2Condition( float flDot, float flDist )
{
	return COND_NONE; // FIXME: disabled for now

	// m_iClip2 == -1 when no secondary clip is used
	if ( m_iClip2 == 0 && UsesSecondaryAmmo() )
		return COND_NO_SECONDARY_AMMO;
	else if ( flDist < m_fMinRange2 )
		return COND_NONE;	// Don't return	COND_TOO_CLOSE_TO_ATTACK only for primary attack
	else if (flDist > m_fMaxRange2 )
		return COND_NONE;	// Don't return COND_TOO_FAR_TO_ATTACK only for primary attack
	else if ( flDot < 0.5 ) // UNDONE: Why check this here? Isn't the AI checking this already?
		return COND_NOT_FACING_ATTACK;

	return COND_CAN_RANGE_ATTACK2;
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CHL2MPSelectFireMachineGun::CHL2MPSelectFireMachineGun( void )
{
	m_fMinRange1	= 65;
	m_fMinRange2	= 65;
	m_fMaxRange1	= 1024;
	m_fMaxRange2	= 1024;
	m_iBurstSize	= 0;
	m_iFireMode		= FIREMODE_FULLAUTO;
}