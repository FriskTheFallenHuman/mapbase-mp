//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifdef _WIN32
#include <windows.h>
#include <dbghelp.h>
#endif
#include "tier0/minidump.h"
#include "tools_minidump.h"

static bool g_bToolsWriteFullMinidumps = false;
static ToolsExceptionHandler g_pCustomExceptionHandler = NULL;


// --------------------------------------------------------------------------------- //
// Internal helpers.
// --------------------------------------------------------------------------------- //

static long __stdcall ToolsExceptionFilter( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
#ifdef _WIN32
	// Non VMPI workers write a minidump and show a crash dialog like normal.
	int iType = MiniDumpNormal;
	if ( g_bToolsWriteFullMinidumps )
		iType = MiniDumpWithDataSegs | MiniDumpWithIndirectlyReferencedMemory;
		
	WriteMiniDumpUsingExceptionInfo( ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo, (MINIDUMP_TYPE)iType );
	return EXCEPTION_CONTINUE_SEARCH;
#else
	return 0;
#endif
}


static long __stdcall ToolsExceptionFilter_Custom( struct _EXCEPTION_POINTERS *ExceptionInfo )
{
#ifdef _WIN32
	// Run their custom handler.
	g_pCustomExceptionHandler( ExceptionInfo->ExceptionRecord->ExceptionCode, ExceptionInfo );
	return EXCEPTION_EXECUTE_HANDLER; // (never gets here anyway)
#else
	return 0;
#endif
}


// --------------------------------------------------------------------------------- //
// Interface functions.
// --------------------------------------------------------------------------------- //

void EnableFullMinidumps( bool bFull )
{
	g_bToolsWriteFullMinidumps = bFull;
}


void SetupDefaultToolsMinidumpHandler()
{
#ifdef _WIN32
	SetUnhandledExceptionFilter( ToolsExceptionFilter );
#endif
}


void SetupToolsMinidumpHandler( ToolsExceptionHandler fn )
{
	g_pCustomExceptionHandler = fn;
#ifdef _WIN32
	SetUnhandledExceptionFilter( ToolsExceptionFilter_Custom );
#endif
}
