//========= Copyright � 1996-2005, Valve Corporation, All rights reserved. ============//
// Authors:
// Iv�n Bravo Bravo (linkedin.com/in/ivanbravobravo), 2017

#include "cbase.h"
#include "bot.h"
#include "in_utils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBotProfile::CBotProfile()
{
	int minSkill = TheGameRules->GetSkillLevel();
	SetSkill( RandomInt( minSkill, minSkill + 2 ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBotProfile::CBotProfile( int skill )
{
	SetSkill( skill );
}

//-----------------------------------------------------------------------------
// Purpose: Sets the level of difficulty
//-----------------------------------------------------------------------------
void CBotProfile::SetSkill( int skill )
{
	skill = clamp( skill, SKILL_EASIEST, SKILL_HARDEST );

	switch( skill )
	{
		case SKILL_EASY:
			SetMemoryDuration( 3.0f );
			SetReactionDelay( RandomFloat( 0.3f, 0.5f ) );
			SetAlertDuration( RandomFloat( 3.0f, 5.0f ) );
			SetAimSpeed( AIM_SPEED_LOW, AIM_SPEED_NORMAL );
			SetAttackDelay( RandomFloat( 0.01f, 0.05f ) );
			SetFavoriteHitbox( HITGROUP_STOMACH );
			SetAggression( 30.0f );
			break;

		case SKILL_MEDIUM:
			SetMemoryDuration( 5.0f );
			SetReactionDelay( RandomFloat( 0.1f, 0.3f ) );
			SetAlertDuration( RandomFloat( 3.0f, 6.0f ) );
			SetAimSpeed( AIM_SPEED_NORMAL, AIM_SPEED_FAST );
			SetAttackDelay( RandomFloat( 0.005f, 0.01f ) );
			SetFavoriteHitbox( RandomInt( HITGROUP_CHEST, HITGROUP_STOMACH ) );
			SetAggression( 60.0f );
			break;

		case SKILL_HARD:
		default:
			SetMemoryDuration( 10.0f );
			SetReactionDelay( RandomFloat( 0.0f, 0.01f ) );
			SetAlertDuration( RandomFloat( 4.0f, 7.0f ) );
			SetAimSpeed( AIM_SPEED_FAST, AIM_SPEED_VERYFAST );
			SetAttackDelay( RandomFloat( 0.0001f, 0.005f ) );
			SetFavoriteHitbox( RandomInt( HITGROUP_CHEST, HITGROUP_STOMACH ) );
			SetAggression( 100.0f );
			break;
	}

	m_iSkillLevel = skill;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the name of the difficulty level
//-----------------------------------------------------------------------------
const char* CBotProfile::GeSkillName()
{
	switch( m_iSkillLevel )
	{
		case SKILL_EASY:
			return "EASY";
			break;

		case SKILL_MEDIUM:
			return "MEDIUM";
			break;

		case SKILL_HARD:
			return "HARD";
			break;
	}

	return "UNKNOWN";
}