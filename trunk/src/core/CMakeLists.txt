project(core)
# nazwa
set(TARGET_TARGETNAME
	core
)

# UWAGA: poniewa¿ u¿ywamy "source_group", ka¿dy nag³ówek/Ÿród³o musi do którejœ
# trafiæ; w przeciwnym wypadku CMake generuje b³êdny projekt

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	ASFAMCData.h
	ASFAMCParser.h
	ConsoleWidget.h
	DataManager.h
	DataTransfer.h
	FAnimation.h
	FileChunkReader.h
	FileReader2Motion.h
	FileUtil.h
	FModel.h
	GridWidget.h
	Intersection.h
	Logger.h
	Mesh.h
	Model.h
	ModelService.h
	RenderService.h
	ServiceManager.h
	SkeletonNode.h
	TimeLine.h
	ToolboxMain.h
	UserInterfaceService.h
	Quaternioncal.h
	ComputeThread.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/BaseDataTypes.h
	${HEADER_PATH}/Bone.h
	${HEADER_PATH}/dae2Motion.h
	${HEADER_PATH}/Enumerators.h
	${HEADER_PATH}/Frame.h
	${HEADER_PATH}/IAnimationGroup.h
	${HEADER_PATH}/IAnimationNode.h
	${HEADER_PATH}/IService.h
	${HEADER_PATH}/IIdentifiable.h
	${HEADER_PATH}/IDataManager.h
	${HEADER_PATH}/IMesh.h
	${HEADER_PATH}/IModel.h
	${HEADER_PATH}/IServiceManager.h
	${HEADER_PATH}/ISkeletonNode.h
	${HEADER_PATH}/Window.h
	${HEADER_PATH}/ModelData.h
	${HEADER_PATH}/QtWidgetInterface.h
	${HEADER_PATH}/SimpleFunctors.h
	${HEADER_PATH}/Skeleton.h
	${HEADER_PATH}/IUserInterface.h
	${HEADER_PATH}/Vec3.h
	${HEADER_PATH}/WidgetInterface.h
	${HEADER_PATH}/QOSGWidget.h
	${HEADER_PATH}/Plugin.h
	${HEADER_PATH}/PluginLoader.h
	${HEADER_PATH}/Export.h
	${HEADER_PATH}/SmartPtr.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	ASFAMCData.cpp
	ASFAMCParser.cpp
	ConsoleWidget.cpp
	DataManager.cpp
	FileChunkReader.cpp
	FileReader2Motion.cpp
	FileUtil.cpp
	GridWidget.cpp
	Logger.cpp
	main.cpp
	ModelService.cpp
	PluginLoader.cpp
	RenderService.cpp
	ServiceManager.cpp
	TimeLine.cpp
	ToolboxMain.cpp
	UserInterfaceService.cpp
	
	AnimationGroup.cpp
	AnimationNode.cpp
#	CustomGroup.cpp
	FModel.cpp
	Intersection.cpp
	Mesh.cpp
	Model.cpp
	SkeletonNode.cpp	
	Quaternioncal.cpp
	ComputeThread.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
    ConsoleWidget.h
    TimeLine.h
    ToolboxMain.h
	GridWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/ConsoleWidget.ui
	ui/GridWidget.ui
	ui/OsgWidget.ui
	ui/TimeLine.ui
	ui/toolboxmaindeffile.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# CONFIG
set(CONFIG_H
	config/ConfigBuffer.h
	config/ConfigurationGroup.h
	config/FileReader.h
	config/IConfigManager.h
	config/Parameter.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\config" FILES ${CONFIG_H})
set(TARGET_H ${TARGET_H} ${CONFIG_H})

set(CONFIG_SRC
	config/ConfigBuffer.cpp
	config/ConfigurationGroup.cpp
	config/FileReader.cpp
	config/Parameter.cpp
)
source_group("${SOURCEGROUP_SOURCES}\\config" FILES ${CONFIG_SRC})
set(TARGET_SRC ${TARGET_SRC} ${CONFIG_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H CorePCH.h)
set(PRECOMPILED_SRC CorePCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


#------------------------------------------------------------------------------
# tworzenie

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(core)
