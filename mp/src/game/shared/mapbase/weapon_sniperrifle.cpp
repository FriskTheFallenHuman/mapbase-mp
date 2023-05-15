//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: Implements a sniper rifle weapon.
//
//			Primary attack: fires a single high-powered shot, then reloads.
//			Secondary attack: cycles sniper scope through zoom levels.
//
// TODO: Circular mask around crosshairs when zoomed in.
// TODO: Shell ejection.
// TODO: Finalize kickback.
// TODO: Animated zoom effect?
//
//=============================================================================

#include "cbase.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "npcevent.h"
#include "in_buttons.h"
#include "rumble_shared.h"
#include "gamestats.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "basecombatcharacter.h"
	#include "ai_basenpc.h"
	#include "game.h"
	#include "ai_memory.h"
	#include "soundent.h"
	#include "hl2mp_player.h"
	#include "basegrenade_shared.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
	#define CWeaponSniperRifle C_WeaponSniperRifle
#endif

#define SNIPER_CONE_PLAYER					vec3_origin	// Spread cone when fired by the player.
#define SNIPER_CONE_NPC						vec3_origin	// Spread cone when fired by NPCs.
#define SNIPER_BULLET_COUNT_PLAYER			1			// Fire n bullets per shot fired by the player.
#define SNIPER_BULLET_COUNT_NPC				1			// Fire n bullets per shot fired by NPCs.
#define SNIPER_TRACER_FREQUENCY_PLAYER		0			// Draw a tracer every nth shot fired by the player.
#define SNIPER_TRACER_FREQUENCY_NPC			0			// Draw a tracer every nth shot fired by NPCs.
#define SNIPER_KICKBACK						3			// Range for punchangle when firing.

#define SNIPER_ZOOM_RATE					0.2			// Interval between zoom levels in seconds.

//-----------------------------------------------------------------------------
// Discrete zoom levels for the scope.
//-----------------------------------------------------------------------------
static int g_nZoomFOV[] =
{
	20,
	5
};


class CWeaponSniperRifle : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponSniperRifle, CBaseHL2MPCombatWeapon );

	CWeaponSniperRifle( void );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

#ifdef GAME_DLL
	virtual int CapabilitiesGet( void )
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1;
	}
#endif // GAME_DLL

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = SNIPER_CONE_PLAYER;
		return cone;
	}

	virtual bool Holster( CBaseCombatWeapon* pSwitchingTo = NULL );
	virtual void ItemPostFrame( void );
	virtual void PrimaryAttack( void );
	virtual bool Reload( void );
	virtual void Zoom( void );
	virtual float GetFireRate( void )
	{
		return 1;
	}

#ifdef GAME_DLL
	virtual void Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator );
#endif // GAME_DLL

	DECLARE_ACTTABLE();

protected:

	CNetworkVar( float, m_fNextZoom );
	CNetworkVar( int, m_nZoomLevel );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSniperRifle, DT_WeaponSniperRifle )

BEGIN_NETWORK_TABLE( CWeaponSniperRifle, DT_WeaponSniperRifle )
#ifdef CLIENT_DLL
	RecvPropFloat( RECVINFO( m_fNextZoom ) ),
	RecvPropInt( RECVINFO( m_nZoomLevel ) ),
#else
	SendPropFloat( SENDINFO( m_fNextZoom ) ),
	SendPropInt( SENDINFO( m_nZoomLevel ) ),
#endif
END_NETWORK_TABLE()

#ifndef GAME_DLL
	BEGIN_PREDICTION_DATA( CWeaponSniperRifle )
	DEFINE_PRED_FIELD( m_fNextZoom, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_nZoomLevel, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	END_PREDICTION_DATA()
#endif // !GAME_DLL

LINK_ENTITY_TO_CLASS( weapon_sniperrifle, CWeaponSniperRifle );
PRECACHE_WEAPON_REGISTER( weapon_sniperrifle );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponSniperRifle::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_SNIPER_RIFLE,		true },

#if EXPANDED_HL2_UNUSED_WEAPON_ACTIVITIES
	// Optional new NPC activities
	// (these should fall back to AR2 animations when they don't exist on an NPC)
	{ ACT_RELOAD,					ACT_RELOAD_SNIPER_RIFLE,			true },
	{ ACT_IDLE,						ACT_IDLE_SNIPER_RIFLE,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SNIPER_RIFLE,		true },

// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SNIPER_RIFLE_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SNIPER_RIFLE,			false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_SNIPER_RIFLE_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_SNIPER_RIFLE,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_SNIPER_RIFLE_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_SNIPER_RIFLE,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_SNIPER_RIFLE_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SNIPER_RIFLE,			false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_SNIPER_RIFLE_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_SNIPER_RIFLE,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_SNIPER_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_SNIPER_RIFLE_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_SNIPER_RIFLE,				false },//always aims
//End readiness activities

	{ ACT_WALK,						ACT_WALK_SNIPER_RIFLE,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_SNIPER_RIFLE,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,					true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,				true },
	{ ACT_RUN,						ACT_RUN_SNIPER_RIFLE,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_SNIPER_RIFLE,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,					true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,				true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SNIPER_RIFLE_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_SNIPER_RIFLE_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_SNIPER_RIFLE_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SNIPER_RIFLE_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SNIPER_RIFLE,		true },

	{ ACT_ARM,						ACT_ARM_RIFLE,							false },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,						false },

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED,			ACT_RANGE_AIM_SNIPER_RIFLE_MED,			false },
	{ ACT_RANGE_ATTACK1_MED,		ACT_RANGE_ATTACK_SNIPER_RIFLE_MED,		false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SNIPER_RIFLE,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SNIPER_RIFLE,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SNIPER_RIFLE,						false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_SNIPER_RIFLE,					false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SNIPER_RIFLE,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,	false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_SNIPER_RIFLE,	false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SNIPER_RIFLE,	false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SNIPER_RIFLE,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SNIPER_RIFLE,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SNIPER_RIFLE,					false },
#endif
};

IMPLEMENT_ACTTABLE( CWeaponSniperRifle );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CWeaponSniperRifle::CWeaponSniperRifle( void )
{
	m_fNextZoom = gpGlobals->curtime;
	m_nZoomLevel = 0;

	m_bReloadsSingly = true;

	m_fMinRange1 = 65;
	m_fMinRange2 = 65;
	m_fMaxRange1 = 2048;
	m_fMaxRange2 = 2048;
}

