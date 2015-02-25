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
	CostumeProfile.h
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
	IMUCostumeProfileEditionWizard.h
	IMUCostumeProfileConfigurationWizard.h
	RecordingWizard.h
	CostumeParser.h
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
	IMUCostumeProfileEditionWizard.cpp
	IMUCostumeProfileConfigurationWizard.cpp
	RecordingWizard.cpp
	CostumeParser.cpp
	CostumeProfile.cpp
)

SET_MOC_FILES(
	CostumeSkeletonMotionHelper.h
	OrientationDelegate.h
	IMUCostumeListWidget.h	
	IMUCostumeProfileEditionWizard.h
	RecordingWizard.h
)

SET_UI_FILES(
	IMUCostumeListWidget.ui	
	IMUCostumeProfileEditionWizard.ui
	RecordingWizard.ui
)

SET_RC_FILES(
	imuCostumeIcons.qrc
)

#------------------------------------------------------------------------------
# prekompilowany nag³ówek
SET_PRECOMPILED_HEADER( PCH.h PCH.cpp )

END_PROJECT()