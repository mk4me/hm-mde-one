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
#include "IMUCostumeConfigurationWizard.h"
#include <plugins/imuCostume/IIMUDataSource.h>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QProgressBar>
#include "CostumeSkeletonMotionHelper.h"
#include <plugins/imuCostume/CostumeIMUExtractor.h>

typedef threadingUtils::StreamAdapterT<IMU::CostumeStream::value_type, IMU::SensorsStreamData, IMU::CostumeIMUExtractor> ExtractedCostumeStreamAdapter;

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);
	ui->costumesTreeWidget->clear();
	QTimer::singleShot(0, this, SLOT(onRefresh()));
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	ui->sensorsTree->clear();
	auto idx = ui->costumesTreeWidget->indexOfTopLevelItem(current);

	if (idx < ds->costumesCout()){
		const auto & cc = ds->costumeDescription(idx).sensorsConfiguration;

		for (const auto & st : cc){
			for (const auto sid : st.second){
				auto sItem = new QTreeWidgetItem;
				sItem->setText(0, QObject::tr("Sensor %1").arg(sid));
				ui->sensorsTree->addTopLevelItem(sItem);
			}
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

			const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

			menu->addSeparator();

			auto load = menu->addAction(tr("Load"));
			auto unload = menu->addAction(tr("Unload"));
			auto cl = ds->costumeLoaded(idx);

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

void IMUCostumeWidget::onRefresh()
{
	IMUCostumeConfigurationWizard w("", "5", 6, 7);

	auto r = w.exec();

	bool failed = false;
	QString message;
	try{
		ds->refreshCostumes();
		ui->costumesTreeWidget->clear();

		for (unsigned int i = 0; i < ds->costumesCout(); ++i){
			const auto & cd = ds->costumeDescription(i);
			auto ci = new QTreeWidgetItem;
			ci->setText(0, QString::fromStdString(cd.rawCostume->ip()));
			ci->setData(0, Qt::UserRole, i);
			ci->setText(1, QString("%1").arg(cd.status->load()));
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

bool configureCostume(IMU::IMUCostumeDataSource * ds, const unsigned int idx,
	IMUCostumeConfigurationWizard::SensorsMappingDetails & mappingDetails,
	IMU::IMUCostumeCalibrationAlgorithmConstPtr & calibrationAlgorithm,
	IMU::IMUCostumeMotionEstimationAlgorithmConstPtr & motionEstimationAlgorithm,
	kinematic::SkeletonConstPtr & skeleton)
{
	const auto & cd = ds->costumeDescription(idx);
	const auto & cfg = cd.sensorsConfiguration;
	auto imuIT = cfg.find(imuCostume::Costume::IMU);
	auto grfIT = cfg.find(imuCostume::Costume::INSOLE);

	IMUCostumeConfigurationWizard w("", cd.rawCostume->ip(), imuIT != cfg.end() ? imuIT->second.size() : 0, grfIT != cfg.end() ? grfIT->second.size() : 0);

	if (w.exec() == QDialog::Accepted)
	{
		mappingDetails = w.mappingDetails();
		calibrationAlgorithm = w.calibrationAlgorithm();
		motionEstimationAlgorithm = w.motionEstimationAlgorithm();
		skeleton = w.skeleton();
		return true;
	}

	return false;
}

bool configureAlgorithms(QWidget * parent, const IMUCostumeConfigurationWizard::SensorsMappingDetails & mappingDetails,
	IMU::IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithmProto,
	IMU::IMUCostumeCalibrationAlgorithmPtr & calibrationAlgorithm,
	IMU::IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithmProto,
	IMU::IMUCostumeMotionEstimationAlgorithmPtr & motionEstimationAlgorithm,
	kinematic::SkeletonConstPtr skeleton,
	IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts &sensorsAdjustment,
	IMU::SensorsMapping &sensorsMapping,
	IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping & estimationAlgorithms,
	unsigned int & initEstimationAlgorithmsSamples,
	unsigned int & calibrationAlgorithmMaxSamples)
{
	struct WizzardPageDetails
	{
		QString title;
		QString subTitle;
		QWidget * widget;
	};

	std::list<WizzardPageDetails> pages;

	initEstimationAlgorithmsSamples = 50;

	for (const auto & md : mappingDetails)
	{
		sensorsAdjustment.insert(IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts::value_type(md.first, { md.second.sensorAdjustment.rotation, md.second.sensorAdjustment.offset }));
		sensorsMapping.insert(IMU::SensorsMapping::value_type(md.first, md.second.jointName));

		IMU::IIMUOrientationEstimationAlgorithmPtr algorithm(md.second.estimationAlgorithm->create());

		estimationAlgorithms.insert(IMU::IIMUDataSource::OrientationEstimationAlgorithmsMapping::value_type(md.first, algorithm));

		auto w = algorithm->configurationWidget();

		if (w != nullptr){
			WizzardPageDetails pd;
			pd.title = QObject::tr("Orientation estimation algorithms configuration");
			pd.subTitle = QObject::tr("Sensor %1 -> Joint %2 : %3 algorithm").arg(md.first).arg(QString::fromStdString(md.second.jointName)).arg(QString::fromStdString(algorithm->name()));
			pd.widget = w;
			pages.push_back(pd);
		}

		initEstimationAlgorithmsSamples = std::max(initEstimationAlgorithmsSamples, algorithm->approximateEstimationDelay());
	}

	calibrationAlgorithm.reset(calibrationAlgorithmProto->create());
	calibrationAlgorithm->initialize(skeleton, sensorsMapping, sensorsAdjustment);

	calibrationAlgorithmMaxSamples = std::max<unsigned int>(100, calibrationAlgorithm->maxCalibrationSteps());
	calibrationAlgorithmMaxSamples = std::min<unsigned int>(calibrationAlgorithmMaxSamples, 1000);

	auto w = calibrationAlgorithm->configurationWidget();
	if (w != nullptr){
		WizzardPageDetails pd;
		pd.title = QObject::tr("Calibration configuration");
		pd.subTitle = QObject::tr("Algorithm %1").arg(QString::fromStdString(calibrationAlgorithm->name()));
		pd.widget = w;
		pages.push_back(pd);
	}

	motionEstimationAlgorithm.reset(motionEstimationAlgorithmProto->create());	

	w = motionEstimationAlgorithm->configurationWidget();
	if (w != nullptr){
		WizzardPageDetails pd;
		pd.title = QObject::tr("Motion estimation configuration");
		pd.subTitle = QObject::tr("Algorithm %1").arg(QString::fromStdString(motionEstimationAlgorithm->name()));
		pd.widget = w;
		pages.push_back(pd);
	}

	if (pages.empty() == false){

		QWizard wizard(parent);

		QWizardPage * wp = nullptr;

		for (const auto & p : pages)
		{
			wp = new QWizardPage(&wizard);
			auto layout = new QVBoxLayout;
			layout->addWidget(p.widget);
			wp->setLayout(layout);
			wp->setTitle(p.title);
			wp->setSubTitle(p.subTitle);
			wizard.addPage(wp);
		}

		wp->setFinalPage(true);

		return wizard.exec() == QDialog::Accepted;
	}

	return true;
}

void IMUCostumeWidget::onLoad()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	IMUCostumeConfigurationWizard::SensorsMappingDetails mappingDetails;

	IMU::IMUCostumeCalibrationAlgorithmConstPtr calibrationAlgorithmProto;

	IMU::IMUCostumeMotionEstimationAlgorithmConstPtr motionEstimationAlgorithmProto;

	kinematic::SkeletonConstPtr skeleton;

	if (configureCostume(ds, idx, mappingDetails, calibrationAlgorithmProto,
		motionEstimationAlgorithmProto, skeleton) == false){

		return;
	}

	IMU::IMUCostumeCalibrationAlgorithmPtr calibrationAlgorithm;

	IMU::IMUCostumeMotionEstimationAlgorithmPtr motionEstimationAlgorithm;

	IMU::IMUCostumeCalibrationAlgorithm::SensorsAdjustemnts sensorsAdjustment;
	IMU::SensorsMapping sensorsMapping;
	std::map<imuCostume::Costume::SensorID, IMU::IIMUOrientationEstimationAlgorithmPtr> estimationAlgorithms;

	unsigned int initEstimationAlgorithmsSamples = 0;
	unsigned int calibrationAlgorithmMaxSamples = 0;

	if (configureAlgorithms(this, mappingDetails, calibrationAlgorithmProto, calibrationAlgorithm,
		motionEstimationAlgorithmProto, motionEstimationAlgorithm,
		skeleton, sensorsAdjustment, sensorsMapping, estimationAlgorithms,
		initEstimationAlgorithmsSamples, calibrationAlgorithmMaxSamples) == false){

		return;
	}

	PLUGIN_LOG_DEBUG("Configure done");

	const auto cd = ds->costumeDescription(idx);

	const auto & sc = cd.sensorsConfiguration;	

	//! TODO - adaptor
	class RawToCANopenExtractor
	{
	public:

		RawToCANopenExtractor() {}
		~RawToCANopenExtractor() {}

		bool verify(const IMU::RawDataStream::value_type & val) const
		{
			return true;
		}

		void extract(const IMU::RawDataStream::value_type & in, IMU::CANopenFramesStream::value_type & out) const
		{
			out = imuCostume::CostumeCANopenIO::extractData(in.buffer.get(), in.length);
		}
	};

	auto canOpenStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::RawDataStream::value_type, IMU::CANopenFramesStream::value_type, RawToCANopenExtractor>>(cd.rawDataStream, RawToCANopenExtractor());

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
			ret = imuCostume::Costume::convert(a);
		}
	};

	auto costumeStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::CANopenFramesStream::value_type, IMU::CostumeStream::value_type, CANopenDataExtractor>>(canOpenStream, CANopenDataExtractor());

	//TODO - mamy wszystko, jesteœmy po konfiguracji wszystkiego
	//mo¿na dalej konfigurowaæ kostium - inicjalizowaæ filtry, kalibrowaæ i ³adowaæ ca³oœæ do DataManager
	//utils::shared_ptr<ExtractedCostumeStreamAdapter> extractorAdapter(new ExtractedCostumeStreamAdapter());
	auto extractorAdapter = utils::make_shared<ExtractedCostumeStreamAdapter>(costumeStream, IMU::CostumeIMUExtractor(cd.sensorsConfiguration));

	CostumeSkeletonMotionHelper csmh(extractorAdapter,
		estimationAlgorithms,
		calibrationAlgorithm,
		initEstimationAlgorithmsSamples + calibrationAlgorithmMaxSamples,
		initEstimationAlgorithmsSamples, this);

	PLUGIN_LOG_DEBUG("Calib done");

	int res = csmh.exec();

	if (res == QDialog::Rejected){
		return;
	}

	try{
		motionEstimationAlgorithm->initialize(skeleton, calibrationAlgorithm->sensorsAdjustemnts(), sensorsMapping);
		ds->loadCalibratedCostume(idx, skeleton, sensorsMapping, sensorsAdjustment, estimationAlgorithms, motionEstimationAlgorithm);
	}
	catch (...){
		QMessageBox::critical(this, tr("Failed to load calibrated costume"), tr("Internal error"));
	}
}

void IMUCostumeWidget::onUnload()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();
	ds->unloadCostume(idx);
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

/*
void IMUCostumeWidget::refreshData()
{
	QString message;

	bool change = false;

	for (const auto & cd : costumeStreams){
		message += "<B>Costume " + QString::fromStdString(cd.first) + "</B><BR>";

		if (cd.second.rawStreamObserver->modified() == true){
			change = true;
			imuCostume::ProtocolSendBufferHelper::Buffer buffer;
			cd.second.rawStream->data(buffer);
			message += QString("<U>Raw data:</U><BR><UL><LI>Length: %1</LI><LI>Data:<BR>").arg(buffer.length);
			uint8_t * c = buffer.buffer.get();

			boost::algorithm::hex(c, c + buffer.length, std::back_inserter(message));

			//for (unsigned int i = 0; i < buffer.length; ++i){				
			//	message += QString("%1").arg(*c++, 2, 16, QLatin1Char('0'));
			//}
			message += "<BR></LI></UL><BR>";

			change = true;
		}

		if (cd.second.costumeStreamObserver->modified() == true){
			change = true;
			imuCostume::Costume::Data data;
			cd.second.costumeStream->data(data);
			message += QString("<U>Unpacked costume data:</U><UL><LI>Timestamp: %1</LI><LI>Data:<UL>").arg(data.timestamp);

			if (data.sensorsData.empty() == true){
				message += "<LI>No data</LI>";
			}
			else{
				for (const auto & sd : data.sensorsData){
					message += QString("<LI>Sensor %1:<UL><LI>Type: %2</LI>").arg(sd->id()).arg(sd->type());

					auto imuData = utils::dynamic_pointer_cast<const imuCostume::Costume::IMUSensor>(sd);
					if (imuData == nullptr){
						message += "<LI>No data or unknown data</LI>";
					}
					else{
						message += QString("<LI>Status: %1</LI><LI>Accelerometer: ").arg(imuData->dataStatus());
						if (imuData->dataStatus() & imuCostume::Costume::IMUSensor::ACC_DATA){
							message += QString("[%1, %2, %3]").arg(imuData->accelerometer().x()).arg(imuData->accelerometer().y()).arg(imuData->accelerometer().z());
						}
						else{
							message += "No data";
						}
						message += "</LI>";

						message += QString("<LI>Gyroscope: ");
						if (imuData->dataStatus() & imuCostume::Costume::IMUSensor::GYRO_DATA){
							message += QString("[%1, %2, %3]").arg(imuData->gyroscope().x()).arg(imuData->gyroscope().y()).arg(imuData->gyroscope().z());
						}
						else{
							message += "No data";
						}
						message += "</LI>";

						message += QString("<LI>Magnetometer: ");
						if (imuData->dataStatus() & imuCostume::Costume::IMUSensor::MAG_DATA){
							message += QString("[%1, %2, %3]").arg(imuData->magnetometer().x()).arg(imuData->magnetometer().y()).arg(imuData->magnetometer().z());
						}
						else{
							message += "No data";
						}
						message += "</LI>";

						message += QString("<LI>Orientation: ");
						if (imuData->dataStatus() & imuCostume::Costume::IMUSensor::ORIENT_DATA){
							message += QString("[(%1, %2, %3), %4]").arg(imuData->orientation().x()).arg(imuData->orientation().y()).arg(imuData->orientation().z()).arg(imuData->orientation().w());
						}
						else{
							message += "No data";
						}
						message += "</LI></UL></LI>";
					}
				}
			}

			message += "</UL></LI></UL>";
		}
	}

	if (change == true){
		ui->textEdit->setText(message);
	}
	else{
		ui->textEdit->setText("No data");
	}
}
*/