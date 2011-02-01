/**
@author Marek Daniluk
@brief Klasa QueryWSDL implementuje funkcjonalno�� interfejsu IQueryable przy pomocy web serwis�w.
*/

#ifndef _QUERYWSDL_H_
#define _QUERYWSDL_H_

#include "IQueryable.h"
#include "BasicQueriesService.h"
#include "BasicUpdatesService.h"

namespace communication
{

	class QueryWSDL : public IQueryable
	{
	protected:	
		/**
		Wska�nik na obiekt klasy BasicQueriesService pozwalaj�cy na korzystanie z web serwisu
		zdefiniowanego do komunikacji z baz� danych generuj�cego zapytania
		*/
		BasicQueriesService* bqs;
		/**
		Wska�nik na obiekt klasy BasicUpdatesService pozwalaj�cy na korzystanie z web serwisu
		zdefiniowanego do komunikacji z baz� danych pozwalaj�cego tworzenie nowych encji
		*/
		BasicUpdatesService* bus;
	public:
		/**
		Konstruktor klasy QueryWSDL.
		*/
		QueryWSDL();
		/**
		Wirtualny destruktor klasy QueryWSDL.
		*/
		virtual ~QueryWSDL();
		/**
		Metoda setWSCredentials ustala login i has�o potrzebne do autoryzacji web serwis�w.
		@param usr nazwa u�ytkownika
		@param pswd haslo u�ytkownika
		*/
		void setWSCredentials(const std::string& usr, const std::string& pswd);
		/**
		Metoda setBasicQueriesServiceUri ustala uri do web serwisu BasicQueriesService.
		@param uri uri do pliku definicji BasicQueriesService
		*/
		void setBasicQueriesServiceUri(const std::string& uri);
		/**
		Metoda setBasicUpdatesServiceUri ustala uri do web serwisu BasicUpdatesService.
		@param uri uri do pliku definicji BasicUpdatesService
		*/
		void setBasicUpdatesServiceUri(const std::string& uri);
		/**
		Listuje pr�by pomiarowe sesji.
		@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
		@return zbi�r pr�b pomiarowych dla wybranej sesji
		*/
		virtual std::map<int, Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu kt�rego sesje maj� zosta� wylistowane
		@return zbi�r sesji dla wybranego labu
		*/
		virtual std::map<int, Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
		@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
		@return zbi�r plik�w dla wybranej encji
		*/
		virtual std::map<int, File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
		informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
		@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
		*/
		virtual std::vector<Trial> listSessionContents();

		///**
		//*/
		//Performer* createPerformer(const std::string& name = "", const std::string& surname = "");
		///**
		//*/
		//Session* createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs);
		///**
		//*/
		//Trial* createTrial(int sessionID, const std::string& trialDescription, int trialDuration);
		///**
		//*/
		//Segment* defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime);
		///**
		//*/
		//int setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//*/
		//int setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//*/
		//int setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//*/
		//int setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//*/
		//int setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update);
		///**
		//*/
		//bool assignSessionToGroup(int sessionID, int groupID);
		///**
		//*/
		//const Performer* getPerformerById(int performerID);
		///**
		//*/
		//const Session* getSessionById(int sessionID);
		///**
		//*/
		//const Segment* getSegmentById(int segmentID);
		///**
		//*/
		//const Trial* getTrialById(int trialID);
		///**
		//*/
		//const std::vector<Session>* listPerformerSessions(int performerID);
		///**
		//*/
		//const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID);
		///**
		//*/
		//const std::vector<Performer>* listPerformers();
		///**
		//*/
		//const std::vector<Performer>* listPerformersWithAttributes();
		///**
		//*/
		//const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID);
		///**
		//*/
		//const std::vector<Segment>* listTrialSegments(int trialID);
		///**
		//*/
		//const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID);
		///**
		//*/
		//const std::vector<Performer>* listLabPerformersWithAttributes(int labID);
		///**
		//*/
		//const std::vector<File>* listSessionFiles(int sessionID);
		///**
		//*/
		//const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType);
		///**
		//*/
		//const std::vector<SessionGroup>* listSessionGroupsDefined();
		///**
		//*/
		//const std::vector<MotionKind>* listMotionKindsDefined();
		///**
		//*/
		//void performQuery(const std::string& query);
	};
}
#endif
