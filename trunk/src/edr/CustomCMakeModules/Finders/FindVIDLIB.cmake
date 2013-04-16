# przygotowanie do szukania
FIND_INIT_EDRUTILS_MODULE(VIDLIB vidlib)

# szukanie
FIND_SHARED(VIDLIB "vidlib" "vidlib")

# skopiowanie
FIND_FINISH(VIDLIB)

if(LIBRARY_VIDLIB_FOUND EQUAL 1)
	FIND_DEPENDENCIES(VIDLIB LIBRARY_VIDLIB_FOUND "FFMPEG")
endif()

#if (NOT FFMPEG_FOUND)
#	message(WARNING "FFMPEG nie zostal znaleziony wiec nie mozna dodac VIDLIB")
#	set(VIDLIB_FOUND 0)
#endif()