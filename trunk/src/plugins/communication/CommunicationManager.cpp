/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <core/Filesystem.h>
#include <plugins/communication/CommunicationManager.h>


using namespace communication;

typedef std::vector<Trial> ServerTrials;
typedef std::vector<LocalTrial> LocalTrials;

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
	this->state = Ready;
}

CommunicationManager::~CommunicationManager()
{
	if(isRunning())
	{
		join();
	}
	if(instance) {
		delete instance;
	}
	instance = NULL;
}

void CommunicationManager::saveToXml(const std::string& filename)
{
	TiXmlDocument document;
	TiXmlDeclaration* declaration = new TiXmlDeclaration("1.0", "", "");
	TiXmlElement* root = new TiXmlElement("Communication");
	document.LinkEndChild(declaration);
	document.LinkEndChild(root);

	//zapis daty
	if(isLastUpdate)
	{
		root->SetAttribute("date", lastUpdate.toString());
	}
	else
	{
		DateTime dt;
		root->SetAttribute("date", dt.toString());
	}

	for(ServerTrials::iterator iterator = this->serverTrials.begin(); iterator != this->serverTrials.end(); ++iterator)
	{
		//<ServerTrial>
		TiXmlElement* strial_element = new TiXmlElement("ServerTrial");
		root->LinkEndChild(strial_element);
		strial_element->SetAttribute("id", (*iterator).id);
		strial_element->SetAttribute("sessionID", (*iterator).sessionID);
		strial_element->SetAttribute("trialDescription", (*iterator).trialDescription);
		for(std::vector<int>::iterator it = (*iterator).trialFiles.begin(); it != (*iterator).trialFiles.end(); ++it)
		{
			//<File>
			TiXmlElement* file_element = new TiXmlElement("File");
			strial_element->LinkEndChild(file_element);
			file_element->SetAttribute("id", (*it));
			//</File>
		}
		//</ServerTrial>
	}
	document.SaveFile(filename);
}

//TODO: trzeba sprawdzic poprawnosc wczytywanych danych
void CommunicationManager::loadFromXml(const std::string& filename)
{
	TiXmlDocument document(filename);
	if(!document.LoadFile())
	{
		LOG_ERROR << ": !Blad wczytania pliku Communication\n";
		return;
	}
	TiXmlHandle hDocument(&document);
	TiXmlElement* strial_element;
	TiXmlHandle hParent(0);

	strial_element = hDocument.FirstChildElement().Element();
	if(!strial_element)
	{
		LOG_ERROR << ": !Blad czytania z pliku Communication\n";
		return;
	}
	hParent = TiXmlHandle(strial_element);

	//data ostatniej aktualizacji danych z serwera
	std::string temp;
	hParent.ToElement()->QueryStringAttribute("date", &temp);
	isLastUpdate = false;
	if(!temp.empty())
	{
		try
		{
			lastUpdate.setDate(temp);
			isLastUpdate = true;
		}
		catch(std::exception& e)
		{
			LOG_ERROR << e.what() << ": !Blad wczytania daty z pliku Communication\n";
		}
	}
	//wczytanie zrzutu danych pomiarowych z serwera
	this->serverTrials.clear();
	strial_element = hParent.FirstChild("ServerTrial").ToElement();
	while(strial_element)
	{
		Trial serverTrial;
		strial_element->QueryIntAttribute("id", &serverTrial.id);
		strial_element->QueryIntAttribute("sessionID", &serverTrial.sessionID);
		strial_element->QueryStringAttribute("trialDescription", &serverTrial.trialDescription);
		TiXmlElement* file_element = strial_element->FirstChildElement("File");
		while(file_element)
		{
			int id;
			file_element->QueryIntAttribute("id", &id);
			serverTrial.trialFiles.push_back(id);
			file_element = file_element->NextSiblingElement();
		}
		serverTrials.push_back(serverTrial);
		strial_element = strial_element->NextSiblingElement("ServerTrial");
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

int CommunicationManager::getProgress() const
{
	return transportManager->getProgress();
}

void CommunicationManager::listSessionContents()
{
	state = UpdatingServerTrials;
	//run();
	start();
}

void CommunicationManager::downloadTrial(unsigned int trialID)
{
	state = DownloadingTrial;
	entityID = trialID;
	//run();
	start();
}

void CommunicationManager::downloadFile(unsigned int fileID)
{
	state = DownloadingFile;
	entityID = fileID;
	//run();
	start();
}

void CommunicationManager::loadLocalTrials()
{
	localTrials.clear();
	dataManager->loadTrials();
	if(dataManager && dataManager->getLocalTrialsCount() > 0)
	{
		for(int i = 0; i < dataManager->getLocalTrialsCount(); i++)
		{
			localTrials.push_back(dataManager->getLocalTrial(i));
		}
	}
	notify();
}

void CommunicationManager::loadActualTrial(const std::string& name)
{
	dataManager->setActualTrial(name);
}

void CommunicationManager::run()
{
	switch(state)
	{
	case DownloadingFile:
		{
			try
			{
				filesToDownload = 1;
				actualFile = 1;
				this->transportManager->downloadFile(entityID, this->trialsDir);
				state = Ready;
			}
			catch(std::runtime_error& e)
			{
				state = Error;
				errorMessage = e.what();
			}
			break;
		}
	case DownloadingTrial:
		{
			try
			{
				for(std::vector<Trial>::iterator it = serverTrials.begin(); it != serverTrials.end(); ++it)
				{
					if(it->id == entityID)
					{
						filesToDownload = it->trialFiles.size();
						actualFile = 0;
						for(std::vector<int>::iterator id = it->trialFiles.begin(); id != it->trialFiles.end(); ++id)
						{
							actualFile++;
							this->transportManager->downloadFile((*id), this->trialsDir);
						}
						break;
					}
				}
				state = Ready;
			}
			catch(std::runtime_error& e)
			{
				state = Error;
				errorMessage = e.what();
			}
			break;
		}
	case UpdatingServerTrials:
		{
			try
			{
				serverTrials.clear();
				serverTrials = queryManager->listSessionContents();
				lastUpdate = DateTime();
				state = Ready;
			}
			catch(std::runtime_error& e)
			{
				state = Error;
				errorMessage = e.what();
			}
			break;
		}
	default:
		{
			return;
		}
	}
	//przeladuj localne zasoby
	loadLocalTrials();
}
