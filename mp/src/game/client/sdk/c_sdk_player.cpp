//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "c_sdk_player.h"
#include "c_user_message_register.h"
#include "weapon_sdkbase.h"
#include "c_basetempentity.h"
#include "iclientvehicle.h"
#include "prediction.h"
#include "view.h"
#include "iviewrender.h"
#include "ivieweffects.h"
#include "view_scene.h"
#include "fx.h"
#include "collisionutils.h"
#include "c_sdk_team.h"
#include "obstacle_pushaway.h"
#include "bone_setup.h"
#include "cl_animevent.h"
#include "soundenvelope.h"
#ifdef MAPBASE
	#include "mapbase/mapbase_viewmodel.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined( CSDKPlayer )
	#undef CSDKPlayer
#endif

// -------------------------------------------------------------------------------- //
// Player animation event. Sent to the client when a player fires, jumps, reloads, etc..
// -------------------------------------------------------------------------------- //

class C_TEPlayerAnimEvent : public C_BaseTempEntity
{
public:
	DECLARE_CLASS( C_TEPlayerAnimEvent, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	virtual void PostDataUpdate( DataUpdateType_t updateType )
	{
		// Create the effect.
		C_SDKPlayer* pPlayer = dynamic_cast< C_SDKPlayer* >( m_hPlayer.Get() );
		if( pPlayer && !pPlayer->IsDormant() )
		{
			pPlayer->DoAnimationEvent( ( PlayerAnimEvent_t )m_iEvent.Get(), m_nData );
		}
	}

public:
	CNetworkHandle( CBasePlayer, m_hPlayer );
	CNetworkVar( int, m_iEvent );
	CNetworkVar( int, m_nData );
};

IMPLEMENT_CLIENTCLASS_EVENT( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent, CTEPlayerAnimEvent );

BEGIN_RECV_TABLE_NOBASE( C_TEPlayerAnimEvent, DT_TEPlayerAnimEvent )
RecvPropEHandle( RECVINFO( m_hPlayer ) ),
				 RecvPropInt( RECVINFO( m_iEvent ) ),
				 RecvPropInt( RECVINFO( m_nData ) )
				 END_RECV_TABLE()

				 void __MsgFunc_ReloadEffect( bf_read& msg )
{
	int iPlayer = msg.ReadShort();
	C_SDKPlayer* pPlayer = dynamic_cast< C_SDKPlayer* >( C_BaseEntity::Instance( iPlayer ) );
	if( pPlayer )
	{
		pPlayer->PlayReloadEffect();
	}

}
USER_MESSAGE_REGISTER( ReloadEffect );

// CSDKPlayerShared Data Tables
//=============================

// specific to the local player ( ideally should not be in CSDKPlayerShared! )
BEGIN_RECV_TABLE_NOBASE( CSDKPlayerShared, DT_SDKSharedLocalPlayerExclusive )
#if defined ( SDK_USE_PLAYERCLASSES )
	RecvPropInt( RECVINFO( m_iPlayerClass ) ),
	RecvPropInt( RECVINFO( m_iDesiredPlayerClass ) ),
#endif
END_RECV_TABLE()

BEGIN_RECV_TABLE_NOBASE( CSDKPlayerShared, DT_SDKPlayerShared )
#if defined ( SDK_USE_STAMINA ) || defined ( SDK_USE_SPRINTING )
	RecvPropFloat( RECVINFO( m_flStamina ) ),
#endif

#if defined ( SDK_USE_PRONE )
	RecvPropBool( RECVINFO( m_bProne ) ),
	RecvPropTime( RECVINFO( m_flGoProneTime ) ),
	RecvPropTime( RECVINFO( m_flUnProneTime ) ),
#endif
#if defined( SDK_USE_SPRINTING )
	RecvPropBool( RECVINFO( m_bIsSprinting ) ),
#endif
RecvPropDataTable( "sdksharedlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_SDKSharedLocalPlayerExclusive ) ),
				   END_RECV_TABLE()

				   BEGIN_RECV_TABLE_NOBASE( C_SDKPlayer, DT_SDKLocalPlayerExclusive )
				   RecvPropInt( RECVINFO( m_iShotsFired ) ),
				   RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),

				   RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
//	RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
				   RecvPropInt( RECVINFO( m_ArmorValue ) ),
				   END_RECV_TABLE()

				   BEGIN_RECV_TABLE_NOBASE( C_SDKPlayer, DT_SDKNonLocalPlayerExclusive )
				   RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),

				   RecvPropFloat( RECVINFO( m_angEyeAngles[0] ) ),
				   RecvPropFloat( RECVINFO( m_angEyeAngles[1] ) ),
				   END_RECV_TABLE()

