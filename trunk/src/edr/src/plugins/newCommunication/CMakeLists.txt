project(newCommunication)
# nazwa
set(TARGET_TARGETNAME
	plugin_newCommunication
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	DataSourceBaseContent.h
	DataSourceContentManager.h
	DownloadStatusWidget.h
	DataSourcePathsManager.h
	NotesWidget.h
	AntropometricDataWidget.h
	DisordersDataWidget.h
	PersonalDataWidget.h
	DefaultPatientCardWidget.h
	PatientCardManager.h
	DataSourceWidget.h
	CommunicationManager.h
	DataSourceConnectionsManager.h
	DataSourceWebServicesManager.h
	DownloadRequest.h
	FilesHelper.h
	DataSourceFilterManager.h
	QMultiToolBox.h
	CommunicationPlugin.h
	DataSource.h
	DataSourceUserPrivilages.h
	DataSourceUserData.h
	DataSourceStatusManager.h
	DataSourceUser.h
	DataSourceLoginManager.h
	DataSourceLocalStorage.h
	DataSourceStatus.h
	DataSourceShallowCopyUtils.h
	DataSourcePrivilagesVerifier.h
	Patient.h
	WSConnection.h
	DataSourcePerspective.h
	DataSourcePerspectiveManager.h
	ServerStatusManager.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/IDataSourceContent.h
	${HEADER_PATH}/IDataSourceContentManager.h
	${HEADER_PATH}/IPatientCard.h
	${HEADER_PATH}/IPatientCardManager.h
	${HEADER_PATH}/IDownloadRequest.h
	${HEADER_PATH}/IFilesHelper.h
	${HEADER_PATH}/DataSourceFilter.h
	${HEADER_PATH}/IDataSourceFilterManager.h
	${HEADER_PATH}/ICommunicationDataSource.h
	${HEADER_PATH}/IPatient.h
	${HEADER_PATH}/IDataSourceUserPrivilages.h
	${HEADER_PATH}/IDataSourceUserData.h
	${HEADER_PATH}/IDataSourceUser.h
	${HEADER_PATH}/DataSourceShallowCopy.h
	${HEADER_PATH}/IDataSourcePerspective.h
	${HEADER_PATH}/IDataSourcePerspectiveManager.h
	${HEADER_PATH}/IDataSourceStatus.h
	${HEADER_PATH}/IDataSourceStatusManager.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	DataSourceBaseContent.cpp
	DataSourceContentManager.cpp
	DownloadStatusWidget.cpp
	DataSourcePathsManager.cpp
	NotesWidget.cpp
	AntropometricDataWidget.cpp
	DisordersDataWidget.cpp
	PersonalDataWidget.cpp
	DefaultPatientCardWidget.cpp
	PatientCardManager.cpp
	DataSourceWidget.cpp
	DataSourceConnectionsManager.cpp
	DataSourceWebServicesManager.cpp
	DownloadRequest.cpp
	FilesHelper.cpp
	DataSourceFilterManager.cpp
	QMultiToolBox.cpp
	Patient.cpp
	CommunicationManager.cpp
	CommunicationPlugin.cpp
	WSConnection.cpp
	DataSource.cpp
	DataSourceStatusManager.cpp
	DataSourceUserPrivilages.cpp
	DataSourceUserData.cpp
	DataSourceUser.cpp
	DataSourceLoginManager.cpp
	DataSourceLocalStorage.cpp
	DataSourceStatus.cpp
	DataSourceShallowCopyUtils.cpp
	DataSourcePrivilagesVerifier.cpp
	DataSourcePerspective.cpp
	DataSourcePerspectiveManager.cpp
	ServerStatusManager.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	DownloadStatusWidget.h
	DataSourceWidget.h
	QMultiToolBox.h
	DefaultPatientCardWidget.h
	PersonalDataWidget.h
	AntropometricDataWidget.h
	DisordersDataWidget.h
	NotesWidget.h
	DataSourcePerspective.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/DownloadStatusWidget.ui
	ui/DataSourceWidget.ui
	ui/DefaultPatientCardWidget.ui
	ui/PersonalDataWidget.ui
	ui/NotesWidget.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------

# RC
set(TARGET_RC
	ui/Icons.qrc
)
QT4_ADD_RESOURCES(TARGET_RCC_SRC ${TARGET_RC})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_RC} ${TARGET_RCC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_RCC_SRC} ${TARGET_RC})
#------------------------------------------------------------------------------
# prekompilowany nag³ówek

set(PRECOMPILED_H CommunicationPCH.h)

set(PRECOMPILED_SRC CommunicationPCH.cpp)

SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")

source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})

set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})

source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})

set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
#instalacja
install(FILES ${PUBLIC_H} DESTINATION include/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/plugins/${PROJECT_NAME} ARCHIVE DESTINATION lib/plugins/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
