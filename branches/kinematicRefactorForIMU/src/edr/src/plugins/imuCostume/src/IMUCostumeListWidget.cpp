#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtWidgets/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <corelib/Filesystem.h>
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
#include <QtWidgets/QInputDialog>
#include "RecordingWizard.h"
#include <fstream>
#include "CostumeParser.h"
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

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
	connect(&recordTimer, SIGNAL(timeout()), this, SLOT(watchRecordedData()));
	statusRefreshTimer.start(1000 / 25);
	QTimer::singleShot(0, this, SLOT(onRefresh()));
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::innerCostumeChange(const imuCostume::CostumeRawIO::CostumeAddress & costumeID)
{
	ui->sensorsTree->clear();

	auto cd = ds->costumeDetails(costumeID);

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

void IMUCostumeWidget::onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	if (current != nullptr){
		auto id = current->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		innerCostumeChange(id);
	}
}

void IMUCostumeWidget::onSensoresListContextMenu(const QPoint & position)
{
	auto cItem = ui->costumesTreeWidget->currentItem();
	auto sItem = ui->sensorsTree->currentItem();

	if (sItem != nullptr && cItem != nullptr){
		QMenu menu(this);

		auto resetStatus = menu.addAction(tr("Reset status"));
		connect(resetStatus, SIGNAL(triggered()), this, SLOT(onResetSensorConnectionStatus()));

		menu.exec(ui->sensorsTree->mapToGlobal(position));
	}
}

