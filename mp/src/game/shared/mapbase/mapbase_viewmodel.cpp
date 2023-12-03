//========= Mapbase MP - https://github.com/FriskTheFallenHuman/mapbase-mp ============//
//
// Purpose: A special variant of predicted_viewmodel with nethworked C_Hands
//
//          This is base of ZM:R zmr_viewmodel credits goes to the ZM:R
//          This was modified to derive from CPredictedViewModel and rmeoval
//          of the built-in bob
//
//=============================================================================//
#include "cbase.h"
#include "mapbase_viewmodel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( mapbase_viewmodel, CMapbaseViewModel );

IMPLEMENT_NETWORKCLASS_ALIASED( MapbaseViewModel, DT_Mapbase_ViewModel )

BEGIN_NETWORK_TABLE( CMapbaseViewModel, DT_Mapbase_ViewModel )
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
	BEGIN_PREDICTION_DATA( C_MapbaseViewModel )
	END_PREDICTION_DATA()
#endif

CMapbaseViewModel::CMapbaseViewModel()
{
#ifdef CLIENT_DLL
	m_bDrawVM = true;
	m_pLastWeapon = nullptr;
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CMapbaseViewModel::~CMapbaseViewModel()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBaseCombatWeapon* CMapbaseViewModel::GetOwningWeapon()
{
	auto* pOwner = CBaseViewModel::GetOwningWeapon();
	if( pOwner )
	{
		return pOwner;
	}

	// Arm viewmodel does not have an owning. Ask our brother.
	if( ViewModelIndex() == VMINDEX_HANDS )
	{
		auto* pPlayer = ThePlayerCast( GetOwner() );

		if( pPlayer )
		{
			CBaseViewModel* vm = pPlayer->GetViewModel( VMINDEX_WEP, false );

			// Apparently this is possible...
			// ???
			if( vm && vm->ViewModelIndex() == VMINDEX_WEP )
			{
				return vm->GetOwningWeapon();
			}
		}
	}

	return nullptr;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
TheBasePlayer* CMapbaseViewModel::GetOwner() const
{
	return ThePlayerCast( CBaseViewModel::GetOwner() );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
TheBaseWeapon* CMapbaseViewModel::GetWeapon() const
{
	return static_cast<TheBaseWeapon*>( CBaseViewModel::GetWeapon() );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int C_MapbaseViewModel::DrawModel( int flags )
{
	if( m_bDrawVM )
	{
		return BaseClass::DrawModel( flags );
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
C_BaseAnimating* C_MapbaseViewModel::FindFollowedEntity()
{
	if( ViewModelIndex() == VMINDEX_HANDS )
	{
		TheBasePlayer* pPlayer = ThePlayerCast( GetOwner() );

		if( pPlayer )
		{
			C_BaseViewModel* vm = pPlayer->GetViewModel( VMINDEX_WEP );
			if( vm )
			{
				return vm;
			}
		}
	}

	return C_BaseAnimating::FindFollowedEntity();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool C_MapbaseViewModel::ShouldPredict()
{
	if( GetOwner() && GetOwner() == TheBasePlayer::TheLocalPlayer )
	{
		return true;
	}

	return CBaseViewModel::ShouldPredict(); // Skip over to CBaseViewModel::ShouldPredict()
}
#endif