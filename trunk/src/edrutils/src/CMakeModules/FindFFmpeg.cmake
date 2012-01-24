# przygotowanie do szukania
FIND_INIT(FFMPEG ffmpeg)

# szukanie
if (FFMPEG_ONLY_MODULES)
	unset(FFMPEG_INCLUDE_DIR CACHE)
	FIND_MODULE(FFMPEG_LIBAVCORE FALSE "avcore<-0,?>")
	FIND_MODULE(FFMPEG_LIBAVCODEC FALSE "avcodec<-52,?>")
	FIND_MODULE(FFMPEG_LIBAVFORMAT FALSE "avformat<-52,?>")
	FIND_MODULE(FFMPEG_LIBAVDEVICE FALSE "avdevice<-52,?>")
	FIND_MODULE(FFMPEG_LIBAVUTIL FALSE "avutil<-50,?>")
	FIND_MODULE(FFMPEG_LIBSWSCALE FALSE "swscale<-0,?>")	
else()
	FIND_SHARED(FFMPEG_LIBAVCORE "avcore<-0,?>" "avcore-0")
	FIND_SHARED(FFMPEG_LIBAVCODEC "avcodec<-52,?>" "avcodec-52")
	FIND_SHARED(FFMPEG_LIBAVFORMAT "avformat<-52,?>" "avformat-52")
	FIND_SHARED(FFMPEG_LIBAVDEVICE "avdevice<-52,?>" "avdevice-52")
	FIND_SHARED(FFMPEG_LIBAVUTIL "avutil<-50,?>" "avutil-50")
	FIND_SHARED(FFMPEG_LIBSWSCALE "swscale<-0,?>" "swscale-0")
endif()

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