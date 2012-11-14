BEGIN_PROJECT("executable")

SET_PRIVATE_HEADERS(
	ToolboxMain.h
)
SET_SOURCE_FILES(
	main.cpp
	ToolboxMain.cpp
)

SET_MOC_FILES(
    ToolboxMain.h
)

SET_UI_FILES(
	toolboxmaindeffile.ui
)
set(CORE_PRIVATES ${PROJECT_ROOT}/src)
include_directories(${CORE_PRIVATES})

END_PROJECT()