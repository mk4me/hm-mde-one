#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/StorageConfigurationWidget.h>
#include "ui_StorageConfigurationWidget.h"
#include <QtGui/QFileDialog>

StorageConfigurationWidget::StorageConfigurationWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::StorageConfigurationWidget)
{
	ui->setupUi(this);

	connect(ui->storagePathLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(loginChanged(QString)));
	connect(ui->storagePasswordLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(passwordChanged(QString)));

	//TODO
	//dodac walidacje storage i has³a
}

StorageConfigurationWidget::~StorageConfigurationWidget()
{

}

const QString StorageConfigurationWidget::storageFile() const
{
	return ui->storagePathLineEdit->text();
}

const QString StorageConfigurationWidget::password() const
{
	return ui->storagePasswordLineEdit->text();
}

void StorageConfigurationWidget::setStorageFile(const QString & storageFile)
{
	ui->storagePathLineEdit->setText(storageFile);
}

void StorageConfigurationWidget::setPassword(const QString & password)
{
	ui->storagePasswordLineEdit->setText(password);
}

void StorageConfigurationWidget::resetStorageFile()
{
	setStorageFile(QString());
}

void StorageConfigurationWidget::resetPassword()
{
	setPassword(QString());
}


void StorageConfigurationWidget::openStorage()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Open storage"), storageFile().isEmpty() == false ? storageFile() : "", tr("Storage File (*.dat)"));

	setStorageFile(sf);
}
