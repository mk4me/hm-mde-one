#include <webserviceslib/BasicQueriesWS.h>
#include <webserviceslib/DateTimeUtils.h>

namespace webservices
{
	const std::string simpleXMLQuerry(const std::string & operation,
		const WSConnectionConstPtr & connection)
	{
		WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection);
		c->setOperation(operation);		
		c->invoke();
		return c->xmlResponse();
	}


	const std::string simpleIDXMLQuerry(const int id, const std::string & operation,
		const WSConnectionConstPtr & connection,
		const std::string & idName = std::string("id"))
	{
		WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection);
		c->setOperation(operation);
		c->setValue(idName, id);
		c->invoke();
		return c->xmlResponse();
	}

	const std::string simpleSubjectXMLQuerry(const int subjectID, const std::string & subjectType,
		const std::string & operation, const WSConnectionConstPtr & connection)
	{
		WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection);
		c->setOperation(operation);
		c->setValue("subjectID", subjectID);
		c->setValue("subjectType", subjectType);
		c->invoke();
		return c->xmlResponse();
	}

GeneralBasicQueriesWS::GeneralBasicQueriesWS(const WSConnectionPtr & connection)
	: WebServiceT<IGeneralBasicQueriesWS>(connection)
{
	
}

GeneralBasicQueriesWS::~GeneralBasicQueriesWS()
{

}

const DateTime GeneralBasicQueriesWS::dataModificationTime() const
{
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("GetDBTimestamp");
	c->invoke(true);

	std::string time;
	c->getValue("GetDBTimestampResult", time);

	return toTime(time);
}

const DateTime GeneralBasicQueriesWS::metadataModificationTime() const
{
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("GetMetadataTimestamp");
	c->invoke(true);

	std::string time;
	c->getValue("GetMetadataTimestampResult", time);

	return toTime(time);
}

MotionBasicQueriesWS::MotionBasicQueriesWS(const WSConnectionPtr & connection)
	: WebServiceT<IMotionBasicQueriesWS>(connection)
{

}

MotionBasicQueriesWS::~MotionBasicQueriesWS()
{

}

const std::string MotionBasicQueriesWS::getPerformerByIdXML(const int id) const
{
	return simpleIDXMLQuerry(id, "GetPerformerByIdXML", connection());	
}


const std::string MotionBasicQueriesWS::getSessionByIdXML(const int id) const
{
	return simpleIDXMLQuerry(id, "GetSessionByIdXML", connection());
}

const std::string MotionBasicQueriesWS::getSessionLabel(const int id) const
{	
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("GetSessionLabel");
	c->setValue("id", id);
	c->invoke(true);

	std::string ret;

	try{
		c->getValue("SessionLabel", ret);
	}catch(...) {

	}

	return ret;
}

const std::string MotionBasicQueriesWS::getTrialByIdXML(const int id) const
{
	return simpleIDXMLQuerry(id, "GetTrialByIdXML", connection());
}

const std::string MotionBasicQueriesWS::getMeasurementConfigurationByIdXML(const int id) const
{
	return simpleIDXMLQuerry(id, "GetMeasurementConfigurationByIdXML", connection());
}

const std::string MotionBasicQueriesWS::getPerformerConfigurationByIdXML(const int id) const
{
	return simpleIDXMLQuerry(id, "GetPerformerConfigurationByIdXML", connection());
}

const std::string MotionBasicQueriesWS::listPerformersXML() const
{
	return simpleXMLQuerry("ListPerformersXML", connection());	
}

const std::string MotionBasicQueriesWS::listPerformersWithAttributesXML() const
{
	return simpleXMLQuerry("ListPerformersWithAttributesXML", connection());
}

const std::string MotionBasicQueriesWS::listLabPerformersWithAttributesXML(const int labID) const
{
	return simpleIDXMLQuerry(labID, "ListLabPerformersWithAttributesXML", connection(), "labID");
}

const std::string MotionBasicQueriesWS::listSessionPerformersWithAttributesXML(const int sessionID) const
{
	return simpleIDXMLQuerry(sessionID, "ListSessionPerformersWithAttributesXML", connection(), "sessionID");
}

const std::string MotionBasicQueriesWS::listPerformerSessionsXML(const int performerID) const
{
	return simpleIDXMLQuerry(performerID, "ListPerformerSessionsXML", connection(), "performerID");
}

const std::string MotionBasicQueriesWS::listPerformerSessionsWithAttributesXML(const int performerID) const
{
	return simpleIDXMLQuerry(performerID, "ListPerformerSessionsWithAttributesXML", connection(), "performerID");
}

const std::string MotionBasicQueriesWS::listLabSessionsWithAttributesXML(const int labID) const
{
	return simpleIDXMLQuerry(labID, "ListLabSessionsWithAttributesXML", connection(), "labID");
}

const std::string MotionBasicQueriesWS::listGroupSessionsWithAttributesXML(const int sessionGroupID) const
{
	return simpleIDXMLQuerry(sessionGroupID, "ListGroupSessionsWithAttributesXML", connection(), "sessionGroupID");
}

