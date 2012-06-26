project(qthtmlgenlib)
# nazwa
set(TARGET_TARGETNAME
	qthtmlgenlib
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
#set(PRIVATE_H
#)

# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/TableGenerator.h
	${HEADER_PATH}/DocumentGenerator.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	TableGenerator.cpp
	DocumentGenerator.cpp
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

#instalacja
install(TARGETS ${TARGET_TARGETNAME} LIBRARY DESTINATION lib/${PROJECT_NAME} ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev )
install(FILES ${PUBLIC_H} DESTINATION include/${PROJECT_NAME} COMPONENT ${PROJECT_NAME}_dev)

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})