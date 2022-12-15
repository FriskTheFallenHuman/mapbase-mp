# response_rules.cmake

set(RESPONSE_RULES_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	RESPONSE_RULES_SOURCE_FILES

	# Source Files
	"${RESPONSE_RULES_DIR}/criteriaset.cpp"
	"${RESPONSE_RULES_DIR}/response_system.cpp"
	"${RESPONSE_RULES_DIR}/response_system.h"
	"${RESPONSE_RULES_DIR}/response_types.cpp"
	"${RESPONSE_RULES_DIR}/response_types_internal.cpp"
	"${RESPONSE_RULES_DIR}/response_types_internal.h"
	"${RESPONSE_RULES_DIR}/rr_convars.cpp"
	"${RESPONSE_RULES_DIR}/rr_response.cpp"
	"${RESPONSE_RULES_DIR}/rr_speechconcept.cpp"
	"${RESPONSE_RULES_DIR}/rrrlib.cpp"

	# Header Files
	"${SRCDIR}/public/responserules/response_host_interface.h"
	"${SRCDIR}/public/responserules/response_types.h"
	"${SRCDIR}/public/responserules/rr_speechconcept.h"
)

add_library( responserules STATIC ${RESPONSE_RULES_SOURCE_FILES} )

target_compile_definitions(
	responserules PRIVATE
	RR_RUNTIME
)

target_include_directories(
	responserules PRIVATE
	"${SRCDIR}/public/responserules"
)