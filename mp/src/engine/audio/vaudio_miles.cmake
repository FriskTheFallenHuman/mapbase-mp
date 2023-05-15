# vaudio_minimp3.cmake
# Disguised has vaudio_miles.cmake so we can override it

set(VAUDIO_MINIMP3_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	VAUDIO_MINIMP3_SOURCE_FILES

	"${VAUDIO_MINIMP3_DIR}/AudioMiniMP3.cpp"
	"${VAUDIO_MINIMP3_DIR}/minimp3.h"
	
	# Public Header Files
	"${SRCDIR}/public/tier1/interface.h"
	"${SRCDIR}/public/vaudio/ivaudio.H"
)

add_library(vaudio_miles MODULE ${VAUDIO_MINIMP3_SOURCE_FILES})

set_target_properties(
	vaudio_miles PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${GAMEDIR}/${BUILD_FOLDER}/bin"
)