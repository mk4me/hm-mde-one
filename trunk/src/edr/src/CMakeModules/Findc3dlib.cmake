# przygotowanie do szukania
FIND_INIT(C3DLIB c3dlib)
FIND_INCLUDE_PLATFORM_HEADERS(C3DLIB c3dlib)

# szukanie
FIND_SHARED(C3DLIB "c3dlib" "c3dlib")

# skopiowanie
FIND_FINISH(C3DLIB)