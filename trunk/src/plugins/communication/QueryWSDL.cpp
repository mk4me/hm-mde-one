/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/QueryWSDL.h>

using namespace communication;

QueryWSDL::QueryWSDL() {
	bqs = new BasicQueriesService();
	bus = new BasicUpdatesService();
}

QueryWSDL::~QueryWSDL() {
	if(bqs) {
		delete bqs;
		bqs = NULL;
	}
	if(bus) {
		delete bus;
		bus = NULL;
	}
}

void QueryWSDL::setWSCredentials(const std::string& usr, const std::string& pswd) {
	bqs->setUser(usr);
	bqs->setPassword(pswd);
	bus->setUser(usr);
	bus->setPassword(pswd);
}

void QueryWSDL::setBasicQueriesServiceUri(const std::string& uri) {
	bqs->setUri(uri);
}

void QueryWSDL::setBasicUpdatesServiceUri(const std::string& uri) {
	bus->setUri(uri);
}

void QueryWSDL::listSessionContents()
{
	bqs->listSessionContents();
}

std::map<int, Trial> QueryWSDL::listSessionTrials(int sessionID) {
	return bqs->listSessionTrials(sessionID);
}

std::map<int, Session> QueryWSDL::listLabSessionsWithAttributes(int labID) {
	return bqs->listLabSessionsWithAttributes(labID);
}

std::map<int, File> QueryWSDL::listFiles(int ID, const std::string& subjectType) {
	return bqs->listFiles(ID, subjectType);
}

//Performer* QueryWSDL::createPerformer(const std::string& name, const std::string& surname) {
//	return bus->createPerformer(name, surname);
//}
//Session* QueryWSDL::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
//	return bus->createSession(labID, performerID, sessionDate, sessionDescription, motionKindName, groupsIDs);
//}
//
//Trial* QueryWSDL::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
//	return bus->createTrial(sessionID, trialDescription, trialDuration);
//}
//
//Segment* QueryWSDL::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
//	return bus->defineTrialSegment(trialID, segmentName, startTime, endTime);
//}
//
//int QueryWSDL::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setPerformerAttribute(performerID, attributeName, attributeVal, update);
//}
//
//int QueryWSDL::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setSessionAttribute(sessionID, attributeName, attributeVal, update);
//}
//
//int QueryWSDL::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setTrialAttribute(trialID, attributeName, attributeVal, update);
//}
//
//int QueryWSDL::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setFileAttribute(fileID, attributeName, attributeVal, update);
//}
//
//int QueryWSDL::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setSegmentAttribute(segmentID, attributeName, attributeVal, update);
//}
//
//bool QueryWSDL::assignSessionToGroup(int sessionID, int groupID) {
//	return bus->assignSessionToGroup(sessionID, groupID);
//}
//
//const Performer* QueryWSDL::getPerformerById(int performerID) {
//	return bqs->getPerformerById(performerID);
//}
//
//const Session* QueryWSDL::getSessionById(int sessionID) {
//	return bqs->getSessionById(sessionID);
//}
//
//const Segment* QueryWSDL::getSegmentById(int segmentID) {
//	return bqs->getSegmentById(segmentID);
//}
//
//const Trial* QueryWSDL::getTrialById(int trialID) {
//	return bqs->getTrialById(trialID);
//}
//
//const std::vector<Session>* QueryWSDL::listPerformerSessions(int performerID) {
//	return bqs->listPerformerSessions(performerID);
//}
//
//const std::vector<Session>* QueryWSDL::listPerformerSessionsWithAttributes(int performerID) {
//	return bqs->listPerformerSessionsWithAttributes(performerID);
//}
//
//const std::vector<Performer>* QueryWSDL::listPerformers() {
//	return bqs->listPerformers();
//}
//
//const std::vector<Performer>* QueryWSDL::listPerformersWithAttributes() {
//	return bqs->listPerformersWithAttributes();
//}

//const std::vector<Trial>* QueryWSDL::listSessionTrialsWithAttributes(int sessionID) {
//	return bqs->listSessionTrialsWithAttributes(sessionID);
//}
//
//const std::vector<Segment>* QueryWSDL::listTrialSegments(int trialID) {
//	return bqs->listTrialSegments(trialID);
//}
//
//const std::vector<Segment>* QueryWSDL::listTrialSegmentsWithAttributes(int trialID) {
//	return bqs->listTrialSegmentsWithAttributes(trialID);
//}
//
//const std::vector<Performer>* QueryWSDL::listLabPerformersWithAttributes(int labID) {
//	return bqs->listLabPerformersWithAttributes(labID);
//}

//const std::vector<File>* QueryWSDL::listSessionFiles(int sessionID) {
//	return bqs->listSessionFiles(sessionID);
//}

//const std::vector<File>* QueryWSDL::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	return bqs->listFilesWithAttributes(ID, subjectType);
//}
//
//const std::vector<SessionGroup>* QueryWSDL::listSessionGroupsDefined() {
//	return bqs->listSessionGroupsDefined();
//}
//
//const std::vector<MotionKind>* QueryWSDL::listMotionKindsDefined() {
//	return bqs->listMotionKindsDefined();
//}
//
//void QueryWSDL::performQuery(const std::string& query) {
//	bqs->performQuery(query);
//}
