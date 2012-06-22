# przygotowanie do szukania
FIND_INIT(ZLIB zlib)

# szukanie
FIND_SHARED_EXT(ZLIB "zlib" "zlibd" "zlib" "zlibd")
# koniec
FIND_FINISH(ZLIB)