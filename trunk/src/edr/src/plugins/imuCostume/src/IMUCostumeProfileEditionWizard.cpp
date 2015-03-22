#include "IMUCostumeProfileEditionWizard.h"
#include "ui_IMUCostumeProfileEditionWizard.h"
#include <QtGui/QMouseEvent>
#include <coreui/CoreVectorInputDelegate.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QStyledItemDelegate>
#include <QtWidgets/QDoubleSpinBox>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QHeaderView>
#include <coreui/CoreRotationWidgetEditor.h>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QMessageBox>
#include "OrientationDelegate.h"
#include <boost/lexical_cast.hpp>
#include <utils/TreeNode.h>
#include "OrientationDelegate.h"
#include "Vector3DDelegate.h"
#include "SensorIDDelegate.h"
#include "SkeletonJointsDelegate.h"
#include "OrientationEstimationAlgorithmDelegate.h"
#include <acclaimformatslib/AsfParser.h>

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeMotionEstimationAlgorithmConstPtr);

Q_DECLARE_METATYPE(kinematic::JointConstPtr);

Q_DECLARE_METATYPE(osg::Vec3d);

Q_DECLARE_METATYPE(IMU::SkeletonConstPtr);

static const int UseIDX = 0;
static const int SensorIDX = 0;
static const int JointIDX = 1;
static const int AlgorithmIDX = 2;
static const int RotationIDX = 3;
static const int OffsetIDX = 4;

using namespace IMU;

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	const QString & profileName,
	QWidget * parent) : QWizard(parent), ui(new Ui::IMUCostumeProfileEditionWizard),
	checkStates(costumeSensors.size(), (int)Qt::PartiallyChecked)
{
	ui->setupUi(this);
	/*init(profileName.isEmpty() == true ? tr("New profile") : profileName,
		costumeSensors, orientAlgorithms, calibAlgorithms, motionAlgorithms, skeletonModels);

	//inicjujemy tablice ustawień sensorów
	ui->sensorsConfigurationTableWidget->setRowCount(costumeSensors.size());	
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	unsigned int row = 0;

	//idziemy po wszystkich sensorach
	for (const auto & id : costumeSensors)
	{
		//SENSOR column
		{
			auto sensorItem = new QTableWidgetItem(QString("%1").arg(id));
			sensorItem->setFlags(sensorItem->flags() | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled);
			sensorItem->setCheckState(Qt::PartiallyChecked);
			ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, sensorItem);
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));
		}

		//JOINT column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(SkeletonJointsDelegate::defaultText()));
		}

		//ALGORITHM column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));			
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(OrientationEstimationAlgorithmDelegate::defaultValue()), Qt::UserRole);
		}

		//ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, RotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(osg::Quat(0,0,0,1)._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, RotationIDX), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		//OFFSET column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(osg::Vec3d(0,0,0), 3, coreUI::CoreVectorEditor::vectorPattern(3))));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(osg::Vec3d(0, 0, 0)), Qt::UserRole);
		}

		++row;
	}

	ui->sensorsConfigurationTableWidget->blockSignals(false);
	*/
}

