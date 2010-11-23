
#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationService.h>

CommunicationService::CommunicationService() : m_name("Communication")
{
	this->m_widget = new CommunicationWidget(this);
	this->m_transport = new communication::TransportWSDL_FTPS();
	this->m_query = new communication::QueryWSDL();
	//----------------------------------------------
	//TODO: dane wpisane na sztywno, dodac zapisywanie ustawien
	this->m_transport->setFTPCredentials("ftps://83.230.112.43/", "testUser", "testUser");
	this->m_transport->setWSCredentials("http://83.230.112.43/Motion/FileStoremanWS.svc?wsdl", "applet_user", "aplet4Motion");
	this->m_query->setWSCredentials("applet_user", "aplet4Motion");
	this->m_query->setBasicQueriesServiceUri("http://v21.pjwstk.edu.pl/Motion/res/BasicQueriesWSStandalone.wsdl");
	this->m_query->setBasicUpdatesServiceUri("");
	//----------------------------------------------

	this->m_model = communication::CommunicationManager::getInstance();
	this->m_model->setTransportManager(this->m_transport);
	this->m_model->setQueryManager(this->m_query);
	this->m_model->attach(this->m_widget);
	try
	{
		this->loadFromXml();
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

CommunicationService::~CommunicationService()
{
	delete this->m_transport;
	this->m_transport = NULL;
	delete this->m_query;
	this->m_query = NULL;
	communication::CommunicationManager::destoryInstance();
	this->m_model = NULL;
}

void CommunicationService::listLabSessionsWithAttributes(unsigned int lab_id)
{
	this->m_model->setSessions(lab_id);
}

void CommunicationService::listSessionTrials(unsigned int session_id)
{
	this->m_model->setTrials(session_id);
}

void CommunicationService::listTrialFiles(unsigned int session_id, unsigned int trial_id)
{
	this->m_model->setFiles(session_id, trial_id);
}

void CommunicationService::downloadFile(unsigned int file_id)
{
	this->m_model->setFile(file_id);
}

const communication::TransportWSDL_FTPS* CommunicationService::getTransportManager() const
{
	return this->m_transport;
}

const communication::QueryWSDL* CommunicationService::getQueryManager() const
{
	return this->m_query;
}

void CommunicationService::setQueryCredentials(const std::string& user, const std::string& password, const std::string& bqs_uri, const std::string& bus_uri)
{
	this->m_query->setWSCredentials(user, password);
	this->m_query->setBasicQueriesServiceUri(bqs_uri);
	this->m_query->setBasicUpdatesServiceUri(bus_uri);
}

void CommunicationService::setTransportFTPCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
	this->m_transport->setFTPCredentials(uri, user, password);
}

void CommunicationService::setTransportWSCredentials(const std::string& user, const std::string& password, const std::string& uri)
{
	this->m_transport->setWSCredentials(uri, user, password);
}

void CommunicationService::run()
{
	std::cout << "thread run\n";
}

void CommunicationService::updateSessionContents()
{
	this->m_model->clearSessions();
	//update sessions
	this->m_model->setSessions(1);
	for(std::map<int, communication::Session>::const_iterator sessions_iterator = this->m_model->getSessions().begin(); sessions_iterator != this->m_model->getSessions().end(); ++sessions_iterator)
	{
		//update trials
		this->m_model->setTrials((*sessions_iterator).first);
		for(std::map<int, communication::Trial>::const_iterator trials_iterator = (*sessions_iterator).second.session_trials.begin(); trials_iterator != (*sessions_iterator).second.session_trials.end(); ++trials_iterator)
		{
			//update files
			this->m_model->setFiles((*sessions_iterator).first, (*trials_iterator).first);
		}
	}
}

void CommunicationService::loadFromXml()
{
	this->m_model->loadFromXml("test.xml");
}

void CommunicationService::saveToXml()
{
	this->m_model->saveToXml("test.xml");
}
