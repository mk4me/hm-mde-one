project(communication)
# nazwa
set(TARGET_TARGETNAME
	communication
)

#add_definitions(-D__LIBRARY_DYNAMIC__)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	commserv.h SettingsDialog.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
#	${HEADER_PATH}/commserv.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	main.cpp commserv.cpp SettingsDialog.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
	commserv.h SettingsDialog.h
)
QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
	ui/commserv.ui
)
QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# CORE
set(CORE_PRIVATE_H
	core/BasicQueriesService.h
	core/BasicUpdatesService.h
	core/CommunicationService.h
	core/DateTime.h
	core/EDRException.h
	core/Entity.h
	core/IQueryable.h
	core/ISendable.h
	core/ITransportable.h
	core/MotionKind.h
	core/Performer.h
	core/QueryWSDL.h
	core/Segment.h
	core/Session.h
	core/SessionGroup.h
	core/TransportWSDL_FTPS.h
	core/Trial.h
	core/WSDL_Wsdlpull.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\core" FILES ${CORE_PRIVATE_H})
set(TARGET_H ${TARGET_H} ${CORE_PRIVATE_H})

set(CORE_PUBLIC_H
#	${HEADER_PATH}/core/CommunicationService.h
#	${HEADER_PATH}/core/TransportWSDL_FTPS.h
#	${HEADER_PATH}/core/WSDL_Wsdlpull.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}\\core" FILES ${CORE_PUBLIC_H})
set(TARGET_H ${TARGET_H} ${CORE_PUBLIC_H})


set(CORE_SRC
	core/BasicQueriesService.cpp
	core/BasicUpdatesService.cpp
	core/DateTime.cpp
	core/CommunicationService.cpp
	core/EDRException.cpp
	core/Entity.cpp
	core/File.cpp
	core/FileStoremanService.cpp
	core/FTPS_cURL.cpp
	core/MotionKind.cpp
	core/Performer.cpp
	core/QueryWSDL.cpp
	core/Segment.cpp
	core/Session.cpp
	core/SessionGroup.cpp
	core/TransportWSDL_FTPS.cpp
	core/Trial.cpp
	core/WSDL_Wsdlpull.cpp
)
source_group("${SOURCEGROUP_SOURCES}\\core" FILES ${CORE_SRC})
set(TARGET_SRC ${TARGET_SRC} ${CORE_SRC})

#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
#add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