// main table
				   IMPLEMENT_CLIENTCLASS_DT( C_SDKPlayer, DT_SDKPlayer, CSDKPlayer )
				   RecvPropDataTable( RECVINFO_DT( m_Shared ), 0, &REFERENCE_RECV_TABLE( DT_SDKPlayerShared ) ),

				   RecvPropDataTable( "sdklocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_SDKLocalPlayerExclusive ) ),
				   RecvPropDataTable( "sdknonlocaldata", 0, 0, &REFERENCE_RECV_TABLE( DT_SDKNonLocalPlayerExclusive ) ),

				   RecvPropEHandle( RECVINFO( m_hRagdoll ) ),

				   RecvPropInt( RECVINFO( m_iPlayerState ) ),

				   RecvPropBool( RECVINFO( m_bSpawnInterpCounter ) ),
				   END_RECV_TABLE()

// ------------------------------------------------------------------------------------------ //
// Prediction tables.
// ------------------------------------------------------------------------------------------ //
				   BEGIN_PREDICTION_DATA_NO_BASE( CSDKPlayerShared )
#if defined ( SDK_USE_STAMINA ) || defined ( SDK_USE_SPRINTING )
	DEFINE_PRED_FIELD( m_flStamina, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
#if defined( SDK_USE_PRONE )
	DEFINE_PRED_FIELD( m_bProne, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flGoProneTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flUnProneTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
#if defined( SDK_USE_SPRINTING )
	DEFINE_PRED_FIELD( m_bIsSprinting, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
#endif
				   END_PREDICTION_DATA()

				   BEGIN_PREDICTION_DATA( C_SDKPlayer )
				   DEFINE_PRED_FIELD( m_flCycle, FIELD_FLOAT, FTYPEDESC_OVERRIDE | FTYPEDESC_PRIVATE | FTYPEDESC_NOERRORCHECK ),
				   DEFINE_PRED_FIELD( m_iShotsFired, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
				   END_PREDICTION_DATA()

				   LINK_ENTITY_TO_CLASS( player, C_SDKPlayer );

class C_SDKRagdoll : public C_BaseAnimatingOverlay
{
public:
	DECLARE_CLASS( C_SDKRagdoll, C_BaseAnimatingOverlay );
	DECLARE_CLIENTCLASS();

	C_SDKRagdoll();
	~C_SDKRagdoll();

	virtual void OnDataChanged( DataUpdateType_t type );

	int GetPlayerEntIndex() const;
	IRagdoll* GetIRagdoll() const;

	void ImpactTrace( trace_t* pTrace, int iDamageType, const char* pCustomImpactName );
	void UpdateOnRemove( void );
	virtual void SetupWeights( const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights );

private:

	C_SDKRagdoll( const C_SDKRagdoll& ) {}

	void Interp_Copy( C_BaseAnimatingOverlay* pDestinationEntity );
	void CreateSDKRagdoll( void );

private:

	EHANDLE	m_hPlayer;
	CNetworkVector( m_vecRagdollVelocity );
	CNetworkVector( m_vecRagdollOrigin );
};

IMPLEMENT_CLIENTCLASS_DT_NOBASE( C_SDKRagdoll, DT_SDKRagdoll, CSDKRagdoll )
RecvPropVector( RECVINFO( m_vecRagdollOrigin ) ),
				RecvPropEHandle( RECVINFO( m_hPlayer ) ),
				RecvPropInt( RECVINFO( m_nModelIndex ) ),
				RecvPropInt( RECVINFO( m_nForceBone ) ),
				RecvPropVector( RECVINFO( m_vecForce ) ),
				RecvPropVector( RECVINFO( m_vecRagdollVelocity ) )
				END_RECV_TABLE()

				C_SDKRagdoll::C_SDKRagdoll()
{

}

C_SDKRagdoll::~C_SDKRagdoll()
{
	PhysCleanupFrictionSounds( this );

	if( m_hPlayer )
	{
		m_hPlayer->CreateModelInstance();
	}
}

void C_SDKRagdoll::Interp_Copy( C_BaseAnimatingOverlay* pSourceEntity )
{
	if( !pSourceEntity )
	{
		return;
	}

	VarMapping_t* pSrc = pSourceEntity->GetVarMapping();
	VarMapping_t* pDest = GetVarMapping();

	// Find all the VarMapEntry_t's that represent the same variable.
	for( int i = 0; i < pDest->m_Entries.Count(); i++ )
	{
		VarMapEntry_t* pDestEntry = &pDest->m_Entries[i];
		const char* pszName = pDestEntry->watcher->GetDebugName();
		for( int j = 0; j < pSrc->m_Entries.Count(); j++ )
		{
			VarMapEntry_t* pSrcEntry = &pSrc->m_Entries[j];
			if( !Q_strcmp( pSrcEntry->watcher->GetDebugName(), pszName ) )
			{
				pDestEntry->watcher->Copy( pSrcEntry->watcher );
				break;
			}
		}
	}
}

void FX_BloodSpray( const Vector& origin, const Vector& normal, float scale, unsigned char r, unsigned char g, unsigned char b, int flags );
void C_SDKRagdoll::ImpactTrace( trace_t* pTrace, int iDamageType, const char* pCustomImpactName )
{
	IPhysicsObject* pPhysicsObject = VPhysicsGetObject();

	if( !pPhysicsObject )
	{
		return;
	}

	Vector dir = pTrace->endpos - pTrace->startpos;

	if( iDamageType == DMG_BLAST )
	{
		dir *= 4000;  // adjust impact strenght

		// apply force at object mass center
		pPhysicsObject->ApplyForceCenter( dir );
	}
	else
	{
		Vector hitpos;

		VectorMA( pTrace->startpos, pTrace->fraction, dir, hitpos );
		VectorNormalize( dir );

		// Blood spray!
		FX_BloodSpray( hitpos, dir, 3, 72, 0, 0, FX_BLOODSPRAY_ALL );

		dir *= 4000;  // adjust impact strenght

		// apply force where we hit it
		pPhysicsObject->ApplyForceOffset( dir, hitpos );
		//Tony; throw in some bleeds! - just use a generic value for damage.
		TraceBleed( 40, dir, pTrace, iDamageType );
	}

	m_pRagdoll->ResetRagdollSleepAfterTime();
}


void C_SDKRagdoll::CreateSDKRagdoll( void )
{
	// First, initialize all our data. If we have the player's entity on our client,
	// then we can make ourselves start out exactly where the player is.
	C_SDKPlayer* pPlayer = dynamic_cast< C_SDKPlayer* >( m_hPlayer.Get() );

	if( pPlayer && !pPlayer->IsDormant() )
	{
		// move my current model instance to the ragdoll's so decals are preserved.
		pPlayer->SnatchModelInstance( this );

		VarMapping_t* varMap = GetVarMapping();

		// Copy all the interpolated vars from the player entity.
		// The entity uses the interpolated history to get bone velocity.
		bool bRemotePlayer = ( pPlayer != C_BasePlayer::GetLocalPlayer() );
		if( bRemotePlayer )
		{
			Interp_Copy( pPlayer );

			SetAbsAngles( pPlayer->GetRenderAngles() );
			GetRotationInterpolator().Reset();

			m_flAnimTime = pPlayer->m_flAnimTime;
			SetSequence( pPlayer->GetSequence() );
			m_flPlaybackRate = pPlayer->GetPlaybackRate();
		}
		else
		{
			// This is the local player, so set them in a default
			// pose and slam their velocity, angles and origin
			SetAbsOrigin( m_vecRagdollOrigin );

			SetAbsAngles( pPlayer->GetRenderAngles() );

			SetAbsVelocity( m_vecRagdollVelocity );

			int iSeq = pPlayer->GetSequence();
			if( iSeq == -1 )
			{
				Assert( false );	// missing walk_lower?
				iSeq = 0;
			}

			SetSequence( iSeq );	// walk_lower, basic pose
			SetCycle( 0.0 );

			Interp_Reset( varMap );
		}
	}
	else
	{
		// overwrite network origin so later interpolation will
		// use this position
		SetNetworkOrigin( m_vecRagdollOrigin );

		SetAbsOrigin( m_vecRagdollOrigin );
		SetAbsVelocity( m_vecRagdollVelocity );

		Interp_Reset( GetVarMapping() );

	}

	SetModelIndex( m_nModelIndex );

	// Make us a ragdoll..
	m_nRenderFX = kRenderFxRagdoll;

	matrix3x4_t boneDelta0[MAXSTUDIOBONES];
	matrix3x4_t boneDelta1[MAXSTUDIOBONES];
	matrix3x4_t currentBones[MAXSTUDIOBONES];
	const float boneDt = 0.05f;

	if( pPlayer && !pPlayer->IsDormant() )
	{
		pPlayer->GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}
	else
	{
		GetRagdollInitBoneArrays( boneDelta0, boneDelta1, currentBones, boneDt );
	}

	InitAsClientRagdoll( boneDelta0, boneDelta1, currentBones, boneDt );
}


void C_SDKRagdoll::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if( type == DATA_UPDATE_CREATED )
	{
		CreateSDKRagdoll();
	}
}

IRagdoll* C_SDKRagdoll::GetIRagdoll() const
{
	return m_pRagdoll;
}

void C_SDKRagdoll::UpdateOnRemove( void )
{
	VPhysicsSetObject( NULL );

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: clear out any face/eye values stored in the material system
//-----------------------------------------------------------------------------
void C_SDKRagdoll::SetupWeights( const matrix3x4_t* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights )
{
	BaseClass::SetupWeights( pBoneToWorld, nFlexWeightCount, pFlexWeights, pFlexDelayedWeights );

	static float destweight[128];
	static bool bIsInited = false;

	CStudioHdr* hdr = GetModelPtr();
	if( !hdr )
	{
		return;
	}

	int nFlexDescCount = hdr->numflexdesc();
	if( nFlexDescCount )
	{
		Assert( !pFlexDelayedWeights );
		memset( pFlexWeights, 0, nFlexWeightCount * sizeof( float ) );
	}

	if( m_iEyeAttachment > 0 )
	{
		matrix3x4_t attToWorld;
		if( GetAttachment( m_iEyeAttachment, attToWorld ) )
		{
			Vector local, tmp;
			local.Init( 1000.0f, 0.0f, 0.0f );
			VectorTransform( local, attToWorld, tmp );
			modelrender->SetViewTarget( GetModelPtr(), GetBody(), tmp );
		}
	}
}

C_BaseAnimating* C_SDKPlayer::BecomeRagdollOnClient()
{
	// Let the C_SDKRagdoll entity do this.
	return NULL;
}

IRagdoll* C_SDKPlayer::GetRepresentativeRagdoll() const
{
	if( m_hRagdoll.Get() )
	{
		C_SDKRagdoll* pRagdoll = ( C_SDKRagdoll* )m_hRagdoll.Get();

		return pRagdoll->GetIRagdoll();
	}
	else
	{
		return NULL;
	}
}

C_SDKPlayer::C_SDKPlayer() : m_iv_angEyeAngles( "C_SDKPlayer::m_iv_angEyeAngles" )
{
	m_PlayerAnimState = CreateSDKPlayerAnimState( this );
	m_Shared.Init( this );

	m_angEyeAngles.Init();
	AddVar( &m_angEyeAngles, &m_iv_angEyeAngles, LATCH_SIMULATION_VAR );

	m_fNextThinkPushAway = 0.0f;

}


C_SDKPlayer::~C_SDKPlayer()
{
	m_PlayerAnimState->Release();

#if defined ( SDK_USE_SPRINTING )
	// Kill the stamina sound!
	if( m_pStaminaSound )
	{
		CSoundEnvelopeController::GetController().SoundDestroy( m_pStaminaSound );
		m_pStaminaSound = NULL;
	}
#endif
}

C_SDKPlayer* C_SDKPlayer::GetLocalSDKPlayer()
{
	return ToSDKPlayer( C_BasePlayer::GetLocalPlayer() );
}

const QAngle& C_SDKPlayer::EyeAngles()
{
	if( IsLocalPlayer() )
	{
		return BaseClass::EyeAngles();
	}
	else
	{
		return m_angEyeAngles;
	}
}
const QAngle& C_SDKPlayer::GetRenderAngles()
{
	if( IsRagdoll() )
	{
		return vec3_angle;
	}
	else
	{
		return m_PlayerAnimState->GetRenderAngles();
	}
}


void C_SDKPlayer::UpdateClientSideAnimation()
{
	m_PlayerAnimState->Update( EyeAngles()[YAW], EyeAngles()[PITCH] );
	BaseClass::UpdateClientSideAnimation();
}
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CStudioHdr* C_SDKPlayer::OnNewModel( void )
{
	CStudioHdr* hdr = BaseClass::OnNewModel();

	InitializePoseParams();

	// Reset the players animation states, gestures
	if( m_PlayerAnimState )
	{
		m_PlayerAnimState->OnNewModel();
	}

	return hdr;
}
//-----------------------------------------------------------------------------
// Purpose: Clear all pose parameters
//-----------------------------------------------------------------------------
void C_SDKPlayer::InitializePoseParams( void )
{
	CStudioHdr* hdr = GetModelPtr();
	Assert( hdr );
	if( !hdr )
	{
		return;
	}

	m_headYawPoseParam = LookupPoseParameter( "head_yaw" );
	GetPoseParameterRange( m_headYawPoseParam, m_headYawMin, m_headYawMax );

	m_headPitchPoseParam = LookupPoseParameter( "head_pitch" );
	GetPoseParameterRange( m_headPitchPoseParam, m_headPitchMin, m_headPitchMax );

	for( int i = 0; i < hdr->GetNumPoseParameters() ; i++ )
	{
		SetPoseParameter( hdr, i, 0.0 );
	}

}
//Tony; TODO!
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
/*void C_SDKPlayer::OnPlayerClassChange( void )
{
	// Init the anim movement vars
	m_PlayerAnimState->SetRunSpeed( GetPlayerClass()->GetMaxSpeed() );
	m_PlayerAnimState->SetWalkSpeed( GetPlayerClass()->GetMaxSpeed() * 0.5 );
}
*/
void C_SDKPlayer::PostDataUpdate( DataUpdateType_t updateType )
{
	// C_BaseEntity assumes we're networking the entity's angles, so pretend that it
	// networked the same value we already have.
	SetNetworkAngles( GetLocalAngles() );

	BaseClass::PostDataUpdate( updateType );

	bool bIsLocalPlayer = IsLocalPlayer();

	if( m_bSpawnInterpCounter != m_bSpawnInterpCounterCache )
	{
		MoveToLastReceivedPosition( true );
		ResetLatched();

		if( bIsLocalPlayer )
		{
			LocalPlayerRespawn();
		}
		m_bSpawnInterpCounterCache = m_bSpawnInterpCounter.m_Value;
	}

}
// Called every time the player respawns
void C_SDKPlayer::LocalPlayerRespawn( void )
{
	ResetToneMapping( 1.0 );
#if defined ( SDK_USE_PRONE )
	m_Shared.m_bForceProneChange = true;
	m_bUnProneToDuck = false;
#endif

	InitSpeeds(); //Tony; initialize player speeds.
}

void C_SDKPlayer::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

	UpdateVisibility();
}

void C_SDKPlayer::PlayReloadEffect()
{
	// Only play the effect for other players.
	if( this == C_SDKPlayer::GetLocalSDKPlayer() )
	{
		Assert( false ); // We shouldn't have been sent this message.
		return;
	}

	// Get the view model for our current gun.
	CWeaponSDKBase* pWeapon = GetActiveSDKWeapon();
	if( !pWeapon )
	{
		return;
	}

	// The weapon needs two models, world and view, but can only cache one. Synthesize the other.
	const CSDKWeaponInfo& info = pWeapon->GetSDKWpnData();
	const model_t* pModel = modelinfo->GetModel( modelinfo->GetModelIndex( info.szViewModel ) );
	if( !pModel )
	{
		return;
	}
	CStudioHdr studioHdr( modelinfo->GetStudiomodel( pModel ), mdlcache );
	if( !studioHdr.IsValid() )
	{
		return;
	}

	// Find the reload animation.
	for( int iSeq = 0; iSeq < studioHdr.GetNumSeq(); iSeq++ )
	{
		mstudioseqdesc_t* pSeq = &studioHdr.pSeqdesc( iSeq );

		if( pSeq->activity == ACT_VM_RELOAD )
		{
			float poseParameters[MAXSTUDIOPOSEPARAM];
			memset( poseParameters, 0, sizeof( poseParameters ) );
			float cyclesPerSecond = Studio_CPS( &studioHdr, *pSeq, iSeq, poseParameters );

			// Now read out all the sound events with their timing
			for( int iEvent = 0; iEvent < pSeq->numevents; iEvent++ )
			{
				mstudioevent_t* pEvent = pSeq->pEvent( iEvent );

				if( pEvent->event == CL_EVENT_SOUND )
				{
					CSDKSoundEvent event;
					event.m_SoundName = pEvent->options;
					event.m_flEventTime = gpGlobals->curtime + pEvent->cycle / cyclesPerSecond;
					m_SoundEvents.AddToTail( event );
				}
			}

			break;
		}
	}
}

void C_SDKPlayer::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	if( IsLocalPlayer() )
	{
		if( ( prediction->InPrediction() && !prediction->IsFirstTimePredicted() ) )
		{
			return;
		}
	}

	MDLCACHE_CRITICAL_SECTION();
	m_PlayerAnimState->DoAnimationEvent( event, nData );
}

bool C_SDKPlayer::ShouldDraw( void )
{
	// If we're dead, our ragdoll will be drawn for us instead.
	if( !IsAlive() )
	{
		return false;
	}

	if( GetTeamNumber() == TEAM_SPECTATOR )
	{
		return false;
	}

	if( State_Get() == STATE_WELCOME )
	{
		return false;
	}
#if defined ( SDK_USE_TEAMS )
	if( State_Get() == STATE_PICKINGTEAM )
	{
		return false;
	}
#endif

#if defined ( SDK_USE_PLAYERCLASSES )
	if( State_Get() == STATE_PICKINGCLASS )
	{
		return false;
	}
#endif

	if( IsLocalPlayer() && IsRagdoll() )
	{
		return true;
	}

	return BaseClass::ShouldDraw();
}

CWeaponSDKBase* C_SDKPlayer::GetActiveSDKWeapon() const
{
	return dynamic_cast< CWeaponSDKBase* >( GetActiveWeapon() );
}

//-----------------------------------------------------------------------------
// Purpose: Non-Caching CalcVehicleView for Scratch SDK + Multiplayer
// TODO: fix the normal CalcVehicleView so that Caching can work in multiplayer.
//-----------------------------------------------------------------------------
void C_SDKPlayer::CalcVehicleView( IClientVehicle* pVehicle,	Vector& eyeOrigin, QAngle& eyeAngles,	float& zNear, float& zFar, float& fov )
{
	Assert( pVehicle );

	// Start with our base origin and angles
	int nRole = pVehicle->GetPassengerRole( this );

	// Get our view for this frame
	pVehicle->GetVehicleViewPosition( nRole, &eyeOrigin, &eyeAngles, &fov );

	// Allows the vehicle to change the clip planes
	pVehicle->GetVehicleClipPlanes( zNear, zFar );

	// Snack off the origin before bob + water offset are applied
	Vector vecBaseEyePosition = eyeOrigin;

	CalcViewRoll( eyeAngles );

	// Apply punch angle
	VectorAdd( eyeAngles, m_Local.m_vecPunchAngle, eyeAngles );

	if( !prediction->InPrediction() )
	{
		// Shake it up baby!
		vieweffects->CalcShake();
		vieweffects->ApplyShake( eyeOrigin, eyeAngles, 1.0 );
	}
}

#if defined ( SDK_USE_PLAYERCLASSES )
bool C_SDKPlayer::CanShowClassMenu( void )
{
#if defined ( SDK_USE_TEAMS )
	return ( GetTeamNumber() == SDK_TEAM_BLUE || GetTeamNumber() == SDK_TEAM_RED );
#else
	return ( GetTeamNumber() != TEAM_SPECTATOR );
#endif
}
#endif

#if defined ( SDK_USE_TEAMS )
bool C_SDKPlayer::CanShowTeamMenu( void )
{
	return true;
}
#endif
void C_SDKPlayer::ClientThink()
{
	UpdateSoundEvents();

	// Pass on through to the base class.
	BaseClass::ClientThink();

	bool bFoundViewTarget = false;

	Vector vForward;
	AngleVectors( GetLocalAngles(), &vForward );

	for( int iClient = 1; iClient <= gpGlobals->maxClients; ++iClient )
	{
		CBaseEntity* pEnt = UTIL_PlayerByIndex( iClient );
		if( !pEnt || !pEnt->IsPlayer() )
		{
			continue;
		}

		if( pEnt->entindex() == entindex() )
		{
			continue;
		}

		Vector vTargetOrigin = pEnt->GetAbsOrigin();
		Vector vMyOrigin =  GetAbsOrigin();

		Vector vDir = vTargetOrigin - vMyOrigin;

		if( vDir.Length() > 128 )
		{
			continue;
		}

		VectorNormalize( vDir );

		if( DotProduct( vForward, vDir ) < 0.0f )
		{
			continue;
		}

		m_vLookAtTarget = pEnt->EyePosition();
		bFoundViewTarget = true;
		break;
	}

	if( bFoundViewTarget == false )
	{
		m_vLookAtTarget = GetAbsOrigin() + vForward * 512;
	}

	if( IsLocalPlayer() )
	{
		if( IsAlive() )
		{
			UpdateIDTarget();
		}

		// Update the player's fog data if necessary.
		UpdateFogController();
	}
	else
	{
		// Update step sounds for all other players
		Vector vel;
		EstimateAbsVelocity( vel );
		UpdateStepSound( GetGroundSurface(), GetAbsOrigin(), vel );
	}

#if defined ( SDK_USE_SPRINTING )
	StaminaSoundThink();
#endif

	// Avoidance
	if( gpGlobals->curtime >= m_fNextThinkPushAway )
	{
		PerformObstaclePushaway( this );
		m_fNextThinkPushAway =  gpGlobals->curtime + PUSHAWAY_THINK_INTERVAL;
	}
}

#if defined ( SDK_USE_SPRINTING )
//-----------------------------------------------------------------------------
// Purpose:Start or stop the stamina breathing sound if necessary
//-----------------------------------------------------------------------------
void C_SDKPlayer::StaminaSoundThink( void )
{
	if( m_bPlayingLowStaminaSound )
	{
		if( !IsAlive() || m_Shared.GetStamina() >= LOW_STAMINA_THRESHOLD )
		{
			// stop the sprint sound
			CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();
			controller.SoundFadeOut( m_pStaminaSound, 1.0, true );

			// SoundFadeOut will destroy this sound, so we will have to create another one
			// if we go below the threshold again soon
			m_pStaminaSound = NULL;

			m_bPlayingLowStaminaSound = false;
		}
	}
	else
	{
		if( IsAlive() && m_Shared.GetStamina() < LOW_STAMINA_THRESHOLD )
		{
			// we are alive and have low stamina
			CLocalPlayerFilter filter;

			CSoundEnvelopeController& controller = CSoundEnvelopeController::GetController();

			if( !m_pStaminaSound )
			{
				m_pStaminaSound = controller.SoundCreate( filter, entindex(), "Player.Sprint" );
			}

			controller.Play( m_pStaminaSound, 0.0, 100 );
			controller.SoundChangeVolume( m_pStaminaSound, 1.0, 2.0 );

			m_bPlayingLowStaminaSound = true;
		}
	}
}
#endif

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_SDKPlayer::UpdateLookAt( void )
{
	// head yaw
	if( m_headYawPoseParam < 0 || m_headPitchPoseParam < 0 )
	{
		return;
	}

	// orient eyes
	m_viewtarget = m_vLookAtTarget;

	// Figure out where we want to look in world space.
	QAngle desiredAngles;
	Vector to = m_vLookAtTarget - EyePosition();
	VectorAngles( to, desiredAngles );

	// Figure out where our body is facing in world space.
	QAngle bodyAngles( 0, 0, 0 );
	bodyAngles[YAW] = GetLocalAngles()[YAW];


	float flBodyYawDiff = bodyAngles[YAW] - m_flLastBodyYaw;
	m_flLastBodyYaw = bodyAngles[YAW];


	// Set the head's yaw.
	float desired = AngleNormalize( desiredAngles[YAW] - bodyAngles[YAW] );
	desired = clamp( desired, m_headYawMin, m_headYawMax );
	m_flCurrentHeadYaw = ApproachAngle( desired, m_flCurrentHeadYaw, 130 * gpGlobals->frametime );

	// Counterrotate the head from the body rotation so it doesn't rotate past its target.
	m_flCurrentHeadYaw = AngleNormalize( m_flCurrentHeadYaw - flBodyYawDiff );
	desired = clamp( desired, m_headYawMin, m_headYawMax );

	SetPoseParameter( m_headYawPoseParam, m_flCurrentHeadYaw );


	// Set the head's yaw.
	desired = AngleNormalize( desiredAngles[PITCH] );
	desired = clamp( desired, m_headPitchMin, m_headPitchMax );

	m_flCurrentHeadPitch = ApproachAngle( desired, m_flCurrentHeadPitch, 130 * gpGlobals->frametime );
	m_flCurrentHeadPitch = AngleNormalize( m_flCurrentHeadPitch );
	SetPoseParameter( m_headPitchPoseParam, m_flCurrentHeadPitch );
}



int C_SDKPlayer::GetIDTarget() const
{
	return m_iIDEntIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Update this client's target entity
//-----------------------------------------------------------------------------
void C_SDKPlayer::UpdateIDTarget()
{
	if( !IsLocalPlayer() )
	{
		return;
	}

	// Clear old target and find a new one
	m_iIDEntIndex = 0;

	// don't show id's in any state but active.
	if( State_Get() != STATE_ACTIVE )
	{
		return;
	}

	trace_t tr;
	Vector vecStart, vecEnd;
	VectorMA( MainViewOrigin(), 1500, MainViewForward(), vecEnd );
	VectorMA( MainViewOrigin(), 10,   MainViewForward(), vecStart );
	UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID, this, COLLISION_GROUP_NONE, &tr );

	if( !tr.startsolid && tr.DidHitNonWorldEntity() )
	{
		C_BaseEntity* pEntity = tr.m_pEnt;

		if( pEntity && ( pEntity != this ) )
		{
			m_iIDEntIndex = pEntity->entindex();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Try to steer away from any players and objects we might interpenetrate
//-----------------------------------------------------------------------------
#define SDK_AVOID_MAX_RADIUS_SQR		5184.0f			// Based on player extents and max buildable extents.
#define SDK_OO_AVOID_MAX_RADIUS_SQR		0.00029f

ConVar sdk_max_separation_force( "sdk_max_separation_force", "256", FCVAR_CHEAT | FCVAR_HIDDEN );

extern ConVar cl_forwardspeed;
extern ConVar cl_backspeed;
extern ConVar cl_sidespeed;

void C_SDKPlayer::AvoidPlayers( CUserCmd* pCmd )
{
// Player Avoidance is only active with teams
#if defined ( SDK_USE_TEAMS )
	// Don't test if the player doesn't exist or is dead.
	if( IsAlive() == false )
	{
		return;
	}

	C_SDKTeam* pTeam = ( C_SDKTeam* )GetTeam();
	if( !pTeam )
	{
		return;
	}

	// Up vector.
	static Vector vecUp( 0.0f, 0.0f, 1.0f );

	Vector vecSDKPlayerCenter = GetAbsOrigin();
	Vector vecSDKPlayerMin = GetPlayerMins();
	Vector vecSDKPlayerMax = GetPlayerMaxs();
	float flZHeight = vecSDKPlayerMax.z - vecSDKPlayerMin.z;
	vecSDKPlayerCenter.z += 0.5f * flZHeight;
	VectorAdd( vecSDKPlayerMin, vecSDKPlayerCenter, vecSDKPlayerMin );
	VectorAdd( vecSDKPlayerMax, vecSDKPlayerCenter, vecSDKPlayerMax );

	// Find an intersecting player or object.
	int nAvoidPlayerCount = 0;
	C_SDKPlayer* pAvoidPlayerList[MAX_PLAYERS];

	C_SDKPlayer* pIntersectPlayer = NULL;
	float flAvoidRadius = 0.0f;

	Vector vecAvoidCenter, vecAvoidMin, vecAvoidMax;
	for( int i = 0; i < pTeam->GetNumPlayers(); ++i )
	{
		C_SDKPlayer* pAvoidPlayer = static_cast< C_SDKPlayer* >( pTeam->GetPlayer( i ) );
		if( pAvoidPlayer == NULL )
		{
			continue;
		}
		// Is the avoid player me?
		if( pAvoidPlayer == this )
		{
			continue;
		}

		// Save as list to check against for objects.
		pAvoidPlayerList[nAvoidPlayerCount] = pAvoidPlayer;
		++nAvoidPlayerCount;

		// Check to see if the avoid player is dormant.
		if( pAvoidPlayer->IsDormant() )
		{
			continue;
		}

		// Is the avoid player solid?
		if( pAvoidPlayer->IsSolidFlagSet( FSOLID_NOT_SOLID ) )
		{
			continue;
		}

		Vector t1, t2;

		vecAvoidCenter = pAvoidPlayer->GetAbsOrigin();
		vecAvoidMin = pAvoidPlayer->GetPlayerMins();
		vecAvoidMax = pAvoidPlayer->GetPlayerMaxs();
		flZHeight = vecAvoidMax.z - vecAvoidMin.z;
		vecAvoidCenter.z += 0.5f * flZHeight;
		VectorAdd( vecAvoidMin, vecAvoidCenter, vecAvoidMin );
		VectorAdd( vecAvoidMax, vecAvoidCenter, vecAvoidMax );

		if( IsBoxIntersectingBox( vecSDKPlayerMin, vecSDKPlayerMax, vecAvoidMin, vecAvoidMax ) )
		{
			// Need to avoid this player.
			if( !pIntersectPlayer )
			{
				pIntersectPlayer = pAvoidPlayer;
				break;
			}
		}
	}

	// Anything to avoid?
	if( !pIntersectPlayer )
	{
		return;
	}

	// Calculate the push strength and direction.
	Vector vecDelta;

	// Avoid a player - they have precedence.
	if( pIntersectPlayer )
	{
		VectorSubtract( pIntersectPlayer->WorldSpaceCenter(), vecSDKPlayerCenter, vecDelta );

		Vector vRad = pIntersectPlayer->WorldAlignMaxs() - pIntersectPlayer->WorldAlignMins();
		vRad.z = 0;

		flAvoidRadius = vRad.Length();
	}

	float flPushStrength = RemapValClamped( vecDelta.Length(), flAvoidRadius, 0, 0, sdk_max_separation_force.GetInt() ); //flPushScale;

	//Msg( "PushScale = %f\n", flPushStrength );

	// Check to see if we have enough push strength to make a difference.
	if( flPushStrength < 0.01f )
	{
		return;
	}

	Vector vecPush;
	if( GetAbsVelocity().Length2DSqr() > 0.1f )
	{
		Vector vecVelocity = GetAbsVelocity();
		vecVelocity.z = 0.0f;
		CrossProduct( vecUp, vecVelocity, vecPush );
		VectorNormalize( vecPush );
	}
	else
	{
		// We are not moving, but we're still intersecting.
		QAngle angView = pCmd->viewangles;
		angView.x = 0.0f;
		AngleVectors( angView, NULL, &vecPush, NULL );
	}

	// Move away from the other player/object.
	Vector vecSeparationVelocity;
	if( vecDelta.Dot( vecPush ) < 0 )
	{
		vecSeparationVelocity = vecPush * flPushStrength;
	}
	else
	{
		vecSeparationVelocity = vecPush * -flPushStrength;
	}

	// Don't allow the max push speed to be greater than the max player speed.
	float flMaxPlayerSpeed = MaxSpeed();
	float flCropFraction = 1.33333333f;

	if( ( GetFlags() & FL_DUCKING ) && ( GetGroundEntity() != NULL ) )
	{
		flMaxPlayerSpeed *= flCropFraction;
	}

	float flMaxPlayerSpeedSqr = flMaxPlayerSpeed * flMaxPlayerSpeed;

	if( vecSeparationVelocity.LengthSqr() > flMaxPlayerSpeedSqr )
	{
		vecSeparationVelocity.NormalizeInPlace();
		VectorScale( vecSeparationVelocity, flMaxPlayerSpeed, vecSeparationVelocity );
	}

	QAngle vAngles = pCmd->viewangles;
	vAngles.x = 0;
	Vector currentdir;
	Vector rightdir;

	AngleVectors( vAngles, &currentdir, &rightdir, NULL );

	Vector vDirection = vecSeparationVelocity;

	VectorNormalize( vDirection );

	float fwd = currentdir.Dot( vDirection );
	float rt = rightdir.Dot( vDirection );

	float forward = fwd * flPushStrength;
	float side = rt * flPushStrength;

	//Msg( "fwd: %f - rt: %f - forward: %f - side: %f\n", fwd, rt, forward, side );

	pCmd->forwardmove	+= forward;
	pCmd->sidemove		+= side;

	// Clamp the move to within legal limits, preserving direction. This is a little
	// complicated because we have different limits for forward, back, and side

	//Msg( "PRECLAMP: forwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );

	float flForwardScale = 1.0f;
	if( pCmd->forwardmove > fabs( cl_forwardspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_forwardspeed.GetFloat() ) / pCmd->forwardmove;
	}
	else if( pCmd->forwardmove < -fabs( cl_backspeed.GetFloat() ) )
	{
		flForwardScale = fabs( cl_backspeed.GetFloat() ) / fabs( pCmd->forwardmove );
	}

	float flSideScale = 1.0f;
	if( fabs( pCmd->sidemove ) > fabs( cl_sidespeed.GetFloat() ) )
	{
		flSideScale = fabs( cl_sidespeed.GetFloat() ) / fabs( pCmd->sidemove );
	}

	float flScale = min( flForwardScale, flSideScale );
	pCmd->forwardmove *= flScale;
	pCmd->sidemove *= flScale;

	//Msg( "Pforwardmove=%f, sidemove=%f\n", pCmd->forwardmove, pCmd->sidemove );
#endif
}

bool C_SDKPlayer::CreateMove( float flInputSampleTime, CUserCmd* pCmd )
{
	static QAngle angMoveAngle( 0.0f, 0.0f, 0.0f );

	VectorCopy( pCmd->viewangles, angMoveAngle );

	BaseClass::CreateMove( flInputSampleTime, pCmd );

	AvoidPlayers( pCmd );

	return true;
}

void C_SDKPlayer::UpdateSoundEvents()
{
	int iNext;
	for( int i = m_SoundEvents.Head(); i != m_SoundEvents.InvalidIndex(); i = iNext )
	{
		iNext = m_SoundEvents.Next( i );

		CSDKSoundEvent* pEvent = &m_SoundEvents[i];
		if( gpGlobals->curtime >= pEvent->m_flEventTime )
		{
			CLocalPlayerFilter filter;
			EmitSound( filter, GetSoundSourceIndex(), STRING( pEvent->m_SoundName ) );

			m_SoundEvents.Remove( i );
		}
	}
}

#ifdef MAPBASE_MP
void C_SDKPlayer::OnSpawn()
{
	if( IsLocalPlayer() )
	{
		// By default display hands.
		// This hack has to be here because SetWeaponVisible isn't called on client when the player spawns.
		C_MapbaseViewModel* pHands = static_cast<C_MapbaseViewModel*>( GetViewModel( VMINDEX_HANDS, false ) );
		if( pHands )
		{
			pHands->SetDrawVM( true );
		}
	}
}

void C_SDKPlayer::TeamChange( int iNewTeam )
{
	BaseClass::TeamChange( iNewTeam );

	// Execute a .cfg base on what team we have change
	if( iNewTeam == SDK_TEAM_BLUE )
	{
		engine->ClientCmd_Unrestricted( "exec sdk_blue.cfg" );
	}
	else if( iNewTeam == SDK_TEAM_RED )
	{
		engine->ClientCmd_Unrestricted( "exec sdk_red.cfg" );
	}
}

#endif