IMUCostumeProfileEditionWizard::IMUCostumeProfileEditionWizard(const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	IMU::CostumeProfileConstPtr profile,
	QWidget * parent) : QWizard(parent), ui(new Ui::IMUCostumeProfileEditionWizard),
	checkStates(costumeSensors.size())
{
	ui->setupUi(this);
	/*init(profile->name.empty() == true ? tr("New profile") : QString::fromStdString(profile->name),
		costumeSensors,	orientAlgorithms, calibAlgorithms, motionAlgorithms, skeletonModels);

	imuCostume::Costume::SensorIDsSet allCostumeSensors(costumeSensors);

	for (const auto & sd : profile->sensorsDescriptions){
		allCostumeSensors.insert(sd.first);
	}

	//inicjujemy tablice ustawień sensorów
	ui->sensorsConfigurationTableWidget->setRowCount(allCostumeSensors.size());

	unsigned int row = 0;

	//idziemy po wszystkich sensorach
	for (const auto & id : allCostumeSensors)
	{
		auto pIT = profile->sensorsDescriptions.find(id);
		auto cIT = costumeSensors.find(id);

		bool use = false;
		bool sensorIDValid = false;

		QString jointName;
		osg::Vec3d offset(0, 0, 0);
		osg::Quat rotation(0, 0, 0, 1);
		IIMUOrientationEstimationAlgorithmConstPtr algorithm;

		if (pIT != profile->sensorsDescriptions.end()
			&& cIT != costumeSensors.end()){

			//mam wartość dla obu wpisów
			use = true;
			sensorIDValid = true;

			jointName = QString::fromStdString(pIT->second.jointName);
			offset = pIT->second.offset;
			rotation = pIT->second.rotation;
			algorithm = pIT->second.orientationEstimationAlgorithm;
		}
		else if (pIT != profile->sensorsDescriptions.end()){
			//tylko profil

			use = false;
			sensorIDValid = false;

			jointName = QString::fromStdString(pIT->second.jointName);
			offset = pIT->second.offset;
			rotation = pIT->second.rotation;
			algorithm = pIT->second.orientationEstimationAlgorithm;
		}
		else{
			//tylko kostium
			use = true;
			sensorIDValid = false;
		}

		//USE column
		{
			auto useItem = new QTableWidgetItem();
			useItem->setFlags(useItem->flags() | Qt::ItemFlag::ItemIsUserCheckable);

			if (use == true){
				useItem->setCheckState(Qt::Checked);
				useItem->setFlags(useItem->flags() | Qt::ItemFlag::ItemIsEnabled);
			}
			else{
				useItem->setCheckState(Qt::Unchecked);
				useItem->setFlags(useItem->flags() & ~Qt::ItemFlag::ItemIsEnabled);
			}

			ui->sensorsConfigurationTableWidget->setItem(row, UseIDX, useItem);
		}

		//SENSOR column
		{
			if (sensorIDValid == true){
				ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, new QTableWidgetItem(QString("%1").arg(id)));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));
			}
			else{
				ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, new QTableWidgetItem(SensorIDDelegate::defaultText()));
			}
		}

		//JOINT column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(jointName.isEmpty() == true ? SkeletonJointsDelegate::defaultText() : jointName));
		}

		//ALGORITHM column
		{
			if (algorithm == nullptr){
				ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));
			}
			else{
				ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(QString::fromStdString(algorithm->name())));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(algorithm));
			}
		}

		//ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, RotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(rotation._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, RotationIDX), QVariant::fromValue(rotation), Qt::UserRole);
		}

		//OFFSET column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(offset, 3, coreUI::CoreVectorEditor::vectorPattern(3))));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(offset), Qt::UserRole);
		}

		++row;
	}
	*/
}

