/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationManager.h>

using namespace communication;

CommunicationManager* CommunicationManager::instance = NULL;

CommunicationManager* CommunicationManager::getInstance()
{
	if(instance == NULL) {
		instance = new CommunicationManager();
	}
	return instance;
}

void CommunicationManager::destoryInstance()
{
}

CommunicationManager::CommunicationManager()
{
	this->transportManager = NULL;
	this->queryManager = NULL;
	this->trialsDir = "data/trials/";
}

CommunicationManager::~CommunicationManager()
{
	if(instance) {
		delete instance;
	}
	instance = NULL;
}

void CommunicationManager::setSessions(unsigned int labID)
{
	this->sessions = this->queryManager->listLabSessionsWithAttributes(labID);
	notify();
}

const CommunicationManager::Sessions& CommunicationManager::getSessions(/*unsigned int labID*/) const
{
	return this->sessions;
}

void CommunicationManager::setTrials(unsigned int sessionID)
{
	this->sessions[sessionID].sessionTrials = this->queryManager->listSessionTrials(sessionID);
	notify();
}

const CommunicationManager::Trials& CommunicationManager::getTrials(unsigned int sessionID) const
{
	Sessions::const_iterator it = this->sessions.find(sessionID);
	if(it != this->sessions.end())
	{
		return (*it).second.sessionTrials;
	}
	else
	{
		throw std::runtime_error("No session with this ID.");
	}
}

void CommunicationManager::setFiles(unsigned int sessionID, unsigned int trialID)
{
	this->sessions[sessionID].sessionTrials[trialID].trialFiles = this->queryManager->listFiles(trialID, "trial");
	notify();
}

const CommunicationManager::Files& CommunicationManager::getFiles(unsigned int sessionID, unsigned int trialID) const
{
	Sessions::const_iterator it = this->sessions.find(sessionID);
	if(it == this->sessions.end())
	{
		throw std::runtime_error("No session with this ID.");
	}
	Trials::const_iterator it2 = (*it).second.sessionTrials.find(trialID);
	if(it2 != (*it).second.sessionTrials.end())
	{
		return (*it2).second.trialFiles;
	}
	else
	{
		throw std::runtime_error("No trial with this ID.");
	}
}

void CommunicationManager::setFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID)
{
	//TODO: sciezka powinna byc pobierana/ustalana przez DataManagera
	this->transportManager->downloadFile(fileID, this->trialsDir);
	notify();
}

const std::string& CommunicationManager::getFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID) const
{
	Sessions::const_iterator it = this->sessions.find(sessionID);
	if(it == this->sessions.end())
	{
		throw std::runtime_error("No session with this ID.");
	}
	Trials::const_iterator it2 = (*it).second.sessionTrials.find(trialID);
	if(it2 == (*it).second.sessionTrials.end())
	{
		throw std::runtime_error("No trial with this ID.");
	}
	Files::const_iterator it3 = (*it2).second.trialFiles.find(fileID);
	if(it3 != (*it2).second.trialFiles.end())
	{
		return (*it3).second.fileName;
	}
	else
	{
		throw std::runtime_error("No file with this ID.");
	}
}

void CommunicationManager::clearSessions()
{
	this->sessions.clear();
	notify();
}

void CommunicationManager::clearTrials(unsigned int sessionID)
{
	Sessions::iterator it = this->sessions.find(sessionID);
	if(it != this->sessions.end())
	{
		(*it).second.sessionTrials.clear();
	}
	notify();
}

