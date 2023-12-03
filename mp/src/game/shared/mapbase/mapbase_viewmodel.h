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
#ifdef HL2MP
	#include "weapon_hl2mpbase.h"
#else
	#include "weapon_sdkbase.h"
#endif // HL2MP

#ifdef CLIENT_DLL
	#ifdef HL2MP
		#include "c_hl2mp_player.h"
	#else
		#include "c_sdk_player.h"
	#endif // HL2MP
#else
	#ifdef HL2MP
		#include "hl2mp_player.h"
	#else
		#include "sdk_player.h"
	#endif // HL2MP
#endif

#ifdef HL2MP
	#define TheBaseWeapon CWeaponHL2MPBase
	#define TheBasePlayer CHL2MP_Player
	#define ThePlayerCast ToHL2MPPlayer
	#define TheLocalPlayer GetLocalPlayer()
#else
	#define TheBaseWeapon CWeaponSDKBase
	#define TheBasePlayer CSDKPlayer
	#define ThePlayerCast ToSDKPlayer
	#define TheLocalPlayer GetLocalSDKPlayer()
#endif // HL2MP

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

	TheBaseWeapon* GetWeapon() const;
#endif

	virtual TheBasePlayer* GetOwner() const;
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
