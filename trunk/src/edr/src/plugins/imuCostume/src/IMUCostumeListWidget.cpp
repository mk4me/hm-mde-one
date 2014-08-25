#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtWidgets/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <OpenThreads/Thread>
#include <corelib/Filesystem.h>
#include "IMUCostumeCalibrationWizard.h"

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui_IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);

	ui->connectPushButton->setChecked(ds->connected());
	ui->connectPushButton->setText(ds->connected() == true ? tr("Disconnect") : tr("Connect"));

#ifndef _DEBUG
	ui->loadDatFileButton->setVisible(false);
#endif
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::onCalibrate()
{
	coreUI::CoreCursorChanger cc;

	auto ci = ui->costumesTreeWidget->currentItem();

	if(ci != nullptr){

		IMUCostumeCalibrationWizard wizard(this);
		auto ret = wizard.exec();
		if (ret == QDialog::Accepted){
			QMessageBox::information(this, tr("Costume connection problem"), tr("Could not connect costume - please retry and follow costume initialization instructions."));
		}

		return;

		unsigned int idx = ci->data(0, Qt::UserRole).toUInt();

		if(ds->isCalibrated(idx) == true &&
			QMessageBox::information(this, tr("Calibration status"), tr("Selected Costume already calibrated. Do You want to re-calibrate it?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::No){

			return;
		}
			

		bool retry = false;

		do {
			retry = true;

			QMessageBox::information(this, tr("Calibration - first step"), tr("Please stand still with Your hands along the body and press OK"), QMessageBox::Ok);

			ds->callibrateFirstPass(idx);

			unsigned int i = 2000;

			while(--i > 0){
				OpenThreads::Thread::microSleep(100);
				QCoreApplication::processEvents();
			}

			QApplication::beep();

			QMessageBox::information(this, tr("Calibration - second step"), tr("Please stand in 'T' pose and press OK"), QMessageBox::Ok);

			ds->callibrateSecondPass(idx);

			i = 2000;

			while(--i > 0){
				OpenThreads::Thread::microSleep(100);
				QCoreApplication::processEvents();
			}

			ds->finalizeCalibration(idx);

			if(ds->isCalibrated(idx) == false){

				if(QMessageBox::question(this, tr("Costume calibration failed"), tr("Costume calibration failed. Would You like to retry calibration?"),
					QMessageBox::Yes, QMessageBox::No) == QMessageBox::No){

					retry = false;
				}else{

					QMessageBox::information(this, tr("Calibration - summary"), tr("Calibration failed for the given costume."), QMessageBox::Ok);

				}

			}else{

				QMessageBox::information(this, tr("Calibration - summary"), tr("Calibration successful for the given costume."), QMessageBox::Ok);

				retry = false;
			}

		}while(retry == true);

	}else{
		QMessageBox::warning(this, tr("Warning - no costume"), tr("No costume selected for configuration. Chose costume from the list and retry"), QMessageBox::Ok, QMessageBox::NoButton);
	}
}

void IMUCostumeWidget::onConnect()
{
	coreUI::CoreCursorChanger cc;

	ui->costumesTreeWidget->clear();

	bool connected = ds->connected();

	if(connected == true){
		ds->disconnectCostiumes();
	}else{
		ds->connectCostiumes();
	}

	if(connected == false && connected == ds->connected()){

		QMessageBox::information(this, tr("Costume connection problem"), tr("Could not connect costume - please retry and follow costume initialization instructions."));
		return;
	}

	ui->connectPushButton->setText(ds->connected() == true ? tr("Disconnect") : tr("Connect"));

	//teraz wype�niam kostiumami je�eli s�
	auto s = ds->costumesCout();

	if(s > 0){
		for(unsigned int i = 0; i < s; ++i){

			QTreeWidgetItem * lwi = new QTreeWidgetItem;
			lwi->setText(1, QString::fromStdString(ds->costumeConfiguration(i).name));
			lwi->setData(0, Qt::UserRole, i);

			ui->costumesTreeWidget->addTopLevelItem(lwi);
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

void IMUCostumeWidget::onLoad()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	ds->loadCostume(idx);
}

void IMUCostumeWidget::onUnload()
{
	const unsigned int idx = ui->costumesTreeWidget->currentItem()->data(0, Qt::UserRole).toUInt();

	ds->unloadCostume(idx);
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


