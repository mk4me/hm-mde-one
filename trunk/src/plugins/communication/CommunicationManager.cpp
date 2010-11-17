/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationManager.h>

using namespace communication;

CommunicationManager* CommunicationManager::m_instance = NULL;

CommunicationManager* CommunicationManager::getInstance()
{
	if(m_instance == NULL) {
		m_instance = new CommunicationManager();
	}
	return m_instance;
}

void CommunicationManager::destoryInstance()
{
}

CommunicationManager::CommunicationManager()
{
	this->m_transport_manager = NULL;
	this->m_query_manager = NULL;
}

CommunicationManager::~CommunicationManager()
{
	if(m_instance) {
		delete m_instance;
	}
	m_instance = NULL;
}

void CommunicationManager::setSessions(unsigned int lab_id)
{
	this->clearSessions(lab_id);
	try
	{
	std::cout << "start\n";
		this->m_sessions = this->m_query_manager->listLabSessionsWithAttributes(lab_id);
	}
	catch(std::string& e)
	{
	std::cout << "ojoj\n";
		this->labSessionsErrorCatcher(e);
	}
	std::cout << "bezblednie\n";
	notify();
}

const CommunicationManager::Sessions& CommunicationManager::getSessions(/*unsigned int lab_id*/) const
{
	return this->m_sessions;
}

void CommunicationManager::setTrials(unsigned int session_id)
{
	this->clearTrials(session_id);
	this->m_trials = this->m_query_manager->listSessionTrials(session_id);
	notify();
}
const CommunicationManager::Trials& CommunicationManager::getTrials(/*unsigned int session_id*/) const
{
	return this->m_trials;
}

void CommunicationManager::setFiles(unsigned int trial_id)
{
	this->clearFiles(trial_id);
	this->m_files = this->m_query_manager->listFiles(trial_id, "trial");
	notify();
}

const CommunicationManager::Files& CommunicationManager::getFiles(/*unsigned int trial_id*/) const
{
	return this->m_files;
}

void CommunicationManager::setFile(unsigned int file_id)
{
	this->m_file_path = this->m_transport_manager->downloadFile(file_id, "");
	notify();
}

const std::string& CommunicationManager::getFile(/*unsigned int file_id*/) const
{
	return this->m_file_path;
}

void CommunicationManager::clearSessions(unsigned int lab_id)
{
	this->m_sessions.clear();
	this->m_trials.clear();
	this->m_files.clear();
	notify();
}

void CommunicationManager::clearTrials(unsigned int session_id)
{
	this->m_trials.clear();
	this->m_files.clear();
	notify();
}

void CommunicationManager::clearFiles(unsigned int trial_id)
{
	this->m_files.clear();
	notify();
}

void CommunicationManager::setTransportManager(ITransportable* transport_manager)
{
	this->m_transport_manager = transport_manager;
	notify();
}

void CommunicationManager::setQueryManager(IQueryable* query_manager)
{
	this->m_query_manager = query_manager;
	notify();
}

ITransportable* CommunicationManager::getTransportManager()
{
	return this->m_transport_manager;
}

IQueryable* CommunicationManager::getQueryManager()
{
	return this->m_query_manager;
}

void CommunicationManager::labSessionsErrorCatcher(std::string& raw_data)
{
	std::string filename = "response.xml";
	std::ofstream tempfile(filename.c_str(), std::ios::out);
	tempfile << raw_data;

	//QDomDocument response;
	TiXmlDocument document(filename);

	//if(!response.setContent(QString(data.c_str())))
	//{
	//	QMessageBox::warning(this, tr("Error"), tr("Cannot parse xml document."));
	//}
	if(!document.LoadFile())
	{
		throw std::ios_base::failure("Failed to load response temporary file.");
	}
	
	//QDomElement root = response.documentElement();
	//QDomNode n = root.firstChild();
	TiXmlElement* root = document.RootElement();
	TiXmlNode* node = root->FirstChild();

	//while(!n.isNull()) {
	//	if(!n.toElement().isNull() && n.toElement().tagName() == "SessionDetailsWithAttributes")
	//	{
	//		parseSession(n.toElement());
	//	}
	//	if(n.nextSibling().toElement().isNull())
	//	{
	//		n = n.firstChild();
	//	}
	//	else
	//	{
	//		n = n.nextSibling();
	//	}
	//}
	while(node != NULL)
	{
		if(node->ToElement() != NULL && node->ToElement()->ValueStr() == "SessionDetailsWithAttributes")
		{
			std::cout << "YAY\n";
		}
		if(node->NextSibling()->ToElement() != NULL)
		{
			node = node->NextSibling();
		}
		else
		{
			node = node->FirstChild();
		}
	}
}

void CommunicationManager::parseSession(const TiXmlElement* element)
{
	//this->m_sessions.push_back();
 //     s.setID(element.firstChildElement("SessionID").text().toInt());
 //     s.setmotionKindID(element.firstChildElement("MotionKindID").text().toInt());
 //     std::string date(element.firstChildElement("SessionDate").text().toStdString());
 //     s.setSessionDate(communication::DateTime(date));
 //     s.setSessionDescription(element.firstChildElement("SessionDescription").text().toStdString());
 //     sessions->push_back(s);
}

//void CommunicationService::parseSession(const QDomElement &element)
//{
//      communication::Session s;
//      s.setID(element.firstChildElement("SessionID").text().toInt());
//      s.setmotionKindID(element.firstChildElement("MotionKindID").text().toInt());
//      std::string date(element.firstChildElement("SessionDate").text().toStdString());
//      s.setSessionDate(communication::DateTime(date));
//      s.setSessionDescription(element.firstChildElement("SessionDescription").text().toStdString());
//      sessions->push_back(s);
//}
