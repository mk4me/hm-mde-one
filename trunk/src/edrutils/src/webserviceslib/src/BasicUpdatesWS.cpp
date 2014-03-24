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

	connection()->setValue("sessionGroupIDs", ids);
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

void MotionBasicUpdatesWS::setFileTypedAttributeValue(const int resourceID, const xmlWsdl::Entity::Type entity,
	const std::string & attributeName, const int fileID, const bool update)
{
	connection()->setOperation("SetFileTypedAttributeValue");
	connection()->setValue("resourceID", resourceID);
	connection()->setValue("entity", xmlWsdl::Entity::convert(entity));
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("fileID", fileID);	
	connection()->setValue("update", update);	
	connection()->invoke();
}

void MotionBasicUpdatesWS::clearAttributeValue(const int resourceID, const std::string & attributeName,
	const xmlWsdl::Entity::Type entity)
{
	connection()->setOperation("ClearAttributeValue");
	connection()->setValue("resourceID", resourceID);
	connection()->setValue("attributeName", attributeName);	
	connection()->setValue("entity", xmlWsdl::Entity::convert(entity));
	connection()->invoke();
}

void MotionBasicUpdatesWS::setMyAnnotationStatus(const int trialID, const xmlWsdl::AnnotationStatus::Type status,
	const std::string & comment)
{
	connection()->setOperation("SetMyAnnotationStatus");
	connection()->setValue("trialID", trialID);
	connection()->setValue("status", static_cast<int>(status));	
	connection()->setValue("comment", comment);
	connection()->invoke();
}

void MotionBasicUpdatesWS::setAnnotationReview(const int trialID, const int userID,
	const xmlWsdl::AnnotationStatus::Type status,
	const std::string & note)
{
	connection()->setOperation("SetAnnotationReview");
	connection()->setValue("trialID", trialID);
	connection()->setValue("userID", userID);
	connection()->setValue("status", static_cast<int>(status));	
	connection()->setValue("note", note);
	connection()->invoke();
}

MedicalBasicUpdatesWS::MedicalBasicUpdatesWS(const WSConnectionPtr & connection)
	: WebServiceT<IMedicalBasicUpdatesWS>(connection)
{
	
}

MedicalBasicUpdatesWS::~MedicalBasicUpdatesWS()
{

}

}