void IMUCostumeProfileEditionWizard::init(const QString & profileName,
	const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels)
{
	ui->setupUi(this);
	setWindowTitle(windowTitle().arg(profileName));

	sensorDelegate = new SensorIDDelegate(ui->sensorsConfigurationTableWidget);
	sensorDelegate->setSensors(costumeSensors);

	skeletonJointDelegate = new SkeletonJointsDelegate(ui->sensorsConfigurationTableWidget);

	vector3DDelegate = new Vector3DDelegate(ui->sensorsConfigurationTableWidget);

	orientationDelegate = new OrientationDelegate(ui->sensorsConfigurationTableWidget);

	orientationAlgorithmDelegate = new OrientationEstimationAlgorithmDelegate(ui->sensorsConfigurationTableWidget);
	orientationAlgorithmDelegate->setOrientationEstimationAlgorithms(orientAlgorithms);

	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(SensorIDX, sensorDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(JointIDX, skeletonJointDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(AlgorithmIDX, orientationAlgorithmDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(RotationIDX, orientationDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(OffsetIDX, vector3DDelegate);

	//MODEL combobox
	{
		ui->modelComboBox->addItem(tr("Select model..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->modelComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->modelComboBox->palette().color(QPalette::Disabled, QPalette::Text));

		if (skeletonModels.empty() == false){
			ui->modelComboBox->insertSeparator(1);
		}

		for (auto m : skeletonModels)
		{
			unsigned int jointsCount = kinematic::SkeletonState::createMapping(*m.second).size();
			unsigned int activeJointsCount = kinematic::SkeletonState::createActiveMapping(*m.second).size();
			ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active)").arg(QString::fromStdString(m.second->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(m.second));
		}
	}

	//CALIBRATION combobox
	{
		ui->calibrationAlgorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->calibrationAlgorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->calibrationAlgorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));		

		if (calibAlgorithms.empty() == false){
			ui->calibrationAlgorithmComboBox->insertSeparator(1);
		}

		unsigned int counter = 1;

		for (auto a : calibAlgorithms)
		{
			ui->calibrationAlgorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
			ui->calibrationAlgorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
		}		
	}

	//MOTION ESTIMATION combobox
	{
		ui->motionEstimationAglorithmComboBox->addItem(tr("Select algorithm..."));

		const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->motionEstimationAglorithmComboBox->model());
		QStandardItem* item = model->item(0);

		item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
		// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
		item->setData(ui->motionEstimationAglorithmComboBox->palette().color(QPalette::Disabled, QPalette::Text));		

		if (motionAlgorithms.empty() == false){
			ui->motionEstimationAglorithmComboBox->insertSeparator(1);
		}

		unsigned int counter = 1;

		for (auto a : motionAlgorithms)
		{
			ui->motionEstimationAglorithmComboBox->addItem(QString::fromStdString(a.second->name()), QVariant::fromValue(a.second));
			ui->motionEstimationAglorithmComboBox->setItemData(counter++, QString("ID: %1").arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first))), Qt::ToolTipRole);
		}		
	}
}

IMUCostumeProfileEditionWizard::~IMUCostumeProfileEditionWizard()
{
	delete ui;
}

void IMUCostumeProfileEditionWizard::pageChanged(int idx)
{	
	auto w = page(idx);

	if (w == ui->modelConfigurationPage)
	{
		verifyModel(-1);
	}
	/*
	else if (w == ui->sensorsMappingPage)
	{
		verifySensorsMapping(-1, -1);
	}
	else if (w == ui->sensorsEstimationPage)
	{
		verifyEstimationAlgorithms(-1, -1);
	}
	else if (w == ui->calibrationPage)
	{
		verifyCalibrationAlgorithm(-1);
	}
	else if (w == ui->motionEstimationPage)
	{
		verifyMotionEstimationAlgorithm(-1);
	}*/	
}

void IMUCostumeProfileEditionWizard::verifyModel(int idx)
{
	bool enable = (ui->modelComboBox->currentIndex() > 0);

	if (enable == true){
		auto skeleton = ui->modelComboBox->currentData(Qt::UserRole).value<IMU::SkeletonConstPtr>();
		skeletonJointDelegate->setJoints(skeleton->root);

		std::set<std::string> jointNames;

		auto nameVisitor = [&jointNames](kinematic::JointConstPtr joint)
		{
			if (joint != nullptr){
				jointNames.insert(joint->value.name);
			}
		};

		utils::TreeNode::visitPostOrder(skeleton->root, nameVisitor);

		//przejść przez kolumne z jointami w mapowaniu i wyzerować te dla których nie ma odpowiednika z aktualnym szkielecie
		auto model = ui->sensorsConfigurationTableWidget->model();
		for (unsigned int i = 0; i < model->rowCount(); ++i)
		{
			auto index = model->index(i, JointIDX);
			auto jointName = model->data(index).toString();
			if (jointNames.find(jointName.toStdString()) == jointNames.end()){
				model->setData(index, SkeletonJointsDelegate::defaultText());
			}
		}
	}

	//button(QWizard::NextButton)->setEnabled(enable);
}

