//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef CLIENT_DLL
	#define CWeaponSMG1 C_WeaponSMG1
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar    sk_plr_dmg_smg1_grenade;
#ifdef MAPBASE
	extern ConVar    sk_npc_dmg_smg1_grenade;
#endif

#define SMG1_GRENADE_DAMAGE 100.0f
#define SMG1_GRENADE_RADIUS 250.0f

class CWeaponSMG1 : public CHL2MPMachineGun
{
public:
	DECLARE_CLASS( CWeaponSMG1, CHL2MPMachineGun );

	CWeaponSMG1();

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	void	Precache( void );
	void	AddViewKick( void );
	void	SecondaryAttack( void );

	int		GetMinBurst()
	{
		return 2;
	}
	int		GetMaxBurst()
	{
		return 5;
	}

	virtual void Equip( CBaseCombatCharacter* pOwner );
	bool	Reload( void );

	float	GetFireRate( void )
	{
		return 0.075f;    // 13.3hz
	}
#ifdef GAME_DLL
	int		CapabilitiesGet( void )
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1;
	}
	int		WeaponRangeAttack2Condition( float flDot, float flDist );
#endif // GAME_DLL
	Activity	GetPrimaryAttackActivity( void );

	virtual const Vector& GetBulletSpread( void )
	{
		static const Vector cone = VECTOR_CONE_5DEGREES;
		return cone;
	}

	const WeaponProficiencyInfo_t* GetProficiencyValues();

#ifdef GAME_DLL
	void FireNPCPrimaryAttack( CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir );
	void Operator_ForceNPCFire( CBaseCombatCharacter*  pOperator, bool bSecondary );
	void Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator );
#endif // GAME_DLL

	DECLARE_ACTTABLE();

protected:

	CNetworkVector( m_vecTossVelocity );
	CNetworkVar( float, m_flNextGrenadeCheck );

private:
	CWeaponSMG1( const CWeaponSMG1& );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponSMG1, DT_WeaponSMG1 )

BEGIN_NETWORK_TABLE( CWeaponSMG1, DT_WeaponSMG1 )
#ifdef CLIENT_DLL
	RecvPropFloat( RECVINFO( m_flNextGrenadeCheck ) ),
	RecvPropVector( RECVINFO( m_vecTossVelocity ) ),
