//========= Mapbase - https://github.com/mapbase-source/source-sdk-2013 ============//
//
// Purpose: VScript functions.
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#if defined( HL2_DLL ) || defined( HL2_CLIENT_DLL )
	#include "hl2_gamerules.h"
	#define TheGameRules	TheGameRules
#else
	#include "sdk_gamerules.h"
	#define TheGameRules	SDKGameRules()
#endif // HL2_DLL || HL2_CLIENT_DLL

#ifndef CLIENT_DLL
	#include "eventqueue.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined( GAME_DLL )
class CScriptConvars : public CAutoGameSystem
{
public:
	void LevelInitPreEntity() OVERRIDE
	{
		m_AllowList.RemoveAll();

		KeyValuesAD kvAllowList( "vscript_convar_allowlist" );
		if( kvAllowList->LoadFromFile( g_pFullFileSystem, "cfg/vscript_convar_allowlist.txt", "GAME" ) )
		{
			FOR_EACH_SUBKEY( kvAllowList, pSubKey )
			{
				if( !V_stricmp( pSubKey->GetString(), "allowed" ) )
				{
					m_AllowList.AddString( pSubKey->GetName() );
				}
			}
		}
	}

	ScriptVariant_t GetClientConvarValue( int clientIndex, const char* name )
	{
		const char* cvar = engine->GetClientConVarValue( clientIndex, name );
		if( cvar )
		{
			return ScriptVariant_t( cvar, true );
		}
		return SCRIPT_VARIANT_NULL;
	}

	ScriptVariant_t GetStr( const char* name )
	{
		ConVarRef cvar( name );
		if( cvar.IsValid() )
		{
			return ScriptVariant_t( cvar.GetString(), true );
		}
		return SCRIPT_VARIANT_NULL;
	}

	ScriptVariant_t GetFloat( const char* name )
	{
		ConVarRef cvar( name );
		if( cvar.IsValid() )
		{
			return ScriptVariant_t( cvar.GetFloat() );
		}
		return SCRIPT_VARIANT_NULL;
	}

	ScriptVariant_t GetInt( const char* name )
	{
		ConVarRef cvar( name );
		if( cvar.IsValid() )
		{
			return ScriptVariant_t( cvar.GetInt() );
		}
		return SCRIPT_VARIANT_NULL;
	}

	ScriptVariant_t GetBool( const char* name )
	{
		ConVarRef cvar( name );
		if( cvar.IsValid() )
		{
			return ScriptVariant_t( cvar.GetBool() );
		}
		return SCRIPT_VARIANT_NULL;
	}

	void SetValue( const char* name, ScriptVariant_t value )
	{
		ConVarRef cvar( name );
		if( !cvar.IsValid() )
		{
			return;
		}

		switch( value.m_type )
		{
			case FIELD_INTEGER:
			{
				cvar.SetValue( value.m_int );

				TheGameRules->SaveConvar( cvar );
				break;
			}
			case FIELD_BOOLEAN:
			{
				cvar.SetValue( value.m_bool );

				TheGameRules->SaveConvar( cvar );
				break;
			}
			case FIELD_FLOAT:
			{
				cvar.SetValue( value.m_float );

				TheGameRules->SaveConvar( cvar );
				break;
			}
			case FIELD_CSTRING:
			{
				cvar.SetValue( value.m_pszString );

				TheGameRules->SaveConvar( cvar );
				break;
			}
			default:
			{
				Warning( "%s.SetValue() unsupported value type %s\n", name, ScriptFieldTypeName( value.m_type ) );
				break;
			}
		}
	}

	bool IsConVarOnAllowList( char const* name )
	{
		CUtlSymbol sym = m_AllowList.Find( name );
		return sym.IsValid();
	}

private:
	CUtlSymbolTable m_AllowList;
} g_ConvarsVScript;

