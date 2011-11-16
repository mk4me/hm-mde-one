/**
@author Marek Daniluk
*/

#include "CommunicationPCH.h"
#include <plugins/communication/BasicQueriesService.h>
#include <boost/tokenizer.hpp>

using namespace communication;

GeneralBasicQueriesService::GeneralBasicQueriesService()
{
}

GeneralBasicQueriesService::~GeneralBasicQueriesService()
{
}

DateTime GeneralBasicQueriesService::dbTimeFormatExtractor(const std::string & time)
{
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    static boost::char_separator<char> sep("-T:");

    DateTime ret;

    //musimy poci¹æ datê na kawa³ki        
    tokenizer tokens(time, sep);
    tokenizer::iterator it = tokens.begin();
    ret.setYear(boost::lexical_cast<int>(*it)); it++;
    ret.setMonth(boost::lexical_cast<int>(*it)); it++;
    ret.setDay(boost::lexical_cast<int>(*it)); it++;
    ret.setHour(boost::lexical_cast<int>(*it)); it++;
    ret.setMinutes(boost::lexical_cast<int>(*it)); it++;
    ret.setSeconds(boost::lexical_cast<int>(*it));

    return ret;
}

DateTime GeneralBasicQueriesService::getLastDBModificationTime()
{
    this->setOperation("GetDBTimestamp");
    if(invoker->status()) {
        
        if(!invoker->invoke()) {
            throw invoker->getXMLResponse();
        }

        Schema::Type type;
        void *val = invoker->getValue("GetDBTimestampResult", type);
        if(val == nullptr) {
            throw std::runtime_error("Bad response in GetDBTimestamp operation.");
        }

        return dbTimeFormatExtractor(*(std::string*)val);

    } else {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

DateTime GeneralBasicQueriesService::getLastMetadataModificationTime()
{
    this->setOperation("GetMetadataTimestamp");
    if(invoker->status()) {

        if(!invoker->invoke()) {
            throw invoker->getXMLResponse();
        }

        Schema::Type type;
        void *val = invoker->getValue("GetMetadataTimestampResult", type);
        if(val == nullptr) {
            throw std::runtime_error("Bad response in GetMetadataTimestamp operation.");
        }
        
        return dbTimeFormatExtractor(*(std::string*)val);

    } else {
        throw std::runtime_error(invoker->errors().c_str());
    }
}

MotionBasicQueriesService::MotionBasicQueriesService()
{
}

MotionBasicQueriesService::~MotionBasicQueriesService()
{
}

std::vector<motionWsdl::Trial> MotionBasicQueriesService::listSessionTrials(int sessionID)
{
	this->setOperation("ListSessionTrialsXML");
	std::vector<motionWsdl::Trial> trials;
	if(invoker->status()) {
		if(!invoker->setValue("sessionID", WsdlConnection::toString<int>(sessionID))) {
			throw std::runtime_error(invoker->errors().c_str());
		}
		if(!invoker->invoke()) {
			throw invoker->getXMLResponse();
		}
		Schema::TypeContainer* tc = NULL;
		tc = invoker->getOutput("ListSessionTrialsXMLResponse");
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
			motionWsdl::Trial trial;
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
				trial.sessionId = *((int*)temp->getValue());
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
            trials.push_back(trial);
		}
	} else {
		throw std::runtime_error(invoker->errors().c_str());
	}
	return trials;
}

std::vector<motionWsdl::Session> MotionBasicQueriesService::listLabSessionsWithAttributes(int labID)
{
	this->setOperation("ListLabSessionsWithAttributesXML");
	this->setValue("labID", WsdlConnection::toString<int>(labID));
	this->invokeOperation();

	std::vector<motionWsdl::Session> sessions;
	Schema::TypeContainer* tc = NULL;
	tc = invoker->getOutput("ListLabSessionsWithAttributesXMLResponse");
	if(!tc) {
		throw std::runtime_error("Fail to get output.");
	}
	TypeContainer* sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
	while(sessionDetails) {
		motionWsdl::Session session;
		void* temp;
		Schema::Type type;
		temp = sessionDetails->getValue("SessionID", type);
		if(temp) {
			session.id = *((int*)temp);
		} else {
			throw std::runtime_error("Bad document structure format: ID.");
		}
		temp = sessionDetails->getValue("UserID", type);
		if(temp) {
			session.userId = *((int*)temp);
		} else {
			throw std::runtime_error("Bad document structure format: user ID.");
		}
		temp = sessionDetails->getValue("LabID", type);
		if(temp) {
			session.labId = *((int*)temp);
		} else {
			throw std::runtime_error("Bad document structure format: lab ID.");
		}
		temp = sessionDetails->getValue("MotionKind", type);
		if(temp) {
			session.motionKind = *((std::string*)temp);
		} else {
			throw std::runtime_error("Bad document structure format: Motion kind.");
		}
		temp = sessionDetails->getValue("SessionDate", type);
		if(temp) {
            //TODO
            //setDate nie dzia³a dla DateTime w formacie YYYY-MM-DD

			//session.sessionDate.setDate(*((std::string*)temp));
		} else {
			throw std::runtime_error("Bad document structure format: session date.");
		}
		temp = sessionDetails->getValue("SessionDescription", type);
		if(temp) {
			session.sessionDescription = ((std::string*)temp)->c_str();
		} else {
			throw std::runtime_error("Bad document structure format: session description.");
		}
		sessionDetails = tc->getChildContainer("SessionDetailsWithAttributes");
        sessions.push_back(session);
	}
	return sessions;
}

std::vector<motionWsdl::File> MotionBasicQueriesService::listFiles(int ID, const std::string& subjectType)
{
	this->setOperation("ListFilesWithAttributesXML");
	this->setValue("subjectID", WsdlConnection::toString<int>(ID));
	this->setValue("subjectType", subjectType);
	this->invokeOperation();

	std::vector<motionWsdl::File> files;
	Schema::TypeContainer* tc = NULL;
	tc = invoker->getOutput("ListFilesWithAttributesXMLResponse");
	if(!tc) {
		throw std::runtime_error("Fail to get output.");
	}
	TypeContainer* fileDetails = tc->getChildContainer("FileDetailsWithAttributes");
	while(fileDetails) {
		motionWsdl::File file;
		TypeContainer* temp;
		temp = fileDetails->getAttributeContainer("FileID");
		if(temp) {
			file.id = *((int*)temp->getValue());
		} else {
			throw std::runtime_error("Bad document structure format.");
		}
		temp = fileDetails->getAttributeContainer("FileName");
		if(temp) {
			file.fileName = *((std::string*)temp->getValue());
		} else {
			throw std::runtime_error("Bad document structure format.");
		}
		temp = fileDetails->getAttributeContainer("FileDescription");
		if(temp) {
			file.fileDescription = *((std::string*)temp->getValue());
		} else {
			throw std::runtime_error("Bad document structure format.");
		}
		fileDetails = tc->getChildContainer("FileDetailsWithAttributes");
        files.push_back(file);
	}
	return files;
}

std::vector<motionWsdl::Trial> MotionBasicQueriesService::listSessionContents()
{
	int id = 1;
	this->setOperation("ListSessionContents");
	this->setValue("pageSize", WsdlConnection::toString<int>(id));
	this->setValue("pageNo", WsdlConnection::toString<int>(id));
	this->invokeOperation();

	std::vector<motionWsdl::Trial> trials;
	Schema::TypeContainer* tc = NULL;
	tc = invoker->getOutput("ListSessionContentsResponse");
	if(!tc) {
		throw std::runtime_error("Fail to get output.");
	}
	TypeContainer* SessionContent = tc->getChildContainer("SessionContent");
	while(SessionContent) {
		int sessionId = 0;
		TypeContainer* temp;
		temp = SessionContent->getChildContainer("SessionID");
		if(temp) {
			sessionId = *((int*)temp->getValue());
		}
		TypeContainer* TrialContent = SessionContent->getChildContainer("TrialContent");
		while(TrialContent) {
			motionWsdl::Trial trial;
			trial.sessionId = sessionId;

			temp = TrialContent->getChildContainer("TrialID");
			if(temp) {
				trial.id = *((int*)temp->getValue());
			} else {
				throw std::runtime_error("Bad document structure format. There is no TrialID.");
			}
			temp = TrialContent->getChildContainer("TrialName");
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
			}
			temp = TrialContent->getChildContainer("FileDetailsWithAttributes");
			while(temp) {
				TypeContainer* temp2 = temp->getAttributeContainer("FileID");
				if(temp2) {
					trial.trialFiles.push_back(*((int*)temp2->getValue()));
				} else {
					throw std::runtime_error("Bad document structure format. There is no FileID.");
				}
				temp = TrialContent->getChildContainer("FileDetailsWithAttributes");
			}
			TrialContent = SessionContent->getChildContainer("TrialContent");
			trials.push_back(trial);
		}
		SessionContent = tc->getChildContainer("SessionContent");
	}
	return trials;
}

MedicalBasicQueriesService::MedicalBasicQueriesService()
{
}

MedicalBasicQueriesService::~MedicalBasicQueriesService()
{
}
