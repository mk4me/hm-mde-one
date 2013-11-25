#include <webserviceslib/BasicUpdatesWS.h>
#include <webserviceslib/DateTimeUtils.h>

namespace webservices
{

MotionBasicUpdatesWS::MotionBasicUpdatesWS(const WSConnectionPtr & connection)
	: WebServiceT<IMotionBasicUpdatesWS>(connection)
{

}

MotionBasicUpdatesWS::~MotionBasicUpdatesWS()
{

}

const int MotionBasicUpdatesWS::createPerformer(const xmlWsdl::PerformerData & performerData)
{
	connection()->setOperation("CreatePerformer");
	connection()->setValue("Name", performerData.name);
	connection()->setValue("Surname", performerData.surname);
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("CreatePerformerResult", ret);

	return ret;
}

const int MotionBasicUpdatesWS::createSession(const int labID, const std::string & motionKindName,
	const DateTime & sessionDate, const std::string & sessionDescription,
	const std::set<int> & sessionGroupIDs)
{
	connection()->setOperation("CreateSession");
	connection()->setValue("labID", labID);
	connection()->setValue("motionKindName", motionKindName);
	connection()->setValue("sessionDate", toString(sessionDate));
	connection()->setValue("sessionDescription", sessionDescription);

	std::string ids;

	//TODO - generowanie identyfikatorów dla grup sesji

	connection()->setValue("sessionGroutIDs", ids);
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("CreateSessionResult", ret);

	return ret;
}

const int MotionBasicUpdatesWS::createTrial(const int sessionID, const std::string & trialDescription)
{
	connection()->setOperation("CreateTrial");
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("trialDescription", trialDescription);
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("CreateTrialResult", ret);

	return ret;
}

const int MotionBasicUpdatesWS::createMeasurementConfiguration(const std::string & mcName,
	const std::string & mcKind, const std::string & mcDescription)
{
	connection()->setOperation("CreateMeasurementConfiguration");
	connection()->setValue("mcName", mcName);
	connection()->setValue("mcKind", mcKind);
	connection()->setValue("mcDescription", mcDescription);
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("CreateMeasurementConfigurationResult", ret);

	return ret;
}

const int MotionBasicUpdatesWS::assignSessionToGroup(const int sessionID, const int sessionsGroupID)
{
	connection()->setOperation("AssignSessionToGroup");
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("sessionsGroupID", sessionsGroupID);	
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("AssignSessionToGroupResult", ret);

	return ret;
}

const int MotionBasicUpdatesWS::assignPerformerToSession(const int performerID, const int sessionID)
{
	connection()->setOperation("AssignPerformerToSession");
	connection()->setValue("performerID", performerID);
	connection()->setValue("sessionID", sessionID);	
	connection()->invoke(true);	

	int ret = -1;
	connection()->getValue<int>("AssignPerformerToSessionResult", ret);

	return ret;
}

const bool MotionBasicUpdatesWS::assignPerformerToMeasurement(const int performerID, const int measurementID)
{
	connection()->setOperation("AssignPerformerToMeasurement");
	connection()->setValue("performerID", performerID);
	connection()->setValue("measurementID", measurementID);	
	connection()->invoke(true);	

	bool ret = false;
	connection()->getValue<bool>("AssignPerformerToMeasurementResult", ret);

	return ret;
}

void MotionBasicUpdatesWS::setPerformerAttribute(const int performerID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetPerformerAttribute");
	connection()->setValue("performerID", performerID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();	
}

void MotionBasicUpdatesWS::setSessionAttribute(const int sessionID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetSessionAttribute");
	connection()->setValue("sessionID", sessionID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();	
}

void MotionBasicUpdatesWS::setTrialAttribute(const int trialID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetTrialAttribute");
	connection()->setValue("trialID", trialID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();	
}

void MotionBasicUpdatesWS::setMeasurementConfAttribute(const int measurementConfID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetMeasurementConfAttribute");
	connection()->setValue("measurementConfID", measurementConfID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();
}

void MotionBasicUpdatesWS::setPerformerConfAttribute(const int performerConfID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetPerformerConfAttribute");
	connection()->setValue("performerConfID", performerConfID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();
}

void MotionBasicUpdatesWS::setFileAttribute(const int fileID, const std::string & attributeName,
	const std::string & attributeValue, const bool update)
{
	connection()->setOperation("SetFileAttribute");
	connection()->setValue("fileID", fileID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("attributeValue", attributeValue);	
	connection()->setValue("update", update);	
	connection()->invoke();
}

void MotionBasicUpdatesWS::setFileTypedAttributeValue(const int resourceID, const std::string & entity,
	const std::string & attributeName, const int fileID, const bool update)
{
	connection()->setOperation("SetFileTypedAttributeValue");
	connection()->setValue("resourceID", resourceID);
	connection()->setValue("entity", entity);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("fileID", fileID);	
	connection()->setValue("update", update);	
	connection()->invoke();
}

void MotionBasicUpdatesWS::clearAttributeValue(const int resourceID, const std::string & attributeName,
	const std::string & entity)
{
	connection()->setOperation("ClearAttributeValue");
	connection()->setValue("resourceID", resourceID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("entity", entity);
	connection()->invoke();
}

MedicalBasicUpdatesWS::MedicalBasicUpdatesWS(const WSConnectionPtr & connection)
	: WebServiceT<IMedicalBasicUpdatesWS>(connection)
{
	
}

MedicalBasicUpdatesWS::~MedicalBasicUpdatesWS()
{

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
