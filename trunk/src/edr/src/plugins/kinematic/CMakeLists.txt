BEGIN_PROJECT("dynamic")


SET_UI_FILES (
	TrajectoriesDialog.ui
	LabeledDoubleSpinBox.ui
)

SET_PRIVATE_HEADERS(
	KinematicParser.h
	AsfParser.h
	KinematicVisualizer.h
	ISchemeDrawer.h
	LineSchemeDrawer.h
	OsgSchemeDrawer.h
	PointSchemeDrawer.h
	GlPointSchemeDrawer.h
	GlLineSchemeDrawer.h
	UniqueCollection.h
	SchemeDrawerContainer.h
	GRFSerie.h
	MarkerSerie.h
	KinematicSerie.h
	SkeletonSerie.h
	TrajectoriesDialog.h
	TrajectoriesDrawer.h
	SchemeDialog.h
	PickHandler.h
	Manipulators.h
	SkeletalVisualizationScheme.h
	VisualizationScheme.h
	MarkersVisualizationScheme.h
	PCH.h
	LabeledDoubleSpinBox.h
)

SET_PUBLIC_HEADERS (
	Wrappers.h
)

SET_SOURCE_FILES(
	KinematicPlugin.cpp
	AsfParser.cpp
	KinematicParser.cpp
	KinematicSerie.cpp
	KinematicVisualizer.cpp
	LineSchemeDrawer.cpp
	PointSchemeDrawer.cpp
	GlPointSchemeDrawer.cpp
	GlLineSchemeDrawer.cpp
	SchemeDrawerContainer.cpp
	GRFSerie.cpp
	MarkerSerie.cpp
	SkeletonSerie.cpp
	TrajectoriesDialog.cpp
	TrajectoriesDrawer.cpp
	SchemeDialog.cpp
	PickHandler.cpp
	Manipulators.cpp
	SkeletalVisualizationScheme.cpp
	VisualizationScheme.cpp
	MarkersVisualizationScheme.cpp
	PCH.cpp
	LabeledDoubleSpinBox.cpp
)

SET_MOC_FILES(
	KinematicVisualizer.h
	SkeletonSerie.h
	MarkerSerie.h
	TrajectoriesDialog.h
	SchemeDialog.h
	LabeledDoubleSpinBox.h
)


SET_RC_FILES (
	kinematicIcons.qrc
)

SET_PRECOMPILED_HEADER( PCH.h PCH.cpp)

GENERATE_PROJECT_TRANSLATIONS()

END_PROJECT()