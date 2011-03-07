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

    struct Trial
    {
        Attrs attrs;
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

//ShallowCopy loadShallowCopy(const std::string& filename)
//{
//    ShallowCopy shallowCopy;
//    TiXmlDocument document(filename);
//    if(!document.LoadFile())
//    {
//        LOG_ERROR(": !Blad wczytania pliku ShallowCopy\n");
//        throw std::runtime_error("Blad wczytania pliku ShallowCopy");
//    }
//    TiXmlHandle hDocument(&document);
//    TiXmlElement* _element;
//    TiXmlHandle hParent(0);
//
//    _element = hDocument.FirstChildElement().Element();
//    if(!_element)
//    {
//        LOG_ERROR(": !Blad czytania z pliku ShallowCopy\n");
//        throw std::runtime_error("Blad wczytania z pliku ShallowCopy");
//    }
//    hParent = TiXmlHandle(_element);
//
//    //Sessions
//    TiXmlElement* sessions_element = hParent.FirstChild("Sessions").ToElement();
//    if(sessions_element)
//    {
//        TiXmlElement* session_element = sessions_element->FirstChildElement("Session");
//        while(session_element)
//        {
//            Session session;
//            session_element->QueryIntAttribute("SessionID", &session.sessionID);
//            session_element->QueryIntAttribute("UserID", &session.userID);
//            session_element->QueryIntAttribute("LabID", &session.labID);
//            session_element->QueryStringAttribute("MotionKind", &session.motionKind);
//            session_element->QueryStringAttribute("SessionDate", &session.sessionDate);
//            session_element->QueryStringAttribute("SessionName", &session.sessionName);
//            session_element->QueryStringAttribute("Tags", &session.tags);
//            session_element->QueryStringAttribute("SessionDescription", &session.sessionDescription);
//
//            //Attrs
//            TiXmlElement* attrs_element = session_element->FirstChildElement("Attrs");
//            if(attrs_element)
//            {
//                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
//                while(attr_element)
//                {
//                    A attr;
//                    attr_element->QueryStringAttribute("Name", &attr.name);
//                    attr_element->QueryStringAttribute("Value", &attr.value);
//                    session.attrs.push_back(attr);
//                    attr_element = attr_element->NextSiblingElement();
//                }
//            }
//            shallowCopy.sessions.push_back(session);
//            session_element = session_element->NextSiblingElement();
//        }
//    }
//    //GroupAssignments
//    TiXmlElement* group_assignments_element = hParent.FirstChild("GroupAssignments").ToElement();
//    if(sessions_element)
//    {
//        TiXmlElement* group_assignment_element = group_assignments_element->FirstChildElement("GroupAssignment");
//        while(group_assignment_element)
//        {
//            GroupAssigment group_assignment;
//            group_assignment_element->QueryIntAttribute("SessionID", &group_assignment.sessionID);
//            group_assignment_element->QueryIntAttribute("SessionGroupID", &group_assignment.sessionGroupID);
//            shallowCopy.groupAssigments.push_back(group_assignment);
//            group_assignment_element = group_assignment_element->NextSiblingElement();
//        }
//    }
//    //Trials
//    TiXmlElement* trials_element = hParent.FirstChild("Trials").ToElement();
//    if(trials_element)
//    {
//        TiXmlElement* trial_element = trials_element->FirstChildElement("Trial");
//        while(trial_element)
//        {
//            Trial trial;
//            trial_element->QueryIntAttribute("TrialID", &trial.trialID);
//            trial_element->QueryIntAttribute("SessionID", &trial.sessionID);
//            trial_element->QueryStringAttribute("TrialName", &trial.trialName);
//            trial_element->QueryStringAttribute("TrialDescription", &trial.trialDescription);
//
//            //Attrs
//            TiXmlElement* attrs_element = trial_element->FirstChildElement("Attrs");
//            if(attrs_element)
//            {
//                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
//                while(attr_element)
//                {
//                    A attr;
//                    attr_element->QueryStringAttribute("Name", &attr.name);
//                    attr_element->QueryStringAttribute("Value", &attr.value);
//                    trial.attrs.push_back(attr);
//                    attr_element = attr_element->NextSiblingElement();
//                }
//            }
//            shallowCopy.trials.push_back(trial);
//            trial_element = trial_element->NextSiblingElement();
//        }
//    }
//    //Performers
//    TiXmlElement* performers_element = hParent.FirstChild("Performers").ToElement();
//    if(performers_element)
//    {
//        TiXmlElement* performer_element = performers_element->FirstChildElement("Trial");
//        while(performer_element)
//        {
//            Performer performer;
//            performer_element->QueryIntAttribute("PerformerID", &performer.performerID);
//            performer_element->QueryStringAttribute("FirstName", &performer.firstName);
//            performer_element->QueryStringAttribute("LastName", &performer.lastName);
//
//            //Attrs
//            TiXmlElement* attrs_element = performer_element->FirstChildElement("Attrs");
//            if(attrs_element)
//            {
//                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
//                while(attr_element)
//                {
//                    A attr;
//                    attr_element->QueryStringAttribute("Name", &attr.name);
//                    attr_element->QueryStringAttribute("Value", &attr.value);
//                    performer.attrs.push_back(attr);
//                    attr_element = attr_element->NextSiblingElement();
//                }
//            }
//            shallowCopy.performers.push_back(performer);
//            performer_element = performer_element->NextSiblingElement();
//        }
//    }
//    //PerformerConfs
//    TiXmlElement* performer_consfs_element = hParent.FirstChild("PerformerConfs").ToElement();
//    if(performer_consfs_element)
//    {
//        TiXmlElement* performer_consf_element = performer_consfs_element->FirstChildElement("PerformerConf");
//        while(performer_consf_element)
//        {
//            PerformerConf performerConf;
//            performer_consf_element->QueryIntAttribute("PerformerConfID", &performerConf.performerConfID);
//            performer_consf_element->QueryIntAttribute("SessionID", &performerConf.sessionID);
//            performer_consf_element->QueryIntAttribute("PerformerID", &performerConf.performerID);
//
//            //Attrs
//            TiXmlElement* attrs_element = performer_consf_element->FirstChildElement("Attrs");
//            if(attrs_element)
//            {
//                TiXmlElement* attr_element = attrs_element->FirstChildElement("A");
//                while(attr_element)
//                {
//                    A attr;
//                    attr_element->QueryStringAttribute("Name", &attr.name);
//                    attr_element->QueryStringAttribute("Value", &attr.value);
//                    performerConf.attrs.push_back(attr);
//                    attr_element = attr_element->NextSiblingElement();
//                }
//            }
//            shallowCopy.performerConfs.push_back(performerConf);
//            performer_consf_element = performer_consf_element->NextSiblingElement();
//        }
//    }
//    LOG_INFO("shallow copy parsed.");
//    return shallowCopy;
//}

}
#endif