//-----------------------------------------------------------------------------
// Purpose: Turns off the zoom when the rifle is holstered.
//-----------------------------------------------------------------------------
bool CWeaponSniperRifle::Holster( CBaseCombatWeapon* pSwitchingTo )
{

	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( pPlayer != NULL )
	{
		if( m_nZoomLevel != 0 )
		{
#ifdef GAME_DLL
			pPlayer->ShowViewModel( true );
#endif // GAME_DLL
			pPlayer->SetFOV( pPlayer, 0 );

			m_nZoomLevel = 0;
		}
	}

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Overloaded to handle the zoom functionality.
//-----------------------------------------------------------------------------
void CWeaponSniperRifle::ItemPostFrame( void )
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( pPlayer == NULL )
	{
		return;
	}

	if( ( m_bInReload ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) )
	{
		FinishReload();
		m_bInReload = false;
	}

	if( pPlayer->m_nButtons & IN_ATTACK2 )
	{
		if( m_fNextZoom <= gpGlobals->curtime )
		{
			Zoom();
			pPlayer->m_nButtons &= ~IN_ATTACK2;
		}
	}
	else if( ( pPlayer->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) )
	{
		if( ( m_iClip1 == 0 && UsesClipsForAmmo1() ) || ( !UsesClipsForAmmo1() && !pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) ) )
		{
			m_bFireOnEmpty = true;
		}

		// Fire underwater?
		if( pPlayer->GetWaterLevel() == 3 && m_bFiresUnderwater == false )
		{
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.2;
			return;
		}
		else
		{
			// If the firing button was just pressed, reset the firing time
			if( pPlayer && pPlayer->m_afButtonPressed & IN_ATTACK )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime;
			}

			PrimaryAttack();
		}
	}

	// -----------------------
	//  Reload pressed / Clip Empty
	// -----------------------
	if( pPlayer->m_nButtons & IN_RELOAD && UsesClipsForAmmo1() && !m_bInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}

	// -----------------------
	//  No buttons down
	// -----------------------
	if( !( ( pPlayer->m_nButtons & IN_ATTACK ) || ( pPlayer->m_nButtons & IN_ATTACK2 ) || ( pPlayer->m_nButtons & IN_RELOAD ) ) )
	{
		// no fire buttons down
		m_bFireOnEmpty = false;

		if( !HasAnyAmmo() && m_flNextPrimaryAttack < gpGlobals->curtime )
		{
			// weapon isn't useable, switch.
			if( !( GetFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY ) && pPlayer->SwitchToNextBestWeapon( this ) )
			{
				m_flNextPrimaryAttack = gpGlobals->curtime + 0.3;
				return;
			}
		}
		else
		{
			// weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
			if( m_iClip1 == 0 && !( GetFlags() & ITEM_FLAG_NOAUTORELOAD ) && m_flNextPrimaryAttack < gpGlobals->curtime )
			{
				Reload();
				return;
			}
		}

		WeaponIdle( );
		return;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Same as base reload but doesn't change the owner's next attack time. This
//			lets us zoom out while reloading. This hack is necessary because our
//			ItemPostFrame is only called when the owner's next attack time has
//			expired.
// Output : Returns true if the weapon was reloaded, false if no more ammo.
//-----------------------------------------------------------------------------
bool CWeaponSniperRifle::Reload( void )
{
	CBaseCombatCharacter* pOwner = GetOwner();
	if( !pOwner )
	{
		return false;
	}

	if( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) > 0 )
	{
		int primary		= MIN( GetMaxClip1() - m_iClip1, pOwner->GetAmmoCount( m_iPrimaryAmmoType ) );
		int secondary	= MIN( GetMaxClip2() - m_iClip2, pOwner->GetAmmoCount( m_iSecondaryAmmoType ) );

		if( primary > 0 || secondary > 0 )
		{
			CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
			if( pPlayer != NULL )
			{
				if( m_nZoomLevel != 0 )
				{
#ifdef GAME_DLL
					pPlayer->ShowViewModel( true );
#endif // GAME_DLL
					pPlayer->SetFOV( pPlayer, 0 );
					m_nZoomLevel = 0;
				}
			}
			else
			{
				return false;
			}

			// Play reload on different channel as it happens after every fire
			// and otherwise steals channel away from fire sound
			WeaponSound( RELOAD );
			SendWeaponAnim( ACT_VM_RELOAD );

			m_flNextPrimaryAttack	= gpGlobals->curtime + SequenceDuration();

			m_bInReload = true;
		}

		return true;
	}

	return false;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSniperRifle::PrimaryAttack( void )
{
	// Only the player fires this way so we can cast safely.
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( !pPlayer )
	{
		return;
	}

	if( gpGlobals->curtime >= m_flNextPrimaryAttack )
	{
		// If my clip is empty (and I use clips) start reload
		if( !m_iClip1 )
		{
			Reload();
			return;
		}

		// MUST call sound before removing a round from the clip of a CMachineGun dvs: does this apply to the sniper rifle? I don't know.
		WeaponSound( SINGLE );

		pPlayer->DoMuzzleFlash();

		SendWeaponAnim( ACT_VM_PRIMARYATTACK );

		// player "shoot" animation
		pPlayer->SetAnimation( PLAYER_ATTACK1 );

		ToHL2MPPlayer( pPlayer )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

		// Don't fire again until fire animation has completed
		m_flNextPrimaryAttack = gpGlobals->curtime + SequenceDuration();
		m_iClip1 = m_iClip1 - 1;

		FireBulletsInfo_t info;
		info.m_vecSrc	 = pPlayer->Weapon_ShootPosition( );
		info.m_vecDirShooting = pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
		info.m_iShots = SNIPER_BULLET_COUNT_PLAYER;
		info.m_flDistance = MAX_TRACE_LENGTH;
		info.m_iAmmoType = m_iPrimaryAmmoType;
		info.m_iTracerFreq = SNIPER_TRACER_FREQUENCY_PLAYER;
		info.m_vecSpread = GetBulletSpread();

		// Fire the bullets
		pPlayer->FireBullets( info );

#ifdef GAME_DLL
		CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 600, 0.2 );
#endif // GAME_DLL

		QAngle vecPunch( random->RandomFloat( -SNIPER_KICKBACK, SNIPER_KICKBACK ), 0, 0 );
		pPlayer->ViewPunch( vecPunch );

		// Indicate out of ammo condition if we run out of ammo.
		if( !m_iClip1 && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
		{
			pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
		}
	}

#ifdef GAME_DLL
	// Register a muzzleflash for the AI.
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
#endif // GAME_DLL
}


//-----------------------------------------------------------------------------
// Purpose: Zooms in using the sniper rifle scope.
//-----------------------------------------------------------------------------
void CWeaponSniperRifle::Zoom( void )
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( !pPlayer )
	{
		return;
	}

	if( m_nZoomLevel >= sizeof( g_nZoomFOV ) / sizeof( g_nZoomFOV[0] ) )
	{
#ifdef GAME_DLL
		pPlayer->ShowViewModel( true );
#endif // GAME_DLL

		// Zoom out to the default zoom level
		WeaponSound( SPECIAL2 );
		pPlayer->SetFOV( pPlayer, 0 );
		m_nZoomLevel = 0;
	}
	else
	{
		if( m_nZoomLevel == 0 )
#ifdef GAME_DLL
			pPlayer->ShowViewModel( false );
#endif // GAME_DLL

		WeaponSound( SPECIAL1 );
		pPlayer->SetFOV( pPlayer, g_nZoomFOV[m_nZoomLevel] );
		m_nZoomLevel++;
	}

	m_fNextZoom = gpGlobals->curtime + SNIPER_ZOOM_RATE;
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSniperRifle::Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_SNIPER_RIFLE_FIRE:
		{
			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition();

			CAI_BaseNPC* npc = pOperator->MyNPCPointer();
			Vector vecSpread;
			if( npc )
			{
				vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );
				vecSpread = VECTOR_CONE_PRECALCULATED;
			}
			else
			{
				AngleVectors( pOperator->GetLocalAngles(), &vecShootDir );
				vecSpread = GetBulletSpread();
			}
			WeaponSound( SINGLE_NPC );
			pOperator->FireBullets( SNIPER_BULLET_COUNT_NPC, vecShootOrigin, vecShootDir, vecSpread, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, SNIPER_TRACER_FREQUENCY_NPC );
			pOperator->DoMuzzleFlash();
			break;
		}

		default:
		{
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
		}
	}
}
#endif // GAME_DLL