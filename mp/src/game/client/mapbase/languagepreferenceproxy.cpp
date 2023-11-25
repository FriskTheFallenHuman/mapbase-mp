//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Material proxy for changing a texture based on the user's language preference.
// 
// Example:
//	UnlitGeneric {
//		$basetexture "example/translated_texture_english"
//
//		Proxies {
//			LanguagePreference {
//				default "example/translated_texture_english"
//				schinese "example/translated_texture_schinese"
//				french "example/translated_texture_french"
//				resultvar "$basetexture"
//			}
//		}
//	}
// 
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "functionproxy.h"
#include "fmtstr.h"
#include "steam/steam_api.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#include "materialsystem/itexture.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CLanguagePreferenceProxy : public CResultProxy
{
public:
	bool Init( IMaterial *pMaterial, KeyValues *pKeyValues ) override;
	void OnBind( void *pC_BaseEntity ) override;

	ITexture *m_pTexture{};
};

bool CLanguagePreferenceProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	if ( !CResultProxy::Init( pMaterial, pKeyValues ) )
	{
		return false;
	}

	const char *szTextureName = pKeyValues->GetString( "default", NULL );
	if( steamapicontext && steamapicontext->SteamApps() )
	{
		szTextureName = pKeyValues->GetString( steamapicontext->SteamApps()->GetCurrentGameLanguage(), szTextureName );
	}

	if ( !szTextureName )
	{
		Warning( "No default or language-specific texture for LanguagePreference proxy in %s\n", pMaterial->GetName() );
		return false;
	}

	m_pTexture = materials->FindTexture( szTextureName, pMaterial->GetTextureGroupName() );

	return !IsErrorTexture( m_pTexture );
}

void CLanguagePreferenceProxy::OnBind( void *pC_BaseEntity )
{
	Assert( m_pResult && m_pTexture );
	Assert( m_pResult->GetType() == MATERIAL_VAR_TYPE_TEXTURE );

	m_pResult->SetTextureValue( m_pTexture );
}

EXPOSE_INTERFACE( CLanguagePreferenceProxy, IMaterialProxy, "LanguagePreference" IMATERIAL_PROXY_INTERFACE_VERSION );
