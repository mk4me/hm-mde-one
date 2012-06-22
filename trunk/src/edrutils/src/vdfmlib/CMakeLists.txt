project(vdfmlib)
# nazwa
set(TARGET_TARGETNAME
	vdfmlib
)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)

# source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/osgVDFBaseModel.h
	${HEADER_PATH}/osgVDFBaseNode.h
	${HEADER_PATH}/osgVDFBasePin.h
	${HEADER_PATH}/osgVDFModel.h
	${HEADER_PATH}/osgVDFNode.h
	${HEADER_PATH}/osgVDFNodeTypeDescriptor.h
	${HEADER_PATH}/osgVDFPin.h
	${HEADER_PATH}/osgVDFRectangleSelectionFactory.h
	${HEADER_PATH}/osgVDFLinearConnectionFactory.h
	${HEADER_PATH}/osgVDFBezieerConnectionFactory.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	osgVDFBaseModel.cpp
	osgVDFBaseNode.cpp
	osgVDFBasePin.cpp
	osgVDFModel.cpp
	osgVDFNode.cpp
	osgVDFPin.cpp
	osgVDFNodeTypeDescriptor.cpp
	osgVDFRectangleSelectionFactory.cpp
	osgVDFLinearConnectionFactory.cpp
	osgVDFBezieerConnectionFactory.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H StdAfx.h)
set(PRECOMPILED_SRC StdAfx.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})

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
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib/${PROJECT_NAME} ARCHIVE DESTINATION lib/${PROJECT_NAME} COMPONENT ${PROJECT_NAME})
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})