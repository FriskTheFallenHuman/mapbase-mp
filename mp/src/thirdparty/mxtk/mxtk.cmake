# mxtk.cmake

set(MXTK_DIR ${CMAKE_CURRENT_LIST_DIR})
set(
	MXTK_SOURCE_FILES

	# Header Files
	"${MXTK_DIR}/include/mx/mx.h"
	"${MXTK_DIR}/include/mx/mxBmp.h"
	"${MXTK_DIR}/include/mx/mxButton.h"
	"${MXTK_DIR}/include/mx/mxCheckBox.h"
	"${MXTK_DIR}/include/mx/mxChoice.h"
	"${MXTK_DIR}/include/mx/mxChooseColor.h"
	"${MXTK_DIR}/include/mx/mxEvent.h"
	"${MXTK_DIR}/include/mx/mxFileDialog.h"
	#"${MXTK_DIR}/include/mx/mxGlWindow.h"
	"${MXTK_DIR}/include/mx/mxGroupBox.h"
	"${MXTK_DIR}/include/mx/mxImage.h"
	"${MXTK_DIR}/include/mx/mxInit.h"
	"${MXTK_DIR}/include/mx/mxLabel.h"
	"${MXTK_DIR}/include/mx/mxLineEdit.h"
	"${MXTK_DIR}/include/mx/mxLinkedList.h"
	"${MXTK_DIR}/include/mx/mxListBox.h"
	"${MXTK_DIR}/include/mx/mxMatSysWindow.h"
	"${MXTK_DIR}/include/mx/mxMenu.h"
	"${MXTK_DIR}/include/mx/mxMenuBar.h"
	"${MXTK_DIR}/include/mx/mxMessageBox.h"
	"${MXTK_DIR}/include/mx/mxpath.h"
	"${MXTK_DIR}/include/mx/mxPcx.h"
	"${MXTK_DIR}/include/mx/mxPopupMenu.h"
	"${MXTK_DIR}/include/mx/mxProgressBar.h"
	"${MXTK_DIR}/include/mx/mxRadioButton.h"
	"${MXTK_DIR}/include/mx/mxScrollbar.h"
	"${MXTK_DIR}/include/mx/mxSlider.h"
	"${MXTK_DIR}/include/mx/mxstring.h"
	"${MXTK_DIR}/include/mx/mxTab.h"
	"${MXTK_DIR}/include/mx/mxTga.h"
	"${MXTK_DIR}/include/mx/mxToggleButton.h"
	"${MXTK_DIR}/include/mx/mxToolTip.h"
	"${MXTK_DIR}/include/mx/mxTreeView.h"
	"${MXTK_DIR}/include/mx/mxWidget.h"
	"${MXTK_DIR}/include/mx/mxWindow.h"

	# Source Files
	"${MXTK_DIR}/src/win32/mx.cpp"
	"${MXTK_DIR}/src/common/mxBmp.cpp"
	"${MXTK_DIR}/src/win32/mxButton.cpp"
	"${MXTK_DIR}/src/win32/mxCheckBox.cpp"
	"${MXTK_DIR}/src/win32/mxChoice.cpp"
	"${MXTK_DIR}/src/win32/mxChooseColor.cpp"
	"${MXTK_DIR}/src/win32/mxFileDialog.cpp"
	#"${MXTK_DIR}/src/win32/mxGlWindow.cpp"
	"${MXTK_DIR}/src/win32/mxGroupBox.cpp"
	"${MXTK_DIR}/src/win32/mxLabel.cpp"
	"${MXTK_DIR}/src/win32/mxLineEdit.cpp"
	"${MXTK_DIR}/src/win32/mxListBox.cpp"
	"${MXTK_DIR}/src/win32/mxlistview.cpp"
	"${MXTK_DIR}/src/win32/mxMatSysWindow.cpp"
	"${MXTK_DIR}/src/win32/mxMenu.cpp"
	"${MXTK_DIR}/src/win32/mxMenuBar.cpp"
	"${MXTK_DIR}/src/win32/mxMessageBox.cpp"
	"${MXTK_DIR}/src/common/mxpath.cpp"
	"${MXTK_DIR}/src/common/mxPcx.cpp"
	"${MXTK_DIR}/src/win32/mxPopupMenu.cpp"
	"${MXTK_DIR}/src/win32/mxProgressBar.cpp"
	"${MXTK_DIR}/src/win32/mxRadioButton.cpp"
	"${MXTK_DIR}/src/win32/mxScrollbar.cpp"
	"${MXTK_DIR}/src/win32/mxSlider.cpp"
	"${MXTK_DIR}/src/common/mxstring.cpp"
	"${MXTK_DIR}/src/win32/mxTab.cpp"
	"${MXTK_DIR}/src/common/mxTga.cpp"
	"${MXTK_DIR}/src/win32/mxToolTip.cpp"
	"${MXTK_DIR}/src/win32/mxTreeView.cpp"
	"${MXTK_DIR}/src/win32/mxWidget.cpp"
	"${MXTK_DIR}/src/win32/mxWindow.cpp"
)

add_library(mxtk STATIC ${MXTK_SOURCE_FILES})

set_property(TARGET mxtk PROPERTY FOLDER "Libs")

set_target_properties(
	mxtk PROPERTIES
	LINK_FLAGS "-e mainCRTStartup"
)

target_include_directories(
	mxtk PRIVATE
	"${MXTK_DIR}/include"
)

target_link_options(
	mxtk PRIVATE 
	"$<$<C_COMPILER_ID:MSVC>:-SAFESEH:NO>"
)