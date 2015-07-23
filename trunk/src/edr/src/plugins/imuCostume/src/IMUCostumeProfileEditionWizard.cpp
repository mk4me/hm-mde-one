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
#include <acclaimformatslib/AsfParser.h>
#include <fstream>

Q_DECLARE_METATYPE(IMU::IIMUOrientationEstimationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeCalibrationAlgorithmConstPtr);
Q_DECLARE_METATYPE(IMU::IMUCostumeMotionEstimationAlgorithmConstPtr);

Q_DECLARE_METATYPE(kinematic::Skeleton::JointConstPtr);

Q_DECLARE_METATYPE(osg::Vec3d);

Q_DECLARE_METATYPE(IMU::SkeletonConstPtr);

static const int UseIDX = 0;
static const int SensorIDX = 0;
static const int JointIDX = 1;
static const int AlgorithmIDX = 2;
static const int PreMulRotationIDX = 3;
static const int PostMulRotationIDX = 4;
static const int OffsetIDX = 5;

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
	init(profileName.isEmpty() == true ? tr("New profile") : profileName,
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
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, JointIDX), SkeletonJointsDelegate::defaultValue(), Qt::UserRole);
		}

		//ALGORITHM column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));			
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(OrientationEstimationAlgorithmDelegate::defaultValue()), Qt::UserRole);
		}

		//PRE MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PreMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(osg::Quat(0,0,0,1)._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PreMulRotationIDX), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		//POST MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PostMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(osg::Quat(0, 0, 0, 1)._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PostMulRotationIDX), QVariant::fromValue(osg::Quat(0, 0, 0, 1)), Qt::UserRole);
		}

		//OFFSET column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(osg::Vec3d(0,0,0), 3, coreUI::CoreVectorEditor::vectorPattern(3))));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(osg::Vec3d(0, 0, 0)), Qt::UserRole);
		}

		++row;
	}

	ui->sensorsConfigurationTableWidget->blockSignals(false);
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
	//ui->setupUi(this);
	init(profile->name.empty() == true ? tr("New profile") : QString::fromStdString(profile->name),
		costumeSensors,	orientAlgorithms, calibAlgorithms, motionAlgorithms, skeletonModels,
		profile->sensorsDescriptions);

	kinematic::LinearizedSkeleton::LocalMapping localMapping;

	if (profile->skeleton != nullptr){

		for (int i = 2; i < ui->modelComboBox->count(); ++i)
		{
			if (ui->modelComboBox->itemData(i).value<IMU::SkeletonConstPtr>()->ID() == profile->skeleton->ID()){
				ui->modelComboBox->setCurrentIndex(i);
				break;
			}
		}

		localMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*(profile->skeleton));

	}

	imuCostume::Costume::SensorIDsSet allCostumeSensors(costumeSensors);

	for (const auto & sd : profile->sensorsDescriptions){
		allCostumeSensors.insert(sd.first);
	}

	//inicjujemy tablice ustawień sensorów
	ui->sensorsConfigurationTableWidget->setRowCount(allCostumeSensors.size());


	ui->sensorsConfigurationTableWidget->blockSignals(true);
	unsigned int row = 0;

	//idziemy po wszystkich sensorach
	for (const auto & id : allCostumeSensors)
	{
		auto pIT = profile->sensorsDescriptions.find(id);
		auto cIT = costumeSensors.find(id);
		auto sID = SensorIDDelegate::defaultValue();
		auto sensorText = SensorIDDelegate::defaultText();

		bool use = false;
		//bool sensorIDValid = false;

		QString jointName;
		osg::Vec3d offset(0, 0, 0);
		osg::Quat preMulRotation(0, 0, 0, 1);
		osg::Quat postMulRotation(0, 0, 0, 1);
		IIMUOrientationEstimationAlgorithmConstPtr algorithm;

		if (pIT != profile->sensorsDescriptions.end()
			&& cIT != costumeSensors.end()){

			//mam wartość dla obu wpisów
			use = true;
			//sensorIDValid = true;
			sID = id;
			sensorText = QString("%1").arg(id);
			jointName = QString::fromStdString(pIT->second.jointName);
			offset = pIT->second.offset;
			preMulRotation = pIT->second.preMulRotation;
			postMulRotation = pIT->second.postMulRotation;
			algorithm = pIT->second.orientationEstimationAlgorithm;
		}
		else if (pIT != profile->sensorsDescriptions.end()){
			//tylko profil

			use = false;
			//sensorIDValid = false;

			jointName = QString::fromStdString(pIT->second.jointName);
			offset = pIT->second.offset;
			preMulRotation = pIT->second.preMulRotation;
			postMulRotation = pIT->second.postMulRotation;
			algorithm = pIT->second.orientationEstimationAlgorithm;
		}
		else{
			//tylko kostium
			use = true;
			//sensorIDValid = false;
		}		

		//JOINT column
		{
			auto it = localMapping.data().right.find(jointName.toStdString());
			if (profile->skeleton == nullptr){
				ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(jointName.isEmpty() == true ? SkeletonJointsDelegate::defaultText() : jointName));
			}
			else{				
				ui->sensorsConfigurationTableWidget->setItem(row, JointIDX, new QTableWidgetItem(it == localMapping.data().right.end() ? SkeletonJointsDelegate::defaultText() : jointName));
			}		
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, JointIDX), it != localMapping.data().right.end() ? it->get_left() : SkeletonJointsDelegate::defaultValue(), Qt::UserRole);
		}		

		//ALGORITHM column
		{
			if (algorithm == nullptr){
				ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(OrientationEstimationAlgorithmDelegate::defaultText()));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(OrientationEstimationAlgorithmDelegate::defaultValue()), Qt::UserRole);
			}
			else{
				ui->sensorsConfigurationTableWidget->setItem(row, AlgorithmIDX, new QTableWidgetItem(QString::fromStdString(algorithm->name())));
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, AlgorithmIDX), QVariant::fromValue(algorithm), Qt::UserRole);
			}
		}

		//PRE MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PreMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(preMulRotation._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PreMulRotationIDX), QVariant::fromValue(preMulRotation), Qt::UserRole);
		}

		//POST MUL ROTATION column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, PostMulRotationIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::quaternionToString(postMulRotation._v)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, PostMulRotationIDX), QVariant::fromValue(postMulRotation), Qt::UserRole);
		}

		//OFFSET column
		{
			ui->sensorsConfigurationTableWidget->setItem(row, OffsetIDX, new QTableWidgetItem(coreUI::CoreVectorEditor::vectorToString(offset, 3, coreUI::CoreVectorEditor::vectorPattern(3))));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, OffsetIDX), QVariant::fromValue(offset), Qt::UserRole);
		}

		//SENSOR column
		{
			auto sensorItem = new QTableWidgetItem(sensorText);
			sensorItem->setFlags(sensorItem->flags() | Qt::ItemFlag::ItemIsUserCheckable | Qt::ItemFlag::ItemIsEnabled);
			sensorItem->setCheckState(use == true ? Qt::Checked : Qt::Unchecked);
			ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, sensorItem);
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));

			/*
			if (sensorIDValid == true){
			ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, new QTableWidgetItem(QString("%1").arg(id)));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(id));
			}
			else{
			ui->sensorsConfigurationTableWidget->setItem(row, SensorIDX, new QTableWidgetItem(SensorIDDelegate::defaultText()));
			ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(row, SensorIDX), QVariant(SensorIDDelegate::defaultValue()));
			}
			*/
		}

		////USE column
		//{
		//	auto useItem = new QTableWidgetItem();
		//	useItem->setFlags(useItem->flags() | Qt::ItemFlag::ItemIsUserCheckable);

		//	if (use == true){
		//		useItem->setCheckState(Qt::Checked);
		//		useItem->setFlags(useItem->flags() | Qt::ItemFlag::ItemIsEnabled);
		//	}
		//	else{
		//		useItem->setCheckState(Qt::Unchecked);
		//		useItem->setFlags(useItem->flags() & ~Qt::ItemFlag::ItemIsEnabled);
		//	}

		//	ui->sensorsConfigurationTableWidget->setItem(row, UseIDX, useItem);
		//}

		++row;
	}

	ui->sensorsConfigurationTableWidget->blockSignals(false);

	if (profile->calibrationAlgorithm != nullptr){

		for (int i = 2; i < ui->calibrationAlgorithmComboBox->count(); ++i)
		{
			if (ui->calibrationAlgorithmComboBox->itemData(i).value<IMU::IMUCostumeCalibrationAlgorithmConstPtr>()->ID() == profile->calibrationAlgorithm->ID()){
				ui->calibrationAlgorithmComboBox->setCurrentIndex(i);
				break;
			}
		}
	}


	if (profile->motionEstimationAlgorithm != nullptr){
		for (int i = 2; i < ui->motionEstimationAglorithmComboBox->count(); ++i)
		{
			if (ui->motionEstimationAglorithmComboBox->itemData(i).value<IMU::IMUCostumeMotionEstimationAlgorithmConstPtr>()->ID() == profile->motionEstimationAlgorithm->ID()){
				ui->motionEstimationAglorithmComboBox->setCurrentIndex(i);
				break;
			}
		}
	}
}