#else
	SendPropFloat( SENDINFO( m_flNextGrenadeCheck ) ),
	SendPropVector( SENDINFO( m_vecTossVelocity ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponSMG1 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_smg1, CWeaponSMG1 );
PRECACHE_WEAPON_REGISTER( weapon_smg1 );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponSMG1::m_acttable[] =
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

IMPLEMENT_ACTTABLE( CWeaponSMG1 );

#ifdef MAPBASE
// Allows Weapon_BackupActivity() to access the SMG1's activity table.
acttable_t* GetSMG1Acttable()
{
	return CWeaponSMG1::m_acttable;
}

int GetSMG1ActtableCount()
{
	return ARRAYSIZE( CWeaponSMG1::m_acttable );
}
#endif

//=========================================================
CWeaponSMG1::CWeaponSMG1( )
{
	m_fMinRange1		= 0;// No minimum range.
	m_fMaxRange1		= 1400;

	m_bAltFiresUnderwater = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::Precache( void )
{
#ifdef GAME_DLL
	UTIL_PrecacheOther( "grenade_ar2" );
#endif // GAME_DLL

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Give this weapon longer range when wielded by an ally NPC.
//-----------------------------------------------------------------------------
void CWeaponSMG1::Equip( CBaseCombatCharacter* pOwner )
{
#ifdef GAME_DLL
	if( pOwner->Classify() == CLASS_PLAYER_ALLY )
	{
		m_fMaxRange1 = 3000;
	}
	else
#endif // GAME_DLL
	{
		m_fMaxRange1 = 1400;
	}

	BaseClass::Equip( pOwner );
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::FireNPCPrimaryAttack( CBaseCombatCharacter* pOperator, Vector& vecShootOrigin, Vector& vecShootDir )
{
	// FIXME: use the returned number of bullets to account for >10hz firerate
	WeaponSoundRealtime( SINGLE_NPC );

	CSoundEnt::InsertSound( SOUND_COMBAT | SOUND_CONTEXT_GUNFIRE, pOperator->GetAbsOrigin(), SOUNDENT_VOLUME_MACHINEGUN, 0.2, pOperator, SOUNDENT_CHANNEL_WEAPON, pOperator->GetEnemy() );
	pOperator->FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_PRECALCULATED,
							MAX_TRACE_LENGTH, m_iPrimaryAmmoType, 2, entindex(), 0 );

	pOperator->DoMuzzleFlash();
	m_iClip1 = m_iClip1 - 1;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::Operator_ForceNPCFire( CBaseCombatCharacter* pOperator, bool bSecondary )
{
	// Ensure we have enough rounds in the clip
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle	angShootDir;
	GetAttachment( LookupAttachment( "muzzle" ), vecShootOrigin, angShootDir );
	AngleVectors( angShootDir, &vecShootDir );
	FireNPCPrimaryAttack( pOperator, vecShootOrigin, vecShootDir );
}

#ifdef MAPBASE
	float GetCurrentGravity( void );
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_SMG1:
		{
			Vector vecShootOrigin, vecShootDir;
			QAngle angDiscard;

			// Support old style attachment point firing
			if( ( pEvent->options == NULL ) || ( pEvent->options[0] == '\0' ) || ( !pOperator->GetAttachment( pEvent->options, vecShootOrigin, angDiscard ) ) )
			{
				vecShootOrigin = pOperator->Weapon_ShootPosition();
			}

			CAI_BaseNPC* npc = pOperator->MyNPCPointer();
			ASSERT( npc != NULL );
			vecShootDir = npc->GetActualShootTrajectory( vecShootOrigin );

			FireNPCPrimaryAttack( pOperator, vecShootOrigin, vecShootDir );
		}
		break;

#ifdef MAPBASE
		case EVENT_WEAPON_AR2_ALTFIRE:
		{
			WeaponSound( WPN_DOUBLE );

			CAI_BaseNPC* npc = pOperator->MyNPCPointer();
			if( !npc )
			{
				return;
			}

			Vector vecShootOrigin, vecShootDir;
			vecShootOrigin = pOperator->Weapon_ShootPosition();
			vecShootDir = npc->GetShootEnemyDir( vecShootOrigin );

			Vector vecTarget = npc->GetAltFireTarget();
			Vector vecThrow;
			if( vecTarget == vec3_origin )
			{
				AngleVectors( npc->EyeAngles(), &vecThrow );    // Not much else to do, unfortunately
			}
			else
			{
				// Because this is happening right now, we can't "VecCheckThrow" and can only "VecDoThrow", you know what I mean?
				// ...Anyway, this borrows from that so we'll never return vec3_origin.
				//vecThrow = VecCheckThrow( this, vecShootOrigin, vecTarget, 600.0, 0.5 );

				vecThrow = ( vecTarget - vecShootOrigin );

				// throw at a constant time
				float time = vecThrow.Length() / 600.0;
				vecThrow = vecThrow * ( 1.0 / time );

				// adjust upward toss to compensate for gravity loss
				vecThrow.z += ( GetCurrentGravity() * 0.5 ) * time * 0.5;
			}

			CGrenadeAR2* pGrenade = ( CGrenadeAR2* )Create( "grenade_ar2", vecShootOrigin, vec3_angle, npc );
			pGrenade->SetAbsVelocity( vecThrow );
			pGrenade->SetLocalAngularVelocity( QAngle( 0, 400, 0 ) );
			pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );

			pGrenade->SetThrower( npc );

			pGrenade->SetGravity( 0.5 ); // lower gravity since grenade is aerodynamic and engine doesn't know it.

			pGrenade->SetDamage( sk_npc_dmg_smg1_grenade.GetFloat() );

			variant_t var;
			var.SetEntity( pGrenade );
			npc->FireNamedOutput( "OnThrowGrenade", var, pGrenade, npc );
		}
		break;
#else
			/*//FIXME: Re-enable
			case EVENT_WEAPON_AR2_GRENADE:
		{
		CAI_BaseNPC *npc = pOperator->MyNPCPointer();

		Vector vecShootOrigin, vecShootDir;
		vecShootOrigin = pOperator->Weapon_ShootPosition();
		vecShootDir = npc->GetShootEnemyDir( vecShootOrigin );

		Vector vecThrow = m_vecTossVelocity;

		CGrenadeAR2 *pGrenade = (CGrenadeAR2*)Create( "grenade_ar2", vecShootOrigin, vec3_angle, npc );
		pGrenade->SetAbsVelocity( vecThrow );
		pGrenade->SetLocalAngularVelocity( QAngle( 0, 400, 0 ) );
		pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY );
		pGrenade->m_hOwner			= npc;
		pGrenade->m_pMyWeaponAR2	= this;
		pGrenade->SetDamage(sk_npc_dmg_ar2_grenade.GetFloat());

		// FIXME: arrgg ,this is hard coded into the weapon???
		m_flNextGrenadeCheck = gpGlobals->curtime + 6;// wait six seconds before even looking again to see if a grenade can be thrown.

		m_iClip2--;
		}
		break;
		*/
#endif

		default:
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
// Output : Activity
//-----------------------------------------------------------------------------
Activity CWeaponSMG1::GetPrimaryAttackActivity( void )
{
	if( m_nShotsFired < 2 )
	{
		return ACT_VM_PRIMARYATTACK;
	}

	if( m_nShotsFired < 3 )
	{
		return ACT_VM_RECOIL1;
	}

	if( m_nShotsFired < 4 )
	{
		return ACT_VM_RECOIL2;
	}

	return ACT_VM_RECOIL3;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
bool CWeaponSMG1::Reload( void )
{
	bool fRet;
	float fCacheTime = m_flNextSecondaryAttack;

	fRet = DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );
	if( fRet )
	{
		// Undo whatever the reload process has done to our secondary
		// attack timer. We allow you to interrupt reloading to fire
		// a grenade.
		m_flNextSecondaryAttack = GetOwner()->m_flNextAttack = fCacheTime;

		WeaponSound( RELOAD );
		ToHL2MPPlayer( GetOwner() )->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );

	}

	return fRet;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::AddViewKick( void )
{
#define	EASY_DAMPEN			0.5f
#define	MAX_VERTICAL_KICK	1.0f	//Degrees
#define	SLIDE_LIMIT			2.0f	//Seconds

	//Get the view kick
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer == NULL )
	{
		return;
	}

	DoMachineGunKick( pPlayer, EASY_DAMPEN, MAX_VERTICAL_KICK, m_fFireDuration, SLIDE_LIMIT );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponSMG1::SecondaryAttack( void )
{
	// Only the player fires this way so we can cast
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer == NULL )
	{
		return;
	}

	//Must have ammo
	if( ( pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) <= 0 ) || ( pPlayer->GetWaterLevel() == 3 ) )
	{
		SendWeaponAnim( ACT_VM_DRYFIRE );
		BaseClass::WeaponSound( EMPTY );
		m_flNextSecondaryAttack = gpGlobals->curtime + 0.5f;
		return;
	}

	if( m_bInReload )
	{
		m_bInReload = false;
	}

	// MUST call sound before removing a round from the clip of a CMachineGun
	BaseClass::WeaponSound( WPN_DOUBLE );

#ifdef GAME_DLL
	pPlayer->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAGS_NONE );
#endif // GAME_DLL

	Vector vecSrc = pPlayer->Weapon_ShootPosition();
	Vector	vecThrow;
	// Don't autoaim on grenade tosses
	AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &vecThrow );
	VectorScale( vecThrow, 1000.0f, vecThrow );

