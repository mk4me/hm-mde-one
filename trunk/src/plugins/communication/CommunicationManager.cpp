/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/CommunicationManager.h>

using namespace communication;

CommunicationManager* CommunicationManager::pInstance = NULL;

CommunicationManager* CommunicationManager::getInstance() {
	if(pInstance == NULL) {
		pInstance = new CommunicationManager();
	}
	return pInstance;
}

CommunicationManager* CommunicationManager::createInstanceFirstTime() {
	if(pInstance == NULL) {
		pInstance = new CommunicationManager();
	} else {
		throw EDRException("Instance already created.");
	}
	return pInstance;
}

void CommunicationManager::destoryInstance() {
	if(pInstance) {
		delete pInstance;
	}
	pInstance = NULL;
}

bool CommunicationManager::init() {
	transportManager = NULL;
	queryManager = NULL;
	return true;
}

void CommunicationManager::setTransportManager(ITransportable* transportManager) {
	if(this->transportManager != NULL) {
		delete this->transportManager;
	}
	this->transportManager = transportManager;
}

void CommunicationManager::setQueryManager(IQueryable* queryManager) {
	if(this->queryManager != NULL) {
		delete this->queryManager;
	}
	this->queryManager = queryManager;
}

int CommunicationManager::storeSessionFile(int sessionID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeSessionFile(sessionID, path, description, filename);
}

int CommunicationManager::storeSessionFiles(int sessionID, const std::string& path, const std::string& description) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeSessionFiles(sessionID, path, description);
}

int CommunicationManager::storePerformerFile(int performerID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storePerformerFile(performerID, path, description, filename);
}

void CommunicationManager::storePerformerFiles(int performerID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	transportManager->storePerformerFiles(performerID, path);
}

int CommunicationManager::storeTrialFile(int trialID, const std::string& path, const std::string& description, const std::string& filename) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->storeTrialFile(trialID, path, description, filename);
}

void CommunicationManager::storeTrialFiles(int trialID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	transportManager->storeTrialFiles(trialID, path);
}

const std::string CommunicationManager::downloadFile(int fileID, const std::string& path) {
	if(transportManager == NULL) {
		throw EDRException("transportManager object is not set.");
	}
	return transportManager->downloadFile(fileID, path);
}

Performer* CommunicationManager::createPerformer(const std::string& name, const std::string& surname) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createPerformer(name, surname);
}
Session* CommunicationManager::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createSession(labID, performerID, sessionDate, sessionDescription, motionKindName, groupsIDs);
}

Trial* CommunicationManager::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->createTrial(sessionID, trialDescription, trialDuration);
}

Segment* CommunicationManager::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->defineTrialSegment(trialID, segmentName, startTime, endTime);
}

int CommunicationManager::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setPerformerAttribute(performerID, attributeName, attributeVal, update);
}

int CommunicationManager::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setSessionAttribute(sessionID, attributeName, attributeVal, update);
}

int CommunicationManager::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setTrialAttribute(trialID, attributeName, attributeVal, update);
}

int CommunicationManager::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setFileAttribute(fileID, attributeName, attributeVal, update);
}

int CommunicationManager::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->setSegmentAttribute(segmentID, attributeName, attributeVal, update);
}

bool CommunicationManager::assignSessionToGroup(int sessionID, int groupID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->assignSessionToGroup(sessionID, groupID);
}

const Performer* CommunicationManager::getPerformerById(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getPerformerById(performerID);
}

const Session* CommunicationManager::getSessionById(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getSessionById(sessionID);
}

const Segment* CommunicationManager::getSegmentById(int segmentID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getSegmentById(segmentID);
}

const Trial* CommunicationManager::getTrialById(int trialID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->getTrialById(trialID);
}

const std::vector<Session>* CommunicationManager::listPerformerSessions(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformerSessions(performerID);
}

const std::vector<Session>* CommunicationManager::listPerformerSessionsWithAttributes(int performerID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformerSessionsWithAttributes(performerID);
}

const std::vector<Performer>* CommunicationManager::listPerformers() {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformers();
}

const std::vector<Performer>* CommunicationManager::listPerformersWithAttributes() {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listPerformersWithAttributes();
}

const std::vector<Trial>* CommunicationManager::listSessionTrials(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listSessionTrials(sessionID);
}

const std::vector<Trial>* CommunicationManager::listSessionTrialsWithAttributes(int sessionID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listSessionTrialsWithAttributes(sessionID);
}
//
//const std::vector<Segment>* CommunicationManager::listTrialSegments(int trialID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listTrialSegments(trialID);
//}
//
//const std::vector<Segment>* CommunicationManager::listTrialSegmentsWithAttributes(int trialID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listTrialSegmentsWithAttributes(trialID);
//}
//
//const std::vector<Performer>* CommunicationManager::listLabPerformersWithAttributes(int labID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listLabPerformersWithAttributes(labID);
//}

const std::vector<Session>* CommunicationManager::listLabSessionsWithAttributes(int labID) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listLabSessionsWithAttributes(labID);
}

//const std::vector<File>* CommunicationManager::listSessionFiles(int sessionID) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listSessionFiles(sessionID);
//}

const std::vector<File>* CommunicationManager::listFiles(int ID, const std::string& subjectType) {
	if(queryManager == NULL) {
		EDRException("queryManager object is not set.");
	}
	return queryManager->listFiles(ID, subjectType);
}

//const std::vector<File>* CommunicationManager::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listFilesWithAttributes(ID, subjectType);
//}
//
//const std::vector<SessionGroup>* CommunicationManager::listSessionGroupsDefined() {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listSessionGroupsDefined();
//}
//
//const std::vector<MotionKind>* CommunicationManager::listMotionKindsDefined() {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	return queryManager->listMotionKindsDefined();
//}
//
//void CommunicationManager::performQuery(const std::string& query) {
//	if(queryManager == NULL) {
//		EDRException("queryManager object is not set.");
//	}
//	queryManager->performQuery(query);
//}
