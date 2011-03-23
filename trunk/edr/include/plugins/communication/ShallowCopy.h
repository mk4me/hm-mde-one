#ifndef HEADER_GUARD_COMMUNICATION_SHALLOWCOPY_H__
#define HEADER_GUARD_COMMUNICATION_SHALLOWCOPY_H__

#include <string>
#include <vector>

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
#endif //HEADER_GUARD_COMMUNICATION_SHALLOWCOPY_H__
