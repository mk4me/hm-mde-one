# przygotowanie do szukania
FIND_INIT2(OSGUI osgui "${PROJECT_INCLUDE_ROOT}/../../edrutils/include" "../../../_out/edrutils/bin/Debug" "../../../_out/edrutils/bin/Release")
FIND_INCLUDE_PLATFORM_HEADERS2(OSGUI osgui "${PROJECT_INCLUDE_ROOT}/../../../_out/edrutils")

# szukanie
FIND_STATIC(OSGUI "osgui")

# skopiowanie
FIND_FINISH(OSGUI)