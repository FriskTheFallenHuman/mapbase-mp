//==== Woots 2016. http://creativecommons.org/licenses/by/2.5/mx/ ===========//

#include "cbase.h"
#include "in_utils.h"

#include "physobj.h"
#include "collisionutils.h"
#include "movevars_shared.h"

#include "ai_basenpc.h"
#include "ai_initutils.h"
#include "ai_hint.h"

#include "BasePropDoor.h"
#include "doors.h"
#include "func_breakablesurf.h"

#include "bot_defs.h"
#include "nav_pathfind.h"
#include "util_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar sv_gameseed( "sv_gameseed", "", FCVAR_NOT_CONNECTED | FCVAR_SERVER );

#define COS_TABLE_SIZE 256
static float cosTable[ COS_TABLE_SIZE ];

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static int SeedLineHash( int seedvalue, const char* sharedname )
{
	CRC32_t retval;

	CRC32_Init( &retval );

	CRC32_ProcessBuffer( &retval, ( void* )&seedvalue, sizeof( int ) );
	CRC32_ProcessBuffer( &retval, ( void* )sharedname, Q_strlen( sharedname ) );

	CRC32_Final( &retval );

	return ( int )( retval );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float Utils::RandomFloat( const char* sharedname, float flMinVal, float flMaxVal )
{
	int seed = SeedLineHash( sv_gameseed.GetInt(), sharedname );
	RandomSeed( seed );
	return ::RandomFloat( flMinVal, flMaxVal );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int Utils::RandomInt( const char* sharedname, int iMinVal, int iMaxVal )
{
	int seed = SeedLineHash( sv_gameseed.GetInt(), sharedname );
	RandomSeed( seed );
	return ::RandomInt( iMinVal, iMaxVal );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
Vector Utils::RandomVector( const char* sharedname, float minVal, float maxVal )
{
	int seed = SeedLineHash( sv_gameseed.GetInt(), sharedname );
	RandomSeed( seed );
	// HACK:  Can't call RandomVector/Angle because it uses rand() not vstlib Random*() functions!
	// Get a random vector.
	Vector random;
	random.x = ::RandomFloat( minVal, maxVal );
	random.y = ::RandomFloat( minVal, maxVal );
	random.z = ::RandomFloat( minVal, maxVal );
	return random;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
QAngle Utils::RandomAngle( const char* sharedname, float minVal, float maxVal )
{
	Assert( CBaseEntity::GetPredictionRandomSeed() != -1 );

	int seed = SeedLineHash( sv_gameseed.GetInt(), sharedname );
	RandomSeed( seed );

	// HACK:  Can't call RandomVector/Angle because it uses rand() not vstlib Random*() functions!
	// Get a random vector.
	Vector random;
	random.x = ::RandomFloat( minVal, maxVal );
	random.y = ::RandomFloat( minVal, maxVal );
	random.z = ::RandomFloat( minVal, maxVal );
	return QAngle( random.x, random.y, random.z );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::NormalizeAngle( float& fAngle )
{
	if( fAngle < 0.0f )
	{
		fAngle += 360.0f;
	}
	else if( fAngle >= 360.0f )
	{
		fAngle -= 360.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::DeNormalizeAngle( float& fAngle )
{
	if( fAngle < -180.0f )
	{
		fAngle += 360.0f;
	}
	else if( fAngle >= 180.0f )
	{
		fAngle -= 360.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::GetAngleDifference( QAngle const& angOrigin, QAngle const& angDestination, QAngle& angDiff )
{
	angDiff = angDestination - angOrigin;

	Utils::DeNormalizeAngle( angDiff.x );
	Utils::DeNormalizeAngle( angDiff.y );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::IsBreakableSurf( CBaseEntity* pEntity )
{
	if( !pEntity )
	{
		return false;
	}

	// Can't take any damage
	if( pEntity->m_takedamage != DAMAGE_YES )
	{
		return false;
	}

	// Breakable surface
	if( ( dynamic_cast<CBreakableSurface*>( pEntity ) ) )
	{
		return true;
	}

	// Breakable wall
	if( ( dynamic_cast<CBreakable*>( pEntity ) ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::IsBreakable( CBaseEntity* pEntity )
{
	if( !pEntity )
	{
		return false;
	}

	// Can't take any damage
	if( pEntity->m_takedamage != DAMAGE_YES )
	{
		return false;
	}

	// Breakable entity
	if( ( dynamic_cast<CBreakableProp*>( pEntity ) ) )
	{
		return true;
	}

	// Breakable wall
	if( ( dynamic_cast<CBreakable*>( pEntity ) ) )
	{
		return true;
	}

	// Breakable surface
	if( ( dynamic_cast<CBreakableSurface*>( pEntity ) ) )
	{
		CBreakableSurface* surf = static_cast< CBreakableSurface* >( pEntity );

		// Its already broken
		if( surf->m_bIsBroken )
		{
			return false;
		}

		return true;
	}

	// Is A door
	if( ( dynamic_cast<CBasePropDoor*>( pEntity ) ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::IsDoor( CBaseEntity* pEntity )
{
	if( !pEntity )
	{
		return false;
	}

	CBaseDoor* pDoor = dynamic_cast<CBaseDoor*>( pEntity );

	if( pDoor )
	{
		return true;
	}

	CBasePropDoor* pPropDoor = dynamic_cast<CBasePropDoor*>( pEntity );

	if( pPropDoor )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseEntity* Utils::FindNearestPhysicsObject( const Vector& vOrigin, float fMaxDist, float fMinMass, float fMaxMass, CBaseEntity* pFrom )
{
	CBaseEntity* pFinalEntity    = NULL;
	CBaseEntity* pThrowEntity    = NULL;
	float flNearestDist            = 0;

	// loop trough all objects we can throw
	do
	{
		// Physics props
		pThrowEntity = gEntList.FindEntityByClassnameWithin( pThrowEntity, "prop_physics", vOrigin, fMaxDist );

		// Doesn't exist anymore
		if( !pThrowEntity )
		{
			continue;
		}

		// Isn't visible
		if( pFrom )
		{
			if( !pFrom->FVisible( pThrowEntity ) )
			{
				continue;
			}
		}

		// Physics information is NULL
		if( !pThrowEntity->VPhysicsGetObject() )
		{
			continue;
		}

		// Can't be move
		if( !pThrowEntity->VPhysicsGetObject()->IsMoveable() )
		{
			continue;
		}

		Vector v_center    = pThrowEntity->WorldSpaceCenter();
		float flDist    = UTIL_DistApprox2D( vOrigin, v_center );

		// Too far from our last prop
		if( flDist > flNearestDist && flNearestDist != 0 )
		{
			continue;
		}

		// Calculate the distance of our enemy
		if( pFrom && pFrom->IsNPC() )
		{
			CAI_BaseNPC* pNPC = dynamic_cast<CAI_BaseNPC*>( pFrom );

			if( pNPC && pNPC->GetEnemy() )
			{
				Vector vecDirToEnemy    = pNPC->GetEnemy()->GetAbsOrigin() - pNPC->GetAbsOrigin();
				vecDirToEnemy.z            = 0;

				Vector vecDirToObject = pThrowEntity->WorldSpaceCenter() - vOrigin;
				VectorNormalize( vecDirToObject );
				vecDirToObject.z = 0;

				if( DotProduct( vecDirToEnemy, vecDirToObject ) < 0.8 )
				{
					continue;
				}
			}
		}

		// Get our mass
		float pEntityMass = pThrowEntity->VPhysicsGetObject()->GetMass();

		// Too Lightweigh
		if( pEntityMass < fMinMass && fMinMass > 0 )
		{
			continue;
		}

		// Too Heavy
		if( pEntityMass > fMaxMass )
		{
			continue;
		}

		// Ignore any prop above our head
		if( v_center.z > vOrigin.z )
		{
			continue;
		}

		if( pFrom )
		{
			Vector vecGruntKnees;
			pFrom->CollisionProp()->NormalizedToWorldSpace( Vector( 0.5f, 0.5f, 0.25f ), &vecGruntKnees );

			vcollide_t* pCollide = modelinfo->GetVCollide( pThrowEntity->GetModelIndex() );

			Vector objMins, objMaxs;
			physcollision->CollideGetAABB( &objMins, &objMaxs, pCollide->solids[0], pThrowEntity->GetAbsOrigin(), pThrowEntity->GetAbsAngles() );

			if( objMaxs.z < vecGruntKnees.z )
			{
				continue;
			}
		}

		// This prop is perfect, save our distance for later
		flNearestDist    = flDist;
		pFinalEntity    = pThrowEntity;

	}
	while( pThrowEntity );

	// We didn't find any
	if( !pFinalEntity )
	{
		return NULL;
	}

	return pFinalEntity;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::IsMoveableObject( CBaseEntity* pEntity )
{
	if( !pEntity || pEntity->IsWorld() )
	{
		return false;
	}

	if( pEntity->IsPlayer() || pEntity->IsNPC() )
	{
		return true;
	}

	if( !pEntity->VPhysicsGetObject() )
	{
		return false;
	}

	if( !pEntity->VPhysicsGetObject()->IsMoveable() )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::RunOutEntityLimit( int iTolerance )
{
	if( gEntList.NumberOfEntities() < ( gpGlobals->maxEntities - iTolerance ) )
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
IGameEvent* Utils::CreateLesson( const char* pLesson, CBaseEntity* pSubject )
{
	IGameEvent* pEvent = gameeventmanager->CreateEvent( pLesson, true );

	if( pEvent )
	{
		if( pSubject )
		{
			pEvent->SetInt( "subject", pSubject->entindex() );
		}
	}

	return pEvent;
}

//-----------------------------------------------------------------------------
// Purpose:	Set [bones] with the Hitbox IDs of the entity.
//			TODO: Rename? (I think technically there are no "bones" here.)
//-----------------------------------------------------------------------------
bool Utils::GetEntityBones( CBaseEntity* pEntity, HitboxBones& bones )
{
	// Invalid
	bones.head = -1;
	bones.chest = -1;
	bones.leftLeg = -1;
	bones.rightLeg = -1;

	enum
	{
		TEAM_COMBINE = 2,
		TEAM_REBELS,
	};

	if( pEntity->IsPlayer() )
	{
		if( pEntity->GetTeamNumber() == TEAM_REBELS )
		{
			bones.head = 0;
			bones.chest = 0;
			bones.leftLeg = 7;
			bones.rightLeg = 11;
			return true;
		}
		else if( pEntity->GetTeamNumber() == TEAM_COMBINE )
		{
			bones.head = 17;
			bones.chest = 17;
			bones.leftLeg = 8;
			bones.rightLeg = 12;
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Fill in [positions] with the entity's hitbox positions
// 			If we do not have the Hitbox IDs of the entity then it will return generic positions.
// 			Use with care: this is often heavy for the engine.
//-----------------------------------------------------------------------------
bool Utils::GetHitboxPositions( CBaseEntity* pEntity, HitboxPositions& positions )
{
	positions.Reset();

	if( pEntity == NULL )
	{
		return false;
	}

	// Generic Positions
	positions.head = pEntity->EyePosition();
	positions.chest = positions.leftLeg = positions.rightLeg = pEntity->WorldSpaceCenter();

	CBaseAnimating* pModel = pEntity->GetBaseAnimating();

	if( pModel == NULL )
	{
		return false;
	}

	CStudioHdr* studioHdr = pModel->GetModelPtr();

	if( studioHdr == NULL )
	{
		return false;
	}

	mstudiohitboxset_t* set = studioHdr->pHitboxSet( pModel->GetHitboxSet() );

	if( set == NULL )
	{
		return false;
	}

	QAngle angles;
	mstudiobbox_t* box = NULL;
	HitboxBones bones;

	if( !GetEntityBones( pEntity, bones ) )
	{
		return false;
	}

	if( bones.head >= 0 )
	{
		box = set->pHitbox( bones.head );
		pModel->GetBonePosition( box->bone, positions.head, angles );
	}

	if( bones.chest >= 0 )
	{
		box = set->pHitbox( bones.chest );
		pModel->GetBonePosition( box->bone, positions.chest, angles );
	}

	if( bones.leftLeg >= 0 )
	{
		box = set->pHitbox( bones.leftLeg );
		pModel->GetBonePosition( box->bone, positions.leftLeg, angles );
	}

	if( bones.rightLeg >= 0 )
	{
		box = set->pHitbox( bones.rightLeg );
		pModel->GetBonePosition( box->bone, positions.rightLeg, angles );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Sets [vecPosition] to the desired hitbox position of the entity'
//-----------------------------------------------------------------------------
bool Utils::GetHitboxPosition( CBaseEntity* pEntity, Vector& vecPosition, HitboxType type )
{
	vecPosition.Invalidate();

	HitboxPositions positions;
	GetHitboxPositions( pEntity, positions );

	if( !positions.IsValid() )
	{
		return false;
	}

	switch( type )
	{
		case HITGROUP_HEAD:
			vecPosition = positions.head;
			break;

		case HITGROUP_CHEST:
		default:
			vecPosition = positions.chest;
			break;

		case HITGROUP_LEFTLEG:
			vecPosition = positions.leftLeg;
			break;

		case HITGROUP_RIGHTLEG:
			vecPosition = positions.rightLeg;
			break;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::InitBotTrig()
{
	for( int i = 0; i < COS_TABLE_SIZE; ++i )
	{
		float angle = ( float )( 2.0f * M_PI * i / ( float )( COS_TABLE_SIZE - 1 ) );
		cosTable[i] = ( float )cos( angle );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float Utils::BotCOS( float angle )
{
	angle = AngleNormalizePositive( angle );
	int i = ( int )( angle * ( COS_TABLE_SIZE - 1 ) / 360.0f );
	return cosTable[i];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float Utils::BotSIN( float angle )
{
	angle = AngleNormalizePositive( angle - 90 );
	int i = ( int )( angle * ( COS_TABLE_SIZE - 1 ) / 360.0f );
	return cosTable[i];
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool Utils::IsIntersecting2D( const Vector& startA, const Vector& endA, const Vector& startB, const Vector& endB, Vector* result )
{
	float denom = ( endA.x - startA.x ) * ( endB.y - startB.y ) - ( endA.y - startA.y ) * ( endB.x - startB.x );
	if( denom == 0.0f )
	{
		// parallel
		return false;
	}

	float numS = ( startA.y - startB.y ) * ( endB.x - startB.x ) - ( startA.x - startB.x ) * ( endB.y - startB.y );
	if( numS == 0.0f )
	{
		// coincident
		return true;
	}

	float numT = ( startA.y - startB.y ) * ( endA.x - startA.x ) - ( startA.x - startB.x ) * ( endA.y - startA.y );

	float s = numS / denom;
	if( s < 0.0f || s > 1.0f )
	{
		// intersection is not within line segment of startA to endA
		return false;
	}

	float t = numT / denom;
	if( t < 0.0f || t > 1.0f )
	{
		// intersection is not within line segment of startB to endB
		return false;
	}

	// compute intesection point
	if( result )
	{
		*result = startA + s * ( endA - startA );
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CPlayer* Utils::GetClosestPlayer( const Vector& vecPosition, float* distance, CPlayer* pIgnore, int team )
{
	CPlayer* pClosest = NULL;
	float closeDist = 999999999999.9f;

	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( i ) );

		if( !pPlayer )
		{
			continue;
		}

		if( !pPlayer->IsAlive() )
		{
			continue;
		}

		if( pPlayer == pIgnore )
		{
			continue;
		}

		if( team && pPlayer->GetTeamNumber() != team )
		{
			continue;
		}

		Vector position = pPlayer->GetAbsOrigin();
		float dist = vecPosition.DistTo( position );

		if( dist < closeDist )
		{
			closeDist = dist;
			pClosest  = pPlayer;
		}
	}

	if( distance )
	{
		*distance = closeDist;
	}

	return pClosest;
}

//-----------------------------------------------------------------------------
// Purpose: Returns if any player of the specified team is in the indicated range
//			near the indicated position
//-----------------------------------------------------------------------------
bool Utils::IsSpotOccupied( const Vector& vecPosition, CPlayer* pIgnore, float closeRange, int avoidTeam )
{
	float distance;
	CPlayer* pPlayer = Utils::GetClosestPlayer( vecPosition, &distance, pIgnore, avoidTeam );

	if( pPlayer && distance < closeRange )
	{
		return true;
	}

	return false;
}

CPlayer* Utils::GetClosestPlayerByClass( const Vector& vecPosition, Class_T classify, float* distance, CPlayer* pIgnore )
{
	CPlayer* pClosest = NULL;
	float closeDist = 999999999999.9f;

	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( i ) );

		if( !pPlayer )
		{
			continue;
		}

		if( !pPlayer->IsAlive() )
		{
			continue;
		}

		if( pPlayer == pIgnore )
		{
			continue;
		}

		if( pPlayer->Classify() != classify )
		{
			continue;
		}

		Vector position = pPlayer->GetAbsOrigin();
		float dist = vecPosition.DistTo( position );

		if( dist < closeDist )
		{
			closeDist = dist;
			pClosest  = pPlayer;
		}
	}

	if( distance )
	{
		*distance = closeDist;
	}

	return pClosest;
}

bool Utils::IsSpotOccupiedByClass( const Vector& vecPosition, Class_T classify, CPlayer* pIgnore, float closeRange )
{
	float distance;
	CPlayer* pPlayer = Utils::GetClosestPlayerByClass( vecPosition, classify, &distance, pIgnore );

	if( pPlayer && distance < closeRange )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns if any player is in the line of fire (FOV) of an exit point
//			to a destination
//-----------------------------------------------------------------------------
bool Utils::IsCrossingLineOfFire( const Vector& vecStart, const Vector& vecFinish, CPlayer* pIgnore, int ignoreTeam )
{
	for( int i = 1; i <= gpGlobals->maxClients; ++i )
	{
		CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( i ) );

		if( !pPlayer )
		{
			continue;
		}

		if( !pPlayer->IsAlive() )
		{
			continue;
		}

		if( pPlayer == pIgnore )
		{
			continue;
		}

		if( ignoreTeam && pPlayer->GetTeamNumber() == ignoreTeam )
		{
			continue;
		}

		// compute player's unit aiming vector
		Vector viewForward;
		AngleVectors( pPlayer->EyeAngles() + pPlayer->GetPunchAngle(), &viewForward );

		const float longRange = 5000.0f;
		Vector playerOrigin = pPlayer->GetAbsOrigin();
		Vector playerTarget = playerOrigin + longRange * viewForward;

		Vector result( 0, 0, 0 );

		if( IsIntersecting2D( vecStart, vecFinish, playerOrigin, playerTarget, &result ) )
		{
			// simple check to see if intersection lies in the Z range of the path
			float loZ, hiZ;

			if( vecStart.z < vecFinish.z )
			{
				loZ = vecStart.z;
				hiZ = vecFinish.z;
			}
			else
			{
				loZ = vecFinish.z;
				hiZ = vecStart.z;
			}

			if( result.z >= loZ && result.z <= hiZ + HumanHeight )
			{
				return true;
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns if position is valid using [criteria] filters
//-----------------------------------------------------------------------------
bool Utils::IsValidSpot( const Vector& vecSpot, const Vector& vecOrigin, const CSpotCriteria& criteria, CPlayer* pOwner )
{
	// Too far from our limit
	if( criteria.m_flMaxRange > 0 && vecOrigin.DistTo( vecSpot ) > criteria.m_flMaxRange )
	{
		return false;
	}

	// Is on the midle of the line of fire
	if( criteria.m_bOutOfLineOfFire && IsCrossingLineOfFire( vecOrigin, vecSpot, pOwner ) )
	{
		return false;
	}

	// Near team enemy
	if( criteria.m_iAvoidTeam && IsSpotOccupied( vecSpot, pOwner, criteria.m_flMinDistanceFromEnemy, criteria.m_iAvoidTeam ) )
	{
		return false;
	}

	if( criteria.m_iAvoidTeam && criteria.m_bOutOfVisibility )
	{
		for( int it = 0; it <= gpGlobals->maxClients; ++it )
		{
			CPlayer* pPlayer = ToInPlayer( UTIL_PlayerByIndex( it ) );

			if( !pPlayer )
			{
				continue;
			}

			if( !pPlayer->IsAlive() )
			{
				continue;
			}

			if( pPlayer->GetTeamNumber() != criteria.m_iAvoidTeam )
			{
				return false;
			}

			if( pPlayer->FVisible( vecSpot ) && pPlayer->IsInFieldOfView( vecSpot ) )
			{
				return false;
			}
		}
	}

	if( criteria.m_bOnlyVisible && pOwner )
	{
		if( !pOwner->FVisible( vecSpot ) || !pOwner->IsInFieldOfView( vecSpot ) )
		{
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a position to hide in a maximum range
//			Bots use it as a first option to hide from enemies.
//-----------------------------------------------------------------------------
bool Utils::FindNavCoverSpot( Vector* vecResult, const Vector& vecOrigin, const CSpotCriteria& criteria, CPlayer* pPlayer, SpotVector* list )
{
	if( vecResult )
	{
		vecResult->Invalidate();
	}

	CNavArea* pStartArea = TheNavMesh->GetNearestNavArea( vecOrigin );

	if( !pStartArea )
	{
		return false;
	}

	int hidingType = ( criteria.m_bIsSniper ) ? HidingSpot::IDEAL_SNIPER_SPOT : HidingSpot::IN_COVER;

	while( true )
	{
		CollectHidingSpotsFunctor collector( pPlayer, vecOrigin, criteria.m_flMaxRange, hidingType );
		SearchSurroundingAreas( pStartArea, vecOrigin, collector, criteria.m_flMaxRange );

		// Filters
		for( int i = 0; i < collector.m_count; ++i )
		{
			if( !IsValidSpot( *collector.m_hidingSpot[i], vecOrigin, criteria, pPlayer ) )
			{
				collector.RemoveSpot( i );
				--i;
				continue;
			}

			if( list )
			{
				Vector vec = *collector.m_hidingSpot[i];
				list->AddToTail( vec );
			}
		}

		if( !vecResult )
		{
			return false;
		}

		// We couldn't have find any
		if( collector.m_count == 0 )
		{
			// Try and hide us in a sniper spot
			if( hidingType == HidingSpot::IN_COVER )
			{
				hidingType = HidingSpot::IDEAL_SNIPER_SPOT;

				if( criteria.m_bIsSniper )
				{
					break;
				}
			}

			// Let's try a less suitable place
			if( hidingType == HidingSpot::IDEAL_SNIPER_SPOT )
			{
				hidingType = HidingSpot::GOOD_SNIPER_SPOT;
			}

			// There's not any spot near
			if( hidingType == HidingSpot::GOOD_SNIPER_SPOT )
			{
				hidingType = HidingSpot::IN_COVER;

				if( !criteria.m_bIsSniper )
				{
					break;
				}
			}
		}
		else
		{
			if( criteria.m_bUseNearest )
			{
				float closest = 9999999999999999999.0f;

				for( int it = 0; it < collector.m_count; ++it )
				{
					Vector vecDummy = *collector.m_hidingSpot[it];
					float distance = vecOrigin.DistTo( vecDummy );

					if( distance < closest )
					{
						closest = distance;
						*vecResult = vecDummy;
					}
				}

				return true;
			}
			else
			{
				int random = ( criteria.m_bUseRandom ) ? ::RandomInt( 0, collector.m_count - 1 ) : collector.GetRandomHidingSpot();
				*vecResult = *collector.m_hidingSpot[random];
				return true;
			}
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a position to hide within the indicated area
//			Bots use it as a last option to hide from enemies.
//-----------------------------------------------------------------------------
bool Utils::FindNavCoverSpotInArea( Vector* vecResult, const Vector& vecOrigin, CNavArea* pArea, const CSpotCriteria& criteria, CPlayer* pPlayer, SpotVector* list )
{
	if( vecResult )
	{
		vecResult->Invalidate();
	}

	if( !pArea )
	{
		return false;
	}

	CUtlVector<Vector> collector;

	for( int e = 0; e <= 15; ++e )
	{
		Vector position = pArea->GetRandomPoint();
		position.z += HumanCrouchHeight;

		if( !position.IsValid() )
		{
			continue;
		}

		if( collector.Find( position ) > -1 )
		{
			continue;
		}

		if( !IsValidSpot( position, vecOrigin, criteria, pPlayer ) )
		{
			continue;
		}

		collector.AddToTail( position );

		// Add it to our list
		if( list )
		{
			list->AddToTail( position );
		}
	}

	if( !vecResult )
	{
		return false;
	}

	if( collector.Count() > 0 )
	{
		if( criteria.m_bUseNearest )
		{
			float closest = 9999999999999999999.0f;

			for( int it = 0; it < collector.Count(); ++it )
			{
				Vector vecDummy = collector.Element( it );
				float distance = vecOrigin.DistTo( vecDummy );

				if( distance < closest )
				{
					closest = distance;
					*vecResult = vecDummy;
				}
			}

			return true;
		}
		else
		{
			int random = ( criteria.m_bUseRandom ) ? ::RandomInt( 0, collector.Count() - 1 ) : 0;
			*vecResult = collector.Element( random );

			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a position to hide using ai_hint (CAI_Hint)
//			Bots use it as a second option to hide from enemies.
//-----------------------------------------------------------------------------
CAI_Hint* Utils::FindHintSpot( const Vector& vecOrigin, const CHintCriteria& hintCriteria, const CSpotCriteria& criteria, CPlayer* pPlayer, SpotVector* list )
{
	CUtlVector<CAI_Hint*> collector;
	CAI_HintManager::FindAllHints( vecOrigin, hintCriteria, &collector );

	if( collector.Count() == 0 )
	{
		return NULL;
	}

	FOR_EACH_VEC( collector, it )
	{
		CAI_Hint* pHint = collector[it];

		if( !pHint )
		{
			continue;
		}

		Vector position = pHint->GetAbsOrigin();

		if( !IsValidSpot( position, vecOrigin, criteria, pPlayer ) )
		{
			collector.Remove( it );
			--it;
			continue;
		}

		if( list )
		{
			Vector vec = position;
			list->AddToTail( vec );
		}
	}

	if( collector.Count() > 0 )
	{
		if( criteria.m_bUseNearest )
		{
			float closest = 9999999999999999999.0f;
			CAI_Hint* pClosest = NULL;

			for( int it = 0; it < collector.Count(); ++it )
			{
				CAI_Hint* pDummy = collector[it];
				Vector vecDummy = pDummy->GetAbsOrigin();
				float distance = vecOrigin.DistTo( vecDummy );

				if( distance < closest )
				{
					closest = distance;
					pClosest = pDummy;
				}
			}

			return pClosest;
		}
		else
		{
			int random = ( criteria.m_bUseRandom ) ? ::RandomInt( 0, collector.Count() - 1 ) : 0;
			return collector[random];
		}
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: Return an interesting spot for the player
//-----------------------------------------------------------------------------
bool Utils::FindIntestingPosition( Vector* vecResult, CPlayer* pPlayer, const CSpotCriteria& criteria )
{
	Vector vecOrigin = pPlayer->GetAbsOrigin();

	if( criteria.m_vecOrigin.IsValid() )
	{
		vecOrigin = criteria.m_vecOrigin;
	}

	// Hints
	if( ::RandomInt( 0, 10 ) < 6 )
	{
		CHintCriteria hintCriteria;
		hintCriteria.AddHintType( HINT_WORLD_VISUALLY_INTERESTING );
		hintCriteria.AddHintType( HINT_WORLD_WINDOW );

		if( criteria.m_iTacticalMode == TACTICAL_MODE_STEALTH )
		{
			hintCriteria.AddHintType( HINT_WORLD_VISUALLY_INTERESTING_STEALTH );
		}

		if( criteria.m_iTacticalMode == TACTICAL_MODE_ASSAULT )
		{
			hintCriteria.AddHintType( HINT_TACTICAL_PINCH );
		}

		//if ( criteria.m_bUseNearest )
		//hintCriteria.SetFlag( bits_HINT_NODE_NEAREST );

		if( criteria.m_flMaxRange > 0 )
		{
			hintCriteria.AddIncludePosition( vecOrigin, criteria.m_flMaxRange );
		}

		CAI_Hint* pHint = FindHintSpot( vecOrigin, hintCriteria, criteria, pPlayer );

		if( pHint )
		{
			if( vecResult )
			{
				pHint->GetPosition( pPlayer, vecResult );
			}

			return true;
		}
	}

	// NavMesh
	if( FindNavCoverSpot( vecResult, vecOrigin, criteria, pPlayer ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Returns a position where the player can hide
//-----------------------------------------------------------------------------
bool Utils::FindCoverPosition( Vector* vecResult, CPlayer* pPlayer, const CSpotCriteria& criteria )
{
	Vector vecOrigin = pPlayer->GetAbsOrigin();

	if( criteria.m_vecOrigin.IsValid() )
	{
		vecOrigin = criteria.m_vecOrigin;
	}

	// NavMesh
	if( FindNavCoverSpot( vecResult, vecOrigin, criteria, pPlayer ) )
	{
		return true;
	}

	// Hints
	CHintCriteria hintCriteria;
	hintCriteria.AddHintType( HINT_TACTICAL_COVER_MED );
	hintCriteria.AddHintType( HINT_TACTICAL_COVER_LOW );

	CAI_Hint* pHint = FindHintSpot( vecOrigin, hintCriteria, criteria, pPlayer );

	if( pHint )
	{
		if( vecResult )
		{
			pHint->GetPosition( pPlayer, vecResult );
		}

		return true;
	}

	// Random Area Spot
	if( FindNavCoverSpotInArea( vecResult, vecOrigin, pPlayer->GetLastKnownArea(), criteria, pPlayer ) )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::FillIntestingPositions( SpotVector* list, CPlayer* pPlayer, const CSpotCriteria& criteria )
{
	Vector vecOrigin = pPlayer->GetAbsOrigin();

	if( criteria.m_vecOrigin.IsValid() )
	{
		vecOrigin = criteria.m_vecOrigin;
	}

	// Hints
	CHintCriteria hintCriteria;
	hintCriteria.AddHintType( HINT_WORLD_VISUALLY_INTERESTING );
	hintCriteria.AddHintType( HINT_WORLD_WINDOW );

	if( criteria.m_iTacticalMode == TACTICAL_MODE_STEALTH )
	{
		hintCriteria.AddHintType( HINT_WORLD_VISUALLY_INTERESTING_STEALTH );
	}

	if( criteria.m_iTacticalMode == TACTICAL_MODE_ASSAULT )
	{
		//hintCriteria.AddHintType( HINT_TACTICAL_ASSAULT_APPROACH );
		hintCriteria.AddHintType( HINT_TACTICAL_PINCH );
	}

	if( criteria.m_flMaxRange > 0 )
	{
		hintCriteria.AddIncludePosition( vecOrigin, criteria.m_flMaxRange );
	}

	FindHintSpot( vecOrigin, hintCriteria, criteria, pPlayer, list );

	// NavMesh
	//FindNavCoverSpot( NULL, vecOrigin, criteria, pPlayer, list );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void Utils::FillCoverPositions( SpotVector* list, CPlayer* pPlayer, const CSpotCriteria& criteria )
{
	Vector vecOrigin = pPlayer->GetAbsOrigin();

	if( criteria.m_vecOrigin.IsValid() )
	{
		vecOrigin = criteria.m_vecOrigin;
	}

	// NavMesh
	FindNavCoverSpot( NULL, vecOrigin, criteria, pPlayer, list );

	// Hints
	CHintCriteria hintCriteria;
	hintCriteria.AddHintType( HINT_TACTICAL_COVER_MED );
	hintCriteria.AddHintType( HINT_TACTICAL_COVER_LOW );

	FindHintSpot( vecOrigin, hintCriteria, criteria, pPlayer, list );

	// Random Area Spot
	FindNavCoverSpotInArea( NULL, vecOrigin, pPlayer->GetLastKnownArea(), criteria, pPlayer, list );
}