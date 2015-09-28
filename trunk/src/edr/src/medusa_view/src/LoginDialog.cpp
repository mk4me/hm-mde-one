#include "LoginDialog.h"


LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent)
{
	setupUi(this);
	#ifdef _DEBUG 
	loginLineEdit->setText("test-student");
	passwordLineEdit->setText("test-Medusa");
	#endif
}

const QString& LoginDialog::getUser()
{
	return loginLineEdit->text();
}

const QString& LoginDialog::getPassword()
{
	return passwordLineEdit->text();
}
