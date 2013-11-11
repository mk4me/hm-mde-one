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

/*
const std::vector<motionWsdl::Trial> MotionBasicQueriesWS::listSessionTrials(int sessionID)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	std::vector<motionWsdl::Trial> ret;
	TiXmlDocument document;

	{
		webservices::IWSConnection::ScopedLock lock(*connection_);
		connection_->setOperation("ListSessionTrialsXML");
		connection_->setValue("sessionID", sessionID);
		connection_->invoke();
            
   
		if(!document.Parse(connection_->xmlResponse().c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	//ListSessionTrialsXMLResponse
	TiXmlElement* trials = hParent.FirstChild("ListSessionTrialsXMLResponse").ToElement();
	trials = trials->FirstChildElement("ListSessionTrialsXMLResult");
	trials = trials->FirstChildElement("SessionTrialList");
	trials = trials->FirstChildElement("SessionTrialList");
	TiXmlElement* trial_details = trials->FirstChildElement("TrialDetails");
	while(trial_details != nullptr) {
		motionWsdl::Trial trial;
		
		trial_details->QueryIntAttribute("TrialID", &trial.id);
		trial_details->QueryIntAttribute("SessionID", &trial.sessionId);
		trial_details->QueryStringAttribute("TrialDescription", &trial.trialDescription);
		
		//TODO
		//uzupełnić brakujące pliki triala lub poprawić strukturę wsdlową

		ret.push_back(trial);

		trial_details = trial_details->NextSiblingElement();
	}
            
    return ret;
}

const std::vector<motionWsdl::Session> MotionBasicQueriesWS::listLabSessionsWithAttributes(int labID)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	std::vector<motionWsdl::Session> ret;

	TiXmlDocument document;

	{
		webservices::IWSConnection::ScopedLock lock(*connection_);
		connection_->setOperation("ListLabSessionsWithAttributesXML");
		connection_->setValue("labID", labID);
		connection_->invoke();

		if(!document.Parse(connection_->xmlResponse().c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	//ListSessionTrialsXMLResponse
	TiXmlElement* sessions = hParent.FirstChild("ListLabSessionsWithAttributesXMLResponse").ToElement();
	TiXmlElement* session_details = sessions->FirstChildElement("SessionDetailsWithAttributes");
	while(session_details != nullptr) {
		motionWsdl::Session session;
		
		session_details->QueryIntAttribute("SessionID", &session.id);
		session_details->QueryIntAttribute("UserID", &session.userId);
		session_details->QueryIntAttribute("LabID", &session.labId);
		session_details->QueryStringAttribute("MotionKind", &session.motionKind);
		std::string date;
		session_details->QueryStringAttribute("SessionDate", &date);

		//TODO
		//date to DateTime
		
		session_details->QueryStringAttribute("SessionDescription", &session.sessionDescription);

		ret.push_back(session);

		session_details = session_details->NextSiblingElement();
	}

	return ret;
}

const std::vector<motionWsdl::File> MotionBasicQueriesWS::listFiles(int subjectID, const std::string& subjectType)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	std::vector<motionWsdl::File> ret;

	TiXmlDocument document;

	{
		webservices::IWSConnection::ScopedLock lock(*connection_);
		connection_->setOperation("ListFilesWithAttributesXML");
		connection_->setValue("subjectID", subjectID);
		connection_->setValue("subjectType", subjectType);
		connection_->invoke();
	
		if(!document.Parse(connection_->xmlResponse().c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	//ListSessionTrialsXMLResponse
	TiXmlElement* files = hParent.FirstChild("ListFilesWithAttributesXMLResponse").ToElement();
	TiXmlElement* file_details = files->FirstChildElement("FileDetailsWithAttributes");
	while(file_details != nullptr) {
		motionWsdl::File file;

		file_details->QueryIntAttribute("FileID", &file.id);
		file_details->QueryStringAttribute("FileName", &file.fileName);
		file_details->QueryStringAttribute("FileDescription", &file.fileDescription);		

		//TODO
		//uzupełnić brakujące składowe file lub poprawić strukturę wsdlową

		ret.push_back(file);

		file_details = file_details->NextSiblingElement();
	}

	return ret;
}

const std::vector<motionWsdl::Trial> MotionBasicQueriesWS::listSessionContents()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	std::vector<motionWsdl::Trial> ret;

	TiXmlDocument document;

	{
		webservices::IWSConnection::ScopedLock lock(*connection_);
		connection_->setOperation("ListSessionContents");
		connection_->setValue("pageSize", 1);
		connection_->setValue("pageNo", 1);
		connection_->invoke();

		if(!document.Parse(connection_->xmlResponse().c_str())) {
			UTILS_ASSERT(false, "Blad parsowania odpowiedzi");
		}
	}

	TiXmlHandle hDocument(&document);
	TiXmlElement* _element;
	TiXmlHandle hParent(0);

	_element = hDocument.FirstChildElement().Element();
	if(!_element) {
		UTILS_ASSERT(false, "Bledna struktura odpowiedzi");
	}
	hParent = TiXmlHandle(_element);

	//ListSessionTrialsXMLResponse
	TiXmlElement* sessions = hParent.FirstChild("ListSessionContentsResponse").ToElement();
	TiXmlElement* session_content = sessions->FirstChildElement("SessionContent");
	while(session_content != nullptr) {
		int sessionId = 0;
		session_content->QueryIntAttribute("SessionID", &sessionId);
		
		TiXmlElement* trial_content = session_content->FirstChildElement("TrialContent");
		while(trial_content) {
			motionWsdl::Trial trial;
			trial.sessionId = sessionId;

			trial_content->QueryIntAttribute("TrialID", &trial.id);
			trial_content->QueryStringAttribute("TrialName", &trial.trialDescription);
			
			TiXmlElement* file_content = trial_content->FirstChildElement("FileDetailsWithAttributes");
			while(file_content != nullptr) {
				int fileID = 0;

				file_content->QueryIntAttribute("FileID", &fileID);

				trial.trialFiles.push_back(fileID);

				file_content = file_content->NextSiblingElement();
			}

			ret.push_back(trial);

			trial_content = trial_content->NextSiblingElement();
		}

		session_content = session_content->NextSiblingElement();
	}

	return ret;
}
*/

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
