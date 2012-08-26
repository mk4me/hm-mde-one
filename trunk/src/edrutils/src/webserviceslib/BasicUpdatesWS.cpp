#include <webserviceslib/BasicUpdatesWS.h>

namespace webservices
{

MotionBasicUpdatesWS::MotionBasicUpdatesWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

MotionBasicUpdatesWS::~MotionBasicUpdatesWS()
{

}

void MotionBasicUpdatesWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & MotionBasicUpdatesWS::connection()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return connection_;
}

const WSConnectionConstPtr & MotionBasicUpdatesWS::connection() const
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return constConnection_;
}



MedicalBasicUpdatesWS::MedicalBasicUpdatesWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

MedicalBasicUpdatesWS::~MedicalBasicUpdatesWS()
{

}

void MedicalBasicUpdatesWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & MedicalBasicUpdatesWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MedicalBasicUpdatesWS::connection() const
{
	return constConnection_;
}


/**
@author Marek Daniluk
*/

//MotionBasicUpdatesService::MotionBasicUpdatesService() { }
//
//MotionBasicUpdatesService::~MotionBasicUpdatesService() { }
//
//Performer* MotionBasicUpdatesService::createPerformer(const std::string& name, const std::string& surname) {
//	this->setOperation("CreatePerformer");
//	if(invoker.status()) {
//		if(!invoker.setValue("Name", new std::string(name)) ||
//			!invoker.setValue("Surname", new std::string(surname))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreatePerformerResult");
//		return new Performer(val, name, surname);
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}

//Session* MotionBasicUpdatesService::createSession(int labID, int performerID, DateTime& sessionDate, const std::string& sessionDescription, const std::string& motionKindName, std::vector<int>& groupsIDs) {
//	this->setOperation("CreateSession");
//	if(invoker.status()) {
//		if(!invoker.setValue("labID", toString<int>(labID)) ||
//			!invoker.setValue("motionKindName", new std::string(motionKindName)) ||
//			!invoker.setValue("performerID", toString<int>(performerID)) ||
//			!invoker.setValue("sessionDate", sessionDate.toString()) ||
//			!invoker.setValue("sessionDescription", new std::string(sessionDescription)) ||
//			!invoker.setValue("int", toStringVector<int>(groupsIDs))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreateSessionResult");
//		return new Session(val, 0, performerID, sessionDate, sessionDescription);
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//Trial* MotionBasicUpdatesService::createTrial(int sessionID, const std::string& trialDescription, int trialDuration) {
//	this->setOperation("CreateTrial");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("trialDuration", toString<int>(trialDuration)) ||
//			!invoker.setValue("trialDescription", new std::string(trialDescription))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("CreateTrialResult");
//		return new Trial(val, trialDescription, trialDuration);
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//Segment* MotionBasicUpdatesService::defineTrialSegment(int trialID, const std::string& segmentName, int startTime, int endTime) {
//	this->setOperation("DefineTrialSegment");
//	if(invoker.status()) {
//		if(!invoker.setValue("trialID", toString<int>(trialID)) ||
//			!invoker.setValue("segmentName", new std::string(segmentName)) ||
//			!invoker.setValue("startTime", toString<int>(startTime)) ||
//			!invoker.setValue("endTime", toString<int>(endTime))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		int val = invoker.getValue<int>("DefineTrialSegment");
//		return new Segment(val, trialID, segmentName, startTime, endTime);
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//bool MotionBasicUpdatesService::assignSessionToGroup(int sessionID, int groupID) {
//	this->setOperation("AssignSessionToGroup");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("groupID", toString<int>(groupID))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<bool>("AssignSessionToGroupResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//int MotionBasicUpdatesService::setPerformerAttribute(int performerID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetPerformerAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", toString<int>(performerID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetPerformerAttributeResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//int MotionBasicUpdatesService::setTrialAttribute(int trialID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetTrialAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("trialID", toString<int>(trialID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetTrialAttributeResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//int MotionBasicUpdatesService::setSessionAttribute(int sessionID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetSessionAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", toString<int>(sessionID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetSessionAttributeResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//int MotionBasicUpdatesService::setFileAttribute(int fileID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetFileAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("fileID", toString<int>(fileID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetFileAttributeResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//int MotionBasicUpdatesService::setSegmentAttribute(int segmentID, const std::string& attributeName, const std::string& attributeVal, bool update) {
//	this->setOperation("SetSegmentAttribute");
//	if(invoker.status()) {
//		if(!invoker.setValue("segmentID", toString<int>(segmentID)) ||
//			!invoker.setValue("attributeName", new std::string(attributeName)) ||
//			!invoker.setValue("attributeValue", new std::string(attributeVal)) ||
//			!invoker.setValue("update", toString<bool>(update))) {
//				throw std::runtime_error("Bad operation arguments.");
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		return invoker.getValue<int>("SetSegmentAttributeResult");
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}


}
