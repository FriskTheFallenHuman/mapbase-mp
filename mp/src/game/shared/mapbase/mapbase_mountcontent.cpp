//========= Mapbase MP - https://github.com/FriskTheFallenHuman/mapbase-mp ============//
//
// Purpose: This is used for the content mounting used in the Game
// 
//          This is the work of Chaos Team/Momentum Team all credits goes to them.
//
//=============================================================================//

#include "cbase.h"
#include "mapbase_mountcontent.h"
#include "filesystem.h"
#include "steam/steam_api.h"
#include "fmtstr.h"

#include "tier0/memdbgon.h"

extern IFileSystem* filesystem;

static void MountGames( bool& gotHL2, bool& gotHL2MP, bool& gotEP1, bool& gotEP2 )
{
    KeyValuesAD gameinfo( "GameInfo" );
    gameinfo->LoadFromFile( filesystem, "gameinfo.txt" );
    KeyValues* pMounts = gameinfo->FindKey( "mount" );
    if ( !pMounts )
        return;

    char path[MAX_PATH];
    CUtlVector<CUtlString> dirs;

    FOR_EACH_TRUE_SUBKEY( pMounts, pMount )
    {
        const auto appId = static_cast<AppId_t>( V_atoui64( pMount->GetName() ) );
        if ( !steamapicontext->SteamApps()->BIsAppInstalled( appId ) )
        {
            CGWarning( 1, CON_GROUP_MAPBASE_MISC, "AppId %lu not installed!\n", appId);
            continue;
        }

        gotHL2 |= appId == 220;
        gotHL2MP |= appId == 320;
        gotEP1 |= appId == 380;
        gotEP2 |= appId == 420;

        steamapicontext->SteamApps()->GetAppInstallDir( appId, path, sizeof( path ) );

        const SearchPathAdd_t head = pMount->GetBool("head") ? PATH_ADD_TO_HEAD : PATH_ADD_TO_TAIL;

        FOR_EACH_TRUE_SUBKEY( pMount, pModDir )
        {
            const char* const modDir = pModDir->GetName();
            const CFmtStr mod( "%s" CORRECT_PATH_SEPARATOR_S "%s", path, modDir );
            dirs.AddToTail( mod.Get() );

            FOR_EACH_VALUE( pModDir, pPath )
            {
                const char* const keyName = pPath->GetName();
                if ( FStrEq( keyName, "vpk" ) )
                {
                    const CFmtStr file( "%s" CORRECT_PATH_SEPARATOR_S "%s.vpk", mod.Get(), pPath->GetString() );
                    filesystem->AddSearchPath( file, "GAME", head );
                }
                else if ( FStrEq( keyName, "dir" ) )
                {
                    const CFmtStr folder( "%s" CORRECT_PATH_SEPARATOR_S "%s", mod.Get(), pPath->GetString() );
                    filesystem->AddSearchPath( folder, "GAME", head );
                }
                else
                    CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Unknown key \"%s\" in mounts\n", keyName );
            }
        }
    }
    for ( const auto& dir : dirs )
    {
        g_pFullFileSystem->AddSearchPath( dir, "GAME" );
    }
}

void MapBaseContent::MountGameFiles()
{
    if ( steamapicontext->SteamApps() )
    {
        bool gotHL2 = false, gotHL2MP = false, gotEP1 = false, gotEP2 = false;
        MountGames( gotHL2, gotHL2MP, gotEP1, gotEP2 );

        char installPath[MAX_PATH];
        uint32 folderLen;

        // Half Life 2
        if ( !gotHL2 )
        {
            bool bOwnHL2 = steamapicontext->SteamApps()->BIsSubscribedApp( 240 );
            if ( !bOwnHL2 ) 
                CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Using Source SDK 2013 MP HL2 Content!\n" );

            folderLen = steamapicontext->SteamApps()->GetAppInstallDir( bOwnHL2 ? 240 : 243750, installPath, MAX_PATH );

            if ( folderLen )
            {
                filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_lv.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_sound_vo_english.vpk", installPath ), "GAME" );
                // Only Half Life 2 has this vpk!
                if ( bOwnHL2 )
                    filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_pak.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_textures.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_sound_misc.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2/hl2_misc.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2", installPath ), "GAME" );
            }
        }
        else
        {
            CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Overridden default HL2 mount logic!\n" );
        }

#ifdef HL2MP
        // Half Life 2 Deathmatch
        if ( !gotHL2MP )
        {
            bool bOwnHL2MP = steamapicontext->SteamApps()->BIsSubscribedApp( 320 );
            if ( !bOwnHL2MP ) 
                CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Using Source SDK 2013 MP HL2MP Content!\n" );

            folderLen = steamapicontext->SteamApps()->GetAppInstallDir( bOwnHL2MP ? 320 : 243750, installPath, MAX_PATH );

            if (folderLen)
            {
                filesystem->AddSearchPath( CFmtStr( "%s/hl2mp/hl2mp_pak.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2mp/download", installPath ), "GAME");
                filesystem->AddSearchPath( CFmtStr( "%s/hl2mp/download", installPath ), "download" );
                filesystem->AddSearchPath( CFmtStr( "%s/hl2mp", installPath ), "GAME" );
            }
        }
        else
        {
            CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Overridden default HL2MP mount logic!\n" );
        }
#endif // HL2MP

#ifdef HL2_EPISODIC
        // Half Life 2: Episode One
        if ( !gotEP1 )
        {
            bool bOwnHL2 = steamapicontext->SteamApps()->BIsSubscribedApp( 380 );
            if ( !bOwnHL2 ) 
                CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Using Source SDK 2013 MP HL2:EP1 Content!\n" );

            folderLen = steamapicontext->SteamApps()->GetAppInstallDir( bOwnHL2 ? 380 : 243730, installPath, MAX_PATH );

            if ( folderLen )
            {
                filesystem->AddSearchPath( CFmtStr( "%s/episodic/ep1_pak.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/episodic", installPath ), "GAME" );
            }
        }
        else
        {
            CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Overridden default HL2:EP1 mount logic!\n" );
        }

        // Half Life 2: Episode Two
        if ( !gotEP2)
        {
            bool bOwnHL2 = steamapicontext->SteamApps()->BIsSubscribedApp( 420 );
            if ( !bOwnHL2 ) 
                CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Using Source SDK 2013 MP HL2:EP2 Content!\n" );

            folderLen = steamapicontext->SteamApps()->GetAppInstallDir( bOwnHL2 ? 420 : 243730, installPath, MAX_PATH );

            if ( folderLen )
            {
                filesystem->AddSearchPath( CFmtStr( "%s/ep2/ep2_pak.vpk", installPath ), "GAME" );
                filesystem->AddSearchPath( CFmtStr( "%s/ep2", installPath ), "GAME" );
            }
        }
        else
        {
            CGWarning( 1, CON_GROUP_MAPBASE_MISC, "Overridden default HL2:EP2 mount logic!\n" );
        }
#endif // HL2_EPISODIC

        if ( developer.GetInt() )
            filesystem->PrintSearchPaths();
    }
}