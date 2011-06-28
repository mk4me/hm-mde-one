/**
@author Marek Daniluk
@brief Dokument z prostymi typami, najczêœciej encjami bazodanowymi powi¹zanymi z ró¿nymi technologiami.
*/

#ifndef HEADER_GUARD_COMMUNICATION_ENTITY_H__
#define HEADER_GUARD_COMMUNICATION_ENTITY_H__

#include <plugins/communication/DateTime.h>

namespace communication
{
    /**
    Encje bazodanowe operacji wsdlowych.
    */
    namespace wsdl
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

        struct Performer
        {
            int id;
            std::string performerFirstname;
            std::string performerLastname;
        };
    }
    
    /**
    Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat MetaData.
    */
    namespace MetaData
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

        struct Enumeration
        {
            std::string enumValue;
        };

        typedef std::vector<Enumeration> Enumerations;

        struct Attribute
        {
            Enumerations enumValues;
            std::string attributeName;
            std::string attributeType;
        };

        typedef std::vector<Attribute> Attributes;

        struct AttributeGroup
        {
            Attributes attributes;
            int attributeGroupID;
            std::string attributeGroupName;
            std::string describedEntity;
            std::string unit;
        };

        typedef std::vector<AttributeGroup> AttributeGroups;

        struct MetaData
        {
            SessionGroups sessionGroups;
            MotionKinds motionKinds;
            Labs labs;
            AttributeGroups attributeGroups;
        };
    }
    
    /**
    Encje bazodanowe z operacji p³ytkiej kopii bazy danych. Schemat ShallowCopy.
    */
    namespace ShallowCopy
    {
        struct A
        {
            std::string name;
            std::string value;
        };

        typedef std::vector<A> Attrs;

        struct Session
        {
            Attrs attrs;
            int sessionID;
            int userID;
            int labID;
            std::string motionKind;
            std::string sessionDate;
            std::string sessionName;
            std::string tags;
            std::string sessionDescription;
        };

        typedef std::vector<Session> Sessions;

        struct GroupAssigment
        {
            int sessionID;
            int sessionGroupID;
        };

        typedef std::vector<GroupAssigment> GroupAssigments;

        struct File
        {
            int fileID;
            std::string fileName;
            std::string fileDescription;
            std::string subdirPath;
        };

        typedef std::vector<File> Files;

        struct Trial
        {
            Attrs attrs;
            Files files;
            int trialID;
            int sessionID;
            std::string trialName;
            std::string trialDescription;
        };

        typedef std::vector<Trial> Trials;

        struct Performer
        {
            Attrs attrs;
            int performerID;
            std::string firstName;
            std::string lastName;
        };

        typedef std::vector<Performer> Performers;

        struct PerformerConf
        {
            Attrs attrs;
            int performerConfID;
            int sessionID;
            int performerID;
        };

        typedef std::vector<PerformerConf> PerformerConfs;

        struct ShallowCopy
        {
            Sessions sessions;
            GroupAssigments groupAssigments;
            Trials trials;
            Performers performers;
            PerformerConfs performerConfs;
        };
    }

    class File
    {
    private:
        communication::ShallowCopy::File file;
        core::Filesystem::Path localPath;
        bool isLocal;
    public:
        File() {};
        File(const communication::ShallowCopy::File& file)
        {
            setFile(file);
        };
        void setFile(const communication::ShallowCopy::File& file)
        {
            this->file = file;
        };
        void setLocalPath(const core::Filesystem::Path& localPath)
        {
            this->localPath = localPath;
        }
        void setIsLocal(bool local)
        {
            isLocal = local;
        };
        const communication::ShallowCopy::File& getFile() const
        {
            return file;
        };
        const core::Filesystem::Path& getLocalPath() const
        {
            return localPath;
        };
        bool getIsLocal() const
        {
            return isLocal;
        };
    };

    typedef core::shared_ptr<File> FilePtr;

    class Trial
    {
    private:
        std::vector<FilePtr> files;
        communication::ShallowCopy::Trial trial;
        //pola pomocnicze
        bool hasEmg;
        bool hasGrf;
        bool hasVideo;
        bool hasMocap;
        bool isLocal;
    public:
        Trial() {};
        Trial(const communication::ShallowCopy::Trial& trial)
        {
            setTrial(trial);
        };
        void setTrial(const communication::ShallowCopy::Trial& trial)
        {
            this->trial = trial;
        };
        void setFiles(const std::vector<FilePtr>& files)
        {
            this->files = files;
        };
        void setHasEmg(bool emg)
        {
            hasEmg = emg;
        };
        void setHasGrf(bool grf)
        {
            hasGrf = grf;
        };
        void setHasVideo(bool video)
        {
            hasVideo = video;
        };
        void setHasMocap(bool mocap)
        {
            hasMocap = mocap;
        };
        void setIsLocal(bool local)
        {
            isLocal = local;
        };
        const std::vector<FilePtr>& getFiles() const
        {
            return files;
        };
        const communication::ShallowCopy::Trial& getTrial() const
        {
            return trial;
        };
        bool getHasEmg() const
        {
            return hasEmg;
        };
        bool getHasGrf() const
        {
            return hasGrf;
        };
        bool getHasVideo() const
        {
            return hasVideo;
        };
        bool getHasMocap() const
        {
            return hasMocap;
        };
        bool getIsLocal() const
        {
            return isLocal;
        };
    };
    
    typedef core::shared_ptr<Trial> TrialPtr;

    class Session
    {
    private:
        std::vector<TrialPtr> trials;
        communication::ShallowCopy::Session session;
    public:
        Session() {};
        Session(const communication::ShallowCopy::Session& session)
        {
            setSession(session);
        };
        void setSession(const communication::ShallowCopy::Session& session)
        {
            this->session = session;
        };
        void setTrials(const std::vector<TrialPtr>& trials)
        {
            this->trials = trials;
        };
        const std::vector<TrialPtr>& getTrials() const
        {
            return trials;
        };
        const communication::ShallowCopy::Session& getSession() const
        {
            return session;
        };
    };
    
    typedef core::shared_ptr<Session> SessionPtr;

    class Performer
    {
    private:
        std::vector<SessionPtr> sessions;
        communication::ShallowCopy::Performer performer;
    public:
        Performer() {};
        Performer(const communication::ShallowCopy::Performer& performer)
        {
            setPerformer(performer);
        };
        void setPerformer(const communication::ShallowCopy::Performer& performer)
        {
            this->performer = performer;
        };
        void setSessions(const std::vector<SessionPtr>& sessions)
        {
            this->sessions = sessions;
        };
        const std::vector<SessionPtr>& getSessions() const
        {
            return sessions;
        };
        const communication::ShallowCopy::Performer& getPerformer() const
        {
            return performer;
        };
    };

    typedef core::shared_ptr<Performer> PerformerPtr;

}
#endif //HEADER_GUARD_COMMUNICATION_ENTITY_H__