void IMUCostumeWidget::onCostumesListContextMenu(const QPoint & position)
{
	const auto cc = ds->costumesCout();

	if(cc > 0){
		QMenu menu(this);
		auto lcc = ds->loadedCostumesCount();

		auto loadAll = menu.addAction(tr("Load all"));
		auto unloadAll = menu.addAction(tr("Unload all"));
		auto setSamplingRateAll = menu.addAction(tr("Set sampling rate all"));
		auto rsConfigAll = menu.addAction(tr("Refresh sensors configuration all"));
		auto resetStatusAll = menu.addAction(tr("Reset connection status all"));

		if (cc > ds->loadedCostumesCount()){
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoadAll()));
			connect(setSamplingRateAll, SIGNAL(triggered()), this, SLOT(onSetSamplingRateAll()));
			connect(rsConfigAll, SIGNAL(triggered()), this, SLOT(onRefreshSensorsConfigurationAll()));
			connect(resetStatusAll, SIGNAL(triggered()), this, SLOT(onResetCostumeConnectionStatusAll()));
		}else{
			loadAll->setEnabled(false);
			setSamplingRateAll->setEnabled(false);
			rsConfigAll->setEnabled(false);
			resetStatusAll->setEnabled(false);			
		}

		if (lcc > 0){			
			connect(unloadAll, SIGNAL(triggered()), this, SLOT(onUnloadAll()));
		}else{
			unloadAll->setEnabled(false);
		}		

		if(ui->costumesTreeWidget->currentItem() != nullptr){

			const auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();

			menu.addSeparator();

			auto load = menu.addAction(tr("Load"));
			auto unload = menu.addAction(tr("Unload"));
			auto setSamplingRate = menu.addAction(tr("Set sampling rate"));
			auto rsConfig = menu.addAction(tr("Refresh sensors configuration"));
			auto resetStatus = menu.addAction(tr("Reset connection status"));

			auto cl = ds->costumeLoaded(id);

			if (cl == false){
				connect(load, SIGNAL(triggered()), this, SLOT(onLoad()));
				connect(setSamplingRate, SIGNAL(triggered()), this, SLOT(onSetSamplingRate()));
				connect(rsConfig, SIGNAL(triggered()), this, SLOT(onRefreshSensorsConfiguration()));
				connect(resetStatus, SIGNAL(triggered()), this, SLOT(onResetCostumeConnectionStatus()));
				unload->setEnabled(false);
			}
			else{
				connect(unload, SIGNAL(triggered()), this, SLOT(onUnload()));
				load->setEnabled(false);
				setSamplingRate->setEnabled(false);
				rsConfig->setEnabled(false);
				resetStatus->setEnabled(false);
			}		
		}

		menu.exec(ui->costumesTreeWidget->mapToGlobal(position));
	}
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

			if (item == ui->costumesTreeWidget->currentItem()){

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

					sItem->setIcon(1, statusIcon(iSS));
					sItem->setText(1, statusDescription(iSS));
				}
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
		coreUI::CoreCursorChanger cc;
		ds->refreshCostumes();
		ui->costumesTreeWidget->clear();
		ui->sensorsTree->clear();

		auto cDetails = ds->costumesDetails();

		ui->recordPushButton->setEnabled(cDetails.empty() == false);

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

		IMU::CostumeProfile profile;

		auto profiles = ds->costumesProfiles();

		if (profiles.empty() == false){
			QStringList plist;

			for (const auto & p : profiles)
			{
				plist.push_back(QString::fromStdString(p.second.name));
			}

			bool ok = false;
			auto profileName = QInputDialog::getItem(this, tr("Choose predefined profile"), tr("Profile"), plist, 0, false, &ok);

			if (ok == false){

				IMUCostumeProfileEditionWizard ew(it->second, tr("New profile"), this);

				auto res = ew.exec();

				if (res != QDialog::Accepted){
					return;
				}

				profile = ew.costumeProfile();

				res = QMessageBox::question(this, tr("Save new profile"), tr("Would You like to save new profile?"));

				if (res == QMessageBox::Yes){
					bool ok = true;
					QString pName;
					while (ok == true){
						pName = QInputDialog::getText(this, tr("Profile name"), tr("Input new profile name"), QLineEdit::Normal, "", &ok);
						if (ok == false){
							
						}else if(plist.contains(pName) == true){
							QMessageBox::information(this, tr("Profile name"), tr("Profile name must be unique and non empty - please try again"));
						}
						else{
							ok = false;
						}
					}

					if (pName.isEmpty() == false){
						profile.name = pName.toStdString();
						ds->registerCostumeProfile(profile);
						QMessageBox::information(this, tr("Profile registered"), tr("Profile registration complete"));
					}
				}
			}
			else{

				auto it = profiles.find(profileName.toStdString());

				profile = it->second;

				auto ret = QMessageBox::question(this, tr("Profile edition?"), tr("Would You like to edit selected profile or use it directly?"), tr("Edit"), tr("Use"));

				if (ret == 0){
					IMUCostumeProfileEditionWizard ew(profile, this);

					auto res = ew.exec();

					if (res == QDialog::Accepted){
						auto p = ew.costumeProfile();
						//TODO
						if (false){

							res = QMessageBox::question(this, tr("Save new profile"), tr("Would You like to save new profile?"));

							if (res == QMessageBox::Yes){
								bool ok = true;
								QString pName;
								while (ok == true){
									pName = QInputDialog::getText(this, tr("Profile name"), tr("Input new profile name"), QLineEdit::Normal, "", &ok);
									if (ok == false){

									}
									else if (plist.contains(pName) == true){
										QMessageBox::information(this, tr("Profile name"), tr("Profile name must be unique and non empty - please try again"));
									}
									else{
										ok = false;
									}
								}

								if (pName.isEmpty() == false){
									profile.name = pName.toStdString();
									ds->registerCostumeProfile(profile);
									QMessageBox::information(this, tr("Profile registered"), tr("Profile registration complete"));
								}
							}
						}

						profile = p;					
					}					
				}
			}
		}
		else{
			IMUCostumeProfileEditionWizard ew(it->second, tr("New profile"), this);

			auto res = ew.exec();

			if (res != QDialog::Accepted){
				return;
			}

			profile = ew.costumeProfile();

			res = QMessageBox::question(this, tr("Save new profile"), tr("Would You like to save new profile?"));

			if (res == QMessageBox::Yes){
				bool ok = true;
				QString pName;
				while (ok == true){
					pName = QInputDialog::getText(this, tr("Profile name"), tr("Input new profile name"));
					if (pName.isEmpty() == true){
						ok = (QMessageBox::question(this, tr("Retry to save profile"), tr("An empty profile name was given - would you like to retry to save profile with proper name?")) == QMessageBox::Yes);
					}
					else if (profiles.find(pName.toStdString()) != profiles.end()){
						QMessageBox::information(this, tr("Profile name"), tr("Profile name must be unique and non empty - please try again"));
					}
					else{
						ok = false;
					}
				}

				if (pName.isEmpty() == false){
					profile.name = pName.toStdString();
					ds->registerCostumeProfile(profile);
					QMessageBox::information(this, tr("Profile registered"), tr("Profile registration complete"));
				}
			}
		}

		PLUGIN_LOG_DEBUG("Profile creation done");

		auto profileInstance = IMU::CostumeProfileInstance::create(profile);

		PLUGIN_LOG_DEBUG("Profile instantiation done");

		auto cw = IMU::IMUCostumeProfileConfigurationWizard::create(profileInstance);
		if (cw != nullptr){
			auto res = cw->exec();
			if (res != QDialog::Accepted){
				return;
			}

			PLUGIN_LOG_DEBUG("Profile configuration done");
		}

		profileInstance.calibrationAlgorithm->initialize(profileInstance.skeleton, profileInstance.sensorsMapping, profileInstance.sensorsAdjustments);

		PLUGIN_LOG_DEBUG("Calibration initialized");

		auto canOpenStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::RawDataStream::value_type, IMU::CANopenFramesStream::value_type, IMU::RawToCANopenExtractor>>(cd.rawDataStream, IMU::RawToCANopenExtractor());

		PLUGIN_LOG_DEBUG("canOpenStream created");

		unsigned int initializeFramesCount = 50;

		for (const auto & sa : profileInstance.sensorsOrientationEstimationAlgorithms)
		{
			initializeFramesCount = std::max(initializeFramesCount, sa.second->approximateEstimationDelay());
		}

		PLUGIN_LOG_DEBUG("Minimum samples for estimation algorithms done");

		auto costumeStream = utils::make_shared<threadingUtils::StreamAdapterT<IMU::CANopenFramesStream::value_type, IMU::CostumeStream::value_type, IMU::CANopenDataExtractor>>(canOpenStream, IMU::CANopenDataExtractor());

		PLUGIN_LOG_DEBUG("costumeStream created");

		//mamy wszystko, jesteœmy po konfiguracji wszystkiego
		//mo¿na dalej konfigurowaæ kostium - inicjalizowaæ filtry, kalibrowaæ i ³adowaæ ca³oœæ do DataManager
		auto extractorAdapter = utils::make_shared<ExtractedCostumeStreamAdapter>(costumeStream, IMU::CostumeIMUExtractor(cd.sensorsConfiguration));

		PLUGIN_LOG_DEBUG("extractorAdapter created");

		CostumeSkeletonMotionHelper csmh(extractorAdapter,
			&profileInstance, initializeFramesCount + profileInstance.calibrationAlgorithm->maxCalibrationSteps(),
			initializeFramesCount, this);

		auto res = csmh.exec();

		if (res == QDialog::Rejected){
			return;
		}

		PLUGIN_LOG_DEBUG("Costume initialization done");

		coreUI::CoreCursorChanger cc;

		try{
			profileInstance.motionEstimationAlgorithm->initialize(profileInstance.skeleton,
				profileInstance.calibrationAlgorithm->sensorsAdjustemnts(), profileInstance.sensorsMapping);

			ds->loadCalibratedCostume(id, profileInstance);
			PLUGIN_LOG_DEBUG("Costume loaded");
		}
		catch (...){
			QMessageBox::critical(this, tr("Failed to load calibrated costume"), tr("Internal error"));
		}		
	}
	else{

	}
}

