
#include "CommunicationPCH.h"
#include "CommunicationWidget.h"
//http://hmdb.pjwstk.edu.pl/Motion/FileStoremanWS.svc?wsdl
//http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl
const QStringList CommunicationWidget::sessionsHeader = QStringList() << "ID" << "Motion kind ID" << "Date" << "Description";

const QStringList CommunicationWidget::trialHeader = QStringList() << "ID" << "Description" << "Duration";

const QStringList CommunicationWidget::fileHeader = QStringList() << "ID" << "Description" << "Name";

CommunicationWidget::CommunicationWidget() : QWidget()
{
	//resize(600, height());
	//UI
	downloadSelectedButton = new QPushButton(tr("Download selected"));
	sessionButton = new QPushButton(tr("List sessions"));
	optionsButton = new QPushButton(tr("Options"));
	optionsButton->setToolTip(tr("Options for FTP and WSDL."));

	//sessionTable
	sessionTable = new QTableWidget();
	setTableView(sessionTable, CommunicationWidget::sessionsHeader);
	//trial table
	trialTable = new QTableWidget();
	setTableView(trialTable, CommunicationWidget::trialHeader);
	//file table
	fileTable = new QTableWidget();
	setTableView(fileTable, CommunicationWidget::fileHeader);

	QDialogButtonBox* buttonsBox = new QDialogButtonBox(Qt::Horizontal);	
	buttonsBox->addButton(optionsButton, QDialogButtonBox::ActionRole);
	
	//layout
	//tabs
	tabs = new QTabWidget(this);
	//tabs->setStyleSheet("QTabBar::tab { margin-left: -14; height: 41px; width: 174px; background-image: url(tab.png); border-bottom: 1px solid #C2C7CB; } QTabBar::tab:selected { background-image: url(tabs.png); } QTabBar::tab:first { margin-left: 0; }");
//	tabbar->setMask(QPixmap::grabWidget(tabs->tabBar(), 0, 0, -1, -1));

	QGroupBox* tab1 = new QGroupBox();
	QVBoxLayout* tab1Layout = new QVBoxLayout();
	QDialogButtonBox* sessionButtonBox = new QDialogButtonBox(Qt::Horizontal);	
	sessionButtonBox->addButton(sessionButton, QDialogButtonBox::ActionRole);
	tab1Layout->addWidget(sessionButtonBox);
	tab1Layout->addWidget(sessionTable);
	tab1->setLayout(tab1Layout);
	tabs->addTab(tab1, tr("Sessions"));

	QGroupBox* tab2 = new QGroupBox();
	trialLabel = new QLabel(tr(""));
	QVBoxLayout* tab2Layout = new QVBoxLayout();
	tab2Layout->addWidget(trialLabel);
	tab2Layout->addWidget(trialTable);
	tab2->setLayout(tab2Layout);
	tabs->addTab(tab2, tr("Trials"));

	QGroupBox* tab3 = new QGroupBox();
	fileLabel = new QLabel(tr(""));
	QDialogButtonBox* downloadButtonBox = new QDialogButtonBox(Qt::Horizontal);	
	downloadButtonBox->addButton(downloadSelectedButton, QDialogButtonBox::ActionRole);
	QVBoxLayout* tab3Layout = new QVBoxLayout();
	tab3Layout->addWidget(fileLabel);
	tab3Layout->addWidget(fileTable);
	tab3Layout->addWidget(downloadButtonBox);
	tab3->setLayout(tab3Layout);
	tabs->addTab(tab3, tr("Files"));

	//main layout
	QVBoxLayout* mainLayout = new QVBoxLayout();
	mainLayout->addWidget(buttonsBox);
	mainLayout->addWidget(tabs);
	setLayout(mainLayout);

	//dialogs
	settings_dialog = new SettingsDialog(this);

	//signals/slots
	connect(settings_dialog, SIGNAL(accepted()), this, SLOT(setChangedSettings()));
	connect(optionsButton, SIGNAL(clicked()), this, SLOT(options()));
	connect(sessionButton, SIGNAL(clicked()), this, SLOT(sessionButtonClicked()));
	connect(downloadSelectedButton, SIGNAL(clicked()), this, SLOT(downloadSelected()));
	connect(sessionTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(selectSession(int, int)));
	connect(trialTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(selectTrial(int, int)));
	connect(fileTable, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(selectFile(int, int)));

	setWindowTitle("QT CommunicationWidget");
	//settings
	commServ = communication::CommunicationManager::createInstanceFirstTime();
	commServ->init();
	transportManager = new communication::TransportWSDL_FTPS();
	queryManager = new communication::QueryWSDL();
	commServ->setTransportManager(transportManager);
	commServ->setQueryManager(queryManager);
	readSettings();
	setChangedSettings();
	sessions = new std::vector<communication::Session>();
	trials = new std::vector<communication::Trial>();
	files = new std::vector<communication::File>();
}

