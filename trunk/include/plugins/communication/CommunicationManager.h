/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystujaca wzorce projektowe fasada oraz singleton do zarzadzania polaczeniem
z baza danych i przesylania/odbierania plikow na serwer. Obecnie implementacja opiera sie o szyfrowany protokol FTP
i web serwisy wsdl. 
*/

#ifndef _COMMUNICATIONMANAGER_H_
#define _COMMUNICATIONMANAGER_H_

#include <tinyxml.h>
#include <OpenThreads/Thread>
#include <utils/ObserverPattern.h>
#include <core/IDataManager.h>
#include <core/IServiceManager.h>
#include <core/LocalTrial.h>
#include "Entity.h"
#include "TransportWSDL_FTPS.h"
#include "QueryWSDL.h"

namespace communication
{

	class CommunicationManager : public utils::Observable<CommunicationManager>, public OpenThreads::Thread
	{
	public:
		enum State
		{
			Ready,
			DownloadingFile,
			DownloadingTrial,
			UpdatingServerTrials,
			Error
		};

		void setTransportManager(ITransportable* transportManager);
		ITransportable* getTransportManager();

		void setQueryManager(IQueryable* queryManager);
		IQueryable* getQueryManager();

		void saveToXml(const std::string& filename);
		void loadFromXml(const std::string& filename);
		void loadLocalTrials();

		void setTrialsDir(const std::string& dir);
		const std::string& getTrialsDir() const;

		int getProgress() const;
		int getFilesToDownloadCount() const {return filesToDownload;};
		int getActualDownloadFileNumber() const {return actualFile;};

		void listSessionContents();
		void downloadTrial(unsigned int trialID);
		void downloadFile(unsigned int fileID);

		const std::vector<Trial>& getServerTrials() const {return serverTrials;};
		const std::vector<LocalTrial>& getLocalTrials() const {return localTrials;};

		bool isUpdated() const {return this->isLastUpdate;};
		const DateTime& getLastUpdateTime() const {return lastUpdate;};

		void setDataManager(IDataManager* dataManager) {this->dataManager = dataManager;loadLocalTrials();};
		IDataManager* getDataManager() {return dataManager;};

		void setServiceManager(IServiceManager* serviceManager) {this->serviceManager = serviceManager;};
		IServiceManager* getServiceManager() {return serviceManager;};

		void loadActualTrial(const std::string& name);

		const State& getState() const {return state;};
		void setState(const State& state) {this->state = state;};
		virtual void run();

		const std::string& getErrorMessage() const {return errorMessage;};

		static CommunicationManager* getInstance();
		static void destoryInstance();

	private:
		/**
		Wskaznik na jedyna instancje klasy CommunicationManager.
		*/
		static CommunicationManager* instance;

		std::string trialsDir;
		State state;
		std::string errorMessage;

		std::vector<LocalTrial> localTrials;
		std::vector<Trial> serverTrials;
		DateTime lastUpdate;
		bool isLastUpdate;
		int entityID;

		int progress;
		int filesToDownload;
		int actualFile;

		IServiceManager* serviceManager;
		IDataManager* dataManager;

		ITransportable* transportManager;
		IQueryable* queryManager;

		CommunicationManager();
		virtual ~CommunicationManager();
	};
}
#endif
