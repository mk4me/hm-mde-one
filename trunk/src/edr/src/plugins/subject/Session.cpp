#include "SubjectPCH.h"
#include <limits>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <plugins/subject/Motion.h>
#include <plugins/subject/Session.h>
#include <plugins/subject/Subject.h>
#include "SubjectService.h"
#include <core/DataAccessors.h>

using namespace PluginSubject;

Session::Session(SubjectID sessionID, const SubjectConstPtr & subject, SubjectID localSessionID, unsigned int year,
    unsigned char month, unsigned char day, const std::vector<core::ObjectWrapperConstPtr> & wrappers)
    : sessionID(sessionID), subject(subject), localSessionID(localSessionID), year(year), month(month), day(day), wrappers(wrappers),
     currentMotionID(0)
{
    //data
    std::stringstream ss;
    ss.fill('0');
    ss << year << "-";
    ss.width(2);
    ss << (unsigned int)month;
    ss.width(0);
    ss << "-";
    ss.width(2);
    ss << (unsigned int)day;

    //data + nazwa subjecta
    localName += ss.str() + "-" + "B";

    std::stringstream ss1;
    ss1.fill('0');
    ss1.width(4);
    ss1 << subject->getID();

    localName += ss1.str() + "-S";

    std::stringstream ss2;
    ss2.fill('0');
    ss2.width(2);
    ss2 << localSessionID;

    localName += ss2.str();
    
    name = "Session";

    std::stringstream ss3;
    ss3.fill('0');
    ss3.width(4);
    ss3 << sessionID;

    name += ss3.str();
}

Session::~Session()
{

}

const std::string & Session::getName() const
{
    return name;
}

const std::string & Session::getLocalName() const
{
    return localName;
}

SubjectID Session::getID() const
{
    return sessionID;
}

SubjectID Session::getLocalID() const
{
    return localSessionID;
}

void Session::getMotions(Motions & motions) const
{
    Motions toFiltering;
    core::queryDataPtr(core::getDataManagerReader(), toFiltering, true);

    for(auto it = toFiltering.begin(); it != toFiltering.end(); it++){
        if((*it)->getSession().get() == this){
            motions.push_back(*it);
        }
    }
}

const SubjectConstPtr & Session::getSubject() const
{
    return subject;
}

unsigned int Session::getYear() const
{
    return year;
}

unsigned char Session::getMonth() const
{
    return month;
}

unsigned char Session::getDay() const
{
    return day;
}

bool Session::hasObjectOfType(const core::TypeInfo& type) const
{
    for(auto it = wrappers.begin(); it != wrappers.end(); it++){

        if((*it)->getTypeInfo() == type) {
            return true;
        }
    }

    Motions motions;
    getMotions(motions);

    for(auto it = motions.begin(); it != motions.end(); it++){
        if((*it)->hasObjectOfType(type) == true){
            return true;
        }
    }

    return false;
}

void Session::getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

FilteredSession::FilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers)
    : originalSession(originalSession), wrappers(wrappers)
{
    for(auto it = motions.begin(); it != motions.end(); it++){
        this->motions.push_back(*it);
    }
}

FilteredSession::~FilteredSession()
{

}

const std::string & FilteredSession::getName() const
{
    return originalSession->getName();
}

const std::string & FilteredSession::getLocalName() const
{
    return originalSession->getLocalName();
}

SubjectID FilteredSession::getID() const
{
    return originalSession->getID();
}

SubjectID FilteredSession::getLocalID() const
{
    return originalSession->getLocalID();
}

void FilteredSession::getMotions(Motions & motions) const
{
    motions.insert(motions.end(), this->motions.begin(), this->motions.end());
}

const SubjectConstPtr & FilteredSession::getSubject() const
{
    return originalSession->getSubject();
}

unsigned int FilteredSession::getYear() const
{
    return originalSession->getYear();
}

unsigned char FilteredSession::getMonth() const
{
    return originalSession->getMonth();
}

unsigned char FilteredSession::getDay() const
{
    return originalSession->getDay();
}

void FilteredSession::getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

const SessionConstPtr & FilteredSession::getOriginalSession() const
{
    return originalSession;
}