void IMUCostumeWidget::onUnload()
{
	coreUI::CoreCursorChanger cc;
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

unsigned int IMUCostumeWidget::getSamplingRate()
{
	return QInputDialog::getInt(this, tr("Data sampling rate"), tr("Enter sampling rate in milliseconds [ms]: "), 20, 10, 2550, 1);
}

void IMUCostumeWidget::onSetSamplingRate()
{
	coreUI::CoreCursorChanger cc;
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	try{
		auto desc = ds->costumeDescription(id);
		auto delay = getSamplingRate();
		desc.rawCostume->setSamplingDelay(delay);
	}
	catch (...){

	}
}

void IMUCostumeWidget::onSetSamplingRateAll()
{
	auto delay = getSamplingRate();
	coreUI::CoreCursorChanger cc;

	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			auto desc = ds->costumeDescription(id);
			desc.rawCostume->setSamplingDelay(delay);
		}
		catch (...){

		}
	}
}

unsigned int IMUCostumeWidget::getSensorsConfigurationSamplesCount()
{
	return QInputDialog::getInt(this, tr("Refresh sensors configuration"), tr("Enter number of samples used for detecting sensors configuration: "), 50, 1, 200, 1);
}

void IMUCostumeWidget::onRefreshSensorsConfiguration()
{
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	auto sc = getSensorsConfigurationSamplesCount();
	coreUI::CoreCursorChanger cc;
	try{
		bool res = ds->refreshCostumeSensorsConfiguration(id, sc);
		if (res == true){
			innerCostumeChange(id);
		}
	}
	catch (...){

	}
}

