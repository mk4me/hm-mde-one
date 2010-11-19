#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl
const QStringList CommunicationWidget::sessionsHeader = QStringList() << "ID" << "Motion kind ID" << "Date" << "Description";

const QStringList CommunicationWidget::trialHeader = QStringList() << "ID" << "Description" << "Duration";

const QStringList CommunicationWidget::fileHeader = QStringList() << "ID" << "Description" << "Name";

CommunicationWidget::CommunicationWidget(CommunicationService* service) : QWidget(), m_service(service)
{
	//resize(600, height());
	//UI
	m_download_button = new QPushButton(tr("Download selected"));
	m_list_lab_sessions_button = new QPushButton(tr("List sessions"));
	m_options_button = new QPushButton(tr("Options"));
	m_options_button->setToolTip(tr("Options for FTP and WSDL."));

	//m_sessions_table
	m_sessions_table = new QTableWidget();
	setTableView(m_sessions_table, CommunicationWidget::sessionsHeader);
	//trial table
	m_trials_table = new QTableWidget();
	setTableView(m_trials_table, CommunicationWidget::trialHeader);
	//file table
	m_files_table = new QTableWidget();
	setTableView(m_files_table, CommunicationWidget::fileHeader);

	//QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
	//buttonsBox->addButton(m_options_button, QDialogButtonBox::ActionRole);
	
	//layout
	//m_table_tabs
	m_table_tabs = new QTabWidget(this);
	//m_table_tabs->setStyleSheet("QTabBar::tab { margin-left: -14; height: 41px; width: 174px; background-image: url(tab.png); border-bottom: 1px solid #C2C7CB; } QTabBar::tab:selected { background-image: url(m_table_tabs.png); } QTabBar::tab:first { margin-left: 0; }");
//	tabbar->setMask(QPixmap::grabWidget(m_table_tabs->tabBar(), 0, 0, -1, -1));

	QGroupBox* tab1 = new QGroupBox();
	QVBoxLayout* tab1Layout = new QVBoxLayout();
	QDialogButtonBox* sessionButtonBox = new QDialogButtonBox(Qt::Horizontal);	
	sessionButtonBox->addButton(m_list_lab_sessions_button, QDialogButtonBox::ActionRole);
	tab1Layout->addWidget(sessionButtonBox);
	tab1Layout->addWidget(m_sessions_table);
	tab1->setLayout(tab1Layout);
	m_table_tabs->addTab(tab1, tr("Sessions"));

	QGroupBox* tab2 = new QGroupBox();
	QVBoxLayout* tab2Layout = new QVBoxLayout();
	tab2Layout->addWidget(m_trials_table);
	tab2->setLayout(tab2Layout);
	m_table_tabs->addTab(tab2, tr("Trials"));

	QGroupBox* tab3 = new QGroupBox();
	QDialogButtonBox* downloadButtonBox = new QDialogButtonBox(Qt::Horizontal);	
	downloadButtonBox->addButton(m_download_button, QDialogButtonBox::ActionRole);
	QVBoxLayout* tab3Layout = new QVBoxLayout();
	tab3Layout->addWidget(m_files_table);
	tab3Layout->addWidget(downloadButtonBox);
	tab3->setLayout(tab3Layout);
	m_table_tabs->addTab(tab3, tr("Files"));

	//main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	//mainLayout->addWidget(buttonsBox);
	mainLayout->addWidget(m_table_tabs);
	setLayout(mainLayout);

	//dialogs
	m_settings_dialog = new SettingsDialog(this);

	//signals/slots
	connect(m_settings_dialog, SIGNAL(accepted()), this, SLOT(settingsDialogAccepted()));
	connect(m_options_button, SIGNAL(clicked()), this, SLOT(optionsButtonClicked()));
	connect(m_list_lab_sessions_button, SIGNAL(clicked()), this, SLOT(sessionButtonClicked()));
	connect(m_download_button, SIGNAL(clicked()), this, SLOT(downloadButtonClicked()));
	connect(m_sessions_table, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(sessionsTableDoubleClicked(int, int)));
	connect(m_trials_table, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(trialsTableDoubleClicked(int, int)));
	connect(m_files_table, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(filesTableDoubleClicked(int, int)));

	setWindowTitle("QT CommunicationWidget");
}

