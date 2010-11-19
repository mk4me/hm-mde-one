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

namespace communication {

	class CommunicationManager : public utils::Observable<CommunicationManager>
	{
	public:
		typedef std::vector<Session> Sessions;
		typedef std::vector<Trial> Trials;
		typedef std::vector<File> Files;

		void setSessions(unsigned int lab_id);
		const Sessions& getSessions(/*unsigned int lab_id*/) const;

		void setTrials(unsigned int session_id);
		const Trials& getTrials(/*unsigned int session_id*/) const;

		void setFiles(unsigned int trial_id);
		const Files& getFiles(/*unsigned int trial_id*/) const;

		void setFile(unsigned int file_id);
		const std::string& getFile(/*unsigned int file_id*/) const;

		void clearSessions(unsigned int lab_id);

		void clearTrials(unsigned int session_id);

		void clearFiles(unsigned int trial_id);

		void setTransportManager(ITransportable* transport_manager);
		ITransportable* getTransportManager();

		void setQueryManager(IQueryable* query_manager);
		IQueryable* getQueryManager();

		static CommunicationManager* getInstance();
		static void destoryInstance();
	private:
		/**
		Wskaznik na jedyna instancje klasy CommunicationManager.
		*/
		static CommunicationManager* m_instance;

		Sessions m_sessions;
		Trials m_trials;
		Files m_files;
		std::string m_file_path;

		ITransportable* m_transport_manager;
		IQueryable* m_query_manager;

		CommunicationManager();
		virtual ~CommunicationManager();

		//metody just in case przy bledach parsera przy atrybutach
		void labSessionsErrorCatcher(std::string& raw_data);
		void parseSession(const TiXmlElement* element);
	};
}
#endif