CommunicationWidget::~CommunicationWidget()
{
	commServ->destoryInstance();
	if(transportManager != NULL)
	{
		delete transportManager;
	}
	if(queryManager != NULL)
	{
		delete queryManager;
	}
	sessions->clear();
	delete sessions;
	trials->clear();
	delete trials;
	files->clear();
	delete files;
}

void CommunicationWidget::sessionButtonClicked()
{
	getSessions();
}

void CommunicationWidget::setChangedSettings()
{
	transportManager->setFTPCredentials(settings_dialog->getFtpUri().toStdString(), settings_dialog->getFtpLogin().toStdString(), settings_dialog->getFtpPassword().toStdString());
	transportManager->setWSCredentials(settings_dialog->getWsdlFSSDocument().toStdString(), settings_dialog->getWsdlLogin().toStdString(), settings_dialog->getWsdlPassword().toStdString());

	queryManager->setWSCredentials(settings_dialog->getWsdlLogin().toStdString(), settings_dialog->getWsdlPassword().toStdString());
	queryManager->setBasicQueriesServiceUri(settings_dialog->getWsdlBQSDocument().toStdString());
	queryManager->setBasicUpdatesServiceUri(settings_dialog->getWsdlBUSDocument().toStdString());
}

void CommunicationWidget::writeSettings()
{
	QSettings settings("Marek Daniluk", "CommunicationWidget");
	settings.setValue("ftpLogin", settings_dialog->getFtpLogin());
	settings.setValue("ftpPassword", settings_dialog->getFtpPassword());
	settings.setValue("ftpUri", settings_dialog->getFtpUri());
	settings.setValue("wsdlLogin", settings_dialog->getWsdlLogin());
	settings.setValue("wsdlPassword", settings_dialog->getWsdlPassword());
	settings.setValue("wsdlBQS", settings_dialog->getWsdlBQSDocument());
	settings.setValue("wsdlBUS", settings_dialog->getWsdlBUSDocument());
	settings.setValue("wsdlFSS", settings_dialog->getWsdlFSSDocument());
}

void CommunicationWidget::readSettings()
{
	QSettings settings("Marek Daniluk", "CommunicationWidget");
	settings_dialog->setFtpLogin(settings.value("ftpLogin").toString());
	settings_dialog->setFtpPassword(settings.value("ftpPassword").toString());
	settings_dialog->setFtpUri(settings.value("ftpUri").toString());
	settings_dialog->setWsdlLogin(settings.value("wsdllogin").toString());
	settings_dialog->setWsdlPassword(settings.value("wsdlPassword").toString());
	settings_dialog->setWsdlBQSDocument(settings.value("wsdlBQS").toString());
	settings_dialog->setWsdlBUSDocument(settings.value("wsdlBUS").toString());
	settings_dialog->setWsdlFSSDocument(settings.value("wsdlFSS").toString());
}

