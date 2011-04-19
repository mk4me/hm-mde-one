/**
@author Marek Daniluk
@brief Klasa BasicQueriesService dziedzicz¹ca po WsdlConnection wykonuj¹ca operacje dokumentu BasicQueriesService.wsdl.
*/

#ifndef HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__
#define HEADER_GUARD_COMMUNICATION_BASICQUERIESSERVICE_H__

#include <plugins/communication/WsdlConnection.h>
#include <plugins/communication/Entity.h>

namespace communication
{
	class BasicQueriesService : public WsdlConnection
	{
    public:
		/**
		Konstruktor klasy BasicQueriesService.
		*/
		BasicQueriesService();
		/**
		Wirtualny destruktor klasy BasicQueriesService.
		*/
		virtual ~BasicQueriesService();
		/**
		Listuje próby pomiarowe sesji.
		@param sessionID id sesji której próby pomiarowe maj¹ zostaæ wylistowane
		@return zbiór prób pomiarowych dla wybranej sesji
		*/
		std::vector<wsdl::Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu którego sesje maj¹ zostaæ wylistowane
		@return zbiór sesji dla wybranego labu
		*/
		std::vector<wsdl::Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla której maj¹ zostaæ wylistowane pliki
		@param subjectType typ encji dla której maj¹ zostaæ wylistowane pliki
		@return zbiór plików dla wybranej encji
		*/
		std::vector<wsdl::File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg³e sesjom próby pomiarowe i
		informacje o plikach poszczególnych sesji i prób pomiarowych.
		@return lista prób pomiarowych, w przysz³oœci powinno znaleŸæ siê drzewko zale¿noœci miêdzy encjami
		*/
		std::vector<wsdl::Trial> listSessionContents();

		//const Performer* getPerformerById(int performerID);

		//const Session* getSessionById(int sessionID);

		//const Segment* getSegmentById(int segmentID);

		//const Trial* getTrialById(int trialID);

		//const std::vector<Session>* listPerformerSessions(int performerID);

		//const std::vector<Session>* listPerformerSessionsXML(int performerID);

		//const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID);

		//const std::vector<Performer>* listPerformers();

		//const std::vector<Performer>* listPerformersWithAttributes();

		//const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID);

		//const std::vector<Segment>* listTrialSegments(int strialID);

		//const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID);

		//const std::vector<Performer>* listLabPerformersWithAttributes(int labID);

		//const std::vector<File>* listSessionFiles(int sessionID);

		//const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType);

		//const std::vector<SessionGroup>* listSessionGroupsDefined();

		//const std::vector<MotionKind>* listMotionKindsDefined();

		//void performQuery(const std::string& query);



		//void listAttributeGroupsDefined(EntityType type);
		//void listAttributesDefined(const std::string& groupName, EntityType type);

		//void genericQuery();
		//void genericQueryUniform();

	};
}
#endif
