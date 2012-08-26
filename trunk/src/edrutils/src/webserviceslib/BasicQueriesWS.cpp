#include <webserviceslib/BasicQueriesWS.h>
#include <webserviceslib/DateTimeUtils.h>
#include <utils/Debug.h>
#include <tinyxml.h>

namespace webservices
{

GeneralBasicQueriesWS::GeneralBasicQueriesWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection)
{
	mutex = this;
}

GeneralBasicQueriesWS::~GeneralBasicQueriesWS()
{

}

void GeneralBasicQueriesWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & GeneralBasicQueriesWS::connection()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return connection_;
}

const WSConnectionConstPtr & GeneralBasicQueriesWS::connection() const
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return constConnection_;
}

const DateTime GeneralBasicQueriesWS::dataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("GetDBTimestamp");
	connection_->invoke(true);
	std::string time;
	connection_->getValue("GetDBTimestampResult", time);

	return toTime(time);
}

const DateTime GeneralBasicQueriesWS::metadataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	webservices::IWSConnection::ScopedLock lockConn(*connection_);
	connection_->setOperation("GetMetadataTimestamp");
	connection_->invoke(true);
	std::string time;
	connection_->getValue("GetMetadataTimestampResult", time);

	return toTime(time);
}

MotionBasicQueriesWS::MotionBasicQueriesWS(const WSConnectionPtr & connection) : connection_(connection), constConnection_(connection), genBasicQueries(connection)
{
	mutex = this;
}

MotionBasicQueriesWS::~MotionBasicQueriesWS()
{

}

void MotionBasicQueriesWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	genBasicQueries.setConnection(connection);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & MotionBasicQueriesWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MotionBasicQueriesWS::connection() const
{
	return constConnection_;
}

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
		//uzupełnić brakujące skłądowe file lub poprawić strukturę wsdlową

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

			
			/*temp = TrialContent->getChildContainer("TrialName");
			if(temp) {
				std::string trialName;
				trialName = *((std::string*)temp->getValue());
				if(trialName.empty()) {
					temp = TrialContent->getChildContainer("Attribute");
					if(temp) {
						temp = temp->getAttributeContainer("Value");
						if(temp) {
							trialName = *((std::string*)temp->getValue());
						} else {
							LOG_DEBUG("Value " << trial.id);
							throw std::runtime_error("Bad document structure format. There is no TrialName.");
						}
					} else {
						throw std::runtime_error("Bad document structure format. There is no Attribute when TrialName is empty.");
					}
				}
				trial.trialDescription = trialName;
			} else {
				LOG_DEBUG("Attribute " << trial.id);
				throw std::runtime_error("Bad document structure format. There is no TrialName.");
			}*/
			
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

const DateTime MotionBasicQueriesWS::dataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    return genBasicQueries.dataModificationTime();
}

const DateTime MotionBasicQueriesWS::metadataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
    return genBasicQueries.metadataModificationTime();
}


MedicalBasicQueriesWS::MedicalBasicQueriesWS(const WSConnectionPtr & connection)
	: connection_(connection), constConnection_(connection), genBasicQueries(connection)
{
	mutex = this;
}

MedicalBasicQueriesWS::~MedicalBasicQueriesWS()
{

}

void MedicalBasicQueriesWS::setConnection(const WSConnectionPtr & connection)
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	genBasicQueries.setConnection(connection);
	connection_ = connection;
	constConnection_ = connection;
}

const WSConnectionPtr & MedicalBasicQueriesWS::connection()
{
	return connection_;
}

const WSConnectionConstPtr & MedicalBasicQueriesWS::connection() const
{
	return constConnection_;
}

const DateTime MedicalBasicQueriesWS::dataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return genBasicQueries.dataModificationTime();
}

const DateTime MedicalBasicQueriesWS::metadataModificationTime()
{
	OpenThreads::ScopedLock<OpenThreads::ReentrantMutex> lock(*mutex);
	return genBasicQueries.metadataModificationTime();
}

}
