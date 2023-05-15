//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//
//=============================================================================//
#ifndef HL2MP_PLAYER_H
#define HL2MP_PLAYER_H
#pragma once

class CHL2MP_Player;

#include "basemultiplayerplayer.h"
#include "hl2_playerlocaldata.h"
#include "hl2_player.h"
#include "simtimer.h"
#include "soundenvelope.h"
#include "hl2mp_playeranimstate.h"
#include "hl2mp_player_shared.h"
#include "hl2mp_gamerules.h"
#include "utldict.h"

//=============================================================================
// Function table for each player state.
//=============================================================================
class CHL2MPPlayerStateInfo
{
public:
	HL2MPPlayerState m_iPlayerState;
	const char* m_pStateName;

	void ( CHL2MP_Player::*pfnEnterState )();	// Init and deinit the state.
	void ( CHL2MP_Player::*pfnLeaveState )();

	void ( CHL2MP_Player::*pfnPreThink )();	// Do a PreThink() in this state.
};

class CHL2MP_Player : public CHL2_Player
{
public:
	DECLARE_CLASS( CHL2MP_Player, CHL2_Player );

	CHL2MP_Player();
	~CHL2MP_Player( void );

	static CHL2MP_Player* CreatePlayer( const char* className, edict_t* ed )
	{
		CHL2MP_Player::s_PlayerEdict = ed;
		return ( CHL2MP_Player* )CreateEntityByName( className );
	}

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();
	DECLARE_PREDICTABLE();

	void			SetupBones( matrix3x4_t* pBoneToWorld, int boneMask );

	virtual void Precache( void );
	virtual void InitialSpawn( void );
	virtual void Spawn( void );
	virtual void PostThink( void );
	virtual void PreThink( void );
	virtual void PlayerDeathThink( void );
	virtual bool HandleCommand_JoinTeam( int team );
	virtual bool ClientCommand( const CCommand& args );
	virtual void CreateViewModel( int viewmodelindex = 0 );
	virtual bool BecomeRagdollOnClient( const Vector& force );
	virtual void Event_Killed( const CTakeDamageInfo& info );
	virtual int OnTakeDamage( const CTakeDamageInfo& inputInfo );
	virtual bool WantsLagCompensationOnEntity( const CBasePlayer* pPlayer, const CUserCmd* pCmd, const CBitVec<MAX_EDICTS>* pEntityTransmitBits ) const;
	virtual void ChangeTeam( int iTeam );

	virtual void Weapon_Drop( CBaseCombatWeapon* pWeapon, const Vector* pvecTarget = NULL, const Vector* pVelocity = NULL );
	virtual void UpdateOnRemove( void );
	virtual void DeathSound( const CTakeDamageInfo& info );
	virtual CBaseEntity* EntSelectSpawnPoint( void );

	bool	ValidatePlayerModel( const char* pModel );

	void CheatImpulseCommands( int iImpulse );
	void CreateRagdollEntity( void );
	virtual void RemoveAllItems( bool removeSuit );
	void GiveAllItems( void );
	void GiveDefaultItems( void );

	void NoteWeaponFired( void );

	void SetAnimation( PLAYER_ANIM playerAnim );

	void SetPlayerModel( void );
	void SetPlayerTeamModel( void );

	float GetNextModelChangeTime( void )
	{
		return m_flNextModelChangeTime;
	}
	float GetNextTeamChangeTime( void )
	{
		return m_flNextTeamChangeTime;
	}
	void  PickDefaultSpawnTeam( void );
	void  SetupPlayerSoundsByModel( const char* pModelName );
	const char* GetPlayerModelSoundPrefix( void );
	int	  GetPlayerModelType( void )
	{
		return m_iPlayerSoundType;
	}

	void  DetonateTripmines( void );

	void State_Transition( HL2MPPlayerState newState );
	void State_Enter( HL2MPPlayerState newState );
	void State_Leave();
	void State_PreThink();
	CHL2MPPlayerStateInfo* State_LookupInfo( HL2MPPlayerState state );

	void State_Enter_ACTIVE();
	void State_PreThink_ACTIVE();
	void State_Enter_OBSERVER_MODE();
	void State_PreThink_OBSERVER_MODE();


	virtual bool StartObserverMode( int mode );
	virtual void StopObserverMode( void );


	Vector m_vecTotalBulletForce;	//Accumulator for bullet force in a single frame

	// Tracks our ragdoll entity.
	CNetworkHandle( CBaseEntity, m_hRagdoll );	// networked entity handle

	// Player avoidance

	void HL2MPPushawayThink( void );

// Shared code
public:

	// This passes the event to the client's and server's CHL2MPPlayerAnimState.
	virtual void DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	//Tony; when model is changed, need to init some stuff.
	virtual CStudioHdr* OnNewModel( void );

	virtual void TraceAttack( const CTakeDamageInfo& info, const Vector& vecDir, trace_t* ptr, CDmgAccumulator* pAccumulator );

	virtual void CalcView( Vector& eyeOrigin, QAngle& eyeAngles, float& zNear, float& zFar, float& fov );

	virtual void DoImpactEffect( trace_t& tr, int nDamageType );

	virtual void FireBullets( const FireBulletsInfo_t& info );

	Vector GetAttackSpread( CBaseCombatWeapon* pWeapon, CBaseEntity* pTarget = NULL );

	virtual Vector GetAutoaimVector( float flDelta );

	virtual void PlayStepSound( Vector& vecOrigin, surfacedata_t* psurface, float fvol, bool force );

	virtual	bool ShouldCollide( int collisionGroup, int contentsMask ) const;

private:

	CHL2MPPlayerAnimState* m_PlayerAnimState;

	void LadderRespawnFix();

	CNetworkQAngle( m_angEyeAngles );

	int m_iLastWeaponFireUsercmd;
	int m_iModelType;
	CNetworkVar( int, m_iSpawnInterpCounter );
	CNetworkVar( int, m_iPlayerSoundType );

	float m_flNextModelChangeTime;
	float m_flNextTeamChangeTime;

	float m_flSlamProtectTime;

	HL2MPPlayerState m_iPlayerState;
	CHL2MPPlayerStateInfo* m_pCurStateInfo;

	bool m_bEnterObserver;

	CNetworkVar( int, m_cycleLatch ); // Network the cycle to clients periodically
	CountdownTimer m_cycleLatchTimer;
};

inline CHL2MP_Player* ToHL2MPPlayer( CBaseEntity* pEntity )
{
	if( !pEntity || !pEntity->IsPlayer() )
	{
		return NULL;
	}

	return dynamic_cast<CHL2MP_Player*>( pEntity );
}

// I have no idea why this was never added
inline CHL2MP_Player* ToHL2MPPlayer( CBasePlayer* pPlayer )
{
	return static_cast<CHL2MP_Player*>( pPlayer );
}

#endif //HL2MP_PLAYER_H
