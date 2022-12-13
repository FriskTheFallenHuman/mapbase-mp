//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Flaming bottle thrown from the hand
//
//=============================================================================//

#include "cbase.h"
#include "grenade_molotov.h"
#include "mapbase/weapon_molotov.h" // load the hl2mp version!!!
#include "soundent.h"
#include "decals.h"
#include "fire.h"
#include "shake.h"
#include "ndebugoverlay.h"
#include "smoke_trail.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "world.h"

#ifdef PORTAL
	#include "portal_util_shared.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MOLOTOV_MAX_DANGER_RADIUS	300

extern short	g_sModelIndexFireball;			// (in combatweapon.cpp) holds the index for the smoke cloud

// Moved to HL2_SharedGameRules because these are referenced by shared AmmoDef functions
extern ConVar    sk_plr_dmg_molotov;
extern ConVar    sk_npc_dmg_molotov;

ConVar	  sk_molotov_radius		( "sk_molotov_radius","0");
#ifdef MAPBASE
ConVar	g_molotov_credit_transfer( "g_molotov_credit_transfer", "1" );
#endif

extern ConVar g_CV_SmokeTrail; // temporary dust explosion switch

#define MOLOTOV_EXPLOSION_VOLUME	1024

BEGIN_DATADESC( CGrenade_Molotov )

	DEFINE_FIELD( m_hSmokeTrail, FIELD_EHANDLE ),
#ifdef MAPBASE_MP
	DEFINE_FIELD( m_hFireTrail, FIELD_EHANDLE ),
