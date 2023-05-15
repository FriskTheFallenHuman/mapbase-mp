//========= Mapbase MP - https://github.com/FriskTheFallenHuman/mapbase-mp ============//
//
// Purpose: A special variant of predicted_viewmodel with nethworked C_Hands +
//          Poseparameters base viewbob.
//
//          This is base of ZM:R zmr_viewmodel credits goes to the ZM:R
//
//=============================================================================//
#ifndef MAPBASE_VIEWMODEL_H
#define MAPBASE_VIEWMODEL_H
#ifdef _WIN32
	#pragma once
#endif

#ifdef CLIENT_DLL
	#define CMapbaseViewModel C_MapbaseViewModel
#endif

#include "predicted_viewmodel.h"
#include "weapon_hl2mpbase.h"

#ifdef CLIENT_DLL
	#include "c_hl2mp_player.h"
#else
	#include "hl2mp_player.h"
#endif

//
// Viewmodel Indeces
//
#define VMINDEX_WEP         0
#define VMINDEX_HANDS       1

class CMapbaseViewModel : public CPredictedViewModel
{
public:
	DECLARE_CLASS( CMapbaseViewModel, CPredictedViewModel );
	DECLARE_NETWORKCLASS();
#ifdef CLIENT_DLL
	DECLARE_PREDICTABLE();
#endif

	CMapbaseViewModel();
	virtual ~CMapbaseViewModel();

#ifdef CLIENT_DLL
	virtual int	DrawModel( int flags );
	virtual C_BaseAnimating*	FindFollowedEntity() OVERRIDE;
	virtual bool	ShouldPredict() OVERRIDE;

	CWeaponHL2MPBase* GetWeapon() const;
#endif

	virtual CHL2MP_Player* GetOwner() const;
	virtual CBaseCombatWeapon* GetOwningWeapon();

#ifdef CLIENT_DLL
	virtual void SetDrawVM( bool state )
	{
		m_bDrawVM = state;
	};
#endif

private:

#ifdef CLIENT_DLL
	bool m_bDrawVM; // We have to override this so the client can decide whether to draw it.

	CBaseCombatWeapon* m_pLastWeapon;
#endif
};

#endif // MAPBASE_VIEWMODEL_H
