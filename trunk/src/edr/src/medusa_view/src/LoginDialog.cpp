#include "LoginDialog.h"


LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
}

const QString& LoginDialog::getUser()
{
	return loginLineEdit->text();
}

const QString& LoginDialog::getPassword()
{
	return passwordLineEdit->text();
}
