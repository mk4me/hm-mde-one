#include "CommunicationPCH.h"
#include <plugins/hmdbCommunication/UserLoginWidget.h>
#include "ui_UserLoginWidget.h"

UserLoginWidget::UserLoginWidget(QWidget * parent, Qt::WindowFlags f)
	: QWidget(parent, f), ui(new Ui::UserLoginDataWidget)
{
	ui->setupUi(this);

	connect(ui->loginLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(loginChanged(QString)));
	connect(ui->passwordLineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(passwordChanged(QString)));
}

UserLoginWidget::~UserLoginWidget()
{

}

const QString UserLoginWidget::login() const
{
	return ui->loginLineEdit->text();
}

const QString UserLoginWidget::password() const
{
	return ui->passwordLineEdit->text();
}

void UserLoginWidget::setLogin(const QString & login)
{	
	ui->loginLineEdit->setText(login);	
}

void UserLoginWidget::setPassword(const QString & password)
{
	ui->passwordLineEdit->setText(password);
}

void UserLoginWidget::resetLogin()
{
	setLogin(QString());
}

void UserLoginWidget::resetPassword()
{
	setPassword(QString());
}