#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/ConnectionConfigurationWidget.h>
#include "ui_ConnectionConfigurationWidget.h"

ConnectionConfigurationWidget::ConnectionConfigurationWidget(const QString & defaultServiceURL,
	const QString & defaultServiceCAPath,
	const QString & defaultDataURL,
	const QString & defaultDataCAPath,
	const QString & defaultDataUser,
	const QString & defaultDataPassword, QWidget * parent,
	Qt::WindowFlags f) : QWidget(parent, f), ui(new Ui::ConnectionConfigurationWidget),
	defaultServiceURL(defaultServiceURL), defaultServiceCAPath(defaultServiceCAPath),
	defaultDataURL(defaultDataURL), defaultDataCAPath(defaultDataCAPath),
	defaultDataUser(defaultDataUser), defaultDataPassword(defaultDataPassword)
{
	ui->setupUi(this);

	//TODO
	//dodaæ walidacjê urli, usera, has³a, dostêpnoœci plików certyfikatów

	if (defaultServiceURL.isEmpty() == true || defaultDataURL.isEmpty() == true ||
		defaultDataUser.isEmpty() == true || defaultDataPassword.isEmpty() == true){
		ui->resetConnectionPushButton->setEnabled(false);
	}
	else{
		resetConnection();
	}
}

ConnectionConfigurationWidget::~ConnectionConfigurationWidget()
{

}

const bool ConnectionConfigurationWidget::validate() const
{
	bool ret = true;
	//TODO
	return ret;
}

const QString ConnectionConfigurationWidget::serviceURL() const
{
	return ui->servicesURLLineEdit->text();
}

const QString ConnectionConfigurationWidget::serviceCAPath() const
{
	return ui->servisecSSLCertificateLineEdit->text();
}

const QString ConnectionConfigurationWidget::dataFTPURL() const
{	
	return ui->dataURLLineEdit->text();
}

const QString ConnectionConfigurationWidget::dataCAPath() const
{
	return ui->ftpSSLCertificateLineEdit->text();
}

const QString ConnectionConfigurationWidget::dataFTPUser() const
{		
	return ui->dataUserLineEdit->text();
}

const QString ConnectionConfigurationWidget::dataFTPPassword() const
{
	return ui->dataPasswordLineEdit->text();
}

void ConnectionConfigurationWidget::resetConnection()
{
	ui->servicesURLLineEdit->setText(defaultServiceURL);
	ui->servisecSSLCertificateLineEdit->setText(defaultServiceCAPath);
	ui->dataURLLineEdit->setText(defaultDataURL);
	ui->ftpSSLCertificateLineEdit->setText(defaultDataCAPath);
	ui->dataUserLineEdit->setText(defaultDataUser);
	ui->dataPasswordLineEdit->setText(defaultDataPassword);
}