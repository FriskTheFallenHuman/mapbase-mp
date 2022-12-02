//========= Copyright © 1996-2008, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef HL2MP_PLAYERANIMSTATE_H
#define HL2MP_PLAYERANIMSTATE_H
#ifdef _WIN32
#pragma once
#endif


#include "convar.h"
#include "multiplayer_animstate.h"

#if defined( CLIENT_DLL )
class C_HL2MP_Player;
#define CHL2MP_Player C_HL2MP_Player
#else
class CHL2MP_Player;
#endif

// ------------------------------------------------------------------------------------------------ //
// CPlayerAnimState declaration.
// ------------------------------------------------------------------------------------------------ //
class CHL2MPPlayerAnimState : public CMultiPlayerAnimState
{
public:
	
	DECLARE_CLASS( CHL2MPPlayerAnimState, CMultiPlayerAnimState );

	CHL2MPPlayerAnimState();
	CHL2MPPlayerAnimState( CBasePlayer *pPlayer, MultiPlayerMovementData_t &movementData );
	~CHL2MPPlayerAnimState();

	void InitHL2MPAnimState( CHL2MP_Player *pPlayer );
	CHL2MP_Player *GetHL2MPPlayer( void )							{ return m_pHL2MPPlayer; }

#ifdef MAPBASE_MP
	virtual void GetOuterAbsVelocity( Vector& vel );
	virtual bool ShouldUpdateAnimState();
#endif
	virtual void ClearAnimationState();
	virtual Activity TranslateActivity( Activity actDesired );
	virtual void Update( float eyeYaw, float eyePitch );

	void	DoAnimationEvent( PlayerAnimEvent_t event, int nData = 0 );

	bool	HandleMoving( Activity &idealActivity );
	bool	HandleJumping( Activity &idealActivity );
	bool	HandleDucking( Activity &idealActivity );
	bool	HandleSwimming( Activity &idealActivity );

	virtual float GetCurrentMaxGroundSpeed();

#ifdef MAPBASE_MP
	bool Uses9WayAnim() const { return m_bIs9Way; }
#endif

private:
	//Tony; temp till 9way!
	bool						SetupPoseParameters( CStudioHdr *pStudioHdr );
	virtual void				EstimateYaw( void );
	virtual void				ComputePoseParam_MoveYaw( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimPitch( CStudioHdr *pStudioHdr );
	virtual void				ComputePoseParam_AimYaw( CStudioHdr *pStudioHdr );
#ifdef MAPBASE_MP
#ifdef CLIENT_DLL
	virtual void				ComputePoseParam_Head( CStudioHdr* pStudioHdr );

	virtual void				UpdateLookAt();
#endif // CLIENT_DLL
#endif
	
	CHL2MP_Player   *m_pHL2MPPlayer;
	bool		m_bInAirWalk;
	float		m_flHoldDeployedPoseUntilTime;

#ifdef MAPBASE_MP
	Vector		m_vLookAtTarget;
	float		m_flLastLookAtUpdate;

	int		m_headYawPoseParam;
	int		m_headPitchPoseParam;
	float		m_headYawMin;
	float		m_headYawMax;
	float		m_headPitchMin;
	float		m_headPitchMax;
	float		m_flLastBodyYaw;
	float		m_flCurrentHeadYaw;
	float		m_flCurrentHeadPitch;
	float		m_flCurrentAimYaw;
	CountdownTimer		m_blinkTimer;

	bool		m_bIs9Way;
#endif
};

CHL2MPPlayerAnimState *CreateHL2MPPlayerAnimState( CHL2MP_Player *pPlayer );



#endif // HL2MP_PLAYERANIMSTATE_H
