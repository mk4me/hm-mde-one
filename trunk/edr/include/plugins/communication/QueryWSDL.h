/**
@author Marek Daniluk
@brief Klasa QueryWSDL implementuje funkcjonalnoœæ interfejsu IQueryable przy pomocy web serwisów.
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
		WskaŸnik na obiekt klasy BasicQueriesService pozwalaj¹cy na korzystanie z web serwisu
		zdefiniowanego do komunikacji z baz¹ danych generuj¹cego zapytania
		*/
		BasicQueriesService* bqs;
		/**
		WskaŸnik na obiekt klasy BasicUpdatesService pozwalaj¹cy na korzystanie z web serwisu
		zdefiniowanego do komunikacji z baz¹ danych pozwalaj¹cego tworzenie nowych encji
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
		Metoda setWSCredentials ustala login i has³o potrzebne do autoryzacji web serwisów.
		@param usr nazwa u¿ytkownika
		@param pswd haslo u¿ytkownika
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
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe maj¹ zostaæ wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		virtual std::map<int, Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje maj¹ zostaæ wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		virtual std::map<int, Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której maj¹ zostaæ wylistowane pliki
		@param subjectType typ encji dla której maj¹ zostaæ wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		virtual std::map<int, File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg³e sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przysz³oœci powinno znaleŸæ siê drzewko zale¿noœci miêdzy encjami
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
