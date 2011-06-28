# nazwa
set(TARGET_TARGETNAME vidlib_test)

# Ÿród³a
set(TARGET_SRC 
	test.cpp
	FFmpegVideoStreamTest.cpp
	OsgAdapterTest.cpp
)

# nag³ówki
set(TARGET_H
	FFmpegVideoStreamTest.h
)

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Test of vidlib")	