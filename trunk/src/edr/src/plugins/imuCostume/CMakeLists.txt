BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Wrappers.h
	IIMUDataSource.h
	IIMUOrientationEstimationAlgorithm.h
	IMUCostumeCalibrationAlgorithm.h
	IMUCostumeMotionEstimationAlgorithm.h
	Types.h
)

SET_PRIVATE_HEADERS (
	IMUCostumeListWidget.h
	DataSource.h
	IMUDatParser.h
	IMUParser.h
	IMUCFGParser.h
	IMUPerspective.h
	PCH.h
	IMUCostumeCalibrationWizard.h
	IMUCostumeConfigurationWizard.h
)

SET_SOURCE_FILES(
	IMUCostumeListWidget.cpp
	DataSource.cpp
	IMUDatParser.cpp
	IMUParser.cpp
	IMUPerspective.cpp
	IMUCFGParser.cpp
	PCH.cpp
	Plugin.cpp
	IMUCostumeCalibrationWizard.cpp
	IMUCostumeConfigurationWizard.cpp
)

SET_MOC_FILES(
	IMUCostumeListWidget.h
	IMUCostumeCalibrationWizard.h
	IMUCostumeConfigurationWizard.h
)

SET_UI_FILES(
	IMUCostumeListWidget.ui
	IMUCostumeCalibrationWizard.ui
	IMUCostumeConfigurationWizard.ui
)

SET_RC_FILES(
	imuCostumeIcons.qrc
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( PCH.h PCH.cpp )

END_PROJECT()