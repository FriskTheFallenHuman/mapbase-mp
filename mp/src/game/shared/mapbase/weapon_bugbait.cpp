//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "gamestats.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	#include "c_te_effect_dispatch.h"
#else
	#include "hl2mp_player.h"
	#include "te_effect_dispatch.h"
	#include "grenade_bugbait.h"
	#include "antlion_maker.h"
	#include "items.h"
	#include "soundent.h"
#endif


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef GAME_DLL
extern ConVar g_CV_SmokeTrail; // temporary dust explosion switch
#endif // GAME_DLL

#define BUGBAIT_RADIUS	4.0f // inches

#ifdef CLIENT_DLL
	#define CWeaponBugBait C_WeaponBugBait
#endif

//-----------------------------------------------------------------------------
// Fragmentation grenades
//-----------------------------------------------------------------------------
class CWeaponBugBait: public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponBugBait, CBaseHL2MPCombatWeapon );
public:
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

public:
	CWeaponBugBait();

	void	Precache( void );
#ifdef GAME_DLL
	void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif
	void	Drop( const Vector &vecVelocity );
	void	OnPickedUp( CBaseCombatCharacter *pNewOwner );
	void	PrimaryAttack( void );
	void	SecondaryAttack( void );
	void	ItemPostFrame( void );

	bool	Deploy( void );
	bool	Holster( CBaseCombatWeapon *pSwitchingTo = NULL );

	bool	HasAnyAmmo( void ) { return true; }
	
	bool	Reload( void );

	bool	ShouldDisplayHUDHint() { return true; }

	void	ThrowBugBait( CBasePlayer *pPlayer );

	// check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
	void	CheckThrowPosition( CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc,
		const QAngle& angles = vec3_angle );

	void	SetSporeEmitterState( bool state = true );

private:

	CWeaponBugBait( const CWeaponBugBait & );

	CNetworkVar( bool,	m_bRedraw );	//Draw the weapon again after throwing a grenade
	CNetworkVar( bool,	m_fDrawbackFinished );
	CNetworkVar( bool,	m_bEmitSpores );
#ifdef GAME_DLL
	EHANDLE		m_hSporeTrail;
#endif // GAME_DLL

	DECLARE_ACTTABLE();
};

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponBugBait::m_acttable[] = 
{
	// HL2:DM activities (for third-person animations in SP)
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

IMPLEMENT_ACTTABLE( CWeaponBugBait );

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponBugBait, DT_WeaponBugBait )

BEGIN_NETWORK_TABLE( CWeaponBugBait, DT_WeaponBugBait )

#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bRedraw ) ),
	RecvPropBool( RECVINFO( m_fDrawbackFinished ) ),
	RecvPropBool( RECVINFO( m_bEmitSpores ) ),
