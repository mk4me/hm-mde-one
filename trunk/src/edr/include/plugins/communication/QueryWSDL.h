/**
@author Marek Daniluk
@brief Klasa MotionQueryWSDL implementuje funkcjonalność interfejsu IQueryable przy pomocy web serwisów.
*/

#ifndef HEADER_GUARD_COMMUNICATION_QUERYWSDL_H__
#define HEADER_GUARD_COMMUNICATION_QUERYWSDL_H__

#include <plugins/communication/BasicQueriesService.h>
#include <plugins/communication/BasicUpdatesService.h>

namespace communication
{
	class MotionQueryWSDL
	{
	protected:	
		/**
		Wskaźnik na obiekt klasy MotionBasicQueriesService pozwalający na korzystanie z web serwisu
		zdefiniowanego do komunikacji z bazą danych generującego zapytania
		*/
		core::shared_ptr<MotionBasicQueriesService> bqs;
		/**
		Wskaźnik na obiekt klasy MotionBasicUpdatesService pozwalający na korzystanie z web serwisu
		zdefiniowanego do komunikacji z bazą danych pozwalającego tworzenie nowych encji
		*/
		core::shared_ptr<MotionBasicUpdatesService> bus;
	public:
		/**
		Konstruktor klasy MotionQueryWSDL.
		*/
		MotionQueryWSDL();
		/**
		Wirtualny destruktor klasy MotionQueryWSDL.
		*/
		virtual ~MotionQueryWSDL();
		/**
		Metoda setWSCredentials ustala login i hasło potrzebne do autoryzacji web serwisów.
		@param usr nazwa użytkownika
		@param pswd haslo użytkownika
		*/
		void setWSCredentials(const std::string& usr, const std::string& pswd);
		/**
		Metoda setBasicQueriesServiceUri ustala uri do web serwisu MotionBasicQueriesService.
		@param uri uri do pliku definicji MotionBasicQueriesService
		*/
		void setBasicQueriesServiceUri(const std::string& uri);
		/**
		Metoda setBasicUpdatesServiceUri ustala uri do web serwisu MotionBasicUpdatesService.
		@param uri uri do pliku definicji MotionBasicUpdatesService
		*/
		void setBasicUpdatesServiceUri(const std::string& uri);

        DateTime getLastDBModificationTime();

        DateTime getLastMetadataModificationTime();

		/**
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe mają zostać wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		virtual std::vector<motionWsdl::Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje mają zostać wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		virtual std::vector<motionWsdl::Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której mają zostać wylistowane pliki
		@param subjectType typ encji dla której mają zostać wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		virtual std::vector<motionWsdl::File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podległe sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przyszłości powinno znaleźć się drzewko zależności między encjami
		*/
		virtual std::vector<motionWsdl::Trial> listSessionContents();

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

    class MedicalQueryWSDL
	{
	protected:	
		/**
		Wskaźnik na obiekt klasy MotionBasicQueriesService pozwalający na korzystanie z web serwisu
		zdefiniowanego do komunikacji z bazą danych generującego zapytania
		*/
		core::shared_ptr<MedicalBasicQueriesService> bqs;
		/**
		Wskaźnik na obiekt klasy MotionBasicUpdatesService pozwalający na korzystanie z web serwisu
		zdefiniowanego do komunikacji z bazą danych pozwalającego tworzenie nowych encji
		*/
		core::shared_ptr<MedicalBasicUpdatesService> bus;
	public:
		/**
		Konstruktor klasy MotionQueryWSDL.
		*/
		MedicalQueryWSDL();
		/**
		Wirtualny destruktor klasy MotionQueryWSDL.
		*/
		virtual ~MedicalQueryWSDL();
		/**
		Metoda setWSCredentials ustala login i hasło potrzebne do autoryzacji web serwisów.
		@param usr nazwa użytkownika
		@param pswd haslo użytkownika
		*/
		void setWSCredentials(const std::string& usr, const std::string& pswd);
		/**
		Metoda setBasicQueriesServiceUri ustala uri do web serwisu MotionBasicQueriesService.
		@param uri uri do pliku definicji MotionBasicQueriesService
		*/
		void setBasicQueriesServiceUri(const std::string& uri);
		/**
		Metoda setBasicUpdatesServiceUri ustala uri do web serwisu MotionBasicUpdatesService.
		@param uri uri do pliku definicji MotionBasicUpdatesService
		*/
		void setBasicUpdatesServiceUri(const std::string& uri);

        DateTime getLastDBModificationTime();

        DateTime getLastMetadataModificationTime();
    };
}
#endif //HEADER_GUARD_COMMUNICATION_QUERYWSDL_H__
