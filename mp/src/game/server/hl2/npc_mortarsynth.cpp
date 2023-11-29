//======= Copyright © 2022, Obsidian Conflict Team, All rights reserved. =======//
//
// Purpose: Mortar Synth NPC
//
//==============================================================================//

#include "cbase.h"
#include "npcevent.h"

#include "ai_interactions.h"
#include "ai_hint.h"
#include "ai_moveprobe.h"
#include "ai_squad.h"
#include "ai_motor.h"
#include "beam_shared.h"
#include "globalstate.h"
#include "soundent.h"
#include "gib.h"
#include "IEffects.h"
#include "ai_route.h"
#include "npc_mortarsynth.h"
#include "prop_combine_ball.h"
#include "explode.h"
#include "te_effect_dispatch.h"
#include "particle_parse.h"
#include "grenade_energy.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar	sk_mortarsynth_health( "sk_mortarsynth_health", "0" );
ConVar	sk_mortarsynth_mortar_rate( "sk_mortarsynth_mortar_rate", "1.8" );
ConVar	sk_mortarsynth_repulse_rate( "sk_mortarsynth_repulse_rate", "1.0" );

extern ConVar sk_energy_grenade_radius;

#define MORTARSYNTH_FIRE		( 1 )

//-----------------------------------------------------------------------------
// Mortarsynth activities
//-----------------------------------------------------------------------------
Activity ACT_MSYNTH_FLINCH_BACK;
Activity ACT_MSYNTH_FLINCH_FRONT;
Activity ACT_MSYNTH_FLINCH_LEFT;
Activity ACT_MSYNTH_FLINCH_RIGHT;
Activity ACT_MSYNTH_FLINCH_BIG_BACK;
Activity ACT_MSYNTH_FLINCH_BIG_FRONT;
Activity ACT_MSYNTH_FLINCH_BIG_LEFT;
Activity ACT_MSYNTH_FLINCH_BIG_RIGHT;

BEGIN_DATADESC( CNPC_Mortarsynth )

