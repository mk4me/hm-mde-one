#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>

CommunicationService::CommunicationService() : name("Communication")
{
	this->widget = new CommunicationWidget(this);
	this->transport = new communication::TransportWSDL_FTPS();
	this->query = new communication::QueryWSDL();
	//----------------------------------------------
	//TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
	this->transport->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
	this->transport->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
	this->query->setWSCredentials("applet_user", "aplet4Motion");
	this->query->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
	this->query->setBasicUpdatesServiceUri("");
	//----------------------------------------------

	this->model = communication::CommunicationManager::getInstance();
	this->model->setTransportManager(this->transport);
	this->model->setQueryManager(this->query);
	this->model->attach(this->widget);
	try
	{
		this->load();
	}
	catch(std::runtime_error& e)
	{
		std::cout << e.what() << std::endl;
	}
}

CommunicationService::~CommunicationService()
{
	delete this->transport;
	this->transport = NULL;
	delete this->query;
	this->query = NULL;
	communication::CommunicationManager::destoryInstance();
	this->model = NULL;
}

void CommunicationService::listLabSessionsWithAttributes(unsigned int labID)
{
	this->model->setSessions(labID);
}

void CommunicationService::listSessionTrials(unsigned int sessionID)
{
	this->model->setTrials(sessionID);
}

void CommunicationService::listTrialFiles(unsigned int sessionID, unsigned int trialID)
{
	this->model->setFiles(sessionID, trialID);
}

void CommunicationService::downloadFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID)
{
	this->model->setFile(sessionID, trialID, fileID);
}

const communication::TransportWSDL_FTPS* CommunicationService::getTransportManager() const
{
	return this->transport;
}

const communication::QueryWSDL* CommunicationService::getQueryManager() const
{
	return this->query;
}

void CommunicationService::setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqsUri, const std::string& busUri)
{
	this->query->setWSCredentials(user, password);
	this->query->setBasicQueriesServiceUri(bqsUri);
	this->query->setBasicUpdatesServiceUri(busUri);
}

void CommunicationService::setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
	this->transport->setFTPCredentials(uri, user, password);
}

void CommunicationService::setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
	this->transport->setWSCredentials(uri, user, password);
}

void CommunicationService::run()
{
	std::cout << "thread run\n";
}

void CommunicationService::updateSessionContents()
{
	this->model->clearSessions();
	//update sessions
	this->model->setSessions(1);
	for(std::map<int, communication::Session>::const_iterator sessions_iterator = this->model->getSessions().begin(); sessions_iterator != this->model->getSessions().end(); ++sessions_iterator)
	{
		//update trials
		this->model->setTrials((*sessions_iterator).first);
		for(std::map<int, communication::Trial>::const_iterator trials_iterator = (*sessions_iterator).second.sessionTrials.begin(); trials_iterator != (*sessions_iterator).second.sessionTrials.end(); ++trials_iterator)
		{
			//update files
			this->model->setFiles((*sessions_iterator).first, (*trials_iterator).first);
		}
	}
}

void CommunicationService::load()
{
	this->model->loadFromXml(this->name);
}

void CommunicationService::save()
{
	this->model->saveToXml(this->name);
}
