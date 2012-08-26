#include "CommunicationPCH.h"
#include <plugins/communication/ShallowCopyParser.h>
#include <core/IDataManager.h>

MotionShallowCopyParser::MotionShallowCopyParser() : shallowCopy(communication::MotionShallowCopy::ShallowCopy::create()),
    object(core::ObjectWrapper::create<communication::MotionShallowCopy::ShallowCopy>())
{
    constShallowCopy = shallowCopy;
    object->set(shallowCopy);
}

MotionShallowCopyParser::~MotionShallowCopyParser()
{
}

void MotionShallowCopyParser::parseFile(const core::Filesystem::Path& path)
{
    this->path = path;

    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku MotionShallowCopy");
    }

    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MotionShallowCopy");
    }
    hParent = TiXmlHandle(_element);

    //Performers
    TiXmlElement* performers_element = hParent.FirstChild("Performers").ToElement();
    if(performers_element) {
        TiXmlElement* performer_element = performers_element->FirstChildElement("Performer");
        while(performer_element) {
            //communication::MotionShallowCopy::Performer * performer = new communication::MotionShallowCopy::Performer;
            communication::MotionShallowCopy::Performer * performer = nullptr;
            
            int perfID;

            performer_element->QueryIntAttribute("PerformerID", &perfID);

            auto perfIT = shallowCopy->performers.find(perfID);
            if(perfIT == shallowCopy->performers.end()){
                performer = new communication::MotionShallowCopy::Performer;
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
                    communication::MotionShallowCopy::Attrs::_Val_type attribute;
                    attr_element->QueryStringAttribute("Name", &attribute.first);
                    attr_element->QueryStringAttribute("Value", &attribute.second);
                    
                    performer->attrs.insert(attribute);
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
            //communication::MotionShallowCopy::Session * session = new communication::MotionShallowCopy::Session;
            communication::MotionShallowCopy::Session * session = nullptr;

            int sessionID;
            session_element->QueryIntAttribute("SessionID", &sessionID);

            auto sessionIT = shallowCopy->sessions.find(sessionID);
            if(sessionIT == shallowCopy->sessions.end()){
                session = new communication::MotionShallowCopy::Session;
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
                    communication::MotionShallowCopy::Attrs::_Val_type attribute;
                    attr_element->QueryStringAttribute("Name", &attribute.first);
                    attr_element->QueryStringAttribute("Value", &attribute.second);

                    session->attrs.insert(attribute);

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Session files
            TiXmlElement* files_element = session_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //communication::MotionShallowCopy::File * file = new communication::MotionShallowCopy::File;
                    communication::MotionShallowCopy::File * file = nullptr;

                    int fileID;

                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy->files.find(fileID);

                    if(fileIT == shallowCopy->files.end()){
                        file = new communication::MotionShallowCopy::File;
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
            communication::MotionShallowCopy::GroupAssigment group_assignment ;
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
            //communication::MotionShallowCopy::Trial * trial = new communication::MotionShallowCopy::Trial;
            communication::MotionShallowCopy::Trial * trial = nullptr;
            
            int sessionID;
            int trialID;
            trial_element->QueryIntAttribute("TrialID", &trialID);

            auto trialIT = shallowCopy->trials.find(trialID);

            if(trialIT == shallowCopy->trials.end()){
                trial = new communication::MotionShallowCopy::Trial;
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
                    communication::MotionShallowCopy::Attrs::_Val_type attribute;
                    attr_element->QueryStringAttribute("Name", &attribute.first);
                    attr_element->QueryStringAttribute("Value", &attribute.second);

                    trial->attrs.insert(attribute);

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Files
            TiXmlElement* files_element = trial_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //communication::MotionShallowCopy::File * file = new communication::MotionShallowCopy::File;
                    communication::MotionShallowCopy::File * file = nullptr;
                    int fileID;
                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy->files.find(fileID);

                    if(fileIT == shallowCopy->files.end()){
                        file = new communication::MotionShallowCopy::File;
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
            //communication::MotionShallowCopy::PerformerConf * performerConf = new communication::MotionShallowCopy::PerformerConf;
            communication::MotionShallowCopy::PerformerConf * performerConf = nullptr;

            int perfConfID;
            performer_consf_element->QueryIntAttribute("PerformerConfID", &perfConfID);

            auto perfConfIT = shallowCopy->performerConfs.find(perfConfID);

            if(perfConfIT == shallowCopy->performerConfs.end()){
                performerConf = new communication::MotionShallowCopy::PerformerConf;
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
                    communication::MotionShallowCopy::Attrs::_Val_type attribute;
                    attr_element->QueryStringAttribute("Name", &attribute.first);
                    attr_element->QueryStringAttribute("Value", &attribute.second);

                    performerConf->attrs.insert(attribute);
                    
                    attr_element = attr_element->NextSiblingElement();
                }
            }
            
            performer_consf_element = performer_consf_element->NextSiblingElement();
        }
    }
    LOG_INFO("Motion shallow copy parsed.");
}

core::IParser* MotionShallowCopyParser::create()
{
    return new MotionShallowCopyParser();
}

void MotionShallowCopyParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription desc;

    desc.types.insert(typeid(communication::MotionShallowCopy::ShallowCopy));
    desc.description = "Shallow copy of motion DB";
    extensions["xml"] = desc;
}

const MotionShallowCopyConstPtr& MotionShallowCopyParser::getShallowCopy() const
{
    return constShallowCopy;
}

const MotionShallowCopyPtr& MotionShallowCopyParser::getShallowCopy() 
{
    return shallowCopy;
}

void MotionShallowCopyParser::getObjects(core::Objects& objects)
{
    objects.insert(object);
}


MedicalShallowCopyParser::MedicalShallowCopyParser() : shallowCopy(communication::MedicalShallowCopy::ShallowCopy::create()),
    object(core::ObjectWrapper::create<communication::MedicalShallowCopy::ShallowCopy>())
{
    constShallowCopy = shallowCopy;
    object->set(shallowCopy);
}

MedicalShallowCopyParser::~MedicalShallowCopyParser()
{
}

void MedicalShallowCopyParser::parseFile(const core::Filesystem::Path& path)
{
    this->path = path;

    TiXmlDocument document(path.string());
    if(!document.LoadFile()) {
        UTILS_ASSERT(false, "Blad wczytania pliku MedicalShallowCopy");
    }

    TiXmlHandle hDocument(&document);
    TiXmlElement* _element;
    TiXmlHandle hParent(0);

    _element = hDocument.FirstChildElement().Element();
    if(!_element) {
        UTILS_ASSERT(false, "Blad wczytania z pliku MedicalShallowCopy");
    }
    hParent = TiXmlHandle(_element);

    //Słowniki
    TiXmlElement* dictionary_elements = hParent.FirstChild("Dictionaries").ToElement();
    if(dictionary_elements != nullptr){
        //Disorders
        TiXmlElement* disorder_elements = dictionary_elements->FirstChild("Disorders")->ToElement();
        if(disorder_elements != nullptr){
            TiXmlElement* disorder_element = disorder_elements->FirstChildElement("Disorder");
            while(disorder_element != nullptr){

                int disorderID;
                disorder_element->QueryIntAttribute("DisorderID", &disorderID);

                auto it = shallowCopy->disorders.find(disorderID);

                if(it == shallowCopy->disorders.end()){
                    it = shallowCopy->disorders.insert(communication::MedicalShallowCopy::Disorders::value_type(disorderID, new communication::MedicalShallowCopy::Disorder)).first;
                    it->second->disorderID = disorderID;
                }

                disorder_element->QueryStringAttribute("DisorderName", &it->second->name);

                disorder_element = disorder_element->NextSiblingElement();
            }
        }

        //TODO
        //ExamGroups <- aktualnie niewykorzystywane
    }

    //Patients
    TiXmlElement* patient_elements = hParent.FirstChild("Patients").ToElement();
    if(patient_elements) {
        TiXmlElement* patient_element = patient_elements->FirstChildElement("Patient");
        while(patient_element) {
            communication::MedicalShallowCopy::Patient * patient = nullptr;

            int patientID;

            patient_element->QueryIntAttribute("PatientID", &patientID);

            auto perfIT = shallowCopy->patients.find(patientID);
            if(perfIT == shallowCopy->patients.end()){
                patient = new communication::MedicalShallowCopy::Patient;
                patient->patientID = patientID;
                shallowCopy->patients[patientID] = patient;
            }else{
                patient = perfIT->second;
            }

            patient_element->QueryIntAttribute("BDRPerformerID", &patient->motionPerformerID);
            patient_element->QueryStringAttribute("FirstName", &patient->name);
            patient_element->QueryStringAttribute("LastName", &patient->surname);

            std::string gender;
            patient_element->QueryStringAttribute("Gender", &gender);
            patient->gender = gender[0];

            shallowCopy->patientsByGender[patient->gender].insert(patient);

            patient_element->QueryStringAttribute("BirthDate", &patient->birthDate);

            patient_element = patient_element->NextSiblingElement();
        }
    }

    //DisordersOccurences
    TiXmlElement* disorder_elements = hParent.FirstChild("DisorderOccurences").ToElement();
    if(disorder_elements) {
        TiXmlElement* disorder_element = disorder_elements->FirstChildElement("DisorderOccurence");
        while(disorder_element) {

            int patientID;
            disorder_element->QueryIntAttribute("PatientID", &patientID);

            int disorderID;
            disorder_element->QueryIntAttribute("DisorderID", &disorderID);

            auto & disorder = shallowCopy->patients[patientID]->disorders[disorderID];
            shallowCopy->patients[patientID]->disorders[disorderID].disorder = shallowCopy->disorders.find(disorderID)->second;
            disorder_element->QueryStringAttribute("Focus", &disorder.focus);

            disorder_element->QueryStringAttribute("DiagnosisDate", &disorder.diagnosisDate);
            disorder_element->QueryStringAttribute("Comments", &disorder.comments);

            shallowCopy->patientsByDisorder[disorderID].insert(shallowCopy->patients[patientID]);

            disorder_element = disorder_element->NextSiblingElement();
        }
    }
    
    LOG_INFO("Medical shallow copy parsed.");
}

core::IParser* MedicalShallowCopyParser::create()
{
    return new MedicalShallowCopyParser();
}

void MedicalShallowCopyParser::getSupportedExtensions(core::IParser::Extensions & extensions) const
{
    core::IParser::ExtensionDescription desc;

    desc.types.insert(typeid(communication::MedicalShallowCopy::ShallowCopy));
    desc.description = "Shallow copy of medical DB";
    extensions["xml"] = desc;
}

const MedicalShallowCopyConstPtr& MedicalShallowCopyParser::getShallowCopy() const
{
    return constShallowCopy;
}

const MedicalShallowCopyPtr& MedicalShallowCopyParser::getShallowCopy() 
{
    return shallowCopy;
}

void MedicalShallowCopyParser::getObjects(core::Objects& objects)
{
    objects.insert(object);
}
