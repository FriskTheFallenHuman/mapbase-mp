//========= Copyright � 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "base_playeranimstate.h"
#include "tier0/vprof.h"
#include "animation.h"
#include "studio.h"
#include "apparent_velocity_helper.h"
#include "utldict.h"
#include "hl2mp_playeranimstate.h"
#include "base_playeranimstate.h"
#include "in_buttons.h"
#include "datacache/imdlcache.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
	#include "iclientvehicle.h"
	#include "c_vehicle_jeep.h"
	//#include "c_vehicle_airboat.h"
#else
	#include "hl2mp_player.h"
	#include "iservervehicle.h"
	#include "vehicle_jeep.h"
	#ifdef MAPBASE_MP
		#include "weapon_physcannon.h"
	#endif // MAPBASE_MP
#endif

#ifdef MAPBASE_MP
	#define MAX_FEET_GOAL_DELTA         45.0f

	// Don't let the head spass out.
	#define HL2MP_LOOKAT_UPDATE_TIME       0.1f
	#define HL2MP_LOOKAT_DIST              100.0f
	#define HL2MP_LOOKAT_DOT               0.5f // < 0 is way too big of an angle.
#endif // MAPBASE_MP

#define HL2MP_RUN_SPEED				320.0f
#define HL2MP_WALK_SPEED			75.0f
#define HL2MP_CROUCHWALK_SPEED		110.0f

extern ConVar mp_showgestureslots;
extern ConVar mp_slammoveyaw;
#ifdef GAME_DLL
	#ifdef MAPBASE_MP
		extern ConVar player_use_anim_enabled;
		extern ConVar player_use_anim_heavy_mass;
	#endif // MAPBASE_MP
#endif // GAME_DLL

ConVar mp_playeranimstate_animtype( "sv_playeranimstate_animtype", "0", FCVAR_REPLICATED | FCVAR_ARCHIVE | FCVAR_CHEAT, "The leg animation type used by the multiplayer animation state. 9way = 0, 8way = 1, GoldSrc = 2" );
//ConVar mp_playeranimstate_bodyyaw( "sv_playeranimstate_bodyyaw", "45.0", FCVAR_NONE, "The maximum body yaw used by the singleplayer animation state." );
//ConVar mp_playeranimstate_use_aim_sequences( "sv_playeranimstate_use_aim_sequences", "1", FCVAR_NONE, "Allows the singleplayer animation state to use aim sequences." );

//-----------------------------------------------------------------------------
// Purpose: The player has no guns
//-----------------------------------------------------------------------------
acttable_t	CHL2MPPlayerAnimState::UnarmedAnimsActtable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_MELEE,				false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_MELEE,				false },
#if EXPANDED_HL2DM_ACTIVITIES
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_MELEE,				false },
#endif
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_MELEE,		false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_MELEE,		false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_MELEE,				false },
};

#ifdef MAPBASE_MP
//-----------------------------------------------------------------------------
// Purpose: The player is carrying a light prop
//-----------------------------------------------------------------------------
acttable_t	CHL2MPPlayerAnimState::CarryLightAnimsActtable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_USE,					false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_USE,					false },
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_USE,					false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_USE,			false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_USE,			false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_USE,					false },
};

