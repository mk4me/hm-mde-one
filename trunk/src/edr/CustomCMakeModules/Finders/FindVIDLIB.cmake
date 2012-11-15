# przygotowanie do szukania
FIND_INIT_EDRUTILS_MODULE(VIDLIB vidlib)

# szukanie
FIND_SHARED(VIDLIB "vidlib" "vidlib")

# skopiowanie
FIND_FINISH(VIDLIB)

if (NOT FFMPEG_FOUND)
	message(WARNING "FFMPEG nie zostal znaleziony wiec nie mozna dodac VIDLIB")
	set(VIDLIB_FOUND 0)
endif()