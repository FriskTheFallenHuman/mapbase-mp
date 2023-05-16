//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors:
// Iv�n Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bot.h"
#include "in_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar bot_primary_attack;
extern ConVar bot_secondary_attack;
extern ConVar bot_tertiary_attack;

//-----------------------------------------------------------------------------
// Purpose: Sets a condition
//-----------------------------------------------------------------------------
void CBot::SetCondition( BCOND condition )
{
	m_nConditions.Set( condition );
}

//-----------------------------------------------------------------------------
// Purpose: Forgets a condition
//-----------------------------------------------------------------------------
void CBot::ClearCondition( BCOND condition )
{
	m_nConditions.Clear( condition );
}

//-----------------------------------------------------------------------------
// Purpose: Returns if the bot is in a condition
//-----------------------------------------------------------------------------
bool CBot::HasCondition( BCOND condition ) const
{
	if( IsConditionsAllowed() )
	{
		Assert( !"Attempt to verify a condition before gathering!" );
		return false;
	}

	return m_nConditions.IsBitSet( condition );
}

//-----------------------------------------------------------------------------
// Purpose: Add a component to the list
//-----------------------------------------------------------------------------
void CBot::AddComponent( IBotComponent* pComponent )
{
	if( !pComponent )
	{
		return;
	}

	pComponent->Reset();

	if( pComponent->IsSchedule() )
	{
		IBotSchedule* pSchedule = dynamic_cast<IBotSchedule*>( pComponent );
		Assert( pSchedule );

		if( pSchedule )
		{
			m_nSchedules.InsertOrReplace( pSchedule->GetID(), pSchedule );
		}
	}
	else
	{
		m_nComponents.InsertOrReplace( pComponent->GetID(), pComponent );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Create the components that the bot will have
//-----------------------------------------------------------------------------
void CBot::SetUpComponents()
{
	// Basic components
	// Each custom A.I. decide which ones to remove and add.
	ADD_COMPONENT( CBotVision );
	ADD_COMPONENT( CBotFollow );
	ADD_COMPONENT( CBotLocomotion );
	ADD_COMPONENT( CBotMemory );
	ADD_COMPONENT( CBotAttack );
	ADD_COMPONENT( CBotDecision ); // This component is mandatory!
}

//-----------------------------------------------------------------------------
// Purpose: Create the schedules that the bot will have
//-----------------------------------------------------------------------------
void CBot::SetUpSchedules()
{
	// Basic schedules
	// Each custom A.I. decide which ones to remove and add.
	ADD_COMPONENT( CHuntEnemySchedule );
	ADD_COMPONENT( CReloadSchedule );
	ADD_COMPONENT( CCoverSchedule );
	ADD_COMPONENT( CHideSchedule );
	ADD_COMPONENT( CChangeWeaponSchedule );
	ADD_COMPONENT( CHideAndHealSchedule );
	ADD_COMPONENT( CHideAndReloadSchedule );
	ADD_COMPONENT( CMoveAsideSchedule );
	ADD_COMPONENT( CCallBackupSchedule );
	ADD_COMPONENT( CDefendSpawnSchedule );
	//ADD_COMPONENT( CInvestigateLocationSchedule ); // TODO: Finish
}

//-----------------------------------------------------------------------------
// Purpose: Returns the [IBotSchedule] registered for this ID
//-----------------------------------------------------------------------------
IBotSchedule* CBot::GetSchedule( int schedule )
{
	int index = m_nSchedules.Find( schedule );

	if( m_nSchedules.IsValidIndex( index ) )
	{
		return m_nSchedules.Element( index );
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the active schedule ID.
//-----------------------------------------------------------------------------
int CBot::GetActiveScheduleID()
{
	if( !GetActiveSchedule() )
	{
		return SCHEDULE_NONE;
	}

	return GetActiveSchedule()->GetID();
}

//-----------------------------------------------------------------------------
// Purpose: Returns the ideal schedule for the bot (With more desire)
//-----------------------------------------------------------------------------
int CBot::SelectIdealSchedule()
{
	if( !GetHost()->IsAlive() )
	{
		return SCHEDULE_NONE;
	}

	float desire = BOT_DESIRE_NONE;
	IBotSchedule* pIdeal = GetActiveSchedule();

	// We have an active schedule
	// We take her initial desire level
	if( pIdeal )
	{
		desire = pIdeal->GetInternalDesire();
	}

	FOR_EACH_MAP( m_nSchedules, it )
	{
		IBotSchedule* pSchedule = m_nSchedules[it];

		// This schedule has a greater desire!
		if( pSchedule->GetInternalDesire() > desire )
		{
			pIdeal = pSchedule;
			desire = pSchedule->GetDesire();
		}
	}

	if( pIdeal && desire > BOT_DESIRE_NONE )
	{
		return pIdeal->GetID();
	}

	return SCHEDULE_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: Updates the current schedule
//-----------------------------------------------------------------------------
void CBot::UpdateSchedule()
{
	VPROF_BUDGET( "UpdateSchedule", VPROF_BUDGETGROUP_BOTS );

	// Maybe an custom A.I. want to change a schedule.
	int idealSchedule = TranslateSchedule( SelectIdealSchedule() );

	if( idealSchedule == SCHEDULE_NONE )
	{
		if( GetActiveSchedule() )
		{
			GetActiveSchedule()->Finish();
			m_nActiveSchedule = NULL;
		}

		return;
	}

	IBotSchedule* pSchedule = GetSchedule( idealSchedule );
	AssertMsg( pSchedule, "GetSchedule == NULL" );

	if( pSchedule == NULL )
	{
		return;
	}

	if( GetActiveSchedule() )
	{
		if( GetActiveSchedule() == pSchedule )
		{
			m_ScheduleTimer.Start();
			GetActiveSchedule()->Update();
			m_ScheduleTimer.End();
			return;
		}
		else
		{
			GetActiveSchedule()->Finish();
		}
	}

	m_ScheduleTimer.Start();
	m_nActiveSchedule = pSchedule;
	m_nActiveSchedule->Start();
	m_nActiveSchedule->Update();
	m_ScheduleTimer.End();
}

//-----------------------------------------------------------------------------
// Purpose: Mark a task as complete
//			TODO: Real life scenario where this is used?
//-----------------------------------------------------------------------------
void CBot::TaskComplete()
{
	if( !GetActiveSchedule() )
	{
		return;
	}

	GetActiveSchedule()->TaskComplete();
}

//-----------------------------------------------------------------------------
// Purpose: Mark as failed a schedule
//			TODO: Real life scenario where this is used?
//-----------------------------------------------------------------------------
void CBot::TaskFail( const char* pWhy )
{
	if( !GetActiveSchedule() )
	{
		return;
	}

	GetActiveSchedule()->Fail( pWhy );
}

//-----------------------------------------------------------------------------
// Purpose: Gets new conditions from environment and statistics
//			Conditions that do not require information about components (vision/smell/hearing)
//-----------------------------------------------------------------------------
void CBot::GatherConditions()
{
	VPROF_BUDGET( "GatherConditions", VPROF_BUDGETGROUP_BOTS );

	GatherHealthConditions();
	GatherWeaponConditions();
	GatherEnemyConditions();
	GatherAttackConditions();
	GatherLocomotionConditions();
}

//-----------------------------------------------------------------------------
// Purpose: Obtains new conditions related to health and damage
//-----------------------------------------------------------------------------
void CBot::GatherHealthConditions()
{
	VPROF_BUDGET( "GatherHealthConditions", VPROF_BUDGETGROUP_BOTS );

	if( GetDecision()->IsLowHealth() )
	{
		SetCondition( BCOND_LOW_HEALTH );
	}

	if( m_iRepeatedDamageTimes == 0 )
	{
		return;
	}

	if( m_iRepeatedDamageTimes <= 2 )
	{
		SetCondition( BCOND_LIGHT_DAMAGE );
	}
	else
	{
		SetCondition( BCOND_REPEATED_DAMAGE );
	}

	if( m_flDamageAccumulated >= 30.0f )
	{
		SetCondition( BCOND_HEAVY_DAMAGE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets new conditions related to current weapon
//-----------------------------------------------------------------------------
void CBot::GatherWeaponConditions()
{
	VPROF_BUDGET( "GatherWeaponConditions", VPROF_BUDGETGROUP_BOTS );

	// We change to the best weapon for this situation
	// TODO: A better place to put this.
	GetDecision()->SwitchToBestWeapon();

	CBaseWeapon* pWeapon = GetHost()->GetActiveBaseWeapon();

	if( pWeapon == NULL )
	{
		SetCondition( BCOND_HELPLESS );
		return;
	}

	// Primary ammunition
	{
		int ammo = 0;
		int totalAmmo = GetHost()->GetAmmoCount( pWeapon->GetPrimaryAmmoType() );
		int totalRange = 30;

		if( pWeapon->UsesClipsForAmmo1() )
		{
			ammo = pWeapon->Clip1();
			int maxAmmo = pWeapon->GetMaxClip1();
			totalRange = ( maxAmmo * 0.5 );

			if( ammo == 0 )
			{
				SetCondition( BCOND_EMPTY_CLIP1_AMMO );
			}
			else if( ammo < totalRange )
			{
				SetCondition( BCOND_LOW_CLIP1_AMMO );
			}
		}

		if( totalAmmo == 0 )
		{
			SetCondition( BCOND_EMPTY_PRIMARY_AMMO );
		}
		else if( totalAmmo < totalRange )
		{
			SetCondition( BCOND_LOW_PRIMARY_AMMO );
		}
	}

	// Secondary ammunition
	{
		int ammo = 0;
		int totalAmmo = GetHost()->GetAmmoCount( pWeapon->GetSecondaryAmmoType() );
		int totalRange = 15;

		if( pWeapon->UsesClipsForAmmo2() )
		{
			ammo = pWeapon->Clip2();
			int maxAmmo = pWeapon->GetMaxClip2();
			totalRange = ( maxAmmo * 0.5 );

			if( ammo == 0 )
			{
				SetCondition( BCOND_EMPTY_CLIP2_AMMO );
			}
			else if( ammo < totalRange )
			{
				SetCondition( BCOND_LOW_CLIP2_AMMO );
			}
		}

		if( totalAmmo == 0 )
		{
			SetCondition( BCOND_EMPTY_SECONDARY_AMMO );
		}
		else if( totalAmmo < totalRange )
		{
			SetCondition( BCOND_LOW_SECONDARY_AMMO );
		}
	}

	// You have no ammunition of any kind, you are defenseless
	if( HasCondition( BCOND_EMPTY_PRIMARY_AMMO ) &&
			HasCondition( BCOND_EMPTY_CLIP1_AMMO ) &&
			HasCondition( BCOND_EMPTY_SECONDARY_AMMO ) &&
			HasCondition( BCOND_EMPTY_CLIP2_AMMO ) )
	{
		SetCondition( BCOND_HELPLESS );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets new conditions related to the current enemy
//-----------------------------------------------------------------------------
void CBot::GatherEnemyConditions()
{
	VPROF_BUDGET( "GatherEnemyConditions", VPROF_BUDGETGROUP_BOTS );

	CEntityMemory* memory = GetPrimaryThreat();

	if( memory == NULL )
	{
		SetCondition( BCOND_WITHOUT_ENEMY );
		return;
	}

	if( !IsCombating() )
	{
		Alert();
	}

	// List of distances to establish useful conditions.
	// TODO: No hard-coded
	const float distances[] =
	{
		100.0f, // Too near
		300.0f, // Near
		800.0f, // Far
		1300.0f // Too far!
	};

	float enemyDistance = memory->GetDistance();

	if( enemyDistance <= distances[0] )
	{
		SetCondition( BCOND_ENEMY_TOO_NEAR );
	}
	if( enemyDistance <= distances[1] )
	{
		SetCondition( BCOND_ENEMY_NEAR );
	}
	if( enemyDistance >= distances[2] )
	{
		SetCondition( BCOND_ENEMY_FAR );
	}
	if( enemyDistance >= distances[3] )
	{
		SetCondition( BCOND_ENEMY_TOO_FAR );
	}

	if( !memory->IsVisible() )
	{
		// We have no vision of the enemy
		SetCondition( BCOND_ENEMY_LOST );
	}
	else
	{
		SetCondition( BCOND_SEE_ENEMY );

		CBaseEntity* pBlockedBy = NULL;

		// No direct line of sight with our enemy
		if( !GetDecision()->IsLineOfSightClear( memory->GetEntity(), &pBlockedBy ) )
		{
			SetCondition( BCOND_ENEMY_OCCLUDED );

			// A friend is in front of us!
			if( GetDecision()->IsFriend( pBlockedBy ) )
			{
				SetCondition( BCOND_ENEMY_OCCLUDED_BY_FRIEND );
			}
		}
	}

	if( GetDecision()->IsAbleToSee( memory->GetLastKnownPosition() ) )
	{
		SetCondition( BCOND_ENEMY_LAST_POSITION_VISIBLE );
	}

	if( !GetEnemy()->IsAlive() )
	{
		if( GetProfile()->IsEasy() )
		{
			if( GetEnemy()->m_lifeState == LIFE_DEAD )
			{
				SetCondition( BCOND_ENEMY_DEAD );
			}
		}
		else
		{
			SetCondition( BCOND_ENEMY_DEAD );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets new conditions related to the attack
//-----------------------------------------------------------------------------
void CBot::GatherAttackConditions()
{
	VPROF_BUDGET( "SelectAttackConditions", VPROF_BUDGETGROUP_BOTS );

	BCOND condition = GetDecision()->ShouldRangeAttack1();

	if( condition != BCOND_NONE )
	{
		SetCondition( condition );
	}

	condition = GetDecision()->ShouldRangeAttack2();

	if( condition != BCOND_NONE )
	{
		SetCondition( condition );
	}

	condition = GetDecision()->ShouldMeleeAttack1();

	if( condition != BCOND_NONE )
	{
		SetCondition( condition );
	}

	condition = GetDecision()->ShouldMeleeAttack2();

	if( condition != BCOND_NONE )
	{
		SetCondition( condition );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::GatherLocomotionConditions()
{
	if( !GetLocomotion() )
	{
		return;
	}

	if( GetLocomotion()->HasDestination() )
	{
		if( GetLocomotion()->IsUnreachable() )
		{
			SetCondition( BCOND_GOAL_UNREACHABLE );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CSquad* CBot::GetSquad()
{
	return GetHost()->GetSquad();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::SetSquad( CSquad* pSquad )
{
	GetHost()->SetSquad( pSquad );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::SetSquad( const char* name )
{
	GetHost()->SetSquad( name );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CBot::IsSquadLeader()
{
	if( !GetSquad() )
	{
		return false;
	}

	return ( GetSquad()->GetLeader() == GetHost() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CPlayer* CBot::GetSquadLeader()
{
	if( !GetSquad() )
	{
		return NULL;
	}

	return GetSquad()->GetLeader();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
IBot* CBot::GetSquadLeaderAI()
{
	if( !GetSquad() )
	{
		return NULL;
	}

	if( !GetSquad()->GetLeader() )
	{
		return NULL;
	}

	return GetSquad()->GetLeader()->GetBotController();
}

//-----------------------------------------------------------------------------
// Purpose: A member of our squad has taken damage
//-----------------------------------------------------------------------------
void CBot::OnMemberTakeDamage( CPlayer* pMember, const CTakeDamageInfo& info )
{
	// They have attacked a normal player, let's report their Bots friends
	if( !pMember->IsBot() && info.GetAttacker() )
	{
		OnMemberReportEnemy( pMember, info.GetAttacker() );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::OnMemberDeath( CPlayer* pMember, const CTakeDamageInfo& info )
{
	if( !GetMemory() || !GetVision() )
	{
		return;
	}

	CEntityMemory* memory = GetMemory()->GetEntityMemory( pMember );

	if( !memory )
	{
		return;
	}

	// RIP
	if( !GetProfile()->IsHardest() && memory->IsVisible() )
	{
		GetVision()->LookAt( "Squad Member Death", pMember->GetAbsOrigin(), PRIORITY_VERY_HIGH, RandomFloat( 0.3f, 1.5f ) );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CBot::OnMemberReportEnemy( CPlayer* pMember, CBaseEntity* pEnemy )
{
	if( !GetMemory() )
	{
		return;
	}

	// Estimate position of the enemy
	Vector vecEstimatedPosition = pEnemy->WorldSpaceCenter();
	const float errorDistance = 100.0f;

	// When a friend reports to us the position of an enemy there should always be a margin of error,
	// A human cannot know the exact position until he sees it with his own eyes.
	vecEstimatedPosition.x += RandomFloat( -errorDistance, errorDistance );
	vecEstimatedPosition.y += RandomFloat( -errorDistance, errorDistance );

	// Update our memory
	GetMemory()->UpdateEntityMemory( pEnemy, vecEstimatedPosition, pMember );
}