//-----------------------------------------------------------------------------
// Purpose: The player is carrying a heavy prop
//-----------------------------------------------------------------------------
acttable_t	CHL2MPPlayerAnimState::CarryHeavyAnimsActtable[] =
{
	{ ACT_MP_STAND_IDLE,				ACT_HL2MP_IDLE_USE_HEAVY,			false },
	{ ACT_MP_RUN,						ACT_HL2MP_RUN_USE_HEAVY,			false },
	{ ACT_MP_WALK,						ACT_HL2MP_WALK_USE_HEAVY,			false },
	{ ACT_MP_CROUCH_IDLE,				ACT_HL2MP_IDLE_CROUCH_USE_HEAVY,	false },
	{ ACT_MP_CROUCHWALK,				ACT_HL2MP_WALK_CROUCH_USE_HEAVY,	false },
	{ ACT_MP_JUMP,						ACT_HL2MP_JUMP_USE_HEAVY,			false },
};
#endif // MAPBASE_MP

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pPlayer -
// Output : CMultiPlayerAnimState*
//-----------------------------------------------------------------------------
CHL2MPPlayerAnimState* CreateHL2MPPlayerAnimState( CHL2MP_Player* pPlayer )
{
	MDLCACHE_CRITICAL_SECTION();

	// Setup the movement data.
	MultiPlayerMovementData_t movementData;
	movementData.m_flBodyYawRate = 720.0f;
	movementData.m_flRunSpeed = HL2MP_RUN_SPEED;
	movementData.m_flWalkSpeed = HL2MP_WALK_SPEED;
	movementData.m_flSprintSpeed = -1.0f;

	// Create animation state for this player.
	CHL2MPPlayerAnimState* pRet = new CHL2MPPlayerAnimState( pPlayer, movementData );

	// Specific HL2MP player initialization.
	pRet->InitHL2MPAnimState( pPlayer );

	return pRet;
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :  -
//-----------------------------------------------------------------------------
CHL2MPPlayerAnimState::CHL2MPPlayerAnimState()
{
	m_pHL2MPPlayer = NULL;

	// Don't initialize HL2MP specific variables here. Init them in InitHL2MPAnimState()
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *pPlayer -
//			&movementData -
//-----------------------------------------------------------------------------
CHL2MPPlayerAnimState::CHL2MPPlayerAnimState( CBasePlayer* pPlayer, MultiPlayerMovementData_t& movementData )
	: CMultiPlayerAnimState( pPlayer, movementData )
{
	m_pHL2MPPlayer = NULL;

	// Don't initialize HL2MP specific variables here. Init them in InitHL2MPAnimState()
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  :  -
//-----------------------------------------------------------------------------
CHL2MPPlayerAnimState::~CHL2MPPlayerAnimState()
{
}

//-----------------------------------------------------------------------------
// Purpose: Initialize HL2MP specific animation state.
// Input  : *pPlayer -
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::InitHL2MPAnimState( CHL2MP_Player* pPlayer )
{
#ifdef MAPBASE_MP
	Assert( pPlayer );
	m_pHL2MPPlayer = pPlayer;

	m_blinkTimer.Invalidate();
	m_flLastLookAtUpdate = 0.0f;

	m_flLastBodyYaw = 0.0f;
	m_flCurrentHeadYaw = 0.0f;
	m_flCurrentHeadPitch = 0.0f;

#ifdef MAPBASE_MP
	m_LegAnimType = ( LegAnimType_t )mp_playeranimstate_animtype.GetInt();
	//m_flMaxBodyYawDegrees = sv_playeranimstate_bodyyaw.GetFloat(); // This isn't used in multiplayer_animstate.cpp
	//m_bUseAimSequences = mp_playeranimstate_use_aim_sequences.GetBool(); // This isn't used in multiplayer_animstate.cpp
#endif
#else
	m_pHL2MPPlayer = pPlayer;
#endif // MAPBASE_MP
}

#ifdef MAPBASE_MP
//-----------------------------------------------------------------------------
// For animations, why would ever want to use the absolute velocity?
// You want the local velocity, the thing that's not affected by outside sources (ie. moving platforms)
// ???????
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::GetOuterAbsVelocity( Vector& vel )
{
	vel = GetHL2MPPlayer()->GetLocalVelocity();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CHL2MPPlayerAnimState::ShouldUpdateAnimState()
{
	// Don't updated the animation states if we're not even being drawed
	if( m_pHL2MPPlayer->IsEffectActive( EF_NODRAW ) )
	{
		return false;
	}

	if( m_pHL2MPPlayer->IsObserver() )
	{
		return false;
	}

	// Why would we update anims if we're not getting updates from server
#ifdef CLIENT_DLL
	if( m_pHL2MPPlayer->IsDormant() )
	{
		return false;
	}
#endif

	return m_pHL2MPPlayer->IsAlive() || m_bDying;
}
#endif // MAPBASE_MP


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::ClearAnimationState( void )
{
	BaseClass::ClearAnimationState();
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : actDesired -
// Output : Activity
//-----------------------------------------------------------------------------
Activity CHL2MPPlayerAnimState::TranslateActivity( Activity actDesired )
{
	Activity translateActivity = BaseClass::TranslateActivity( actDesired );

	if( GetHL2MPPlayer()->GetActiveWeapon() )
	{
		bool required = false;
		translateActivity = GetHL2MPPlayer()->GetActiveWeapon()->ActivityOverride( translateActivity, &required );
	}
	else if( UnarmedAnimsActtable )
	{
		acttable_t* pTable = UnarmedAnimsActtable;
		int actCount = ARRAYSIZE( UnarmedAnimsActtable );

		for( int i = 0; i < actCount; i++, pTable++ )
		{
			if( actDesired == pTable->baseAct )
			{
				translateActivity = ( Activity )pTable->weaponAct;
			}
		}
	}
#ifdef MAPBASE_MP
#ifdef GAME_DLL
	else if( GetHL2MPPlayer()->GetUseEntity() && GetHL2MPPlayer()->GetUseEntity()->ClassMatches( "player_pickup" ) && player_use_anim_enabled.GetBool() )
	{
		CBaseEntity* pHeldEnt = GetPlayerHeldEntity( GetHL2MPPlayer() );
		float flMass = pHeldEnt ?
					   ( pHeldEnt->VPhysicsGetObject() ? PlayerPickupGetHeldObjectMass( GetHL2MPPlayer()->GetUseEntity(), pHeldEnt->VPhysicsGetObject() ) : player_use_anim_heavy_mass.GetFloat() ) :
					   ( GetHL2MPPlayer()->GetUseEntity()->VPhysicsGetObject() ? GetHL2MPPlayer()->GetUseEntity()->GetMass() : player_use_anim_heavy_mass.GetFloat() );
		if( flMass >= player_use_anim_heavy_mass.GetFloat() )
		{
			// Heavy versions
			acttable_t* pTable = CarryHeavyAnimsActtable;
			int actCount = ARRAYSIZE( CarryHeavyAnimsActtable );

			for( int i = 0; i < actCount; i++, pTable++ )
			{
				if( actDesired == pTable->baseAct )
				{
					translateActivity = ( Activity )pTable->weaponAct;
				}
			}
		}
		else
		{
			acttable_t* pTable = CarryLightAnimsActtable;
			int actCount = ARRAYSIZE( CarryLightAnimsActtable );

			for( int i = 0; i < actCount; i++, pTable++ )
			{
				if( actDesired == pTable->baseAct )
				{
					translateActivity = ( Activity )pTable->weaponAct;
				}
			}
		}
	}
#endif
#endif // MAPBASE_MP

	return translateActivity;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::Update( float eyeYaw, float eyePitch )
{
	// Profile the animation update.
	VPROF( "CHL2MPPlayerAnimState::Update" );

#ifndef MAPBASE_MP
	// Get the HL2MP player.
	CHL2MP_Player* pHL2MPPlayer = GetHL2MPPlayer();
	if( !pHL2MPPlayer )
	{
		return;
	}
#endif // MAPBASE_MP

	// Get the studio header for the player.
#ifdef MAPBASE_MP
	CStudioHdr* pStudioHdr = GetHL2MPPlayer()->GetModelPtr();
#else
	CStudioHdr* pStudioHdr = pHL2MPPlayer->GetModelPtr();
#endif
	if( !pStudioHdr )
	{
		return;
	}

#ifdef MAPBASE_MP
	// We're not updating if we don't need to.
#ifdef CLIENT_DLL
	// This will screw up localplayer's ragdoll.
	//if ( pHL2MPPlayer->IsLocalPlayer() && !C_BasePlayer::ShouldDrawLocalPlayer() )
	//    return;
#endif
#endif

	// Check to see if we should be updating the animation state - dead, ragdolled?
	if( !ShouldUpdateAnimState() )
	{
		ClearAnimationState();
		return;
	}

	// Store the eye angles.
	m_flEyeYaw = AngleNormalize( eyeYaw );
	m_flEyePitch = AngleNormalize( eyePitch );

	// Compute the player sequences.
	ComputeSequences( pStudioHdr );

	if( SetupPoseParameters( pStudioHdr ) )
	{
		// Pose parameter - what direction are the player's legs running in.
		ComputePoseParam_MoveYaw( pStudioHdr );

		// Pose parameter - Torso aiming (up/down).
		ComputePoseParam_AimPitch( pStudioHdr );

		// Pose parameter - Torso aiming (rotation).
		ComputePoseParam_AimYaw( pStudioHdr );

#ifdef MAPBASE_MP
		// Pose parameter - Head + eyes
		// NOTE: If we ever decide to care/care less about lag compensation on players, we can just make these shared/all on client.
		// IIRC pose parameters aren't lag compensated anyway.
#ifdef CLIENT_DLL
		ComputePoseParam_Head( pStudioHdr );
#endif
#endif
	}

#ifdef CLIENT_DLL
	if( C_BasePlayer::ShouldDrawLocalPlayer() )
	{
		m_pHL2MPPlayer->SetPlaybackRate( 1.0f );
	}
#endif

	if( mp_showgestureslots.GetInt() == GetBasePlayer()->entindex() )
	{
		DebugGestureInfo();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : event -
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::DoAnimationEvent( PlayerAnimEvent_t event, int nData )
{
	Activity iGestureActivity = ACT_INVALID;

	switch( event )
	{
		case PLAYERANIMEVENT_ATTACK_PRIMARY:
		{
			// Weapon primary fire.
			if( m_pHL2MPPlayer->GetFlags() & FL_DUCKING )
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_ATTACK_CROUCH_PRIMARYFIRE );
			}
			else
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_ATTACK_STAND_PRIMARYFIRE );
			}

			iGestureActivity = ACT_VM_PRIMARYATTACK;
			break;
		}

		case PLAYERANIMEVENT_VOICE_COMMAND_GESTURE:
		{
			if( !IsGestureSlotActive( GESTURE_SLOT_ATTACK_AND_RELOAD ) )
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ( Activity )nData );
			}

			break;
		}
		case PLAYERANIMEVENT_ATTACK_SECONDARY:
		{
			// Weapon secondary fire.
			if( m_pHL2MPPlayer->GetFlags() & FL_DUCKING )
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_ATTACK_CROUCH_SECONDARYFIRE );
			}
			else
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_ATTACK_STAND_SECONDARYFIRE );
			}

			iGestureActivity = ACT_VM_PRIMARYATTACK;
			break;
		}
		case PLAYERANIMEVENT_ATTACK_PRE:
		{
			if( m_pHL2MPPlayer->GetFlags() & FL_DUCKING )
			{
				// Weapon pre-fire. Used for minigun windup, sniper aiming start, etc in crouch.
				iGestureActivity = ACT_MP_ATTACK_CROUCH_PREFIRE;
			}
			else
			{
				// Weapon pre-fire. Used for minigun windup, sniper aiming start, etc.
				iGestureActivity = ACT_MP_ATTACK_STAND_PREFIRE;
			}

			RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, iGestureActivity, false );

			break;
		}
		case PLAYERANIMEVENT_ATTACK_POST:
		{
			RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_ATTACK_STAND_POSTFIRE );
			break;
		}

		case PLAYERANIMEVENT_RELOAD:
		{
			// Weapon reload.
#ifdef MAPBASE_MP
			if( GetHL2MPPlayer()->GetFlags() & FL_DUCKING )
#else
			if( GetBasePlayer()->GetFlags() & FL_DUCKING )
#endif
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_CROUCH );
			else
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_STAND );
			}
			break;
		}
		case PLAYERANIMEVENT_RELOAD_LOOP:
		{
			// Weapon reload.
#ifdef MAPBASE_MP
			if( GetHL2MPPlayer()->GetFlags() & FL_DUCKING )
#else
			if( GetBasePlayer()->GetFlags() & FL_DUCKING )
#endif
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_CROUCH_LOOP );
			else
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_STAND_LOOP );
			}
			break;
		}
		case PLAYERANIMEVENT_RELOAD_END:
		{
			// Weapon reload.
#ifdef MAPBASE_MP
			if( GetHL2MPPlayer()->GetFlags() & FL_DUCKING )
#else
			if( GetBasePlayer()->GetFlags() & FL_DUCKING )
#endif
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_CROUCH_END );
			else
			{
				RestartGesture( GESTURE_SLOT_ATTACK_AND_RELOAD, ACT_MP_RELOAD_STAND_END );
			}
			break;
		}
		default:
		{
			BaseClass::DoAnimationEvent( event, nData );
			break;
		}
	}

	// This only gets in the way. We send weapon animations ourselves.