void IMUCostumeProfileEditionWizard::init(const QString & profileName,
	const imuCostume::Costume::SensorIDsSet & costumeSensors,
	const IMU::IIMUDataSource::OrientationEstimationAlgorithms & orientAlgorithms,
	const IMU::IIMUDataSource::CostumeCalibrationAlgorithms & calibAlgorithms,
	const IMU::IIMUDataSource::CostumeMotionEstimationAlgorithms & motionAlgorithms,
	const IMU::IIMUDataSource::SkeletonModels & skeletonModels,
	const IMU::CostumeProfile::SensorsDescriptions & sensorsDescriptions)
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
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(PreMulRotationIDX, orientationDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(PostMulRotationIDX, orientationDelegate);
	ui->sensorsConfigurationTableWidget->setItemDelegateForColumn(OffsetIDX, vector3DDelegate);

	std::set<std::string> profileJointsNames;

	for (const auto & sd : sensorsDescriptions)
	{
		profileJointsNames.insert(sd.second.jointName);
	}

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
			unsigned int jointsCount = m.second->root()->size();
			unsigned int activeJointsCount = kinematic::LinearizedSkeleton::createNonLeafOrder(*m.second).size();
			ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active)").arg(QString::fromStdString(m.second->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(m.second));

			if (profileJointsNames.empty() == false){
				std::set<std::string> skeletonJointsNames;
				kinematic::LinearizedSkeleton::Visitor::visit(*(m.second), [&skeletonJointsNames](kinematic::Skeleton::JointConstPtr joint)
				{
					skeletonJointsNames.insert(joint->value().name());
				});

				std::vector<std::string> jointsDifference(std::max(skeletonJointsNames.size(), profileJointsNames.size()));

				auto it = std::set_difference(profileJointsNames.begin(), profileJointsNames.end(), skeletonJointsNames.begin(), skeletonJointsNames.end(), jointsDifference.begin());
				if (it != jointsDifference.begin()){
					auto item = model->item(ui->modelComboBox->count()-1);
					item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
					// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
					item->setData(ui->modelComboBox->palette().color(QPalette::Disabled, QPalette::Text));
				}
			}			
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

