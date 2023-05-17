#	hlmv.cmake

set(HLMV_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	HLMV_SOURCE_FILES

	# mxtk

	# Header Files
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mx.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxBmp.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxButton.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxCheckBox.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxChoice.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxChooseColor.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxEvent.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxFileDialog.h"
	#"${SRCDIR}/thirdparty/mxtk/include/mx/mxGlWindow.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxGroupBox.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxImage.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxInit.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxLabel.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxLineEdit.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxLinkedList.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxListBox.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxMatSysWindow.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxMenu.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxMenuBar.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxMessageBox.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxpath.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxPcx.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxPopupMenu.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxProgressBar.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxRadioButton.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxScrollbar.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxSlider.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxstring.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxTab.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxTga.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxToggleButton.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxToolTip.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxTreeView.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxWidget.h"
	# "${SRCDIR}/thirdparty/mxtk/include/mx/mxWindow.h"

	# Source Files
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mx.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/common/mxBmp.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxButton.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxCheckBox.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxChoice.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxChooseColor.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxFileDialog.cpp"
	#"${SRCDIR}/thirdparty/mxtk/src/win32/mxGlWindow.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxGroupBox.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxLabel.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxLineEdit.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxListBox.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxlistview.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxMatSysWindow.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxMenu.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxMenuBar.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxMessageBox.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/common/mxpath.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/common/mxPcx.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxPopupMenu.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxProgressBar.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxRadioButton.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxScrollbar.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxSlider.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/common/mxstring.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxTab.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/common/mxTga.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxToolTip.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxTreeView.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxWidget.cpp"
	# "${SRCDIR}/thirdparty/mxtk/src/win32/mxWindow.cpp"

	# Source Files
	"${SRCDIR}/public/bone_setup.cpp"
	"${SRCDIR}/public/CollisionUtils.cpp"
	"${SRCDIR}/public/filesystem_helpers.cpp"
	"${SRCDIR}/public/filesystem_init.cpp"
	"${SRCDIR}/public/studio.cpp"
	#"${SRCDIR}/public/tier0/memoverride.cpp"

	"${HLMV_DIR}/attachments_window.cpp"
	"${HLMV_DIR}/ControlPanel.cpp"
	"${HLMV_DIR}/debugdrawmodel.cpp"
	"${HLMV_DIR}/FileAssociation.cpp"
	"${HLMV_DIR}/matsyswin.cpp"
	"${HLMV_DIR}/mdlviewer.cpp"
	"${HLMV_DIR}/mxLineEdit2.cpp"
	#"${HLMV_DIR}/pakviewer.cpp"
	"${HLMV_DIR}/physmesh.cpp"
	"${HLMV_DIR}/studio_flex.cpp"
	"${HLMV_DIR}/studio_render.cpp"
	"${HLMV_DIR}/studio_utils.cpp"
	"${HLMV_DIR}/sys_win.cpp"
	"${HLMV_DIR}/ViewerSettings.cpp"
	"${HLMV_DIR}/camera.cpp"

	# Header Files
	"${HLMV_DIR}/attachments_window.h"
	"${HLMV_DIR}/ControlPanel.h"
	"${HLMV_DIR}/debugdrawmodel.h"
	"${HLMV_DIR}/FileAssociation.h"
	"${HLMV_DIR}/matsyswin.h"
	"${HLMV_DIR}/mdlviewer.h"
	#"${HLMV_DIR}/pakviewer.h"
	"${HLMV_DIR}/physmesh.h"
	"${HLMV_DIR}/studio_render.h"
	"${HLMV_DIR}/StudioModel.h"
	"${HLMV_DIR}/sys.h"
	"${HLMV_DIR}/ViewerSettings.h"
	"${HLMV_DIR}/mxLineEdit2.h"
	"${HLMV_DIR}/resource.h"
	"${HLMV_DIR}/camera.h"
	
	# Resources
	"${HLMV_DIR}/hlmv.rc"
	"${HLMV_DIR}/icon1.ico"
)

add_executable(hlmv ${HLMV_SOURCE_FILES})

set_property(TARGET hlmv PROPERTY FOLDER "Tools//GUI Tools")

set_target_properties(
	hlmv PROPERTIES
	# LINK_FLAGS "-e mainCRTStartup"
	RUNTIME_OUTPUT_DIRECTORY "${GAMEDIR}/bin"
)

target_include_directories(
	hlmv PRIVATE
	"${SRCDIR}/thirdparty/mxtk/include"
	"${SRCDIR}/utils/common"
	"${SRCDIR}/public"
	"${SRCDIR}/public/mathlib"
)

target_link_options(
	hlmv PRIVATE 
	"$<$<C_COMPILER_ID:MSVC>:-SAFESEH:NO>"
)

target_link_libraries(
	hlmv PRIVATE
	mxtk
	"${LIBPUBLIC}/appframework${STATIC_LIB_EXT}"
	"${LIBPUBLIC}/bitmap${STATIC_LIB_EXT}"
	mathlib
	"${LIBPUBLIC}/tier0${STATIC_LIB_EXT}"
	tier1
	"${LIBPUBLIC}/tier2${STATIC_LIB_EXT}"
	"${LIBPUBLIC}/vstdlib${STATIC_LIB_EXT}"
	$<${IS_WINDOWS}:comctl32>
)