#ifndef MAPBASE_MP
#ifdef CLIENT_DLL
	// Make the weapon play the animation as well
	if( iGestureActivity != ACT_INVALID )
	{
		CBaseCombatWeapon* pWeapon = GetHL2MPPlayer()->GetActiveWeapon();
		if( pWeapon )
		{
			pWeapon->EnsureCorrectRenderingModel();
			pWeapon->SendWeaponAnim( iGestureActivity );
			// Force animation events!
			pWeapon->ResetEventsParity();		// reset event parity so the animation events will occur on the weapon.
			pWeapon->DoAnimationEvents( pWeapon->GetModelPtr() );
		}
	}
#endif
#endif // MAPBASE_MP
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
//-----------------------------------------------------------------------------
bool CHL2MPPlayerAnimState::HandleSwimming( Activity& idealActivity )
{
#ifdef MAPBASE_MP
	// We don't have any swimming animations.
	// TODO: this is true for ZM:R but we may want to add animations in the future
	//		 revisit this later!
	if( m_pHL2MPPlayer->GetWaterLevel() >= WL_Waist )
	{
		idealActivity = ACT_MP_JUMP;
		return true;
	}

	return false;
#else
	bool bInWater = BaseClass::HandleSwimming( idealActivity );
	return bInWater;
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHL2MPPlayerAnimState::HandleMoving( Activity& idealActivity )
{
#ifdef MAPBASE_MP
	if( GetOuterXYSpeed() > MOVING_MINIMUM_SPEED )
	{
#if EXPANDED_HL2DM_ACTIVITIES
		if( m_pPlayer->GetButtons() & IN_WALK )
		{
			idealActivity = ACT_MP_WALK;
		}
		else
		{
			idealActivity = ACT_MP_RUN;
		}
#else
		idealActivity = ACT_MP_RUN;
#endif
	}
#if EXPANDED_HL2DM_ACTIVITIES
	else
	{
		idealActivity = ACT_MP_STAND_IDLE;
	}
#endif

	return true;
#else
	return BaseClass::HandleMoving( idealActivity );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHL2MPPlayerAnimState::HandleDucking( Activity& idealActivity )
{
#ifdef MAPBASE_MP
	if( m_bInSwim && ( ( m_pHL2MPPlayer->GetFlags() & FL_ONGROUND ) == 0 ) )
	{
		return false;
	}
#endif

#ifndef MAPBASE_MP
	if( m_pHL2MPPlayer->GetFlags() & FL_DUCKING )
	{
		if( GetOuterXYSpeed() < MOVING_MINIMUM_SPEED )
		{
			idealActivity = ACT_MP_CROUCH_IDLE;
		}
		else
		{
			idealActivity = ACT_MP_CROUCHWALK;
		}

		return true;
	}
#else
	return BaseClass::HandleDucking( idealActivity );
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
// Input  : *idealActivity -
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHL2MPPlayerAnimState::HandleJumping( Activity& idealActivity )
{
#ifdef MAPBASE_MP
	if( m_pHL2MPPlayer->GetMoveType() == MOVETYPE_NOCLIP )
	{
		m_bJumping = false;
		return false;
	}

	// airwalk more like hl2mp, we airwalk until we have 0 velocity, then it's the jump animation
	// underwater we're alright we airwalking
	if( !m_bJumping && !( m_pHL2MPPlayer->GetFlags() & FL_ONGROUND ) && m_pHL2MPPlayer->GetWaterLevel() <= WL_NotInWater )
	{
		if( !m_fGroundTime )
		{
			m_fGroundTime = gpGlobals->curtime;
		}
		else if( ( gpGlobals->curtime - m_fGroundTime ) > 0 && GetOuterXYSpeed() < MOVING_MINIMUM_SPEED )
		{
			m_bJumping = true;
			m_bFirstJumpFrame = false;
			m_flJumpStartTime = 0;
		}
	}
#endif // MAPBASE_MP

	if( m_bJumping )
	{
		static bool bNewJump = false; //Tony; hl2mp players only have a 'hop'

		if( m_bFirstJumpFrame )
		{
			m_bFirstJumpFrame = false;
			RestartMainSequence();	// Reset the animation.
		}

		// Check to see if we hit water and stop jumping animation.
		if( m_pHL2MPPlayer->GetWaterLevel() >= WL_Waist )
		{
			m_bJumping = false;
			RestartMainSequence();
		}
		// Don't check if he's on the ground for a sec.. sometimes the client still has the
		// on-ground flag set right when the message comes in.
		else if( gpGlobals->curtime - m_flJumpStartTime > 0.2f )
		{
			if( m_pHL2MPPlayer->GetFlags() & FL_ONGROUND )
			{
				m_bJumping = false;
				RestartMainSequence();

				if( bNewJump )
				{
					RestartGesture( GESTURE_SLOT_JUMP, ACT_MP_JUMP_LAND );
				}
			}
		}

		// if we're still jumping
		if( m_bJumping )
		{
			if( bNewJump )
			{
				if( ( gpGlobals->curtime - m_flJumpStartTime ) > 0.5f )
				{
					idealActivity = ACT_MP_JUMP_FLOAT;
				}
				else
				{
					idealActivity = ACT_MP_JUMP_START;
				}
			}
			else
			{
				idealActivity = ACT_MP_JUMP;
			}
		}
	}

	if( m_bJumping )
	{
		return true;
	}

	return false;
}

bool CHL2MPPlayerAnimState::SetupPoseParameters( CStudioHdr* pStudioHdr )
{
	// Check to see if this has already been done.
	if( m_bPoseParameterInit )
	{
		return true;
	}

	// Save off the pose parameter indices.
	if( !pStudioHdr )
	{
		return false;
	}

#ifdef MAPBASE_MP
	//m_bIs9Way = false;
	m_bPoseParameterInit = true;

	MDLCACHE_CRITICAL_SECTION();

	for( int i = 0; i < pStudioHdr->GetNumPoseParameters(); i++ )
	{
		m_pHL2MPPlayer->SetPoseParameter( i, 0.0 );
	}

#ifdef CLIENT_DLL
	m_headYawPoseParam = m_pHL2MPPlayer->LookupPoseParameter( "head_yaw" );
	if( m_headYawPoseParam < 0 )
	{
		return false;
	}

	m_pHL2MPPlayer->GetPoseParameterRange( m_headYawPoseParam, m_headYawMin, m_headYawMax );

	m_headPitchPoseParam = m_pHL2MPPlayer->LookupPoseParameter( "head_pitch" );
	if( m_headPitchPoseParam < 0 )
	{
		return false;
	}

	m_pHL2MPPlayer->GetPoseParameterRange( m_headPitchPoseParam, m_headPitchMin, m_headPitchMax );
#endif

	//
	// 9way blending
	//
	if( ModelUses9WaysAnimation() )
	{
		// Look for the movement blenders.
		m_PoseParameterData.m_iMoveX = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "move_x" );
		m_PoseParameterData.m_iMoveY = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "move_y" );
		if( ( m_PoseParameterData.m_iMoveX < 0 ) || ( m_PoseParameterData.m_iMoveY < 0 ) )
		{
			return false;
		}

		// Look for the aim pitch blender.
		m_PoseParameterData.m_iAimPitch = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "body_pitch" );
		if( m_PoseParameterData.m_iAimPitch < 0 )
		{
			return false;
		}

		// Look for aim yaw blender.
		m_PoseParameterData.m_iAimYaw = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "body_yaw" );
		if( m_PoseParameterData.m_iAimYaw < 0 )
		{
			return false;
		}

		m_PoseParameterData.m_iMoveYaw = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "move_yaw" );
		//m_PoseParameterData.m_iMoveScale = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "move_scale" );
		if( ( m_PoseParameterData.m_iMoveYaw < 0 ) /* || (m_PoseParameterData.m_iMoveScale < 0)*/ )
		{
			return false;
		}
	}
	//
	// Fallback to 8way blending
	//
	else
	{
		// 8way blending doesn't use move_x/move_y
		// and it only has one "move_yaw" param.
		m_PoseParameterData.m_iMoveX =
			m_PoseParameterData.m_iMoveY = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "move_yaw" );
		if( ( m_PoseParameterData.m_iMoveX < 0 ) || ( m_PoseParameterData.m_iMoveY < 0 ) )
		{
			return false;
		}

		// Look for the aim pitch blender.
		m_PoseParameterData.m_iAimPitch = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "aim_pitch" );
		if( m_PoseParameterData.m_iAimPitch < 0 )
		{
			return false;
		}

		// Look for aim yaw blender.
		m_PoseParameterData.m_iAimYaw = m_pHL2MPPlayer->LookupPoseParameter( pStudioHdr, "aim_yaw" );
		if( m_PoseParameterData.m_iAimYaw < 0 )
		{
			return false;
		}
	}

	//m_bPoseParameterInit = true;

	return true;
