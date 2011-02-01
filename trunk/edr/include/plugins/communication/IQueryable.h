/**
@author Marek Daniluk
@brief Interfejs IQueryable przykrywa uslugi do odpytywania bazy danych.
*/

#ifndef _IQUERYABLE_H_
#define _IQUERYABLE_H_

#include "Entity.h"

namespace communication
{

	class IQueryable
	{
	public:
		/**
		Wirtualny destruktor klasy IQueryable.
		*/
		virtual ~IQueryable() {}
		/**
		Listuje pr�by pomiarowe sesji.
		@param sessionID id sesji kt�rej pr�by pomiarowe maj� zosta� wylistowane
		@return zbi�r pr�b pomiarowych dla wybranej sesji
		*/
		virtual std::map<int, Trial> listSessionTrials(int sessionID) = 0;
		/**
		Listuje sesje dla danego laboratorium.
		@param labID id labu kt�rego sesje maj� zosta� wylistowane
		@return zbi�r sesji dla wybranego labu
		*/
		virtual std::map<int, Session> listLabSessionsWithAttributes(int labID) = 0;
		/**
		Listuje pliki dla encji typu subjectType i id ID.
		@param ID id encji dla kt�rej maj� zosta� wylistowane pliki
		@param subjectType typ encji dla kt�rej maj� zosta� wylistowane pliki
		@return zbi�r plik�w dla wybranej encji
		*/
		virtual std::map<int, File> listFiles(int ID, const std::string& subjectType) = 0;
		/**
		TODO: Listuje encje bazodanowe, sesje, podleg�e sesjom pr�by pomiarowe i
		informacje o plikach poszczeg�lnych sesji i pr�b pomiarowych.
		@return lista pr�b pomiarowych, w przysz�o�ci powinno znale�� si� drzewko zale�no�ci mi�dzy encjami
		*/
		virtual std::vector<Trial> listSessionContents() = 0;

		///**
		//*/
		//virtual Performer* createPerformer(const std::string& name = "", const std::string& surname = "") = 0;
		///**
		//*/
		//virtual Session* createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) = 0;
		///**
		//*/
		//virtual Trial* createTrial(int sessionID, const std::string& trialDescription, int trialDuration) = 0;
		///**
		//*/
		//virtual Segment* defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) = 0;
		///**
		//*/
		//virtual int setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		///**
		//*/
		//virtual int setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		///**
		//*/
		//virtual int setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		///**
		//*/
		//virtual int setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		///**
		//*/
		//virtual int setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		///**
		//*/
		//virtual bool assignSessionToGroup(int sessionID, int groupID) = 0;
		////---------------------------------------------------------------------------------------------------------------------
		////---------------------------------------------------------------------------------------------------------------------
		///**
		//*/
		//virtual const Performer* getPerformerById(int performerID) = 0;
		///**
		//*/
		//virtual const Session* getSessionById(int sessionID) = 0;
		///**
		//*/
		//virtual const Segment* getSegmentById(int segmentID) = 0;
		///**
		//*/
		//virtual const Trial* getTrialById(int trialID) = 0;
		///**
		//*/
		//virtual const std::vector<Session>* listPerformerSessions(int performerID) = 0;
		///**
		//*/
		//virtual const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID) = 0;
		///**
		//*/
		//virtual const std::vector<Performer>* listPerformers() = 0;
		///**
		//*/
		//virtual const std::vector<Performer>* listPerformersWithAttributes() = 0;
		/**
		*/
		//virtual const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID) = 0;
		///**
		//*/
		//virtual const std::vector<Segment>* listTrialSegments(int trialID) = 0;
		///**
		//*/
		//virtual const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID) = 0;
		///**
		//*/
		//virtual const std::vector<Performer>* listLabPerformersWithAttributes(int labID) = 0;
		///**
		//*/
		//virtual const std::vector<File>* listSessionFiles(int sessionID) = 0;
		///**
		//*/
		//virtual const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType) = 0;
		///**
		//*/
		//virtual const std::vector<SessionGroup>* listSessionGroupsDefined() = 0;
		///**
		//*/
		//virtual const std::vector<MotionKind>* listMotionKindsDefined() = 0;
		///**
		//*/
		//virtual void performQuery(const std::string& query) = 0;
		///*
		//void listAttributeGroupsDefined(EntityType type);
		//void listAttributesDefined(const std::string& groupName, EntityType type);
		//void genericQuery();
		//void genericQueryUniform();
		//*/
	};
}
#endif
