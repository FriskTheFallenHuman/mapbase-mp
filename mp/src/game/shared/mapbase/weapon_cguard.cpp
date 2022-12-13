//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:		Combine guard gun, strider destroyer
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "npcevent.h"
#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "gamerules.h"		// For g_pGameRules
#include "in_buttons.h"
#include "rumble_shared.h"
#include "vstdlib/random.h"
#include "gamestats.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "basecombatcharacter.h"
	#include "ai_basenpc.h"
	#include "hl2mp_player.h"
	#include "soundent.h"
	#include "concussive_explosion.h"
#endif

#ifdef CLIENT_DLL
	#define CWeaponCGuard C_WeaponCGuard
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CWeaponCGuard : public CBaseHL2MPCombatWeapon
{
public:
	DECLARE_CLASS( CWeaponCGuard, CBaseHL2MPCombatWeapon );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponCGuard( void );
	
	void Precache( void );
	void PrimaryAttack( void );
	void AddViewKick( void );
	void DelayedFire( void );
	void ItemPostFrame( void );
	void AlertTargets( void );
	void UpdateLasers( void );

#ifdef GAME_DLL
	int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
#endif // GAME_DLL

	DECLARE_ACTTABLE();

protected:
	CNetworkVar( float, m_flChargeTime );
	CNetworkVar( bool,	m_bFired );

	//int		m_beamIndex;
	//int		m_haloIndex;
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponCGuard, DT_WeaponCGuard )

BEGIN_NETWORK_TABLE( CWeaponCGuard, DT_WeaponCGuard )
#ifdef CLIENT_DLL
	RecvPropFloat( RECVINFO( m_flChargeTime ) ),
	RecvPropBool( RECVINFO( m_bFired ) ),
#else
	SendPropFloat( SENDINFO( m_flChargeTime ) ),
	SendPropBool( SENDINFO( m_bFired ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( CWeaponCGuard )
	DEFINE_PRED_FIELD( m_bFired, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flChargeTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_cguard, CWeaponCGuard );
PRECACHE_WEAPON_REGISTER( weapon_cguard );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t CWeaponCGuard::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_SMG1,			true },
	{ ACT_RELOAD,					ACT_RELOAD_SMG1,				true },
	{ ACT_IDLE,						ACT_IDLE_SMG1,					true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG1,			true },

	{ ACT_WALK,						ACT_WALK_RIFLE,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true  },
	
// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG1_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG1_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG1,			false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_RIFLE_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_RIFLE,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_RIFLE_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_RIFLE,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG1_RELAXED,			false },//never aims	
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_RIFLE_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG1,			false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_RIFLE_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_RIFLE_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_RIFLE,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_RIFLE_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_RIFLE_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_RIFLE,				false },//always aims
//End readiness activities

	{ ACT_WALK_AIM,					ACT_WALK_AIM_RIFLE,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_RIFLE,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_RIFLE,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SMG1,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG1_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_SMG1_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_SMG1_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG1_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG1,		true },

#if EXPANDED_HL2_WEAPON_ACTIVITIES
	{ ACT_ARM,						ACT_ARM_RIFLE,					false },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,				false },
#endif

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED,			ACT_RANGE_AIM_SMG1_MED,			false },
	{ ACT_RANGE_ATTACK1_MED,		ACT_RANGE_ATTACK_SMG1_MED,		false },

	{ ACT_COVER_WALL_R,				ACT_COVER_WALL_R_RIFLE,			false },
	{ ACT_COVER_WALL_L,				ACT_COVER_WALL_L_RIFLE,			false },
	{ ACT_COVER_WALL_LOW_R,			ACT_COVER_WALL_LOW_R_RIFLE,		false },
	{ ACT_COVER_WALL_LOW_L,			ACT_COVER_WALL_LOW_L_RIFLE,		false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SMG1,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SMG1,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SMG1,						false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_SMG1,					false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SMG1,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_SMG1, false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG1,	 false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SMG1,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SMG1,					false },
};

IMPLEMENT_ACTTABLE( CWeaponCGuard );

