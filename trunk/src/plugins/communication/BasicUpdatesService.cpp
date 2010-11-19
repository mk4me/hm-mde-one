/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/BasicUpdatesService.h>

using namespace communication;

BasicUpdatesService::BasicUpdatesService() { }

BasicUpdatesService::~BasicUpdatesService() { }
//
//Performer* BasicUpdatesService::createPerformer(const std::string& name, const std::string& surname) {
//	this->setOperation("CreatePerformer");
//	if(invoker.status()) {
//		if(!invoker.setValue("Name", new std::string(name)) ||
//			!invoker.setValue("Surname", new std::string(surname))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreatePerformerResult");
//		return new Performer(val, name, surname);
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}

//Session* BasicUpdatesService::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
//	this->setOperation("CreateSession");
//	if(invoker.status()) {
//		if(!invoker.setValue("labID", toString<int>(labID)) ||
//			!invoker.setValue("motionKindName", new std::string(motionKindName)) ||
//			!invoker.setValue("performerID", toString<int>(performerID)) ||
//			!invoker.setValue("sessionDate", sessionDate.toString()) ||
//			!invoker.setValue("sessionDescription", new std::string(sessionDescription)) ||
//			!invoker.setValue("int", toStringVector<int>(groupsIDs))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreateSessionResult");
//		return new Session(val, 0, performerID, sessionDate, sessionDescription);
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//Trial* BasicUpdatesService::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
//	this->setOperation("CreateTrial");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("trialDuration", toString<int>(trialDuration)) ||
//			!invoker.setValue("trialDescription", new std::string(trialDescription))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreateTrialResult");
//		return new Trial(val, trialDescription, trialDuration);
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//Segment* BasicUpdatesService::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
//	this->setOperation("DefineTrialSegment");
//	if(invoker.status()) {
//		if(!invoker.setValue("trialID", toString<int>(trialID)) ||
//			!invoker.setValue("segmentName", new std::string(segmentName)) ||
//			!invoker.setValue("startTime", toString<int>(startTime)) ||
//			!invoker.setValue("endTime", toString<int>(endTime))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("DefineTrialSegment");
//		return new Segment(val, trialID, segmentName, startTime, endTime);
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//bool BasicUpdatesService::assignSessionToGroup(int sessionID, int groupID) {
//	this->setOperation("AssignSessionToGroup");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("groupID", toString<int>(groupID))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<bool>("AssignSessionToGroupResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//int BasicUpdatesService::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetPerformerAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", toString<int>(performerID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetPerformerAttributeResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//int BasicUpdatesService::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetTrialAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("trialID", toString<int>(trialID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetTrialAttributeResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//int BasicUpdatesService::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetSessionAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetSessionAttributeResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//int BasicUpdatesService::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetFileAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("fileID", toString<int>(fileID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetFileAttributeResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//int BasicUpdatesService::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetSegmentAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("segmentID", toString<int>(segmentID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw EDRException("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetSegmentAttributeResult");
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
