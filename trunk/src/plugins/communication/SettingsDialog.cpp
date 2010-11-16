
#include "CommunicationPCH.h"
#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent) {
	//UI
	wsdlLoginLabel = new QLabel(tr("Login"));
	wsdlPasswordLabel = new QLabel(tr("Password"));
	wsdlFSSLabel = new QLabel(tr("FileStoremanServices"));
	wsdlBQSLabel = new QLabel(tr("BasicQueriesServices"));
	wsdlBUSLabel = new QLabel(tr("BasicUpdatesServices"));
	ftpLoginLineEdit = new QLineEdit();
	ftpPasswordLineEdit = new QLineEdit();
	ftpPasswordLineEdit->setEchoMode(QLineEdit::Password);
	ftpUriLineEdit = new QLineEdit();
	wsdlLoginLineEdit = new QLineEdit();
	wsdlPasswordLineEdit = new QLineEdit();
	wsdlPasswordLineEdit->setEchoMode(QLineEdit::Password);
	wsdlFSSLineEdit = new QLineEdit();
	wsdlBQSLineEdit = new QLineEdit();
	wsdlBUSLineEdit = new QLineEdit();
	fssButton = new QPushButton(tr("Browse"));
	bqsButton = new QPushButton(tr("Browse"));
	busButton = new QPushButton(tr("Browse"));
	buttonsBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

	
	//buddies
	wsdlLoginLabel->setBuddy(wsdlLoginLineEdit);
	wsdlPasswordLabel->setBuddy(wsdlPasswordLineEdit);
	wsdlFSSLabel->setBuddy(wsdlFSSLineEdit);
	wsdlBQSLabel->setBuddy(wsdlBQSLineEdit);
	wsdlBUSLabel->setBuddy(wsdlBUSLineEdit);

	//layout
	ftpGroup = new QGroupBox(tr("FTP"));
	wsdlGroup = new QGroupBox(tr("WSDL"));
	ftpLayout = new QFormLayout();
	ftpLayout->addRow(tr("Login:"), ftpLoginLineEdit);
	ftpLayout->addRow(tr("Password:"), ftpPasswordLineEdit);
	ftpLayout->addRow(tr("Adres:"), ftpUriLineEdit);
	ftpGroup->setLayout(ftpLayout);
	QGridLayout* wsdlLayout = new QGridLayout();
	wsdlLayout->addWidget(wsdlLoginLabel, 0, 0);
	wsdlLayout->addWidget(wsdlPasswordLabel, 1, 0);
	wsdlLayout->addWidget(wsdlFSSLabel, 2, 0);
	wsdlLayout->addWidget(wsdlBQSLabel, 3, 0);
	wsdlLayout->addWidget(wsdlBUSLabel, 4, 0);
	wsdlLayout->addWidget(wsdlLoginLineEdit, 0, 1);
	wsdlLayout->addWidget(wsdlPasswordLineEdit, 1, 1);
	wsdlLayout->addWidget(wsdlFSSLineEdit, 2, 1);
	wsdlLayout->addWidget(wsdlBQSLineEdit, 3, 1);
	wsdlLayout->addWidget(wsdlBUSLineEdit, 4, 1);
	wsdlLayout->addWidget(fssButton, 2, 2);
	wsdlLayout->addWidget(bqsButton, 3, 2);
	wsdlLayout->addWidget(busButton, 4, 2);
	wsdlGroup->setLayout(wsdlLayout);
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(ftpGroup);
	mainLayout->addWidget(wsdlGroup);
	mainLayout->addWidget(buttonsBox);
	setLayout(mainLayout);

	//slots
	connect(fssButton, SIGNAL(clicked()), this, SLOT(fssOpenFile()));
	connect(bqsButton, SIGNAL(clicked()), this, SLOT(bqsOpenFile()));
	connect(busButton, SIGNAL(clicked()), this, SLOT(busOpenFile()));
	connect(buttonsBox, SIGNAL(accepted()), this, SLOT(accept()));
	connect(buttonsBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void SettingsDialog::fssOpenFile() {
	wsdlFSSLineEdit->setText(QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("WSDL Files (*.asmx *svc *.wsdl);;All files (*.*)")));
}

void SettingsDialog::bqsOpenFile() {
	wsdlBQSLineEdit->setText(QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("WSDL Files (*.asmx *svc *.wsdl);;All files (*.*)")));
}

void SettingsDialog::busOpenFile(){
	wsdlBUSLineEdit->setText(QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("WSDL Files (*.asmx *svc *.wsdl);;All files (*.*)")));
}

SettingsDialog::~SettingsDialog(void)
{
	delete ftpLoginLineEdit;
	delete ftpPasswordLineEdit;
	delete ftpUriLineEdit;
	delete wsdlLoginLabel;
	delete wsdlPasswordLabel;
	delete wsdlFSSLabel;
	delete wsdlBQSLabel;
	delete wsdlBUSLabel;
	delete wsdlLoginLineEdit;
	delete wsdlPasswordLineEdit;
	delete wsdlFSSLineEdit;
	delete wsdlBQSLineEdit;
	delete wsdlBUSLineEdit;
	delete fssButton;
	delete bqsButton;
	delete busButton;
	delete ftpGroup;
	delete wsdlGroup;
}

const QString SettingsDialog::getFtpLogin() const
{
	return ftpLoginLineEdit->text();
}

const QString SettingsDialog::getFtpPassword() const
{
	return ftpPasswordLineEdit->text();
}

const QString SettingsDialog::getFtpUri() const
{
	return ftpUriLineEdit->text();
}

const QString SettingsDialog::getWsdlLogin() const
{
	return wsdlLoginLineEdit->text();
}

const QString SettingsDialog::getWsdlPassword() const
{
	return wsdlPasswordLineEdit->text();
}

const QString SettingsDialog::getWsdlFSSDocument() const
{
	return wsdlFSSLineEdit->text();
}

const QString SettingsDialog::getWsdlBQSDocument() const
{
	return wsdlBQSLineEdit->text();
}

const QString SettingsDialog::getWsdlBUSDocument() const
{
	return wsdlBUSLineEdit->text();
}

void SettingsDialog::setFtpLogin(const QString& login)
{
	ftpLoginLineEdit->setText(login);
}

void SettingsDialog::setFtpPassword(const QString& password)
{
	ftpPasswordLineEdit->setText(password);
}

void SettingsDialog::setFtpUri(const QString& uri)
{
	ftpUriLineEdit->setText(uri);
}

void SettingsDialog::setWsdlLogin(const QString& login)
{
	wsdlLoginLineEdit->setText(login);
}

void SettingsDialog::setWsdlPassword(const QString& password)
{
	wsdlPasswordLineEdit->setText(password);
}

void SettingsDialog::setWsdlFSSDocument(const QString& uri)
{
	wsdlFSSLineEdit->setText(uri);
}

void SettingsDialog::setWsdlBQSDocument(const QString& uri)
{
	wsdlBQSLineEdit->setText(uri);
}

void SettingsDialog::setWsdlBUSDocument(const QString& uri)
{
	wsdlBUSLineEdit->setText(uri);
}
