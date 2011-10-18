#include "CommunicationPCH.h"
#include <plugins/communication/ShallowCopyParser.h>
#include <core/IDataManager.h>

ShallowCopyParser::ShallowCopyParser() : shallowCopy(communication::ShallowCopy::ShallowCopy::create()),
    object(core::ObjectWrapper::create<communication::ShallowCopy::ShallowCopy>())
{
    constShallowCopy = shallowCopy;
    object->set(shallowCopy);
}

ShallowCopyParser::~ShallowCopyParser()
{
}

void ShallowCopyParser::parseFile(core::IDataManager* dataManager, const core::Filesystem::Path& path)
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

    //Performers
    TiXmlElement* performers_element = hParent.FirstChild("Performers").ToElement();
    if(performers_element) {
        TiXmlElement* performer_element = performers_element->FirstChildElement("Performer");
        while(performer_element) {
            //communication::ShallowCopy::Performer * performer = new communication::ShallowCopy::Performer;
            communication::ShallowCopy::Performer * performer = nullptr;
            
            int perfID;

            performer_element->QueryIntAttribute("PerformerID", &perfID);

            auto perfIT = shallowCopy->performers.find(perfID);
            if(perfIT == shallowCopy->performers.end()){
                performer = new communication::ShallowCopy::Performer;
                performer->performerID = perfID;
                shallowCopy->performers[perfID] = performer;
            }else{
                performer = perfIT->second;
            }

            //Attrs
            TiXmlElement* attrs_element = performer_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    communication::ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    
                    performer->attrs[attr.name] = attr;
                    attr_element = attr_element->NextSiblingElement();
                }
            }

            performer_element = performer_element->NextSiblingElement();
        }
    }

    //Sessions
    TiXmlElement* sessions_element = hParent.FirstChild("Sessions").ToElement();
    if(sessions_element) {
        TiXmlElement* session_element = sessions_element->FirstChildElement("Session");
        while(session_element) {
            //communication::ShallowCopy::Session * session = new communication::ShallowCopy::Session;
            communication::ShallowCopy::Session * session = nullptr;

            int sessionID;
            session_element->QueryIntAttribute("SessionID", &sessionID);

            auto sessionIT = shallowCopy->sessions.find(sessionID);
            if(sessionIT == shallowCopy->sessions.end()){
                session = new communication::ShallowCopy::Session;
                session->sessionID = sessionID;
                shallowCopy->sessions[sessionID] = session;
            }else{
                session = sessionIT->second;
            }

            session->performerConf = nullptr;

            session_element->QueryIntAttribute("UserID", &session->userID);
            session_element->QueryIntAttribute("LabID", &session->labID);
            session_element->QueryStringAttribute("MotionKind", &session->motionKind);
            session_element->QueryStringAttribute("SessionDate", &session->sessionDate);
            session_element->QueryStringAttribute("SessionName", &session->sessionName);
            session_element->QueryStringAttribute("Tags", &session->tags);
            session_element->QueryStringAttribute("SessionDescription", &session->sessionDescription);

            //Attrs
            TiXmlElement* attrs_element = session_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    communication::ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    
                    session->attrs[attr.name]  = attr;
                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Session files
            TiXmlElement* files_element = session_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //communication::ShallowCopy::File * file = new communication::ShallowCopy::File;
                    communication::ShallowCopy::File * file = nullptr;

                    int fileID;

                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy->files.find(fileID);

                    if(fileIT == shallowCopy->files.end()){
                        file = new communication::ShallowCopy::File;
                        file->fileID = fileID;
                        shallowCopy->files[fileID] = file;
                    }else{
                        file = fileIT->second;
                    }
                    
                    file->trial = nullptr;
                    file->session = session;

                    
                    file_element->QueryStringAttribute("FileName", &file->fileName);
                    file_element->QueryStringAttribute("FileDescription", &file->fileDescription);
                    file_element->QueryStringAttribute("SubdirPath", &file->subdirPath);
                    
                    session->files[file->fileID] = file;
                    
                    file_element = file_element->NextSiblingElement();
                }
            }

            session_element = session_element->NextSiblingElement();
        }
    }
    //GroupAssignments
    TiXmlElement* group_assignments_element = hParent.FirstChild("GroupAssignments").ToElement();
    if(group_assignments_element) {
        TiXmlElement* group_assignment_element = group_assignments_element->FirstChildElement("GroupAssignment");
        while(group_assignment_element) {
            communication::ShallowCopy::GroupAssigment group_assignment ;
            int sessionID;
            group_assignment_element->QueryIntAttribute("SessionID", &sessionID);
            group_assignment.sessions[sessionID] = shallowCopy->sessions[sessionID];

            UTILS_ASSERT(group_assignment.sessions[sessionID] != nullptr);

            group_assignment_element->QueryIntAttribute("SessionGroupID", &group_assignment.sessionGroupID);
            
            shallowCopy->groupAssigments.push_back(group_assignment);
            group_assignment_element = group_assignment_element->NextSiblingElement();
        }
    }
    //Trials
    TiXmlElement* trials_element = hParent.FirstChild("Trials").ToElement();
    if(trials_element) {
        TiXmlElement* trial_element = trials_element->FirstChildElement("Trial");
        while(trial_element) {
            //communication::ShallowCopy::Trial * trial = new communication::ShallowCopy::Trial;
            communication::ShallowCopy::Trial * trial = nullptr;
            
            int sessionID;
            int trialID;
            trial_element->QueryIntAttribute("TrialID", &trialID);

            auto trialIT = shallowCopy->trials.find(trialID);

            if(trialIT == shallowCopy->trials.end()){
                trial = new communication::ShallowCopy::Trial;
                trial->trialID = trialID;
                shallowCopy->trials[trialID] = trial;
            }else{
                trial = trialIT->second;
            }
            
            trial_element->QueryIntAttribute("SessionID", &sessionID);
            trial->session = shallowCopy->sessions[sessionID];

            UTILS_ASSERT(trial->session != nullptr);

            trial->session->trials[trial->trialID] = trial;
            
            trial_element->QueryStringAttribute("TrialName", &trial->trialName);
            trial_element->QueryStringAttribute("TrialDescription", &trial->trialDescription);

            //Attrs
            TiXmlElement* attrs_element = trial_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    communication::ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    
                    trial->attrs[attr.name] = attr;
                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Files
            TiXmlElement* files_element = trial_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //communication::ShallowCopy::File * file = new communication::ShallowCopy::File;
                    communication::ShallowCopy::File * file = nullptr;
                    int fileID;
                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy->files.find(fileID);

                    if(fileIT == shallowCopy->files.end()){
                        file = new communication::ShallowCopy::File;
                        file->fileID = fileID;
                        shallowCopy->files[fileID] = file;
                    }else{
                        file = fileIT->second;
                    }

                    file->trial = trial;
                    file->session = trial->session;

                    
                    file_element->QueryStringAttribute("FileName", &file->fileName);
                    file_element->QueryStringAttribute("FileDescription", &file->fileDescription);
                    file_element->QueryStringAttribute("SubdirPath", &file->subdirPath);
                    
                    trial->files[file->fileID] = file;
                    file_element = file_element->NextSiblingElement();
                }
            }

            trial_element = trial_element->NextSiblingElement();
        }
    }
    
    //PerformerConfs
    TiXmlElement* performer_consfs_element = hParent.FirstChild("PerformerConfs").ToElement();
    if(performer_consfs_element) {
        TiXmlElement* performer_consf_element = performer_consfs_element->FirstChildElement("PerformerConf");
        while(performer_consf_element) {
            //communication::ShallowCopy::PerformerConf * performerConf = new communication::ShallowCopy::PerformerConf;
            communication::ShallowCopy::PerformerConf * performerConf = nullptr;

            int perfConfID;
            performer_consf_element->QueryIntAttribute("PerformerConfID", &perfConfID);

            auto perfConfIT = shallowCopy->performerConfs.find(perfConfID);

            if(perfConfIT == shallowCopy->performerConfs.end()){
                performerConf = new communication::ShallowCopy::PerformerConf;
                performerConf->performerConfID = perfConfID;
                shallowCopy->performerConfs[performerConf->performerConfID] = performerConf;
            }else{
                performerConf = perfConfIT->second;
            }

            int id;
            performer_consf_element->QueryIntAttribute("SessionID", &id);

            performerConf->session = shallowCopy->sessions[id];
            UTILS_ASSERT(performerConf->session != nullptr);

            performer_consf_element->QueryIntAttribute("PerformerID", &id);

            performerConf->performer = shallowCopy->performers[id];
            UTILS_ASSERT(performerConf->performer != nullptr);

            //update sesji
            performerConf->session->performerConf = performerConf;
            //update performera
            performerConf->performer->performerConfs[performerConf->performerConfID] = performerConf;


            //Attrs
            TiXmlElement* attrs_element = performer_consf_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    communication::ShallowCopy::A attr;
                    attr_element->QueryStringAttribute("Name", &attr.name);
                    attr_element->QueryStringAttribute("Value", &attr.value);
                    
                    performerConf->attrs[attr.name] = attr;
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            
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

const ShallowCopyConstPtr& ShallowCopyParser::getShallowCopy() const
{
    return constShallowCopy;
}

void ShallowCopyParser::getObjects(std::vector<core::ObjectWrapperPtr>& objects)
{
    objects.push_back(object);
}