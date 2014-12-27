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

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);

	ui->costumesTreeWidget->clear();
	for (unsigned int i = 0; i < ds->costumesCout(); ++i){
		auto ci = new QTreeWidgetItem;
		const auto a = ds->costumeAddress(i);
		ci->setText(0, QString::fromStdString(a.ip));
		ci->setData(0, Qt::UserRole, i);
		auto s = ds->costumeStatus(i);
		ci->setText(1, QString("%1").arg(s));
		ui->costumesTreeWidget->addTopLevelItem(ci);
	}

	connect(&refreshTimer, SIGNAL(timeout()), this, SLOT(refreshData()));
	refreshTimer.start(100);
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	refreshTimer.stop();
	delete ui;
}

void IMUCostumeWidget::onCostumeChange(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
	ui->sensorsTree->clear();
	auto idx = ui->costumesTreeWidget->indexOfTopLevelItem(current);

	if (idx < ds->costumesCout()){
		auto cc = ds->costumeConfiguration(idx);

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

		if(cc != ds->costumesLoadedCount()){
			loadAll->setEnabled(true);
			connect(loadAll, SIGNAL(triggered()), this, SLOT(onLoadAll()));
		}else{
			loadAll->setEnabled(false);
		}

		if(ds->costumesLoadedCount() > 0){
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
			
	menu->exec(mapToGlobal(position));
}

void IMUCostumeWidget::onRefresh()
{
	IMUCostumeConfigurationWizard w(utils::shared_ptr<imuCostume::CostumeRawIO>(), 12, 0);

	w.exec();
}

void IMUCostumeWidget::onLoad()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	ds->loadCostume(idx);
	auto data = ds->costumeData(idx);

	if (data.empty() == false){

		CostumeStreams cs;

		for (auto & d : data){
			if (d->data()->isSupported(typeid(threadingUtils::IStreamT<imuCostume::ProtocolSendBufferHelper::Buffer>)) == true){
				cs.rawStream = d->get();
				cs.rawStreamObserver.reset(new threadingUtils::ResetableStreamStatusObserver);
				cs.rawStream->attachObserver(cs.rawStreamObserver);
			}
			else if (d->data()->isSupported(typeid(threadingUtils::IStreamT<imuCostume::Costume::Data>)) == true){
				cs.costumeStream = d->get();
				cs.costumeStreamObserver.reset(new threadingUtils::ResetableStreamStatusObserver);
				cs.costumeStream->attachObserver(cs.costumeStreamObserver);
			}
		}

		auto address = ds->costumeAddress(idx).ip;

		costumeStreams[address] = cs;
	}
}

void IMUCostumeWidget::onUnload()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	auto address = ds->costumeAddress(idx).ip;

	ds->unloadCostume(idx);

	costumeStreams.erase(address);
}

void IMUCostumeWidget::onLoadAll()
{
	ds->loadAllCostumes();
}

void IMUCostumeWidget::onUnloadAll()
{
	ds->unloadAllCostumes();
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

			/*for (unsigned int i = 0; i < buffer.length; ++i){				
				message += QString("%1").arg(*c++, 2, 16, QLatin1Char('0'));
			}*/
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
