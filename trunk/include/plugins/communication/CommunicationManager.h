/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystujaca wzorce projektowe fasada oraz singleton do zarzadzania polaczeniem
z baza danych i przesylania/odbierania plikow na serwer. Obecnie implementacja opiera sie o szyfrowany protokol FTP
i web serwisy wsdl. 
*/

#ifndef _COMMUNICATIONMANAGER_H_
#define _COMMUNICATIONMANAGER_H_

#include <tinyxml.h>
#include <utils/ObserverPattern.h>
#include "Entity.h"
#include "TransportWSDL_FTPS.h"
#include "QueryWSDL.h"

namespace communication
{

	class CommunicationManager : public utils::Observable<CommunicationManager>
	{
	public:
		typedef std::map<int, Session> Sessions;
		typedef std::map<int, Trial> Trials;
		typedef std::map<int, File> Files;

		void setSessions(unsigned int labID);
		const Sessions& getSessions(/*unsigned int labID*/) const;

		void setTrials(unsigned int sessionID);
		const Trials& getTrials(unsigned int sessionID) const;

		void setFiles(unsigned int sessionID, unsigned int trialID);
		const Files& getFiles(unsigned int sessionID, unsigned int trialID) const;

		void setFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID);
		const std::string& getFile(unsigned int sessionID, unsigned int trialID, unsigned int fileID) const;

		void clearSessions();
		void clearTrials(unsigned int sessionID);
		void clearFiles(unsigned int sessionID, unsigned int trialID);

		void saveToXml(const std::string& filename);
		void loadFromXml(const std::string& filename);

		void setTransportManager(ITransportable* transportManager);
		ITransportable* getTransportManager();

		void setQueryManager(IQueryable* queryManager);
		IQueryable* getQueryManager();

		static CommunicationManager* getInstance();
		static void destoryInstance();
	private:
		/**
		Wskaznik na jedyna instancje klasy CommunicationManager.
		*/
		static CommunicationManager* instance;

		Sessions sessions;

		ITransportable* transportManager;
		IQueryable* queryManager;

		CommunicationManager();
		virtual ~CommunicationManager();

		//metody just in case przy bledach parsera przy atrybutach
		//void labSessionsErrorCatcher(std::string& raw_data);
		//void parseSession(const TiXmlElement* element);
	};
}
#endif
