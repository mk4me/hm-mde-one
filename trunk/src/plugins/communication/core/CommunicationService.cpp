/**
@author Marek Daniluk
*/

#include "CommunicationService.h"

using namespace EDRCM;

CommunicationService* CommunicationService::pInstance = NULL;

CommunicationService* CommunicationService::getInstance() {
	if(pInstance == NULL) {
		pInstance = new CommunicationService();
	}
	return pInstance;
}

CommunicationService* CommunicationService::createInstanceFirstTime() {
	if(pInstance == NULL) {
		pInstance = new CommunicationService();
	} else {
		throw EDRException("Instance already created.");
	}
	return pInstance;
}

void CommunicationService::destoryInstance() {
	if(pInstance) {
		delete pInstance;
	}
	pInstance = NULL;
}

bool CommunicationService::init() {
	transportManager = NULL;
	queryManager = NULL;
	return true;
}

void CommunicationService::setTransportManager(ITransportable* transportManager) {
	if(this->transportManager != NULL) {
		delete this->transportManager;
	}
	this->transportManager = transportManager;
}

void CommunicationService::setQueryManager(IQueryable* queryManager) {
	if(this->queryManager != NULL) {
		delete this->queryManager;
	}
	this->queryManager = queryManager;
}

int CommunicationService::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeSessionFile(sessionID, path, description, filename);
}

int CommunicationService::storeSessionFiles(int sessionID, const std::string& path, const std::string& description) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeSessionFiles(sessionID, path, description);
}

int CommunicationService::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storePerformerFile(performerID, path, description, filename);
}

void CommunicationService::storePerformerFiles(int performerID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	transportManager->storePerformerFiles(performerID, path);
}

int CommunicationService::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeTrialFile(trialID, path, description, filename);
}

void CommunicationService::storeTrialFiles(int trialID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	transportManager->storeTrialFiles(trialID, path);
}

const std::string CommunicationService::downloadFile(int fileID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->downloadFile(fileID, path);
}

Performer* CommunicationService::createPerformer(const std::string& name, const std::string& surname) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createPerformer(name, surname);
}
Session* CommunicationService::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createSession(labID, performerID, sessionDate, sessionDescription, motionKindName, groupsIDs);
}

Trial* CommunicationService::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createTrial(sessionID, trialDescription, trialDuration);
}

Segment* CommunicationService::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->defineTrialSegment(trialID, segmentName, startTime, endTime);
}

int CommunicationService::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setPerformerAttribute(performerID, attributeName, attributeVal, update);
}

int CommunicationService::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setSessionAttribute(sessionID, attributeName, attributeVal, update);
}

int CommunicationService::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setTrialAttribute(trialID, attributeName, attributeVal, update);
}

int CommunicationService::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setFileAttribute(fileID, attributeName, attributeVal, update);
}

int CommunicationService::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setSegmentAttribute(segmentID, attributeName, attributeVal, update);
}

bool CommunicationService::assignSessionToGroup(int sessionID, int groupID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->assignSessionToGroup(sessionID, groupID);
}

const Performer* CommunicationService::getPerformerById(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getPerformerById(performerID);
}

const Session* CommunicationService::getSessionById(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getSessionById(sessionID);
}

const Segment* CommunicationService::getSegmentById(int segmentID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getSegmentById(segmentID);
}

const Trial* CommunicationService::getTrialById(int trialID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getTrialById(trialID);
}

const std::vector<Session>* CommunicationService::listPerformerSessions(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformerSessions(performerID);
}

const std::vector<Session>* CommunicationService::listPerformerSessionsWithAttributes(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformerSessionsWithAttributes(performerID);
}

const std::vector<Performer>* CommunicationService::listPerformers() {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformers();
}

const std::vector<Performer>* CommunicationService::listPerformersWithAttributes() {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformersWithAttributes();
}

const std::vector<Trial>* CommunicationService::listSessionTrials(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listSessionTrials(sessionID);
}

const std::vector<Trial>* CommunicationService::listSessionTrialsWithAttributes(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listSessionTrialsWithAttributes(sessionID);
}
//
//const std::vector<Segment>* CommunicationService::listTrialSegments(int trialID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listTrialSegments(trialID);
//}
//
//const std::vector<Segment>* CommunicationService::listTrialSegmentsWithAttributes(int trialID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listTrialSegmentsWithAttributes(trialID);
//}
//
//const std::vector<Performer>* CommunicationService::listLabPerformersWithAttributes(int labID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listLabPerformersWithAttributes(labID);
//}

const std::vector<Session>* CommunicationService::listLabSessionsWithAttributes(int labID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listLabSessionsWithAttributes(labID);
}

//const std::vector<File>* CommunicationService::listSessionFiles(int sessionID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listSessionFiles(sessionID);
//}

const std::vector<File>* CommunicationService::listFiles(int ID, const std::string& subjectType) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listFiles(ID, subjectType);
}

//const std::vector<File>* CommunicationService::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listFilesWithAttributes(ID, subjectType);
//}
//
//const std::vector<SessionGroup>* CommunicationService::listSessionGroupsDefined() {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listSessionGroupsDefined();
//}
//
//const std::vector<MotionKind>* CommunicationService::listMotionKindsDefined() {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listMotionKindsDefined();
//}
//
//void CommunicationService::performQuery(const std::string& query) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	queryManager->performQuery(query);
//}