#else
	// Tony; just set them both to the same for now.
	m_PoseParameterData.m_iMoveX = GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_yaw" );
	m_PoseParameterData.m_iMoveY = GetBasePlayer()->LookupPoseParameter( pStudioHdr, "move_yaw" );
	if( ( m_PoseParameterData.m_iMoveX < 0 ) || ( m_PoseParameterData.m_iMoveY < 0 ) )
	{
		return false;
	}

	// Look for the aim pitch blender.
	m_PoseParameterData.m_iAimPitch = GetBasePlayer()->LookupPoseParameter( pStudioHdr, "aim_pitch" );
	if( m_PoseParameterData.m_iAimPitch < 0 )
	{
		return false;
	}

	// Look for aim yaw blender.
	m_PoseParameterData.m_iAimYaw = GetBasePlayer()->LookupPoseParameter( pStudioHdr, "aim_yaw" );
	if( m_PoseParameterData.m_iAimYaw < 0 )
	{
		return false;
	}
#endif

	//m_bPoseParameterInit = true;

	return true;
}

float SnapYawTo( float flValue );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::EstimateYaw( void )
{
	// Get the frame time.
	float flDeltaTime = gpGlobals->frametime;
	if( flDeltaTime == 0.0f )
	{
		return;
	}

#ifdef MAPBASE_MP
	//
	// 9way blending
	//
	if( ModelUses9WaysAnimation() )
	{
		// Get the player's velocity and angles.
		Vector vecEstVelocity;
		GetOuterAbsVelocity( vecEstVelocity );
		QAngle angles = m_pHL2MPPlayer->GetLocalAngles();

		// If we are not moving, sync up the feet and eyes slowly.
		if( vecEstVelocity.x == 0.0f && vecEstVelocity.y == 0.0f )
		{
			float flYawDelta = angles[YAW] - m_PoseParameterData.m_flEstimateYaw;
			flYawDelta = AngleNormalize( flYawDelta );

			if( flDeltaTime < 0.25f )
			{
				flYawDelta *= ( flDeltaTime * 4.0f );
			}
			else
			{
				flYawDelta *= flDeltaTime;
			}

			m_PoseParameterData.m_flEstimateYaw += flYawDelta;
			AngleNormalize( m_PoseParameterData.m_flEstimateYaw );
		}
		else
		{
			m_PoseParameterData.m_flEstimateYaw = ( atan2( vecEstVelocity.y, vecEstVelocity.x ) * 180.0f / M_PI );
			m_PoseParameterData.m_flEstimateYaw = clamp( m_PoseParameterData.m_flEstimateYaw, -180.0f, 180.0f );
		}
	}
	else
	{
#endif
#ifndef MAPBASE_MP
		float dt = gpGlobals->frametime;
#endif

		// Get the player's velocity and angles.
		Vector vecEstVelocity;
		GetOuterAbsVelocity( vecEstVelocity );
#ifdef MAPBASE_MP
		// Use eye yaw.
		//QAngle angles = GetBasePlayer()->GetLocalAngles();
		QAngle angles = QAngle( 0.0f, m_flEyeYaw, 0.0f );
#else
		QAngle angles = GetBasePlayer()->GetLocalAngles();
#endif
		if( vecEstVelocity.y == 0 && vecEstVelocity.x == 0 )
		{
			float flYawDiff = angles[YAW] - m_PoseParameterData.m_flEstimateYaw;
			flYawDiff = flYawDiff - ( int )( flYawDiff / 360 ) * 360;
			if( flYawDiff > 180 )
			{
				flYawDiff -= 360;
			}
			if( flYawDiff < -180 )
			{
				flYawDiff += 360;
			}

#ifndef MAPBASE_MP
			if( dt < 0.25 )
			{
				flYawDiff *= dt * 4;
			}
			else
			{
				flYawDiff *= dt;
			}
#else
			if( flDeltaTime < 0.25 )
			{
				flYawDiff *= flDeltaTime * 4;
			}
			else
			{
				flYawDiff *= flDeltaTime;
			}
#endif


			m_PoseParameterData.m_flEstimateYaw += flYawDiff;
			m_PoseParameterData.m_flEstimateYaw = m_PoseParameterData.m_flEstimateYaw - ( int )( m_PoseParameterData.m_flEstimateYaw / 360 ) * 360;
		}
		else
		{
			m_PoseParameterData.m_flEstimateYaw = ( atan2( vecEstVelocity.y, vecEstVelocity.x ) * 180 / M_PI );

			if( m_PoseParameterData.m_flEstimateYaw > 180 )
			{
				m_PoseParameterData.m_flEstimateYaw = 180;
			}
			else if( m_PoseParameterData.m_flEstimateYaw < -180 )
			{
				m_PoseParameterData.m_flEstimateYaw = -180;
			}
		}
	}
}
//-----------------------------------------------------------------------------
// Purpose: Override for backpeddling
// Input  : dt -
//-----------------------------------------------------------------------------
float SnapYawTo( float flValue );
void CHL2MPPlayerAnimState::ComputePoseParam_MoveYaw( CStudioHdr* pStudioHdr )
{
	// Get the estimated movement yaw.
	EstimateYaw();

#ifdef MAPBASE
	//
	// 9way blending
	//
	if( ModelUses9WaysAnimation() )
	{
		// Get the view yaw.
		float flAngle = AngleNormalize( m_flEyeYaw );

		// Calc side to side turning - the view vs. movement yaw.
		float flYaw = flAngle - m_PoseParameterData.m_flEstimateYaw;
		flYaw = AngleNormalize( -flYaw );

		// Get the current speed the character is running.
		bool bIsMoving;
		float flSpeed = CalcMovementSpeed( &bIsMoving );

		// Setup the 9-way blend parameters based on our speed and direction.
		Vector2D vecCurrentMoveYaw( 0.0f, 0.0f );
		if( bIsMoving )
		{
			GetMovementFlags( pStudioHdr );

			if( mp_slammoveyaw.GetBool() )
			{
				flYaw = SnapYawTo( flYaw );
			}

			if( m_LegAnimType == LEGANIM_9WAY )
			{
				// convert YAW back into vector
				vecCurrentMoveYaw.x = cos( DEG2RAD( flYaw ) );
				vecCurrentMoveYaw.y = -sin( DEG2RAD( flYaw ) );
				// push edges out to -1 to 1 box
				float flInvScale = MAX( fabs( vecCurrentMoveYaw.x ), fabs( vecCurrentMoveYaw.y ) );
				if( flInvScale != 0.0f )
				{
					vecCurrentMoveYaw.x /= flInvScale;
					vecCurrentMoveYaw.y /= flInvScale;
				}

				// find what speed was actually authored
				m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveX, vecCurrentMoveYaw.x );
				m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, vecCurrentMoveYaw.y );
				float flMaxSpeed = m_pHL2MPPlayer->GetSequenceGroundSpeed( GetBasePlayer()->GetSequence() );

				// scale playback
				if( flMaxSpeed > flSpeed )
				{
					vecCurrentMoveYaw.x *= flSpeed / flMaxSpeed;
					vecCurrentMoveYaw.y *= flSpeed / flMaxSpeed;
				}

				// Set the 9-way blend movement pose parameters.
				m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveX, vecCurrentMoveYaw.x );
				m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, vecCurrentMoveYaw.y );
			}
			else
			{
				// find what speed was actually authored
				m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveYaw, flYaw );
				//m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveScale, 1.0f ); // Mapbase Model doesn't have "move_scale"
				//float flMaxSpeed = m_pHL2MPPlayer->GetSequenceGroundSpeed( m_pHL2MPPlayer->GetSequence() );

				// scale playback
				/*if (flMaxSpeed > flSpeed)
				{
					m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveScale, flSpeed / flMaxSpeed );
				}*/
			}
		}
		else
		{
			// Set the 9-way blend movement pose parameters.
			m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveX, 0.0f );
			m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, 0.0f );
		}
	}
	else
