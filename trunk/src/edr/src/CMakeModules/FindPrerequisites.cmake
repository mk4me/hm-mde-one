if (WIN32) 
	set(PREREQUISITES_FOUND 1)
elseif (UNIX)
	# przygotowanie do szukania
	FIND_INIT(PREREQUISITES prerequisites)

	# szukanie
	#FIND_SHARED(ZLIB "z<lib,?>" "zlib<lib,?>")
	#FIND_GLOB(PREREQUISITES_LIB_ "lib.so.*.*.*" "lib.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_AV_CODEC "libavcodec.so.*.*.*" "libavcodec.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_AV_DEVICE "libavdevice.so.*.*.*" "libavdevice.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_AV_FILTER "libavfilter.so.*.*.*" "libavfilter.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_AV_FORMAT "libavformat.so.*.*.*" "libavformat.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_AV_UTIL "libavutil.so.*.*.*" "libavutil.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_APR "libapr*.so.*.*.*" "libcrypto.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_CRYPTO "libcrypto.so.*.*.*" "libcrypto.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_FREETYPE "libfreetype.so.*.*.*" "libfreetype.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_GIF "libgif.so.*.*.*" "libgif.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_GLUT "libglut.so.*.*" "libglut.so.*.*")
	FIND_GLOB(PREREQUISITES_LIB_JPEG "libjpeg.so.*.*.*" "libjpeg.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_MNG "libmng.so.*.*.*" "libmng.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_PNG15 "libpng15.so.*.*.*" "libpng15.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_SSL "libssl.so.*.*.*" "libssl.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_SWRESAMPLE "libswresample.so.*.*.*" "libswresample.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_SWSCALE "libswscale.so.*.*.*" "libswscale.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_TIFF "libtiff.so.*.*.*" "libtiff.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_TIFFXX "libtiffxx.so.*.*.*" "libtiffxx.so.*.*.*")

	if (
	#PREREQUISITES_LIB_AV_CODEC_FOUND AND
	#PREREQUISITES_LIB_AV_DEVICE_FOUND AND
	#PREREQUISITES_LIB_AV_FILTER_FOUND AND
	#PREREQUISITES_LIB_AV_FORMAT_FOUND AND
	#PREREQUISITES_LIB_AV_UTIL_FOUND AND
	PREREQUISITES_LIB_APR_FOUND AND
	PREREQUISITES_LIB_CRYPTO_FOUND AND
	PREREQUISITES_LIB_FREETYPE_FOUND AND
	PREREQUISITES_LIB_GIF_FOUND AND
	PREREQUISITES_LIB_GLUT_FOUND AND
	PREREQUISITES_LIB_JPEG_FOUND AND
	PREREQUISITES_LIB_MNG_FOUND AND
	PREREQUISITES_LIB_PNG15_FOUND AND 
	PREREQUISITES_LIB_SSL_FOUND AND 
	#PREREQUISITES_LIB_SWRESAMPLE_FOUND AND
	#PREREQUISITES_LIB_SWSCALE_FOUND AND
	PREREQUISITES_LIB_TIFF_FOUND AND
	PREREQUISITES_LIB_TIFFXX_FOUND)
		set(PREREQUISITES_FOUND 1)
	endif()
	# skopiowanie
	FIND_FINISH(PREREQUISITES)
endif()