#endif

	// Function Pointers
	DEFINE_ENTITYFUNC( MolotovTouch ),
	DEFINE_THINKFUNC( MolotovThink ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( grenade_molotov, CGrenade_Molotov );

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
CGrenade_Molotov::CGrenade_Molotov( void )
{
	m_hSmokeTrail  = NULL;
#ifdef MAPBASE_MP
	m_hFireTrail = NULL;
#endif // MAPBASE_MP
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CGrenade_Molotov::Spawn( void )
{
	Precache();
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_BOUNCE );
	SetSolid( SOLID_BBOX ); 
	SetCollisionGroup( COLLISION_GROUP_PROJECTILE );

	SetModel( "models/props_junk/garbage_glassbottle001a.mdl" );

	UTIL_SetSize( this, Vector( -6, -6, -2 ), Vector( 6, 6, 2 ) );

	//SetUse( &CGrenade_Molotov::DetonateUse );
	SetTouch( &CGrenade_Molotov::MolotovTouch );
	SetThink( &CGrenade_Molotov::MolotovThink );
	SetNextThink( gpGlobals->curtime + 0.1f );

	if( GetOwnerEntity() && GetOwnerEntity()->IsPlayer() )
		SetDamage( sk_plr_dmg_molotov.GetFloat() );
	else
		SetDamage( sk_npc_dmg_molotov.GetFloat() );

	SetDamageRadius( sk_molotov_radius.GetFloat() );

	m_takedamage	= DAMAGE_YES;
	SetHealth(1 );

	SetGravity( UTIL_ScaleForGravity( 400 ) );	// use a lower gravity for grenades to make them easier to see
	SetFriction( 0.8 );  // Give a little bounce so can flatten
	SetSequence( 1 );

	m_fDangerRadius = 100;

	m_fSpawnTime = gpGlobals->curtime;

	// -------------
	// Smoke trail.
	// -------------
	if( g_CV_SmokeTrail.GetInt() )
	{
		m_hSmokeTrail = SmokeTrail::CreateSmokeTrail();
		
		if( m_hSmokeTrail )
		{
			m_hSmokeTrail->m_SpawnRate = 48;
			m_hSmokeTrail->m_ParticleLifetime = 1.0f;
			m_hSmokeTrail->m_StartColor.Init( 0.2f, 0.2f, 0.2f );
			m_hSmokeTrail->m_EndColor.Init( 0.0, 0.0, 0.0 );
			m_hSmokeTrail->m_StartSize = 8;
			m_hSmokeTrail->m_EndSize = m_hSmokeTrail->m_StartSize * 4;
			m_hSmokeTrail->m_SpawnRadius = 4;
			m_hSmokeTrail->m_MinSpeed = 8;
			m_hSmokeTrail->m_MaxSpeed = 16;
			m_hSmokeTrail->m_Opacity = 0.25f;

			m_hSmokeTrail->SetLifetime( 20.0f );
			m_hSmokeTrail->FollowEntity( this );
		}
	}

#ifdef MAPBASE_MP
	// -------------
	// Fire trail.
	// -------------
	if( g_CV_SmokeTrail.GetInt() )
	{
		m_hFireTrail = CFireTrail::CreateFireTrail();
		
		if( m_hFireTrail )
		{
			m_hFireTrail->SetLifetime( 20.0f );
			m_hFireTrail->FollowEntity( this, 0 );
		}
	}
#endif // MAPBASE_MP
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CGrenade_Molotov::MolotovTouch( CBaseEntity *pOther )
{
	Assert( pOther );
	if ( !pOther->IsSolid() )
		return;

	// If I'm live go ahead and blow up
	if ( m_bIsLive )
	{
		Detonate();
	}
	else
	{
		// If I'm not live, only blow up if I'm hitting an chacter that
		// is not the owner of the weapon
		CBaseCombatCharacter *pBCC = ToBaseCombatCharacter( pOther );
		if ( pBCC && GetThrower() != pBCC )
		{
			m_bIsLive = true;
			Detonate();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CGrenade_Molotov::Detonate( void ) 
{
	if ( !m_bIsLive )
		return;

	m_bIsLive		= false;
	m_takedamage	= DAMAGE_NO;

	if( m_hSmokeTrail )
	{
		UTIL_Remove( m_hSmokeTrail );
		m_hSmokeTrail = NULL;
	}

#ifdef MAPBASE_MP
	if( m_hFireTrail )
	{
		UTIL_Remove( m_hFireTrail );
		m_hFireTrail = NULL;
	}
#endif // MAPBASE_MP

	CPASFilter filter( GetAbsOrigin() );

	te->Explosion( filter, 0.0,
		&GetAbsOrigin(), 
		g_sModelIndexFireball,
		2.0, 
		15,
		TE_EXPLFLAG_NOPARTICLES,
		m_DmgRadius,
		m_flDamage );

	Vector vecForward = GetAbsVelocity();
	VectorNormalize(vecForward);
	trace_t		tr;
	UTIL_TraceLine ( GetAbsOrigin(), GetAbsOrigin() + 60 * vecForward, MASK_SHOT, 
		this, COLLISION_GROUP_NONE, &tr );

	if ( ( tr.m_pEnt != GetWorldEntity() ) || ( tr.hitbox != 0 ) )
	{
		// non-world needs smaller decals
		if( tr.m_pEnt && !tr.m_pEnt->IsNPC() )
			UTIL_DecalTrace( &tr, "SmallScorch" );
	}
	else
		UTIL_DecalTrace( &tr, "Scorch" );

	// TODO: Turn this back into a prop_physics when we touch a water surface
	int contents = UTIL_PointContents ( GetAbsOrigin() );
	if ( ( contents & MASK_WATER ) )
	{
		UTIL_Remove( this );
		return;
	}

// Start some fires
	int i;
	QAngle vecTraceAngles;
	Vector vecTraceDir;
	trace_t firetrace;

	for( i = 0 ; i < 16 ; i++ )
	{
		// build a little ray
		vecTraceAngles[PITCH]	= random->RandomFloat(45, 135);
		vecTraceAngles[YAW]		= random->RandomFloat(0, 360);
		vecTraceAngles[ROLL]	= 0.0f;

		AngleVectors( vecTraceAngles, &vecTraceDir );

		Vector vecStart, vecEnd;

		vecStart = GetAbsOrigin() + ( tr.plane.normal * 128 );
		vecEnd = vecStart + vecTraceDir * 512;

		UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID_BRUSHONLY, this, COLLISION_GROUP_NONE, &firetrace );

		Vector	ofsDir = ( firetrace.endpos - GetAbsOrigin() );
		float	offset = VectorNormalize( ofsDir );

		if ( offset > 128 )
			offset = 128;

		//Get our scale based on distance
		float scale	 = 0.1f + ( 0.75f * ( 1.0f - ( offset / 128.0f ) ) );
		float growth = 0.1f + ( 0.75f * ( offset / 128.0f ) );

		if( firetrace.fraction != 1.0 )
		{
			FireSystem_StartFire( firetrace.endpos, scale, growth, 30.0f, (SF_FIRE_START_ON|SF_FIRE_SMOKELESS|SF_FIRE_NO_GLOW), (CBaseEntity*) this, FIRE_NATURAL );
		}
	}
// End Start some fires

	UTIL_ScreenShake( GetAbsOrigin(), 25.0, 150.0, 1.0, 750, SHAKE_START );

	RadiusDamage ( CTakeDamageInfo( this, GetThrower(), m_flDamage, DMG_BLAST ), GetAbsOrigin(), m_DmgRadius, CLASS_NONE, NULL );

	UTIL_Remove( this );
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CGrenade_Molotov::MolotovThink( void )
{
	// See if I can lose my owner (has dropper moved out of way?)
	// Want do this so owner can throw the brickbat
	if (GetOwnerEntity())
	{
		trace_t tr;
		Vector	vUpABit = GetAbsOrigin();
		vUpABit.z += 5.0;

		CBaseEntity* saveOwner	= GetOwnerEntity();
		SetOwnerEntity( NULL );
		UTIL_TraceEntity( this, GetAbsOrigin(), vUpABit, MASK_SOLID, &tr );
		if ( tr.startsolid || tr.fraction != 1.0 )
		{
			SetOwnerEntity( saveOwner );
		}
	}
	SetNextThink( gpGlobals->curtime + 0.1f );

	SetNextThink( gpGlobals->curtime + 0.05f );

	if ( !m_bIsLive )
	{
		// Go live after a short delay
		if ( m_fSpawnTime + MAX_MOLOTOV_NO_COLLIDE_TIME < gpGlobals->curtime )
			m_bIsLive  = true;
	}
	
	// If I just went solid and my velocity is zero, it means I'm resting on
	// the floor already when I went solid so blow up
	if ( m_bIsLive )
	{
		if ( GetAbsVelocity().Length() == 0.0 ||
			GetGroundEntity() != NULL )
			Detonate();
	}

	// The old way of making danger sounds would scare the crap out of EVERYONE between you and where the grenade
	// was going to hit. The radius of the danger sound now 'blossoms' over the grenade's lifetime, making it seem
	// dangerous to a larger area downrange than it does from where it was fired.
	if( m_fDangerRadius <= MOLOTOV_MAX_DANGER_RADIUS )
		m_fDangerRadius += ( MOLOTOV_MAX_DANGER_RADIUS * 0.05 );

	CSoundEnt::InsertSound( SOUND_DANGER, GetAbsOrigin() + GetAbsVelocity() * 0.5, m_fDangerRadius, 0.2, this, SOUNDENT_CHANNEL_REPEATED_DANGER );
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CGrenade_Molotov::Event_Killed( const CTakeDamageInfo &info )
{
#ifdef MAPBASE
	if ( g_molotov_credit_transfer.GetBool() && info.GetAttacker()->MyCombatCharacterPointer() )
	{
		CBaseCombatCharacter *pBCC = info.GetAttacker()->MyCombatCharacterPointer();
		SetThrower( pBCC );
		SetOwnerEntity( pBCC );
	}
#endif
	Detonate( );
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CGrenade_Molotov::Precache( void )
{
	BaseClass::Precache();

	PrecacheModel( "models/props_junk/garbage_glassbottle001a.mdl" );

	UTIL_PrecacheOther( "_firesmoke" );
}