bool IMUCostumeProfileEditionWizard::verifyCalibrationAndMotionAlgorithms() const
{
	return (ui->calibrationAlgorithmComboBox->currentIndex() > 0) && (ui->motionEstimationAglorithmComboBox->currentIndex() > 0);
}

void IMUCostumeProfileEditionWizard::verifyCalibrationAlgorithm(int idx)
{
	bool enable = verifyCalibrationAndMotionAlgorithms();

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::verifyMotionEstimationAlgorithm(int idx)
{
	bool enable = verifyCalibrationAndMotionAlgorithms();

	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::onOneToOneMappingChanged(int state)
{
	if (state == Qt::Checked){
		auto model = ui->sensorsConfigurationTableWidget->model();

		//szukamy duplikatów
		bool found = false;
		{
			
			std::set<std::string> jointsNames;
			for (unsigned int i = 0; i < model->rowCount(); ++i){
				auto jointName = model->data(model->index(i, JointIDX)).toString().toStdString();
				if (jointsNames.find(jointName) != jointsNames.end()){
					found = true;
					break;
				}
			}
		}

		if (found == true){
			auto ret = QMessageBox::question(this, tr("Duplicate joints found"), tr("You wish to use one-to-one sensor to joint mapping but some joints have more than one sensor attached. Would You like to reset duplicates?"));
			if (ret == QMessageBox::No){
				ui->oneToOneMappingCheckBox->blockSignals(true);
				ui->oneToOneMappingCheckBox->setChecked(Qt::Unchecked);
				ui->oneToOneMappingCheckBox->blockSignals(false);
				return;
			}
		}

		std::set<std::string> jointsNames;
		for (unsigned int i = 0; i < model->rowCount(); ++i){
			auto jointName = model->data(model->index(i, JointIDX)).toString().toStdString();
			if (jointsNames.find(jointName) != jointsNames.end()){
				//ju� mamy taki joint
				model->setData(model->index(i, JointIDX), SkeletonJointsDelegate::defaultText());
			}
			else{
				//jointa nie by�o - zapami�tujemy
				jointsNames.insert(jointName);
			}
		}
	}	
}

void IMUCostumeProfileEditionWizard::onSensorConfigurationCellChange(int row, int column)
{
	int checked = 0;
	int partial = 0;

	auto checkStateChanged = false;

	if (column == UseIDX){
		checkStateChanged = checkStates[row] != ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState();
	}

	if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() != Qt::Unchecked){

		if (checkStateChanged == true && checkStates[row] == Qt::PartiallyChecked){
			ui->sensorsConfigurationTableWidget->blockSignals(true);
			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(Qt::Unchecked);				
			ui->sensorsConfigurationTableWidget->blockSignals(false);			
		}
		else{
			++checked;

			auto sensorIDData = ui->sensorsConfigurationTableWidget->item(row, SensorIDX)->data(Qt::DisplayRole);
			auto jointData = ui->sensorsConfigurationTableWidget->item(row, JointIDX)->data(Qt::DisplayRole);
			auto algoData = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);

			if (sensorIDData.isValid() && jointData.isValid() && algoData.isValid()){

				auto sid = sensorIDData.toInt();
				auto jointName = jointData.toString();
				auto algo = algoData.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

				if (sid > -1 && jointName != SkeletonJointsDelegate::defaultText()
					&& algo != nullptr){

				}
				else{
					++partial;
				}

			}
			else{
				++partial;
			}

			ui->sensorsConfigurationTableWidget->blockSignals(true);
			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(partial == 0 ? Qt::Checked : Qt::PartiallyChecked);
			ui->sensorsConfigurationTableWidget->blockSignals(false);
		}
	}

	checkStates[row] = ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState();

	//TODO
	//verify other rows - target to ok: at least one checked and no partial checks
	//button(QWizard::NextButton)->setEnabled(enable);
}

