# gamepadui_base.cmake

include_guard(GLOBAL)

set(GAMEPADUI_BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	GAMEPADUI_BASE_SOURCE_FILES
	"${GAMEPADUI_BASE_DIR}/gamepadui_achievements.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_basepanel.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_bonusmaps.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_button.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_frame.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_genericconfirmation.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_genericframes.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_interface.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_mainmenu.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_scrollbar.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_newgame.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_savegame.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_options.cpp"
	"${GAMEPADUI_BASE_DIR}/gamepadui_util.cpp"
	"${SRCDIR}/public/vgui_controls/vgui_controls.cpp"
	"${SRCDIR}/common/language.cpp"

	# Header Files
	"${GAMEPADUI_BASE_DIR}/igamepadui.h"

	"${GAMEPADUI_BASE_DIR}/gamepadui_basepanel.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_button.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_frame.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_genericconfirmation.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_genericframes.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_gradient_helper.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_image.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_interface.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_mainmenu.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_panel.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_scrollbar.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_scroll.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_string.h"
	"${GAMEPADUI_BASE_DIR}/gamepadui_util.h"
)

function(target_use_gamepadui_base target)
	target_sources(
		${target} PRIVATE
		${GAMEPADUI_BASE_SOURCE_FILES}
	)

	target_include_directories(
		${target} PRIVATE
		"${GAMEPADUI_BASE_DIR}"
		"${SRCDIR}/vgui2/include"
		"${SRCDIR}/vgui2/controls"
	)

	target_compile_definitions(
		${target} PRIVATE
		GAMEPADUI_DLL
		VERSION_SAFE_STEAM_API_INTERFACES
		strncpy=use_Q_strncpy_instead
		_snprintf=use_Q_snprintf_instead
	)
	
	target_link_libraries(
		${target} PRIVATE

		"$<${IS_OSX}:-framework Carbon>"
		$<${IS_LINUX}:rt>
		$<${IS_WINDOWS}:winmm>
		"$<$<AND:${IS_WINDOWS},${BUILD_REPLAY}>:wsock32;Ws2_32>"

		"${LIBPUBLIC}/bitmap${STATIC_LIB_EXT}"
		"${LIBPUBLIC}/choreoobjects${STATIC_LIB_EXT}"
		"${LIBPUBLIC}/dmxloader${STATIC_LIB_EXT}"
		mathlib
		"${LIBPUBLIC}/matsys_controls${STATIC_LIB_EXT}"
		tier1
		"${LIBPUBLIC}/tier2${STATIC_LIB_EXT}"
		"${LIBPUBLIC}/tier3${STATIC_LIB_EXT}"
		vgui_controls
		"${LIBPUBLIC}/vtf${STATIC_LIB_EXT}"
		steam_api

		"$<$<BOOL:${RETAIL}>:${LIBCOMMON}/libjpeg>"
		"$<$<BOOL:${RETAIL}>:libpng>"
	)
endfunction()