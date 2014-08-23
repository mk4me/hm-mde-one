/********************************************************************
	created:  2014/08/23	16:20:32
	filename: IMUCostumeCalibrationWizard.h
	author:	  Mateusz Janiak

	purpose:
*********************************************************************/
#ifndef __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONWIZARD_H__
#define __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONWIZARD_H__

#include <QtWidgets/QWizard>

namespace Ui
{
	class IMUCostumeCalibrationWizard;
}

class IMUCostumeCalibrationWizard : public QWizard
{
	Q_OBJECT;

public:

	IMUCostumeCalibrationWizard(QWidget * parent = nullptr, Qt::WindowFlags f = 0);
	virtual ~IMUCostumeCalibrationWizard();

private:

	Ui::IMUCostumeCalibrationWizard * ui;

};

#endif	// __HEADER_GUARD_IMU__IMUCOSTUMECALIBRATIONWIZARD_H__