#include <webserviceslib/IncrementalBranchShallowCopyParser.h>
#include <utils/Debug.h>
#include <tinyxml2.h>
#include <webserviceslib/DateTimeUtils.h>
#include <webserviceslib/Entity.h>
#include <webserviceslib/ShallowCopy.h>


namespace webservices
{


void parseBranch( tinyxml2::XMLHandle &hParent, IncrementalBranchShallowCopy::ShallowCopy &shallowCopy );
void parseFilesBranch( tinyxml2::XMLElement* trial_element, IncrementalBranchShallowCopy::ShallowCopy &shallowCopy, IncrementalBranchShallowCopy::Files& files, IncrementalBranchShallowCopy::Trial& trial );

void IncrementalBranchShallowCopyParser::parseFile( const std::string & path, IncrementalBranchShallowCopy& shallowCopy )
{
    tinyxml2::XMLDocument document;
    if(!document.LoadFile(path.c_str())) {
        UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
    }

    tinyxml2::XMLHandle hDocument(&document);
    tinyxml2::XMLElement* _element;
    tinyxml2::XMLHandle hParent(0);

    _element = hDocument.FirstChildElement().ToElement();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
    }
    hParent = tinyxml2::XMLHandle(_element);

    
    tinyxml2::XMLElement* modified = hParent.FirstChildElement("Modified").ToElement();
    if (modified) {
        parseBranch(tinyxml2::XMLHandle(modified), shallowCopy.modified);
    }
    
    tinyxml2::XMLElement* added = hParent.FirstChildElement("Added").ToElement();
    if (added) {
        parseBranch(tinyxml2::XMLHandle(added), shallowCopy.added);
    }

}

void parseBranch( tinyxml2::XMLHandle &hParent, IncrementalBranchShallowCopy::ShallowCopy &shallowCopy )
{
    ////Performers
    //tinyxml2::XMLElement* performers_element = hParent.FirstChildElement("Performers").ToElement();
    //if (performers_element) {
    //    tinyxml2::XMLElement* performer_element = performers_element->FirstChildElement("Performer");
    //    while(performer_element) {
    //        //newMotionShallowCopy::Performer * performer = new MotionShallowCopy::Performer;
    //        MotionShallowCopy::Performer * performer = nullptr;

    //        int perfID;

    //        performer_element->QueryIntAttribute("PerformerID", &perfID);

    //        auto perfIT = shallowCopy.performers.find(perfID);
    //        if(perfIT == shallowCopy.performers.end()){
    //            performer = new MotionShallowCopy::Performer;
    //            performer->performerID = perfID;
    //            shallowCopy.performers[perfID] = performer;
    //            performer->patient = nullptr;
    //        }else{
    //            performer = perfIT->second;
    //        }

    //        //Attrs
    //        tinyxml2::XMLElement* attrs_element = performer_element->FirstChildElement("Attrs");
    //        if(attrs_element) {
    //            tinyxml2::XMLElement* attr_element = attrs_element->FirstChildElement("A");
    //            while(attr_element) {
    //                MotionShallowCopy::Attrs::key_type name;
    //                MotionShallowCopy::Attrs::mapped_type value;

    //                name = attr_element->Attribute("Name");
    //                value = attr_element->Attribute("Value");

    //                performer->attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));
    //                attr_element = attr_element->NextSiblingElement();
    //            }
    //        }

    //        performer_element = performer_element->NextSiblingElement();
    //    }
    //}

    ////Sessions
    //tinyxml2::XMLElement* sessions_element = hParent.FirstChildElement("Sessions").ToElement();
    //if(sessions_element) {
    //    tinyxml2::XMLElement* session_element = sessions_element->FirstChildElement("Session");
    //    while(session_element) {
    //        //newMotionShallowCopy::Session * session = new MotionShallowCopy::Session;
    //        MotionShallowCopy::Session * session = nullptr;

    //        int sessionID;
    //        session_element->QueryIntAttribute("SessionID", &sessionID);

    //        auto sessionIT = shallowCopy.sessions.find(sessionID);
    //        if(sessionIT == shallowCopy.sessions.end()){
    //            session = new MotionShallowCopy::Session;
    //            session->sessionID = sessionID;
    //            shallowCopy.sessions[sessionID] = session;
    //        }else{
    //            session = sessionIT->second;
    //        }

    //        session->performerConf = nullptr;
    //        session->groupAssigment = nullptr;

    //        session_element->QueryIntAttribute("UserID", &session->userID);
    //        session_element->QueryIntAttribute("LabID", &session->labID);
    //        session->motionKind = session_element->Attribute("MotionKind");
    //        {
    //            std::string sessionDate;
    //            sessionDate = session_element->Attribute("SessionDate");
    //            session->sessionDate = toTime(sessionDate);
    //        }
    //        session->sessionName = session_element->Attribute("SessionName");
    //        session->tags = session_element->Attribute("Tags");
    //        session->sessionDescription = session_element->Attribute("SessionDescription");
    //        session->emgConf = session_element->Attribute("EMGConf");

    //        //Attrs
    //        tinyxml2::XMLElement* attrs_element = session_element->FirstChildElement("Attrs");
    //        if(attrs_element) {
    //            tinyxml2::XMLElement* attr_element = attrs_element->FirstChildElement("A");
    //            while(attr_element) {
    //                MotionShallowCopy::Attrs::key_type name;
    //                MotionShallowCopy::Attrs::mapped_type value;

    //                name = attr_element->Attribute("Name");
    //                value = attr_element->Attribute("Value");

    //                session->attrs.insert(MotionShallowCopy::Attrs::value_type(name,value));

    //                attr_element = attr_element->NextSiblingElement();
    //            }
    //        }

    //        //Session files
    //        tinyxml2::XMLElement* files_element = session_element->FirstChildElement("Files");
    //        if(files_element) {
    //            tinyxml2::XMLElement* file_element = files_element->FirstChildElement("File");
    //            while(file_element) {
    //                //newMotionShallowCopy::File * file = new MotionShallowCopy::File;
    //                MotionShallowCopy::File * file = nullptr;

    //                int fileID;

    //                file_element->QueryIntAttribute("FileID", &fileID);

    //                auto fileIT = shallowCopy.files.find(fileID);

    //                if(fileIT == shallowCopy.files.end()){
    //                    file = new MotionShallowCopy::File;
    //                    file->fileID = fileID;
    //                    shallowCopy.files[fileID] = file;
    //                }else{
    //                    file = fileIT->second;
    //                }

    //                file->trial = nullptr;
    //                file->session = session;
    //                file->fileSize = 0;
    //                unsigned int s = 0;

    //                file->fileName = file_element->Attribute("FileName");
    //                file->fileDescription = file_element->Attribute("FileDescription");
    //                file->subdirPath = file_element->Attribute("SubdirPath");
    //                file_element->QueryUnsignedAttribute("Size", &s);
    //                file->fileSize = s;

    //                session->files[file->fileID] = file;

    //                file_element = file_element->NextSiblingElement();
    //            }
    //        }

    //        session_element = session_element->NextSiblingElement();
    //    }
    //}
    ////GroupAssignments
    //tinyxml2::XMLElement* group_assignments_element = hParent.FirstChildElement("GroupAssignments").ToElement();
    //if(group_assignments_element) {
    //    tinyxml2::XMLElement* group_assignment_element = group_assignments_element->FirstChildElement("GroupAssignment");
    //    while(group_assignment_element) {
    //        MotionShallowCopy::GroupAssigment * group_assignment = nullptr;

    //        int sessionGroupID;

    //        group_assignment_element->QueryIntAttribute("SessionGroupID", &sessionGroupID);

    //        auto groupAssigmentIT = shallowCopy.groupAssigments.find(sessionGroupID);

    //        if(groupAssigmentIT == shallowCopy.groupAssigments.end()){
    //            group_assignment = new MotionShallowCopy::GroupAssigment;
    //            group_assignment->sessionGroupID = sessionGroupID;
    //            shallowCopy.groupAssigments[sessionGroupID] = group_assignment;

    //        }else{
    //            group_assignment = groupAssigmentIT->second;
    //        }

    //        int sessionID;

    //        group_assignment_element->QueryIntAttribute("SessionID", &sessionID);

    //        auto sessionIT = shallowCopy.sessions.find(sessionID);

    //        if(sessionIT != shallowCopy.sessions.end()){
    //            sessionIT->second->groupAssigment = group_assignment;
    //            group_assignment->sessions[sessionIT->first] = sessionIT->second;
    //        }

    //        group_assignment_element = group_assignment_element->NextSiblingElement();
    //    }
    //}
    //Trials
    tinyxml2::XMLElement* trials_element = hParent.FirstChildElement("Trials").ToElement();
    if(trials_element) {
        tinyxml2::XMLElement* trial_element = trials_element->FirstChildElement("Trial");
        while(trial_element) {
            IncrementalBranchShallowCopy::Trial  trial;
            trial_element->QueryIntAttribute("TrialID", &trial.trialID);
            trial_element->QueryIntAttribute("SessionID", &trial.sessionID);

            //trial->session->trials[trial->trialID] = trial;

            trial.trialName = trial_element->Attribute("TrialName");
            trial.trialDescription = trial_element->Attribute("TrialDescription");

            //Attrs
            tinyxml2::XMLElement* attrs_element = trial_element->FirstChildElement("Attrs");
            if(attrs_element) {
                tinyxml2::XMLElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
                    MotionShallowCopy::Attrs::key_type name;
                    MotionShallowCopy::Attrs::mapped_type value;

                    name = attr_element->Attribute("Name");
                    value = attr_element->Attribute("Value");

                    trial.attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Files

            tinyxml2::XMLElement* added_files_element = trial_element->FirstChildElement("AddedFiles");
            if(added_files_element) {
                parseFilesBranch(added_files_element, shallowCopy, trial.addedFiles, trial);
            }

            tinyxml2::XMLElement* modified_files_element = trial_element->FirstChildElement("ModifiedFiles");
            if(modified_files_element) {
                parseFilesBranch(modified_files_element, shallowCopy, trial.modifiedFiles, trial);
            }
            shallowCopy.trials.push_back(trial);
            trial_element = trial_element->NextSiblingElement();
        }
    }

    ////PerformerConfs
    //tinyxml2::XMLElement* performer_consfs_element = hParent.FirstChildElement("PerformerConfs").ToElement();
    //if(performer_consfs_element) {
    //    tinyxml2::XMLElement* performer_consf_element = performer_consfs_element->FirstChildElement("PerformerConf");
    //    while(performer_consf_element) {
    //        //newMotionShallowCopy::PerformerConf * performerConf = new MotionShallowCopy::PerformerConf;
    //        MotionShallowCopy::PerformerConf * performerConf = nullptr;

    //        int perfConfID;
    //        performer_consf_element->QueryIntAttribute("PerformerConfID", &perfConfID);

    //        auto perfConfIT = shallowCopy.performerConfs.find(perfConfID);

    //        if(perfConfIT == shallowCopy.performerConfs.end()){
    //            performerConf = new MotionShallowCopy::PerformerConf;
    //            performerConf->performerConfID = perfConfID;
    //            shallowCopy.performerConfs[performerConf->performerConfID] = performerConf;
    //        }else{
    //            performerConf = perfConfIT->second;
    //        }

    //        int id;
    //        performer_consf_element->QueryIntAttribute("SessionID", &id);

    //        performerConf->session = shallowCopy.sessions[id];
    //        UTILS_ASSERT(performerConf->session != nullptr);

    //        performer_consf_element->QueryIntAttribute("PerformerID", &id);

    //        performerConf->performer = shallowCopy.performers[id];
    //        UTILS_ASSERT(performerConf->performer != nullptr);

    //        //update sesji
    //        performerConf->session->performerConf = performerConf;
    //        //update performera
    //        performerConf->performer->performerConfs[performerConf->performerConfID] = performerConf;


    //        //Attrs
    //        tinyxml2::XMLElement* attrs_element = performer_consf_element->FirstChildElement("Attrs");
    //        if(attrs_element) {
    //            tinyxml2::XMLElement* attr_element = attrs_element->FirstChildElement("A");
    //            while(attr_element) {
    //                MotionShallowCopy::Attrs::key_type name;
    //                MotionShallowCopy::Attrs::mapped_type value;

    //                name = attr_element->Attribute("Name");
    //                value = attr_element->Attribute("Value");

    //                performerConf->attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));

    //                attr_element = attr_element->NextSiblingElement();
    //            }
    //        }

    //        performer_consf_element = performer_consf_element->NextSiblingElement();
    //    }
    //}
}

void parseFilesBranch( tinyxml2::XMLElement* files_element, IncrementalBranchShallowCopy::ShallowCopy &shallowCopy, IncrementalBranchShallowCopy::Files& files, IncrementalBranchShallowCopy::Trial& trial )
{
    tinyxml2::XMLElement* file_element = files_element->FirstChildElement("File");
    while(file_element) {
        //newMotionShallowCopy::File * file = new MotionShallowCopy::File;
        IncrementalBranchShallowCopy::File file;
        file_element->QueryIntAttribute("FileID", &file.fileID);
        /*file.trial = trial;
        file.session = trial->session;*/
        file.fileSize = 0;
        unsigned int s = 0;

        file.fileName = file_element->Attribute("FileName");
        file.fileDescription = file_element->Attribute("FileDescription");
        file.subdirPath = file_element->Attribute("SubdirPath");
        file_element->QueryUnsignedAttribute("Size", &s);
        file.fileSize = s;

        file.trialID = trial.trialID;
        file.sessionID = trial.sessionID;
        files.push_back(file);
        file_element = file_element->NextSiblingElement();
    }
}

}

