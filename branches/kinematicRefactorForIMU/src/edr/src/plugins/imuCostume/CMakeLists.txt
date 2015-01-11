BEGIN_PROJECT("dynamic")

SET_PUBLIC_HEADERS (
	Export.h
	Streams.h
	CostumeIMUExtractor.h	
	IIMUDataSource.h
	IIMUOrientationEstimationAlgorithm.h
	IMUCostumeCalibrationAlgorithm.h
	IMUCostumeMotionEstimationAlgorithm.h
	Types.h
)

SET_PRIVATE_HEADERS (
	CostumeSkeletonMotionHelper.h
	OrientationDelegate.h
	IMUCostumeListWidget.h
	DataSource.h
	IMUDatParser.h
	IMUParser.h
	IMUCFGParser.h
	IMUPerspective.h
	PCH.h
	IMUCostumeCalibrationWizard.h
	IMUCostumeProfileEditionWizard.h
	IMUCostumeProfileConfigurationWizard.h
)

SET_SOURCE_FILES(
	CostumeIMUExtractor.cpp
	CostumeSkeletonMotionHelper.cpp
	OrientationDelegate.cpp
	IMUCostumeListWidget.cpp
	DataSource.cpp
	IMUDatParser.cpp
	IMUParser.cpp
	IMUPerspective.cpp
	IMUCFGParser.cpp
	PCH.cpp
	Plugin.cpp
	IMUCostumeCalibrationWizard.cpp
	IMUCostumeProfileEditionWizard.cpp
	IMUCostumeProfileConfigurationWizard.cpp
)

SET_MOC_FILES(
	CostumeSkeletonMotionHelper.h
	OrientationDelegate.h
	IMUCostumeListWidget.h
	IMUCostumeCalibrationWizard.h
	IMUCostumeProfileEditionWizard.h
)

SET_UI_FILES(
	IMUCostumeListWidget.ui
	IMUCostumeCalibrationWizard.ui
	IMUCostumeProfileEditionWizard.ui
)

SET_RC_FILES(
	imuCostumeIcons.qrc
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( PCH.h PCH.cpp )

END_PROJECT()