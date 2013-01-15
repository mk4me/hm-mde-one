BEGIN_PROJECT("static")

# SET_PUBLIC_HEADERS(
	# MultiView.h
	# MultiViewCameraItem.h
	# MultiViewManipulatorItem.h
	# OsgSceneDump.h
# )	# MultiViewWidgetItem.h
	# MultiViewFunctorWidgetAdapter.h


SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)

CONFIGURE_PUBLIC_HEADER("Config.h.in" "Config.h")


SET_SOURCE_FILES(
	MultiView.cpp
	MultiViewWidgetItem.cpp
	Filesystem.cpp
	MultiViewFunctorWidgetAdapter.cpp
	MultiViewCameraItem.cpp
	MultiViewManipulatorItem.cpp
	OsgSceneDump.cpp
	CorePCH.cpp
)

# SET_MOC_FILES(
	# MultiViewWidgetItem.h
# )

SET_PRIVATE_HEADERS(
	CorePCH.h
)

END_PROJECT()
