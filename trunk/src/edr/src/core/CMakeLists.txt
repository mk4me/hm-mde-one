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
	AppInitializer.h
	MainWindow.h
	EDRConsoleWidget.h
	EDRConsoleInnerWidget.h
	DataManager.h
	SceneGraphWidget.h
	LogInitializer.h
	ServiceManager.h
	#ToolboxMain.h
	UserInterfaceService.h
	VisualizerWidget.h
	VisualizerManager.h
	ObjectOutput.h
	ObjectSource.h
	Visualizer.h
	SampleProcessor.h
	ObjectSlots.h
	EDRTitleBar.h
	EDRDockWidget.h
	EDRDockInnerWidget.h
	TestEDRWidget.h
	WorkflowItemEncapsulator.h
	InputItem.h
	InputDescription.h
	OutputDescription.h
	OutputItem.h
	InputOutputItem.h
	EDRConfig.h
	EDRDFNode.h
	EDRDFSourceNode.h
	EDRDFPin.h
	EDRDataFlow.h
	Workflow.h
	WorkflowManager.h
	DataProcessorManager.h
	WorkflowService.h
	WorkflowWidget.h
	WorkflowVDFModel.h
	WorkflowVDFConnDelContextMenu.h
	LocalDataSource.h
	LocalDataSourceWidget.h
	DataSource.h
	DataSourceManager.h
	Log.h
	PluginLoader.h
	#
	
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/BaseDataTypes.h
	${HEADER_PATH}/Filesystem.h
	${HEADER_PATH}/IService.h
	${HEADER_PATH}/IIdentifiable.h
	${HEADER_PATH}/IDataManager.h
	${HEADER_PATH}/IParser.h
	${HEADER_PATH}/IServiceManager.h
	${HEADER_PATH}/Window.h
	${HEADER_PATH}/IUserInterface.h
	${HEADER_PATH}/Plugin.h
	${HEADER_PATH}/Export.h
	${HEADER_PATH}/SmartPtr.h
	# ${HEADER_PATH}/Parameter.h
	# ${HEADER_PATH}/Parameters.h
	# ${HEADER_PATH}/GroupData.h
	# ${HEADER_PATH}/Header.h
	# ${HEADER_PATH}/Data.h
	${HEADER_PATH}/ILog.h
	${HEADER_PATH}/ObjectWrapper.h
	${HEADER_PATH}/ObjectWrapperCollection.h
	${HEADER_PATH}/IObjectSource.h
	${HEADER_PATH}/IVisualizer.h
	${HEADER_PATH}/IVisualizerManager.h
	${HEADER_PATH}/IDataProcessor.h
	${HEADER_PATH}/IObjectOutput.h
	${HEADER_PATH}/PluginCommon.h
	${HEADER_PATH}/ObjectWrapperFactory.h
	${HEADER_PATH}/TypeInfo.h
	${HEADER_PATH}/IEDRTitleBar.h
	${HEADER_PATH}/IInputDescription.h 
	${HEADER_PATH}/IOutputDescription.h
	${HEADER_PATH}/IInputProcessItem.h
	${HEADER_PATH}/IOutputProcessItem.h
	${HEADER_PATH}/IInputOutputProcessItem.h
	${HEADER_PATH}/IConfigurable.h
	${HEADER_PATH}/IWorkflowItemBase.h
	${HEADER_PATH}/IClonable.h
	${HEADER_PATH}/ISerializable.h
	${HEADER_PATH}/IPath.h
	${HEADER_PATH}/IWorkflow.h
	${HEADER_PATH}/IWorkflowManager.h
	${HEADER_PATH}/IDataProcessorManager.h
	${HEADER_PATH}/IDataSource.h
	${HEADER_PATH}/IDataSourceManager.h
	#${HEADER_PATH}/IntegumentarySystem.h
	#${HEADER_PATH}/ISubjectSystem.h
	#${HEADER_PATH}/Motion.h
	#${HEADER_PATH}/MuscularSystem.h
	#${HEADER_PATH}/Session.h
	#${HEADER_PATH}/SkeletalSystem.h
	#${HEADER_PATH}/SolidSystem.h
	#${HEADER_PATH}/Subject.h
	#${HEADER_PATH}/SubjectSystemConfiguration.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	AppInitializer.cpp
	MainWindow.cpp
	EDRConsoleWidget.cpp
	EDRConsoleInnerWidget.cpp
	DataManager.cpp
	SceneGraphWidget.cpp
	LogInitializer.cpp
	#main.cpp
	PluginLoader.cpp
	ServiceManager.cpp
	#ToolboxMain.cpp
	UserInterfaceService.cpp
	VisualizerWidget.cpp
	VisualizerManager.cpp
	Visualizer.cpp
	ObjectSlots.cpp
	DataProcessor.cpp
	EDRTitleBar.cpp
	EDRDockWidget.cpp
	EDRDockInnerWidget.cpp	
	TestEDRWidget.cpp
	InputDescription.cpp
	OutputDescription.cpp
	EDRConfig.cpp
	EDRDFNode.cpp
	EDRDFSourceNode.cpp
	EDRDFPin.cpp
	EDRDataFlow.cpp
	DataProcessorManager.cpp
	Workflow.cpp
	WorkflowWidget.cpp
	WorkflowManager.cpp
	WorkflowService.cpp
	WorkflowVDFModel.cpp
	WorkflowVDFConnDelContextMenu.cpp
	LocalDataSource.cpp
	LocalDataSourceWidget.cpp
	DataSource.cpp
	DataSourceManager.cpp
	# Subject.cpp
	# Motion.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	EDRConsoleWidget.h
	EDRConsoleInnerWidget.h
    MainWindow.h
	SceneGraphWidget.h
	VisualizerWidget.h
	EDRTitleBar.h
	EDRDockWidget.h
	EDRDockInnerWidget.h
	TestEDRWidget.h
	WorkflowWidget.h
	WorkflowVDFModel.h
	WorkflowVDFConnDelContextMenu.h
	LocalDataSourceWidget.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/SceneGraphWidget.ui
	ui/OsgWidget.ui
	#ui/toolboxmaindeffile.ui
	ui/VisualizerWidget.ui
	ui/DataProcessorWidget.ui
	ui/EDRTitleBar.ui
	ui/EDRDockInnerWidget.ui
	ui/EDRConsoleInnerWidget.ui
	ui/TestEDRWidget.ui
	ui/WorkflowWidget.ui
	ui/LocalDataSourceWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# RC
