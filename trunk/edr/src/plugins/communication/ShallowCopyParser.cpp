#include "CommunicationPCH.h"
#include <plugins/communication/ShallowCopyParser.h>
#include <core/IDataManager.h>

ShallowCopyParser::ShallowCopyParser()
{
    object = core::ObjectWrapper::create<ShallowCopy::ShallowCopy>();
    object->set(ShallowCopy::ShallowCopyPtr(new ShallowCopy::ShallowCopy()));
}

ShallowCopyParser::~ShallowCopyParser()
{
}

void ShallowCopyParser::parseFile(core::IDataManager* dataManager, const boost::filesystem::path& path)
{
    this->path = path;

    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku ShallowCopy");
    }
    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku ShallowCopy");
    }
    hParent = TiXmlHandle(_element);

    //Sessions
    TiXmlElement* sessions_element = hParent.FirstChild("Sessions").ToElement();
    if(sessions_element) {
        TiXmlElement* session_element = sessions_element->FirstChildElement("Session");
        while(session_element) {
            ShallowCopy::Session session;
            session_element->QueryIntAttribute("SessionID", &session.sessionID);
            session_element->QueryIntAttribute("UserID", &session.userID);
            session_element->QueryIntAttribute("LabID", &session.labID);
            session_element->QueryStringAttribute("MotionKind", &session.motionKind);
            session_element->QueryStringAttribute("SessionDate", &session.sessionDate);
            session_element->QueryStringAttribute("SessionName", &session.sessionName);
            session_element->QueryStringAttribute("Tags", &session.tags);
            session_element->QueryStringAttribute("SessionDescription", &session.sessionDescription);

            //Attrs
            TiXmlElement* attrs_element = session_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    session.attrs.push_back(attr);
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            object->get<ShallowCopy::ShallowCopy>()->sessions.push_back(session);
            session_element = session_element->NextSiblingElement();
        }
    }
    //GroupAssignments
    TiXmlElement* group_assignments_element = hParent.FirstChild("GroupAssignments").ToElement();
    if(sessions_element) {
        TiXmlElement* group_assignment_element = group_assignments_element->FirstChildElement("GroupAssignment");
        while(group_assignment_element) {
            ShallowCopy::GroupAssigment group_assignment;
            group_assignment_element->QueryIntAttribute("SessionID", &group_assignment.sessionID);
            group_assignment_element->QueryIntAttribute("SessionGroupID", &group_assignment.sessionGroupID);
            object->get<ShallowCopy::ShallowCopy>()->groupAssigments.push_back(group_assignment);
            group_assignment_element = group_assignment_element->NextSiblingElement();
        }
    }
    //Trials
    TiXmlElement* trials_element = hParent.FirstChild("Trials").ToElement();
    if(trials_element) {
        TiXmlElement* trial_element = trials_element->FirstChildElement("Trial");
        while(trial_element) {
            ShallowCopy::Trial trial;
            trial_element->QueryIntAttribute("TrialID", &trial.trialID);
            trial_element->QueryIntAttribute("SessionID", &trial.sessionID);
            trial_element->QueryStringAttribute("TrialName", &trial.trialName);
            trial_element->QueryStringAttribute("TrialDescription", &trial.trialDescription);

            //Attrs
            TiXmlElement* attrs_element = trial_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    trial.attrs.push_back(attr);
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            

            //Files
            TiXmlElement* files_element = trial_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    ShallowCopy::File file;
                    file_element->QueryIntAttribute("FileID", &file.fileID);
                    file_element->QueryStringAttribute("FileName", &file.fileName);
                    file_element->QueryStringAttribute("FileDescription", &file.fileDescription);
                    file_element->QueryStringAttribute("SubdirPath", &file.subdirPath);
                    trial.files.push_back(file);
                    file_element = file_element->NextSiblingElement();
                }
            }

            object->get<ShallowCopy::ShallowCopy>()->trials.push_back(trial);
            trial_element = trial_element->NextSiblingElement();
        }
    }
    //Performers
    TiXmlElement* performers_element = hParent.FirstChild("Performers").ToElement();
    if(performers_element) {
        TiXmlElement* performer_element = performers_element->FirstChildElement("Performer");
        while(performer_element) {
            ShallowCopy::Performer performer;
            performer_element->QueryIntAttribute("PerformerID", &performer.performerID);
            performer_element->QueryStringAttribute("FirstName", &performer.firstName);
            performer_element->QueryStringAttribute("LastName", &performer.lastName);

            //Attrs
            TiXmlElement* attrs_element = performer_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    performer.attrs.push_back(attr);
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            object->get<ShallowCopy::ShallowCopy>()->performers.push_back(performer);
            performer_element = performer_element->NextSiblingElement();
        }
    }
    //PerformerConfs
    TiXmlElement* performer_consfs_element = hParent.FirstChild("PerformerConfs").ToElement();
    if(performer_consfs_element) {
        TiXmlElement* performer_consf_element = performer_consfs_element->FirstChildElement("PerformerConf");
        while(performer_consf_element) {
            ShallowCopy::PerformerConf performerConf;
            performer_consf_element->QueryIntAttribute("PerformerConfID", &performerConf.performerConfID);
            performer_consf_element->QueryIntAttribute("SessionID", &performerConf.sessionID);
            performer_consf_element->QueryIntAttribute("PerformerID", &performerConf.performerID);

            //Attrs
            TiXmlElement* attrs_element = performer_consf_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    performerConf.attrs.push_back(attr);
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            object->get<ShallowCopy::ShallowCopy>()->performerConfs.push_back(performerConf);
            performer_consf_element = performer_consf_element->NextSiblingElement();
        }
    }
    LOG_INFO("shallow copy parsed.");
}

core::IParser* ShallowCopyParser::create()
{
    return new ShallowCopyParser();
}

std::string ShallowCopyParser::getSupportedExtensions() const
{
    return "xml";
}

const ShallowCopy::ShallowCopy& ShallowCopyParser::getShallowCopy() const
{
    return *object->get<ShallowCopy::ShallowCopy>();
}

void ShallowCopyParser::getObjects(std::vector<core::ObjectWrapperPtr>& objects)
{
    objects.push_back(object);
}