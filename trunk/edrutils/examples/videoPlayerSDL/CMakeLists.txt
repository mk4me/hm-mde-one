# nazwa
set(TARGET_TARGETNAME videoPlayerSDL)


# Ÿród³a
set(TARGET_SRC 
	videoPlayerSDL.cpp
)

# nag³ówki
set(TARGET_H
)

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Examples ${TARGET_TARGETNAME}")	