void IMUCostumeProfileEditionWizard::refreshProfile()
{
	/*
	SensorsMappingDetails ret;

	auto modelMapping = ui->sensorsMappingTableWidget->model();
	auto modelEstimating = ui->sensorsEstimationAlgorithmsTableWidget->model();

	UTILS_ASSERT(modelMapping->rowCount() == modelEstimating->rowCount());

	for (unsigned int i = 0; i < modelMapping->rowCount(); ++i)
	{
		imuCostume::Costume::SensorID sID = modelMapping->data(modelMapping->index(i, 0)).toInt();
		SensorMappingDetails smd;
		smd.jointName = modelMapping->data(modelMapping->index(i, 1)).toString().toStdString();
		smd.sensorAdjustment.offset = modelMapping->data(modelMapping->index(i, 2), Qt::UserRole).value<osg::Vec3d>();
		smd.sensorAdjustment.rotation = modelMapping->data(modelMapping->index(i, 3), Qt::UserRole).value<osg::Quat>();
		smd.estimationAlgorithm = modelEstimating->data(modelEstimating->index(i, 1), Qt::UserRole).value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

		ret.insert(SensorsMappingDetails::value_type(sID, smd));
	}

	ui->calibrationAlgorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>();
	ui->motionEstimationAglorithmComboBox->currentData(Qt::UserRole).value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>();
	ui->modelComboBox->currentData(Qt::UserRole).value<kinematic::SkeletonConstPtr>();
	*/
	setWindowModified(true);
}

CostumeProfile IMUCostumeProfileEditionWizard::costumeProfile() const
{
	CostumeProfile ret;
	ret.name = "New profile";

	auto calibData = ui->calibrationAlgorithmComboBox->currentData();
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

	auto scm = ui->sensorsConfigurationTableWidget->model();
	for (unsigned int i = 0; i < scm->rowCount(); ++i)
	{
		if (ui->sensorsConfigurationTableWidget->item(i, UseIDX)->checkState() == Qt::Checked){

			auto sensorID = scm->data(scm->index(i, SensorIDX)).toUInt();

			CostumeProfile::SensorDescription sd;

			sd.jointName = scm->data(scm->index(i, JointIDX)).toString().toStdString();
			sd.offset = scm->data(scm->index(i, OffsetIDX), Qt::UserRole).value<osg::Vec3d>();
			sd.rotation = scm->data(scm->index(i, RotationIDX), Qt::UserRole).value<osg::Quat>();

			auto algo = scm->data(scm->index(i, AlgorithmIDX), Qt::UserRole).value<IIMUOrientationEstimationAlgorithmConstPtr>();

			sd.orientationEstimationAlgorithm.reset(algo->create());		

			ret.sensorsDescriptions.insert(CostumeProfile::SensorsDescriptions::value_type(sensorID, sd));			
		}
	}	

	return ret;
}

void IMUCostumeProfileEditionWizard::onLoadModel()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open model file"), QString(), tr("Acclaim skeleton file file (*.asf)"));

	core::Filesystem::Path p(file.toStdString());
	if (core::Filesystem::isRegularFile(p)) {
		try{
			acclaim::AsfParser asfParser;
			auto as = asfParser.parse(p.string());
			kinematic::Skeleton skeleton;
			kinematic::Skeleton::convert(as, skeleton);

			boost::uuids::basic_random_generator<boost::mt19937> gen;
			boost::uuids::uuid u = gen();

			auto registeredSkeleton = utils::make_shared<const IMU::Skeleton>(u, skeleton);

			unsigned int jointsCount = kinematic::SkeletonState::createMapping(*registeredSkeleton).size();
			unsigned int activeJointsCount = kinematic::SkeletonState::createActiveMapping(*registeredSkeleton).size();
			ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active").arg(QString::fromStdString(registeredSkeleton->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(registeredSkeleton));
			ui->modelComboBox->setCurrentIndex(ui->modelComboBox->count() - 1);
		}
		catch (std::exception & e){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with following error: ").arg(file) + QString::fromStdString(e.what()));
		}
		catch (...){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with unknown error"));
		}
	}
}