#endif
	{
		// view direction relative to movement
		float flYaw;
#ifdef MAPBASE_MP
		// This fucked feet movement.
		// I spent way too much time (AGAIN) debugging this shit.
		// The above code has it right. Why the fuck was it changed?
		// AFAIK, it wouldn't only affect us. Really weird that it wasn't caught. It probably was but I copied an older version. Oh well...
		//QAngle	angles = m_pHL2MPPlayer->GetAbsAngles();
		//float ang = angles[ YAW ];
		float ang = m_flEyeYaw;
#else
		QAngle	angles = GetBasePlayer()->GetLocalAngles();
		float ang = angles[ YAW ];
#endif // MAPBASE_MP
		if( ang > 180.0f )
		{
			ang -= 360.0f;
		}
		else if( ang < -180.0f )
		{
			ang += 360.0f;
		}

		// calc side to side turning
		flYaw = ang - m_PoseParameterData.m_flEstimateYaw;
		// Invert for mapping into 8way blend
		flYaw = -flYaw;
		flYaw = flYaw - ( int )( flYaw / 360 ) * 360;

		if( flYaw < -180 )
		{
			flYaw = flYaw + 360;
		}
		else if( flYaw > 180 )
		{
			flYaw = flYaw - 360;
		}

		//Tony; oops, i inverted this previously above.
#ifdef MAPBASE_MP
		m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, flYaw );
