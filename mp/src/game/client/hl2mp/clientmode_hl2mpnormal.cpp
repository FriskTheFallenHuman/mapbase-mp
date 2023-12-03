//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws the normal TF2 or HL2 HUD.
//
// $Workfile:     $
// $Date:         $
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "clientmode_hl2mpnormal.h"
#include "vgui_int.h"
#include "hud.h"
#include "cdll_client_int.h"
#include "iinput.h"
#include <vgui/IInput.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/AnimationController.h>
#include "iinput.h"
#include "filesystem.h"
#include "hud_basechat.h"
#include "view_shared.h"
#include "view.h"
#include "ivrenderview.h"
#include "model_types.h"
#include "iefx.h"
#include "dlight.h"
#include <imapoverview.h>
#include "c_playerresource.h"
#include <KeyValues.h>
#include "text_message.h"
#include "panelmetaclassmgr.h"
#include "hl2mpclientscoreboard.h"
#include "hl2mptextwindow.h"
#include "buymenu.h"
#include "c_baseplayer.h"
#include "c_weapon__stubs.h"		//Tony; add stubs
#ifdef MAPBASE
	#include "cam_thirdperson.h"
#endif // MAPBASE

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern bool g_bRollingCredits;

class CHudChat;

IClientMode* g_pClientMode = NULL;

//Tony; add stubs for cycler weapon and cubemap.
STUB_WEAPON_CLASS( cycler_weapon,   WeaponCycler,   C_BaseCombatWeapon );
STUB_WEAPON_CLASS( weapon_cubemap,  WeaponCubemap,  C_BaseCombatWeapon );

// default FOV for HL2
ConVar default_fov( "default_fov", "75", FCVAR_CHEAT );

ConVar fov_desired( "fov_desired", "75", FCVAR_ARCHIVE | FCVAR_USERINFO, "Sets the base field-of-view.", true, 75.0, true, MAX_FOV );

#define SCREEN_FILE		"scripts/vgui_screens.txt"

//-----------------------------------------------------------------------------
// Globals
//-----------------------------------------------------------------------------
vgui::HScheme g_hVGuiCombineScheme = 0;

//-----------------------------------------------------------------------------
// HACK: the detail sway convars are archive, and default to 0.  Existing CS:S players thus have no detail
// prop sway.  We'll force them to DoD's default values for now.  What we really need in the long run is
// a system to apply changes to archived convars' defaults to existing players.
extern ConVar cl_detail_max_sway;
extern ConVar cl_detail_avoid_radius;
extern ConVar cl_detail_avoid_force;
extern ConVar cl_detail_avoid_recover_speed;

// --------------------------------------------------------------------------------- //
// CHL2MPModeManager implementation.
// --------------------------------------------------------------------------------- //
void CHL2MPModeManager::Init()
{
	g_pClientMode = GetClientModeNormal();

	PanelMetaClassMgr()->LoadMetaClassDefinitionFile( SCREEN_FILE );
}

void CHL2MPModeManager::LevelInit( const char* newmap )
{
	g_pClientMode->LevelInit( newmap );

	// HACK: the detail sway convars are archive, and default to 0.  Existing CS:S players thus have no detail
	// prop sway.  We'll force them to DoD's default values for now.
	if( !cl_detail_max_sway.GetFloat() &&
			!cl_detail_avoid_radius.GetFloat() &&
			!cl_detail_avoid_force.GetFloat() &&
			!cl_detail_avoid_recover_speed.GetFloat() )
	{
		cl_detail_max_sway.SetValue( "5" );
		cl_detail_avoid_radius.SetValue( "64" );
		cl_detail_avoid_force.SetValue( "0.4" );
		cl_detail_avoid_recover_speed.SetValue( "0.25" );
	}

	ConVarRef voice_steal( "voice_steal" );
	if( voice_steal.IsValid() )
	{
		voice_steal.SetValue( 1 );
	}

#ifdef MAPBASE
	g_ThirdPersonManager.Init();
#endif // MAPBASE
}

void CHL2MPModeManager::LevelShutdown( void )
{
	g_pClientMode->LevelShutdown();
}

static CHL2MPModeManager g_ModeManager;
IVModeManager* modemanager = ( IVModeManager* )& g_ModeManager;

// --------------------------------------------------------------------------------- //
// CHudViewport implementation.
// --------------------------------------------------------------------------------- //
IViewPortPanel* CHudViewport::CreatePanelByName( const char* szPanelName )
{
	IViewPortPanel* newpanel = NULL;

	if( Q_strcmp( PANEL_SCOREBOARD, szPanelName ) == 0 )
	{
		newpanel = new CHL2MPClientScoreBoardDialog( this );
		return newpanel;
	}
	else if( Q_strcmp( PANEL_INFO, szPanelName ) == 0 )
	{
		newpanel = new CHL2MPTextWindow( this );
		return newpanel;
	}
	else if( Q_strcmp( PANEL_SPECGUI, szPanelName ) == 0 )
	{
		newpanel = new CHL2MPSpectatorGUI( this );
		return newpanel;
	}


	return BaseClass::CreatePanelByName( szPanelName );
}

