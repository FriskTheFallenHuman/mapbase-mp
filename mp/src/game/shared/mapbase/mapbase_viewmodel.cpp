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
    if ( pOwner )
        return pOwner;

    // Arm viewmodel does not have an owning. Ask our brother.
    if ( ViewModelIndex() == VMINDEX_HANDS )
    {
        auto* pPlayer = ToHL2MPPlayer( GetOwner() );

        if ( pPlayer )
        {
            CBaseViewModel* vm = pPlayer->GetViewModel( VMINDEX_WEP, false );

            // Apparently this is possible...
            // ???
            if ( vm && vm->ViewModelIndex() == VMINDEX_WEP )
                return vm->GetOwningWeapon();
        }
    }

    return nullptr;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHL2MP_Player* CMapbaseViewModel::GetOwner() const
{
    return ToHL2MPPlayer( CBaseViewModel::GetOwner() );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponHL2MPBase* CMapbaseViewModel::GetWeapon() const
{
    return static_cast<CWeaponHL2MPBase*>( CBaseViewModel::GetWeapon() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_MapbaseViewModel::DrawModel( int flags )
{
    if ( m_bDrawVM )
        return BaseClass::DrawModel( flags );

    return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BaseAnimating* C_MapbaseViewModel::FindFollowedEntity()
{
    if ( ViewModelIndex() == VMINDEX_HANDS )
    {
        CHL2MP_Player* pPlayer = ToHL2MPPlayer( GetOwner() );

        if ( pPlayer )
        {
            C_BaseViewModel* vm = pPlayer->GetViewModel( VMINDEX_WEP );
            if ( vm )
                return vm;
        }
    }

    return C_BaseAnimating::FindFollowedEntity();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_MapbaseViewModel::ShouldPredict()
{
    if ( GetOwner() && GetOwner() == CHL2MP_Player::GetLocalPlayer() )
        return true;
    
    return CBaseViewModel::ShouldPredict(); // Skip over to CBaseViewModel::ShouldPredict()
}
#endif