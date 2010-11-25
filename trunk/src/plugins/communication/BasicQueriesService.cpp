/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/BasicQueriesService.h>

using namespace communication;

BasicQueriesService::BasicQueriesService() { }

BasicQueriesService::~BasicQueriesService() { }

std::map<int, Trial> BasicQueriesService::listSessionTrials(int sessionID) {
	this->setOperation("ListSessionTrialsXML");
	std::map<int, Trial> trials;
	if(invoker.status()) {
		if(!invoker.setValue("sessionID", WSDL_Wsdlpull::toString<int>(sessionID))) {
			throw std::runtime_error(invoker.errors().c_str());
		}
		if(!invoker.invoke()) {
			throw invoker.getXMLResponse();
		}
		Schema::TypeContainer* tc = NULL;
		tc = invoker.getOutput("ListSessionTrialsXMLResponse");
		if(!tc) {
			throw std::runtime_error("Fail to get output.");
		}
		tc = tc->getChildContainer("ListSessionTrialsXMLResult");
		if(!tc) {
			throw std::runtime_error("Fail to get output.");
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
				throw std::runtime_error("Bad document structure format.");
			}
			temp = TrialDetails->getAttributeContainer("SessionID");
			if(temp) {
				trial.sessionID = *((int*)temp->getValue());
			}
			else
			{
				throw std::runtime_error("Bad document structure format.");
			}
			temp = TrialDetails->getAttributeContainer("TrialDescription");
			if(temp) {
				trial.trialDescription = ((std::string*)temp->getValue())->c_str();
			}
			else
			{
				throw std::runtime_error("Bad document structure format.");
			}
			TrialDetails = SessionTrialList->getChildContainer("TrialDetails");
			trials[trial.id] = trial;
		}
	} else {
		throw std::runtime_error(invoker.errors().c_str());
	}
	return trials;
}

std::map<int, Session> BasicQueriesService::listLabSessionsWithAttributes(int labID) {
	this->setOperation("ListLabSessionsWithAttributesXML");
	this->setValue("labID", WSDL_Wsdlpull::toString<int>(labID));
	this->invokeOperation();

	std::map<int, Session> sessions;
	Schema::TypeContainer* tc = NULL;
	tc = invoker.getOutput("ListLabSessionsWithAttributesXMLResponse");
	if(!tc)
	{
		throw std::runtime_error("Fail to get output.");
	}
	TypeContainer* sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
	while(sessionDetails)
	{
		Session session;
		void* temp;
		Schema::Type type;
		temp = sessionDetails->getValue("SessionID", type);
		if(temp)
		{
			session.id = *((int*)temp);
		}
		else
		{
			throw std::runtime_error("Bad document structure format: ID.");
		}
		temp = sessionDetails->getValue("UserID", type);
		if(temp)
		{
			session.userID = *((int*)temp);
		}
		else
		{
			throw std::runtime_error("Bad document structure format: user ID.");
		}
		temp = sessionDetails->getValue("LabID", type);
		if(temp)
		{
			session.labID = *((int*)temp);
		}
		else
		{
			throw std::runtime_error("Bad document structure format: lab ID.");
		}
		temp = sessionDetails->getValue("MotionKind", type);
		if(temp)
		{
			session.motionKind = *((std::string*)temp);
		}
		else
		{
			throw std::runtime_error("Bad document structure format: Motion kind.");
		}
		temp = sessionDetails->getValue("SessionDate", type);
		if(temp)
		{
			session.sessionDate.setDate(*((std::string*)temp));
		}
		else
		{
			throw std::runtime_error("Bad document structure format: session date.");
		}
		temp = sessionDetails->getValue("SessionDescription", type);
		if(temp)
		{
			session.sessionDescription = ((std::string*)temp)->c_str();
		}
		else
		{
			throw std::runtime_error("Bad document structure format: session description.");
		}
		sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
		sessions[session.id] = session;
	}
	return sessions;
}

std::map<int, File> BasicQueriesService::listFiles(int ID, const std::string& subjectType) {
	this->setOperation("ListFilesWithAttributesXML");
	this->setValue("subjectID", WSDL_Wsdlpull::toString<int>(ID));
	this->setValue("subjectType", subjectType);
	this->invokeOperation();

	std::map<int, File> files;
	Schema::TypeContainer* tc = NULL;
	tc = invoker.getOutput("ListFilesWithAttributesXMLResponse");
	if(!tc)
	{
		throw std::runtime_error("Fail to get output.");
	}
	TypeContainer* fileDetails = tc->getChildContainer("FileDetailsWithAttributes");
	while(fileDetails)
	{
		File file;
		TypeContainer* temp;
		temp = fileDetails->getAttributeContainer("FileID");
		if(temp)
		{
			file.id = *((int*)temp->getValue());
		}
		else
		{
			throw std::runtime_error("Bad document structure format.");
		}
		temp = fileDetails->getAttributeContainer("FileName");
		if(temp)
		{
			file.fileName = *((std::string*)temp->getValue());
		}
		else
		{
			throw std::runtime_error("Bad document structure format.");
		}
		temp = fileDetails->getAttributeContainer("FileDescription");
		if(temp)
		{
			file.fileDescription = *((std::string*)temp->getValue());
		}
		else
		{
			throw std::runtime_error("Bad document structure format.");
		}
		fileDetails = tc->getChildContainer("FileDetailsWithAttributes");
		files[file.id] = file;
	}
	return files;
}

