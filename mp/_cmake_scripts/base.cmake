# base.cmake

# NOTE: We use 0 or 1 so we can use these more easily in generator expressions
# Initialize them with default values that we then set later

set(IS_WINDOWS 0)
set(IS_LINUX 0)
set(IS_OSX 0)
set(IS_POSIX 0)

set(IS_XCODE 0)
set(IS_SOURCESDK 1)

if (WIN32)
	set(IS_WINDOWS 1)
endif()

if (UNIX AND NOT APPLE)
	set(IS_LINUX 1)
endif()

if (APPLE)
	set(IS_OSX 1)
	if (${CMAKE_GENERATOR} STREQUAL "Xcode")
		set(IS_XCODE 1)
	endif()
endif()

if (UNIX)
	set(IS_POSIX 1)
endif()

if (${IS_WINDOWS})
	set(_DLL_EXT ".dll")
	set(STATIC_LIB_EXT ".lib")
	set(IMPLIB_EXT ".lib")
elseif (${IS_OSX})
	set(_DLL_EXT ".dylib")
	set(STATIC_LIB_EXT ".a")
	set(IMPLIB_EXT ".dylib")
elseif(${IS_LINUX})
	set(_DLL_EXT ".so")
	set(STATIC_LIB_EXT ".a")
	set(IMPLIB_EXT ".so")
endif()

# Mapbase only settings
option(MAPBASE "Build the game code with Mapbase features enable" ON)
option(MAPBASE_MP "Build the game code with extra features tailored for Multiplayer" ON)
option(MAPBASE_RPC "Build the game code with Mapbase's Rich Presence Client implementations (requires discord-rpc.dll in game bin)" OFF)
option(MAPBASE_VSCRIPT "Build the game code with VScript implementation (note: interfaces still exist, just the provided implementation is not present)" ON)
option(MAPBASE_RESPONSE_SYSTEM "Build the game code with new Response System library based on the Alien Swarm SDK" ON)

# Stock source engine options
option(RETAIL "Build in retail mode" OFF)
option(STAGING_ONLY "Staging only" OFF)

set(RAD_TELEMETRY_DISABLED ${IS_SOURCESDK})
set(TF_BETA 0)
set(BUILD_REPLAY 0)
set(DEDICATED 0)

add_compile_definitions(
	$<$<BOOL:${RETAIL}>:_RETAIL>
	$<$<BOOL:${STAGING_ONLY}>:STAGING_ONLY>
	$<${TF_BETA}:TF_BETA>
	$<${RAD_TELEMETRY_DISABLED}:RAD_TELEMETRY_DISABLED>
	$<${MAPBASE}:MAPBASE>
	$<$<BOOL:${MAPBASE}>:MAPBASE>
	$<$<BOOL:${MAPBASE_MP}>:MAPBASE_MP>
	_DLL_EXT=${_DLL_EXT}
	FRAME_POINTER_OMISSION_DISABLED
)