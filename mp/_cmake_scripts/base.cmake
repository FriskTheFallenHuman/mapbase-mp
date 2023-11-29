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
set(MAPBASE "ON" CACHE BOOL "Build the game code with Mapbase features enable.")
set(MAPBASE_MP "ON" CACHE BOOL "Build the game code with extra features tailored for Multiplayer.")
set(MAPBASE_RPC "OFF" CACHE BOOL "Build the game code with Mapbase's Rich Presence Client implementations (requires discord-rpc.dll in game bin)")
set(MAPBASE_VSCRIPT "ON" CACHE BOOL "Build the game code with VScript implementation (note: interfaces still exist, just the provided implementation is not present)")
set(MAPBASE_RESPONSE_SYSTEM "ON" CACHE BOOL "Build the game code with new Response System library based on the Alien Swarm SDK.")
set(MAPBASE_BOTS "ON" CACHE BOOL "Build the game code with Bots code.")
set(MAPBASE_GIGALIB "OFF" CACHE BOOL "Apply engine patches and hooks for mapbase.")

# These ones gets ignore if MAPBASE_GIGALIB is OFF
set(MAPBASE_ENGINE_PATCHES "ON" CACHE BOOL "Enable bytepatching engine binaries with various fixes and tweaks.")
set(MAPBASE_ENGINE_DETOURS "ON" CACHE BOOL "Enable detouring engine functions with various fixes and tweaks, including an anti server lag measure.")
set(MAPBASE_SERVER_BLACKLISTS "ON" CACHE BOOL "Enable blacklisting certain server IPs from all clients.")
set(MAPBASE_FLUSH_DLS "ON" CACHE BOOL "Enable optionally flushing server downloadables every time the client disconnects from a server.")
set(MAPBASE_SENTRY_IO "OFF" CACHE BOOL "Enable SentryIO telemetry / crash reporting.")

# Configure these at your liking
set(BLACKLISTS_URL "https://tf2classic.com/api/bad.txt" CACHE STRING "The URL to where the blacklisted file is hosted.")
set(BLACKLISTS_CONTACT_URL "https://tf2classic.com/" CACHE STRING "The URL to report a server.")
set(SENTRY_URL "https://tf2classic.org/sentry" CACHE STRING "A valid https URL that points to a document containing a valid Sentry.IO DSN (including self hosted)")
set(SENTRY_PRIVACY_POLICY_URL "https://tf2classic.com/privacy" CACHE STRING "Enable SentryIO telemetry / crash reporting.")

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
	$<$<BOOL:${MAPBASE}>:MAPBASE>
	$<$<BOOL:${MAPBASE_MP}>:MAPBASE_MP>
	_DLL_EXT=${_DLL_EXT}
	FRAME_POINTER_OMISSION_DISABLED
)