void IMUCostumeWidget::onRefreshSensorsConfigurationAll()
{
	imuCostume::CostumeRawIO::CostumeAddress currentID;
	auto ci = ui->costumesTreeWidget->currentItem();
	if (ci != nullptr){
		if (ci->data(0, Qt::UserRole).canConvert<imuCostume::CostumeRawIO::CostumeAddress>() == true){
			currentID = ci->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		}
	}

	auto sc = getSensorsConfigurationSamplesCount();
	coreUI::CoreCursorChanger cc;
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			bool res = ds->refreshCostumeSensorsConfiguration(id, sc);
			if (res == true && id == currentID){
				innerCostumeChange(id);
			}
		}
		catch (...){

		}
	}
}

void IMUCostumeWidget::onResetCostumeConnectionStatus()
{
	auto id = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();	
	coreUI::CoreCursorChanger cc;
	try{
		ds->resetCostumeStatus(id);		
	}
	catch (...){

	}

	refreshStatus();
}

void IMUCostumeWidget::onResetSensorConnectionStatus()
{
	auto ci = ui->sensorsTree->currentItem();
	auto costumeID = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
	auto sensorID = ci->data(0, Qt::UserRole).value<imuCostume::Costume::SensorID>();
	coreUI::CoreCursorChanger cc;
	try{
		ds->resetSensorStatus(costumeID, sensorID);
		ci->setIcon(1, statusIcon(IMU::IIMUDataSource::UNKNOWN));
		ci->setText(1, statusDescription(IMU::IIMUDataSource::UNKNOWN));
	}
	catch (...){

	}	
}

void IMUCostumeWidget::onResetCostumeConnectionStatusAll()
{
	coreUI::CoreCursorChanger cc;
	for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
	{
		auto id = ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>();
		try{
			ds->resetCostumeStatus(id);			
		}
		catch (...){

		}
	}

	refreshStatus();
}

void IMUCostumeWidget::watchRecordedData()
{
	if (recordOutput->costumesDataBuffer.size() > 0.5 * recordOutput->costumesDataBuffer.maxBufferSize())
	{
		IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT data;
		recordOutput->costumesDataBuffer.data(data);

		IMU::CostumeParser::save(*outputFile, data);
		outputFile->flush();
	}
}

void IMUCostumeWidget::onRecord(const bool record)
{
	if (record == true){

		std::set<IMU::IIMUDataSource::CostumeID> costumes;

		for (unsigned int i = 0; i < ui->costumesTreeWidget->topLevelItemCount(); ++i)
		{
			costumes.insert(ui->costumesTreeWidget->topLevelItem(i)->data(0, Qt::UserRole).value<imuCostume::CostumeRawIO::CostumeAddress>());
		}

		RecordingWizard w(costumes, this);

		auto ret = w.exec();

		if (ret == QWizard::Accepted){

			coreUI::CoreCursorChanger cc;
			outputFile = utils::make_shared<std::ofstream>(w.outputPath().toStdString().c_str());			

			if (outputFile->is_open() == true){
				recordOutput = utils::make_shared<IMU::IIMUDataSource::RecordingOutput>();
				recordOutput->costumesDataBuffer.setMaxBufferSize(10000);
				recordOutput->costumesToRecord = w.costumes();		

				recordTimer.start(500);

				ds->startRecording(recordOutput);
			}
			else{

				//TODO - info ¿e siê nie uda³o jednak pliku stworzyæ do zapisu
				ui->recordPushButton->blockSignals(true);
				ui->recordPushButton->setChecked(!record);
				ui->recordPushButton->blockSignals(false);
			}
		}
		else{
			ui->recordPushButton->blockSignals(true);
			ui->recordPushButton->setChecked(!record);
			ui->recordPushButton->blockSignals(false);
		}

	}
	else{
		coreUI::CoreCursorChanger cc;
		recordTimer.stop();
		ds->stopRecording(recordOutput);

		//koñczymy zapis danych

		IMU::IIMUDataSource::CostumesRecordingDataBuffer::ListT data;
		recordOutput->costumesDataBuffer.data(data);

		IMU::CostumeParser::save(*outputFile, data);

		outputFile->close();
		outputFile.reset();
		recordOutput.reset();
	}
}