#ifdef GAME_DLL
	//Create the grenade
	CGrenadeAR2* pGrenade = ( CGrenadeAR2* )Create( "grenade_ar2", vecSrc, vec3_angle, pPlayer );
	if( pGrenade )
	{
		pGrenade->SetAbsVelocity( vecThrow );

		pGrenade->SetLocalAngularVelocity( RandomAngle( -400, 400 ) );
		pGrenade->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
		pGrenade->SetThrower( GetOwner() );
		pGrenade->SetDamage( sk_plr_dmg_smg1_grenade.GetFloat() );
	}
#endif // GAME_DLL

	SendWeaponAnim( ACT_VM_SECONDARYATTACK );

#ifdef GAME_DLL
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 1000, 0.2, GetOwner(), SOUNDENT_CHANNEL_WEAPON );
#endif // GAME_DLL

	// player "shoot" animation
#ifdef MAPBASE
	pPlayer->SetAnimation( PLAYER_ATTACK2 );
#else
	pPlayer->SetAnimation( PLAYER_ATTACK1 );
#endif

	ToHL2MPPlayer( pPlayer )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_SECONDARY );

	// Decrease ammo
	pPlayer->RemoveAmmo( 1, m_iSecondaryAmmoType );

	// Can shoot again immediately
	m_flNextPrimaryAttack = gpGlobals->curtime + 0.5f;

	// Can blow up after a short delay (so have time to release mouse button)
	m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;

#ifdef GAME_DLL
	// Register a muzzleflash for the AI.
	pPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );

	m_iSecondaryAttacks++;
	gamestats->Event_WeaponFired( pPlayer, false, GetClassname() );
#endif // GAME_DLL
}

#ifdef GAME_DLL
#define	COMBINE_MIN_GRENADE_CLEAR_DIST 256