#else
		GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, flYaw );
#endif // MAPBASE_MP
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::ComputePoseParam_AimPitch( CStudioHdr* pStudioHdr )
{
	// Get the view pitch.
	float flAimPitch = m_flEyePitch;

	// Set the aim pitch pose parameter and save.
	m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iAimPitch, -flAimPitch );
	m_DebugAnimData.m_flAimPitch = flAimPitch;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::ComputePoseParam_AimYaw( CStudioHdr* pStudioHdr )
{
	// Get the movement velocity.
	Vector vecVelocity;
	GetOuterAbsVelocity( vecVelocity );

	// Check to see if we are moving.
	bool bMoving = ( vecVelocity.Length() > 1.0f ) ? true : false;

	// If we are moving or are prone and undeployed.
	// If you are forcing aim yaw, your code is almost definitely broken if you don't include a delay between
	// teleporting and forcing yaw. This is due to an unfortunate interaction between the command lookback window,
	// and the fact that m_flEyeYaw is never propogated from the server to the client.
	// TODO: Fix this after Halloween 2014.
	if( bMoving || m_bForceAimYaw )
	{
		// The feet match the eye direction when moving - the move yaw takes care of the rest.
		m_flGoalFeetYaw = m_flEyeYaw;
	}
	// Else if we are not moving.
	else
	{
		// Initialize the feet.
		if( m_PoseParameterData.m_flLastAimTurnTime <= 0.0f )
		{
			m_flGoalFeetYaw	= m_flEyeYaw;
			m_flCurrentFeetYaw = m_flEyeYaw;
			m_PoseParameterData.m_flLastAimTurnTime = gpGlobals->curtime;
		}
		// Make sure the feet yaw isn't too far out of sync with the eye yaw.
		// TODO: Do something better here!
		else
		{
			float flYawDelta = AngleNormalize( m_flGoalFeetYaw - m_flEyeYaw );

			if( fabs( flYawDelta ) > 45.0f/*m_AnimConfig.m_flMaxBodyYawDegrees*/ )
			{
				float flSide = ( flYawDelta > 0.0f ) ? -1.0f : 1.0f;
				m_flGoalFeetYaw += ( 45.0f/*m_AnimConfig.m_flMaxBodyYawDegrees*/ * flSide );
			}
		}
	}

	// Fix up the feet yaw.
	m_flGoalFeetYaw = AngleNormalize( m_flGoalFeetYaw );
	if( m_flGoalFeetYaw != m_flCurrentFeetYaw )
	{
		// If you are forcing aim yaw, your code is almost definitely broken if you don't include a delay between
		// teleporting and forcing yaw. This is due to an unfortunate interaction between the command lookback window,
		// and the fact that m_flEyeYaw is never propogated from the server to the client.
		// TODO: Fix this after Halloween 2014.
		if( m_bForceAimYaw )
		{
			m_flCurrentFeetYaw = m_flGoalFeetYaw;
		}
		else
		{
			ConvergeYawAngles( m_flGoalFeetYaw, /*DOD_BODYYAW_RATE*/720.0f, gpGlobals->frametime, m_flCurrentFeetYaw );
			m_flLastAimTurnTime = gpGlobals->curtime;
		}
	}

	// Rotate the body into position.
	m_angRender[YAW] = m_flCurrentFeetYaw;

	// Find the aim(torso) yaw base on the eye and feet yaws.
	float flAimYaw = m_flEyeYaw - m_flCurrentFeetYaw;
	flAimYaw = AngleNormalize( flAimYaw );

	// Set the aim yaw and save.
#ifdef MAPBASE_MP
	m_pHL2MPPlayer->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iAimYaw, -flAimYaw );