CommunicationWidget::~CommunicationWidget()
{
}

void CommunicationWidget::sessionButtonClicked()
{
	try
	{
		this->m_service->listLabSessionsWithAttributes(1);
	}
	catch(communication::EDRException& e)
	{
		QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
	}
}

void CommunicationWidget::settingsDialogAccepted()
{
	writeSettings();
}

void CommunicationWidget::writeSettings()
{
	QSettings settings("Marek Daniluk", "CommunicationWidget");
	settings.setValue("ftpLogin", m_settings_dialog->getFtpLogin());
	settings.setValue("ftpPassword", m_settings_dialog->getFtpPassword());
	settings.setValue("ftpUri", m_settings_dialog->getFtpUri());
	settings.setValue("wsdlLogin", m_settings_dialog->getWsdlLogin());
	settings.setValue("wsdlPassword", m_settings_dialog->getWsdlPassword());
	settings.setValue("wsdlBQS", m_settings_dialog->getWsdlBQSDocument());
	settings.setValue("wsdlBUS", m_settings_dialog->getWsdlBUSDocument());
	settings.setValue("wsdlFSS", m_settings_dialog->getWsdlFSSDocument());

	this->m_service->setQueryCredentials(m_settings_dialog->getWsdlLogin().toStdString(), m_settings_dialog->getWsdlPassword().toStdString(), m_settings_dialog->getWsdlBQSDocument().toStdString(), m_settings_dialog->getWsdlBUSDocument().toStdString());

	this->m_service->setTransportFTPCredentials(m_settings_dialog->getFtpLogin().toStdString(), m_settings_dialog->getFtpPassword().toStdString(), m_settings_dialog->getFtpUri().toStdString());
	this->m_service->setTransportWSCredentials(m_settings_dialog->getWsdlLogin().toStdString(), m_settings_dialog->getWsdlPassword().toStdString(), m_settings_dialog->getWsdlFSSDocument().toStdString());
}

void CommunicationWidget::readSettings()
{
	QSettings settings("Marek Daniluk", "CommunicationWidget");
	m_settings_dialog->setFtpLogin(settings.value("ftpLogin").toString());
	m_settings_dialog->setFtpPassword(settings.value("ftpPassword").toString());
	m_settings_dialog->setFtpUri(settings.value("ftpUri").toString());
	m_settings_dialog->setWsdlLogin(settings.value("wsdllogin").toString());
	m_settings_dialog->setWsdlPassword(settings.value("wsdlPassword").toString());
	m_settings_dialog->setWsdlBQSDocument(settings.value("wsdlBQS").toString());
	m_settings_dialog->setWsdlBUSDocument(settings.value("wsdlBUS").toString());
	m_settings_dialog->setWsdlFSSDocument(settings.value("wsdlFSS").toString());
	
	this->m_service->setQueryCredentials(m_settings_dialog->getWsdlLogin().toStdString(), m_settings_dialog->getWsdlPassword().toStdString(), m_settings_dialog->getWsdlBQSDocument().toStdString(), m_settings_dialog->getWsdlBUSDocument().toStdString());

	this->m_service->setTransportFTPCredentials(m_settings_dialog->getFtpLogin().toStdString(), m_settings_dialog->getFtpPassword().toStdString(), m_settings_dialog->getFtpUri().toStdString());
	this->m_service->setTransportWSCredentials(m_settings_dialog->getWsdlLogin().toStdString(), m_settings_dialog->getWsdlPassword().toStdString(), m_settings_dialog->getWsdlFSSDocument().toStdString());
}

