project(vidlib)
# nazwa
set(TARGET_TARGETNAME
	vidlib
)

CONFIG_OPTION(VIDLIB_ENABLE_EXCEPTIONS "Enable exeptions?" ON)
CONFIG_OPTION(VIDLIB_LIBRARY_STATIC "Build static library?" OFF)
add_definitions(-DVIDLIB_LIBRARY)

#------------------------------------------------------------------------------

# nag³ówki prywatne
set(PRIVATE_H
	Converter.h
	VideoBufferChunk.h
	VidLibPrivate.h
)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRIVATE_H})

# nag³ówki publiczne
string(REPLACE ${PROJECT_ROOT} ${PROJECT_INCLUDE_ROOT} HEADER_PATH ${PROJECT_SOURCE_DIR})
set(PUBLIC_H
	${HEADER_PATH}/BufferedVideoStream.h
	${HEADER_PATH}/Picture.h
	${HEADER_PATH}/PixelFormat.h
	${HEADER_PATH}/VideoError.h
	${HEADER_PATH}/VideoStream.h
	${HEADER_PATH}/VideoBuffer.h
	${HEADER_PATH}/FFmpegError.h
	${HEADER_PATH}/FFmpegVideoStream.h
	${HEADER_PATH}/Export.h
)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES ${PUBLIC_H})
set(TARGET_H ${PRIVATE_H} ${PUBLIC_H})

#------------------------------------------------------------------------------

# Ÿród³a
set(TARGET_SRC
	BufferedVideoStream.cpp
	Converter.cpp
	FFmpegError.cpp
	FFmpegVideoStream.cpp
	Picture.cpp
	VideoBuffer.cpp
	VideoBufferChunk.cpp
	VideoStream.cpp
	VideoError.cpp
)
source_group("${SOURCEGROUP_SOURCES}" FILES ${TARGET_SRC})

#------------------------------------------------------------------------------

if (CONFIG_VIDLIB_ENABLE_OSG)
	# OSG
	set(OSG_H
		${HEADER_PATH}/osg/BufferedVideoImageStream.h
		${HEADER_PATH}/osg/VideoImageStream.h
		${HEADER_PATH}/osg/VideoImageStreamSizeOptimizer.h
		${HEADER_PATH}/osg/GenericImageStream.h
		${HEADER_PATH}/osg/ReaderWriterVidLib.h
		${HEADER_PATH}/osg/FFmpegImageStream.h
		${HEADER_PATH}/osg/VideoImage.h
		${HEADER_PATH}/osg/OsgAdapter.h
	)
	source_group("${SOURCEGROUP_PUBLIC_HEADERS}\\osg" FILES ${OSG_H})
	set(TARGET_H ${TARGET_H} ${OSG_H})

	set(OSG_SRC
		osg/BufferedVideoImageStream.cpp
		osg/ReaderWriterVidLib.cpp
		osg/VideoImageStream.cpp
		osg/VideoImageStreamSizeOptimizer.cpp
		osg/FFmpegImageStream.cpp
		osg/VideoImage.cpp
		osg/OsgAdapter.cpp
	)
	source_group("${SOURCEGROUP_SOURCES}\\osg" FILES ${OSG_SRC})
	set(TARGET_SRC ${TARGET_SRC} ${OSG_SRC})
endif()

#------------------------------------------------------------------------------
# konfiguracja

# Kopiujemy konfiguracja
configure_file( "Config.h.in" "${PROJECT_BINARY_DIR}/Config.h" )
set(TARGET_H ${TARGET_H} "${PROJECT_BINARY_DIR}/Config.h" Config.h.in)
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES Config.h.in)
source_group("${SOURCEGROUP_PUBLIC_HEADERS}" FILES "${PROJECT_BINARY_DIR}/Config.h")

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
set(PRECOMPILED_H PCH.h)
set(PRECOMPILED_SRC PCH.cpp)
SET_PRECOMPILED_HEADER(${PRECOMPILED_H} ${PRECOMPILED_SRC} "${TARGET_SRC}")
source_group("${SOURCEGROUP_PRIVATE_HEADERS}" FILES ${PRECOMPILED_H})
set(TARGET_H ${TARGET_H} ${PRECOMPILED_H})
source_group("${SOURCEGROUP_SOURCES}" FILES ${PRECOMPILED_SRC})
set(TARGET_SRC ${TARGET_SRC} ${PRECOMPILED_SRC})


#------------------------------------------------------------------------------
# tworzenie

# kompilacja do wspó³dzielonej biblioteki
if (CONFIG_VIDLIB_LIBRARY_STATIC)
	add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
else()
	add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
endif()

#instalacja
install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib COMPONENT vidlib)
install(FILES ${PUBLIC_H} DESTINATION include/${TARGET_TARGETNAME})
install(FILES ${OSG_H} DESTINATION include/${TARGET_TARGETNAME}/osg)
install(FILES "${PROJECT_BINARY_DIR}/Config.h" DESTINATION include/${TARGET_TARGETNAME})
# nag³ówki
include_directories(${DEFAULT_PROJECT_INCLUDES})
# biblioteki
target_link_libraries(${TARGET_TARGETNAME} ${DEFAULT_PROJECT_LIBS})	
# nazwa
set_target_properties(${TARGET_TARGETNAME} PROPERTIES PROJECT_LABEL "Library ${TARGET_TARGETNAME}")


ON_PROJECT_ADDED(${TARGET_TARGETNAME})
