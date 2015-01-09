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
#include <plugins/imuCostume/IIMUDataSource.h>

namespace Ui {
	class IMUCostumeConfigurationWizard;
}

class IMUCostumeConfigurationWizard : public QWizard
{
	Q_OBJECT;

private:

	class JointsItemDelegate;

public:

	//! Szczegó³y mapowania sensora
	struct SensorMappingDetails
	{
		//! Nazwa jointa z którym skojarzony jest sensor
		std::string jointName;
		//! Prototyp algorytmu estymacji orientacji czujnika
		IMU::IIMUOrientationEstimationAlgorithmConstPtr estimationAlgorithm;
		//! Opis sensora wzglêdem skojarzonego jointa - lokalne przesuniêcie i skrêcenie
		IMU::IMUCostumeCalibrationAlgorithm::SensorAdjustment sensorAdjustment;
	};

	//! Mapa identyfokatorw sensorów do szczegó³ów ich mapowania
	typedef std::map<imuCostume::Costume::SensorID, SensorMappingDetails> SensorsMappingDetails;	

public:

	IMUCostumeConfigurationWizard(const std::string & name,
		const std::string & ip,
		const unsigned int imusCount = 0,
		const unsigned int insolesCount = 0);

	virtual ~IMUCostumeConfigurationWizard();

	virtual bool eventFilter(QObject * watched, QEvent * event) override;	

	SensorsMappingDetails mappingDetails() const;

	IMU::IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithm() const;

	IMU::IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithm() const;

	kinematic::SkeletonConstPtr skeleton() const;

private slots:

	void onEstimationAlgorithmSelectionModeChange(int idx);
	void pageChanged(int idx);
	void verifyModel(int idx);
	void verifySensorsMapping(int row, int col);
	void verifyEstimationAlgorithms(int row, int col);
	void verifyCalibrationAlgorithm(int idx);
	void verifyMotionEstimationAlgorithm(int idx);
	void onOneToOneMappingChanged(int state);

private:

	utils::shared_ptr<imuCostume::CostumeRawIO> costume;

	Ui::IMUCostumeConfigurationWizard * ui;

	JointsItemDelegate * jointsItemDelegate;

	std::map<unsigned int, std::list<kinematic::SkeletonConstPtr>> skeletons;
};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__