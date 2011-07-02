project(c3dlib)
# nazwa
set(TARGET_TARGETNAME c3dlib)

# nag³ówki prywatne
set(PRIVATE_H
	#stdafx.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/C3DParser.h
	${HEADER_PATH}/Export.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------
# konfiguracja

# Kopiujemy konfiguracja
#configure_file( "Config.h.in" "${PROJECT_BINARY_DIR}/Config.h" )
#set(TARGET_H ${TARGET_H} "${PROJECT_BINARY_DIR}/Config.h" Config.h.in)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES Config.h.in)
#source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES "${PROJECT_BINARY_DIR}/Config.h")

#------------------------------------------------------------------------------


# Ÿród³a
set(TARGET_SRC 
	C3DParser.cpp
	)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})
#set(TARGET_SRC ${TARGET_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
# set(PRECOMPILED_H stdafx.h)
# set(PRECOMPILED_SRC stdafx.cpp)
# SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
# set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
# source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
# set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")

# instalacja
install(TARGETS ${TARGET_TARGETNAME} DESTINATION lib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
#install(FILES "${PROJECT_BINARY_DIR}/Config.h" DESTINATION include/${TARGET_TARGETNAME})

# ¿eby mo¿na by³o u¿yæ tej biblioteki w makrach
ON_PROJECT_ADDED(${TARGET_TARGETNAME})