#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtWidgets/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <corelib/Filesystem.h>
#include "IMUCostumeCalibrationWizard.h"
#include "corelib/IVisualizerManager.h"
#include "corelib/PluginCommon.h"
#include "utils/ObjectWrapper.h"
#include "osg/PositionAttitudeTransform"
#include "osg/MatrixTransform"
#include "osg/Geode"
#include "osg/ShapeDrawable"
#include "osg/PolygonMode"
#include "osg/ref_ptr"
#include "osgDB/ReadFile"
#include "osg/LineWidth"
#include "osg/StateAttribute"
#include "osg/StateSet"
#include <imucostumelib/CostumeRawIO.h>
#include <imucostumelib/ImuCostume.h>
#include <imucostumelib/CANopenSensor.h>
#include <imucostumelib/ImuSensor.h>
#include <boost/algorithm/hex.hpp>
#include <thread>
#include "IMUCostumeProfileEditionWizard.h"
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QProgressBar>
#include "CostumeSkeletonMotionHelper.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>
#include "IMUCostumeProfileConfigurationWizard.h"

Q_DECLARE_METATYPE(imuCostume::CostumeRawIO::CostumeAddress);

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

QIcon statusIcon(IMU::IIMUDataSource::ConnectionStatus connectionStatus)
{
	switch (connectionStatus)
	{
	case IMU::IIMUDataSource::ONLINE:
		return QIcon(QString::fromUtf8(":/imuCostume/icons/dostepny.png"));		
	case IMU::IIMUDataSource::OFFLINE:
		return QIcon(QString::fromUtf8(":/imuCostume/icons/niedostepny.png"));		
	case IMU::IIMUDataSource::CONNECTION_PROBLEMS:
		return QIcon(QString::fromUtf8(":/imuCostume/icons/czesciowodostepny.png"));
	case IMU::IIMUDataSource::UNKNOWN:
		return QIcon(QString::fromUtf8(":/imuCostume/icons/nieznanydostepny.png"));
	default:
		return QIcon(QString::fromUtf8(":/imuCostume/icons/nieznanydostepny.png"));
	}
}

QString statusDescription(IMU::IIMUDataSource::ConnectionStatus connectionStatus)
{
	switch (connectionStatus)
	{
	case IMU::IIMUDataSource::ONLINE:
		return QObject::tr("online");
	case IMU::IIMUDataSource::OFFLINE:
		return QObject::tr("offline");
	case IMU::IIMUDataSource::CONNECTION_PROBLEMS:
		return QObject::tr("problematic");
	case IMU::IIMUDataSource::UNKNOWN:
		return QObject::tr("unknown");
	default:
		return QObject::tr("unknown");
	}
}

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);
	ui->costumesTreeWidget->clear();
	ui->sensorsTree->clear();
	connect(&statusRefreshTimer, SIGNAL(timeout()), this, SLOT(refreshStatus()));
	statusRefreshTimer.start(1000 / 25);
	QTimer::singleShot(0, this, SLOT(onRefresh()));
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	ui->sensorsTree->clear();
	auto id = current->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

	auto cd = ds->costumeDetails(id);	

	for (const auto & st : cd.descriptionDetails.sensorsConfiguration){
		for (const auto sid : st.second){
			auto sstatus = cd.statusDetails.sensorsStatus.find(sid)->second;
			auto sItem = new QTreeWidgetItem;
			sItem->setText(0, QObject::tr("Sensor %1").arg(sid));
			sItem->setData(0, Qt::UserRole, QVariant::fromValue(sid));
			sItem->setIcon(1, statusIcon(sstatus));
			sItem->setText(1, statusDescription(sstatus));
			ui->sensorsTree->addTopLevelItem(sItem);
		}
	}	
}

