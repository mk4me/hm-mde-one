/**
@author Marek Daniluk
@brief Dokument z prostymi typami, najczêœciej encjami bazodanowymi powi¹zanymi z ró¿nymi technologiami.
*/

#ifndef HEADER_GUARD_COMMUNICATION_ENTITY_H__
#define HEADER_GUARD_COMMUNICATION_ENTITY_H__

#include <string>
#include <map>
#include <set>
#include <vector>
#include <webserviceslib/DateTime.h>

namespace webservices
{
    /**
    Encje bazodanowe operacji wsdlowych.
    */
    namespace motionWsdl
    {
        struct Trial
        {
            int id;
            int sessionId;
            std::string trialDescription;
            std::vector<int> trialFiles;
        };

        struct Session
        {
            int id;
            int userId;
            int labId;
            std::string motionKind;
            std::string sessionDescription;
            std::string sessionLabel;
            DateTime sessionDate;
            std::vector<int> sessionTrials;
        };

        struct File
        {
            int id;
            std::string fileName;
            std::string fileDescription;
            std::string fileSubdir;
            std::string fileAttributeName;
        };

        typedef int Performer;
    }

    namespace medicalWsdl
    {

    }

    //! Forward declarations
    namespace MotionShallowCopy
    {
        struct File;
        struct Trial;
        struct Session;
        struct Performer;
        struct PerformerConf;
        struct ShallowCopy;
    }

    //! Forward declarations
    namespace MedicalShallowCopy
    {
        struct Disorder;
        struct DisorderOccurence;
        struct Patient;
    }
    
    /**
    Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat MotionMetaData.
    */
    namespace MotionMetaData
    {
        struct SessionGroup
        {
            int sessionGroupID;
            std::string sessionGroupName;
        };

        typedef std::vector<SessionGroup> SessionGroups;

        struct MotionKind
        {
            std::string motionKindName;
        };

        typedef std::vector<MotionKind> MotionKinds;

        struct Lab
        {
            int labID;
            std::string labName;
        };

        typedef std::vector<Lab> Labs;

        struct Attribute
        {
            std::string attributeName;
            std::string attributeType;
            std::string unit;
        };

        typedef std::map<std::string, Attribute> Attributes;

        struct AttributeGroup
        {
            Attributes attributes;
            int attributeGroupID;
            std::string attributeGroupName;
            std::string describedEntity;
        };

        typedef std::vector<AttributeGroup> AttributeGroups;

        struct MetaData
        {
            DateTime timestamp;
            SessionGroups sessionGroups;
            MotionKinds motionKinds;
            Labs labs;
            AttributeGroups attributeGroups;

			MetaData() : timestamp(0) {}
        };
    }

    namespace MedicalMetaData
    {
        struct ExamType
        {
            int id;
            std::string name;
        };

        typedef std::map<int, ExamType> ExamTypes;

        struct DisorderType
        {
            int id;
            std::string name;
        };

        typedef std::map<int, DisorderType> DisorderTypes;

        struct MetaData
        {
            DateTime timestamp;
            ExamTypes examTypes;
            DisorderTypes disorderTypes;

			MetaData() : timestamp(0) {}
        };
    }
    
    /**
    Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat MotionShallowCopy.
    */
    namespace MotionShallowCopy
    {
        struct File;
        struct Trial;
        struct Session;
        struct Performer;
        struct PerformerConf;
        struct ShallowCopy;

        typedef std::map<std::string, std::string> Attrs;

        struct File
        {
            bool isSessionFile() const
            {
                return trial == nullptr;
            }

            Trial * trial;
            Session * session;
            int fileID;
            std::string fileName;
            std::string fileDescription;
            std::string subdirPath;
			unsigned long long fileSize;
        };

        //typedef std::vector<File> Files;
        typedef std::map<int, File*> Files;

        struct Trial
        {
            Session * session;
            Attrs attrs;
            Files files;
            int trialID;
            //int sessionID;
            std::string trialName;
            std::string trialDescription;
        };

        //typedef std::vector<Trial> Trials;
        typedef std::map<int, Trial*> Trials;

        struct Session
        {
            PerformerConf * performerConf;
            Trials trials;
            Attrs attrs;
            int sessionID;
            int userID;
            int labID;
            std::string motionKind;
            std::string sessionDate;
            std::string sessionName;
            std::string tags;
            std::string sessionDescription;
            Files files;
        };

        //typedef std::vector<Session> Sessions;
        typedef std::map<int, Session*> Sessions;

        struct GroupAssigment
        {
            //int sessionID;
            Sessions sessions;
            int sessionGroupID;
        };

        typedef std::vector<GroupAssigment> GroupAssigments;

        struct PerformerConf
        {
            Attrs attrs;
            int performerConfID;
            //int sessionID;
            Performer * performer;
            //int performerID;
            Session * session;
        };

        //typedef std::vector<PerformerConf> PerformerConfs;
        typedef std::map<int, PerformerConf*> PerformerConfs;

        struct Performer
        {
            Attrs attrs;
            int performerID;
            PerformerConfs performerConfs;
            MedicalShallowCopy::Patient * patient;
        };

        //typedef std::vector<Performer> Performers;
        typedef std::map<int, Performer*> Performers;

        struct ShallowCopy
        {
        public:
            DateTime timestamp;
            Sessions sessions;
            GroupAssigments groupAssigments;
            Trials trials;
            Performers performers;
            PerformerConfs performerConfs;
            Files files;

            ~ShallowCopy()
            {
                for(auto it = sessions.begin(); it != sessions.end(); it++){
                    delete it->second;
                }

                for(auto it = trials.begin(); it != trials.end(); it++){
                    delete it->second;
                }

                for(auto it = performers.begin(); it != performers.end(); it++){
                    delete it->second;
                }

                for(auto it = performerConfs.begin(); it != performerConfs.end(); it++){
                    delete it->second;
                }

                for(auto it = files.begin(); it != files.end(); it++){
                    delete it->second;
                }
            }

			ShallowCopy() : timestamp(0) {}
        };
    }

    namespace MedicalShallowCopy
    {
        struct Disorder
        {
            int disorderID;
            std::string name;
        };

        typedef std::map<int, Disorder*> Disorders;

        struct DisorderOccurence
        {
            Disorder * disorder;
            std::string focus;
            std::string diagnosisDate;
            std::string comments;
        };

        typedef std::map<int, DisorderOccurence> PatientDisorders;

        struct Patient
        {
            int patientID;
            int motionPerformerID;
            std::string name;
            std::string surname;
            std::string birthDate;
            char gender;
            PatientDisorders disorders;
            MotionShallowCopy::Performer * performer;
        };

        typedef std::map<int, Patient*> Patients;

        typedef std::map<char, std::set<Patient*> > PatientsByGender;

        typedef std::map<int, std::set<Patient*> > PatientsByDisorder;

        struct ShallowCopy
        {
            DateTime timestamp;
            Disorders disorders;
            Patients patients;
            PatientsByGender patientsByGender;
            PatientsByDisorder patientsByDisorder;

            ~ShallowCopy()
            {
                for(auto it = disorders.begin(); it !=disorders.end(); it++){
                    delete it->second;
                }

                for(auto it = patients.begin(); it != patients.end(); it++){
                    delete it->second;
                }
            }

			ShallowCopy() : timestamp(0) {}
        };
    }
}
#endif //HEADER_GUARD_COMMUNICATION_ENTITY_H__
