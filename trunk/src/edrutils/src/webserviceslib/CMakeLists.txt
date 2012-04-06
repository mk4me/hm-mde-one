project(webserviceslib)
# nazwa
set(TARGET_TARGETNAME
	webserviceslib
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
#set(PRIVATE_H
#)

# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/IAccountFactoryWS.h
	${HEADER_PATH}/AccountFactoryWS.h
	${HEADER_PATH}/IWSConnection.h
	${HEADER_PATH}/IDownloadHelper.h
	${HEADER_PATH}/IFtpsConnection.h
	${HEADER_PATH}/DownloadHelper.h
	${HEADER_PATH}/IWebService.h
	${HEADER_PATH}/IAdministrationWS.h
	${HEADER_PATH}/AdministrationWS.h
	${HEADER_PATH}/IAuthorizationWS.h
	${HEADER_PATH}/AuthorizationWS.h
	${HEADER_PATH}/IBasicQueriesWS.h
	${HEADER_PATH}/BasicQueriesWS.h
	${HEADER_PATH}/IBasicUpdatesWS.h
	${HEADER_PATH}/BasicUpdatesWS.h
	${HEADER_PATH}/IFileStoremanWS.h
	${HEADER_PATH}/FileStoremanWS.h
	${HEADER_PATH}/IUserPersonalSpaceWS.h
	${HEADER_PATH}/UserPersonalSpaceWS.h
	${HEADER_PATH}/IWSCredentials.h
	${HEADER_PATH}/MetadataParser.h
	${HEADER_PATH}/ShallowCopyParser.h
	${HEADER_PATH}/FtpsConnection.h
	${HEADER_PATH}/DateTime.h
	${HEADER_PATH}/DateTimeUtils.h
	${HEADER_PATH}/Entity.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	AccountFactoryWS.cpp
	DownloadHelper.cpp
	AdministrationWS.cpp
	AuthorizationWS.cpp
	BasicUpdatesWS.cpp
	BasicQueriesWS.cpp
	FileStoremanWS.cpp	
	UserPersonalSpaceWS.cpp
	MetadataParser.cpp
	ShallowCopyParser.cpp
	FtpsConnection.cpp
	DateTime.cpp
	DateTimeUtils.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})

#instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})