void CommunicationWidget::getSessions()
{
	try
	{
		setTableView(sessionTable, CommunicationWidget::sessionsHeader);
		setTableView(trialTable, CommunicationWidget::trialHeader);
		setTableView(fileTable, CommunicationWidget::fileHeader);

		//listuje sesje laboratorium o id 1 w bazie
		const std::vector<communication::Session>* ss = commServ->listLabSessionsWithAttributes(0); //listLabSessionsWithAttributes(1);
		sessions->clear();
		trials->clear();
		files->clear();
		for(unsigned int i = 0; i < ss->size(); i++)
		{
			sessions->push_back(ss->at(i));
		}
	}
	catch(std::string& exc)
	{
		QMessageBox::warning(this, tr("Error"), tr("Parser exception."));
		//SessionDetailsWithAttributesParser(exc);
	}
	catch(communication::EDRException& exc)
	{
		QMessageBox::warning(this, tr("Error"), tr(exc.getExceptionMessage().c_str()));
		communication::EDRException::log(exc.getExceptionMessage());
	}
	for(unsigned int i = 0; i < sessions->size(); i++)
	{
		sessionTable->insertRow(i);
		QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg(sessions->at(i).getID()));
		i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i2 = new QTableWidgetItem(tr("%1").arg(sessions->at(i).getmotionKindID()));
		i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg(sessions->at(i).getSessionDate().toString().c_str()));
		i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		QTableWidgetItem* i4 = new QTableWidgetItem(tr("%1").arg(sessions->at(i).getSessionDescription().c_str()));
		i4->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		sessionTable->setItem(i, 0, i1);
		sessionTable->setItem(i, 1, i2);
		sessionTable->setItem(i, 2, i3);
		sessionTable->setItem(i, 3, i4);
	}
}
//
//void CommunicationWidget::SessionDetailsWithAttributesParser(const std::string& data)
//{
//	QDomDocument response;
//	qDebug(data.c_str());
//	if(!response.setContent(QString(data.c_str())))
//	{
//		QMessageBox::warning(this, tr("Error"), tr("Cannot parse xml document."));
//	}
//	else
//	{
//		sessions->clear();
//		trials->clear();
//		files->clear();
//		QDomElement root = response.documentElement();
//		QDomNode n = root.firstChild();
//		while(!n.isNull()) {
//			if(!n.toElement().isNull() && n.toElement().tagName() == "SessionDetailsWithAttributes")
//			{
//				parseSession(n.toElement());
//			}
//			if(n.nextSibling().toElement().isNull())
//			{
//				n = n.firstChild();
//			}
//			else
//			{
//				n = n.nextSibling();
//			}
//		}
//	}
//}
//
//void CommunicationWidget::parseSession(const QDomElement &element)
//{
//	communication::Session s;
//	s.setID(element.firstChildElement("SessionID").text().toInt());
//	s.setmotionKindID(element.firstChildElement("MotionKindID").text().toInt());
//	std::string date(element.firstChildElement("SessionDate").text().toStdString());
//	s.setSessionDate(communication::DateTime(date));
//	s.setSessionDescription(element.firstChildElement("SessionDescription").text().toStdString());
//	sessions->push_back(s);
//}

void CommunicationWidget::options()
{
	if(settings_dialog->exec() == 1)
	{
		writeSettings();
	}
	else
	{
		readSettings();
	}
}

void CommunicationWidget::setTableView(QTableWidget* table, const QStringList& header)
{
	table->clear();
	table->setColumnCount(header.size());
	table->setRowCount(0);
	table->setHorizontalHeaderLabels(header);
	table->resizeColumnsToContents();
}

bool CommunicationWidget::validateQuerySettings()
{
	QSettings settings("Marek Daniluk", "CommunicationWidget");

	if(settings.value("wsdllogin").toString().length() > 0 && settings.value("wsdlPassword").toString().length() > 0 && settings.value("wsdlBQS").toString().length() > 0)
	{
		return true;
	}
	QMessageBox::warning(this, "Error", "No specified options in settings for querying.");
	return false;
}

void CommunicationWidget::getSessionTrials()
{
	if(sessionTable->currentRow() != -1)
	{
		this->session_folder = tr("S_id%1").arg(sessionTable->item(sessionTable->currentRow(), 0)->text());
		try
		{
			setTableView(trialTable, CommunicationWidget::trialHeader);
			setTableView(fileTable, CommunicationWidget::fileHeader);
			const std::vector<communication::Trial>* sessionTrials = commServ->listSessionTrials(sessionTable->item(sessionTable->currentRow(), 0)->text().toInt());
			trials->clear();
			files->clear();
			for(unsigned int i = 0; i < sessionTrials->size(); i++)
			{
				trials->push_back(sessionTrials->at(i));
			}
		}
		catch(std::string exc)
		{
			trials->clear();
			files->clear();
			QMessageBox::warning(this, tr("Error"), tr(exc.c_str()));
		}
		catch(communication::EDRException& exc)
		{
			trials->clear();
			files->clear();
			QMessageBox::warning(this, tr("Error"), tr(exc.getExceptionMessage().c_str()));
			communication::EDRException::log(exc.getExceptionMessage());
		}
		trialLabel->setText(tr("Session ID %1").arg(sessionTable->item(sessionTable->currentRow(), 0)->text()));
		for(unsigned int i = 0; i < trials->size(); i++)
		{
			QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg(trials->at(i).getID()));
			i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* i2 = new QTableWidgetItem(QString::fromStdString(trials->at(i).getTrialDescription().c_str()));
			i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg(trials->at(i).getDuration()));
			i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			trialTable->insertRow(i);
			trialTable->setItem(i, 0, i1);
			trialTable->setItem(i, 1, i2);
			trialTable->setItem(i, 2, i3);
		}
	}
	else
	{
		QMessageBox::warning(this, "Error", "Select session for trials.");
	}
}