void IMUCostumeProfileEditionWizard::onSensorsConfigurationTableContextMenu(const QPoint & position)
{
	//wyznaczamy ile mamy zaznaczonych, niezaznaczonych
	unsigned int selected = 0;
	unsigned int unselected = 0;

	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() == Qt::Unchecked){
			++unselected;
		}
		else{
			++selected;
		}
	}

	QMenu menu(ui->sensorsConfigurationTableWidget);
	auto action = menu.addAction(tr("Select All"));
	if (unselected > 0){
		connect(action, SIGNAL(triggered()), this, SLOT(onSelectAllSensors()));
	}
	else{
		action->setEnabled(false);
	}

	action = menu.addAction(tr("Deselect All"));
	if (selected > 0){
		connect(action, SIGNAL(triggered()), this, SLOT(onDeselectAllSensors()));
	}
	else{
		action->setEnabled(false);
	}

	action = menu.addAction(tr("Inverse selection"));
	connect(action, SIGNAL(triggered()), this, SLOT(onInversSensorsSelection()));

	menu.addSeparator();

	action = menu.addAction(tr("Select estimation algorithm"));
	connect(action, SIGNAL(triggered()), this, SLOT(onSelectCommonEstimationAlgorithm()));

	menu.exec(ui->sensorsConfigurationTableWidget->mapToGlobal(position));
}

void IMUCostumeProfileEditionWizard::onSelectAllSensors()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		auto checkState = Qt::PartiallyChecked;
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() == Qt::Unchecked){		

			auto sensorIDData = ui->sensorsConfigurationTableWidget->item(row, SensorIDX)->data(Qt::DisplayRole);
			auto jointData = ui->sensorsConfigurationTableWidget->item(row, JointIDX)->data(Qt::DisplayRole);
			auto algoData = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);

			if (sensorIDData.isValid() && jointData.isValid() && algoData.isValid()){

				auto sid = sensorIDData.toInt();
				auto jointName = jointData.toString();
				auto algo = algoData.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

				if (sid > -1 && jointName != SkeletonJointsDelegate::defaultText()
					&& algo != nullptr){
					checkState = Qt::Checked;
				}
			}			
			
			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(checkState);
			checkStates[row] = checkState;
		}
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);
}

