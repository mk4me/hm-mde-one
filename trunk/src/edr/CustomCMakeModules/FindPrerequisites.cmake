# przygotowanie do szukania
FIND_INIT(PREREQUISITES prerequisites)
unset(PREREQUISITES_INCLUDE_DIR CACHE)
if (WIN32) 
	#FIND_DLL(PREREQUISITES_LIB_APR "libapr*.so.*.*.*" "libcrypto.so.*.*.*")
	#FIND_DLL(PREREQUISITES_LIB_CRYPTO "libcrypto.so.*.*.*" "libcrypto.so.*.*.*")
	#FIND_DLL(PREREQUISITES_LIB_FREETYPE "libfreetype.so.*.*.*" "libfreetype.so.*.*.*")
	####FIND_DLL(PREREQUISITES_LIB_GIF "giflib4" "giflib4")
	FIND_DLL(PREREQUISITES_LIB_GLUT "glut32" "glut32")
	#FIND_DLL(PREREQUISITES_LIB_JPEG "libjpeg.so.*.*.*" "libjpeg.so.*.*.*")
	#FIND_DLL(PREREQUISITES_LIB_MNG "libmng.so.*.*.*" "libmng.so.*.*.*")
	FIND_DLL(PREREQUISITES_LIB_EAY "libeay32" "libeay32")
	FIND_DLL(PREREQUISITES_LIB_PNG15 "libpng15" "libpng15d")
	FIND_DLL(PREREQUISITES_LIB_SSL "ssleay32" "ssleay32")
	FIND_DLL(PREREQUISITES_LIB_TIFF "libtiff" "libtiffd")
	#FIND_DLL(PREREQUISITES_LIB_TIFFXX "libtiffxx.so.*.*.*" "libtiffxx.so.*.*.*")
elseif (UNIX)
	#FIND_GLOB(PREREQUISITES_LIB_APR "libapr*.so.*.*.*" "libcrypto.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_CRYPTO "libcrypto.so.*.*.*" "libcrypto.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_FREETYPE "libfreetype.so.*.*.*" "libfreetype.so.*.*.*")
	####FIND_GLOB(PREREQUISITES_LIB_GIF "libgif.so.*.*.*" "libgif.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_GLUT "libglut.so.*.*" "libglut.so.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_JPEG "libjpeg.so.*.*.*" "libjpeg.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_MNG "libmng.so.*.*.*" "libmng.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_EAY "libeay.so.*.*" "libeay.so.*.*")
	FIND_GLOB(PREREQUISITES_LIB_PNG15 "libpng15.so.*.*.*" "libpng15.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_SSL "libssl.so.*.*.*" "libssl.so.*.*.*")
	FIND_GLOB(PREREQUISITES_LIB_TIFF "libtiff.so.*.*.*" "libtiff.so.*.*.*")
	#FIND_GLOB(PREREQUISITES_LIB_TIFFXX "libtiffxx.so.*.*.*" "libtiffxx.so.*.*.*")
endif()

if (
	#PREREQUISITES_LIB_APR_FOUND AND
	#PREREQUISITES_LIB_CRYPTO_FOUND AND
	#PREREQUISITES_LIB_FREETYPE_FOUND AND
	####PREREQUISITES_LIB_GIF_FOUND AND
	PREREQUISITES_LIB_GLUT_FOUND AND
	#PREREQUISITES_LIB_JPEG_FOUND AND
	#PREREQUISITES_LIB_MNG_FOUND 
	PREREQUISITES_LIB_EAY_FOUND AND
	PREREQUISITES_LIB_PNG15_FOUND AND 
	PREREQUISITES_LIB_SSL_FOUND AND 
	PREREQUISITES_LIB_TIFF_FOUND) # AND PREREQUISITES_LIB_TIFFXX_FOUND)
		set(PREREQUISITES_FOUND 1)
	endif()
# skopiowanie
FIND_FINISH(PREREQUISITES)