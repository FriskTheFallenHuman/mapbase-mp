@echo off
setlocal ENABLEDELAYEDEXPANSION

rem ================================
rem ==== STEAM PATH LOCALIZATION ===

rem Getting Steam path from registry
rem for /F "usebackq tokens=2*" %%I in (`reg query "HKCU\Software\Valve\Steam"^|find /I "SteamPath"`) do set steampath=%%J
for /f "usebackq tokens=1,2,*" %%i in (`reg query "HKCU\Software\Valve\Steam" /v "SteamPath"`) do set "steampath=%%~k"
rem Since this is loop, we cannot use ERRORLEVEL here
rem if not ERRORLEVEL 0 goto error

rem Replacing "/"'s with "\" in some cases
set steampath=%steampath:/=\%

rem Testing common paths
if not exist "%steampath%\steam.exe" (
	if not exist "%ProgramFiles(x86)%\steam.exe" (
		if not exist "%ProgramFiles%\steam.exe" (
			goto error
		) else (
			set steampath=%ProgramFiles%
		)
	) else set steampath=%ProgramFiles(x86)%
)

rem ==== STEAM PATH LOCALIZATION END ===
rem ====================================

rem ================================
rem ==== MOD PATH CONFIGURATIONS ===

rem == Set the absolute path to your mod's game directory here ==
set GAMEDIR=%cd%\..\..\..\game\mapbase_scratch

rem == Set the relative or absolute path to Source SDK Base 2013 Multiplayer\bin ==
set SDKBINDIR=%steampath%\steamapps\common\Source SDK Base 2013 Multiplayer\bin

rem ==  Set the Path to your mod's root source code ==
rem This should already be correct, accepts relative paths only!
set SOURCEDIR=..\..

rem ==== MOD PATH CONFIGURATIONS END ===
rem ====================================

call _buildsdkshaders.bat
call _buildincludes.bat _lux_shaders_list        -game %GAMEDIR% -source %SOURCEDIR% -force30
pause