//-----------------------------------------------------------------------------
// Purpose:
// Input  : flDot -
//			flDist -
// Output : int
//-----------------------------------------------------------------------------
int CWeaponSMG1::WeaponRangeAttack2Condition( float flDot, float flDist )
{
	CAI_BaseNPC* npcOwner = GetOwner()->MyNPCPointer();

	return COND_NONE;

	/*
		// --------------------------------------------------------
		// Assume things haven't changed too much since last time
		// --------------------------------------------------------
		if (gpGlobals->curtime < m_flNextGrenadeCheck )
			return m_lastGrenadeCondition;
	*/

	// -----------------------
	// If moving, don't check.
	// -----------------------
	if( npcOwner->IsMoving() )
	{
		return COND_NONE;
	}

	CBaseEntity* pEnemy = npcOwner->GetEnemy();

	if( !pEnemy )
	{
		return COND_NONE;
	}

	Vector vecEnemyLKP = npcOwner->GetEnemyLKP();
	if( !( pEnemy->GetFlags() & FL_ONGROUND ) && pEnemy->GetWaterLevel() == 0 && vecEnemyLKP.z > ( GetAbsOrigin().z + WorldAlignMaxs().z ) )
	{
		//!!!BUGBUG - we should make this check movetype and make sure it isn't FLY? Players who jump a lot are unlikely to
		// be grenaded.
		// don't throw grenades at anything that isn't on the ground!
		return COND_NONE;
	}

	// --------------------------------------
	//  Get target vector
	// --------------------------------------
	Vector vecTarget;
	if( random->RandomInt( 0, 1 ) )
	{
		// magically know where they are
		vecTarget = pEnemy->WorldSpaceCenter();
	}
	else
	{
		// toss it to where you last saw them
		vecTarget = vecEnemyLKP;
	}
	// vecTarget = m_vecEnemyLKP + (pEnemy->BodyTarget( GetLocalOrigin() ) - pEnemy->GetLocalOrigin());
	// estimate position
	// vecTarget = vecTarget + pEnemy->m_vecVelocity * 2;


	if( ( vecTarget - npcOwner->GetLocalOrigin() ).Length2D() <= COMBINE_MIN_GRENADE_CLEAR_DIST )
	{
		// crap, I don't want to blow myself up
		m_flNextGrenadeCheck = gpGlobals->curtime + 1; // one full second.
		return ( COND_NONE );
	}

	// ---------------------------------------------------------------------
	// Are any friendlies near the intended grenade impact area?
	// ---------------------------------------------------------------------
	CBaseEntity* pTarget = NULL;

	while( ( pTarget = gEntList.FindEntityInSphere( pTarget, vecTarget, COMBINE_MIN_GRENADE_CLEAR_DIST ) ) != NULL )
	{
		//Check to see if the default relationship is hatred, and if so intensify that
		if( npcOwner->IRelationType( pTarget ) == D_LI )
		{
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextGrenadeCheck = gpGlobals->curtime + 1; // one full second.
			return ( COND_WEAPON_BLOCKED_BY_FRIEND );
		}
	}

	// ---------------------------------------------------------------------
	// Check that throw is legal and clear
	// ---------------------------------------------------------------------
	// FIXME: speed is based on difficulty...

	Vector vecToss = VecCheckThrow( this, npcOwner->GetLocalOrigin() + Vector( 0, 0, 60 ), vecTarget, 600.0, 0.5 );
	if( vecToss != vec3_origin )
	{
		m_vecTossVelocity = vecToss;

		// don't check again for a while.
		// JAY: HL1 keeps checking - test?
		//m_flNextGrenadeCheck = gpGlobals->curtime;
		m_flNextGrenadeCheck = gpGlobals->curtime + 0.3; // 1/3 second.
		return COND_CAN_RANGE_ATTACK2;
	}
	else
	{
		// don't check again for a while.
		m_flNextGrenadeCheck = gpGlobals->curtime + 1; // one full second.
		return COND_WEAPON_SIGHT_OCCLUDED;
	}
}
#endif // GAME_DLL

//-----------------------------------------------------------------------------
const WeaponProficiencyInfo_t* CWeaponSMG1::GetProficiencyValues()
{
	static WeaponProficiencyInfo_t proficiencyTable[] =
	{
		{ 7.0,		0.75	},
		{ 5.00,		0.75	},
		{ 10.0 / 3.0, 0.75	},
		{ 5.0 / 3.0,	0.75	},
		{ 1.00,		1.0		},
	};

	COMPILE_TIME_ASSERT( ARRAYSIZE( proficiencyTable ) == WEAPON_PROFICIENCY_PERFECT + 1 );

	return proficiencyTable;
}