#else
	SendPropBool( SENDINFO( m_bRedraw ) ),
	SendPropBool( SENDINFO( m_fDrawbackFinished ) ),
	SendPropBool( SENDINFO( m_bEmitSpores ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponBugBait )
	DEFINE_PRED_FIELD( m_bRedraw, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_fDrawbackFinished, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bEmitSpores, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_bugbait, CWeaponBugBait );
PRECACHE_WEAPON_REGISTER( weapon_bugbait );

void DropPrimedBugBait( CHL2MP_Player *pPlayer, CBaseCombatWeapon *pWeapon )
{
	CWeaponBugBait *pBugBait = dynamic_cast<CWeaponBugBait*>( pWeapon );

	if ( pBugBait )
	{
		pBugBait->ThrowBugBait( pPlayer );
		//pBugBait->DecrementAmmo( pPlayer );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponBugBait::CWeaponBugBait( void ) : CBaseHL2MPCombatWeapon()
{
	m_bRedraw = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::Precache( void )
{
	BaseClass::Precache();

#ifndef CLIENT_DLL
	UTIL_PrecacheOther( "npc_grenade_bugbait" );
#endif

	PrecacheScriptSound( "WeaponBugBait.Throw" );
	PrecacheScriptSound( "WeaponBugBait.Splat" );
	PrecacheScriptSound( "WeaponBugBait.Roll" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::Drop( const Vector &vecVelocity )
{
	BaseClass::Drop( vecVelocity );

#ifdef GAME_DLL
	// -------------
	// Spore Explosion.
	// -------------
	if( g_CV_SmokeTrail.GetInt() )
	{
		m_hSporeTrail = SporeExplosion::CreateSporeExplosion();
		
		if( m_hSporeTrail )
		{
			SporeExplosion *pSporeExplosion = (SporeExplosion *)m_hSporeTrail.Get();

			QAngle	angles;
			VectorAngles( Vector(0,0,1), angles );

			pSporeExplosion->SetAbsAngles( angles );
			pSporeExplosion->SetAbsOrigin( GetAbsOrigin() );
			pSporeExplosion->SetParent( this );

			pSporeExplosion->m_flSpawnRate			= 16.0f;
			pSporeExplosion->m_flParticleLifetime	= 0.5f;
			pSporeExplosion->SetRenderColor( 0.0f, 0.5f, 0.25f, 0.15f );

			pSporeExplosion->m_flStartSize			= 32;
			pSporeExplosion->m_flEndSize			= 48;
			pSporeExplosion->m_flSpawnRadius		= 4;

			pSporeExplosion->SetLifetime( 9999 );
		}
	}
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponBugBait::Deploy( void )
{
	m_bRedraw = false;
	m_fDrawbackFinished = false;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponBugBait::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_bRedraw = false;
	m_fDrawbackFinished = false;

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : true - 
//-----------------------------------------------------------------------------
void CWeaponBugBait::SetSporeEmitterState( bool state )
{
	m_bEmitSpores = state;
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEvent - 
//			*pOperator - 
//-----------------------------------------------------------------------------
void CWeaponBugBait::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	bool fThrewBugBait = false;

	switch( pEvent->event )
	{
		case EVENT_WEAPON_SEQUENCE_FINISHED:
			m_fDrawbackFinished = true;
			break;

		case EVENT_WEAPON_THROW:
			ThrowBugBait( pOwner );
			//DecrementAmmo( pOwner );
			fThrewBugBait = true;
			break;

		default:
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}

	if( fThrewBugBait )
	{
		m_flNextPrimaryAttack	= gpGlobals->curtime + 0.5;
		m_flTimeWeaponIdle = FLT_MAX; //NOTE: This is set once the animation has finished up!
	}
}

#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponBugBait::Reload( void )
{
	if ( ( m_bRedraw ) && ( m_flNextPrimaryAttack <= gpGlobals->curtime ) )
	{
		//Redraw the weapon
		SendWeaponAnim( ACT_VM_DRAW );

		//Update our times
		m_flNextPrimaryAttack	= gpGlobals->curtime + SequenceDuration();
		m_flTimeWeaponIdle = gpGlobals->curtime + SequenceDuration();

		//Mark this as done
		m_bRedraw = false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::OnPickedUp( CBaseCombatCharacter *pNewOwner )
{
	BaseClass::OnPickedUp( pNewOwner );

#ifdef GAME_DLL
	if ( m_hSporeTrail )
		UTIL_Remove( m_hSporeTrail );
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::SecondaryAttack( void )
{
	if ( m_bRedraw )
		return;

	CBaseCombatCharacter *pOwner  = GetOwner();
	if ( pOwner == NULL )
		return;

	CBasePlayer *pPlayer = ToBasePlayer( pOwner );
	if ( pPlayer == NULL )
		return;

	// Squeeze!
	CPASAttenuationFilter filter( this );

	EmitSound( filter, entindex(), "Weapon_Bugbait.Splat" );

#ifdef GAME_DLL
	if ( CGrenadeBugBait::ActivateBugbaitTargets( GetOwner(), GetAbsOrigin(), true ) == false )
	{
		g_AntlionMakerManager.BroadcastFollowGoal( GetOwner() );
	}
#endif // GAME_DLL

	SendWeaponAnim( ACT_VM_SECONDARYATTACK );
	m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();

#ifdef GAME_DLL
	m_iSecondaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, false, GetClassname() );
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::PrimaryAttack( void )
{
	if ( m_bRedraw )
		return;

	CBaseCombatCharacter *pOwner  = GetOwner();
	if ( pOwner == NULL )
		return;

	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );;
	if ( !pPlayer )
		return;

	// Note that this is a primary attack and prepare the grenade attack to pause.
	SendWeaponAnim( ACT_VM_HAULBACK );
	
	// Put both of these off indefinitely. We do not know how long
	// the player will hold the grenade.
	m_flTimeWeaponIdle = FLT_MAX;
	m_flNextPrimaryAttack = FLT_MAX;

#ifdef GAME_DLL
	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, true, GetClassname() );
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponBugBait::ItemPostFrame( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return;

#ifdef MAPBASE
	if ( pOwner->HasSpawnFlags( SF_PLAYER_SUPPRESS_FIRING ) )
	{
		WeaponIdle();
		return;
	}
#endif

	if( m_fDrawbackFinished )
	{
		if( !( pOwner->m_nButtons & IN_ATTACK ) )
		{
			SendWeaponAnim( ACT_VM_THROW );
			ToHL2MPPlayer( pOwner )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

			//Tony; fire the sequence
			m_fDrawbackFinished = false;
		}
	}
	else
	{
		if ( ( pOwner->m_nButtons & IN_ATTACK ) && ( m_flNextPrimaryAttack < gpGlobals->curtime ) )
			PrimaryAttack();
	}

	BaseClass::ItemPostFrame();

	if ( m_bRedraw )
	{
		if ( IsViewModelSequenceFinished() )
		{
			Reload();
		}
	}
}

//---------------------------------------------------------------------------------------------------
// Purpose: check a throw from vecSrc.  If not valid, move the position back along the line to vecEye
//---------------------------------------------------------------------------------------------------
void CWeaponBugBait::CheckThrowPosition( CBasePlayer *pPlayer, const Vector &vecEye, Vector &vecSrc, const QAngle& angles )
{
	// Compute an extended AABB that takes into account the requested bugbait rotation.
	// This will prevent bugbait from going through nearby solids when model initially intersects with any.
	matrix3x4_t rotation;
	AngleMatrix(angles, rotation);
	Vector mins, maxs;
	RotateAABB(rotation, -Vector(BUGBAIT_RADIUS + 2, BUGBAIT_RADIUS + 2, 2),
		Vector(BUGBAIT_RADIUS + 2, BUGBAIT_RADIUS + 2, BUGBAIT_RADIUS * 2 + 2), mins, maxs);
	trace_t tr;
	UTIL_TraceHull( vecEye, vecSrc, mins, maxs, pPlayer->PhysicsSolidMaskForEntity(),
		pPlayer, pPlayer->GetCollisionGroup(), &tr );
	
	if ( tr.DidHit() )
	{
		vecSrc = tr.endpos;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//-----------------------------------------------------------------------------
void CWeaponBugBait::ThrowBugBait( CBasePlayer *pPlayer )
{
#ifndef CLIENT_DLL
	Vector	vecEye = pPlayer->EyePosition();
	Vector	vForward, vRight;

	pPlayer->EyeVectors( &vForward, &vRight, NULL );
	Vector vecSrc = vecEye + vForward * 18.0f + vRight * 8.0f;
	CheckThrowPosition( pPlayer, vecEye, vecSrc );
//	vForward[0] += 0.1f;
	vForward[2] += 0.1f;

	Vector vecThrow;
	pPlayer->GetVelocity( &vecThrow, NULL );
	vecThrow += vForward * 1200;

	CGrenadeBugBait *pBugbait = BugBaitGrenade_Create( vecSrc, vec3_angle, vecThrow, QAngle( 600, random->RandomInt( -1200,1200 ), 0 ), pPlayer );
	if ( pBugbait != NULL )
	{
		// If the shot is clear to the player, give the missile a grace period
		trace_t	tr;
		UTIL_TraceLine( pPlayer->EyePosition(), pPlayer->EyePosition() + ( vForward * 128 ), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
		
		if ( tr.fraction == 1.0 )
			pBugbait->SetGracePeriod( 0.1f );
	}
#endif

	m_bRedraw = true;

	WeaponSound( SINGLE );
	
	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

#ifdef GAME_DLL
	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, true, GetClassname() );
#endif // GAME_DLL
}