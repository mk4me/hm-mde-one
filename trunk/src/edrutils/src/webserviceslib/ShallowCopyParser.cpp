#include <webserviceslib/ShallowCopyParser.h>
#include <utils/Debug.h>
#include <tinyxml.h>
#include <webserviceslib/DateTimeUtils.h>
#include <webserviceslib/Entity.h>

namespace webservices
{

void MotionShallowCopyParser::parseFile(const std::string & path, MotionShallowCopy::ShallowCopy & shallowCopy)
{
    TiXmlDocument document(path);
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

	//data utworzenia płytkiej kopii

	std::string date;
	_element->QueryStringAttribute("LastModified", &date);

	//mamy date to ja rozpakowujemy
	shallowCopy.timestamp = toTime(date);

    //Performers
    TiXmlElement* performers_element = hParent.FirstChild("Performers").ToElement();
    if(performers_element) {
        TiXmlElement* performer_element = performers_element->FirstChildElement("Performer");
        while(performer_element) {
            //newMotionShallowCopy::Performer * performer = new MotionShallowCopy::Performer;
            MotionShallowCopy::Performer * performer = nullptr;

            int perfID;

            performer_element->QueryIntAttribute("PerformerID", &perfID);

            auto perfIT = shallowCopy.performers.find(perfID);
            if(perfIT == shallowCopy.performers.end()){
                performer = new MotionShallowCopy::Performer;
                performer->performerID = perfID;
                shallowCopy.performers[perfID] = performer;
				performer->patient = nullptr;
            }else{
                performer = perfIT->second;
            }

            //Attrs
            TiXmlElement* attrs_element = performer_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
					MotionShallowCopy::Attrs::key_type name;
					MotionShallowCopy::Attrs::mapped_type value;

                    attr_element->QueryStringAttribute("Name", &name);
                    attr_element->QueryStringAttribute("Value", &value);

                    performer->attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));
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
            //newMotionShallowCopy::Session * session = new MotionShallowCopy::Session;
            MotionShallowCopy::Session * session = nullptr;

            int sessionID;
            session_element->QueryIntAttribute("SessionID", &sessionID);

            auto sessionIT = shallowCopy.sessions.find(sessionID);
            if(sessionIT == shallowCopy.sessions.end()){
                session = new MotionShallowCopy::Session;
                session->sessionID = sessionID;
                shallowCopy.sessions[sessionID] = session;
            }else{
                session = sessionIT->second;
            }

            session->performerConf = nullptr;
			session->groupAssigment = nullptr;

            session_element->QueryIntAttribute("UserID", &session->userID);
            session_element->QueryIntAttribute("LabID", &session->labID);
            session_element->QueryStringAttribute("MotionKind", &session->motionKind);
            session_element->QueryStringAttribute("SessionDate", &session->sessionDate);
            session_element->QueryStringAttribute("SessionName", &session->sessionName);
            session_element->QueryStringAttribute("Tags", &session->tags);
            session_element->QueryStringAttribute("SessionDescription", &session->sessionDescription);
			session_element->QueryStringAttribute("EMGConf", &session->emgConf);

            //Attrs
            TiXmlElement* attrs_element = session_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
					MotionShallowCopy::Attrs::key_type name;
					MotionShallowCopy::Attrs::mapped_type value;

                    attr_element->QueryStringAttribute("Name", &name);
                    attr_element->QueryStringAttribute("Value", &value);

                    session->attrs.insert(MotionShallowCopy::Attrs::value_type(name,value));

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Session files
            TiXmlElement* files_element = session_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //newMotionShallowCopy::File * file = new MotionShallowCopy::File;
                    MotionShallowCopy::File * file = nullptr;

                    int fileID;

                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy.files.find(fileID);

                    if(fileIT == shallowCopy.files.end()){
                        file = new MotionShallowCopy::File;
                        file->fileID = fileID;
                        shallowCopy.files[fileID] = file;
                    }else{
                        file = fileIT->second;
                    }

                    file->trial = nullptr;
                    file->session = session;
					file->fileSize = 0;
					unsigned int s = 0;

					file_element->QueryStringAttribute("FileName", &file->fileName);
					file_element->QueryStringAttribute("FileDescription", &file->fileDescription);
					file_element->QueryStringAttribute("SubdirPath", &file->subdirPath);
					file_element->QueryUnsignedAttribute("Size", &s);
					file->fileSize = s;

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
            MotionShallowCopy::GroupAssigment * group_assignment = nullptr;

			int sessionGroupID;

			group_assignment_element->QueryIntAttribute("SessionGroupID", &sessionGroupID);

			auto groupAssigmentIT = shallowCopy.groupAssigments.find(sessionGroupID);

			if(groupAssigmentIT == shallowCopy.groupAssigments.end()){
				group_assignment = new MotionShallowCopy::GroupAssigment;
				group_assignment->sessionGroupID = sessionGroupID;
				shallowCopy.groupAssigments[sessionGroupID] = group_assignment;

			}else{
				group_assignment = groupAssigmentIT->second;
			}

			int sessionID;

            group_assignment_element->QueryIntAttribute("SessionID", &sessionID);

			auto sessionIT = shallowCopy.sessions.find(sessionID);

			if(sessionIT != shallowCopy.sessions.end()){
				sessionIT->second->groupAssigment = group_assignment;
				group_assignment->sessions[sessionIT->first] = sessionIT->second;
			}

            group_assignment_element = group_assignment_element->NextSiblingElement();
        }
    }
    //Trials
    TiXmlElement* trials_element = hParent.FirstChild("Trials").ToElement();
    if(trials_element) {
        TiXmlElement* trial_element = trials_element->FirstChildElement("Trial");
        while(trial_element) {
            //newMotionShallowCopy::Trial * trial = new MotionShallowCopy::Trial;
            MotionShallowCopy::Trial * trial = nullptr;

            int sessionID;
            int trialID;
            trial_element->QueryIntAttribute("TrialID", &trialID);

            auto trialIT = shallowCopy.trials.find(trialID);

            if(trialIT == shallowCopy.trials.end()){
                trial = new MotionShallowCopy::Trial;
                trial->trialID = trialID;
                shallowCopy.trials[trialID] = trial;
            }else{
                trial = trialIT->second;
            }

            trial_element->QueryIntAttribute("SessionID", &sessionID);
            trial->session = shallowCopy.sessions[sessionID];

            UTILS_ASSERT(trial->session != nullptr);

            trial->session->trials[trial->trialID] = trial;

            trial_element->QueryStringAttribute("TrialName", &trial->trialName);
            trial_element->QueryStringAttribute("TrialDescription", &trial->trialDescription);

            //Attrs
            TiXmlElement* attrs_element = trial_element->FirstChildElement("Attrs");
            if(attrs_element) {
                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
                while(attr_element) {
					MotionShallowCopy::Attrs::key_type name;
					MotionShallowCopy::Attrs::mapped_type value;

                    attr_element->QueryStringAttribute("Name", &name);
                    attr_element->QueryStringAttribute("Value", &value);

                    trial->attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            //Files
            TiXmlElement* files_element = trial_element->FirstChildElement("Files");
            if(files_element) {
                TiXmlElement* file_element = files_element->FirstChildElement("File");
                while(file_element) {
                    //newMotionShallowCopy::File * file = new MotionShallowCopy::File;
                    MotionShallowCopy::File * file = nullptr;
                    int fileID;
                    file_element->QueryIntAttribute("FileID", &fileID);

                    auto fileIT = shallowCopy.files.find(fileID);

                    if(fileIT == shallowCopy.files.end()){
                        file = new MotionShallowCopy::File;
                        file->fileID = fileID;
                        shallowCopy.files[fileID] = file;
                    }else{
                        file = fileIT->second;
                    }

                    file->trial = trial;
                    file->session = trial->session;
					file->fileSize = 0;
					unsigned int s = 0;

                    file_element->QueryStringAttribute("FileName", &file->fileName);
                    file_element->QueryStringAttribute("FileDescription", &file->fileDescription);
                    file_element->QueryStringAttribute("SubdirPath", &file->subdirPath);
					file_element->QueryUnsignedAttribute("Size", &s);
					file->fileSize = s;

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
            //newMotionShallowCopy::PerformerConf * performerConf = new MotionShallowCopy::PerformerConf;
            MotionShallowCopy::PerformerConf * performerConf = nullptr;

            int perfConfID;
            performer_consf_element->QueryIntAttribute("PerformerConfID", &perfConfID);

            auto perfConfIT = shallowCopy.performerConfs.find(perfConfID);

            if(perfConfIT == shallowCopy.performerConfs.end()){
                performerConf = new MotionShallowCopy::PerformerConf;
                performerConf->performerConfID = perfConfID;
                shallowCopy.performerConfs[performerConf->performerConfID] = performerConf;
            }else{
                performerConf = perfConfIT->second;
            }

            int id;
            performer_consf_element->QueryIntAttribute("SessionID", &id);

            performerConf->session = shallowCopy.sessions[id];
            UTILS_ASSERT(performerConf->session != nullptr);

            performer_consf_element->QueryIntAttribute("PerformerID", &id);

            performerConf->performer = shallowCopy.performers[id];
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
					MotionShallowCopy::Attrs::key_type name;
					MotionShallowCopy::Attrs::mapped_type value;

                    attr_element->QueryStringAttribute("Name", &name);
                    attr_element->QueryStringAttribute("Value", &value);

                    performerConf->attrs.insert(MotionShallowCopy::Attrs::value_type(name, value));

                    attr_element = attr_element->NextSiblingElement();
                }
            }

            performer_consf_element = performer_consf_element->NextSiblingElement();
        }
    }
}

void MedicalShallowCopyParser::parseFile(const std::string & path, MedicalShallowCopy::ShallowCopy & shallowCopy)
{
    TiXmlDocument document(path);
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

                auto it = shallowCopy.disorders.find(disorderID);

                if(it == shallowCopy.disorders.end()){
                    it = shallowCopy.disorders.insert(MedicalShallowCopy::Disorders::value_type(disorderID, new MedicalShallowCopy::Disorder)).first;
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
            MedicalShallowCopy::Patient * patient = nullptr;

            int patientID;

            patient_element->QueryIntAttribute("PatientID", &patientID);

            auto perfIT = shallowCopy.patients.find(patientID);
            if(perfIT == shallowCopy.patients.end()){
                patient = new MedicalShallowCopy::Patient;
                patient->patientID = patientID;
                shallowCopy.patients[patientID] = patient;
				patient->performer = nullptr;
            }else{
                patient = perfIT->second;
            }

            patient_element->QueryIntAttribute("BDRPerformerID", &patient->motionPerformerID);
            patient_element->QueryStringAttribute("FirstName", &patient->name);
            patient_element->QueryStringAttribute("LastName", &patient->surname);

            std::string gender;
            patient_element->QueryStringAttribute("Gender", &gender);
            patient->gender = gender[0];

            shallowCopy.patientsByGender[patient->gender].insert(patient);

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

            auto & disorder = shallowCopy.patients[patientID]->disorders[disorderID];
            shallowCopy.patients[patientID]->disorders[disorderID].disorder = shallowCopy.disorders.find(disorderID)->second;
            disorder_element->QueryStringAttribute("Focus", &disorder.focus);

            disorder_element->QueryStringAttribute("DiagnosisDate", &disorder.diagnosisDate);
            disorder_element->QueryStringAttribute("Comments", &disorder.comments);

            shallowCopy.patientsByDisorder[disorderID].insert(shallowCopy.patients[patientID]);

            disorder_element = disorder_element->NextSiblingElement();
        }
    }
}

}