//-----------------------------------------------------------------------------
// Constructor
//-----------------------------------------------------------------------------
CWeaponCGuard::CWeaponCGuard( void )
{
	m_flNextPrimaryAttack	= gpGlobals->curtime;
	m_flChargeTime			= gpGlobals->curtime;
	m_bFired				= true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::Precache( void )
{
#ifdef GAME_DLL
	UTIL_PrecacheOther( "concussiveblast" );
#endif

	//m_beamIndex = PrecacheModel( "sprites/bluelaser1.vmt" );
	//m_haloIndex = PrecacheModel( "sprites/blueshaft1.vmt" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::AlertTargets( void )
{
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	// Fire the bullets
	Vector vecSrc	 = pPlayer->Weapon_ShootPosition( );
	Vector vecAiming;

	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecAiming );
	VectorScale( vecAiming, 1000.0f, vecAiming );

	Vector	impactPoint	= vecSrc + ( vecAiming * MAX_TRACE_LENGTH );

	trace_t	tr;

	UTIL_TraceLine( vecSrc, impactPoint, MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr );
	
	if ( ( vecSrc-tr.endpos ).Length() > 1024 )
		return;

#ifdef GAME_DLL
	CSoundEnt::InsertSound( SOUND_DANGER, tr.endpos, 128, 0.5f );
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
/*
void CWeaponCGuard::UpdateLasers( void )
{
	//Only update the lasers whilst charging
	if ( ( m_flChargeTime < gpGlobals->curtime ) || ( m_bFired ) )
		return;

	Vector	start, end, v_forward, v_right, v_up;

	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

	pPlayer->GetVectors( &v_forward, &v_right, &v_up );

	//Get the position of the laser
	start = pPlayer->Weapon_ShootPosition( );

	start += ( v_forward * 8.0f ) + ( v_right * 3.0f ) + ( v_up * -2.0f );

	end = start + ( v_forward * MAX_TRACE_LENGTH );

	float	angleOffset = ( 1.0f - ( m_flChargeTime - gpGlobals->curtime ) ) / 1.0f;
	Vector	offset[4];

	offset[0] = Vector( 0.0f,  0.5f, -0.5f );
	offset[1] = Vector( 0.0f,  0.5f,  0.5f );
	offset[2] = Vector( 0.0f, -0.5f, -0.5f );
	offset[3] = Vector( 0.0f, -0.5f,  0.5f );

	QAngle  v_ang;
	Vector	v_dir;

	angleOffset *= 2.0f;

	if ( angleOffset > 1.0f )
		angleOffset = 1.0f;

	for ( int i = 0; i < 4; i++ )
	{
		Vector	ofs = start + ( v_forward * offset[i][0] ) + ( v_right * offset[i][1] ) + ( v_up * offset[i][2] );

		float hScale = ( offset[i][1] <= 0.0f ) ? 1.0f : -1.0f;
		float vScale = ( offset[i][2] <= 0.0f ) ? 1.0f : -1.0f;

		VectorAngles( v_forward, v_ang );
		v_ang[PITCH] = UTIL_AngleMod( v_ang[PITCH] + ( (1.0f-angleOffset) * 15.0f * vScale ) );
		v_ang[YAW] = UTIL_AngleMod( v_ang[YAW] + ( (1.0f-angleOffset) * 15.0f * hScale ) );

		AngleVectors( v_ang, &v_dir );

		trace_t	tr;
		UTIL_TraceLine( ofs, ofs + ( v_dir * MAX_TRACE_LENGTH ), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

		UTIL_Beam( ofs, tr.endpos, m_beamIndex, 0, 0, 2.0f, 0.1f, 2, 0, 1, 0, 255, 255, 255, 32, 100 );
		
		UTIL_Beam( ofs, tr.endpos, m_haloIndex, 0, 0, 2.0f, 0.1f, 4, 0, 1, 16, 255, 255, 255, 8, 100 );
	}
}
*/

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::PrimaryAttack( void )
{
	if ( m_flChargeTime >= gpGlobals->curtime )
		return;
		
	AlertTargets();

	WeaponSound( SPECIAL1 );

	//UTIL_ScreenShake( GetAbsOrigin(), 10.0f, 100.0f, 2.0f, 128, SHAKE_START, false );

	m_flChargeTime	= gpGlobals->curtime + 1.0f;
	m_bFired		= false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::ItemPostFrame( void )
{
	//FIXME: UpdateLasers();

	if ( ( m_flChargeTime < gpGlobals->curtime ) && ( m_bFired == false ) )
	{
		DelayedFire();
	}

	BaseClass::ItemPostFrame();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::DelayedFire( void )
{
	if ( m_flChargeTime >= gpGlobals->curtime )
		return;

	if ( m_bFired )
		return;

	m_bFired = true;

	// Only the player fires this way so we can cast
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );
	if ( pPlayer == NULL )
		return;
	
	// Abort here to handle burst and auto fire modes
	if ( ( GetMaxClip1() != -1 && m_iClip1 == 0 ) || ( GetMaxClip1() == -1 && !pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) ) )
		return;

	// MUST call sound before removing a round from the clip of a CMachineGun
	BaseClass::WeaponSound( SINGLE );

#ifdef GAME_DLL
	pPlayer->RumbleEffect( RUMBLE_AR2, 0, RUMBLE_FLAG_LOOP );

	pPlayer->DoMuzzleFlash();

	// Register a muzzleflash for the AI.
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );	
#endif // GAME_DLL

	// Make sure we don't fire more than the amount in the clip, if this weapon uses clips
	m_iClip1 = m_iClip1 - 1;

	// Fire the bullets
	Vector vecSrc	 = pPlayer->Weapon_ShootPosition();
	Vector vecAiming;

	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecAiming );
	VectorScale( vecAiming, 1000.0f, vecAiming );

	//Factor in the view kick
	AddViewKick();

	Vector	impactPoint	= vecSrc + ( vecAiming * MAX_TRACE_LENGTH );

	trace_t	tr;
	UTIL_TraceHull( vecSrc, impactPoint, Vector( -2, -2, -2 ), Vector( 2, 2, 2 ), MASK_SHOT, pPlayer, COLLISION_GROUP_NONE, &tr );

#ifdef GAME_DLL
	CreateConcussiveBlast( tr.endpos, tr.plane.normal, this, 1.0 );
#endif // GAME_DLL

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	// player "shoot" animation
	pPlayer->SetAnimation( PLAYER_ATTACK1 );

	ToHL2MPPlayer( pPlayer )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	// Decrease ammo
	pPlayer->RemoveAmmo( 1, m_iPrimaryAmmoType );

	// Can shoot again immediately
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

#ifdef GAME_DLL
	m_iPrimaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, false, GetClassname() );
#endif // GAME_DLL
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponCGuard::AddViewKick( void )
{
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if ( pPlayer == NULL )
		return;

#ifdef GAME_DLL
	color32 white = {255, 255, 255, 64};
	UTIL_ScreenFade( pPlayer, white, 0.1, 0, FFADE_IN  );
#endif // GAME_DLL

	//Disorient the player
	QAngle angles;

	angles.x = SharedRandomFloat( "cguardpax", 5.0f, 5.0f );
	angles.y = SharedRandomFloat( "cguardpay", -8.0f, -8.0f );
	angles.z = 0.0f;

	pPlayer->ViewPunch( angles );
}