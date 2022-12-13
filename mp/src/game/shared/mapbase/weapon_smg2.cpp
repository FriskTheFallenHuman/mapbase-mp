//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_hl2mpbase.h"
#include "weapon_hl2mpbase_selectfiremachinegun.h"
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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
	#define CWeaponSMG2 C_WeaponSMG2
#endif

class CWeaponSMG2 : public CHL2MPSelectFireMachineGun
{
public:
	DECLARE_CLASS(CWeaponSMG2, CHL2MPSelectFireMachineGun);

	CWeaponSMG2();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	const Vector& GetBulletSpread(void);

	virtual void Precache(void);
	virtual void AddViewKick(void);
#ifdef GAME_DLL
	virtual void Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator );
#endif // GAME_DLL
	virtual float GetFireRate( void ) { return 0.1f; }
#ifdef GAME_DLL
	virtual int CapabilitiesGet( void ) { return bits_CAP_WEAPON_RANGE_ATTACK1; }
#endif // GAME_DLL

	DECLARE_ACTTABLE();

private:
	CWeaponSMG2( const CWeaponSMG2 & );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSMG2, DT_WeaponSMG2 )

BEGIN_NETWORK_TABLE( CWeaponSMG2, DT_WeaponSMG2 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSMG2 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_smg2, CWeaponSMG2 );
PRECACHE_WEAPON_REGISTER( weapon_smg2 );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponSMG2::m_acttable[] = 
{
	{ ACT_RANGE_ATTACK1, ACT_RANGE_ATTACK_SMG2, true },

#if EXPANDED_HL2_UNUSED_WEAPON_ACTIVITIES
	// Optional new NPC activities
	// (these should fall back to SMG animations when they don't exist on an NPC)
	{ ACT_RELOAD,					ACT_RELOAD_SMG2,			true },
	{ ACT_IDLE,						ACT_IDLE_SMG2,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_SMG2,		true },

// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_SMG2_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_SMG2_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_SMG2,			false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_SMG2_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_SMG2_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_SMG2,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_SMG2_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_SMG2_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_SMG2,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_SMG2_RELAXED,			false },//never aims	
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_SMG2_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_SMG2,			false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_SMG2_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_SMG2_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_SMG2,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_SMG2_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_SMG2_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_SMG2,				false },//always aims
//End readiness activities

	{ ACT_WALK,						ACT_WALK_SMG2,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_SMG2,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,			true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,		true },
	{ ACT_RUN,						ACT_RUN_SMG2,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_SMG2,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,			true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,		true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_SMG2,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_SMG2_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_SMG2_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_SMG2_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_SMG2_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_SMG2,		true },

	{ ACT_ARM,						ACT_ARM_RIFLE,					false },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,				false },

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED,			ACT_RANGE_AIM_SMG2_MED,			false },
	{ ACT_RANGE_ATTACK1_MED,		ACT_RANGE_ATTACK_SMG2_MED,		false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_SMG2,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_SMG2,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_SMG2,						false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_SMG2,					false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_SMG2,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG2,	false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_SMG2, false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_SMG2,	 false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RELOAD_SMG2,			false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RELOAD_SMG2,			false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_SMG2,					false },
#endif
};

IMPLEMENT_ACTTABLE( CWeaponSMG2 );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponSMG2::CWeaponSMG2( )
{
	m_fMaxRange1 = 2000;
	m_fMinRange1 = 32;

	m_iFireMode = FIREMODE_3RNDBURST;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSMG2::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const Vector &CWeaponSMG2::GetBulletSpread( void )
{
	static const Vector cone = VECTOR_CONE_10DEGREES;
	return cone;
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSMG2::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_SMG2:
		{
			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition( );

			CAI_BaseNPC *npc = pOperator->MyNPCPointer();
			ASSERT( npc != NULL );
			vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

			WeaponSound( SINGLE_NPC );
			pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED, MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2 );
			pOperator->DoMuzzleFlash();
			m_iClip1 = m_iClip1 - 1;
		}
		break;
		default:
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSMG2::AddViewKick( void )
{
	#define	EASY_DAMPEN			0.5f
	#define	MAX_VERTICAL_KICK	2.0f	//Degrees
	#define	SLIDE_LIMIT			1.0f	//Seconds
	
	//Get the view kick
	CBasePlayer *pPlayer = ToBasePlayer( GetOwner() );

	if (!pPlayer)
		return;

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}
