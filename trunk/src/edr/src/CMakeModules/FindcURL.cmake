# przygotowanie do szukania
FIND_INIT(CURL curl)

# szukanie
FIND_SHARED(CURL_LIBCURL "libcurl<d,?><_imp,?>" "libcurl")

# skopiowanie
FIND_FINISH(CURL)

if (CURL_LIBCURL_FOUND)
	set(CURL_FOUND 1)
	if (WIN32)
		set(CURL_CUSTOM_COMPILER_DEFINITIONS _WINSOCKAPI_)
	elseif (UNIX)
		set(CURL_CUSTOM_COMPILER_DEFINITIONS HAVE_SYS_SOCKET_H)
	endif()
 endif()
