#include "PCH.h"
#include "IMUCostumeCalibrationWizard.h"
#include "ui_IMUCostumeCalibrationWizard.h"

IMUCostumeCalibrationWizard::IMUCostumeCalibrationWizard(QWidget * parent, Qt::WindowFlags f)
	: QWizard(parent, f), ui(new Ui::IMUCostumeCalibrationWizard)
{
	ui->setupUi(this);
}

IMUCostumeCalibrationWizard::~IMUCostumeCalibrationWizard()
{

}