#include "IMUCostumeProfileEditionWizard.h"
#include <QtWidgets/QWizardPage>
#include "ui_ModelConfigurationWizardPage.h"
#include "ui_ModelPhysicalDimensionsWizardPage.h"
#include "ui_SensorsConfigurationWizardPage.h"
#include "ui_MotionEstimationConfigurationWizardPage.h"
#include <QtGui/QMouseEvent>
#include <coreui/CoreVectorInputDelegate.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMenu>
#include <coreui/CoreRotationWidgetEditor.h>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QInputDialog>
#include <QtWidgets/QMessageBox>
#include "OrientationDelegate.h"
#include <boost/lexical_cast.hpp>
#include "OrientationDelegate.h"
#include "Vector3DDelegate.h"
#include "SensorIDDelegate.h"
#include "SkeletonJointsDelegate.h"
#include "OrientationEstimationAlgorithmDelegate.h"
#include <fstream>
#include "ModelConfigurationWizardPage.h"
#include "ModelPhysicalDimensionsWizardPage.h"
#include "SensorsConfigurationWizardPage.h"
#include "MotionEstimationConfigurationWizardPage.h"

using namespace IMU;

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	const QString & profileName,
	QWidget * parent) : QWizard(parent)
{
	setWindowTitle(tr("New profile"));
	auto modelPage = new ModelConfigurationWizardPage(skeletonModels);
	addPage(modelPage);
	addPage(new ModelPhysicalDimensionsWizardPage());
	addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, modelPage));
	addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms));
}

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	IMU::CostumeProfileConstPtr profile,
	QWidget * parent) : QWizard(parent)
{
	if (profile == nullptr){
		setWindowTitle(tr("New profile"));
		auto modelPage = new ModelConfigurationWizardPage(skeletonModels);
		addPage(modelPage);
		addPage(new ModelPhysicalDimensionsWizardPage());
		addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, modelPage));
		addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms));
	}
	else{
		setWindowTitle(QString::fromStdString(profile->name));
		ModelConfigurationWizardPage * modelPage = nullptr;
		if (profile->skeleton != nullptr){
			modelPage = new ModelConfigurationWizardPage(skeletonModels, profile->skeleton->ID());
			addPage(modelPage);
		}
		else{
			modelPage = new ModelConfigurationWizardPage(skeletonModels);
			addPage(modelPage);
		}

		addPage(new ModelPhysicalDimensionsWizardPage());

		addPage(new SensorsConfigurationWizardPage(costumeSensors, orientAlgorithms, profile->sensorsDescriptions, modelPage));

		core::UniqueID cID;
		core::UniqueID mID;
		if (profile->calibrationAlgorithm != nullptr){
			cID = profile->calibrationAlgorithm->ID();
		}

		if (profile->motionEstimationAlgorithm != nullptr){
			mID = profile->motionEstimationAlgorithm->ID();
		}

		addPage(new MotionEstimationConfigurationWizardPage(calibAlgorithms, motionAlgorithms, cID, mID));
	}
}

IMUCostumeProfileEditionWizard::~IMUCostumeProfileEditionWizard()
{
	
}

CostumeProfile IMUCostumeProfileEditionWizard::costumeProfile() const
{
	CostumeProfile ret;

	ret.name = "New profile";

	/*auto calibData = ui->calibrationAlgorithmComboBox->currentData();
	auto motionData = ui->motionEstimationAglorithmComboBox->currentData();
	auto skeletonData = ui->modelComboBox->currentData();

	if (calibData.isValid() == true && calibData.canConvert<IMUCostumeCalibrationAlgorithmConstPtr>() == true){
		auto calibAlgorithm = calibData.value<IMUCostumeCalibrationAlgorithmConstPtr>();
		if (calibAlgorithm != nullptr){
			ret.calibrationAlgorithm.reset(calibAlgorithm->create());
		}
	}

	if (motionData.isValid() == true && motionData.canConvert<IMUCostumeMotionEstimationAlgorithmConstPtr>() == true){
		auto motionAlgorithm = motionData.value<IMUCostumeMotionEstimationAlgorithmConstPtr>();
		if (motionAlgorithm != nullptr){
			ret.motionEstimationAlgorithm.reset(motionAlgorithm->create());
		}
	}

	if (skeletonData.isValid() == true && skeletonData.canConvert<SkeletonConstPtr>() == true){
		auto skeleton = skeletonData.value<SkeletonConstPtr>();
		if (skeleton != nullptr){
			ret.skeleton.reset(new Skeleton(*skeleton));
		}
	}

	IMU::IMUCostumeCalibrationAlgorithm::SensorsDescriptions sds;

	auto scm = ui->sensorsConfigurationTableWidget->model();
	for (unsigned int i = 0; i < scm->rowCount(); ++i)
	{
		if (ui->sensorsConfigurationTableWidget->item(i, UseIDX)->checkState() == Qt::Checked){

			auto sensorID = scm->data(scm->index(i, SensorIDX)).toUInt();

			CostumeProfile::SensorDescription sd;
			sd.jointIdx = scm->data(scm->index(i, JointIDX), Qt::UserRole).toInt();
			sd.jointName = scm->data(scm->index(i, JointIDX)).toString().toStdString();
			sd.offset = scm->data(scm->index(i, OffsetIDX), Qt::UserRole).value<osg::Vec3d>();
			sd.preMulRotation = scm->data(scm->index(i, PreMulRotationIDX), Qt::UserRole).value<osg::Quat>();
			sd.postMulRotation = scm->data(scm->index(i, PostMulRotationIDX), Qt::UserRole).value<osg::Quat>();

			auto algo = scm->data(scm->index(i, AlgorithmIDX), Qt::UserRole).value<IIMUOrientationEstimationAlgorithmConstPtr>();

			sd.orientationEstimationAlgorithm.reset(algo->create());		

			ret.sensorsDescriptions.insert({ sensorID, sd });
			sds.insert({ sensorID, sd });
		}
	}*/

	ret.skeleton.reset(new IMU::Skeleton(*dynamic_cast<ModelConfigurationWizardPage*>(page(0))->skeleton()));
	ret.sensorsDescriptions = dynamic_cast<SensorsConfigurationWizardPage*>(page(2))->sensorsDescriptions();
	ret.calibrationAlgorithm.reset(dynamic_cast<MotionEstimationConfigurationWizardPage*>(page(3))->calibrationAlgorithm()->create());
	ret.motionEstimationAlgorithm.reset(dynamic_cast<MotionEstimationConfigurationWizardPage*>(page(3))->motionEstimationAlgorithm()->create());

	//ret.activeJoints = ret.motionEstimationAlgorithm->activeJoints(ret.skeleton, ret.sensorsDescriptions);

	return ret;
}