//-----------------------------------------------------------------------------
// ClientModeHLNormal implementation
//-----------------------------------------------------------------------------
ClientModeHL2MPNormal::ClientModeHL2MPNormal()
{
	m_pViewport = new CHudViewport();
	m_pViewport->Start( gameuifuncs, gameeventmanager );
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
ClientModeHL2MPNormal::~ClientModeHL2MPNormal()
{
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ClientModeHL2MPNormal::Init()
{
	BaseClass::Init();

	// Load up the combine control panel scheme
	g_hVGuiCombineScheme = vgui::scheme()->LoadSchemeFromFileEx( enginevgui->GetPanel( PANEL_CLIENTDLL ), "resource/CombinePanelScheme.res", "CombineScheme" );
	if( !g_hVGuiCombineScheme )
	{
		Warning( "Couldn't load combine panel scheme!\n" );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int ClientModeHL2MPNormal::GetDeathMessageStartHeight( void )
{
	return m_pViewport->GetDeathMessageStartHeight();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ClientModeHL2MPNormal::PostRenderVGui()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool ClientModeHL2MPNormal::CanRecordDemo( char* errorMsg, int length ) const
{
	C_BasePlayer* player = C_BasePlayer::GetLocalPlayer();
	if( !player )
	{
		return true;
	}

	if( !player->IsAlive() )
	{
		return true;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool ClientModeHL2MPNormal::ShouldDrawCrosshair( void )
{
	return ( g_bRollingCredits == false );
}

#ifdef MAPBASE
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void ClientModeHL2MPNormal::OverrideView( CViewSetup* pSetup )
{
	QAngle camAngles;

	// Let the player override the view.
	C_BasePlayer* pPlayer = C_BasePlayer::GetLocalPlayer();
	if( !pPlayer )
	{
		return;
	}

	pPlayer->OverrideView( pSetup );

	if( ::input->CAM_IsThirdPerson() )
	{
		const Vector& cam_ofs = g_ThirdPersonManager.GetCameraOffsetAngles();
		Vector cam_ofs_distance;

		if( g_ThirdPersonManager.IsOverridingThirdPerson() )
		{
			cam_ofs_distance = g_ThirdPersonManager.GetDesiredCameraOffset();
		}
		else
		{
			cam_ofs_distance = g_ThirdPersonManager.GetFinalCameraOffset();
		}

		cam_ofs_distance *= g_ThirdPersonManager.GetDistanceFraction();

		camAngles[ PITCH ] = cam_ofs[ PITCH ];
		camAngles[ YAW ] = cam_ofs[ YAW ];
		camAngles[ ROLL ] = 0;

		Vector camForward, camRight, camUp;


		if( g_ThirdPersonManager.IsOverridingThirdPerson() == false )
		{
			engine->GetViewAngles( camAngles );
		}

		// get the forward vector
		AngleVectors( camAngles, &camForward, &camRight, &camUp );

		VectorMA( pSetup->origin, -cam_ofs_distance[0], camForward, pSetup->origin );
		VectorMA( pSetup->origin, cam_ofs_distance[1], camRight, pSetup->origin );
		VectorMA( pSetup->origin, cam_ofs_distance[2], camUp, pSetup->origin );

		// Override angles from third person camera
		VectorCopy( camAngles, pSetup->angles );
	}
	else if( ::input->CAM_IsOrthographic() )
	{
		pSetup->m_bOrtho = true;
		float w, h;
		::input->CAM_OrthographicSize( w, h );
		w *= 0.5f;
		h *= 0.5f;
		pSetup->m_OrthoLeft   = -w;
		pSetup->m_OrthoTop    = -h;
		pSetup->m_OrthoRight  = w;
		pSetup->m_OrthoBottom = h;
	}
}
#endif // MAPBASE

// Instance the singleton and expose the interface to it.
IClientMode* GetClientModeNormal()
{
	static ClientModeHL2MPNormal g_ClientModeNormal;
	return &g_ClientModeNormal;
}

ClientModeHL2MPNormal* GetClientModeHL2MPNormal()
{
	Assert( dynamic_cast< ClientModeHL2MPNormal* >( GetClientModeNormal() ) );

	return static_cast< ClientModeHL2MPNormal* >( GetClientModeNormal() );
}