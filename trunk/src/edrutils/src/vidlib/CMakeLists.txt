BEGIN_PROJECT("dynamic")


CONFIG_OPTION(VIDLIB_ENABLE_EXCEPTIONS "Enable exeptions?" ON)
CONFIG_OPTION(VIDLIB_LIBRARY_STATIC "Build static library?" OFF)
add_definitions(-DVIDLIB_LIBRARY)

#------------------------------------------------------------------------------

SET_PRIVATE_HEADERS(
	Converter.h
	VideoBufferChunk.h
	VidLibPrivate.h
)

set (PUBLIC 
	BufferedVideoStream.h
	Picture.h
	PixelFormat.h
	VideoError.h
	VideoStream.h
	VideoBuffer.h
	FFmpegError.h
	FFmpegVideoStream.h
	Export.h
	FileSequenceVideoStream.h
	VideoStreamAdapter.h
	)
	
set(SOURCE
	BufferedVideoStream.cpp
	Converter.cpp
	FFmpegError.cpp
	FFmpegVideoStream.cpp
	Picture.cpp
	VideoBuffer.cpp
	VideoBufferChunk.cpp
	VideoStream.cpp
	VideoError.cpp
	FileSequenceVideoStream.cpp
	VideoStreamAdapter.cpp
	)



if (CONFIG_VIDLIB_ENABLE_OSG)
	# OSG
	set(PUBLIC ${PUBLIC}
		osg/BufferedVideoImageStream.h
		osg/VideoImageStream.h
		osg/VideoImageStreamSizeOptimizer.h
		osg/GenericImageStream.h
		osg/ReaderWriterVidLib.h
		osg/FFmpegImageStream.h
		osg/VideoImage.h
		osg/OsgAdapter.h
	)
	
	set(SOURCE ${SOURCE}
		osg/BufferedVideoImageStream.cpp
		osg/ReaderWriterVidLib.cpp
		osg/VideoImageStream.cpp
		osg/VideoImageStreamSizeOptimizer.cpp
		osg/FFmpegImageStream.cpp
		osg/VideoImage.cpp
		osg/OsgAdapter.cpp
	)
endif()

SET_PUBLIC_HEADERS(
	${PUBLIC}
)


SET_SOURCE_FILES(
	${SOURCE}
)
#------------------------------------------------------------------------------
# konfiguracja
#SET_CONFIGURATION_INPUT_FILES( Config.h.in )
#set(CONFIG_INPUT_FILE Config.h.in)
configure_file(configuration/Config.h.in  "${PROJECT_BINARY_DIR}/${PROJECT_NAME}/Config.h" )
# Kopiujemy konfiguracja
#configure_file( ${CONFIG_INPUT_FILE} "${PROJECT_BINARY_DIR}/Config.h" )
#source_group("Configuration template" FILES ${CONFIG_INPUT_FILE})
#source_group("Local configuration" FILES "${PROJECT_BINARY_DIR}/Config.h")



# # kompilacja do wspó³dzielonej biblioteki
# if (CONFIG_VIDLIB_LIBRARY_STATIC)
	# add_library(${TARGET_TARGETNAME} STATIC ${TARGET_SRC} ${TARGET_H})
# else()
	# add_library(${TARGET_TARGETNAME} SHARED ${TARGET_SRC} ${TARGET_H})
	# if(WIN32)
		# install(TARGETS ${TARGET_TARGETNAME} RUNTIME DESTINATION bin COMPONENT ${PROJECT_NAME})
	# endif(WIN32)
# endif()

END_PROJECT()
