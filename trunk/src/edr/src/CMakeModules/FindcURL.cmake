# przygotowanie do szukania
FIND_INIT(CURL curl)

# szukanie
FIND_SHARED(CURL_LIBCURL "libcurl<d,?><_imp,?>" "libcurl")

# skopiowanie
FIND_FINISH(CURL)

if (CURL_LIBCURL_FOUND)
	set(CURL_FOUND 1)
	if (WIN32)
		add_definitions(-D_WINSOCKAPI_)
	elseif (UNIX)
		add_definitions(-DHAVE_SYS_SOCKET_H)
	endif()
	#include (CheckIncludeFile)
	#include (CheckIncludeFiles)
	#CHECK_INCLUDE_FILES("sys/socket.h" HAVE_SYS_SOCKET_H)
 endif()
