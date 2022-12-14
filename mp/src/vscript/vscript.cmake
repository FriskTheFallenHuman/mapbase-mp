# vscript.cmake

set(VSCRIPT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	VSCRIPT_SOURCE_FILES

	# Source Files
	"${VSCRIPT_DIR}/vscript.cpp"
	"${VSCRIPT_DIR}/vscript_bindings_base.cpp"
	"${VSCRIPT_DIR}/vscript_bindings_math.cpp"

	# Header Files
	"${VSCRIPT_DIR}/vscript_bindings_base.h"
	"${VSCRIPT_DIR}/vscript_bindings_math.h"

	# VMs->Squirrel
	"${VSCRIPT_DIR}/vms/squirrel/vscript_squirrel.cpp"
	"${VSCRIPT_DIR}/vms/squirrel/vscript_squirrel.nut"

	# VMs->Squirrel->Thirdparty
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdaux.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdblob.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdio.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdmath.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdrex.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdstream.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdstring.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdsystem.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqapi.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqbaselib.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqclass.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqcompiler.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqdebug.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqfuncstate.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqlexer.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqmem.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqobject.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqstate.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqtable.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqvm.cpp"
)

set_source_files_properties(
	# VMs->Squirrel->Thirdparty
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdaux.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdblob.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdio.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdmath.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdrex.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdstream.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdstring.cpp"
	"${THIRDPARTYDIR}/squirrel/sqstdlib/sqstdsystem.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqapi.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqbaselib.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqclass.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqcompiler.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqdebug.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqfuncstate.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqlexer.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqmem.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqobject.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqstate.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqtable.cpp"
	"${THIRDPARTYDIR}/squirrel/squirrel/sqvm.cpp"
	PROPERTIES
	COMPILE_FLAGS "/wd4100 /wd4611 /wd4127 /wd4244 /wd4702 /wd4706 /wd4800"
)

add_library( vscript STATIC ${VSCRIPT_SOURCE_FILES} )

target_compile_definitions(
	vscript PRIVATE
	$<$<BOOL:${MAPBASE_VSCRIPT}>:MAPBASE_VSCRIPT>
)

target_include_directories(
	vscript PRIVATE
	"${VSCRIPT_DIR}/vms/squirrel"
	"${THIRDPARTYDIR}/squirrel/include"
	"${THIRDPARTYDIR}/squirrel/squirrel"
)