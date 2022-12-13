//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Implements the tripmine grenade.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "util.h"
#include "shake.h"
#include "grenade_tripwire.h"
#include "grenade_homer.h"
#include "rope.h"
#include "rope_shared.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar    sk_dmg_tripwire		( "sk_dmg_tripwire","0");
ConVar    sk_tripwire_radius	( "sk_tripwire_radius","0"); 

#define TRIPWIRE_MODEL	"models/Weapons/w_grenade.mdl"
#define TRIPWIRE_ATTACH_SOUND "TripwireGrenade.Hook"
#define TRIPWIRE_ACTIVATE_SOUND "TripwireGrenade.Activate"
#define TRIPWIRE_FLY_SOUND "TripwireGrenade.FlySound"
#define TRIPWIRE_SHOT_SOUND "TripwireGrenade.ShootRope"

#define TRIPWIRE_LAUNCH_VEL		1200
#define TRIPWIRE_SPIN_MAG		50
#define TRIPWIRE_SPIN_SPEED		100
#define TRIPWIRE_OFFSET 50
#define TRIPWIRE_MAX_ROPE_LEN	1500

LINK_ENTITY_TO_CLASS( npc_grenade_tripwire, CTripwireGrenade );

BEGIN_DATADESC( CTripwireGrenade )

	DEFINE_FIELD( m_flPowerUp,		FIELD_TIME ),
	DEFINE_FIELD( m_nMissileCount,	FIELD_INTEGER ),
	DEFINE_FIELD( m_vecDir,			FIELD_VECTOR ),
	DEFINE_FIELD( m_vTargetPos,		FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vTargetOffset,	FIELD_VECTOR ),
	DEFINE_FIELD( m_pRope,			FIELD_CLASSPTR ),
	DEFINE_FIELD( m_pHook,			FIELD_CLASSPTR ),

	// Function Pointers
	DEFINE_THINKFUNC( WarningThink ),
	DEFINE_THINKFUNC( PowerupThink ),
	DEFINE_THINKFUNC( RopeBreakThink ),
	DEFINE_THINKFUNC( FireThink ),

END_DATADESC()

