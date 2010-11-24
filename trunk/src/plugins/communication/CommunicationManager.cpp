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
	this->m_sessions = this->m_query_manager->listLabSessionsWithAttributes(lab_id);
	notify();
	//try
	//{
	//	this->m_sessions = this->m_query_manager->listLabSessionsWithAttributes(lab_id);
	//}
	//catch(std::string& e)
	//{
	//	this->labSessionsErrorCatcher(e);
	//}
}

const CommunicationManager::Sessions& CommunicationManager::getSessions(/*unsigned int lab_id*/) const
{
	return this->m_sessions;
}

void CommunicationManager::setTrials(unsigned int session_id)
{
	this->m_sessions[session_id].session_trials = this->m_query_manager->listSessionTrials(session_id);
	notify();
}

const CommunicationManager::Trials& CommunicationManager::getTrials(unsigned int session_id) const
{
	Sessions::const_iterator it = this->m_sessions.find(session_id);
	if(it != this->m_sessions.end())
	{
		return (*it).second.session_trials;
	}
	else
	{
		throw std::exception("No session with this ID.");
	}
}

void CommunicationManager::setFiles(unsigned int session_id, unsigned int trial_id)
{
	this->m_sessions[session_id].session_trials[trial_id].trial_files = this->m_query_manager->listFiles(trial_id, "trial");
	notify();
}

const CommunicationManager::Files& CommunicationManager::getFiles(unsigned int session_id, unsigned int trial_id) const
{
	Sessions::const_iterator it = this->m_sessions.find(session_id);
	if(it == this->m_sessions.end())
	{
		throw std::exception("No session with this ID.");
	}
	Trials::const_iterator it2 = (*it).second.session_trials.find(trial_id);
	if(it2 != (*it).second.session_trials.end())
	{
		return (*it2).second.trial_files;
	}
	else
	{
		throw std::exception("No trial with this ID.");
	}
}

void CommunicationManager::setFile(unsigned int session_id, unsigned int trial_id, unsigned int file_id)
{
	this->m_transport_manager->downloadFile(file_id, "");
	notify();
}

const std::string& CommunicationManager::getFile(unsigned int session_id, unsigned int trial_id, unsigned int file_id) const
{
	Sessions::const_iterator it = this->m_sessions.find(session_id);
	if(it == this->m_sessions.end())
	{
		throw std::exception("No session with this ID.");
	}
	Trials::const_iterator it2 = (*it).second.session_trials.find(trial_id);
	if(it2 == (*it).second.session_trials.end())
	{
		throw std::exception("No trial with this ID.");
	}
	Files::const_iterator it3 = (*it2).second.trial_files.find(file_id);
	if(it3 != (*it2).second.trial_files.end())
	{
		return (*it3).second.file_name;
	}
	else
	{
		throw std::exception("No file with this ID.");
	}
}

void CommunicationManager::clearSessions()
{
	this->m_sessions.clear();
	notify();
}

void CommunicationManager::clearTrials(unsigned int session_id)
{
	Sessions::iterator it = this->m_sessions.find(session_id);
	if(it != this->m_sessions.end())
	{
		(*it).second.session_trials.clear();
	}
	notify();
}

void CommunicationManager::clearFiles(unsigned int session_id, unsigned int trial_id)
{
	Sessions::iterator it = this->m_sessions.find(session_id);
	if(it != this->m_sessions.end())
	{
		Trials::iterator itr = this->m_sessions[session_id].session_trials.find(trial_id);
		if(itr != this->m_sessions[session_id].session_trials.end())
		{
			(*itr).second.trial_files.clear();
		}
	}
	notify();
}

void CommunicationManager::saveToXml(const std::string& filename)
{
	TiXmlDocument document;
	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* root = new TiXmlElement("Communication");
	document.LinkEndChild(declaration);
	document.LinkEndChild(root);

	for(Sessions::iterator sessions_iterator = this->m_sessions.begin(); sessions_iterator != this->m_sessions.end(); ++sessions_iterator)
	{
		//<Session>
		TiXmlElement* session_element = new TiXmlElement("Session");
		root->LinkEndChild(session_element);
		session_element->SetAttribute("id", (*sessions_iterator).second.id);
		session_element->SetAttribute("lab_id", (*sessions_iterator).second.lab_id);
		session_element->SetAttribute("motion_kind", (*sessions_iterator).second.motion_kind);
		session_element->SetAttribute("session_date", (*sessions_iterator).second.session_date.toString());
		session_element->SetAttribute("session_description", (*sessions_iterator).second.session_description);
		session_element->SetAttribute("session_label", (*sessions_iterator).second.session_label);
		session_element->SetAttribute("user_id", (*sessions_iterator).second.user_id);
		for(Trials::iterator trials_iterator = (*sessions_iterator).second.session_trials.begin(); trials_iterator != (*sessions_iterator).second.session_trials.end(); ++trials_iterator)
		{
		//<Trial>
			TiXmlElement* trial_element = new TiXmlElement("Trial");
			session_element->LinkEndChild(trial_element);
			trial_element->SetAttribute("id", (*trials_iterator).second.id);
			trial_element->SetAttribute("session_id", (*trials_iterator).second.session_id);
			trial_element->SetAttribute("trial_description", (*trials_iterator).second.trial_description);
			for(Files::iterator files_iterator = (*trials_iterator).second.trial_files.begin(); files_iterator != (*trials_iterator).second.trial_files.end(); ++files_iterator)
			{
				//<File>
				TiXmlElement* file_element = new TiXmlElement("File");
				trial_element->LinkEndChild(file_element);
				file_element->SetAttribute("id", (*files_iterator).second.id);
				file_element->SetAttribute("file_name", (*files_iterator).second.file_name);
				file_element->SetAttribute("file_description", (*files_iterator).second.file_description);
				file_element->SetAttribute("file_attribute_name", (*files_iterator).second.file_attribute_name);
				file_element->SetAttribute("file_subdir", (*files_iterator).second.file_subdir);
				//</File>
			}
			//</Trial>
		}
		//</Session>
	}
	document.SaveFile(filename);
}

