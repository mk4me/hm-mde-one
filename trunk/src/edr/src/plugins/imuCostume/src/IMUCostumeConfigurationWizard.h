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

namespace Ui {
	class IMUCostumeConfigurationWizard;
}

class IMUCostumeConfigurationWizard : public QWizard
{
	Q_OBJECT;

private:

	class JointsItemDelegate;

public:

	IMUCostumeConfigurationWizard(utils::shared_ptr<imuCostume::CostumeRawIO> costume,
		const unsigned int imusCount = 0,
		const unsigned int insolesCount = 0);

	virtual ~IMUCostumeConfigurationWizard();

	virtual bool eventFilter(QObject * watched, QEvent * event) override;

	void * configuredCostumeStream();

private slots:

	void onEstimationAlgorithmSelectionModeChange(int state);

private:

	utils::shared_ptr<imuCostume::CostumeRawIO> costume;

	Ui::IMUCostumeConfigurationWizard * ui;

	JointsItemDelegate * jointsItemDelegate;
};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECONFIGURATIONWIZARD_H__