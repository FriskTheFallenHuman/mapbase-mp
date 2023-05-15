//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		TRIPWIRE
//
//=============================================================================//
#include "cbase.h"
#include "mapbase/weapon_tripwire.h" // Load the hl2mp version!
#include "gamerules.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "engine/IEngineSound.h"

#if defined( CLIENT_DLL )
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
	#include "grenade_tripwire.h"
	#include "entitylist.h"
	#include "eventqueue.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponTripwire, DT_WeaponTripwire )

BEGIN_NETWORK_TABLE( CWeaponTripwire, DT_WeaponTripwire )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bNeedReload ) ),
	RecvPropBool( RECVINFO( m_bClearReload ) ),
	RecvPropBool( RECVINFO( m_bAttachTripwire ) ),
#else
	SendPropBool( SENDINFO( m_bNeedReload ) ),
	SendPropBool( SENDINFO( m_bClearReload ) ),
	SendPropBool( SENDINFO( m_bAttachTripwire ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
	BEGIN_PREDICTION_DATA( CWeaponTripwire )
	DEFINE_PRED_FIELD( m_bNeedReload, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bClearReload, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bAttachTripwire, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	END_PREDICTION_DATA()
#endif

#ifndef CLIENT_DLL
	BEGIN_DATADESC( CWeaponTripwire )
	DEFINE_FIELD( m_bNeedReload, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bClearReload, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bAttachTripwire, FIELD_BOOLEAN ),
	END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( weapon_tripwire, CWeaponTripwire );
PRECACHE_WEAPON_REGISTER( weapon_tripwire );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponTripwire::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1,				ACT_RANGE_ATTACK_TRIPWIRE,				true },

	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_GRENADE,					 false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_GRENADE,			 false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_GRENADE,					  false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_GRENADE,					 false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_GRENADE,			 false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,	 false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_GRENADE, false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_GRENADE,	 false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_GRENADE,		 false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_GRENADE,		 false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_GRENADE,					 false },
};