const std::string MotionBasicQueriesWS::listMeasurementConfigSessionsWithAttributesXML(const int measurementConfID) const
{
	return simpleIDXMLQuerry(measurementConfID, "ListMeasurementConfigSessionsWithAttributesXML", connection(), "measurementConfID");
}

const std::string MotionBasicQueriesWS::listSessionSessionGroups(const int sessionID) const
{
	return simpleIDXMLQuerry(sessionID, "ListSessionSessionGroups", connection(), "sessionID");
}

const std::string MotionBasicQueriesWS::listSessionTrialsXML(const int sessionID) const
{
	return simpleIDXMLQuerry(sessionID, "ListSessionTrialsXML", connection(), "sessionID");
}

const std::string MotionBasicQueriesWS::listSessionTrialsWithAttributesXML(const int sessionID) const
{
	return simpleIDXMLQuerry(sessionID, "ListSessionTrialsWithAttributesXML", connection(), "sessionID");
}

const std::string MotionBasicQueriesWS::listMeasurementConfigurationsWithAttributesXML() const
{
	return simpleXMLQuerry("ListMeasurementConfigurationsWithAttributesXML", connection());
}

const std::string MotionBasicQueriesWS::listSessionPerformerConfsWithAttributesXML(const int sessionID) const
{
	return simpleIDXMLQuerry(sessionID, "ListSessionPerformerConfsWithAttributesXML", connection(), "sessionID");
}

const std::string MotionBasicQueriesWS::listFilesXML(const int subjectID, const std::string & subjectType) const
{
	return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFilesXML", connection());
}

const std::string MotionBasicQueriesWS::listFilesWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFilesWithAttributesXML", connection());
}

const std::string MotionBasicQueriesWS::listFileAttributeDataXML(const int subjectID, const std::string & subjectType) const
{
	return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFileAttributeDataXML", connection());
}

const std::string MotionBasicQueriesWS::listFileAttributeDataWithAttributesXML(const int subjectID, const std::string & subjectType) const
{
	return simpleSubjectXMLQuerry(subjectID, subjectType, "ListFileAttributeDataWithAttributesXML", connection());
}

const std::string MotionBasicQueriesWS::listAttributesDefined(const std::string & attributeGroupName,
	const std::string & entityKind) const
{
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("ListAttributesDefined");
	c->setValue("attributeGroupName", attributeGroupName);
	c->setValue("entityKind", entityKind);
	c->invoke();
	return c->xmlResponse();
}

const std::string MotionBasicQueriesWS::listAttributeGroupsDefined(const std::string & entityKind) const
{
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("ListAttributeGroupsDefined");
	c->setValue("entityKind", entityKind);	
	c->invoke();
	return c->xmlResponse();
}

const std::string MotionBasicQueriesWS::listSessionGroupsDefined() const
{
	return simpleXMLQuerry("ListSessionGroupsDefined", connection());
}

const std::string MotionBasicQueriesWS::listMotionKindsDefined() const
{
	return simpleXMLQuerry("ListMotionKindsDefined", connection());
}

const std::string MotionBasicQueriesWS::listEnumValues(const std::string & attributeName,
	const std::string & entityKind) const
{
	WSConnectionPtr c = boost::const_pointer_cast<IWSConnection>(connection());
	c->setOperation("ListEnumValues");
	c->setValue("attributeName", attributeName);
	c->setValue("entityKind", entityKind);
	c->invoke();
	return c->xmlResponse();
}

const std::string MotionBasicQueriesWS::listMyAnnotationsXML() const
{
	return simpleXMLQuerry("ListMyAnnotationsXML", connection());
}

const std::string MotionBasicQueriesWS::listAnnotationsXML() const
{
	return simpleXMLQuerry("ListAnnotationsXML", connection());
}

const std::string MotionBasicQueriesWS::listMyAwaitingAnnotationsXML() const
{
	return simpleXMLQuerry("ListMyAwaitingAnnotationsXML", connection());
}

const std::string MotionBasicQueriesWS::listMyReviewedAnnotationsXML() const
{
	return simpleXMLQuerry("ListMyReviewedAnnotationsXML", connection());
}

const DateTime MotionBasicQueriesWS::dataModificationTime() const
{
	return GeneralBasicQueriesWS(boost::const_pointer_cast<IWSConnection>(connection())).dataModificationTime();    
}

const DateTime MotionBasicQueriesWS::metadataModificationTime() const
{
	return GeneralBasicQueriesWS(boost::const_pointer_cast<IWSConnection>(connection())).metadataModificationTime();
}


MedicalBasicQueriesWS::MedicalBasicQueriesWS(const WSConnectionPtr & connection)
	: WebServiceT<IMedicalBasicQueriesWS>(connection)
{
	
}

MedicalBasicQueriesWS::~MedicalBasicQueriesWS()
{

}

const DateTime MedicalBasicQueriesWS::dataModificationTime() const
{
	return GeneralBasicQueriesWS(boost::const_pointer_cast<IWSConnection>(connection())).dataModificationTime(); 
}

const DateTime MedicalBasicQueriesWS::metadataModificationTime() const
{
	return GeneralBasicQueriesWS(boost::const_pointer_cast<IWSConnection>(connection())).metadataModificationTime();
}

}