set(TARGET_RC
	ui/ConsoleWidget.qrc
	ui/Icons.qrc
)
QT4_ADD_RESOURCES(TARGET_RCC_SRC ${TARGET_RC})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_RC} ${TARGET_RCC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_RCC_SRC} ${TARGET_RC})

# #------------------------------------------------------------------------------
# # CONFIG
# set(CONFIG_H
	# config/ConfigBuffer.h
	# config/ConfigurationGroup.h
	# config/FileReader.h
	# config/IConfigManager.h
	# config/ConfigParameter.h
# )
# source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\config" FILES ${CONFIG_H})
# set(TARGET_H ${TARGET_H} ${CONFIG_H})

# set(CONFIG_SRC
	# config/ConfigBuffer.cpp
	# config/ConfigurationGroup.cpp
	# config/FileReader.cpp
	# config/ConfigParameter.cpp
# )
# source_group("${SOURCEGROUP_SOURCES}\\config" FILES ${CONFIG_SRC})
# set(TARGET_SRC ${TARGET_SRC} ${CONFIG_SRC})

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

# kompilacja do exe
option(EDR_WIN32_ENABLE_CONSOLE "Enable console on Win32?" OFF)
# if (EDR_WIN32_ENABLE_CONSOLE)
	# add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# else()
	# add_executable(${TARGET_TARGETNAME} WIN32 ${TARGET_SRC} ${TARGET_H})
# endif()

add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})

#instalacja
#install(TARGETS ${TARGET_TARGETNAME} DESTINATION bin COMPONENT core)
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(core)
