BEGIN_PROJECT("static")

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")

SET_SOURCE_FILES(
	Filesystem.cpp
	#OsgSceneDump.cpp
	CorePCH.cpp
)

SET_PRIVATE_HEADERS(
	CorePCH.h
)

SET_PRECOMPILED_HEADER (CorePCH.h CorePCH.cpp)

END_PROJECT()