IMPLEMENT_ACTTABLE( CWeaponTripwire );

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CWeaponTripwire::Spawn( )
{
	BaseClass::Spawn();

	Precache( );

	//UTIL_SetSize( this, Vector( -4,-4,-2 ),Vector( 4,4,2 ) );

	FallInit();	// get ready to fall down

	SetThink( NULL );

	// Give one piece of default ammo when first picked up
	m_iClip2 = 1;
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CWeaponTripwire::Precache( void )
{
	BaseClass::Precache();

	PrecacheScriptSound( "Weapon_Tripwire.Attach" );

#ifdef GAME_DLL
	UTIL_PrecacheOther( "npc_tripwire" );
#endif // GAME_DLL

}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
bool CWeaponTripwire::Holster( CBaseCombatWeapon* pSwitchingTo )
{
	SetThink( NULL );

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Tripwire has no reload, but must call weapon idle to update state
//-----------------------------------------------------------------------------
bool CWeaponTripwire::Reload( void )
{
	WeaponIdle( );
	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponTripwire::PrimaryAttack( void )
{
	CBaseCombatCharacter* pOwner  = GetOwner();
	if( !pOwner )
	{
		return;
	}

	if( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
	{
		return;
	}

	if( CanAttachTripwire() )
	{
		StartTripwireAttach();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponTripwire::TripwireAttach( void )
{
	CHL2MP_Player* pOwner  = ToHL2MPPlayer( GetOwner() );
	if( !pOwner )
	{
		return;
	}

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pOwner->EyePosition();

	QAngle angles = pOwner->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + ( vecAiming * 128 ), MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if( tr.fraction < 1.0 )
	{
		CBaseEntity* pEntity = tr.m_pEnt;
		if( pEntity && !( pEntity->GetFlags() & FL_CONVEYOR ) )
		{
#ifndef CLIENT_DLL
			QAngle angles;
			VectorAngles( tr.plane.normal, angles );

			angles.x += 90;

			CBaseEntity* pEnt = CBaseEntity::Create( "npc_tripwire", tr.endpos + tr.plane.normal * 3, angles, NULL );
			if( pEnt )
			{
				CTripwireGrenade* pWire = ( CTripwireGrenade* )pEnt;
#if !defined(NO_STEAM)
				CBasePlayer* pPlayer = dynamic_cast< CBasePlayer* >( pOwner );
				if( pPlayer )
				{
					pWire->SetSteamID( pPlayer->GetSteamIDAsUInt64() );
				}
#endif
				//pWire->m_hOwner = GetOwner();
				pWire->SetThrower( GetOwner() );
			}
#endif
			pOwner->RemoveAmmo( 1, m_iSecondaryAmmoType );

			EmitSound( "Weapon_Tripwire.Attach" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponTripwire::StartTripwireAttach( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( !pPlayer )
	{
		return;
	}

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pPlayer->EyePosition();

	QAngle angles = pPlayer->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	UTIL_TraceLine( vecSrc, vecSrc + ( vecAiming * 128 ), MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );

	if( tr.fraction < 1.0 )
	{
		ConColorMsg( Color( 6, 255, 0, 255 ), "[StartTripwireAttach] hitting: %f\n", tr.fraction );

		CBaseEntity* pEntity = tr.m_pEnt;
		if( pEntity && !( pEntity->GetFlags() & FL_CONVEYOR ) )
		{
			// player "shoot" animation
			pPlayer->SetAnimation( PLAYER_ATTACK1 );

			//Tony: ???
			ToHL2MPPlayer( pPlayer )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );


			// -----------------------------------------
			//  Play attach animation
			// -----------------------------------------
			SendWeaponAnim( ACT_SLAM_TRIPMINE_ATTACH );

			m_bNeedReload		= true;
			m_bAttachTripwire	= true;
		}
		else
		{
			ConColorMsg( Color( 0, 255, 210, 255 ), "[StartTripwireAttach] no deploy\n" );
		}
	}

	// needs a higher delay on all of these, a minimum time really - to elimiate refires.
	m_flNextPrimaryAttack	= 0.05f + gpGlobals->curtime + SequenceDuration();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponTripwire::CanAttachTripwire( void )
{
	CHL2MP_Player* pOwner = ToHL2MPPlayer( GetOwner() );
	if( !pOwner )
	{
		return false;
	}

	Vector vecSrc, vecAiming;

	// Take the eye position and direction
	vecSrc = pOwner->EyePosition();

	QAngle angles = pOwner->GetLocalAngles();

	AngleVectors( angles, &vecAiming );

	trace_t tr;

	Vector	vecEnd = vecSrc + ( vecAiming * 42 );
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

	if( tr.fraction < 1.0 )
	{
		// Don't attach to a living creature
		if( tr.m_pEnt )
		{
			CBaseEntity* pEntity = tr.m_pEnt;
			CBaseCombatCharacter* pBCC = ToBaseCombatCharacter( pEntity );
			if( pBCC )
			{
				return false;
			}

			//block attaching to another grenade
			CBaseGrenade* pGrenade = dynamic_cast< CBaseGrenade* >( pEntity );
			if( pGrenade )
			{
				return false;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :
// Output :
//-----------------------------------------------------------------------------
void CWeaponTripwire::ItemPostFrame( void )
{
	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );
	if( !pOwner )
	{
		return;
	}

#ifdef MAPBASE
	if( pOwner->HasSpawnFlags( SF_PLAYER_SUPPRESS_FIRING ) )
	{
		WeaponIdle();
		return;
	}
#endif

	if( !m_bNeedReload && ( pOwner->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) )
	{
		PrimaryAttack();
	}
	// -----------------------
	//  No buttons down
	// -----------------------
	else
	{
		WeaponIdle( );
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponTripwire::WeaponIdle( void )
{
	// Ready to switch animations?
	if( HasWeaponIdleTimeElapsed() )
	{
		if( m_bClearReload )
		{
			m_bNeedReload  = false;
			m_bClearReload = false;
		}

		CBaseCombatCharacter* pOwner  = GetOwner();
		if( !pOwner )
		{
			return;
		}

		int iAnim = 0;

		if( m_bAttachTripwire )
		{
			TripwireAttach();
			iAnim = ACT_SLAM_TRIPMINE_ATTACH2;
		}
		else if( m_bNeedReload )
		{
			// If owner had ammo draw the correct tripwire type
			if( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) > 0 )
			{
				iAnim = ACT_SLAM_TRIPMINE_DRAW;
				m_bClearReload = true;
			}
			else
			{
#ifdef GAME_DLL
				pOwner->Weapon_Drop( this );
				UTIL_Remove( this );
#endif // GAME_DLL
			}
		}
		else if( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
		{
#ifdef GAME_DLL
			pOwner->Weapon_Drop( this );
			UTIL_Remove( this );
#endif // GAME_DLL
		}

		// If I don't need to reload just do the appropriate idle
		else
		{
			iAnim = ACT_SLAM_TRIPMINE_IDLE;
		}

		SendWeaponAnim( iAnim );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponTripwire::Deploy( void )
{
	CBaseCombatCharacter* pOwner  = GetOwner();
	if( !pOwner )
	{
		return false;
	}

	SetModel( GetViewModel() );

	// If detonator is already armed
	m_bNeedReload = false;

	return DefaultDeploy( ( char* )GetViewModel(), ( char* )GetWorldModel(), ACT_SLAM_STICKWALL_ND_DRAW, ( char* )GetAnimPrefix() );
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponTripwire::CWeaponTripwire( void )
{
	m_bNeedReload = true;
	m_bClearReload = false;
	m_bAttachTripwire = false;
}