void CommunicationManager::loadFromXml(const std::string& filename)
{
	TiXmlDocument document(filename);
	if(!document.LoadFile())
	{
		throw std::ios_base::failure("Failed to load file.");
	}
	TiXmlHandle hDocument(&document);
	TiXmlElement* session_element;
	TiXmlHandle hParent(0);

	session_element = hDocument.FirstChildElement().Element();
	if(!session_element)
	{
		throw std::exception("Failed to read file.");
	}
	hParent = TiXmlHandle(session_element);

	this->m_sessions.clear();
	session_element = hParent.FirstChild("Session").ToElement();
	while(session_element)
	{
		communication::Session session;
		session_element->QueryIntAttribute("id", &session.id);
		session_element->QueryIntAttribute("lab_id", &session.lab_id);
		session_element->QueryIntAttribute("user_id", &session.user_id);
		session_element->QueryStringAttribute("motion_kind", &session.motion_kind);
		session_element->QueryStringAttribute("session_description", &session.session_description);
		session_element->QueryStringAttribute("session_label", &session.session_label);

		std::string temp_date;
		session_element->QueryStringAttribute("session_date", &temp_date);
		session.session_date.setDate(temp_date);

		this->m_sessions[session.id] = session;
		
		TiXmlElement* trial_element = session_element->FirstChildElement("Trial");
		while(trial_element)
		{
			communication::Trial trial;
			trial_element->QueryIntAttribute("id", &trial.id);
			trial_element->QueryIntAttribute("session_id", &trial.session_id);
			trial_element->QueryStringAttribute("trial_description", &trial.trial_description);
			this->m_sessions[session.id].session_trials[trial.id] = trial;

			TiXmlElement* file_element = trial_element->FirstChildElement("File");
			while(file_element)
			{
				communication::File file;
				file_element->QueryIntAttribute("id", &file.id);
				file_element->QueryStringAttribute("file_name", &file.file_name);
				file_element->QueryStringAttribute("file_description", &file.file_description);
				file_element->QueryStringAttribute("file_attribute_name", &file.file_attribute_name);
				file_element->QueryStringAttribute("file_subdir", &file.file_subdir);

				this->m_sessions[session.id].session_trials[trial.id].trial_files[file.id] = file;
				file_element = file_element->NextSiblingElement();
			}
			trial_element = trial_element->NextSiblingElement();
		}
		session_element = session_element->NextSiblingElement();
	}
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

//void CommunicationManager::labSessionsErrorCatcher(std::string& raw_data)
//{
//	std::string filename = "response.xml";
//	std::ofstream tempfile(filename.c_str(), std::ios::out);
//	tempfile << raw_data;
//
//	//QDomDocument response;
//	TiXmlDocument document(filename);
//
//	//if(!response.setContent(QString(data.c_str())))
//	//{
//	//	QMessageBox::warning(this, tr("Error"), tr("Cannot parse xml document."));
//	//}
//	if(!document.LoadFile())
//	{
//		throw std::ios_base::failure("Failed to load response temporary file.");
//	}
//	
//	//QDomElement root = response.documentElement();
//	//QDomNode n = root.firstChild();
//	TiXmlElement* root = document.RootElement();
//	TiXmlNode* node = root->FirstChild();
//
//	//while(!n.isNull()) {
//	//	if(!n.toElement().isNull() && n.toElement().tagName() == "SessionDetailsWithAttributes")
//	//	{
//	//		parseSession(n.toElement());
//	//	}
//	//	if(n.nextSibling().toElement().isNull())
//	//	{
//	//		n = n.firstChild();
//	//	}
//	//	else
//	//	{
//	//		n = n.nextSibling();
//	//	}
//	//}
//	while(node != NULL)
//	{
//		if(node->ToElement() != NULL && node->ToElement()->ValueStr() == "SessionDetailsWithAttributes")
//		{
//			std::cout << "YAY\n";
//		}
//		if(node->NextSibling()->ToElement() != NULL)
//		{
//			node = node->NextSibling();
//		}
//		else
//		{
//			node = node->FirstChild();
//		}
//	}
//}
//
//void CommunicationManager::parseSession(const TiXmlElement* session_element)
//{
//	//this->m_sessions.push_back();
// //     s.setID(session_element.firstChildElement("SessionID").text().toInt());
// //     s.setmotionKindID(session_element.firstChildElement("MotionKindID").text().toInt());
// //     std::string date(session_element.firstChildElement("SessionDate").text().toStdString());
// //     s.setSessionDate(communication::DateTime(date));
// //     s.setSessionDescription(session_element.firstChildElement("SessionDescription").text().toStdString());
// //     sessions->push_back(s);
//}
//
////void CommunicationService::parseSession(const QDomElement &session_element)
////{
////      communication::Session s;
////      s.setID(session_element.firstChildElement("SessionID").text().toInt());
////      s.setmotionKindID(session_element.firstChildElement("MotionKindID").text().toInt());
////      std::string date(session_element.firstChildElement("SessionDate").text().toStdString());
////      s.setSessionDate(communication::DateTime(date));
////      s.setSessionDescription(session_element.firstChildElement("SessionDescription").text().toStdString());
////      sessions->push_back(s);
////}