void BasicQueriesService::listSessionContents()
{
	int id = 0;
	this->setOperation("ListSessionContents");
	this->setValue("pageSize", WSDL_Wsdlpull::toString<int>(id));
	this->setValue("pageNo", WSDL_Wsdlpull::toString<int>(id));
	this->invokeOperation();
}

//TODO: A Schema Parser runtime_error occurred while parsing the response at line 1:444
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
//		//	throw std::runtime_error(invoker.errors().c_str());
//		//}
//		//if(!invoker.invoke()) {
//		//	throw std::runtime_error(invoker.errors().c_str());
//		//}
//		Schema::Type type;
//		std::string* name = ((std::string*)invoker.getValue("FirstName", type));
//		std::string* surname = ((std::string*)invoker.getValue("LastName", type));
//		if(name != NULL && surname != NULL) {
//			return new Performer(performerID, name->c_str(), surname->c_str());
//		} else {
//			throw std::runtime_error("Bad operation return values.");
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
////A Schema Parser runtime_error occurred while parsing the response at line 1:438
////Schema Parser Exception : Could not find element SessionDetailsWithAttributes in
////GetSessionByIdXMLResponse
////Error validating schema instance
//const Session* BasicQueriesService::getSessionById(int sessionID) {
//	this->setOperation("GetSessionByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(sessionID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
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
//			session.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new Session();
//}
//
////A Schema Parser runtime_error occurred while parsing the response at line 1:438
////Schema Parser Exception : Could not find element SegmentDetailsWithAttributes in
////GetSegmentByIdXMLResponse
////Error validating schema instance
//const Segment* BasicQueriesService::getSegmentById(int segmentID) {
//	this->setOperation("GetSegmentByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(segmentID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		/*Schema::Type type;
//		std::string* temp = ((std::string*)invoker.getValue("FirstName", type));
//		if(temp != NULL) {
//			session.setName(temp->c_str());
//		}
//		temp = ((std::string*)invoker.getValue("LastName", type));
//		if(temp != NULL) {
//			session.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new Segment();
//}
//
////A Schema Parser runtime_error occurred while parsing the response at line 1:432
////Schema Parser Exception : Could not find element TrialDetailsWithAttributes in
////GetTrialByIdXMLResponse
////Error validating schema instance
//const Trial* BasicQueriesService::getTrialById(int trialID) {
//	this->setOperation("GetTrialByIdXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("id", WSDL_Wsdlpull::toString<int>(trialID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		/*Schema::Type type;
//		std::string* temp = ((std::string*)invoker.getValue("FirstName", type));
//		if(temp != NULL) {
//			session.setName(temp->c_str());
//		}
//		temp = ((std::string*)invoker.getValue("LastName", type));
//		if(temp != NULL) {
//			session.setSurname(temp->c_str());
//		}*/
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new Trial();
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessions(int performerID) {
//	this->setOperation("ListPerformerSessionsXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", WSDL_Wsdlpull::toString<int>(performerID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		std::vector<Session>* sessions = new std::vector<Session>();
//		Schema::TypeContainer* tc = NULL;
//		tc = invoker.getOutput("ListPerformerSessionsXMLResponse");
//		if(!tc) {
//			throw std::runtime_error("Fail to get output.");
//		}
//		tc = tc->getChildContainer("ListPerformerSessionsXMLResult");
//		if(!tc) {
//			throw std::runtime_error("Fail to get output.");
//		}
//		TypeContainer* performerSessionList = tc->getChildContainer("PerformerSessionList");
//		TypeContainer* sessionDetails = performerSessionList->getChildContainer("SessionDetails");
//		while(sessionDetails) {
//			Session session;
//			TypeContainer* temp;
//			temp = sessionDetails->getAttributeContainer("SessionID");
//			if(temp) {
//				session.setID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw std::runtime_error("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("MotionKindID");
//			if(temp) {
//				session.setmotionKindID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw std::runtime_error("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("PerformerID");
//			if(temp) {
//				session.setPerformerID(*((int*)temp->getValue()));
//			}
//			else
//			{
//				throw std::runtime_error("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("SessionDate");
//			if(temp) {
//				session.setSessionDate(DateTime(*((std::string*)temp->getValue())));
//			}
//			else
//			{
//				throw std::runtime_error("Bad document structure format.");
//			}
//			temp = sessionDetails->getAttributeContainer("SessionDescription");
//			if(temp) {
//				session.setSessionDescription(((std::string*)temp->getValue())->c_str());
//			}
//			else
//			{
//				throw std::runtime_error("Bad document structure format.");
//			}
//			sessionDetails = performerSessionList->getChildContainer("SessionDetails");
//			sessions->push_back(session);
//		}
//		return sessions;
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessionsWithAttributes(int performerID) {
//	throw std::runtime_error("not supported yet.");
//}
//
//const std::vector<Session>* BasicQueriesService::listPerformerSessionsXML(int performerID) {
//	this->setOperation("ListPerformerSessionsXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("performerID", WSDL_Wsdlpull::toString<int>(performerID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		std::vector<Session>* sessions = new std::vector<Session>();
//		Schema::TypeContainer* tc = NULL;
//		tc = invoker.getOutput("ListPerformerSessionsXMLResponse");
//		if(!tc) {
//			throw std::runtime_error("Fail to get output.");
//		}
//
//		tc = tc->getChildContainer("ListPerformerSessionsXMLResult");
//		if(!tc) {
//			throw std::runtime_error("Fail to get output.");
//		}
//		TypeContainer* plainSession = tc->getChildContainer("PlainSessionDetails");
//		while(plainSession) {
//			TypeContainer* temp;
//			Session session;
//			temp = plainSession->getChildContainer("SessionID");
//			if(temp) {
//				session.setID(*((int*)temp->getValue()));
//			}
//			temp = plainSession->getChildContainer("PerformerID");
//			if(temp) {
//				session.setPerformerID(*((int*)temp->getValue()));
//			}
//			temp = plainSession->getChildContainer("SessionDate");
//			if(temp) {
//				session.setSessionDate(DateTime(*((std::string*)temp->getValue())));
//			}
//			temp = plainSession->getChildContainer("SessionDescription");
//			if(temp) {
//				session.setSessionDescription(((std::string*)temp->getValue())->c_str());
//			}
//			plainSession = tc->getChildContainer("PlainSessionDetails");
//			sessions->push_back(session);
//		}
//		return sessions;
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Performer>* BasicQueriesService::listPerformers() {
//	this->setOperation("ListPerformersXML");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		std::vector<Performer>* sessions = new std::vector<Performer>();
//		return sessions;
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
//
//const std::vector<Performer>* BasicQueriesService::listPerformersWithAttributes() {
//	this->setOperation("ListPerformersWithAttributesXML");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new std::vector<Performer>();
//}

//const std::vector<Trial>* BasicQueriesService::listSessionTrialsWithAttributes(int sessionID) {
//	throw std::runtime_error("not supported yet.");
//}
//
//const std::vector<Segment>* BasicQueriesService::listTrialSegments(int trialID) {
//	throw std::runtime_error("not supported yet.");
//}
//
//const std::vector<Segment>* BasicQueriesService::listTrialSegmentsWithAttributes(int trialID) {
//	throw std::runtime_error("not supported yet.");
//}
//
//const std::vector<Performer>* BasicQueriesService::listLabPerformersWithAttributes(int labID) {
//	this->setOperation("ListLabPerformersWithAttributesXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("labID", WSDL_Wsdlpull::toString<int>(labID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new std::vector<Performer>();
//}

//const std::vector<File>* BasicQueriesService::listSessionFiles(int sessionID) {
//	this->setOperation("ListSessionFiles");
//	if(invoker.status()) {
//		if(!invoker.setValue("sessionID", WSDL_Wsdlpull::toString<int>(sessionID))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new std::vector<File>();
//}

//const std::vector<File>* BasicQueriesService::listFilesWithAttributes(int ID, const std::string& subjectType) {
//	throw std::runtime_error("not supported yet.");
//}
//
//const std::vector<SessionGroup>* BasicQueriesService::listSessionGroupsDefined() {
//	this->setOperation("ListSessionGroupsDefined");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new std::vector<SessionGroup>();
//}
//
//const std::vector<MotionKind>* BasicQueriesService::listMotionKindsDefined() {
//	this->setOperation("ListMotionKindsDefined");
//	if(invoker.status()) {
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//	return new std::vector<MotionKind>();
//}
//
//void BasicQueriesService::performQuery(const std::string& query) {
//	this->setOperation("GenericQueryXML");
//	if(invoker.status()) {
//		if(!invoker.setValue("query", new std::string(query))) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//		if(!invoker.invoke()) {
//			throw std::runtime_error(invoker.errors().c_str());
//		}
//	} else {
//		throw std::runtime_error(invoker.errors().c_str());
//	}
//}
