# przygotowanie do szukania
FIND_INIT(FFMPEG ffmpeg)


# szukanie
FIND_SHARED(FFMPEG_LIBAVCORE "avcore<-0,?>" "avcore-0")
FIND_SHARED(FFMPEG_LIBAVCODEC "avcodec<-52,?>" "avcodec-52")
FIND_SHARED(FFMPEG_LIBAVFORMAT "avformat<-52,?>" "avformat-52")
FIND_SHARED(FFMPEG_LIBAVDEVICE "avdevice<-52,?>" "avdevice-52")
FIND_SHARED(FFMPEG_LIBAVUTIL "avutil<-50,?>" "avutil-50")
FIND_SHARED(FFMPEG_LIBSWSCALE "swscale<-0,?>" "swscale-0")

# skopiowanie
FIND_FINISH(FFMPEG)

# sprawdzenie
if (FFMPEG_LIBAVCORE_FOUND AND
	FFMPEG_LIBAVCODEC_FOUND AND
	FFMPEG_LIBAVFORMAT_FOUND AND
	FFMPEG_LIBAVDEVICE_FOUND AND
	FFMPEG_LIBAVUTIL_FOUND AND
	FFMPEG_LIBSWSCALE_FOUND)
	set(FFMPEG_FOUND 1)
endif()