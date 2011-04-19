/**
@author Marek Daniluk
@brief Klasa BasicQueriesService dziedzicz�ca po WsdlConnection wykonuj�ca operacje dokumentu BasicQueriesService.wsdl.
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
		Listuje pr�by pomiarowe sesji.
		@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
		@return zbi�r pr�b pomiarowych dla wybranej sesji
		*/
		std::vector<wsdl::Trial> listSessionTrials(int sessionID);
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu kt�rego sesje maj� zosta� wylistowane
		@return zbi�r sesji dla wybranego labu
		*/
		std::vector<wsdl::Session> listLabSessionsWithAttributes(int labID);
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
		@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
		@return zbi�r plik�w dla wybranej encji
		*/
		std::vector<wsdl::File> listFiles(int ID, const std::string& subjectType);
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
		informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
		@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
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
