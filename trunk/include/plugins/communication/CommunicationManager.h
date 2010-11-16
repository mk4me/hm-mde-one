/**
@author Marek Daniluk
@brief Klasa CommunicationManager wykorzystujaca wzorce projektowe fasada oraz singleton do zarzadzania polaczeniem
z baza danych i przesylania/odbierania plikow na serwer. Obecnie implementacja opiera sie o szyfrowany protokol FTP
i web serwisy wsdl. 
*/

#ifndef _COMMUNICATIONMANAGER_H_
#define _COMMUNICATIONMANAGER_H_

#include "TransportWSDL_FTPS.h"
#include "QueryWSDL.h"

namespace communication {

	class CommunicationManager {
	private:
		/**
		Wskaznik na jedyna instancje klasy CommunicationManager.
		*/
		static CommunicationManager* pInstance;
		/**
		Wskaznik na obiekt odpowiedzialny za transport plikow.
		*/
		ITransportable* transportManager;
		/**
		Wskaznik na obiekt odpowiedzialny za odpytywanie bazy danych.
		*/
		IQueryable* queryManager;
	public:
		/**
		Pusty konstruktor klasy CommunicationManager.
		*/
		CommunicationManager() {}
		/**
		Pusty wirtualny destruktor klasy CommunicationManager.
		*/
		virtual ~CommunicationManager() {}
		/**
		Metoda statyczna zwracajaca instancje klasy CommunicationManager. Ta metoda pozwala nam na 
		zarzadzanie polaczeniem z baza danych. Jest to jedyny sposob na otrzymanie instancji klasy,
		gdyz wykorzystano przy niej wzorzec projektowy singleton.
		@return statyczny wskaznik na obiekt typu CommunicationManager
		*/
		static CommunicationManager* getInstance();
		/**
		Metoda statyczna zwracajaca instancje klasy CommunicationManager pierwszy raz.
		@return statyczny wskaznik na obiekt typu CommunicationManager
		*/
		static CommunicationManager* createInstanceFirstTime();
		/**
		Metoda zwalniajaca pamiec po klasie CommunicationManager. Nalezy wywolac za kazdym razem gdy
		konczymy prace z instancja klasy. Jest to jedyny sposob na zwolnienie zasobow z powodu wykorzystania
		singletona.
		*/
		void destoryInstance();
		/**
		Metoda inicjalizujaca wszystkie obiekty powiazane z CommunicationManager.
		@return true w przypadku powodzenia i false gdy wystapia bledy.
		*/
		bool init();
		/**
		Metoda ustalajaca managera transportu plikow
		@param transportManager wskaznik na obiekt wykonujacy zadania transportu plikow
		*/
		void setTransportManager(ITransportable* transportManager);
		/**
		Metoda ustalajaca managera odpytywania bazy danych
		@param queryManager wskaznik na obiekt wykonujacy operacje na bazie danych
		*/
		void setQueryManager(IQueryable* queryManager);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrole bazy danych.
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param sessionID id sesji ktora wczesniej zostala juz umieszczona w bazie danych
		@param path sciezka do katalogu z plikami do wgrania na serwer
		@param description
		*/
		int storeSessionFiles(int sessionID, const std::string& path, const std::string& description);
		/**
		Realizuje wprowadzenie pojedynczego pliku przez performera pod kontrole bazy danych.
		@param performerID id performera
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param performerID id performera
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		void storePerformerFiles(int performerID, const std::string& path);
		/**
		Realizuje wprowadzenie pojedynczego pliku sesji pod kontrole bazy danych.
		@param trialID id trial
		@param path wzgledna sciezka do pliku na dysku serwera w stosunku do korzenia obszaru Uslugi Transportu Plikow
		@param description opis pliku
		@param filename nazwa pliku
		@return id pliku nadany w ramach tabeli "Plik" w bazie danych
		*/
		int storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param trialID id trial
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		void storeTrialFiles(int trialID, const std::string& path);
		/**
		Realizuje wprowadzenie plikow pod kontrole bazy danych.
		@param fileID
		@param path sciezka do katalogu z plikami do wgrania na serwer
		*/
		const std::string downloadFile(int fileID, const std::string& path);
		/**
		*/
		Performer* createPerformer(const std::string& name = "", const std::string& surname = "");
		/**
		*/
		Session* createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs);
		/**
		*/
		Trial* createTrial(int sessionID, const std::string& trialDescription, int trialDuration);
		/**
		*/
		Segment* defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime);
		/**
		*/
		int setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update);
		/**
		*/
		int setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update);
		/**
		*/
		int setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update);
		/**
		*/
		int setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update);
		/**
		*/
		int setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update);
		/**
		*/
		bool assignSessionToGroup(int sessionID, int groupID);
		/**
		*/
		const Performer* getPerformerById(int performerID);
		/**
		*/
		const Session* getSessionById(int sessionID);
		/**
		*/
		const Segment* getSegmentById(int segmentID);
		/**
		*/
		const Trial* getTrialById(int trialID);
		/**
		*/
		const std::vector<Session>* listPerformerSessions(int performerID);
		/**
		*/
		const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID);
		/**
		*/
		const std::vector<Performer>* listPerformers();
		/**
		*/
		const std::vector<Performer>* listPerformersWithAttributes();
		/**
		*/
		const std::vector<Trial>* listSessionTrials(int sessionID);
		/**
		*/
		const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID);
		/**
		*/
		const std::vector<Segment>* listTrialSegments(int trialID);
		/**
		*/
		const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID);
		/**
		*/
		const std::vector<Performer>* listLabPerformersWithAttributes(int labID);
		/**
		*/
		const std::vector<Session>* listLabSessionsWithAttributes(int labID);
		/**
		*/
		const std::vector<File>* listSessionFiles(int sessionID);
		/**
		*/
		const std::vector<File>* listFiles(int ID, const std::string& subjectType);
		/**
		*/
		const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType);
		/**
		*/
		const std::vector<SessionGroup>* listSessionGroupsDefined();
		/**
		*/
		const std::vector<MotionKind>* listMotionKindsDefined();
		/**
		*/
		void performQuery(const std::string& query);
	};
}
#endif
