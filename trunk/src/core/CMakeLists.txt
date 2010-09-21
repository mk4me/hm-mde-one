project(core)
# nazwa
set(TARGET_TARGETNAME
	core
)

# UWAGA: poniewa¿ u¿ywamy "source_group", ka¿dy nag³ówek/Ÿród³o musi do którejœ
# trafiæ; w przeciwnym wypadku CMake generuje b³êdny projekt

#------------------------------------------------------------------------------
# pliki
set(TARGET_H
	AdapterWidget.h
	Animation.h
	ConsoleWidget.h
	FileChunkReader.h
	FileReader2Motion.h
	FileUtil.h
	GridWidget.h
	Logger.h
	ModelService.h
	OsgControlWidget.h
	PluginServices.h
	QOSGWidget.h
	QTMainWindow.h
	RenderService.h
	ServiceManager.h
	TimeLine.h
	ToolboxMain.h
	UserInterfaceService.h
	WindowFactory.h
)
source_group("Header files" FILES ${TARGET_H})

set(TARGET_SRC
	AdapterWidget.cpp
	ConsoleWidget.cpp
	FileChunkReader.cpp
	FileReader2Motion.cpp
	FileUtil.cpp
	GridWidget.cpp
	Logger.cpp
	main.cpp
	ModelService.cpp
	PluginServices.cpp
	QOSGWidget.cpp
	QTMainWindow.cpp
	RenderService.cpp
	ServiceManager.cpp
	TimeLine.cpp
	ToolboxMain.cpp
	UserInterfaceService.cpp
	WindowFactory.cpp
	
	../OsgExtensions/AnimationGroup.cpp
	../OsgExtensions/AnimationNode.cpp
#	../OsgExtensions/CustomGroup.cpp
	../OsgExtensions/FModel.cpp
	../OsgExtensions/Intersection.cpp
	../OsgExtensions/Mesh.cpp
	../OsgExtensions/Model.cpp
	../OsgExtensions/SkeletonNode.cpp
	
	../OsgExtensions/tinyXML/tinystr.cpp
	../OsgExtensions/tinyXML/tinyxml.cpp
	../OsgExtensions/tinyXML/tinyxmlerror.cpp
	../OsgExtensions/tinyXML/tinyxmlparser.cpp
	
)

SET(TARGET_MOC_H
    ConsoleWidget.h
    TimeLine.h
    ToolboxMain.h
	GridWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})
source_group("Source files" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------
# UI
set(CORE_UI
	ui/Console.ui
	ui/GridWidget.ui
	ui/OsgWidget.ui
	ui/TimeLine.ui
	ui/toolboxmaindeffile.ui
)
QT4_WRAP_UI(CORE_UI_H ${CORE_UI})
source_group("Header files\\ui" FILES ${CORE_UI_H} ${CORE_UI})
set(TARGET_H ${TARGET_H} ${CORE_UI_H} ${CORE_UI})

#------------------------------------------------------------------------------
# CONFIG
set(CONFIG_H
	config/ConfigBuffer.h
	config/ConfigurationGroup.h
	config/FileReader.h
	config/IConfigManager.h
	config/Parameter.h
)
source_group("Header files\\config" FILES ${CONFIG_H})
set(TARGET_H ${TARGET_H} ${CONFIG_H})

set(CONFIG_SRC
	config/ConfigBuffer.cpp
	config/ConfigurationGroup.cpp
	config/FileReader.cpp
	config/Parameter.cpp
)
source_group("Source files\\config" FILES ${CONFIG_SRC})
set(TARGET_SRC ${TARGET_SRC} ${CONFIG_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
