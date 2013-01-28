BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS(
	CoreDockWidgetSet.h
	CoreDockWidgetManager.h
	CoreConsoleWidget.h
	CoreMainWindow.h	
	CoreAction.h
	CoreDockWidget.h
	CoreFlexiToolBar.h
	CoreFlexiToolBarSection.h
	CoreTitleBar.h
	CoreWidgetAction.h
	CoreVisualizerWidget.h
	ICoreActionSection.h
)

SET_SOURCE_FILES(
	CoreDockWidgetSet.cpp
	CoreDockWidgetManager.cpp
	CoreVisualizerWidget.cpp
	CoreMainWindow.cpp
	CoreConsoleWidget.cpp
	CoreAction.cpp
	CoreDockWidget.cpp
	CoreFlexiToolBar.cpp
	CoreFlexiToolBarSection.cpp
	CoreTitleBar.cpp
	CoreWidgetAction.cpp
	CoreUiPCH.cpp
)

SET_PRIVATE_HEADERS(
	CoreUiPCH.h
)

SET_MOC_FILES(
	CoreDockWidgetSet.h
	CoreDockWidgetManager.h
	CoreConsoleWidget.h
	CoreMainWindow.h	
	CoreAction.h
	CoreDockWidget.h
	CoreFlexiToolBar.h
	CoreFlexiToolBarSection.h
	CoreTitleBar.h
	CoreWidgetAction.h
)

SET_UI_FILES(
	CoreConsoleWidget.ui
	CoreFlexiToolBarSection.ui
	CoreTitleBar.ui
)

SET_RC_FILES (
	CoreIcons.qrc
)

SET_PRECOMPILED_HEADER (CoreUiPCH.h CoreUiPCH.cpp)

END_PROJECT()