CTripwireGrenade::CTripwireGrenade()
{
	m_vecDir.Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::Spawn( void )
{
	Precache();

	SetMoveType( MOVETYPE_FLY );
	SetSolid( SOLID_BBOX );
	AddSolidFlags( FSOLID_NOT_SOLID );

	SetModel( TRIPWIRE_MODEL );

	m_nMissileCount	= 0;
	
	UTIL_SetSize( this, Vector( -4, -4, -2 ), Vector( 4, 4, 2 ) );

	m_flPowerUp = gpGlobals->curtime + 1.2;	// <<CHECK>>get rid of this
	
	SetThink( &CTripwireGrenade::WarningThink );
	SetNextThink( gpGlobals->curtime + 1.0f );

	m_takedamage = DAMAGE_YES;

	SetHealth(1 );

	m_pRope = NULL;
	m_pHook = NULL;

	// Tripwire grenade sits at 90 on wall so rotate back to get m_vecDir
	QAngle angles = GetLocalAngles();
	angles.x -= 90;

	AngleVectors( angles, &m_vecDir );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::Precache( void )
{
	PrecacheModel( TRIPWIRE_MODEL );
	PrecacheScriptSound( TRIPWIRE_ACTIVATE_SOUND );
	PrecacheScriptSound( TRIPWIRE_ATTACH_SOUND );
	PrecacheScriptSound( TRIPWIRE_FLY_SOUND );
	PrecacheScriptSound( TRIPWIRE_SHOT_SOUND );

	UTIL_PrecacheOther( "tripwire_hook" );
	UTIL_PrecacheOther( "grenade_homer" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::WarningThink( void  )
{
	// play activate sound
	EmitSound( TRIPWIRE_ACTIVATE_SOUND );

	// set to power up
	SetThink( &CTripwireGrenade::PowerupThink );
	SetNextThink( gpGlobals->curtime + 1.0f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::PowerupThink( void  )
{
	if ( gpGlobals->curtime > m_flPowerUp )
	{
		MakeRope( );
		RemoveSolidFlags( FSOLID_NOT_SOLID );
		m_bIsLive = true;
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::BreakRope( void )
{
	if ( m_pRope )
	{
		m_pRope->m_RopeFlags |= ROPE_COLLIDE | ROPE_USE_WIND;
		m_pRope->DetachPoint( 0 );

		Vector vVelocity;
		m_pHook->GetVelocity( &vVelocity, NULL );
		if ( vVelocity.Length() > 1 )
			m_pRope->DetachPoint( 1 );

		UTIL_Remove( m_pRope );
		m_pRope = NULL;

		UTIL_Remove( m_pHook );
		m_pHook = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::MakeRope( void )
{
	SetThink( &CTripwireGrenade::RopeBreakThink );

	// Delay first think slightly so rope has time
	// to appear if person right in front of it
	SetNextThink( gpGlobals->curtime + 1.0f );

	// Create hook for end of tripwire
	m_pHook = (CTripwireHook*)CBaseEntity::Create( "tripwire_hook", GetLocalOrigin(), GetLocalAngles() );
	if (m_pHook)
	{
		Vector vShootVel = 800*(m_vecDir + Vector(0,0,0.3)+RandomVector(-0.01,0.01));
		m_pHook->SetVelocity( vShootVel, vec3_origin);
		m_pHook->SetOwnerEntity( this );
		m_pHook->m_hGrenade		= this;

		m_pRope = CRopeKeyframe::Create(this,m_pHook,0,0);
		if (m_pRope)
		{
			m_pRope->m_Width		= 1;
			m_pRope->m_RopeLength	= 3;
			m_pRope->m_Slack		= 100;
			m_pRope->m_Subdiv		= 64;

			CPASAttenuationFilter filter( this,TRIPWIRE_SHOT_SOUND );
			EmitSound( filter, entindex(),TRIPWIRE_SHOT_SOUND );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::Attach( void )
{
	StopSound( TRIPWIRE_SHOT_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTripwireGrenade::RopeBreakThink( void  )
{
	// See if I can go solid yet (has dropper moved out of way?)
	if ( IsSolidFlagSet( FSOLID_NOT_SOLID ) )
	{
		trace_t tr;
		Vector	vUpBit = GetAbsOrigin();
		vUpBit.z += 5.0;

		UTIL_TraceEntity( this, GetAbsOrigin(), vUpBit, MASK_SHOT, &tr );
		if ( !tr.startsolid && ( tr.fraction == 1.0 ) )
			RemoveSolidFlags( FSOLID_NOT_SOLID );
	}

	// Check if rope had gotten beyond it's max length
	float flRopeLength = ( GetAbsOrigin()-m_pHook->GetAbsOrigin() ).Length();
	if ( flRopeLength > TRIPWIRE_MAX_ROPE_LEN )
	{
		// Shoot missiles at hook
		m_iHealth = 0;
		BreakRope();
		m_vTargetPos = m_pHook->GetAbsOrigin();
		CrossProduct ( m_vecDir, Vector( 0, 0, 1 ), m_vTargetOffset );
		m_vTargetOffset *= TRIPWIRE_OFFSET; 
		SetThink( &CTripwireGrenade::FireThink );
		FireThink();
	}

	// Check to see if can see hook
	// NOT MASK_SHOT because we want only simple hit boxes
	trace_t tr;
	UTIL_TraceLine( GetAbsOrigin(), m_pHook->GetAbsOrigin(), MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	// If can't see hook
	CBaseEntity *pEntity = tr.m_pEnt;
	if ( tr.fraction != 1.0 && pEntity != m_pHook )
	{
		// Shoot missiles at place where rope was intersected
		m_iHealth = 0;
		BreakRope();
		m_vTargetPos = tr.endpos;
		CrossProduct ( m_vecDir, Vector( 0, 0, 1 ), m_vTargetOffset );
		m_vTargetOffset *= TRIPWIRE_OFFSET; 
		SetThink( &CTripwireGrenade::FireThink );
		FireThink();
		return;
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}

//------------------------------------------------------------------------------
// Purpose : Die if I take any damage
//------------------------------------------------------------------------------
int CTripwireGrenade::OnTakeDamage_Alive( const CTakeDamageInfo &info )
{
	// Killed upon any damage
	Event_Killed( info );
	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: If someone damaged, me shoot of my missiles and die
//-----------------------------------------------------------------------------
void CTripwireGrenade::Event_Killed( const CTakeDamageInfo &info )
{
	if ( m_iHealth > 0 )
	{
		// Fire missiles and blow up
		for ( int i=0; i<6; i++ )
		{
			Vector vTargetPos = GetAbsOrigin() + RandomVector( -600,600 );
			FireMissile( vTargetPos );
		}

		BreakRope();
		SetHealth( 0 ); // Why would i continue spewing rockets if i'm death?
		UTIL_Remove( this );
	}
}

//------------------------------------------------------------------------------
// Purpose : Fire a missile at the target position
//------------------------------------------------------------------------------
void CTripwireGrenade::FireMissile(const Vector &vTargetPos)
{
	Vector vTargetDir = ( vTargetPos - GetAbsOrigin() );
	VectorNormalize( vTargetDir );

	float flGravity = 0.0001;	// No gravity on the missiles
	bool  bSmokeTrail = true;
	float flHomingSpeed = 0;
	Vector vLaunchVelocity = TRIPWIRE_LAUNCH_VEL*vTargetDir;
	float flSpinMagnitude = TRIPWIRE_SPIN_MAG;
	float flSpinSpeed = TRIPWIRE_SPIN_SPEED;

	//<<CHECK>> hold in string_t
	CGrenadeHomer *pHomer = CGrenadeHomer::CreateGrenadeHomer( MAKE_STRING( TRIPWIRE_MODEL ), MAKE_STRING( TRIPWIRE_FLY_SOUND ),  GetAbsOrigin(), vec3_angle, edict() );
	if ( pHomer )
	{
		pHomer->Spawn();
		pHomer->SetSpin( flSpinMagnitude, flSpinSpeed );
		pHomer->SetHoming( 0, 0, 0, 0, 0 );
		pHomer->SetDamage( sk_dmg_tripwire.GetFloat() );
		pHomer->SetDamageRadius( sk_tripwire_radius.GetFloat() );
		pHomer->Launch( this, NULL, vLaunchVelocity, flHomingSpeed, flGravity, bSmokeTrail );

		// Calculate travel time
		float flTargetDist	= ( GetAbsOrigin() - vTargetPos).Length();

		pHomer->m_flDetonateTime = gpGlobals->curtime + flTargetDist / TRIPWIRE_LAUNCH_VEL;
	}

}

//------------------------------------------------------------------------------
// Purpose : Shoot off a series of missiles over time, then go intert
//------------------------------------------------------------------------------
void CTripwireGrenade::FireThink()
{
	SetNextThink( gpGlobals->curtime + 0.16f );

	Vector vTargetPos		= m_vTargetPos + (m_vTargetOffset * m_nMissileCount);
	FireMissile(vTargetPos);

	vTargetPos		= m_vTargetPos - (m_vTargetOffset * m_nMissileCount);
	FireMissile(vTargetPos);


	m_nMissileCount++;
	if ( m_nMissileCount > 4 )
	{
		SetHealth( -1 );
		SetThink( NULL );
	}
}

// ####################################################################
//   CTripwireHook
//
//		This is what the tripwire shoots out at the end of the rope
// ####################################################################
LINK_ENTITY_TO_CLASS( tripwire_hook, CTripwireHook );

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC( CTripwireHook )

	DEFINE_FIELD( m_hGrenade, FIELD_EHANDLE ),
	DEFINE_FIELD( m_bAttached, FIELD_BOOLEAN ),

END_DATADESC()

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CTripwireHook::Spawn( void )
{

	Precache();
	SetModel( TRIPWIRE_MODEL );	//<<CHECK>>

	UTIL_SetSize(this, Vector( -1, -1, -1), Vector( 1, 1, 1 ) );

	m_takedamage = DAMAGE_NO;
	m_bAttached = false;

	CreateVPhysics();
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
bool CTripwireHook::CreateVPhysics()
{
	// Create the object in the physics system
	IPhysicsObject *pPhysicsObject = VPhysicsInitNormal( SOLID_BBOX, 0, false );
	
	// Make sure I get touch called for static geometry
	if ( pPhysicsObject )
	{
		int flags = pPhysicsObject->GetCallbackFlags();
		flags |= CALLBACK_GLOBAL_TOUCH_STATIC;
		pPhysicsObject->SetCallbackFlags( flags );
	}
	return true;
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CTripwireHook::Precache( void )
{
	PrecacheModel( TRIPWIRE_MODEL ); //<<CHECK>>
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CTripwireHook::EndTouch( CBaseEntity *pOther )
{
	//<<CHECK>>do instead by clearing touch function
	if ( !m_bAttached )
	{
		m_bAttached = true;

		SetVelocity( vec3_origin, vec3_origin );
		SetMoveType( MOVETYPE_NONE );

		EmitSound( TRIPWIRE_ATTACH_SOUND );

		// Let the tripwire grenade know that I've attached
		CTripwireGrenade* pWire = dynamic_cast<CTripwireGrenade*>( (CBaseEntity*)m_hGrenade );
		if ( pWire )
			pWire->Attach();
	}
}

//------------------------------------------------------------------------------
// Purpose :
//------------------------------------------------------------------------------
void CTripwireHook::SetVelocity( const Vector &velocity, const AngularImpulse &angVelocity )
{
	IPhysicsObject *pPhysicsObject = VPhysicsGetObject();
	if ( pPhysicsObject )
		pPhysicsObject->AddVelocity( &velocity, &angVelocity );
}