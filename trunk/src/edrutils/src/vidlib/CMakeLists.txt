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

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")

END_PROJECT()
