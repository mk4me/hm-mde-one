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
	class IMUCostumeProfileEditionWizard;
}

class IMUCostumeProfileEditionWizard : public QWizard
{
	Q_OBJECT;

private:

	class JointsItemDelegate;

public:

	IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & sensorsIDs,
		const QString & profileName = tr("Unnamed profile"), QWidget * parent = nullptr);

	IMUCostumeProfileEditionWizard(const IMU::CostumeProfile & costumeProfile,
		QWidget * parent = nullptr);

	virtual ~IMUCostumeProfileEditionWizard();

	virtual bool eventFilter(QObject * watched, QEvent * event) override;

	IMU::CostumeProfile costumeProfile() const;

private:

	void refreshProfile();

private slots:

	void onEstimationAlgorithmSelectionModeChange(int idx);
	void pageChanged(int idx);
	void verifyModel(int idx);
	void verifySensorsMapping(int row, int col);
	void verifyEstimationAlgorithms(int row, int col);
	void verifyCalibrationAlgorithm(int idx);
	void verifyMotionEstimationAlgorithm(int idx);
	void onOneToOneMappingChanged(int state);
	void onSingleEstimationAlgorithmChange(int idx);

private:	

	Ui::IMUCostumeProfileEditionWizard * ui;

	JointsItemDelegate * jointsItemDelegate;

	std::map<unsigned int, std::list<kinematic::SkeletonConstPtr>> skeletons;

	IMU::CostumeProfile baseCostumeProfile_;	
};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__