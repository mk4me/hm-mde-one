project(video)
# nazwa
set(TARGET_TARGETNAME
	video
)
#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
)
#source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${EDR_ROOT} ${EDR_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
)
#source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
)
#source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

# metadane generowane przez MOC
SET(TARGET_MOC_H
)
#QT4_WRAP_CPP(TARGET_MOC_SRC ${TARGET_MOC_H})
#source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_MOC_SRC})
set(TARGET_SRC ${TARGET_SRC} ${TARGET_MOC_SRC})

#------------------------------------------------------------------------------
# UI
set(TARGET_UI
)
#QT4_WRAP_UI(TARGET_UI_H ${TARGET_UI})
#source_group("${SOURCEGROUP_UI}" FILES ${TARGET_UI_H} ${TARGET_UI})
set(TARGET_H ${TARGET_H} ${TARGET_UI_H} ${TARGET_UI})

#------------------------------------------------------------------------------
# CORE
set(CORE_PRIVATE_H
	core/Converter.h
	core/FFmpegError.h
	core/FFmpegVideoStream.h
	core/Utils.h
	core/VideoBuffer.h
	core/VideoBufferChunk.h
	core/VideoManager.h
	core/VM.h
	core/VMPrivate.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\core" FILES ${CORE_PRIVATE_H})
set(TARGET_H ${TARGET_H} ${CORE_PRIVATE_H})

set(CORE_PUBLIC_H
	${HEADER_PATH}/core/Picture.h
	${HEADER_PATH}/core/PixelFormat.h
	${HEADER_PATH}/core/VideoError.h
	${HEADER_PATH}/core/VideoStream.h
	${HEADER_PATH}/core/BufferedVideoStream.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}\\core" FILES ${CORE_PUBLIC_H})
set(TARGET_H ${TARGET_H} ${CORE_PUBLIC_H})


set(CORE_SRC
	core/BufferedVideoStream.cpp
	core/Converter.cpp
	core/FFmpegVideoStream.cpp
	core/Picture.cpp
	core/PixelFormat.cpp
	core/VideoBuffer.cpp
	core/VideoBufferChunk.cpp
	core/VideoManager.cpp
	core/VideoStream.cpp
)
source_group("${SOURCEGROUP_SOURCES}\\core" FILES ${CORE_SRC})
set(TARGET_SRC ${TARGET_SRC} ${CORE_SRC})

#------------------------------------------------------------------------------
# OSG
set(OSG_H
	osg/BufferedVideoImageStream.h
	osg/VideoImageStream.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\osg" FILES ${OSG_H})
set(TARGET_H ${TARGET_H} ${OSG_H})

set(OSG_SRC
	osg/BufferedVideoImageStream.cpp
	osg/ReaderWriterVMPlugin.cpp
	osg/VideoImageStream.cpp
)
source_group("${SOURCEGROUP_SOURCES}\\osg" FILES ${OSG_SRC})
set(TARGET_SRC ${TARGET_SRC} ${OSG_SRC})

#------------------------------------------------------------------------------
# konfiguracja

# Kopiujemy konfiguracja
configure_file( "core/Config.h.in" "${PROJECT_BINARY_DIR}/core/Config.h" )
set(TARGET_H ${TARGET_H} "${PROJECT_BINARY_DIR}/core/Config.h")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}\\core" FILES "${PROJECT_BINARY_DIR}/core/Config.h")


#------------------------------------------------------------------------------
# tworzenie

# obsluga OpenGL
set(QT_USE_QTOPENGL TRUE) 
include(${QT_USE_FILE})

# kompilacja do wspó³dzielonej biblioteki
add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	

ON_PROJECT_ADDED(${TARGET_TARGETNAME})
