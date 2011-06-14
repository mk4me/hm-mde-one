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
#	ASFAMCData.h
#	ASFAMCParser.h
#	C3DModel.h
	ConsoleWidget.h
	EDRConsoleWidget.h
	EDRConsoleInnerWidget.h
	DataManager.h
	#DataTransfer.h
	#Factory.h
	#FAnimation.h
	#FileChunkReader.h
	#FileReader2Motion.h
	#FileUtil.h
	#FModel.h
	SceneGraphWidget.h
	#Intersection.h
	LogInitializer.h
	#Marker.h
	#Mesh.h
	#Model.h
	#RenderService.h
	ServiceManager.h
	#SkeletonNode.h
	ToolboxMain.h
	UserInterfaceService.h
	#Quaternioncal.h
	ComputeThread.h
	VisualizerWidget.h
	VisualizerManager.h
	ObjectOutput.h
	ObjectSource.h
	Visualizer.h
	#VisualizerTitleBar.h
	#VisualizerTitleBarComboPick.h
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
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/BaseDataTypes.h
	#${HEADER_PATH}/Bone.h	
	${HEADER_PATH}/DateTime.h
	#${HEADER_PATH}/Enumerators.h
	${HEADER_PATH}/Filesystem.h
	#${HEADER_PATH}/Frame.h
	#${HEADER_PATH}/IAnimationGroup.h
	#${HEADER_PATH}/IAnimationNode.h
	${HEADER_PATH}/IService.h
	${HEADER_PATH}/IIdentifiable.h
	${HEADER_PATH}/IDataManager.h
	#${HEADER_PATH}/IMesh.h
	#{HEADER_PATH}/IModel.h
	${HEADER_PATH}/IParser.h
	${HEADER_PATH}/IServiceManager.h
	#${HEADER_PATH}/ISkeletonNode.h
	${HEADER_PATH}/Window.h
	#${HEADER_PATH}/ModelData.h
	${HEADER_PATH}/QtWidgetInterface.h
	${HEADER_PATH}/SimpleFunctors.h
	#${HEADER_PATH}/Skeleton.h
	${HEADER_PATH}/IUserInterface.h
	#${HEADER_PATH}/Vec3.h
	${HEADER_PATH}/WidgetInterface.h
	${HEADER_PATH}/Plugin.h
	${HEADER_PATH}/PluginLoader.h
	${HEADER_PATH}/Export.h
	${HEADER_PATH}/SmartPtr.h
	${HEADER_PATH}/Chart.h
	${HEADER_PATH}/c3dParser.h
	${HEADER_PATH}/c3dParserEx.h
	${HEADER_PATH}/C3D_Data.h
	${HEADER_PATH}/ChartData.h
	${HEADER_PATH}/ChartPointer.h
	${HEADER_PATH}/Parameter.h
	${HEADER_PATH}/Parameters.h
	${HEADER_PATH}/GroupData.h
	${HEADER_PATH}/Header.h
	${HEADER_PATH}/Data.h
	${HEADER_PATH}/Log.h
	${HEADER_PATH}/LocalTrial.h
	${HEADER_PATH}/ObjectWrapper.h
	${HEADER_PATH}/ObjectWrapperCollection.h
	${HEADER_PATH}/IObjectSource.h
	${HEADER_PATH}/IVisualizer.h
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
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
#	ASFAMCData.cpp
#	ASFAMCParser.cpp
#	C3DModel.cpp
	ConsoleWidget.cpp
	EDRConsoleWidget.cpp
	EDRConsoleInnerWidget.cpp
	DataManager.cpp
	#Factory.cpp
	#FileChunkReader.cpp
	#FileReader2Motion.cpp
	#FileUtil.cpp
	SceneGraphWidget.cpp
	LogInitializer.cpp
	main.cpp
	PluginLoader.cpp
	#RenderService.cpp
	ServiceManager.cpp
	ToolboxMain.cpp
	UserInterfaceService.cpp
	
	#AnimationGroup.cpp
	#AnimationNode.cpp
#	CustomGroup.cpp
	#FModel.cpp
	#Intersection.cpp
	#Marker.cpp
	#Mesh.cpp
	#Model.cpp
	#SkeletonNode.cpp	
	#Quaternioncal.cpp
	ComputeThread.cpp
	VisualizerWidget.cpp
	VisualizerManager.cpp
	Visualizer.cpp
	#VisualizerTitleBar.cpp
	#VisualizerTitleBarComboPick.cpp
	ObjectSlots.cpp
	DataProcessor.cpp
	EDRTitleBar.cpp
	EDRDockWidget.cpp
	EDRDockInnerWidget.cpp	
	TestEDRWidget.cpp
	WorkflowItemEncapsulator.cpp
	InputItem.cpp
	InputDescription.cpp
	OutputDescription.cpp
	OutputItem.cpp
	InputOutputItem.cpp
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
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
    ConsoleWidget.h
	EDRConsoleWidget.h
	EDRConsoleInnerWidget.h
    ToolboxMain.h
	SceneGraphWidget.h
	VisualizerWidget.h
	#VisualizerTitleBar.h
	#VisualizerTitleBarComboPick.h
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
	ui/ConsoleWidget.ui
	ui/SceneGraphWidget.ui
	ui/OsgWidget.ui
	ui/toolboxmaindeffile.ui
	ui/VisualizerWidget.ui
	#ui/VisualizerTitleBar.ui
	#ui/VisualizerTitleBarComboPick.ui
	#ui/VisualizerTitleBarButtonPerSource.ui
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

#------------------------------------------------------------------------------
# CONFIG
set(CONFIG_H
	config/ConfigBuffer.h
	config/ConfigurationGroup.h
	config/FileReader.h
	config/IConfigManager.h
	config/ConfigParameter.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\config" FILES ${CONFIG_H})
set(TARGET_H ${TARGET_H} ${CONFIG_H})

set(CONFIG_SRC
	config/ConfigBuffer.cpp
	config/ConfigurationGroup.cpp
	config/FileReader.cpp
	config/ConfigParameter.cpp
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

# kompilacja do exe
option(EDR_WIN32_ENABLE_CONSOLE "Enable console on Win32?" OFF)
if (EDR_WIN32_ENABLE_CONSOLE)
	add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
else()
	add_executable(${TARGET_TARGETNAME} WIN32 ${TARGET_SRC} ${TARGET_H})
endif()

#instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION bin COMPONENT core)
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(core)
