# nazwa
set(TARGET_TARGETNAME communication_core)

#src
set(TARGET_SRC
BasicQueriesService.cpp BasicUpdatesService.cpp DateTime.cpp CommunicationService.cpp EDRException.cpp Entity.cpp File.cpp FileStoremanService.cpp FTPS_cURL.cpp MotionKind.cpp Performer.cpp QueryWSDL.cpp Segment.cpp Session.cpp SessionGroup.cpp TransportWSDL_FTPS.cpp Trial.cpp WSDL_Wsdlpull.cpp
)

#headers
set(TARGET_H
BasicQueriesService.h BasicUpdatesService.h DateTime.h EDRException.h Entity.h CommunicationService.h File.h FileStoremanService.h FTPS_cURL.h IAuthenticable.h IFTP.h IQueryable.h ISendable.h ITransportable.h MotionKind.h Performer.h QueryWSDL.h Segment.h Session.h SessionGroup.h TransportWSDL_FTPS.h Trial.h WSDL_Wsdlpull.h
)

#preprocessor definitions
add_definitions(-DWITH_CURL)

#compile
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})

#Additional Include Directories:
include_directories(${DEFAULT_PROJECT_INCLUDES})

target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})
