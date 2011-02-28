# nazwa
set(TARGET_TARGETNAME KinematicModelLibrary_test)

# Ÿród³a
set(TARGET_SRC 
	test.cpp
	AsfParserTest.cpp
	AmcParserTest.cpp
	BvhParserTest.cpp
	MiscTest.cpp
	KinematicModelTest.cpp
)

# nag³ówki
set(TARGET_H
	AsfParserTest.h
	AmcParserTest.h
	BvhParserTest.h
	MiscTest.h
	KinematicModelTest.h
)

# kompilacja do wspó³dzielonej biblioteki
add_executable(${TARGET_TARGETNAME} ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories( ${DEFAULT_PROJECT_INCLUDES} )
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# labelka
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Test of KinematicModelLibrary")	