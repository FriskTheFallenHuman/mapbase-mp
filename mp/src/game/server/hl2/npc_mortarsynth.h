//======= Copyright © 2022, Obsidian Conflict Team, All rights reserved. =======//
//
// Purpose: Mortar Synth NPC
//
//==============================================================================//

#ifndef NPC_MORTARSYNTH_H
#define NPC_MORTARSYNTH_H
#ifdef _WIN32
	#pragma once
#endif

#include "npc_basescanner.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
class CNPC_Mortarsynth : public CNPC_BaseScanner
{
	DECLARE_CLASS( CNPC_Mortarsynth, CNPC_BaseScanner );

public:
	CNPC_Mortarsynth();

	Class_T			Classify( void )
	{
		return( CLASS_COMBINE );
	}
	int				GetSoundInterests( void )
	{
		return ( SOUND_WORLD | SOUND_COMBAT | SOUND_PLAYER | SOUND_DANGER | SOUND_BULLET_IMPACT | SOUND_PHYSICS_DANGER | SOUND_MOVE_AWAY );
	}

	void			Precache( void );
	void			Spawn( void );

	void			Event_Killed( const CTakeDamageInfo& info );
	int				OnTakeDamage_Alive( const CTakeDamageInfo& info );
	virtual void	Gib( void );
	virtual bool	HasHumanGibs( void )
	{
		return false;
	}; //Fenix: Hack to get rid of the human gibs set to CLASS_COMBINE

	int				SelectSchedule( void );
	virtual int		TranslateSchedule( int scheduleType );
	void			HandleAnimEvent( animevent_t* pEvent );

	void			RunTask( const Task_t* pTask );
	void			StartTask( const Task_t* pTask );

	void			UpdateOnRemove( void );

	virtual bool	IsHeavyDamage( const CTakeDamageInfo& info );

	bool			CanThrowGrenade( const Vector& vecTarget );
	bool			CheckCanThrowGrenade( const Vector& vecTarget );
	virtual	bool	CanGrenadeEnemy( bool bUseFreeKnowledge = true );

	virtual	bool	CanRepelEnemy();

	virtual void	StopLoopingSounds( void );

	//Fenix: Override baseclass functions to disable the sound prefix system
	void			IdleSound( void ) {};
	void			DeathSound( const CTakeDamageInfo& info ) {};
	void			AlertSound( void ) {};
	void			PainSound( const CTakeDamageInfo& info ) {};

protected:
	virtual float	MinGroundDist( void );

	virtual void	MoveExecute_Alive( float flInterval );
	virtual bool	OverridePathMove( CBaseEntity* pMoveTarget, float flInterval );
	bool			OverrideMove( float flInterval );
	void			MoveToTarget( float flInterval, const Vector& MoveTarget );
	virtual void	MoveToAttack( float flInterval );

	void			PlayFlySound( void );

	void			UseRepulserWeapon( void );
	void			PushEntity( CBaseEntity* pTarget );

	virtual void	TurnHeadToTarget( float flInterval, const Vector& moveTarget );


	float			m_flNextFlinch;
	float			m_flNextRepulse;

private:
	Vector			m_vecTossVelocity;
	Vector			m_vTargetBanking;


	DEFINE_CUSTOM_AI;

	// Custom schedules
	enum
	{
		SCHED_MORTARSYNTH_PATROL = BaseClass::NEXT_SCHEDULE,
		SCHED_MORTARSYNTH_ATTACK,
		SCHED_MORTARSYNTH_REPEL,
		SCHED_MORTARSYNTH_CHASE_ENEMY,
		SCHED_MORTARSYNTH_CHASE_TARGET,
		SCHED_MORTARSYNTH_HOVER,

		NEXT_SCHEDULE,
	};

	// Custom tasks
	enum
	{
		TASK_ENERGY_REPEL = BaseClass::NEXT_TASK,

		NEXT_TASK,
	};

	DECLARE_DATADESC();
};

#endif // NPC_DEFENDER_H