#else
	GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iAimYaw, -flAimYaw );
#endif
	m_DebugAnimData.m_flAimYaw	= flAimYaw;

	// Turn off a force aim yaw - either we have already updated or we don't need to.
	m_bForceAimYaw = false;

#ifndef CLIENT_DLL
#ifdef MAPBASE_MP
	QAngle angle = m_pHL2MPPlayer->GetAbsAngles();
#else
	QAngle angle = GetBasePlayer()->GetAbsAngles();
#endif
	angle[YAW] = m_flCurrentFeetYaw;
#ifdef MAPBASE_MP
	m_pHL2MPPlayer->SetAbsAngles( angle );
#else
	GetBasePlayer()->SetAbsAngles( angle );
#endif
#endif
}

#ifdef MAPBASE_MP
#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::UpdateLookAt()
{
	if( ( gpGlobals->curtime - m_flLastLookAtUpdate ) < HL2MP_LOOKAT_UPDATE_TIME )
	{
		return;
	}

	if( !m_pHL2MPPlayer->IsAlive() )
	{
		return;
	}


	// Update player model's look at target.
	bool bFoundViewTarget = false;

	Vector vForward;
	AngleVectors( m_pHL2MPPlayer->EyeAngles(), &vForward );

	Vector vMyOrigin = m_pHL2MPPlayer->GetAbsOrigin();

	for( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		C_HL2MP_Player* pPlayer = ToHL2MPPlayer( UTIL_PlayerByIndex( i ) );
		if( !pPlayer )
		{
			continue;
		}

		// Don't look at dead players :(
		if( !pPlayer->IsAlive() )
		{
			continue;
		}

		if( pPlayer->IsObserver() )
		{
			continue;
		}

		if( pPlayer->IsEffectActive( EF_NODRAW ) )
		{
			continue;
		}

		if( pPlayer->entindex() == m_pHL2MPPlayer->entindex() )
		{
			continue;
		}


		Vector vTargetOrigin = pPlayer->GetAbsOrigin();

		Vector vDir = vTargetOrigin - vMyOrigin;

		if( vDir.LengthSqr() > ( HL2MP_LOOKAT_DIST * HL2MP_LOOKAT_DIST ) )
		{
			continue;
		}

		VectorNormalize( vDir );

		if( DotProduct( vForward, vDir ) < HL2MP_LOOKAT_DOT )
		{
			continue;
		}

		m_vLookAtTarget = pPlayer->EyePosition();
		bFoundViewTarget = true;
		break;
	}

	if( !bFoundViewTarget )
	{
		m_vLookAtTarget = m_pHL2MPPlayer->EyePosition() + vForward * 512.0f;
	}


	m_flLastLookAtUpdate = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CHL2MPPlayerAnimState::ComputePoseParam_Head( CStudioHdr* hdr )
{
	UpdateLookAt();


	// orient eyes
	m_pHL2MPPlayer->SetLookat( m_vLookAtTarget );

	// blinking
	if( m_blinkTimer.IsElapsed() )
	{
		m_pHL2MPPlayer->BlinkEyes();
		m_blinkTimer.Start( RandomFloat( 1.5f, 4.0f ) );
	}

	// Figure out where we want to look in world space.
	QAngle desiredAngles;
	Vector to = m_vLookAtTarget - m_pHL2MPPlayer->EyePosition();
	VectorAngles( to, desiredAngles );

	// Figure out where our body is facing in world space.
	QAngle bodyAngles( 0, 0, 0 );
	bodyAngles[YAW] = m_flGoalFeetYaw; // Just use our feet goal yaw.


	float flBodyYawDiff = bodyAngles[YAW] - m_flLastBodyYaw;
	m_flLastBodyYaw = bodyAngles[YAW];


	// Set the head's yaw.
	// Take into account aim yaw.
	float desired = AngleNormalize( desiredAngles[YAW] - bodyAngles[YAW] - m_flCurrentAimYaw );
	desired = clamp( desired, m_headYawMin, m_headYawMax );
	m_flCurrentHeadYaw = ApproachAngle( desired, m_flCurrentHeadYaw, 130 * gpGlobals->frametime );

	// Counterrotate the head from the body rotation so it doesn't rotate past its target.
	m_flCurrentHeadYaw = AngleNormalize( m_flCurrentHeadYaw - flBodyYawDiff );
	desired = clamp( desired, m_headYawMin, m_headYawMax );

	m_pHL2MPPlayer->SetPoseParameter( m_headYawPoseParam, m_flCurrentHeadYaw );


	// Set the head's pitch.
	desired = AngleNormalize( desiredAngles[PITCH] );
	desired = clamp( desired, m_headPitchMin, m_headPitchMax );

	m_flCurrentHeadPitch = ApproachAngle( desired, m_flCurrentHeadPitch, 130 * gpGlobals->frametime );
	m_flCurrentHeadPitch = AngleNormalize( m_flCurrentHeadPitch );
	m_pHL2MPPlayer->SetPoseParameter( m_headPitchPoseParam, m_flCurrentHeadPitch );
}
#endif

#endif // MAPBASE_MP

//-----------------------------------------------------------------------------
// Purpose: Override the default, because hl2mp models don't use moveX
// Input  :  -
// Output : float
//-----------------------------------------------------------------------------
float CHL2MPPlayerAnimState::GetCurrentMaxGroundSpeed()
{
#ifdef MAPBASE_MP
	return BaseClass::GetCurrentMaxGroundSpeed();
#else
	CStudioHdr* pStudioHdr = GetBasePlayer()->GetModelPtr();

	if( pStudioHdr == NULL )
	{
		return 1.0f;
	}

//	float prevX = GetBasePlayer()->GetPoseParameter( m_PoseParameterData.m_iMoveX );
	float prevY = GetBasePlayer()->GetPoseParameter( m_PoseParameterData.m_iMoveY );

	float d = sqrt( /*prevX * prevX + */prevY * prevY );
	float newY;//, newX;
	if( d == 0.0 )
	{
//		newX = 1.0;
		newY = 0.0;
	}
	else
	{
//		newX = prevX / d;
		newY = prevY / d;
	}

//	GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveX, newX );
	GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, newY );

	float speed = GetBasePlayer()->GetSequenceGroundSpeed( GetBasePlayer()->GetSequence() );

//	GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveX, prevX );
	GetBasePlayer()->SetPoseParameter( pStudioHdr, m_PoseParameterData.m_iMoveY, prevY );

	return speed;
#endif
}