void CommunicationManager::clearFiles(unsigned int sessionID, unsigned int trialID)
{
	Sessions::iterator it = this->sessions.find(sessionID);
	if(it != this->sessions.end())
	{
		Trials::iterator itr = this->sessions[sessionID].sessionTrials.find(trialID);
		if(itr != this->sessions[sessionID].sessionTrials.end())
		{
			(*itr).second.trialFiles.clear();
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

	for(Sessions::iterator sessions_iterator = this->sessions.begin(); sessions_iterator != this->sessions.end(); ++sessions_iterator)
	{
		//<Session>
		TiXmlElement* session_element = new TiXmlElement("Session");
		root->LinkEndChild(session_element);
		session_element->SetAttribute("id", (*sessions_iterator).second.id);
		session_element->SetAttribute("labID", (*sessions_iterator).second.labID);
		session_element->SetAttribute("motionKind", (*sessions_iterator).second.motionKind);
		session_element->SetAttribute("sessionDate", (*sessions_iterator).second.sessionDate.toString());
		session_element->SetAttribute("sessionDescription", (*sessions_iterator).second.sessionDescription);
		session_element->SetAttribute("sessionLabel", (*sessions_iterator).second.sessionLabel);
		session_element->SetAttribute("userID", (*sessions_iterator).second.userID);
		for(Trials::iterator trials_iterator = (*sessions_iterator).second.sessionTrials.begin(); trials_iterator != (*sessions_iterator).second.sessionTrials.end(); ++trials_iterator)
		{
		//<Trial>
			TiXmlElement* trial_element = new TiXmlElement("Trial");
			session_element->LinkEndChild(trial_element);
			trial_element->SetAttribute("id", (*trials_iterator).second.id);
			trial_element->SetAttribute("sessionID", (*trials_iterator).second.sessionID);
			trial_element->SetAttribute("trialDescription", (*trials_iterator).second.trialDescription);
			for(Files::iterator files_iterator = (*trials_iterator).second.trialFiles.begin(); files_iterator != (*trials_iterator).second.trialFiles.end(); ++files_iterator)
			{
				//<File>
				TiXmlElement* file_element = new TiXmlElement("File");
				trial_element->LinkEndChild(file_element);
				file_element->SetAttribute("id", (*files_iterator).second.id);
				file_element->SetAttribute("fileName", (*files_iterator).second.fileName);
				file_element->SetAttribute("fileDescription", (*files_iterator).second.fileDescription);
				file_element->SetAttribute("fileAttributeName", (*files_iterator).second.fileAttributeName);
				file_element->SetAttribute("fileSubdir", (*files_iterator).second.fileSubdir);
				//</File>
			}
			//</Trial>
		}
		//</Session>
	}
	document.SaveFile(filename);
}

//TODO: trzeba sprawdzic poprawnosc wczytywanych danych
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
		throw std::runtime_error("Failed to read file.");
	}
	hParent = TiXmlHandle(session_element);

	this->sessions.clear();
	session_element = hParent.FirstChild("Session").ToElement();
	while(session_element)
	{
		communication::Session session;
		session_element->QueryIntAttribute("id", &session.id);
		session_element->QueryIntAttribute("labID", &session.labID);
		session_element->QueryIntAttribute("userID", &session.userID);
		session_element->QueryStringAttribute("motionKind", &session.motionKind);
		session_element->QueryStringAttribute("sessionDescription", &session.sessionDescription);
		session_element->QueryStringAttribute("sessionLabel", &session.sessionLabel);

		std::string temp_date;
		session_element->QueryStringAttribute("sessionDate", &temp_date);
		try
		{
			session.sessionDate.setDate(temp_date);
		}
		catch(std::exception& e)
		{
			std::cout << e.what() << ": !Blad wczytania daty z pliku Communication\n";
		}

		this->sessions[session.id] = session;
		
		TiXmlElement* trial_element = session_element->FirstChildElement("Trial");
		while(trial_element)
		{
			communication::Trial trial;
			trial_element->QueryIntAttribute("id", &trial.id);
			trial_element->QueryIntAttribute("sessionID", &trial.sessionID);
			trial_element->QueryStringAttribute("trialDescription", &trial.trialDescription);
			this->sessions[session.id].sessionTrials[trial.id] = trial;

			TiXmlElement* file_element = trial_element->FirstChildElement("File");
			while(file_element)
			{
				communication::File file;
				file_element->QueryIntAttribute("id", &file.id);
				file_element->QueryStringAttribute("fileName", &file.fileName);
				file_element->QueryStringAttribute("fileDescription", &file.fileDescription);
				file_element->QueryStringAttribute("fileAttributeName", &file.fileAttributeName);
				file_element->QueryStringAttribute("fileSubdir", &file.fileSubdir);

				this->sessions[session.id].sessionTrials[trial.id].trialFiles[file.id] = file;
				file_element = file_element->NextSiblingElement();
			}
			trial_element = trial_element->NextSiblingElement();
		}
		session_element = session_element->NextSiblingElement();
	}
	notify();
}

void CommunicationManager::setTransportManager(ITransportable* transportManager)
{
	this->transportManager = transportManager;
	notify();
}

void CommunicationManager::setQueryManager(IQueryable* queryManager)
{
	this->queryManager = queryManager;
	notify();
}

ITransportable* CommunicationManager::getTransportManager()
{
	return this->transportManager;
}

IQueryable* CommunicationManager::getQueryManager()
{
	return this->queryManager;
}

void CommunicationManager::setTrialsDir(const std::string& dir)
{
	this->trialsDir = dir;
}

const std::string& CommunicationManager::getTrialsDir() const
{
	return this->trialsDir;
}
