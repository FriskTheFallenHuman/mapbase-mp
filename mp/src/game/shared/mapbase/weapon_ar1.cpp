//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbase.h"
#include "weapon_hl2mpbase_machinegun.h"
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
	#include "grenade_ar2.h"
	#include "ai_memory.h"
	#include "soundent.h"
	#include "hl2mp_player.h"
	#include "basegrenade_shared.h"
#endif

#ifdef CLIENT_DLL
	#define CWeaponAR1 C_WeaponAR1
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define DAMAGE_PER_SECOND 10

#define MAX_SETTINGS	5

float RateOfFire[ MAX_SETTINGS ] =
{
	0.1,
	0.2,
	0.5,
	0.7,
	1.0,
};

float Damage[ MAX_SETTINGS ] =
{
	2,
	4,
	10,
	14,
	20,
};

#ifdef MAPBASE
	extern acttable_t* GetAR2Acttable();
	extern int GetAR2ActtableCount();
#endif

//=========================================================
//=========================================================
class CWeaponAR1 : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponAR1, CHL2MPMachineGun );

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponAR1();

	bool	Deploy( void );

	float GetFireRate( void )
	{
		return RateOfFire[ m_ROF ];
	}

#ifdef GAME_DLL
	int CapabilitiesGet( void )
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1;
	}
#endif // GAME_DLL

	void SecondaryAttack( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static const Vector cone = VECTOR_CONE_10DEGREES;
		return cone;
	}

#ifdef GAME_DLL
	void Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator )
	{
		switch( pEvent->event )
		{
			case EVENT_WEAPON_AR1:
			{
				Vector vecShootOrigin, vecShootDir;
				vecShootOrigin = pOperator->Weapon_ShootPosition( );

				CAI_BaseNPC* npc = pOperator->MyNPCPointer();
				ASSERT( npc != NULL );

				vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

				WeaponSound( SINGLE_NPC );
				pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2 );
				pOperator->DoMuzzleFlash();
			}
			break;
			default:
				BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
				break;
		}
	}
#endif // GAME_DLL

#ifdef MAPBASE
	virtual acttable_t*		GetBackupActivityList()
	{
		return GetAR2Acttable();
	}
	virtual int				GetBackupActivityListCount()
	{
		return GetAR2ActtableCount();
	}
#endif

	DECLARE_ACTTABLE();

protected:

	CNetworkVar( int, m_ROF );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponAR1, DT_WeaponAR1 )

BEGIN_NETWORK_TABLE( CWeaponAR1, DT_WeaponAR1 )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_ROF ) ),
#else
	SendPropInt( SENDINFO( m_ROF ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponAR1 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_ar1, CWeaponAR1 );
PRECACHE_WEAPON_REGISTER( weapon_ar1 );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponAR1::m_acttable[] =
{
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_AR1,		true },

#if EXPANDED_HL2_UNUSED_WEAPON_ACTIVITIES
	// Optional new NPC activities
	// (these should fall back to AR2 animations when they don't exist on an NPC)
	{ ACT_RELOAD,					ACT_RELOAD_AR1,				true },
	{ ACT_IDLE,						ACT_IDLE_AR1,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_AR1,			true },

// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_AR1_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_AR1_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_AR1,				false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_AR1_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_AR1_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_AR1,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_AR1_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_AR1_STIMULATED,			false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_AR1,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_AR1_RELAXED,			false },//never aims
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_AR1_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_AR1,				false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_AR1_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_AR1_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_AR1,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_AR1_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_AR1_STIMULATED,		false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_AR1,				false },//always aims
//End readiness activities

	{ ACT_WALK,						ACT_WALK_AR1,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_AR1,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_AR1,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_AR1,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_AR1,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_AR1_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_AR1_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_AR1_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_AR1_LOW,				false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_AR1,			true },

	{ ACT_ARM,						ACT_ARM_RIFLE,					false },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,				false },

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED,			ACT_RANGE_AIM_AR1_MED,			false },
	{ ACT_RANGE_ATTACK1_MED,		ACT_RANGE_ATTACK_AR1_MED,		false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_AR1,						false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_AR1,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_AR1,						false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_AR1,						false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_AR1,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR1,		false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_AR1,	false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_AR1,		false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_AR1,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_AR1,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_AR1,						false },
#endif
};

IMPLEMENT_ACTTABLE( CWeaponAR1 );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CWeaponAR1::CWeaponAR1( )
{
	m_ROF = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponAR1::Deploy( void )
{
	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponAR1::SecondaryAttack( void )
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( pPlayer )
	{
		pPlayer->m_nButtons &= ~IN_ATTACK2;
	}

	m_flNextSecondaryAttack = gpGlobals->curtime + 0.1;

	m_ROF += 1;

	if( m_ROF == MAX_SETTINGS )
	{
		m_ROF = 0;
	}

	int i;

	Msg( "\n" );
	for( i = 0 ; i < MAX_SETTINGS ; i++ )
	{
		if( i == m_ROF )
		{
			Msg( "|" );
		}
		else
		{
			Msg( "-" );
		}
	}
	Msg( "\n" );
}
