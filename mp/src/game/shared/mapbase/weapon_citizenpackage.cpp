//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "weapon_citizenpackage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponCitizenPackage, DT_WeaponCitizenPackage )

BEGIN_NETWORK_TABLE( CWeaponCitizenPackage, DT_WeaponCitizenPackage )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponCitizenPackage )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_citizenpackage, CWeaponCitizenPackage );
PRECACHE_WEAPON_REGISTER( weapon_citizenpackage );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponCitizenPackage::m_acttable[] =
{
	{ ACT_IDLE,						ACT_IDLE_PACKAGE,					false },
	{ ACT_WALK,						ACT_WALK_PACKAGE,					false },
#if EXPANDED_HL2_WEAPON_ACTIVITIES
	{ ACT_RUN,						ACT_RUN_PACKAGE,					false },

	{ ACT_IDLE_ANGRY,				ACT_IDLE_PACKAGE,					false },
	{ ACT_WALK_AIM,					ACT_WALK_PACKAGE,					false },
	{ ACT_RUN_AIM,					ACT_RUN_PACKAGE,					false },
#endif
};
IMPLEMENT_ACTTABLE( CWeaponCitizenPackage );

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponCitizenSuitcase, DT_WeaponCitizenSuitcase )

BEGIN_NETWORK_TABLE( CWeaponCitizenSuitcase, DT_WeaponCitizenSuitcase )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponCitizenSuitcase )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_citizensuitcase, CWeaponCitizenSuitcase );
PRECACHE_WEAPON_REGISTER( weapon_citizensuitcase );

//-----------------------------------------------------------------------------
// Maps base activities to weapons-specific ones so our characters do the right things.
//-----------------------------------------------------------------------------
acttable_t	CWeaponCitizenSuitcase::m_acttable[] =
{
	{ ACT_IDLE,						ACT_IDLE_SUITCASE,					false },
	{ ACT_WALK,						ACT_WALK_SUITCASE,					false },
#if EXPANDED_HL2_WEAPON_ACTIVITIES
	{ ACT_RUN,						ACT_RUN_SUITCASE,					false },

	{ ACT_IDLE_ANGRY,				ACT_IDLE_SUITCASE,					false },
	{ ACT_WALK_AIM,					ACT_WALK_SUITCASE,					false },
	{ ACT_RUN_AIM,					ACT_RUN_SUITCASE,					false },
#endif
};

IMPLEMENT_ACTTABLE( CWeaponCitizenSuitcase );