void CommunicationWidget::getTrialFiles()
{
	if(sessionTable->currentRow() != -1)
	{
		this->trial_folder = tr("T_id%1").arg(trialTable->item(trialTable->currentRow(), 0)->text());
		try
		{
			setTableView(fileTable, CommunicationWidget::fileHeader);
			int id = trialTable->item(trialTable->currentRow(), 0)->text().toInt();
			const std::vector<communication::File>* trialFiles = commServ->listFiles(id, "trial");
			files->clear();
			for(unsigned int i = 0; i < trialFiles->size(); i++)
			{
				files->push_back(trialFiles->at(i));
			}
		}
		catch(std::string exc)
		{
			files->clear();
			QMessageBox::warning(this, tr("Error"), tr(exc.c_str()));
		}
		catch(communication::EDRException& exc)
		{
			files->clear();
			QMessageBox::warning(this, tr("Error"), tr(exc.getExceptionMessage().c_str()));
			communication::EDRException::log(exc.getExceptionMessage());
		}
		fileLabel->setText(tr("Trial ID %1").arg(trialTable->item(trialTable->currentRow(), 0)->text()));
		for(unsigned int i = 0; i < files->size(); i++)
		{
			QTableWidgetItem* i1 = new QTableWidgetItem(tr("%1").arg(files->at(i).getID()));
			i1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* i2 = new QTableWidgetItem(tr("%1").arg(files->at(i).getFileDescription().c_str()));
			i2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			QTableWidgetItem* i3 = new QTableWidgetItem(tr("%1").arg(files->at(i).getFilename().c_str()));
			i3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
			fileTable->insertRow(i);
			fileTable->setItem(i, 0, i1);
			fileTable->setItem(i, 1, i2);
			fileTable->setItem(i, 2, i3);
		}
	}
	else
	{
		QMessageBox::warning(this, "Error", "Select trial for files.");
	}
}

void CommunicationWidget::downloadSelected()
{
	if(fileTable->currentRow() != -1)
	{
		try
		{
			std::string file_name = commServ->downloadFile(fileTable->item(fileTable->currentRow(), 0)->text().toInt(), "");
			qDebug(file_name.c_str());
			if(!QDir(this->session_folder).exists())
			{
				QDir().mkdir(this->session_folder);
			}
			if(!QDir(tr("%1\\%2").arg(this->session_folder).arg(this->trial_folder)).exists())
			{
				QDir(this->session_folder).mkdir(this->trial_folder);
			}
			QFile::copy(tr("%1").arg(file_name.c_str()), tr("%1\\%2\\%3").arg(this->session_folder).arg(this->trial_folder).arg(file_name.c_str()));
			QFile::remove(tr("%1").arg(file_name.c_str()));
		}
		catch(communication::EDRException& exc)
		{
			QMessageBox::warning(this, tr("Error"), tr(exc.getExceptionMessage().c_str()));
			communication::EDRException::log(exc.getExceptionMessage());
		}
	}
	else
	{
		QMessageBox::warning(this, "Error", "Select trial for files.");
	}
}

void CommunicationWidget::selectSession(int row, int column)
{
	tabs->setCurrentIndex(1);
	getSessionTrials();
}

void CommunicationWidget::selectTrial(int row, int column)
{
	tabs->setCurrentIndex(2);
	getTrialFiles();
}

void CommunicationWidget::selectFile(int row, int column)
{
	downloadSelected();
}
