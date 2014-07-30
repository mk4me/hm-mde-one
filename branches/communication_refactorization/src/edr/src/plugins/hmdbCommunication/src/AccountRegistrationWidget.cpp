#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/AccountRegistrationWidget.h>
#include "ui_AccountRegistrationWidget.h"

#include <plugins/hmdbCommunication/IHMDBSource.h>
#include <corelib/PluginCommon.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreCursorChanger.h>
#include <coreui/CorePopup.h>

AccountRegistrationWidget::AccountRegistrationWidget(QWidget * parent, Qt::WindowFlags f) : QWidget(parent, f),
	ui(new Ui::AccountRegistrationWidget)
{
	ui->setupUi(this);

	connect(ui->regRepPassEdit, SIGNAL(textChanged(QString)), this, SIGNAL(passwordRepeatChanged(QString)));
	connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(userNameChanged(QString)));
	connect(ui->surnameEdit, SIGNAL(textChanged(QString)), this, SIGNAL(surnameChanged(QString)));
	connect(ui->emailEdit, SIGNAL(textChanged(QString)), this, SIGNAL(emailChanged(QString)));
}

AccountRegistrationWidget::~AccountRegistrationWidget()
{

}

const QString AccountRegistrationWidget::passwordRepeat() const
{
	return ui->regRepPassEdit->text();
}

const QString AccountRegistrationWidget::userName() const
{
	return ui->nameEdit->text();
}

const QString AccountRegistrationWidget::surname() const
{
	return ui->surnameEdit->text();
}

const QString AccountRegistrationWidget::email() const
{
	return ui->emailEdit->text();
}

void AccountRegistrationWidget::setPasswordRepeat(const QString & passwordRepeat)
{
	ui->regRepPassEdit->setText(passwordRepeat);
}

void AccountRegistrationWidget::setUserName(const QString & userName)
{
	ui->nameEdit->setText(userName);
}

void AccountRegistrationWidget::setSurname(const QString & surname)
{
	ui->surnameEdit->setText(surname);
}

void AccountRegistrationWidget::setEmail(const QString & email)
{
	ui->emailEdit->setText(email);
}

void AccountRegistrationWidget::resetPasswordRepeat()
{
	setPasswordRepeat(QString());
}

void AccountRegistrationWidget::resetUserName()
{
	setUserName(QString());
}

void AccountRegistrationWidget::resetSurname()
{
	setSurname(QString());
}

void AccountRegistrationWidget::resetEmail()
{
	setEmail(QString());
}

/*
void AccountRegistrationWidget::registerAccount()
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
				hmdbSource->registerAccount(ui->serviceURLLineEdit->text().toStdString(),
					ui->regLoginEdit->text().toStdString(),
					ui->regPassEdit->text().toStdString(),
					ui->nameEdit->text().toStdString(),
					ui->surnameEdit->text().toStdString(),
					ui->emailEdit->text().toStdString());

				ccc.restore();
				message = tr("Registration successful");
			}
			catch (std::exception & e){
				message = tr("Registration failed with the following error: ") + QString::fromStdString(e.what());
			}
			catch (...){
				message = tr("Registration failed with UNKNOWN error");
			}

			coreUI::CorePopup::showMessage(tr("Registration status"), message);
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