void IMUCostumeProfileEditionWizard::onDeselectAllSensors()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() != Qt::Unchecked){

			ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(Qt::Unchecked);
			checkStates[row] = Qt::Unchecked;
		}
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);
}

void IMUCostumeProfileEditionWizard::onInversSensorsSelection()
{
	ui->sensorsConfigurationTableWidget->blockSignals(true);
	for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		auto checkState = Qt::Unchecked;

		if (ui->sensorsConfigurationTableWidget->item(row, UseIDX)->checkState() != Qt::Unchecked){

		}
		else{

			checkState = Qt::PartiallyChecked;

			auto sensorIDData = ui->sensorsConfigurationTableWidget->item(row, SensorIDX)->data(Qt::DisplayRole);
			auto jointData = ui->sensorsConfigurationTableWidget->item(row, JointIDX)->data(Qt::DisplayRole);
			auto algoData = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);

			if (sensorIDData.isValid() && jointData.isValid() && algoData.isValid()){

				auto sid = sensorIDData.toInt();
				auto jointName = jointData.toString();
				auto algo = algoData.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();

				if (sid > -1 && jointName != SkeletonJointsDelegate::defaultText()
					&& algo != nullptr){
					checkState = Qt::Checked;
				}
			}
		}

		ui->sensorsConfigurationTableWidget->item(row, UseIDX)->setCheckState(checkState);
		checkStates[row] = checkState;
	}
	ui->sensorsConfigurationTableWidget->blockSignals(false);
}