void IMUCostumeWidget::onCostumesListContextMenu(const QPoint & position)
{
	QMenu * menu = new QMenu(this);
	const auto cc = ds->costumesCout();

	if(cc > 0){

		auto loadAll = menu->addAction(tr("Load all"));
		auto unloadAll = menu->addAction(tr("Unload all"));	

		if (cc != ds->loadedCostumesCount()){
			loadAll->setEnabled(true);
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoadAll()));
		}else{
			loadAll->setEnabled(false);
		}

		if (ds->loadedCostumesCount() > 0){
			unloadAll->setEnabled(true);
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnloadAll()));
		}else{
			unloadAll->setEnabled(false);
		}

		if(ui->costumesTreeWidget->currentItem() != nullptr){

			const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

			menu->addSeparator();

			auto load = menu->addAction(tr("Load"));
			auto unload = menu->addAction(tr("Unload"));
			auto cl = ds->costumeLoaded(id);

			load->setEnabled(cl == false);
			unload->setEnabled(cl == true);

			connect(load, SIGNAL(triggered()), this, SLOT(onLoad()));
			connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
		}
	}else{
		menu->addAction("--- " + tr("No options") + " ---");
	}
			
	menu->exec(ui->costumesTreeWidget->mapToGlobal(position));
}

void IMUCostumeWidget::refreshStatus()
{
	auto cStatus = ds->costumesStatus();
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto item = ui->costumesTreeWidget->topLevelItem(i);
		auto id = item->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		auto it = cStatus.find(id);

		auto iS = IMU::IIMUDataSource::UNKNOWN;

		if (it != cStatus.end()){
			iS = it->second.status;

			for (unsigned int j = 0; j < ui->sensorsTree->topLevelItemCount(); ++j)
			{
				auto sItem = ui->sensorsTree->topLevelItem(j);
				auto id = sItem->data(0, Qt::UserRole).value<imuCostume::Costume::SensorID>();
				auto iSS = IMU::IIMUDataSource::UNKNOWN;
				auto sit = it->second.sensorsStatus.find(id);
				if (sit != it->second.sensorsStatus.end())
				{
					iSS = sit->second;
				}

				sItem->setIcon(1, statusIcon(iS));
				sItem->setText(1, statusDescription(iS));
			}
		}

		item->setIcon(1, statusIcon(iS));
		item->setText(1, statusDescription(iS));
	}
}

void IMUCostumeWidget::onRefresh()
{
	bool failed = false;
	QString message;
	try{
		ds->refreshCostumes();
		ui->costumesTreeWidget->clear();
		ui->sensorsTree->clear();

		auto cDetails = ds->costumesDetails();

		for (const auto & cd : cDetails){
			auto ci = new QTreeWidgetItem;
			ci->setText(0, QString("%1:%2").arg(QString::fromStdString(cd.second.descriptionDetails.rawCostume->ip())).arg(cd.second.descriptionDetails.rawCostume->port()));
			ci->setData(0, Qt::UserRole, QVariant::fromValue(imuCostume::CostumeRawIO::CostumeAddress({ cd.second.descriptionDetails.rawCostume->ip(), cd.second.descriptionDetails.rawCostume->port() })));
			ci->setIcon(1, statusIcon(cd.second.statusDetails.status));
			ci->setText(1, statusDescription(cd.second.statusDetails.status));
			ui->costumesTreeWidget->addTopLevelItem(ci);
		}
	}
	catch (std::exception & e)
	{
		failed = true;
		message = tr("Error while refreshing costumes list: ") + QString::fromStdString(e.what());
	}
	catch (...){
		failed = true;
		message = tr("Unknown error while refreshing costumes list - probably network failure");
	}

	if (failed == true)
	{
		QMessageBox::critical(this, tr("Refresh error"), message);
	}
}

