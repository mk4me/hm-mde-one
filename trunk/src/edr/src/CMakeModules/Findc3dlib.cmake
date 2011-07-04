# przygotowanie do szukania
FIND_INIT2(C3DLIB c3dlib "${PROJECT_INCLUDE_ROOT}/../../edrutils/include" "../../../_out/edrutils/bin/Debug" "../../../_out/edrutils/bin/Release")
FIND_INCLUDE_PLATFORM_HEADERS2(C3DLIB c3dlib "${PROJECT_INCLUDE_ROOT}/../../../_out/edrutils")

# szukanie
FIND_SHARED(C3DLIB "c3dlib" "c3dlib")

# skopiowanie
FIND_FINISH(C3DLIB)