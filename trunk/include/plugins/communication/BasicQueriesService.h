/**
@author Marek Daniluk
@brief Klasa BasicQueriesService dziedziczaca po EDRWSDL wykonujaca operacje dokumentu BasicQueriesService.wsdl.
Klasa powstala na bazie dokumentacji i kodu zrodlowego zespolu bazodanowego.
*/

#ifndef _BASICQUERIESSERVICE_H_
#define _BASICQUERIESSERVICE_H_

#include "WSDL_Wsdlpull.h"
#include "Session.h"
#include "SessionGroup.h"
#include "MotionKind.h"
#include "Segment.h"
#include "Trial.h"
#include "File.h"
#include "Performer.h"

namespace communication {

	class BasicQueriesService : public WSDL_Wsdlpull {
	public:
		/**
		Konstruktor klasy BasicQueriesService.
		*/
		BasicQueriesService();
		/**
		Wirtualny destruktor klasy BasicQueriesService.
		*/
		virtual ~BasicQueriesService();

		const Performer* getPerformerById(int performerID);

		const Session* getSessionById(int sessionID);

		const Segment* getSegmentById(int segmentID);

		const Trial* getTrialById(int trialID);

		const std::vector<Session>* listPerformerSessions(int performerID);

		const std::vector<Session>* listPerformerSessionsXML(int performerID);

		const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID);

		const std::vector<Performer>* listPerformers();

		const std::vector<Performer>* listPerformersWithAttributes();

		std::vector<Trial> listSessionTrials(int sessionID);

		const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID);

		const std::vector<Segment>* listTrialSegments(int strialID);

		const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID);

		const std::vector<Performer>* listLabPerformersWithAttributes(int labID);

		std::vector<Session> listLabSessionsWithAttributes(int labID);

		const std::vector<File>* listSessionFiles(int sessionID);

		std::vector<File> listFiles(int ID, const std::string& subjectType);

		const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType);

		const std::vector<SessionGroup>* listSessionGroupsDefined();

		const std::vector<MotionKind>* listMotionKindsDefined();

		void performQuery(const std::string& query);



		//void listAttributeGroupsDefined(EntityType type);
		//void listAttributesDefined(const std::string& groupName, EntityType type);

		//void genericQuery();
		//void genericQueryUniform();

	};
}
#endif
