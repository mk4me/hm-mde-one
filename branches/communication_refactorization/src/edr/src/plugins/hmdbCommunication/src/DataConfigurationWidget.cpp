#include "CommunicationPCH.h"
#include "DataConfigurationWidget.h"
#include "ui_DataConfigurationWidget.h"
#include <QtGui/QFileDialog>

DataConfigurationWidget::DataConfigurationWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::DataConfigurationWidget)
{
	ui->setupUi(this);
	connect(ui->dataURLLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(urlChanged(QString)));
	connect(ui->dataSSLCertificateLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(certificatePathChanged(QString)));
}

DataConfigurationWidget::~DataConfigurationWidget()
{

}

const QString DataConfigurationWidget::url() const
{
	return ui->dataURLLineEdit->text();
}

const QString DataConfigurationWidget::certificatePath() const
{
	return ui->dataSSLCertificateLineEdit->text();
}

void DataConfigurationWidget::setUrl(const QString & url)
{
	ui->dataURLLineEdit->setText(url);
}

void DataConfigurationWidget::setCertificatePath(const QString & caPath)
{
	ui->dataSSLCertificateLineEdit->setText(caPath);
}

void DataConfigurationWidget::resetUrl()
{
	setUrl(QString());
}

void DataConfigurationWidget::resetCertificatePath()
{
	setCertificatePath(QString());
}

void DataConfigurationWidget::openCertificateFile()
{
	auto sf = QFileDialog::getOpenFileName(this,
		tr("Open certificate"), certificatePath().isEmpty() == false ? certificatePath() : "", tr("Certificate File (*.crt)"));

	setCertificatePath(sf);
}