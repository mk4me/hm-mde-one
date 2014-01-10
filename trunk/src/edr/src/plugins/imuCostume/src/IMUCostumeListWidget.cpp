#include "PCH.h"
#include "IMUCostumeListWidget.h"
#include "ui_IMUCostumeListWidget.h"
#include "DataSource.h"
#include <QtGui/QMenu>
#include <coreui/CoreCursorChanger.h>
#include <QtGui/QMessageBox>

IMUCostumeWidget::IMUCostumeWidget(IMU::IMUCostumeDataSource * ds,
	QWidget * parent, const Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui_IMUCostumeListWidget), ds(ds)
{
	ui->setupUi(this);

	ui->connectPushButton->setChecked(ds->connected());
	ui->connectPushButton->setText(ds->connected() == true ? tr("Disconnect") : tr("Connect"));
}

IMUCostumeWidget::~IMUCostumeWidget()
{
	delete ui;
}

void IMUCostumeWidget::onConnect()
{
	coreUI::CoreCursorChanger cc;

	ui->costumesListWidget->clear();

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

	//teraz wype³niam kostiumami je¿eli s¹
	auto s = ds->costumesCout();

	if(s > 0){
		for(unsigned int i = 0; i < s; ++i){

			QListWidgetItem * lwi = new QListWidgetItem;
			lwi->setText(QString::fromStdString(ds->costumeConfiguration(i).name));
			lwi->setData(Qt::UserRole, i);

			ui->costumesListWidget->addItem(lwi);
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

		if(ui->costumesListWidget->currentItem() != nullptr){

			const unsigned int idx = ui->costumesListWidget->currentItem()->data(Qt::UserRole).toUInt();

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
	const unsigned int idx = ui->costumesListWidget->currentItem()->data(Qt::UserRole).toUInt();

	ds->loadCostume(idx);
}

void IMUCostumeWidget::onUnload()
{
	const unsigned int idx = ui->costumesListWidget->currentItem()->data(Qt::UserRole).toUInt();

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