BEGIN_SCRIPTDESC_ROOT_NAMED( CScriptConvars, "Convars", SCRIPT_SINGLETON "Provides an interface for getting and setting convars on the server." )
DEFINE_SCRIPTFUNC( GetClientConvarValue, "Returns the convar value for the entindex as a string. Only works with client convars with the FCVAR_USERINFO flag." )
DEFINE_SCRIPTFUNC( GetStr, "Returns the convar as a string. May return null if no such convar." )
DEFINE_SCRIPTFUNC( GetFloat, "Returns the convar as a float. May return null if no such convar." )
DEFINE_SCRIPTFUNC( GetInt, "Returns the convar as an integer. May return null if no such convar." )
DEFINE_SCRIPTFUNC( GetBool, "Returns the convar as a boolean. May return null if no such convar." )
DEFINE_SCRIPTFUNC( SetValue, "Sets the value of the convar. The convar must be in cfg/vscript_convar_allowlist.txt to be set. Supported types are bool, int, float, string." )
DEFINE_SCRIPTFUNC( IsConVarOnAllowList, "Checks if the convar is allowed to be used and is in cfg/vscript_convar_allowlist.txt." )
END_SCRIPTDESC();
#endif // GAME_DLL

static float AttribHookValueFloat( float value, char const* szName, HSCRIPT hEntity )
{
	CBaseEntity* pEntity = ToEnt( hEntity );
	if( !pEntity )
	{
		return value;
	}

	return value;
}

static int AttribHookValueInt( int value, char const* szName, HSCRIPT hEntity )
{
	CBaseEntity* pEntity = ToEnt( hEntity );
	if( !pEntity )
	{
		return value;
	}

	return value;
}

#if defined( GAME_DLL )
extern CBaseEntity* CreatePlayerLoadSave( Vector vOrigin, float flDuration, float flHoldTime, float flLoadTime );

HSCRIPT ScriptGameOver( const char* pszMessage, float flDelay, float flFadeTime, float flLoadTime, int r, int g, int b )
{
	CBaseEntity* pPlayer = AI_GetSinglePlayer();
	if( pPlayer )
	{
		UTIL_ShowMessage( pszMessage, ToBasePlayer( pPlayer ) );
		ToBasePlayer( pPlayer )->NotifySinglePlayerGameEnding();
	}
	else
	{
		// TODO: How should MP handle this?
		return NULL;
	}

	CBaseEntity* pReload = CreatePlayerLoadSave( vec3_origin, flFadeTime, flLoadTime + 1.0f, flLoadTime );
	if( pReload )
	{
		pReload->SetRenderColor( r, g, b, 255 );
		g_EventQueue.AddEvent( pReload, "Reload", flDelay, pReload, pReload );
	}

	return ToHScript( pReload );
}

#ifdef HL2_DLL
bool ScriptMegaPhyscannonActive()
{
	return TheGameRules->MegaPhyscannonActive();
}
#endif // HL2_DLL
#endif // GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void RegisterGameScriptFunctions( void )
{
#ifndef CLIENT_DLL
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptGameOver, "GameOver", "Ends the game and reloads the last save." );
#ifdef HL2_DLL
	ScriptRegisterFunctionNamed( g_pScriptVM, ScriptMegaPhyscannonActive, "MegaPhyscannonActive", "Checks if supercharged gravity gun mode is enabled." );
#endif // HL2_DLL
#endif

#ifdef MAPBASE_MP
	ScriptRegisterFunctionNamed( g_pScriptVM, AttribHookValueFloat, "GetAttribFloatValue", "Fetch an attribute that is assigned to the provided weapon" );
	ScriptRegisterFunctionNamed( g_pScriptVM, AttribHookValueInt, "GetAttribIntValue", "Fetch an attribute that is assigned to the provided weapon" );

#if defined( GAME_DLL )
	g_pScriptVM->RegisterInstance( &g_ConvarsVScript, "Convars" );
#endif
#endif // MAPBASE_MP
}
