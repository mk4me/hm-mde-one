/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/QueryWSDL.h>

using namespace communication;

MotionQueryWSDL::MotionQueryWSDL() : bqs(new MotionBasicQueriesService()),
    bus(new MotionBasicUpdatesService())
{
    
}

MotionQueryWSDL::~MotionQueryWSDL()
{
}

void MotionQueryWSDL::setWSCredentials(const std::string& usr, const std::string& pswd)
{
    bqs->setUser(usr);
    bqs->setPassword(pswd);
    bus->setUser(usr);
    bus->setPassword(pswd);
}

void MotionQueryWSDL::setBasicQueriesServiceUri(const std::string& uri)
{
    bqs->setUri(uri);
}

void MotionQueryWSDL::setBasicUpdatesServiceUri(const std::string& uri)
{
    bus->setUri(uri);
}

std::vector<motionWsdl::Trial> MotionQueryWSDL::listSessionContents()
{
    return bqs->listSessionContents();
}

std::vector<motionWsdl::Trial> MotionQueryWSDL::listSessionTrials(int sessionID)
{
    return bqs->listSessionTrials(sessionID);
}

std::vector<motionWsdl::Session> MotionQueryWSDL::listLabSessionsWithAttributes(int labID)
{
    return bqs->listLabSessionsWithAttributes(labID);
}

std::vector<motionWsdl::File> MotionQueryWSDL::listFiles(int ID, const std::string& subjectType)
{
    return bqs->listFiles(ID, subjectType);
}

DateTime MotionQueryWSDL::getLastDBModificationTime()
{
    return bqs->getLastDBModificationTime();
}

DateTime MotionQueryWSDL::getLastMetadataModificationTime()
{
    return bqs->getLastMetadataModificationTime();
}

MedicalQueryWSDL::MedicalQueryWSDL() : bqs(new MedicalBasicQueriesService()),
    bus(new MedicalBasicUpdatesService())
{
    
}

MedicalQueryWSDL::~MedicalQueryWSDL()
{
}

void MedicalQueryWSDL::setWSCredentials(const std::string& usr, const std::string& pswd)
{
    bqs->setUser(usr);
    bqs->setPassword(pswd);
    bus->setUser(usr);
    bus->setPassword(pswd);
}

void MedicalQueryWSDL::setBasicQueriesServiceUri(const std::string& uri)
{
    bqs->setUri(uri);
}

void MedicalQueryWSDL::setBasicUpdatesServiceUri(const std::string& uri)
{
    bus->setUri(uri);
}

//Performer* MotionQueryWSDL::createPerformer(const std::string& name, const std::string& surname) {
//	return bus->createPerformer(name, surname);
//}
//Session* MotionQueryWSDL::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
//	return bus->createSession(labID, performerID, sessionDate, sessionDescription, motionKindName, groupsIDs);
//}
//
//Trial* MotionQueryWSDL::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
//	return bus->createTrial(sessionID, trialDescription, trialDuration);
//}
//
//Segment* MotionQueryWSDL::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
//	return bus->defineTrialSegment(trialID, segmentName, startTime, endTime);
//}
//
//int MotionQueryWSDL::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setPerformerAttribute(performerID, attributeName, attributeVal, update);
//}
//
//int MotionQueryWSDL::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setSessionAttribute(sessionID, attributeName, attributeVal, update);
//}
//
//int MotionQueryWSDL::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setTrialAttribute(trialID, attributeName, attributeVal, update);
//}
//
//int MotionQueryWSDL::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setFileAttribute(fileID, attributeName, attributeVal, update);
//}
//
//int MotionQueryWSDL::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	return bus->setSegmentAttribute(segmentID, attributeName, attributeVal, update);
//}
//
//bool MotionQueryWSDL::assignSessionToGroup(int sessionID, int groupID) {
//	return bus->assignSessionToGroup(sessionID, groupID);
//}
//
//const Performer* MotionQueryWSDL::getPerformerById(int performerID) {
//	return bqs->getPerformerById(performerID);
//}
//
//const Session* MotionQueryWSDL::getSessionById(int sessionID) {
//	return bqs->getSessionById(sessionID);
//}
//
//const Segment* MotionQueryWSDL::getSegmentById(int segmentID) {
//	return bqs->getSegmentById(segmentID);
//}
//
//const Trial* MotionQueryWSDL::getTrialById(int trialID) {
//	return bqs->getTrialById(trialID);
//}
//
//const std::vector<Session>* MotionQueryWSDL::listPerformerSessions(int performerID) {
//	return bqs->listPerformerSessions(performerID);
//}
//
//const std::vector<Session>* MotionQueryWSDL::listPerformerSessionsWithAttributes(int performerID) {
//	return bqs->listPerformerSessionsWithAttributes(performerID);
//}
//
//const std::vector<Performer>* MotionQueryWSDL::listPerformers() {
//	return bqs->listPerformers();
//}
//
//const std::vector<Performer>* MotionQueryWSDL::listPerformersWithAttributes() {
//	return bqs->listPerformersWithAttributes();
//}

//const std::vector<Trial>* MotionQueryWSDL::listSessionTrialsWithAttributes(int sessionID) {
//	return bqs->listSessionTrialsWithAttributes(sessionID);
//}
//
//const std::vector<Segment>* MotionQueryWSDL::listTrialSegments(int trialID) {
//	return bqs->listTrialSegments(trialID);
//}
//
//const std::vector<Segment>* MotionQueryWSDL::listTrialSegmentsWithAttributes(int trialID) {
//	return bqs->listTrialSegmentsWithAttributes(trialID);
//}
//
//const std::vector<Performer>* MotionQueryWSDL::listLabPerformersWithAttributes(int labID) {
//	return bqs->listLabPerformersWithAttributes(labID);
//}

//const std::vector<File>* MotionQueryWSDL::listSessionFiles(int sessionID) {
//	return bqs->listSessionFiles(sessionID);
//}

//const std::vector<File>* MotionQueryWSDL::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	return bqs->listFilesWithAttributes(ID, subjectType);
//}
//
//const std::vector<SessionGroup>* MotionQueryWSDL::listSessionGroupsDefined() {
//	return bqs->listSessionGroupsDefined();
//}
//
//const std::vector<MotionKind>* MotionQueryWSDL::listMotionKindsDefined() {
//	return bqs->listMotionKindsDefined();
//}
//
//void MotionQueryWSDL::performQuery(const std::string& query) {
//	bqs->performQuery(query);
//}