void CommunicationWidget::optionsButtonClicked()
{
	this->m_settings_dialog->exec();
}

void CommunicationWidget::setTableView(QTableWidget* table, const QStringList& header)
{
	table->clear();
	table->setColumnCount(header.size());
	table->setRowCount(0);
	table->setHorizontalHeaderLabels(header);
	table->resizeColumnsToContents();
}

void CommunicationWidget::downloadButtonClicked()
{
	if(m_files_table->currentRow() != -1)
	{
		try
		{
			this->m_service->downloadFile(m_files_table->item(m_files_table->currentRow(), 0)->text().toInt());
		}
		catch(communication::EDRException& e)
		{
			QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
		}
	}
	else
	{
		QMessageBox::warning(this, tr("Warning"), tr("Select file to download."));
	}
}

void CommunicationWidget::sessionsTableDoubleClicked(int row, int column)
{
	try
	{
		this->m_service->listSessionTrials(m_sessions_table->item(m_sessions_table->currentRow(), 0)->text().toInt());
		this->m_table_tabs->setCurrentIndex(1);
	}
	catch(communication::EDRException& e)
	{
		QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
	}
}

void CommunicationWidget::trialsTableDoubleClicked(int row, int column)
{
	try
	{
		this->m_service->listTrialFiles(m_trials_table->item(m_trials_table->currentRow(), 0)->text().toInt());
		this->m_table_tabs->setCurrentIndex(2);
	}
	catch(communication::EDRException& e)
	{
		QMessageBox::critical(this, tr("Error"), tr(e.getExceptionMessage().c_str()));
	}
}

void CommunicationWidget::filesTableDoubleClicked(int row, int column)
{
	downloadButtonClicked();
}

void CommunicationWidget::update(const communication::CommunicationManager* subject)
{
	setTableView(m_sessions_table, CommunicationWidget::sessionsHeader);
	for(communication::CommunicationManager::Sessions::const_iterator it = subject->getSessions().begin(); it != subject->getSessions().end(); ++it)
	{
		QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg((*it).id));
		i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i2 = new QTableWidgetItem(tr("%1").arg((*it).motion_kind.c_str()));
		i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg((*it).session_date.toString().c_str()));
		i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i4 = new QTableWidgetItem(tr("%1").arg((*it).session_description.c_str()));
		i4->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		int row = m_sessions_table->rowCount();
		m_sessions_table->insertRow(row);
		m_sessions_table->setItem(row, 0, i1);
		m_sessions_table->setItem(row, 1, i2);
		m_sessions_table->setItem(row, 2, i3);
		m_sessions_table->setItem(row, 3, i4);
	}
	
	setTableView(m_trials_table, CommunicationWidget::trialHeader);
	for(communication::CommunicationManager::Trials::const_iterator it = subject->getTrials().begin(); it != subject->getTrials().end(); ++it)
	{
		QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg((*it).id));
		i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i2 = new QTableWidgetItem(tr("%1").arg((*it).trial_description.c_str()));
		i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg((*it).session_id));
		i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		int row = m_trials_table->rowCount();
		m_trials_table->insertRow(row);
		m_trials_table->setItem(row, 0, i1);
		m_trials_table->setItem(row, 1, i2);
		m_trials_table->setItem(row, 2, i3);
	}

	setTableView(m_files_table, CommunicationWidget::fileHeader);
	for(communication::CommunicationManager::Files::const_iterator it = subject->getFiles().begin(); it != subject->getFiles().end(); ++it)
	{
		QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg((*it).id));
		i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i2 = new QTableWidgetItem(tr("%1").arg((*it).file_description.c_str()));
		i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg((*it).file_name.c_str()));
		i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

		int row = m_files_table->rowCount();
		m_files_table->insertRow(row);
		m_files_table->setItem(row, 0, i1);
		m_files_table->setItem(row, 1, i2);
		m_files_table->setItem(row, 2, i3);
	}
}
