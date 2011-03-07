set(TARGET_TARGETNAME
	QtSample
)

#------------------------------------------------------------------------------
# nag³ówki
set(TARGET_H
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${TARGET_H})

#------------------------------------------------------------------------------
# Ÿród³a
set(TARGET_SRC 
	QtSample.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------
# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Examples ${TARGET_TARGETNAME}")