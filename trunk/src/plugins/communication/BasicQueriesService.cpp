/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/BasicQueriesService.h>

using namespace communication;

BasicQueriesService::BasicQueriesService() { }

BasicQueriesService::~BasicQueriesService() { }

//TODO: A Schema Parser exception occurred while parsing the response at line 1:444
//Schema Parser Exception : Could not find element PerformerDetailsWithAttributes
//in GetPerformerByIdXMLResponse
//Error validating schema instance
//const Performer* BasicQueriesService::getPerformerById(int performerID) {
//	this->setOperation("GetPerformerByIdXML");
//	this->setValue("id", WSDL_Wsdlpull::toString<int>(performerID));
//	this->invokeOperation();
//
//	if(invoker.status()) {
//		//if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(performerID))) {
//		//	throw EDRException(invoker.errors().c_str());
//		//}
//		//if(!invoker.invoke()) {
//		//	throw EDRException(invoker.errors().c_str());
//		//}
//		Schema::Type type;
//		std::string* name = ((std::string*)invoker.getValue("FirstName", type));
//		std::string* surname = ((std::string*)invoker.getValue("LastName", type));
//		if(name != NULL && surname != NULL) {
//			return new Performer(performerID, name->c_str(), surname->c_str());
//		} else {
//			throw EDRException("Bad operation return values.");
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
////A Schema Parser exception occurred while parsing the response at line 1:438
////Schema Parser Exception : Could not find element SessionDetailsWithAttributes in
////GetSessionByIdXMLResponse
////Error validating schema instance
//const Session* BasicQueriesService::getSessionById(int sessionID) {
//	this->setOperation("GetSessionByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(sessionID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}/*
//		Schema::Type type;
//		int SessionID = invoker.getValue<int>("SessionID", type));
//		int UserID = invoker.getValue<int>("UserID", type));
//		int UserID = invoker.getValue<int>("LabID", type));
//		int motionKindName = invoker.getValue<int>("motionKindName", type));
//		int PerformerID = invoker.getValue<int>("PerformerID", type));
//		int SessionID = invoker.getValue<int>("SessionID", type));
//		int UserID = invoker.getValue<int>("UserID", type));
//		temp = ((std::string*)invoker.getValue("LastName", type));
//		if(temp != NULL) {
//			s.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new Session();
//}
//
////A Schema Parser exception occurred while parsing the response at line 1:438
////Schema Parser Exception : Could not find element SegmentDetailsWithAttributes in
////GetSegmentByIdXMLResponse
////Error validating schema instance
//const Segment* BasicQueriesService::getSegmentById(int segmentID) {
//	this->setOperation("GetSegmentByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(segmentID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		/*Schema::Type type;
//		std::string* temp = ((std::string*)invoker.getValue("FirstName", type));
//		if(temp != NULL) {
//			s.setName(temp->c_str());
//		}
//		temp = ((std::string*)invoker.getValue("LastName", type));
//		if(temp != NULL) {
//			s.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new Segment();
//}
//
////A Schema Parser exception occurred while parsing the response at line 1:432
////Schema Parser Exception : Could not find element TrialDetailsWithAttributes in
////GetTrialByIdXMLResponse
////Error validating schema instance
//const Trial* BasicQueriesService::getTrialById(int trialID) {
//	this->setOperation("GetTrialByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(trialID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		/*Schema::Type type;
//		std::string* temp = ((std::string*)invoker.getValue("FirstName", type));
//		if(temp != NULL) {
//			s.setName(temp->c_str());
//		}
//		temp = ((std::string*)invoker.getValue("LastName", type));
//		if(temp != NULL) {
//			s.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new Trial();
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessions(int performerID) {
//	this->setOperation("ListPerformerSessionsXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", WSDL_Wsdlpull::toString<int>(performerID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		std::vector<Session>* v = new std::vector<Session>();
//		Schema::TypeContainer* tc = NULL;
//		tc = invoker.getOutput("ListPerformerSessionsXMLResponse");
//		if(!tc) {
//			throw EDRException("Fail to get output.");
//		}
//		tc = tc->getChildContainer("ListPerformerSessionsXMLResult");
//		if(!tc) {
//			throw EDRException("Fail to get output.");
//		}
//		TypeContainer* performerSessionList = tc->getChildContainer("PerformerSessionList");
//		TypeContainer* sessionDetails = performerSessionList->getChildContainer("SessionDetails");
//		while(sessionDetails) {
//			Session s;
//			TypeContainer* temp;
//			temp = sessionDetails->getAttributeContainer("SessionID");
//			if(temp) {
//				s.setID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw EDRException("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("MotionKindID");
//			if(temp) {
//				s.setmotionKindID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw EDRException("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("PerformerID");
//			if(temp) {
//				s.setPerformerID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw EDRException("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("SessionDate");
//			if(temp) {
//				s.setSessionDate(DateTime(*((std::string*)temp->getValue())));
//			}
//			else
//			{
//				throw EDRException("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("SessionDescription");
//			if(temp) {
//				s.setSessionDescription(((std::string*)temp->getValue())->c_str());
//			}
//			else
//			{
//				throw EDRException("Bad document structure format.");
//			}
//			sessionDetails = performerSessionList->getChildContainer("SessionDetails");
//			v->push_back(s);
//		}
//		return v;
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessionsWithAttributes(int performerID) {
//	throw EDRException("not supported yet.");
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessionsXML(int performerID) {
//	this->setOperation("ListPerformerSessionsXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", WSDL_Wsdlpull::toString<int>(performerID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		std::vector<Session>* v = new std::vector<Session>();
//		Schema::TypeContainer* tc = NULL;
//		tc = invoker.getOutput("ListPerformerSessionsXMLResponse");
//		if(!tc) {
//			throw EDRException("Fail to get output.");
//		}
//
//		tc = tc->getChildContainer("ListPerformerSessionsXMLResult");
//		if(!tc) {
//			throw EDRException("Fail to get output.");
//		}
//		TypeContainer* plainSession = tc->getChildContainer("PlainSessionDetails");
//		while(plainSession) {
//			TypeContainer* temp;
//			Session s;
//			temp = plainSession->getChildContainer("SessionID");
//			if(temp) {
//				s.setID(*((int*)temp->getValue()));
//			}
//			temp = plainSession->getChildContainer("PerformerID");
//			if(temp) {
//				s.setPerformerID(*((int*)temp->getValue()));
//			}
//			temp = plainSession->getChildContainer("SessionDate");
//			if(temp) {
//				s.setSessionDate(DateTime(*((std::string*)temp->getValue())));
//			}
//			temp = plainSession->getChildContainer("SessionDescription");
//			if(temp) {
//				s.setSessionDescription(((std::string*)temp->getValue())->c_str());
//			}
//			plainSession = tc->getChildContainer("PlainSessionDetails");
//			v->push_back(s);
//		}
//		return v;
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Performer>* BasicQueriesService::listPerformers() {
//	this->setOperation("ListPerformersXML");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		std::vector<Performer>* v = new std::vector<Performer>();
//		return v;
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Performer>* BasicQueriesService::listPerformersWithAttributes() {
//	this->setOperation("ListPerformersWithAttributesXML");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new std::vector<Performer>();
//}

std::vector<Trial> BasicQueriesService::listSessionTrials(int sessionID) {
	this->setOperation("ListSessionTrialsXML");
	std::vector<Trial> t;
	if(invoker.status()) {
		if(!invoker.setValue("sessionID", WSDL_Wsdlpull::toString<int>(sessionID))) {
			throw EDRException(invoker.errors().c_str());
		}
		if(!invoker.invoke()) {
			throw invoker.getXMLResponse();
		}
		Schema::TypeContainer* tc = NULL;
		tc = invoker.getOutput("ListSessionTrialsXMLResponse");
		if(!tc) {
			throw EDRException("Fail to get output.");
		}
		tc = tc->getChildContainer("ListSessionTrialsXMLResult");
		if(!tc) {
			throw EDRException("Fail to get output.");
		}
		TypeContainer* SessionTrialList = tc->getChildContainer("SessionTrialList");
		TypeContainer* TrialDetails = SessionTrialList->getChildContainer("TrialDetails");
		while(TrialDetails) {
			Trial trial;
			TypeContainer* temp;
			temp = TrialDetails->getAttributeContainer("TrialID");
			if(temp) {
				trial.id = *((int*)temp->getValue());
			}
			else
			{
				throw EDRException("Bad document structure format.");
			}
			temp = TrialDetails->getAttributeContainer("SessionID");
			if(temp) {
				trial.session_id = *((int*)temp->getValue());
			}
			else
			{
				throw EDRException("Bad document structure format.");
			}
			temp = TrialDetails->getAttributeContainer("TrialDescription");
			if(temp) {
				trial.trial_description = ((std::string*)temp->getValue())->c_str();
			}
			else
			{
				throw EDRException("Bad document structure format.");
			}
			TrialDetails = SessionTrialList->getChildContainer("TrialDetails");
			t.push_back(trial);
		}
	} else {
		throw EDRException(invoker.errors().c_str());
	}
	return t;
}

//const std::vector<Trial>* BasicQueriesService::listSessionTrialsWithAttributes(int sessionID) {
//	throw EDRException("not supported yet.");
//}
//
//const std::vector<Segment>* BasicQueriesService::listTrialSegments(int trialID) {
//	throw EDRException("not supported yet.");
//}
//
//const std::vector<Segment>* BasicQueriesService::listTrialSegmentsWithAttributes(int trialID) {
//	throw EDRException("not supported yet.");
//}
//
//const std::vector<Performer>* BasicQueriesService::listLabPerformersWithAttributes(int labID) {
//	this->setOperation("ListLabPerformersWithAttributesXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("labID", WSDL_Wsdlpull::toString<int>(labID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new std::vector<Performer>();
//}

//------------------------------------
//listowanie sesji danego laboratorium
//------------------------------------
std::vector<Session> BasicQueriesService::listLabSessionsWithAttributes(int labID) {
	this->setOperation("ListLabSessionsWithAttributesXML");
	this->setValue("labID", WSDL_Wsdlpull::toString<int>(labID));
	this->invokeOperation();

	std::vector<Session> v;
	Schema::TypeContainer* tc = NULL;
	tc = invoker.getOutput("ListLabSessionsWithAttributesXMLResponse");
	if(!tc)
	{
		throw EDRException("Fail to get output.");
	}
	TypeContainer* sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
	while(sessionDetails)
	{
		Session s;
		void* temp;
		Schema::Type type;
		temp = sessionDetails->getValue("SessionID", type);
		if(temp)
		{
			s.id = *((int*)temp);
		}
		else
		{
			throw EDRException("Bad document structure format: ID.");
		}
		temp = sessionDetails->getValue("MotionKind", type);
		if(temp)
		{
			s.motion_kind = *((std::string*)temp);
		}
		else
		{
			throw EDRException("Bad document structure format: Motion kind.");
		}
		temp = sessionDetails->getValue("SessionDate", type);
		if(temp)
		{
			s.session_date.setDate(*((std::string*)temp));
		}
		else
		{
			throw EDRException("Bad document structure format: session date.");
		}
		temp = sessionDetails->getValue("SessionDescription", type);
		if(temp)
		{
			s.session_description = ((std::string*)temp)->c_str();
		}
		else
		{
			throw EDRException("Bad document structure format: session description.");
		}
		sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
		v.push_back(s);
	}
	return v;
}

//const std::vector<File>* BasicQueriesService::listSessionFiles(int sessionID) {
//	this->setOperation("ListSessionFiles");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", WSDL_Wsdlpull::toString<int>(sessionID))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new std::vector<File>();
//}

std::vector<File> BasicQueriesService::listFiles(int ID, const std::string& subjectType) {
	this->setOperation("ListFilesWithAttributesXML");
	this->setValue("subjectID", WSDL_Wsdlpull::toString<int>(ID));
	this->setValue("subjectType", subjectType);
	this->invokeOperation();

	std::vector<File> f;
	Schema::TypeContainer* tc = NULL;
	tc = invoker.getOutput("ListFilesWithAttributesXMLResponse");
	if(!tc)
	{
		throw EDRException("Fail to get output.");
	}
	tc = tc->getChildContainer("ListFilesWithAttributesXMLResult");
	if(!tc)
	{
		throw EDRException("Fail to get output.");
	}
	TypeContainer* FileList = tc->getChildContainer("FileWithAttributesList");
	TypeContainer* FileDetails = FileList->getChildContainer("FileDetailsWithAttributes");
	while(FileDetails)
	{
		File file;
		TypeContainer* temp;
		temp = FileDetails->getAttributeContainer("FileID");
		if(temp)
		{
			file.id = *((int*)temp->getValue());
		}
		else
		{
			throw EDRException("Bad document structure format.");
		}
		temp = FileDetails->getAttributeContainer("FileName");
		if(temp)
		{
			file.file_name = *((std::string*)temp->getValue());
		}
		else
		{
			throw EDRException("Bad document structure format.");
		}
		temp = FileDetails->getAttributeContainer("FileDescription");
		if(temp)
		{
			file.file_description = *((std::string*)temp->getValue());
		}
		else
		{
			throw EDRException("Bad document structure format.");
		}
		FileDetails = FileList->getChildContainer("FileDetails");
		f.push_back(file);
	}
	return f;
}

//const std::vector<File>* BasicQueriesService::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	throw EDRException("not supported yet.");
//}
//
//const std::vector<SessionGroup>* BasicQueriesService::listSessionGroupsDefined() {
//	this->setOperation("ListSessionGroupsDefined");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new std::vector<SessionGroup>();
//}
//
//const std::vector<MotionKind>* BasicQueriesService::listMotionKindsDefined() {
//	this->setOperation("ListMotionKindsDefined");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//	return new std::vector<MotionKind>();
//}
//
//void BasicQueriesService::performQuery(const std::string& query) {
//	this->setOperation("GenericQueryXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("query", new std::string(query))) {
//			throw EDRException(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw EDRException(invoker.errors().c_str());
//		}
//	} else {
//		throw EDRException(invoker.errors().c_str());
//	}
//}
