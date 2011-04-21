project(communication)
# nazwa
set(TARGET_TARGETNAME
	plugin_communication
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	CommunicationPlugin.h
	CommunicationWidgetEx.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/BasicQueriesService.h
	${HEADER_PATH}/BasicUpdatesService.h
	${HEADER_PATH}/CommunicationManager.h
	${HEADER_PATH}/CommunicationService.h
	${HEADER_PATH}/Entity.h
	${HEADER_PATH}/FileStoremanService.h
	${HEADER_PATH}/FtpsConnection.h
	${HEADER_PATH}/ICommunication.h
	${HEADER_PATH}/QueryWSDL.h
	${HEADER_PATH}/MetadataParser.h
	${HEADER_PATH}/ShallowCopyParser.h
	${HEADER_PATH}/TransportWSDL_FTPS.h
	${HEADER_PATH}/WsdlConnection.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
#	main.cpp
	BasicQueriesService.cpp
	BasicUpdatesService.cpp
	CommunicationManager.cpp
	CommunicationPlugin.cpp
	CommunicationService.cpp
	CommunicationWidgetEx.cpp
	FileStoremanService.cpp
	FtpsConnection.cpp
	QueryWSDL.cpp
	MetadataParser.cpp
	ShallowCopyParser.cpp
	TransportWSDL_FTPS.cpp
	WsdlConnection.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	CommunicationWidgetEx.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/CommunicationWidgetEx.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

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
install(TARGETS ${TARGET_TARGETNAME} DESTINATION bin COMPONENT communication)
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
