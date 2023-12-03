//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
// $NoKeywords: $
//=============================================================================//

#ifndef SDK_SCOREBOARD_H
#define SDK_SCOREBOARD_H
#ifdef _WIN32
	#pragma once
#endif

#include <clientscoreboarddialog.h>

//-----------------------------------------------------------------------------
// Purpose: Game ScoreBoard
//-----------------------------------------------------------------------------
class CSDKScoreboard : public CClientScoreBoardDialog
{
private:
	DECLARE_CLASS_SIMPLE( CSDKScoreboard, CClientScoreBoardDialog );

public:
	CSDKScoreboard( IViewPort* pViewPort );
	~CSDKScoreboard();

	virtual void Reset();
	virtual void Update();

	// vgui overrides for rounded corner background
	virtual void PaintBackground();
	virtual void PaintBorder();
	virtual void ApplySchemeSettings( vgui::IScheme* pScheme );

private:

	void UpdateItemVisibiity();
	void InitPlayerList( vgui::SectionedListPanel* pPlayerList, int teamNumber );
	void UpdateTeamInfo();
	void UpdatePlayerList();
	void UpdateSpectatorList();
	bool GetPlayerScoreInfo( int playerIndex, KeyValues* outPlayerInfo );

	bool ShouldShowAsSpectator( int iPlayerIndex );
	void FireGameEvent( IGameEvent* event );

	static bool SDKPlayerSortFunc( vgui::SectionedListPanel* list, int itemID1, int itemID2 );

	// rounded corners
	Color m_bgColor;
	Color m_borderColor;

	int m_iImageDead;

	int m_iStoredScoreboardWidth; // Store the full scoreboard width.

	// player lists
	vgui::SectionedListPanel* m_pPlayerListDM;	//Deathmatch Player list.
	vgui::SectionedListPanel* m_pPlayerListRed;
	vgui::SectionedListPanel* m_pPlayerListBlue;

	CPanelAnimationVarAliasType( int, m_iStatusWidth, "status_width", "35", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iFragsWidth, "frags_width", "30", "proportional_int" );

	vgui::Label*	m_pPlayerCountLabel_DM;
	vgui::Label* m_pScoreHeader_DM;
	vgui::Label*	m_pDeathsHeader_DM;
	vgui::Label*	m_pPingHeader_DM;
	vgui::Label*	m_pPingLabel_DM;

	vgui::Label*	m_pPlayerCountLabel_Red;
	vgui::Label*	m_pScoreHeader_Red;
	vgui::Label*	m_pScoreLabel_Red;
	vgui::Label*	m_pDeathsHeader_Red;
	vgui::Label*	m_pPingHeader_Red;
	vgui::Label*	m_pPingLabel_Red;

	vgui::Label*	m_pPlayerCountLabel_Blue;
	vgui::Label*	m_pScoreHeader_Blue;
	vgui::Label*	m_pScoreLabel_Blue;
	vgui::Label*	m_pDeathsHeader_Blue;
	vgui::Label*	m_pPingHeader_Blue;
	vgui::Label*	m_pPingLabel_Blue;

	// Create the vertical line so we can hide it in single column mode.
	vgui::ImagePanel* m_pVertLine;
};

#endif // SDK_SCOREBOARD_H
