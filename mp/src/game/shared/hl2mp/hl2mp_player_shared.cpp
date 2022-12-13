//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "weapon_hl2mpbase.h"
#include "datacache/imdlcache.h"
#include "takedamageinfo.h"
#include "hl2mp_gamerules.h"
#include "engine/IEngineSound.h"
#include "SoundEmitterSystem/isoundemittersystembase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	#include "prediction.h"
#else
	#include "hl2mp_player.h"
	#include "ilagcompensationmanager.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar sv_footsteps;

const char *g_ppszPlayerSoundPrefixNames[PLAYER_SOUNDS_MAX] =
{
	"NPC_Citizen",
	"NPC_CombineS",
	"NPC_MetroPolice",
};

const char *CHL2MP_Player::GetPlayerModelSoundPrefix( void )
{
	return g_ppszPlayerSoundPrefixNames[m_iPlayerSoundType];
}

extern void SpawnBlood( Vector vecSpot, const Vector &vecDir, int bloodColor, float flDamage );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator )
{
	Vector vecOrigin = ptr->endpos - vecDir * 4;

	if ( m_takedamage )
	{
#ifdef GAME_DLL
		if ( pAccumulator )
		{
			pAccumulator->AccumulateMultiDamage( info, this );
		}
		else
#endif // GAME_DLL
		{
			AddMultiDamage( info, this );
		}

		int blood = BloodColor();

		CBaseEntity *pAttacker = info.GetAttacker();
		if ( pAttacker )
		{
			if ( HL2MPRules()->IsTeamplay() && pAttacker->InSameTeam( this ) == true )
				return;
		}
		
#if defined(MAPBASE) && defined(GAME_DLL)
		if ( blood != DONT_BLEED && DamageFilterAllowsBlood( info ) )
#else
		if ( blood != DONT_BLEED )
#endif
		{
			SpawnBlood( vecOrigin, vecDir, blood, info.GetDamage() );// a little surface blood.
			TraceBleed( info.GetDamage(), vecDir, ptr, info.GetDamageType() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::DoImpactEffect( trace_t &tr, int nDamageType )
{
	if ( GetActiveWeapon() )
	{
		GetActiveWeapon()->DoImpactEffect( tr, nDamageType );
		return;
	}

	BaseClass::DoImpactEffect( tr, nDamageType );
}

#ifndef CLIENT_DLL
void TE_PlayerAnimEvent( CBasePlayer* pPlayer, PlayerAnimEvent_t playerAnim, int nData );
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
#ifdef CLIENT_DLL
	if ( IsLocalPlayer() )
	{
		if ( ( prediction->InPrediction() && !prediction->IsFirstTimePredicted() ) )
			return;
	}

	MDLCACHE_CRITICAL_SECTION();
#endif

	m_PlayerAnimState->DoAnimationEvent( event, nData );

#ifndef CLIENT_DLL
	TE_PlayerAnimEvent( this, event, nData );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CStudioHdr *CHL2MP_Player::OnNewModel( void )
{
	CStudioHdr *hdr = BaseClass::OnNewModel();

#ifdef CLIENT_DLL
	InitializePoseParams();
#endif

	// Reset the players animation states, gestures
	if ( m_PlayerAnimState )
		m_PlayerAnimState->OnNewModel();

	return hdr;
}

//-----------------------------------------------------------------------------
// Consider the weapon's built-in accuracy, this character's proficiency with
// the weapon, and the status of the target. Use this information to determine
// how accurately to shoot at the target.
//-----------------------------------------------------------------------------
Vector CHL2MP_Player::GetAttackSpread( CBaseCombatWeapon *pWeapon, CBaseEntity *pTarget )
{
	if ( pWeapon )
		return pWeapon->GetBulletSpread( WEAPON_PROFICIENCY_PERFECT );
	
	return VECTOR_CONE_15DEGREES;
}

//-----------------------------------------------------------------------------
// Purpose: multiplayer does not do autoaiming.
//-----------------------------------------------------------------------------
Vector CHL2MP_Player::GetAutoaimVector( float flScale )
{
	//No Autoaim
	Vector	forward;
	AngleVectors( EyeAngles() + m_Local.m_vecPunchAngle, &forward );
	return	forward;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : step - 
//			fvol - 
//			force - force sound to play
//-----------------------------------------------------------------------------
void CHL2MP_Player::PlayStepSound( Vector &vecOrigin, surfacedata_t *psurface, float fvol, bool force )
{
	if ( gpGlobals->maxClients > 1 && !sv_footsteps.GetFloat() )
		return;

#if defined( CLIENT_DLL )
	// during prediction play footstep sounds only once
	if ( !prediction->IsFirstTimePredicted() )
		return;
#endif

	if ( GetFlags() & FL_DUCKING )
		return;

	BaseClass::PlayStepSound( vecOrigin, psurface, fvol, force );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : collisionGroup - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHL2MP_Player::ShouldCollide( int collisionGroup, int contentsMask ) const
{
	if ( HL2MPRules()->IsTeamplay() )
	{
		if ( collisionGroup == COLLISION_GROUP_PLAYER_MOVEMENT || collisionGroup == COLLISION_GROUP_PROJECTILE )
		{
			switch( GetTeamNumber() )
			{
			case TEAM_REBELS:
				if ( !( contentsMask & CONTENTS_TEAM2 ) )
					return false;
				break;

			case TEAM_COMBINE:
				if ( !( contentsMask & CONTENTS_TEAM1 ) )
					return false;
				break;
			}
		}
	}

	return BaseClass::ShouldCollide( collisionGroup, contentsMask );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : eyeOrigin - 
//			eyeAngles - 
//			zNear - 
//			zFar - 
//			fov - 
//-----------------------------------------------------------------------------
void CHL2MP_Player::CalcView( Vector &eyeOrigin, QAngle &eyeAngles, float &zNear, float &zFar, float &fov )
{
#ifdef CLIENT_DLL
	if ( m_lifeState != LIFE_ALIVE && !IsObserver() )
	{
		Vector origin = EyePosition();			

		IRagdoll *pRagdoll = GetRepresentativeRagdoll();

		if ( pRagdoll )
		{
			origin = pRagdoll->GetRagdollOrigin();
			origin.z += VEC_DEAD_VIEWHEIGHT_SCALED( this ).z; // look over ragdoll, not through
		}

		BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );

		eyeOrigin = origin;
		
		Vector vForward; 
		AngleVectors( eyeAngles, &vForward );

		VectorNormalize( vForward );
		VectorMA( origin, -CHASE_CAM_DISTANCE_MAX, vForward, eyeOrigin );

		Vector WALL_MIN( -WALL_OFFSET, -WALL_OFFSET, -WALL_OFFSET );
		Vector WALL_MAX( WALL_OFFSET, WALL_OFFSET, WALL_OFFSET );

		trace_t trace; // clip against world
		C_BaseEntity::PushEnableAbsRecomputations( false ); // HACK don't recompute positions while doing RayTrace
		UTIL_TraceHull( origin, eyeOrigin, WALL_MIN, WALL_MAX, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &trace );
		C_BaseEntity::PopEnableAbsRecomputations();

		if (trace.fraction < 1.0)
		{
			eyeOrigin = trace.endpos;
		}
		
		return;
	}
#endif // CLIENT_DLL

	BaseClass::CalcView( eyeOrigin, eyeAngles, zNear, zFar, fov );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHL2MP_Player::FireBullets( const FireBulletsInfo_t& info )
{
#ifdef GAME_DLL
	// Move other players back to history positions based on local player's lag
	lagcompensation->StartLagCompensation( this, this->GetCurrentCommand() );
#endif // GAME_DLL

#ifdef GAME_DLL
	NoteWeaponFired();
#endif // GAME_DLL

	BaseClass::FireBullets( info );

#ifdef GAME_DLL
	// Move other players back to history positions based on local player's lag
	lagcompensation->FinishLagCompensation( this );
#endif // GAME_DLL
}
