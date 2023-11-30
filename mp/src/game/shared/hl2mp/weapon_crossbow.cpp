//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "npcevent.h"
#include "gamerules.h"
#include "in_buttons.h"
#include "vstdlib/random.h"
#include "engine/IEngineSound.h"
#include "IEffects.h"
#include "Sprite.h"
#include "SpriteTrail.h"
#include "beam_shared.h"
#include "rumble_shared.h"
#include "gamestats.h"
#include "decals.h"

#ifdef PORTAL
	#include "portal_util_shared.h"
#endif

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	#include "c_te_effect_dispatch.h"
	#include "input.h"
#else
	#include "basecombatcharacter.h"
	#include "ai_basenpc.h"
	#include "hl2mp_player.h"
	#include "te_effect_dispatch.h"
	#include "soundent.h"
	#include "game.h"
	#ifdef MAPBASE
		#include "func_break.h"
	#endif // MAPBASE
#endif

#include "weapon_hl2mpbasehlmpcombatweapon.h"
#include "effect_dispatch_data.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BOLT_MODEL "models/crossbow_bolt.mdl"
#define GLOW_SPRITE "sprites/light_glow02_noz.vmt"
#define GLOW_TRAIL_SPRITE "sprites/bluelaser1.vmt"

#define BOLT_AIR_VELOCITY	2500
#define BOLT_WATER_VELOCITY	1500
#define	BOLT_SKIN_NORMAL	0
#define BOLT_SKIN_GLOW		1


#ifdef GAME_DLL

extern ConVar sk_plr_dmg_crossbow;
extern ConVar sk_npc_dmg_crossbow;

#ifdef MAPBASE
	ConVar g_weapon_crossbow_new_hit_locations( "g_weapon_crossbow_new_hit_locations", "1", FCVAR_NONE, "Toggles new crossbow knockback that properly pushes back the correct limbs." );
#endif

void TE_StickyBolt( IRecipientFilter& filter, float delay,	Vector vecDirection, const Vector* origin );

//-----------------------------------------------------------------------------
// Crossbow Bolt
//-----------------------------------------------------------------------------
class CCrossbowBolt : public CBaseCombatCharacter
{
	DECLARE_CLASS( CCrossbowBolt, CBaseCombatCharacter );

public:
#ifdef MAPBASE
	CCrossbowBolt();
#else
	CCrossbowBolt() { };
#endif
	~CCrossbowBolt();

	Class_T Classify( void )
	{
		return CLASS_NONE;
	}

public:
	void Spawn( void );
	void Precache( void );
	void BubbleThink( void );
	void BoltTouch( CBaseEntity* pOther );
	bool CreateVPhysics( void );
	unsigned int PhysicsSolidMaskForEntity() const;
#ifdef MAPBASE
	static CCrossbowBolt* BoltCreate( const Vector& vecOrigin, const QAngle& angAngles, CBaseCombatCharacter* pentOwner = NULL );

	void InputSetDamage( inputdata_t& inputdata );
	float m_flDamage;

	virtual void SetDamage( float flDamage )
	{
		m_flDamage = flDamage;
	}
#else
	static CCrossbowBolt* BoltCreate( const Vector& vecOrigin, const QAngle& angAngles, CBasePlayer* pentOwner = NULL );
#endif

protected:

	bool	CreateSprites( void );

	CHandle<CSprite>		m_pGlowSprite;
	CHandle<CSpriteTrail>	m_pGlowTrail;

	int		m_iDamage;

	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();
};
LINK_ENTITY_TO_CLASS( crossbow_bolt, CCrossbowBolt );

BEGIN_DATADESC( CCrossbowBolt )
// Function Pointers
DEFINE_FUNCTION( BubbleThink ),
				 DEFINE_FUNCTION( BoltTouch ),

				 // These are recreated on reload, they don't need storage
				 DEFINE_FIELD( m_pGlowSprite, FIELD_EHANDLE ),
				 DEFINE_FIELD( m_pGlowTrail, FIELD_EHANDLE ),

#ifdef MAPBASE
	DEFINE_KEYFIELD( m_flDamage, FIELD_FLOAT, "Damage" ),

	// Inputs
	DEFINE_INPUTFUNC( FIELD_FLOAT, "SetDamage", InputSetDamage ),
#endif

				 END_DATADESC()

				 IMPLEMENT_SERVERCLASS_ST( CCrossbowBolt, DT_CrossbowBolt )
				 END_SEND_TABLE()

#ifdef MAPBASE
	CCrossbowBolt* CCrossbowBolt::BoltCreate( const Vector& vecOrigin, const QAngle& angAngles, CBaseCombatCharacter* pentOwner )
#else
	CCrossbowBolt* CCrossbowBolt::BoltCreate( const Vector& vecOrigin, const QAngle& angAngles, CBasePlayer* pentOwner )
