#include "ConnectCostumeInputDialog.h"
#include "ui_ConnectCostumeInputDialog.h"
#include <QtGui/QValidator>
#include <QtCore/QRegExp>

ConnectCostumeInputDialog::ConnectCostumeInputDialog(QWidget *parent,
	Qt::WindowFlags f, const QString & ip) : QDialog(parent, f),
	ui(new Ui::ConnectCostumeInputDialog)
{
	ui->setupUi(this);	
	ui->ipLineEdit->setValidator(new QRegExpValidator(
		QRegExp("^([1]?/D/d?|2[0-4]/d|25[0-5])/.([1]?/D/d?|2[0-4]/d|25[0-5])/([1]?/d/d?|2[0-4]/d|25 [0-5])/([1]?/d/d?|2[0-4]/d|25[0-5])$"), this));

	ui->ipLineEdit->setText(ip);
}

ConnectCostumeInputDialog::~ConnectCostumeInputDialog()
{
	delete ui;
}

QString ConnectCostumeInputDialog::ip() const
{
	return ui->ipLineEdit->text();
}

unsigned int ConnectCostumeInputDialog::port() const
{
	return ui->portSpinBox->value();
}