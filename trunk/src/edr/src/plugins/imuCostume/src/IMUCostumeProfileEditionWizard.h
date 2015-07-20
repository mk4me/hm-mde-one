/********************************************************************
	created:  2014/12/21	19:18:18
	filename: IMUCostumeConfigurationWizard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__

#include <imucostumelib/CostumeRawIO.h>
#include <QtWidgets/QWizard>
#include <osg/Vec3d>
#include <osg/Quat>
#include <map>
#include <vector>
#include <plugins/imuCostume/IIMUDataSource.h>

namespace Ui {
	class IMUCostumeProfileEditionWizard;
}

class SensorIDDelegate;
class SkeletonJointsDelegate;
class Vector3DDelegate;
class OrientationDelegate;
class OrientationEstimationAlgorithmDelegate;

class IMUCostumeProfileEditionWizard : public QWizard
{
	Q_OBJECT;

public:

	IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		const QString & profileName = tr("New Profile"),
		QWidget * parent = nullptr);

	IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		IMU::CostumeProfileConstPtr profile,
		QWidget * parent = nullptr);

	virtual ~IMUCostumeProfileEditionWizard();	

	IMU::CostumeProfile costumeProfile() const;

private:

	void init(const QString & profileName,
		const imuCostume::Costume::SensorIDsSet & costumeSensors,
		const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
		const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
		const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
		const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
		const IMU::CostumeProfile::SensorsDescriptions & sensorsDescriptions = IMU::CostumeProfile::SensorsDescriptions());

	void refreshProfile();
	bool verifyCalibrationAndMotionAlgorithms() const;

private slots:
	
	void pageChanged(int idx);
	void verifyModel(int idx);		
	void verifyCalibrationAlgorithm(int idx);
	void verifyMotionEstimationAlgorithm(int idx);
	void onOneToOneMappingChanged(int state);
	void onSensorConfigurationCellChange(int row, int column);
	void onLoadModel();
	void onSensorsConfigurationTableContextMenu(const QPoint & position);

	void onSelectAllSensors();
	void onDeselectAllSensors();
	void onInversSensorsSelection();
	void onSelectCommonEstimationAlgorithm();

private:	
	
	std::vector<int> checkStates;
	SensorIDDelegate * sensorDelegate;
	SkeletonJointsDelegate * skeletonJointDelegate;
	Vector3DDelegate * vector3DDelegate;
	OrientationDelegate * orientationDelegate;
	OrientationEstimationAlgorithmDelegate * orientationAlgorithmDelegate;

	Ui::IMUCostumeProfileEditionWizard * ui;
};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__