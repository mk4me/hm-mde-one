BEGIN_PROJECT("static")

# SET_PUBLIC_HEADERS(
	# MultiView.h
	# MultiViewWidgetItem.h
	# MultiViewFunctorWidgetAdapter.h
	# MultiViewCameraItem.h
	# MultiViewManipulatorItem.h
	# OsgSceneDump.h
# )

SET_CONFIGURATION_INPUT_FILES(
	"Config.h.in"
)
configure_file( "configuration/Config.h.in" "${PROJECT_BINARY_DIR}/core/Config.h" )

SET_SOURCE_FILES(
	Window.cpp
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