void IMUCostumeWidget::onLoad()
{
	const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

	auto cd = ds->costumeDescription(id);
	const auto & sc = cd.sensorsConfiguration;
	imuCostume::Costume::SensorIDsSet sIDs;

	auto it = sc.find(imuCostume::Costume::IMU);
	if (it != sc.end()){
		IMUCostumeProfileEditionWizard ew(it->second, tr("New profile"), this);

		auto res = ew.exec();

		if (res != QDialog::Accepted){
			return;
		}

		PLUGIN_LOG_DEBUG("Profile creation done");

		auto profileInstance = IMU::IIMUDataSource::CostumeProfileInstance::create(ew.costumeProfile());
		auto cw = IMU::IMUCostumeProfileConfigurationWizard::create(profileInstance);
		if (cw != nullptr){
			res = cw->exec();
			if (res != QDialog::Accepted){
				return;
			}

			PLUGIN_LOG_DEBUG("Profile configuration done");
		}

		profileInstance.calibrationAlgorithm->initialize(profileInstance.skeleton, profileInstance.sensorsMapping, profileInstance.sensorsAdjustments);		

		auto canOpenStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::RawDataStream::value_type, IMU::CANopenFramesStream::value_type, IMU::RawToCANopenExtractor>>(cd.rawDataStream, IMU::RawToCANopenExtractor());

		class CANopenDataExtractor
		{
		public:

			CANopenDataExtractor() {}

			bool verify(const IMU::CANopenFramesStream::value_type & a)
			{
				return true;
			}

			void extract(const IMU::CANopenFramesStream::value_type & a, IMU::CostumeStream::value_type & ret) const
			{
				PLUGIN_LOG_DEBUG("CANopenDataExtractor");
				ret = imuCostume::Costume::convert(a);
			}
		};

		unsigned int initializeFramesCount = 50;

		for (const auto & sa : profileInstance.sensorsOrientationEstimationAlgorithms)
		{
			initializeFramesCount = std::max(initializeFramesCount, sa.second->approximateEstimationDelay());
		}

		auto costumeStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::CANopenFramesStream::value_type, IMU::CostumeStream::value_type, IMU::CANopenDataExtractor>>(canOpenStream, IMU::CANopenDataExtractor());

		//mamy wszystko, jesteœmy po konfiguracji wszystkiego
		//mo¿na dalej konfigurowaæ kostium - inicjalizowaæ filtry, kalibrowaæ i ³adowaæ ca³oœæ do DataManager
		auto extractorAdapter = utils::make_shared<ExtractedCostumeStreamAdapter>(costumeStream, IMU::CostumeIMUExtractor(cd.sensorsConfiguration));

		CostumeSkeletonMotionHelper csmh(extractorAdapter,
			&profileInstance, initializeFramesCount + profileInstance.calibrationAlgorithm->maxCalibrationSteps(),
			initializeFramesCount, this);

		res = csmh.exec();

		if (res == QDialog::Rejected){
			return;
		}

		PLUGIN_LOG_DEBUG("Costume initialization");

		try{
			profileInstance.motionEstimationAlgorithm->initialize(profileInstance.skeleton,
				profileInstance.calibrationAlgorithm->sensorsAdjustemnts(), profileInstance.sensorsMapping);

			ds->loadCalibratedCostume(id, profileInstance);
		}
		catch (...){
			QMessageBox::critical(this, tr("Failed to load calibrated costume"), tr("Internal error"));
		}

		PLUGIN_LOG_DEBUG("Costume loaded");
	}
	else{

	}
}

void IMUCostumeWidget::onUnload()
{
	const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	ds->unloadCostume(id);
}

void IMUCostumeWidget::onLoadAll()
{
	//ds->loadAllCostumes();
}

void IMUCostumeWidget::onUnloadAll()
{
	//ds->unloadAllCostumes();
}

void IMUCostumeWidget::onLoadDatFile()
{
    //ds->testMethod();
    //return;
    QString file = QFileDialog::getOpenFileName(this, tr("Open Image"), QString(), tr("Imu file (*.dat)"));
    core::Filesystem::Path p(file.toStdString());
    if (core::Filesystem::isRegularFile(p)) {
        //ds->loadDatFile(p);
    }
    
}

void IMUCostumeWidget::testCommunication()
{
	try{

		auto costumeList = imuCostume::CostumeRawIO::listAvailableCostumes();

		if (costumeList.empty() == false){
			auto costumeAddress = costumeList.front();

			std::unique_ptr<imuCostume::CostumeRawIO> rawCostume(new imuCostume::CostumeRawIO(costumeAddress.ip));
		}
	}
	catch (std::exception & e){
		std::string errorcode = e.what();
		errorcode += "_";
	}
	catch (...){
		std::string errorcode = "unknown error";
		errorcode += "_";
	}
}