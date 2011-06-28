# przygotowanie do szukania
FIND_INIT2(VIDLIB vidlib "${PROJECT_INCLUDE_ROOT}/../../edrutils/include" "../../../_out/edrutils/bin/Debug" "../../../_out/edrutils/bin/Release")
FIND_INCLUDE_PLATFORM_HEADERS2(VIDLIB vidlib "${PROJECT_INCLUDE_ROOT}/../../../_out/edrutils")

# szukanie
FIND_SHARED(VIDLIB_LIB "vidlib" "vidlib")
#FIND_MODULE(VIDLIB_LIBAVCORE "avcore<-0,?>" "avcore-0")
#FIND_MODULE(VIDLIB_LIBAVCODEC "avcodec<-52,?>" "avcodec-52")
#FIND_MODULE(VIDLIB_LIBAVFORMAT "avformat<-52,?>" "avformat-52")
#FIND_MODULE(VIDLIB_LIBAVDEVICE "avdevice<-52,?>" "avdevice-52")
#FIND_MODULE(VIDLIB_LIBAVUTIL "avutil<-50,?>" "avutil-50")
#FIND_MODULE(VIDLIB_LIBSWSCALE "swscale<-0,?>" "swscale-0")

# skopiowanie
FIND_FINISH(VIDLIB)

# sprawdzenie
if (FIND_RESULTS_LOGICAL_AND)
	set(VIDLIB_FOUND 1)
endif()