#endif
{
	// Create a new entity with CCrossbowBolt private data
	CCrossbowBolt* pBolt = ( CCrossbowBolt* )CreateEntityByName( "crossbow_bolt" );
	UTIL_SetOrigin( pBolt, vecOrigin );
	pBolt->SetAbsAngles( angAngles );
	pBolt->Spawn();
	pBolt->SetOwnerEntity( pentOwner );

#ifdef MAPBASE
	if( pentOwner && pentOwner->IsNPC() )
	{
		pBolt->m_flDamage = sk_npc_dmg_crossbow.GetFloat();
	}
	//else
	//	pBolt->m_flDamage = sk_plr_dmg_crossbow.GetFloat();
#endif

	return pBolt;
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CCrossbowBolt::CCrossbowBolt( void )
{
	// Independent bolts without m_flDamage set need damage
	m_flDamage = sk_plr_dmg_crossbow.GetFloat();
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CCrossbowBolt::~CCrossbowBolt( void )
{
	if( m_pGlowSprite )
	{
		UTIL_Remove( m_pGlowSprite );
	}

	if( m_pGlowTrail )
	{
		UTIL_Remove( m_pGlowTrail );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCrossbowBolt::CreateVPhysics( void )
{
	// Create the object in the physics system
	VPhysicsInitNormal( SOLID_BBOX, FSOLID_NOT_STANDABLE, false );

	return true;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
unsigned int CCrossbowBolt::PhysicsSolidMaskForEntity() const
{
	return ( BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_HITBOX ) & ~CONTENTS_GRATE;
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CCrossbowBolt::CreateSprites( void )
{
	if( m_pGlowSprite  == NULL )
	{
		// Start up the eye glow
		m_pGlowSprite = CSprite::SpriteCreate( GLOW_SPRITE, GetLocalOrigin(), false );

		if( m_pGlowSprite != NULL )
		{
			m_pGlowSprite->FollowEntity( this );
			m_pGlowSprite->SetTransparency( kRenderGlow, 255, 255, 255, 128, kRenderFxNoDissipation );
			m_pGlowSprite->SetScale( 0.2f );
			m_pGlowSprite->TurnOff();
		}
	}

	if( m_pGlowTrail  == NULL )
	{
		// Start up the eye trail
		m_pGlowTrail = CSpriteTrail::SpriteTrailCreate( GLOW_TRAIL_SPRITE, GetLocalOrigin(), false );

		if( m_pGlowTrail != NULL )
		{
			m_pGlowTrail->FollowEntity( this );
			m_pGlowTrail->SetTransparency( kRenderTransAdd, 255, 255, 255, 128, kRenderFxNone );
			m_pGlowTrail->SetStartWidth( 8.0f );
			m_pGlowTrail->SetEndWidth( 1.0f );
			m_pGlowTrail->SetLifeTime( 0.5f );
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCrossbowBolt::Spawn( void )
{
	Precache( );

	SetModel( BOLT_MODEL );
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	UTIL_SetSize( this, -Vector( 1, 1, 1 ), Vector( 1, 1, 1 ) );
	SetSolid( SOLID_BBOX );
	SetGravity( 0.05f );

	// Make sure we're updated if we're underwater
	UpdateWaterState();

	SetTouch( &CCrossbowBolt::BoltTouch );

	SetThink( &CCrossbowBolt::BubbleThink );
	SetNextThink( gpGlobals->curtime + 0.1f );

	CreateSprites();

	// Make us glow until we've hit the wall
	m_nSkin = BOLT_SKIN_GLOW;
}


void CCrossbowBolt::Precache( void )
{
	PrecacheModel( BOLT_MODEL );

	PrecacheMaterial( GLOW_SPRITE );
	PrecacheMaterial( GLOW_TRAIL_SPRITE );
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCrossbowBolt::InputSetDamage( inputdata_t& inputdata )
{
	m_flDamage = inputdata.value.Float();
}
#endif


//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pOther -
//-----------------------------------------------------------------------------
void CCrossbowBolt::BoltTouch( CBaseEntity* pOther )
{
	if( pOther->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS | FSOLID_TRIGGER ) )
	{
		// Some NPCs are triggers that can take damage (like antlion grubs). We should hit them.
#ifdef MAPBASE
		// But some physics objects that are also triggers (like weapons) shouldn't go through this check.
		//
		// Note: rpg_missile has the same code, except it properly accounts for weapons in a different way.
		// This was discovered after I implemented this and both work fine, but if this ever causes problems,
		// use rpg_missile's implementation:
		//
		// if ( pOther->IsSolidFlagSet(FSOLID_TRIGGER|FSOLID_VOLUME_CONTENTS) && pOther->GetCollisionGroup() != COLLISION_GROUP_WEAPON )
		//
		if( pOther->GetMoveType() == MOVETYPE_NONE && ( ( pOther->m_takedamage == DAMAGE_NO ) || ( pOther->m_takedamage == DAMAGE_EVENTS_ONLY ) ) )
#else
		if( ( pOther->m_takedamage == DAMAGE_NO ) || ( pOther->m_takedamage == DAMAGE_EVENTS_ONLY ) )
#endif
			return;
	}

	if( pOther->m_takedamage != DAMAGE_NO )
	{
		trace_t	tr;
		tr = BaseClass::GetTouchTrace();

		Vector	vecNormalizedVel = GetAbsVelocity();
		ClearMultiDamage();

		VectorNormalize( vecNormalizedVel );

#if defined(HL2_EPISODIC)
		//!!!HACKHACK - specific hack for ep2_outland_10 to allow crossbow bolts to pass through her bounding box when she's crouched in front of the player
		// (the player thinks they have clear line of sight because Alyx is crouching, but her BBOx is still full-height and blocks crossbow bolts.
		if( GetOwnerEntity() && GetOwnerEntity()->IsPlayer() && pOther->Classify() == CLASS_PLAYER_ALLY_VITAL && FStrEq( STRING( gpGlobals->mapname ), "ep2_outland_10" ) )
		{
			// Change the owner to stop further collisions with Alyx. We do this by making her the owner.
			// The player won't get credit for this kill but at least the bolt won't magically disappear!
			SetOwnerEntity( pOther );
			return;
		}
#endif//HL2_EPISODIC

#ifdef MAPBASE
		if( g_weapon_crossbow_new_hit_locations.GetInt() > 0 )
		{
			// A very experimental and weird way of getting a crossbow bolt to deal accurate knockback.
			CBaseAnimating* pOtherAnimating = pOther->GetBaseAnimating();
			if( pOtherAnimating && pOtherAnimating->GetModelPtr() && pOtherAnimating->GetModelPtr()->numbones() > 1 )
			{
				int iClosestBone = -1;
				float flCurDistSqr = Square( 128.0f );
				matrix3x4_t bonetoworld;
				Vector vecBonePos;

				for( int i = 0; i < pOtherAnimating->GetModelPtr()->numbones(); i++ )
				{
					pOtherAnimating->GetBoneTransform( i, bonetoworld );
					MatrixPosition( bonetoworld, vecBonePos );

					float flDist = vecBonePos.DistToSqr( GetLocalOrigin() );
					if( flDist < flCurDistSqr )
					{
						iClosestBone = i;
						flCurDistSqr = flDist;
					}
				}

				if( iClosestBone != -1 )
				{
					tr.physicsbone = pOtherAnimating->GetPhysicsBone( iClosestBone );
				}
			}
		}
#endif

		if( GetOwnerEntity() && GetOwnerEntity()->IsPlayer() && pOther->IsNPC() )
		{
#ifdef MAPBASE
			CTakeDamageInfo	dmgInfo( this, GetOwnerEntity(), m_flDamage, DMG_NEVERGIB );
#else
			CTakeDamageInfo	dmgInfo( this, GetOwnerEntity(), sk_plr_dmg_crossbow.GetFloat(), DMG_NEVERGIB );
#endif
			dmgInfo.AdjustPlayerDamageInflictedForSkillLevel();
			CalculateMeleeDamageForce( &dmgInfo, vecNormalizedVel, tr.endpos, 0.7f );
			dmgInfo.SetDamagePosition( tr.endpos );
			pOther->DispatchTraceAttack( dmgInfo, vecNormalizedVel, &tr );

			SetMoveType( MOVETYPE_NONE );

			AddEffects( EF_NODRAW );
			SetTouch( NULL );
			SetThink( &CCrossbowBolt::SUB_Remove );
			SetNextThink( gpGlobals->curtime + 2.0f );

			if( m_pGlowSprite != NULL )
			{
				m_pGlowSprite->TurnOff();
			}

			if( m_pGlowTrail != NULL )
			{
				m_pGlowTrail->TurnOff();
			}

			CBasePlayer* pPlayer = ToBasePlayer( GetOwnerEntity() );
			if( pPlayer )
			{
				gamestats->Event_WeaponHit( pPlayer, true, "weapon_crossbow", dmgInfo );
			}
		}
		else
		{
#ifdef MAPBASE
			CTakeDamageInfo	dmgInfo( this, GetOwnerEntity(), m_flDamage, DMG_BULLET | DMG_NEVERGIB );
#else
			CTakeDamageInfo	dmgInfo( this, GetOwnerEntity(), sk_plr_dmg_crossbow.GetFloat(), DMG_BULLET | DMG_NEVERGIB );
#endif
			CalculateMeleeDamageForce( &dmgInfo, vecNormalizedVel, tr.endpos, 0.7f );
			dmgInfo.SetDamagePosition( tr.endpos );
			pOther->DispatchTraceAttack( dmgInfo, vecNormalizedVel, &tr );
			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = GetAbsVelocity();
			float speed = VectorNormalize( vecDir );

			// See if we should reflect off this surface
			float hitDot = DotProduct( tr.plane.normal, -vecDir );

			if( ( hitDot < 0.5f ) && ( speed > 100 ) )
			{
				// Go through glass
				if( pOther->GetCollisionGroup() == COLLISION_GROUP_BREAKABLE_GLASS )
				{
					return;
				}

				// Go through thin material types
				if( FClassnameIs( pOther, "func_breakable" ) || FClassnameIs( pOther, "func_breakable_surf" ) )
				{
					CBreakable* pOtherEntity = static_cast<CBreakable*>( pOther );
					if( ( pOtherEntity->GetMaterialType() == matGlass ) || ( pOtherEntity->GetMaterialType() == matWeb ) )
					{
						return;
					}
				}

				Vector vReflection = 2.0f * tr.plane.normal * hitDot + vecDir;

				QAngle reflectAngles;

				VectorAngles( vReflection, reflectAngles );

				SetLocalAngles( reflectAngles );

				SetAbsVelocity( vReflection * speed * 0.75f );
			}
			else
			{
				// Go through glass
				if( pOther->GetCollisionGroup() == COLLISION_GROUP_BREAKABLE_GLASS )
				{
					return;
				}

				// Go through thin material types
				if( FClassnameIs( pOther, "func_breakable" ) || FClassnameIs( pOther, "func_breakable_surf" ) )
				{
					CBreakable* pOtherEntity = static_cast<CBreakable*>( pOther );
					if( ( pOtherEntity->GetMaterialType() == matGlass ) || ( pOtherEntity->GetMaterialType() == matWeb ) )
					{
						return;
					}
				}

				SetThink( &CCrossbowBolt::SUB_Remove );
				SetNextThink( gpGlobals->curtime + 2.0f );

				//FIXME: We actually want to stick (with hierarchy) to what we've hit
				SetMoveType( MOVETYPE_NONE );

				Vector vForward;

				AngleVectors( GetAbsAngles(), &vForward );
				VectorNormalize( vForward );

				UTIL_ImpactTrace( &tr, DMG_BULLET );

				AddEffects( EF_NODRAW );
				SetTouch( NULL );
				SetThink( &CCrossbowBolt::SUB_Remove );
				SetNextThink( gpGlobals->curtime + 2.0f );

				if( m_pGlowSprite != NULL )
				{
					m_pGlowSprite->TurnOff();
				}

				if( m_pGlowTrail != NULL )
				{
					m_pGlowTrail->TurnOff();
				}
			}
		}

		ApplyMultiDamage();
	}
	else
	{
		trace_t	tr;
		tr = BaseClass::GetTouchTrace();

		// See if we struck the world
		if( pOther->GetCollisionGroup() == COLLISION_GROUP_NONE && !( tr.surface.flags & SURF_SKY ) )
		{
			EmitSound( "Weapon_Crossbow.BoltHitWorld" );

			// if what we hit is static architecture, can stay around for a while.
			Vector vecDir = GetAbsVelocity();
			float speed = VectorNormalize( vecDir );

			// See if we should reflect off this surface
			float hitDot = DotProduct( tr.plane.normal, -vecDir );

			if( ( hitDot < 0.5f ) && ( speed > 100 ) )
			{
				Vector vReflection = 2.0f * tr.plane.normal * hitDot + vecDir;

				QAngle reflectAngles;

				VectorAngles( vReflection, reflectAngles );

				SetLocalAngles( reflectAngles );

				SetAbsVelocity( vReflection * speed * 0.75f );

				// Start to sink faster
				SetGravity( 1.0f );
			}
			else
			{
				SetThink( &CCrossbowBolt::SUB_Remove );
				SetNextThink( gpGlobals->curtime + 2.0f );

				//FIXME: We actually want to stick (with hierarchy) to what we've hit
				SetMoveType( MOVETYPE_NONE );

				Vector vForward;

				AngleVectors( GetAbsAngles(), &vForward );
				VectorNormalize( vForward );

				CEffectData	data;

				data.m_vOrigin = tr.endpos;
				data.m_vNormal = vForward;
				data.m_nEntIndex = 0;

				DispatchEffect( "BoltImpact", data );

				UTIL_ImpactTrace( &tr, DMG_BULLET );

				AddEffects( EF_NODRAW );
				SetTouch( NULL );
				SetThink( &CCrossbowBolt::SUB_Remove );
				SetNextThink( gpGlobals->curtime + 2.0f );

				if( m_pGlowSprite != NULL )
				{
					m_pGlowSprite->TurnOn();
					m_pGlowSprite->FadeAndDie( 3.0f );
				}

				if( m_pGlowTrail != NULL )
				{
					m_pGlowTrail->TurnOn();
					m_pGlowSprite->FadeAndDie( 3.0f );
				}
			}

			// Shoot some sparks
			if( UTIL_PointContents( GetAbsOrigin() ) != CONTENTS_WATER )
			{
				g_pEffects->Sparks( GetAbsOrigin() );
			}
		}
		else
		{
			// Put a mark unless we've hit the sky
			if( ( tr.surface.flags & SURF_SKY ) == false )
			{
				UTIL_ImpactTrace( &tr, DMG_BULLET );
			}

			UTIL_Remove( this );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CCrossbowBolt::BubbleThink( void )
{
	QAngle angNewAngles;

	VectorAngles( GetAbsVelocity(), angNewAngles );
	SetAbsAngles( angNewAngles );

	SetNextThink( gpGlobals->curtime + 0.1f );

	// Make danger sounds out in front of me, to scare snipers back into their hole
	CSoundEnt::InsertSound( SOUND_DANGER_SNIPERONLY, GetAbsOrigin() + GetAbsVelocity() * 0.2, 120.0f, 0.5f, this, SOUNDENT_CHANNEL_REPEATED_DANGER );

	if( GetWaterLevel()  == 0 )
	{
		return;
	}

	UTIL_BubbleTrail( GetAbsOrigin() - GetAbsVelocity() * 0.1f, GetAbsOrigin(), 5 );
}

#endif

//-----------------------------------------------------------------------------
// CWeaponCrossbow
//-----------------------------------------------------------------------------

#ifdef CLIENT_DLL
	#define CWeaponCrossbow C_WeaponCrossbow
#endif

class CWeaponCrossbow : public CBaseHL2MPCombatWeapon
{
	DECLARE_CLASS( CWeaponCrossbow, CBaseHL2MPCombatWeapon );
public:

	CWeaponCrossbow( void );

	virtual void	Precache( void );
	virtual void	PrimaryAttack( void );
	virtual void	SecondaryAttack( void );
	virtual bool	Deploy( void );
	virtual void	Drop( const Vector& vecVelocity );
	virtual bool	Holster( CBaseCombatWeapon* pSwitchingTo = NULL );
	virtual bool	Reload( void );
#ifdef MAPBASE
	virtual void	Reload_NPC( bool bPlaySound = true );
#endif
	virtual void	ItemPostFrame( void );
	virtual void	ItemBusyFrame( void );
#ifdef GAME_DLL
	virtual void Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator );
#ifdef MAPBASE
	virtual void	Operator_ForceNPCFire( CBaseCombatCharacter* pOperator, bool bSecondary );
#endif
#endif
	virtual bool	SendWeaponAnim( int iActivity );
	virtual bool	IsWeaponZoomed()
	{
		return m_bInZoom;
	}

	bool	ShouldDisplayHUDHint()
	{
		return true;
	}

#ifdef MAPBASE
#ifdef GAME_DLL
	int		CapabilitiesGet( void )
	{
		return bits_CAP_WEAPON_RANGE_ATTACK1;
	}
#endif // GAME_DLL
	virtual int	GetMinBurst()
	{
		return 1;
	}
	virtual int	GetMaxBurst()
	{
		return 1;
	}

	virtual float	GetMinRestTime( void )
	{
		return 3.0f;    // 1.5f
	}
	virtual float	GetMaxRestTime( void )
	{
		return 3.0f;    // 2.0f
	}

	virtual float GetFireRate( void )
	{
		return 5.0f;
	}

	virtual const Vector& GetBulletSpread( void )
	{
		static Vector cone = VECTOR_CONE_15DEGREES;
		if( !GetOwner() || !GetOwner()->IsNPC() )
		{
			return cone;
		}

		static Vector NPCCone = VECTOR_CONE_5DEGREES;

		return NPCCone;
	}
#endif

	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	DECLARE_ACTTABLE();

private:

	void	StopEffects( void );
	void	SetSkin( int skinNum );
	void	CheckZoomToggle( void );
	void	FireBolt( void );
#ifdef MAPBASE
	void	SetBolt( int iSetting );
#ifdef GAME_DLL
	void	FireNPCBolt( CAI_BaseNPC* pOwner, Vector& vecShootOrigin, Vector& vecShootDir );
#endif
#endif
	void	ToggleZoom( void );

	// Various states for the crossbow's charger
	enum ChargerState_t
	{
		CHARGER_STATE_START_LOAD,
		CHARGER_STATE_START_CHARGE,
		CHARGER_STATE_READY,
		CHARGER_STATE_DISCHARGE,
		CHARGER_STATE_OFF,
	};

	void	CreateChargerEffects( void );
	void	SetChargerState( ChargerState_t state );
	void	DoLoadEffect( void );

private:

	// Charger effects
	ChargerState_t		m_nChargeState;

#ifndef CLIENT_DLL
	CHandle<CSprite>	m_hChargerSprite;
#endif

	CNetworkVar( bool,	m_bInZoom );
	CNetworkVar( bool,	m_bMustReload );

	CWeaponCrossbow( const CWeaponCrossbow& );
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponCrossbow, DT_WeaponCrossbow )

BEGIN_NETWORK_TABLE( CWeaponCrossbow, DT_WeaponCrossbow )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bInZoom ) ),
	RecvPropBool( RECVINFO( m_bMustReload ) ),
#else
	SendPropBool( SENDINFO( m_bInZoom ) ),
	SendPropBool( SENDINFO( m_bMustReload ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
	BEGIN_PREDICTION_DATA( CWeaponCrossbow )
	DEFINE_PRED_FIELD( m_bInZoom, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bMustReload, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	END_PREDICTION_DATA()
#endif

LINK_ENTITY_TO_CLASS( weapon_crossbow, CWeaponCrossbow );

PRECACHE_WEAPON_REGISTER( weapon_crossbow );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponCrossbow::m_acttable[] =
{
#if EXPANDED_HL2_WEAPON_ACTIVITIES
	{ ACT_RANGE_ATTACK1,			ACT_RANGE_ATTACK_CROSSBOW,		true },
	{ ACT_RELOAD,					ACT_RELOAD_CROSSBOW,			true },
	{ ACT_IDLE,						ACT_IDLE_CROSSBOW,				true },
	{ ACT_IDLE_ANGRY,				ACT_IDLE_ANGRY_CROSSBOW,		true },

// Readiness activities (not aiming)
	{ ACT_IDLE_RELAXED,				ACT_IDLE_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_IDLE_STIMULATED,			ACT_IDLE_CROSSBOW_STIMULATED,		false },
	{ ACT_IDLE_AGITATED,			ACT_IDLE_ANGRY_CROSSBOW,			false },//always aims

	{ ACT_WALK_RELAXED,				ACT_WALK_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_WALK_STIMULATED,			ACT_WALK_CROSSBOW_STIMULATED,		false },
	{ ACT_WALK_AGITATED,			ACT_WALK_AIM_CROSSBOW,				false },//always aims

	{ ACT_RUN_RELAXED,				ACT_RUN_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_RUN_STIMULATED,			ACT_RUN_CROSSBOW_STIMULATED,		false },
	{ ACT_RUN_AGITATED,				ACT_RUN_AIM_CROSSBOW,				false },//always aims

// Readiness activities (aiming)
	{ ACT_IDLE_AIM_RELAXED,			ACT_IDLE_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_IDLE_AIM_STIMULATED,		ACT_IDLE_AIM_CROSSBOW_STIMULATED,	false },
	{ ACT_IDLE_AIM_AGITATED,		ACT_IDLE_ANGRY_CROSSBOW,			false },//always aims

	{ ACT_WALK_AIM_RELAXED,			ACT_WALK_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_WALK_AIM_STIMULATED,		ACT_WALK_AIM_CROSSBOW_STIMULATED,	false },
	{ ACT_WALK_AIM_AGITATED,		ACT_WALK_AIM_CROSSBOW,				false },//always aims

	{ ACT_RUN_AIM_RELAXED,			ACT_RUN_CROSSBOW_RELAXED,			false },//never aims
	{ ACT_RUN_AIM_STIMULATED,		ACT_RUN_AIM_CROSSBOW_STIMULATED,	false },
	{ ACT_RUN_AIM_AGITATED,			ACT_RUN_AIM_CROSSBOW,				false },//always aims
//End readiness activities

	{ ACT_WALK,						ACT_WALK_CROSSBOW,					true },
	{ ACT_WALK_AIM,					ACT_WALK_AIM_CROSSBOW,				true },
	{ ACT_WALK_CROUCH,				ACT_WALK_CROUCH_RIFLE,				true },
	{ ACT_WALK_CROUCH_AIM,			ACT_WALK_CROUCH_AIM_RIFLE,			true },
	{ ACT_RUN,						ACT_RUN_CROSSBOW,					true },
	{ ACT_RUN_AIM,					ACT_RUN_AIM_CROSSBOW,				true },
	{ ACT_RUN_CROUCH,				ACT_RUN_CROUCH_RIFLE,				true },
	{ ACT_RUN_CROUCH_AIM,			ACT_RUN_CROUCH_AIM_RIFLE,			true },
	{ ACT_GESTURE_RANGE_ATTACK1,	ACT_GESTURE_RANGE_ATTACK_CROSSBOW,	true },
	{ ACT_RANGE_ATTACK1_LOW,		ACT_RANGE_ATTACK_CROSSBOW_LOW,		true },
	{ ACT_COVER_LOW,				ACT_COVER_CROSSBOW_LOW,				false },
	{ ACT_RANGE_AIM_LOW,			ACT_RANGE_AIM_CROSSBOW_LOW,			false },
	{ ACT_RELOAD_LOW,				ACT_RELOAD_CROSSBOW_LOW,			false },
	{ ACT_GESTURE_RELOAD,			ACT_GESTURE_RELOAD_CROSSBOW,		true },

	{ ACT_ARM,						ACT_ARM_RIFLE,					false },
	{ ACT_DISARM,					ACT_DISARM_RIFLE,				false },
#else
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
#endif

#if EXPANDED_HL2_COVER_ACTIVITIES
	{ ACT_RANGE_AIM_MED,			ACT_RANGE_AIM_CROSSBOW_MED,			false },
	{ ACT_RANGE_ATTACK1_MED,		ACT_RANGE_ATTACK_CROSSBOW_MED,		false },
#endif

	// HL2:DM activities (for third-person animations in SP)
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_CROSSBOW,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_CROSSBOW,				false },

	{ ACT_MP_RUN,						ACT_HL2MP_RUN_CROSSBOW,						false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_CROSSBOW,					false },
#endif
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_CROSSBOW,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK2_CROSSBOW,    false },
#else
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,	ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	 false },
#endif

	{ ACT_MP_RELOAD_STAND,				ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	false },
	{ ACT_MP_RELOAD_CROUCH,				ACT_HL2MP_GESTURE_RANGE_ATTACK_CROSSBOW,	false },

	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_CROSSBOW,					false },
};

IMPLEMENT_ACTTABLE( CWeaponCrossbow );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CWeaponCrossbow::CWeaponCrossbow( void )
{
	m_bReloadsSingly	= true;
	m_bFiresUnderwater	= true;
	m_bAltFiresUnderwater = true;
	m_bInZoom			= false;
	m_bMustReload		= false;

#ifdef MAPBASE
	m_fMinRange1 = 24;
	m_fMaxRange1 = 5000;
	m_fMinRange2 = 24;
	m_fMaxRange2 = 5000;
#endif
}

#define	CROSSBOW_GLOW_SPRITE	"sprites/light_glow02_noz.vmt"
#define	CROSSBOW_GLOW_SPRITE2	"sprites/blueflare1.vmt"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::Precache( void )
{
#ifndef CLIENT_DLL
	UTIL_PrecacheOther( "crossbow_bolt" );
#endif

	PrecacheScriptSound( "Weapon_Crossbow.BoltHitBody" );
	PrecacheScriptSound( "Weapon_Crossbow.BoltHitWorld" );
	PrecacheScriptSound( "Weapon_Crossbow.BoltSkewer" );

	PrecacheModel( CROSSBOW_GLOW_SPRITE );
	PrecacheModel( CROSSBOW_GLOW_SPRITE2 );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::PrimaryAttack( void )
{
	if( m_bInZoom && g_pGameRules->IsMultiplayer() )
	{
//		FireSniperBolt();
		FireBolt();
	}
	else
	{
		FireBolt();
	}

	// Signal a reload
	m_bMustReload = true;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration( ACT_VM_PRIMARYATTACK ) );

	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );
	if( pPlayer )
	{
#ifdef GAME_DLL
		m_iPrimaryAttacks++;
		gamestats->Event_WeaponFired( pPlayer, true, GetClassname() );
#endif
#ifdef MAPBASE
		ToHL2MPPlayer( pPlayer )->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
#endif
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::SecondaryAttack( void )
{
	//NOTENOTE: The zooming is handled by the post/busy frames
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponCrossbow::Reload( void )
{
	if( BaseClass::Reload() )
	{
		m_bMustReload = false;
		return true;
	}

	return false;
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::Reload_NPC( bool bPlaySound )
{
	BaseClass::Reload_NPC( bPlaySound );

	SetBolt( 0 );
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::CheckZoomToggle( void )
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer->m_afButtonPressed & IN_ATTACK2 )
	{
		ToggleZoom();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::ItemBusyFrame( void )
{
	// Allow zoom toggling even when we're reloading
	CheckZoomToggle();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::ItemPostFrame( void )
{
	// Allow zoom toggling
	CheckZoomToggle();

	if( m_bMustReload && HasWeaponIdleTimeElapsed() )
	{
		Reload();
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::FireBolt( void )
{
	if( m_iClip1 <= 0 )
	{
		if( !m_bFireOnEmpty )
		{
			Reload();
		}
		else
		{
			WeaponSound( EMPTY );
			m_flNextPrimaryAttack = 0.15;
		}

		return;
	}

	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );

	if( pOwner == NULL )
	{
		return;
	}

#ifdef GAME_DLL
	pOwner->RumbleEffect( RUMBLE_357, 0, RUMBLE_FLAG_RESTART );

	Vector vecAiming	= pOwner->GetAutoaimVector( 0 );
	Vector vecSrc		= pOwner->Weapon_ShootPosition();

	QAngle angAiming;
	VectorAngles( vecAiming, angAiming );

#if defined(HL2_EPISODIC)
	// !!!HACK - the other piece of the Alyx crossbow bolt hack for Outland_10 (see ::BoltTouch() for more detail)
	if( FStrEq( STRING( gpGlobals->mapname ), "ep2_outland_10" ) )
	{
		trace_t tr;
		UTIL_TraceLine( vecSrc, vecSrc + vecAiming * 24.0f, MASK_SOLID, pOwner, COLLISION_GROUP_NONE, &tr );

		if( tr.m_pEnt != NULL && tr.m_pEnt->Classify() == CLASS_PLAYER_ALLY_VITAL )
		{
			// If Alyx is right in front of the player, make sure the bolt starts outside of the player's BBOX, or the bolt
			// will instantly collide with the player after the owner of the bolt is switched to Alyx in ::BoltTouch(). We
			// avoid this altogether by making it impossible for the bolt to collide with the player.
			vecSrc += vecAiming * 24.0f;
		}
	}
#endif

	CCrossbowBolt* pBolt = CCrossbowBolt::BoltCreate( vecSrc, angAiming, pOwner );

	if( pOwner->GetWaterLevel() == 3 )
	{
		pBolt->SetAbsVelocity( vecAiming * BOLT_WATER_VELOCITY );
	}
	else
	{
		pBolt->SetAbsVelocity( vecAiming * BOLT_AIR_VELOCITY );
	}

#endif

	m_iClip1--;

#ifdef MAPBASE
	SetBolt( 1 );
#endif

	pOwner->ViewPunch( QAngle( -2, 0, 0 ) );

	WeaponSound( SINGLE );
	WeaponSound( SPECIAL2 );

#ifdef GAME_DLL
	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 200, 0.2 );
#endif

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	if( !m_iClip1 && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
	{
		// HEV suit - indicate out of ammo condition
		pOwner->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );
	}

	m_flNextPrimaryAttack = m_flNextSecondaryAttack	= gpGlobals->curtime + 0.75;

	DoLoadEffect();
	SetChargerState( CHARGER_STATE_DISCHARGE );
}

//-----------------------------------------------------------------------------
// Purpose:
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponCrossbow::Deploy( void )
{
	if( m_iClip1 <= 0 )
	{
#ifdef MAPBASE
		SetBolt( 1 );
#endif
		return DefaultDeploy( ( char* )GetViewModel(), ( char* )GetWorldModel(), ACT_CROSSBOW_DRAW_UNLOADED, ( char* )GetAnimPrefix() );
	}

	SetSkin( BOLT_SKIN_GLOW );

#ifdef MAPBASE
	SetBolt( 0 );
#endif

	return BaseClass::Deploy();
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose: Sets whether or not the bolt is visible
//-----------------------------------------------------------------------------
inline void CWeaponCrossbow::SetBolt( int iSetting )
{
	int iBody = FindBodygroupByName( "bolt" );
	if( iBody != -1 /*|| (GetOwner() && GetOwner()->IsPlayer())*/ ) // TODO: Player models check the viewmodel instead of the worldmodel, but setting the bodygroup regardless can cause a crash, so we need a better solution
	{
		SetBodygroup( iBody, iSetting );
	}
	else
	{
		m_nSkin = iSetting;
	}
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::FireNPCBolt( CAI_BaseNPC* pOwner, Vector& vecShootOrigin, Vector& vecShootDir )
{
	Assert( pOwner );

	QAngle angAiming;
	VectorAngles( vecShootDir, angAiming );

	CCrossbowBolt* pBolt = CCrossbowBolt::BoltCreate( vecShootOrigin, angAiming, pOwner );

	if( pOwner->GetWaterLevel() == 3 )
	{
		pBolt->SetAbsVelocity( vecShootDir * BOLT_WATER_VELOCITY );
	}
	else
	{
		pBolt->SetAbsVelocity( vecShootDir * BOLT_AIR_VELOCITY );
	}

	m_iClip1--;

	SetBolt( 1 );

	WeaponSound( SINGLE_NPC );
	WeaponSound( SPECIAL2 );

	CSoundEnt::InsertSound( SOUND_COMBAT, GetAbsOrigin(), 200, 0.2 );

	m_flNextPrimaryAttack = m_flNextSecondaryAttack	= gpGlobals->curtime + 2.5f;

	SetSkin( BOLT_SKIN_GLOW );
	SetChargerState( CHARGER_STATE_DISCHARGE );
}
#endif
#endif

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pSwitchingTo -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CWeaponCrossbow::Holster( CBaseCombatWeapon* pSwitchingTo )
{
	StopEffects();

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::ToggleZoom( void )
{
	CBasePlayer* pPlayer = ToBasePlayer( GetOwner() );

	if( pPlayer == NULL )
	{
		return;
	}

#ifdef GAME_DLL
	if( m_bInZoom )
	{
		if( pPlayer->SetFOV( this, 0, 0.2f ) )
		{
			m_bInZoom = false;
		}
	}
	else
	{
		if( pPlayer->SetFOV( this, 20, 0.1f ) )
		{
			m_bInZoom = true;
		}
	}
#endif
}

#define	BOLT_TIP_ATTACHMENT	2

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::CreateChargerEffects( void )
{
#ifndef CLIENT_DLL
	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );

	if( m_hChargerSprite != NULL )
	{
		return;
	}

	m_hChargerSprite = CSprite::SpriteCreate( CROSSBOW_GLOW_SPRITE, GetAbsOrigin(), false );

	if( m_hChargerSprite )
	{
		m_hChargerSprite->SetAttachment( pOwner->GetViewModel(), BOLT_TIP_ATTACHMENT );
		m_hChargerSprite->SetTransparency( kRenderTransAdd, 255, 128, 0, 255, kRenderFxNoDissipation );
		m_hChargerSprite->SetBrightness( 0 );
		m_hChargerSprite->SetScale( 0.1f );
		m_hChargerSprite->TurnOff();
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : skinNum -
//-----------------------------------------------------------------------------
void CWeaponCrossbow::SetSkin( int skinNum )
{
	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );

	if( pOwner == NULL )
	{
		return;
	}

	CBaseViewModel* pViewModel = pOwner->GetViewModel();

	if( pViewModel == NULL )
	{
		return;
	}

	pViewModel->m_nSkin = skinNum;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::DoLoadEffect( void )
{
	IPredictionSystem::SuppressHostEvents( NULL );

	SetSkin( BOLT_SKIN_GLOW );

	CBasePlayer* pOwner = ToBasePlayer( GetOwner() );

	if( pOwner == NULL )
	{
		return;
	}

	//Tony; change this up a bit; on the server, dispatch an effect but don't send it to the client who fires
	//on the client, create an effect either in the view model, or on the world model if first person.
	CEffectData	data;

	data.m_nAttachmentIndex = 1;

#ifdef GAME_DLL
	data.m_nEntIndex = entindex();

	CPASFilter filter( data.m_vOrigin );
	filter.RemoveRecipient( pOwner );
	te->DispatchEffect( filter, 0.0, data.m_vOrigin, "CrossbowLoad", data );
#else
	CBaseViewModel* pViewModel = pOwner->GetViewModel();

	if( pViewModel != NULL )
	{

		if( ::input->CAM_IsThirdPerson() )
		{
			data.m_hEntity = pViewModel->GetRefEHandle();
		}
		else
		{
			data.m_hEntity = GetRefEHandle();
		}
		DispatchEffect( "CrossbowLoad", data );
	}
#endif

	//Tony; switched this up, always attach it to the weapon, not the view model!!
#ifndef CLIENT_DLL
	CSprite* pBlast = CSprite::SpriteCreate( CROSSBOW_GLOW_SPRITE2, GetAbsOrigin(), false );

	if( pBlast )
	{
		pBlast->SetAttachment( this, 1 );
		pBlast->SetTransparency( kRenderTransAdd, 255, 255, 255, 255, kRenderFxNone );
		pBlast->SetBrightness( 128 );
		pBlast->SetScale( 0.2f );
		pBlast->FadeOutFromSpawn();
	}
#endif

}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : state -
//-----------------------------------------------------------------------------
void CWeaponCrossbow::SetChargerState( ChargerState_t state )
{
	// Make sure we're setup
	CreateChargerEffects();

	// Don't do this twice
	if( state == m_nChargeState )
	{
		return;
	}

	m_nChargeState = state;

	switch( m_nChargeState )
	{
		case CHARGER_STATE_START_LOAD:

			WeaponSound( SPECIAL1 );

			// Shoot some sparks and draw a beam between the two outer points
			DoLoadEffect();

#ifdef MAPBASE
			SetBolt( 0 );
#endif

			break;
#ifndef CLIENT_DLL
		case CHARGER_STATE_START_CHARGE:
		{
			IPredictionSystem::SuppressHostEvents( NULL );

			if( m_hChargerSprite == NULL )
			{
				break;
			}

			m_hChargerSprite->SetBrightness( 32, 0.5f );
			m_hChargerSprite->SetScale( 0.025f, 0.5f );
			m_hChargerSprite->TurnOn();
		}

		break;

		case CHARGER_STATE_READY:
		{
			// Get fully charged
			if( m_hChargerSprite == NULL )
			{
				break;
			}

			m_hChargerSprite->SetBrightness( 80, 1.0f );
			m_hChargerSprite->SetScale( 0.1f, 0.5f );
			m_hChargerSprite->TurnOn();
		}

		break;

		case CHARGER_STATE_DISCHARGE:
		{
			SetSkin( BOLT_SKIN_NORMAL );

			if( m_hChargerSprite == NULL )
			{
				break;
			}

			m_hChargerSprite->SetBrightness( 0 );
			m_hChargerSprite->TurnOff();
		}

		break;
#endif
		case CHARGER_STATE_OFF:
		{
			SetSkin( BOLT_SKIN_NORMAL );

#ifndef CLIENT_DLL
			if( m_hChargerSprite == NULL )
			{
				break;
			}

			m_hChargerSprite->SetBrightness( 0 );
			m_hChargerSprite->TurnOff();
#endif
		}
		break;

		default:
			break;
	}
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pEvent -
//			*pOperator -
//-----------------------------------------------------------------------------
void CWeaponCrossbow::Operator_HandleAnimEvent( animevent_t* pEvent, CBaseCombatCharacter* pOperator )
{
	switch( pEvent->event )
	{
		case EVENT_WEAPON_THROW:
			SetChargerState( CHARGER_STATE_START_LOAD );
			break;

		case EVENT_WEAPON_THROW2:
			SetChargerState( CHARGER_STATE_START_CHARGE );
			break;

		case EVENT_WEAPON_THROW3:
			SetChargerState( CHARGER_STATE_READY );
			break;

#ifdef MAPBASE
		case EVENT_WEAPON_SMG1:
		{
			CAI_BaseNPC* pNPC = pOperator->MyNPCPointer();
			Assert( pNPC );

			Vector vecSrc = pNPC->Weapon_ShootPosition();
			Vector vecAiming = pNPC->GetActualShootTrajectory( vecSrc );

			FireNPCBolt( pNPC, vecSrc, vecAiming );
			//m_bMustReload = true;
		}
		break;
#endif

		default:
			BaseClass::Operator_HandleAnimEvent( pEvent, pOperator );
			break;
	}
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::Operator_ForceNPCFire( CBaseCombatCharacter* pOperator, bool bSecondary )
{
	// Ensure we have enough rounds in the clip
	m_iClip1++;

	Vector vecShootOrigin, vecShootDir;
	QAngle	angShootDir;
	GetAttachment( LookupAttachment( "muzzle" ), vecShootOrigin, angShootDir );
	AngleVectors( angShootDir, &vecShootDir );
	FireNPCBolt( pOperator->MyNPCPointer(), vecShootOrigin, vecShootDir );

	//m_bMustReload = true;
}
#endif
#endif

//-----------------------------------------------------------------------------
// Purpose: Set the desired activity for the weapon and its viewmodel counterpart
// Input  : iActivity - activity to play
//-----------------------------------------------------------------------------
bool CWeaponCrossbow::SendWeaponAnim( int iActivity )
{
	int newActivity = iActivity;

	// The last shot needs a non-loaded activity
	if( ( newActivity == ACT_VM_IDLE ) && ( m_iClip1 <= 0 ) )
	{
		newActivity = ACT_VM_FIDGET;
	}

	//For now, just set the ideal activity and be done with it
	return BaseClass::SendWeaponAnim( newActivity );
}

//-----------------------------------------------------------------------------
// Purpose: Stop all zooming and special effects on the viewmodel
//-----------------------------------------------------------------------------
void CWeaponCrossbow::StopEffects( void )
{
	// Stop zooming
	if( m_bInZoom )
	{
		ToggleZoom();
	}

	// Turn off our sprites
	SetChargerState( CHARGER_STATE_OFF );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CWeaponCrossbow::Drop( const Vector& vecVelocity )
{
	StopEffects();
	BaseClass::Drop( vecVelocity );
}