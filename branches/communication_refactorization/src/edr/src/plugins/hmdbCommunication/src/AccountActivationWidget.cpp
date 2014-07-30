#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/AccountActivationWidget.h>
#include <plugins/hmdbCommunication/IHMDBSource.h>
#include "ui_AccountActivationWidget.h"
#include "ui_ServiceConfigurationWidget.h"
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreCursorChanger.h>
#include <coreui/CorePopup.h>

AccountActivationWidget::AccountActivationWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::AccountActivationWidget)
{	
	ui->setupUi(this);
	connect(ui->loginLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(loginChanged(QString)));
	connect(ui->activationCodeLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(activationCodeChanged(QString)));
}

AccountActivationWidget::~AccountActivationWidget()
{

}

const QString AccountActivationWidget::login() const
{
	return ui->loginLineEdit->text();
}

const QString AccountActivationWidget::activationCode() const
{
	return ui->activationCodeLineEdit->text();
}

void AccountActivationWidget::setLogin(const QString & login)
{
	ui->loginLineEdit->setText(login);
}

void AccountActivationWidget::setActivationCode(const QString & activationCode)
{
	ui->activationCodeLineEdit->setText(activationCode);
}

void AccountActivationWidget::resetLogin()
{
	setLogin(QString());
}

void AccountActivationWidget::resetActivationCode()
{
	setActivationCode(QString());
}

/*
void AccountActivationWidget::activate()
{
	coreUI::CoreCursorChanger ccc;

	if (verify() == false){
		return;
	}

	auto sm = plugin::getSourceManager();
	if (sm != nullptr){
		auto hmdbSource = core::querySource<hmdbCommunication::IHMDBSource>(sm);
		if (hmdbSource != nullptr){			
			QString message;
			try{
				hmdbSource->activateAccount(ui->serviceURLLineEdit->text().toStdString(),
					ui->loginLineEdit->text().toStdString(),
					ui->activationCodeLineEdit->text().toStdString());

				ccc.restore();
				message = tr("Activation successful");
			}
			catch (std::exception & e){
				message = tr("Activation failed with the following error: ") + QString::fromStdString(e.what());				
			}
			catch (...){
				message = tr("Activation failed with UNKNOWN error");				
			}

			coreUI::CorePopup::showMessage(tr("Activation status"), message);
		}
		else{
			PLUGIN_LOG_ERROR("Could not find HMDBSource");
		}
	}
	else{
		PLUGIN_LOG_ERROR("Uninitialized source manager");
	}
}
*/