DEFINE_EMBEDDED( m_KilledInfo ),

				 DEFINE_FIELD( m_fNextFlySoundTime,		FIELD_TIME ),
				 DEFINE_FIELD( m_flNextRepulse,		FIELD_TIME ),
				 DEFINE_FIELD( m_pSmokeTrail,			FIELD_CLASSPTR ),
				 DEFINE_FIELD( m_flNextFlinch,			FIELD_TIME ),

				 END_DATADESC()


				 LINK_ENTITY_TO_CLASS( npc_mortarsynth, CNPC_Mortarsynth );


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CNPC_Mortarsynth::CNPC_Mortarsynth()
{
	m_flAttackNearDist = sk_energy_grenade_radius.GetFloat() + 50;
	m_flAttackFarDist = 500;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::Spawn( void )
{
	Precache();

	BaseClass::Spawn();

	SetModel( STRING( GetModelName() ) );

	SetHullType( HULL_LARGE_CENTERED );
	SetHullSizeNormal();

	SetMoveType( MOVETYPE_STEP );

	SetBloodColor( BLOOD_COLOR_YELLOW );

	m_iHealth = sk_mortarsynth_health.GetFloat();
	m_iMaxHealth = m_iHealth;

	m_flFieldOfView	= VIEW_FIELD_WIDE;

	m_fNextFlySoundTime = 0;
	m_flNextRepulse = 0;
}
//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
int CNPC_Mortarsynth::OnTakeDamage_Alive( const CTakeDamageInfo& info )
{
	if( m_flNextFlinch < gpGlobals->curtime )
	{
		// Fenix: Horrible hack to get proper light and heavy flinches
		Activity aFlinchFront, aFlinchBack, aFlinchLeft, aFlinchRight;

		if( IsHeavyDamage( info ) )
		{
			aFlinchFront = ACT_MSYNTH_FLINCH_BIG_FRONT;
			aFlinchBack = ACT_MSYNTH_FLINCH_BIG_BACK;
			aFlinchLeft = ACT_MSYNTH_FLINCH_BIG_LEFT;
			aFlinchRight = ACT_MSYNTH_FLINCH_BIG_RIGHT;
		}
		else
		{
			aFlinchFront = ACT_MSYNTH_FLINCH_FRONT;
			aFlinchBack = ACT_MSYNTH_FLINCH_BACK;
			aFlinchLeft = ACT_MSYNTH_FLINCH_LEFT;
			aFlinchRight = ACT_MSYNTH_FLINCH_RIGHT;
		}

		Vector vFacing = BodyDirection2D();
		Vector vDamageDir = ( info.GetDamagePosition() - WorldSpaceCenter() );

		vDamageDir.z = 0.0f;

		VectorNormalize( vDamageDir );

		Vector vDamageRight, vDamageUp;
		VectorVectors( vDamageDir, vDamageRight, vDamageUp );

		float damageDot = DotProduct( vFacing, vDamageDir );
		float damageRightDot = DotProduct( vFacing, vDamageRight );

		// See if it's in front
		if( damageDot > 0.0f )
		{
			// See if it's right
			if( damageRightDot > 0.0f )
			{
				SetActivity( ( Activity ) aFlinchRight );
			}
			else
			{
				SetActivity( ( Activity ) aFlinchLeft );
			}
		}
		else
		{
			// Otherwise it's from behind
			if( damageRightDot < 0.0f )
			{
				SetActivity( ( Activity ) aFlinchBack );
			}
			else
			{
				SetActivity( ( Activity ) aFlinchFront );
			}
		}
	}

	m_flNextFlinch = gpGlobals->curtime + 6;

	return ( BaseClass::OnTakeDamage_Alive( info ) );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : &info -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::IsHeavyDamage( const CTakeDamageInfo& info )
{
	// Struck by blast
	if( info.GetDamageType() & DMG_BLAST )
	{
		if( info.GetDamage() > 25.0f )
		{
			return true;
		}
	}

	// Struck by large object
	if( info.GetDamageType() & DMG_CRUSH )
	{
		IPhysicsObject* pPhysObject = info.GetInflictor()->VPhysicsGetObject();

		if( ( pPhysObject != NULL ) && ( pPhysObject->GetGameFlags() & FVPHYSICS_WAS_THROWN ) )
		{
			// Always take hits from a combine ball
			if( UTIL_IsAR2CombineBall( info.GetInflictor() ) )
			{
				return true;
			}

			// If we're under half health, stop being interrupted by heavy damage
			if( GetHealth() < ( GetMaxHealth() * 0.25 ) )
			{
				return false;
			}

			// Ignore physics damages that don't do much damage
			if( info.GetDamage() < 20.0f )
			{
				return false;
			}

			return true;
		}

		return false;
	}

	return false;
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CNPC_Mortarsynth::Gib( void )
{
	if( IsMarkedForDeletion() )
	{
		return;
	}

	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_carapace.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_core.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_engine.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_left_arm.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_left_pincer.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_right_arm.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_right_pincer.mdl" );
	CGib::SpawnSpecificGibs( this, 1, 1000, 800, "models/gibs/mortarsynth_tail.mdl" );

	BaseClass::Gib();
}

//-----------------------------------------------------------------------------
// Purpose: Overriden from npc_basescanner.cpp, to avoid DiveBomb attack
// Input  : pInflictor -
//			pAttacker -
//			flDamage -
//			bitsDamageType -
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::Event_Killed( const CTakeDamageInfo& info )
{
	// Copy off the takedamage info that killed me, since we're not going to call
	// up into the base class's Event_Killed() until we gib. (gibbing is ultimate death)
	m_KilledInfo = info;

	// Interrupt whatever schedule I'm on
	SetCondition( COND_SCHEDULE_DONE );

	StopLoopingSounds();

	Gib();
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : Type -
//-----------------------------------------------------------------------------
int CNPC_Mortarsynth::TranslateSchedule( int scheduleType )
{
	switch( scheduleType )
	{
		case SCHED_RANGE_ATTACK1:
			return SCHED_MORTARSYNTH_ATTACK;

		case SCHED_MELEE_ATTACK1:
			return SCHED_MORTARSYNTH_REPEL;
	}
	return BaseClass::TranslateSchedule( scheduleType );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::HandleAnimEvent( animevent_t* pEvent )
{
	if( pEvent->event == MORTARSYNTH_FIRE )
	{
		Vector vShootStart;
		QAngle vShootAng;

		GetAttachment( "mortar", vShootStart, vShootAng );

		CGrenadeEnergy::Shoot( this, vShootStart, m_vecTossVelocity, vShootAng );

		EmitSound( "NPC_Mortarsynth.Shoot" );

		return;
	}

	BaseClass::HandleAnimEvent( pEvent );
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
void CNPC_Mortarsynth::StopLoopingSounds( void )
{
	StopSound( "NPC_Mortarsynth.Hover" );
	StopSound( "NPC_Mortarsynth.HoverAlarm" );
	StopSound( "NPC_Mortarsynth.Shoot" );
	StopSound( "NPC_Mortarsynth.EnergyShoot" );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::Precache( void )
{
	if( !GetModelName() )
	{
		SetModelName( MAKE_STRING( "models/mortarsynth.mdl" ) );
	}

	PrecacheModel( STRING( GetModelName() ) );

	PrecacheModel( "models/gibs/mortarsynth_carapace.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_core.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_engine.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_left_arm.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_left_pincer.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_right_arm.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_right_pincer.mdl" );
	PrecacheModel( "models/gibs/mortarsynth_tail.mdl" );

	PrecacheScriptSound( "NPC_Mortarsynth.Hover" );
	PrecacheScriptSound( "NPC_Mortarsynth.HoverAlarm" );
	PrecacheScriptSound( "NPC_Mortarsynth.EnergyShoot" );
	PrecacheScriptSound( "NPC_Mortarsynth.Shoot" );

	PrecacheParticleSystem( "weapon_combine_ion_cannon_d" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pTask -
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::RunTask( const Task_t* pTask )
{
	switch( pTask->iTask )
	{
		// If my enemy has moved significantly, update my path
		case TASK_WAIT_FOR_MOVEMENT:
		{
			CBaseEntity* pEnemy = GetEnemy();
			if( pEnemy && IsCurSchedule( SCHED_MORTARSYNTH_CHASE_ENEMY ) && GetNavigator()->IsGoalActive() )
			{
				Vector vecEnemyPosition = pEnemy->EyePosition();
				if( GetNavigator()->GetGoalPos().DistToSqr( vecEnemyPosition ) > 40 * 40 )
				{
					GetNavigator()->UpdateGoalPos( vecEnemyPosition );
				}
			}
			BaseClass::RunTask( pTask );
			break;
		}

		default:
		{
			BaseClass::RunTask( pTask );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets the appropriate next schedule based on current condition
//			bits.
//-----------------------------------------------------------------------------
int CNPC_Mortarsynth::SelectSchedule( void )
{
	// -------------------------------
	// If I'm in a script sequence
	// -------------------------------
	if( m_NPCState == NPC_STATE_SCRIPT )
	{
		return( BaseClass::SelectSchedule() );
	}

	// -------------------------------
	// Flinch
	// -------------------------------
	if( HasCondition( COND_LIGHT_DAMAGE ) || HasCondition( COND_HEAVY_DAMAGE ) )
	{
		if( m_NPCState == NPC_STATE_ALERT )
		{
			if( m_iHealth < ( 3 * m_iMaxHealth / 4 ) )
			{
				return SCHED_TAKE_COVER_FROM_ENEMY;
			}
		}
	}

	// -------------------------------
	// If I'm directly blocked
	// -------------------------------
	if( HasCondition( COND_SCANNER_FLY_BLOCKED ) )
	{
		if( GetTarget() != NULL )
		{
			return SCHED_MORTARSYNTH_CHASE_TARGET;
		}
		else if( GetEnemy() != NULL )
		{
			return SCHED_MORTARSYNTH_CHASE_ENEMY;
		}
	}

	// ----------------------------------------------------------
	//  If I have an enemy
	// ----------------------------------------------------------
	if( GetEnemy() != NULL && GetEnemy()->IsAlive() )
	{
		if( GetTarget()	== NULL	)
		{
			// Patrol if the enemy has vanished
			if( HasCondition( COND_LOST_ENEMY ) )
			{
				return SCHED_MORTARSYNTH_PATROL;
			}

			// Use the repulse weapon if an enemy is too close to me
			if( CanRepelEnemy() )
			{
				return SCHED_MELEE_ATTACK1;
			}

			// Attack if it's time
			else if( gpGlobals->curtime >= m_flNextAttack )
			{
				if( CanGrenadeEnemy() )
				{
					return SCHED_RANGE_ATTACK1;
				}
			}
		}

		// Otherwise fly in low for attack
		return SCHED_MORTARSYNTH_HOVER;
	}

	// Default to patrolling around
	return SCHED_MORTARSYNTH_PATROL;
}

//-----------------------------------------------------------------------------
// Purpose: Called just before we are deleted.
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::UpdateOnRemove( void )
{
	StopLoopingSounds();

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : pTask -
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::StartTask( const Task_t* pTask )
{
	switch( pTask->iTask )
	{
		case TASK_ENERGY_REPEL:
		{
			UseRepulserWeapon();
			TaskComplete();
			break;
		}

		default:
			BaseClass::StartTask( pTask );
			break;
	}
}

//------------------------------------------------------------------------------
// Purpose:
//------------------------------------------------------------------------------
float CNPC_Mortarsynth::MinGroundDist( void )
{
	return 72;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : flInterval -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::OverrideMove( float flInterval )
{
	Vector vMoveTargetPos( 0, 0, 0 );
	CBaseEntity* pMoveTarget = NULL;

	if( !GetNavigator()->IsGoalActive() || ( GetNavigator()->GetCurWaypointFlags() & bits_WP_TO_PATHCORNER ) )
	{
		// Select move target
		if( GetTarget() != NULL )
		{
			pMoveTarget = GetTarget();
			// Select move target position
			vMoveTargetPos = GetTarget()->GetAbsOrigin();
		}
		else if( GetEnemy() != NULL )
		{
			pMoveTarget = GetEnemy();
			// Select move target position
			vMoveTargetPos = GetEnemy()->GetAbsOrigin();
		}
	}
	else
	{
		vMoveTargetPos = GetNavigator()->GetCurWaypointPos();
	}

	ClearCondition( COND_SCANNER_FLY_CLEAR );
	ClearCondition( COND_SCANNER_FLY_BLOCKED );

	// See if we can fly there directly
	if( pMoveTarget )
	{
		trace_t tr;
		AI_TraceHull( GetAbsOrigin(), vMoveTargetPos, GetHullMins(), GetHullMaxs(), MASK_NPCSOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &tr );

		if( tr.fraction != 1.0 )
		{
			SetCondition( COND_SCANNER_FLY_CLEAR );
		}
		else
		{
			SetCondition( COND_SCANNER_FLY_BLOCKED );
		}
	}

	// If I have a route, keep it updated and move toward target
	if( GetNavigator()->IsGoalActive() )
	{
		// -------------------------------------------------------------
		// If I'm on a path check LOS to my next node, and fail on path
		// if I don't have LOS.  Note this is the only place I do this,
		// so the manhack has to collide before failing on a path
		// -------------------------------------------------------------
		if( GetNavigator()->IsGoalActive() && !( GetNavigator()->GetPath()->CurWaypointFlags() & bits_WP_TO_PATHCORNER ) )
		{
			AIMoveTrace_t moveTrace;
			GetMoveProbe()->MoveLimit( NAV_FLY, GetAbsOrigin(), GetNavigator()->GetCurWaypointPos(), MASK_NPCSOLID, GetEnemy(), &moveTrace );

			if( IsMoveBlocked( moveTrace ) )
			{
				TaskFail( FAIL_NO_ROUTE );
				GetNavigator()->ClearGoal();
				return true;
			}
		}

		if( OverridePathMove( pMoveTarget, flInterval ) )
		{
			return true;
		}
	}
	// ----------------------------------------------
	//	If attacking
	// ----------------------------------------------
	else if( pMoveTarget )
	{
		MoveToAttack( flInterval );
	}
	// -----------------------------------------------------------------
	// If I don't have a route, just decelerate
	// -----------------------------------------------------------------
	else if( !GetNavigator()->IsGoalActive() )
	{
		float myDecay = 9.5;
		Decelerate( flInterval, myDecay );

		// -------------------------------------
		// If I have an enemy, turn to face him
		// -------------------------------------
		if( GetEnemy() )
		{
			TurnHeadToTarget( flInterval, GetEnemy()->EyePosition() );
		}
	}

	MoveExecute_Alive( flInterval );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Accelerates toward a given position.
// Input  : flInterval - Time interval over which to move.
//			vecMoveTarget - Position to move toward.
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::MoveToTarget( float flInterval, const Vector& vecMoveTarget )
{
	if( flInterval <= 0 )
	{
		return;
	}

	// Look at our inspection target if we have one
	if( GetEnemy() != NULL )
		// Otherwise at our enemy
	{
		TurnHeadToTarget( flInterval, GetEnemy()->EyePosition() );
	}
	else
		// Otherwise face our motion direction
	{
		TurnHeadToTarget( flInterval, vecMoveTarget );
	}

	// -------------------------------------
	// Move towards our target
	// -------------------------------------
	float myAccel;
	float myZAccel = 400.0f;
	float myDecay  = 0.15f;

	Vector vecCurrentDir;

	// Get the relationship between my current velocity and the way I want to be going.
	vecCurrentDir = GetCurrentVelocity();
	VectorNormalize( vecCurrentDir );

	Vector targetDir = vecMoveTarget - GetAbsOrigin();
	float flDist = VectorNormalize( targetDir );

	float flDot;
	flDot = DotProduct( targetDir, vecCurrentDir );

	if( flDot > 0.25 )
		// If my target is in front of me, my flight model is a bit more accurate.
	{
		myAccel = 250;
	}
	else
		// Have a harder time correcting my course if I'm currently flying away from my target.
	{
		myAccel = 128;
	}

	// Clamp lateral acceleration
	if( myAccel > flDist / flInterval )
	{
		myAccel = flDist / flInterval;
	}

	// Clamp vertical movement
	if( myZAccel > flDist / flInterval )
	{
		myZAccel = flDist / flInterval;
	}

	MoveInDirection( flInterval, targetDir, myAccel, myZAccel, myDecay );
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : flInterval -
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::MoveToAttack( float flInterval )
{
	if( GetEnemy() == NULL )
	{
		return;
	}

	if( flInterval <= 0 )
	{
		return;
	}

	TurnHeadToTarget( flInterval, GetEnemy()->EyePosition() );

	// -------------------------------------
	// Move towards our target
	// -------------------------------------
	float myAccel;
	float myZAccel = 400.0f;
	float myDecay  = 0.15f;

	Vector vecCurrentDir;

	// Get the relationship between my current velocity and the way I want to be going.
	vecCurrentDir = GetCurrentVelocity();
	VectorNormalize( vecCurrentDir );

	Vector targetDir = GetEnemy()->EyePosition() - GetAbsOrigin();
	float flDist = VectorNormalize( targetDir );

	float flDot;
	flDot = DotProduct( targetDir, vecCurrentDir );

	if( flDot > 0.25 )
		// If my target is in front of me, my flight model is a bit more accurate.
	{
		myAccel = 250;
	}
	else
		// Have a harder time correcting my course if I'm currently flying away from my target.
	{
		myAccel = 128;
	}

	// Clamp lateral acceleration
	if( myAccel > flDist / flInterval )
	{
		myAccel = flDist / flInterval;
	}

	// Clamp vertical movement
	if( myZAccel > flDist / flInterval )
	{
		myZAccel = flDist / flInterval;
	}

	Vector idealPos;

	if( flDist < m_flAttackNearDist )
	{
		idealPos = -targetDir;
	}
	else if( flDist > m_flAttackFarDist )
	{
		idealPos = targetDir;
	}

	MoveInDirection( flInterval, idealPos, myAccel, myZAccel, myDecay );
}

//-----------------------------------------------------------------------------
// Purpose: Turn head yaw into facing direction. Overriden from ai_basenpc_physicsflyer.cpp
// Input  : flInterval, MoveTarget
// Output :
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::TurnHeadToTarget( float flInterval, const Vector& MoveTarget )
{
	float desYaw = UTIL_AngleDiff( VecToYaw( MoveTarget - GetLocalOrigin() ), GetLocalAngles().y );

	// If I've flipped completely around, reverse angles
	float fYawDiff = m_fHeadYaw - desYaw;
	if( fYawDiff > 180 )
	{
		m_fHeadYaw -= 360;
	}
	else if( fYawDiff < -180 )
	{
		m_fHeadYaw += 360;
	}

	float iRate = 0.8;

	// Make frame rate independent
	float timeToUse = flInterval;
	while( timeToUse > 0 )
	{
		m_fHeadYaw = ( iRate * m_fHeadYaw ) + ( 1 - iRate ) * desYaw;
		timeToUse -= 0.1;
	}

	while( m_fHeadYaw > 360 )
	{
		m_fHeadYaw -= 360.0f;
	}

	while( m_fHeadYaw < 0 )
	{
		m_fHeadYaw += 360.f;
	}

	SetBoneController( 0, m_fHeadYaw );
}

//-----------------------------------------------------------------------------
// Purpose: Overriden from npc_basescanner.cpp
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::MoveExecute_Alive( float flInterval )
{
	// Amount of noise to add to flying
	float noiseScale = 3.0f;

	SetCurrentVelocity( GetCurrentVelocity() + VelocityToAvoidObstacles( flInterval ) );

	// Add time-coherent noise to the current velocity so that it never looks bolted in place.
	AddNoiseToVelocity( noiseScale );

	float maxSpeed = GetEnemy() ? ( GetMaxSpeed() * 2.0f ) : GetMaxSpeed();

	// Limit fall speed
	LimitSpeed( maxSpeed );

	QAngle angles = GetLocalAngles();

	// Make frame rate independent
	float	iRate	 = 0.5;
	float timeToUse = flInterval;

	while( timeToUse > 0 )
	{
		// Get the relationship between my current velocity and the way I want to be going.
		Vector vecCurrentDir = GetCurrentVelocity();
		VectorNormalize( vecCurrentDir );

		// calc relative banking targets
		Vector forward, right;
		GetVectors( &forward, &right, NULL );

		Vector m_vTargetBanking;

		m_vTargetBanking.x	= 40 * DotProduct( forward, vecCurrentDir );
		m_vTargetBanking.z	= 40 * DotProduct( right, vecCurrentDir );

		m_vCurrentBanking.x = ( iRate * m_vCurrentBanking.x ) + ( 1 - iRate ) * ( m_vTargetBanking.x );
		m_vCurrentBanking.z = ( iRate * m_vCurrentBanking.z ) + ( 1 - iRate ) * ( m_vTargetBanking.z );
		timeToUse = -0.1;
	}
	angles.x = m_vCurrentBanking.x;
	angles.z = m_vCurrentBanking.z;
	angles.y = 0;

	SetLocalAngles( angles );

	PlayFlySound();

	WalkMove( GetCurrentVelocity() * flInterval, MASK_NPCSOLID );
}

//-----------------------------------------------------------------------------
// Purpose: Handles movement towards the last move target. Overriden from npc_basescanner.cpp
// Input  : flInterval -
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::OverridePathMove( CBaseEntity* pMoveTarget, float flInterval )
{
	// Save our last patrolling direction
	Vector lastPatrolDir = GetNavigator()->GetCurWaypointPos() - GetAbsOrigin();

	// Continue on our path
	if( ProgressFlyPath( flInterval, pMoveTarget, ( MASK_NPCSOLID | CONTENTS_WATER ), !IsCurSchedule( SCHED_MORTARSYNTH_PATROL ) ) == AINPP_COMPLETE )
	{
		if( IsCurSchedule( SCHED_MORTARSYNTH_PATROL ) )
		{
			m_vLastPatrolDir = lastPatrolDir;
			VectorNormalize( m_vLastPatrolDir );
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::PlayFlySound( void )
{
	if( IsMarkedForDeletion() )
	{
		return;
	}

	if( gpGlobals->curtime > m_fNextFlySoundTime )
	{
		EmitSound( "NPC_Mortarsynth.Hover" );

		// If we are hurting, play some scary hover sounds :O
		if( GetHealth() < ( GetMaxHealth() / 3 ) )
		{
			EmitSound( "NPC_Mortarsynth.HoverAlarm" );
		}

		m_fNextFlySoundTime	= gpGlobals->curtime + 1.0;
	}
}

//---------------------------------------------------------------------------------
// Purpose : Checks if an enemy is within radius, in order to use the repulse wave
// Output : Returns true on success, false on failure.
//---------------------------------------------------------------------------------
bool CNPC_Mortarsynth::CanRepelEnemy()
{
	if( gpGlobals->curtime < m_flNextRepulse )
	{
		return false;
	}

	float flDist;
	flDist = ( GetEnemy()->GetAbsOrigin() - GetAbsOrigin() ).Length();

	if( flDist < sk_energy_grenade_radius.GetFloat() + 50 )
	{
		return true;
	}

	return false;
}

//------------------------------------------------------------------------------
// Purpose : Pushes away anything within a radius. Base code from point_push
//------------------------------------------------------------------------------
void CNPC_Mortarsynth::UseRepulserWeapon( void )
{
	CTakeDamageInfo	info;

	info.SetAttacker( this );
	info.SetInflictor( this );
	info.SetDamage( 10.0f );
	info.SetDamageType( DMG_DISSOLVE );

	// Get a collection of entities in a radius around us
	CBaseEntity* pEnts[256];
	int numEnts = UTIL_EntitiesInSphere( pEnts, 256, GetAbsOrigin(), m_flAttackNearDist, 0 );
	for( int i = 0; i < numEnts; i++ )
	{
		// Fenix: Failsafe
		if( pEnts[i] == NULL )
		{
			continue;
		}

		// Fenix: Dont push myself or other mortarsynths
		if( FClassnameIs( pEnts[i], "npc_mortarsynth" ) )
		{
			continue;
		}

		// Must be solid
		if( pEnts[i]->IsSolid() == false )
		{
			continue;
		}

		// Cannot be parented (only push parents)
		if( pEnts[i]->GetMoveParent() != NULL )
		{
			continue;
		}

		// Must be moveable
		if( pEnts[i]->GetMoveType() != MOVETYPE_VPHYSICS &&
				pEnts[i]->GetMoveType() != MOVETYPE_WALK &&
				pEnts[i]->GetMoveType() != MOVETYPE_STEP )
		{
			continue;
		}

		// Push it along
		PushEntity( pEnts[i] );

		pEnts[i]->TakeDamage( info );
	}

	DispatchParticleEffect( "weapon_combine_ion_cannon_d", GetAbsOrigin(), GetAbsAngles() );

	EmitSound( "NPC_Mortarsynth.EnergyShoot" );

	m_flNextRepulse = gpGlobals->curtime + sk_mortarsynth_repulse_rate.GetFloat();
}

//-----------------------------------------------------------------------------
// Purpose : Pushes away anything within a radius. Base code from point_push
//-----------------------------------------------------------------------------
void CNPC_Mortarsynth::PushEntity( CBaseEntity* pTarget )
{
	Vector vecPushDir;

	vecPushDir = ( pTarget->BodyTarget( GetAbsOrigin(), false ) - GetAbsOrigin() );

	switch( pTarget->GetMoveType() )
	{
		case MOVETYPE_NONE:
		case MOVETYPE_PUSH:
		case MOVETYPE_NOCLIP:
			break;

		case MOVETYPE_VPHYSICS:
		{
			IPhysicsObject* pPhys = pTarget->VPhysicsGetObject();
			if( pPhys )
			{
				// UNDONE: Assume the velocity is for a 100kg object, scale with mass
				pPhys->ApplyForceCenter( 10 * 1.0f * 100.0f * vecPushDir * pPhys->GetMass() * gpGlobals->frametime );
				return;
			}
		}
		break;

		case MOVETYPE_STEP:
		{
			// NPCs cannot be lifted up properly, they need to move in 2D
			vecPushDir.z = 0.0f;

			// NOTE: Falls through!
		}

		default:
		{
			Vector vecPush = ( 10 * vecPushDir * 1.0f );
			if( pTarget->GetFlags() & FL_BASEVELOCITY )
			{
				vecPush = vecPush + pTarget->GetBaseVelocity();
			}
			if( vecPush.z > 0 && ( pTarget->GetFlags() & FL_ONGROUND ) )
			{
				pTarget->SetGroundEntity( NULL );
				Vector origin = pTarget->GetAbsOrigin();
				origin.z += 1.0f;
				pTarget->SetAbsOrigin( origin );
			}

			pTarget->SetBaseVelocity( vecPush );
			pTarget->AddFlag( FL_BASEVELOCITY );
		}
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose : Checks if an enemy exists, in order to attack
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::CanGrenadeEnemy( bool bUseFreeKnowledge )
{
	CBaseEntity* pEnemy = GetEnemy();

	Assert( pEnemy != NULL );

	if( pEnemy )
	{
		if( bUseFreeKnowledge )
		{
			// throw to where we think they are.
			return CanThrowGrenade( GetEnemies()->LastKnownPosition( pEnemy ) );
		}
		else
		{
			// hafta throw to where we last saw them.
			return CanThrowGrenade( GetEnemies()->LastSeenPosition( pEnemy ) );
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Return true if the combine has grenades, hasn't checked lately, and
//			can throw a grenade at the target point.
// Input  : &vecTarget -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::CanThrowGrenade( const Vector& vecTarget )
{
	float flDist;
	flDist = ( vecTarget - GetAbsOrigin() ).Length();

	if( flDist > 2048 || flDist < sk_energy_grenade_radius.GetFloat() + 50 )
	{
		// Too close or too far!
		m_flNextAttack = gpGlobals->curtime + 1; // one full second.
		return false;
	}

	// ---------------------------------------------------------------------
	// Are any of my squad members near the intended grenade impact area?
	// ---------------------------------------------------------------------

	// Fenix: Disabled for us, as different NPCs that the mapper may place on the same squad can't interpret the sound warning, so the mortarsynth would hold fire forever
	// Feel free to re-enable if such problem doesn't exist on your mod

	/*if ( m_pSquad )
	{
		if ( m_pSquad->SquadMemberInRange( vecTarget, sk_energy_grenade_radius.GetFloat() + 50 ) )
		{
			// crap, I might blow my own guy up. Don't throw a grenade and don't check again for a while.
			m_flNextAttack = gpGlobals->curtime + 1; // one full second.

			// Tell my squad members to clear out so I can get a grenade in
			CSoundEnt::InsertSound( SOUND_MOVE_AWAY | SOUND_CONTEXT_COMBINE_ONLY, vecTarget, 300, 0.1 );
			return false;
		}
	}*/

	return CheckCanThrowGrenade( vecTarget );
}


//-----------------------------------------------------------------------------
// Purpose: Returns true if the combine can throw a grenade at the specified target point
// Input  : &vecTarget -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CNPC_Mortarsynth::CheckCanThrowGrenade( const Vector& vecTarget )
{
	// ---------------------------------------------------------------------
	// Check that throw is legal and clear
	// ---------------------------------------------------------------------
	Vector vecToss;
	Vector vecMins = -Vector( 4, 4, 4 );
	Vector vecMaxs = Vector( 4, 4, 4 );

	// Have to try a high toss. Do I have enough room?
	trace_t tr;
	AI_TraceLine( EyePosition(), EyePosition() + Vector( 0, 0, 64 ), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

	if( tr.fraction != 1.0 )
	{
		return false;
	}

	vecToss = VecCheckToss( this, EyePosition(), vecTarget, -1, 1.0, true, &vecMins, &vecMaxs );

	if( vecToss != vec3_origin )
	{
		m_vecTossVelocity = vecToss;

		// don't check again for a while.
		m_flNextAttack = gpGlobals->curtime + sk_mortarsynth_mortar_rate.GetFloat();
		return true;
	}
	else
	{
		// don't check again for a while.
		m_flNextAttack = gpGlobals->curtime + 1; // one full second.
		return false;
	}
}

//-----------------------------------------------------------------------------
//
// Schedules
//
//-----------------------------------------------------------------------------

AI_BEGIN_CUSTOM_NPC( npc_mortarsynth, CNPC_Mortarsynth )
DECLARE_TASK( TASK_ENERGY_REPEL )

DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_BACK )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_FRONT )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_LEFT )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_RIGHT )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_BIG_BACK )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_BIG_FRONT )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_BIG_LEFT )
DECLARE_ACTIVITY( ACT_MSYNTH_FLINCH_BIG_RIGHT )

//=========================================================
// > SCHED_MORTARSYNTH_PATROL
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_PATROL,

	"	Tasks"
	"		TASK_SET_TOLERANCE_DISTANCE			32"
	"		TASK_SET_ROUTE_SEARCH_TIME			5"	// Spend 5 seconds trying to build a path if stuck
	"		TASK_GET_PATH_TO_RANDOM_NODE		2000"
	"		TASK_RUN_PATH						0"
	"		TASK_WAIT_FOR_MOVEMENT				0"
	""
	"	Interrupts"
	"		COND_GIVE_WAY"
	"		COND_NEW_ENEMY"
	"		COND_SEE_ENEMY"
	"		COND_SEE_FEAR"
	"		COND_HEAR_COMBAT"
	"		COND_HEAR_DANGER"
	"		COND_HEAR_PLAYER"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_PROVOKED"
)

//=========================================================
// > SCHED_MORTARSYNTH_ATTACK
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_ATTACK,

	"	Tasks"
	"		TASK_PLAY_SEQUENCE					ACTIVITY:ACT_RANGE_ATTACK1"
	"		TASK_SET_SCHEDULE					SCHEDULE:SCHED_TAKE_COVER_FROM_ENEMY"
	"		TASK_WAIT							5"
	""
	"	Interrupts"
)

//=========================================================
// > SCHED_MORTARSYNTH_REPEL
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_REPEL,

	"	Tasks"
	"		TASK_ENERGY_REPEL					0"
	"		TASK_WAIT							0.1"
	""
	"	Interrupts"
)

//=========================================================
// > SCHED_MORTARSYNTH_CHASE_ENEMY
//
//  Different interrupts than normal chase enemy.
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_CHASE_ENEMY,

	"	Tasks"
	"		 TASK_SET_FAIL_SCHEDULE				SCHEDULE:SCHED_MORTARSYNTH_PATROL"
	"		 TASK_SET_TOLERANCE_DISTANCE		120"
	"		 TASK_GET_PATH_TO_ENEMY				0"
	"		 TASK_RUN_PATH						0"
	"		 TASK_WAIT_FOR_MOVEMENT				0"
	""
	""
	"	Interrupts"
	"		COND_SCANNER_FLY_CLEAR"
	"		COND_NEW_ENEMY"
	"		COND_ENEMY_DEAD"
	"		COND_LOST_ENEMY"
)

//=========================================================
// > SCHED_MSYNTH_CHASE_TARGET
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_CHASE_TARGET,

	"	Tasks"
	"		TASK_SET_FAIL_SCHEDULE				SCHEDULE:SCHED_MORTARSYNTH_HOVER"
	"		TASK_SET_TOLERANCE_DISTANCE			120"
	"		TASK_GET_PATH_TO_TARGET				0"
	"		TASK_RUN_PATH						0"
	"		TASK_WAIT_FOR_MOVEMENT				0"
	""
	"	Interrupts"
	"		COND_SCANNER_FLY_CLEAR"
	"		COND_NEW_ENEMY"
)

//=========================================================
// > SCHED_MORTARSYNTH_HOVER
//=========================================================
DEFINE_SCHEDULE
(
	SCHED_MORTARSYNTH_HOVER,

	"	Tasks"
	"		TASK_WAIT							5"
	""
	"	Interrupts"
	"		COND_GIVE_WAY"
	"		COND_NEW_ENEMY"
	"		COND_SEE_ENEMY"
	"		COND_SEE_FEAR"
	"		COND_HEAR_COMBAT"
	"		COND_HEAR_DANGER"
	"		COND_HEAR_PLAYER"
	"		COND_LIGHT_DAMAGE"
	"		COND_HEAVY_DAMAGE"
	"		COND_PROVOKED"
	"		COND_SCANNER_FLY_BLOCKED"
)

AI_END_CUSTOM_NPC()