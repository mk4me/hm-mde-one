/**
@author Marek Daniluk
@brief Interfejs IQueryable przykrywa uslugi do odpytywania bazy danych.
*/

#ifndef _IQUERYABLE_H_
#define _IQUERYABLE_H_

#include <vector>
#include "Segment.h"
#include "Session.h"
#include "SessionGroup.h"
#include "Performer.h"
#include "MotionKind.h"
#include "Trial.h"
#include "File.h"
#include "DateTime.h"

namespace EDRCM {

	class IQueryable {
	public:
		/**
		Wirtualny destruktor klasy IQueryable.
		*/
		virtual ~IQueryable() {}
		/**
		*/
		virtual Performer* createPerformer(const std::string& name = "", const std::string& surname = "") = 0;
		/**
		*/
		virtual Session* createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) = 0;
		/**
		*/
		virtual Trial* createTrial(int sessionID, const std::string& trialDescription, int trialDuration) = 0;
		/**
		*/
		virtual Segment* defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) = 0;
		/**
		*/
		virtual int setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		/**
		*/
		virtual int setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		/**
		*/
		virtual int setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		/**
		*/
		virtual int setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		/**
		*/
		virtual int setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) = 0;
		/**
		*/
		virtual bool assignSessionToGroup(int sessionID, int groupID) = 0;
		//---------------------------------------------------------------------------------------------------------------------
		//---------------------------------------------------------------------------------------------------------------------
		/**
		*/
		virtual const Performer* getPerformerById(int performerID) = 0;
		/**
		*/
		virtual const Session* getSessionById(int sessionID) = 0;
		/**
		*/
		virtual const Segment* getSegmentById(int segmentID) = 0;
		/**
		*/
		virtual const Trial* getTrialById(int trialID) = 0;
		/**
		*/
		virtual const std::vector<Session>* listPerformerSessions(int performerID) = 0;
		/**
		*/
		virtual const std::vector<Session>* listPerformerSessionsWithAttributes(int performerID) = 0;
		/**
		*/
		virtual const std::vector<Performer>* listPerformers() = 0;
		/**
		*/
		virtual const std::vector<Performer>* listPerformersWithAttributes() = 0;
		/**
		*/
		virtual const std::vector<Trial>* listSessionTrials(int sessionID) = 0;
		/**
		*/
		virtual const std::vector<Trial>* listSessionTrialsWithAttributes(int sessionID) = 0;
		/**
		*/
		virtual const std::vector<Segment>* listTrialSegments(int trialID) = 0;
		/**
		*/
		virtual const std::vector<Segment>* listTrialSegmentsWithAttributes(int trialID) = 0;
		/**
		*/
		virtual const std::vector<Performer>* listLabPerformersWithAttributes(int labID) = 0;
		/**
		*/
		virtual const std::vector<Session>* listLabSessionsWithAttributes(int labID) = 0;
		/**
		*/
		virtual const std::vector<File>* listSessionFiles(int sessionID) = 0;
		/**
		*/
		virtual const std::vector<File>* listFiles(int ID, const std::string& subjectType) = 0;
		/**
		*/
		virtual const std::vector<File>* listFilesWithAttributes(int ID, const std::string& subjectType) = 0;
		/**
		*/
		virtual const std::vector<SessionGroup>* listSessionGroupsDefined() = 0;
		/**
		*/
		virtual const std::vector<MotionKind>* listMotionKindsDefined() = 0;
		/**
		*/
		virtual void performQuery(const std::string& query) = 0;
		/*
		void listAttributeGroupsDefined(EntityType type);
		void listAttributesDefined(const std::string& groupName, EntityType type);
		void genericQuery();
		void genericQueryUniform();
		*/
	};
}
#endif