void IMUCostumeProfileEditionWizard::onSelectCommonEstimationAlgorithm()
{
	//najpierw sprawdzamy czy nie ma już tak że wszystkie algo są takie 
	IMU::IIMUOrientationEstimationAlgorithmConstPtr algo;
	bool same = true;

	auto data = ui->sensorsConfigurationTableWidget->item(0, AlgorithmIDX)->data(Qt::UserRole);

	if (data.isValid() == true){
		algo = data.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>();
	}

	for (unsigned int row = 1; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
	{
		data = ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->data(Qt::UserRole);

		if (data.isValid() == true){
			if (algo != data.value<IMU::IIMUOrientationEstimationAlgorithmConstPtr>()){
				same = false;
				break;
			}
		}
		else if (algo != nullptr){
			same = false;
			break;
		}
	}

	QStringList values;
	values << OrientationEstimationAlgorithmDelegate::defaultText();

	int currentID = 0;

	auto algorithms = orientationAlgorithmDelegate->orientationEstimationAlgorithms();

	int i = 1;
	for (const auto & a : algorithms)
	{
		values << QString("%1 (%2)").arg(QString::fromStdString(a.second->name())).arg(QString::fromStdString(boost::lexical_cast<std::string>(a.first)));
		
		if (same == true && algo == a.second){
			currentID = i;
		}

		++i;
	}

	bool ok = true;

	auto item = QInputDialog::getItem(this, tr("Orientation estimation algorithm"), tr("Algorithm"), values, currentID, false, &ok);

	if (ok == true && item != values.first()){
		//ui->sensorsConfigurationTableWidget->blockSignals(true);
		auto it = std::find(values.begin(), values.end(), item);
		unsigned int idx = std::distance(values.begin(), it);
		--idx;

		auto algoIT = algorithms.begin();
		std::advance(algoIT, idx);

		auto nameData = QVariant::fromValue(QString::fromStdString(algoIT->second->name()));
		auto valueData = QVariant::fromValue(algoIT->second);

		for (unsigned int row = 0; row < ui->sensorsConfigurationTableWidget->rowCount(); ++row)
		{
			ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->setData(Qt::DisplayRole, nameData);
			ui->sensorsConfigurationTableWidget->item(row, AlgorithmIDX)->setData(Qt::UserRole, valueData);
		}

		//ui->sensorsConfigurationTableWidget->blockSignals(false);
	}
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
		const auto order = kinematic::LinearizedSkeleton::createNonLeafOrder(*skeleton);
		const auto joints = treeContainer::Linearization::convert(order);
		skeletonJointDelegate->setJoints(joints);

		//przejść przez kolumne z jointami w mapowaniu i wyzerować te dla których nie ma odpowiednika w aktualnym szkielecie
		auto model = ui->sensorsConfigurationTableWidget->model();
		for (unsigned int i = 0; i < model->rowCount(); ++i)
		{
			auto index = model->index(i, JointIDX);
			auto jointName = model->data(index, Qt::DisplayRole).toString().toStdString();
			if (joints.right.find(jointName) == joints.right.end()){
				model->setData(index, SkeletonJointsDelegate::defaultText());
				model->setData(index, SkeletonJointsDelegate::defaultValue(), Qt::UserRole);
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

			int partial = 0;

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
	//verify other rows - target to ok: at least one checked and/or no partial checks
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
	}

	ret.activeJoints = ret.motionEstimationAlgorithm->activeJoints(ret.skeleton, sds);

	return ret;
}

void IMUCostumeProfileEditionWizard::onLoadModel()
{
	auto file = QFileDialog::getOpenFileName(this, tr("Open model file"), QString(), tr("Acclaim skeleton file file (*.asf)"));

	core::Filesystem::Path p(file.toStdString());
	if (core::Filesystem::isRegularFile(p)) {
		try{
			std::ifstream file(p.string());
			auto as = acclaim::AsfParser::parse(file, true);
			kinematic::Skeleton skeleton;
			kinematic::Skeleton::convert(as, skeleton);

			boost::uuids::basic_random_generator<boost::mt19937> gen;
			boost::uuids::uuid u = gen();

			auto registeredSkeleton = utils::make_shared<const IMU::Skeleton>(u, as.name, skeleton);

			unsigned int jointsCount = registeredSkeleton->root()->size();
			unsigned int activeJointsCount = kinematic::LinearizedSkeleton::createNonLeafOrder(*registeredSkeleton).size();
			ui->modelComboBox->addItem(tr("Model %1: %2 joints (%3 active").arg(QString::fromStdString(registeredSkeleton->name)).arg(jointsCount).arg(activeJointsCount), QVariant::fromValue(registeredSkeleton));

			///------------------------------------------------
			std::set<std::string> profileJointsNames;			

			for (unsigned int i = 0; i < ui->sensorsConfigurationTableWidget->rowCount(); ++i)
			{
				profileJointsNames.insert(ui->sensorsConfigurationTableWidget->item(i, JointIDX)->data(Qt::DisplayRole).toString().toStdString());
			}

			if (profileJointsNames.size() != ui->sensorsConfigurationTableWidget->rowCount())
			{
				std::set<std::string>().swap(profileJointsNames);
			}

			if (profileJointsNames.empty() == false){

				const QStandardItemModel* model = qobject_cast<const QStandardItemModel*>(ui->modelComboBox->model());

				std::set<std::string> skeletonJointsNames;
				kinematic::LinearizedSkeleton::Visitor::visit(*registeredSkeleton, [&skeletonJointsNames](kinematic::Skeleton::JointConstPtr joint)
				{
					skeletonJointsNames.insert(joint->value().name());
				});

				std::vector<std::string> jointsDifference(std::max(skeletonJointsNames.size(), profileJointsNames.size()));

				auto it = std::set_difference(profileJointsNames.begin(), profileJointsNames.end(), skeletonJointsNames.begin(), skeletonJointsNames.end(), jointsDifference.begin());
				if (it != jointsDifference.begin()){
					auto item = model->item(ui->modelComboBox->count()-1);
					item->setFlags(item->flags() & ~(Qt::ItemIsSelectable | Qt::ItemIsEnabled));
					// visually disable by greying out - works only if combobox has been painted already and palette returns the wanted color
					item->setData(ui->modelComboBox->palette().color(QPalette::Disabled, QPalette::Text));
				}
			}
			///------------------------------------------------

			ui->modelComboBox->setCurrentIndex(ui->modelComboBox->count() - 1);

			//poprawić indexy stawów w konfiguracji
			auto localMapping = kinematic::LinearizedSkeleton::createNonLeafMapping(*registeredSkeleton);
			
			for (unsigned int i = 0; i < ui->sensorsConfigurationTableWidget->rowCount(); ++i)
			{
				auto it = localMapping.data().right.find(ui->sensorsConfigurationTableWidget->item(i, JointIDX)->data(Qt::DisplayRole).toString().toStdString());
				ui->sensorsConfigurationTableWidget->model()->setData(ui->sensorsConfigurationTableWidget->model()->index(i, JointIDX), it->get_left(), Qt::UserRole);
			}
		}
		catch (std::exception & e){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with following error: ").arg(file) + QString::fromStdString(e.what()));
		}
		catch (...){
			QMessageBox::critical(this, tr("Loading model failure"), tr("Failed to load model file: %1 with unknown error"));
		}
	}
}
