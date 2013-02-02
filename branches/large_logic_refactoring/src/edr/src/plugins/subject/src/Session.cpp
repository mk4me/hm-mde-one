#include "SubjectPCH.h"
#include "Session.h"
#include <plugins/subject/IMotion.h>
#include <plugins/subject/ISubject.h>
#include <corelib/DataAccessors.h>

using namespace PluginSubject;

PluginSubject::SubjectID Session::globalID = 0;

PluginSubject::SubjectID Session::nextGlobalID()
{
	if(globalID == std::numeric_limits<PluginSubject::SubjectID>::max()){
		throw std::runtime_error("Session overflow");
	}

	return globalID++;
}

Session::Session(const core::ObjectWrapperConstPtr & subject, const PluginSubject::SubjectConstPtr & unpackedSubject,
	SubjectID localSessionID, const core::ConstObjectsList & wrappers)
    : sessionID(nextGlobalID()), subject(subject), unpackedSubject(unpackedSubject), localSessionID(localSessionID), wrappers(wrappers),
     currentMotionID(0)
{
	generateNames(localName, name, unpackedSubject->getID(), localSessionID, sessionID);
}

Session::~Session()
{

}

void Session::generateNames(std::string & localName, std::string & globalName, PluginSubject::SubjectID subjectID,
	PluginSubject::SubjectID localSessionID, PluginSubject::SubjectID sessionID)
{
	//nazwa subjecta
	localName = "B";

	std::stringstream ss1;
	ss1.fill('0');
	ss1.width(4);
	ss1 << subjectID;

	localName += ss1.str() + "-S";

	std::stringstream ss2;
	ss2.fill('0');
	ss2.width(2);
	ss2 << localSessionID;

	localName += ss2.str();

	globalName = "Session";

	std::stringstream ss3;
	ss3.fill('0');
	ss3.width(4);
	ss3 << sessionID;

	globalName += ss3.str();
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

void Session::getMotions(core::ConstObjectsList & motions) const
{
	static const core::TypeInfo type = core::TypeInfo(typeid(PluginSubject::IMotion));
    core::ConstObjectsList toFiltering;
    core::getDataManagerReader()->getObjects(toFiltering, type, false);

    for(auto it = toFiltering.begin(); it != toFiltering.end(); ++it){
        PluginSubject::MotionConstPtr motion;
		bool ok = (*it)->tryGet(motion, false);
		if(ok && motion != nullptr && motion->getUnpackedSession()->getID() == sessionID){
            motions.push_back(*it);
        }
    }
}

const core::ObjectWrapperConstPtr & Session::getSubject() const
{
    return subject;
}

const PluginSubject::SubjectConstPtr & Session::getUnpackedSubject() const
{
	return unpackedSubject;
}

bool Session::hasObjectOfType(const core::TypeInfo& type) const
{
    for(auto it = wrappers.begin(); it != wrappers.end(); ++it){

        if((*it)->getTypeInfo() == type) {
            return true;
        }
    }

    core::ConstObjectsList motions;
    getMotions(motions);

    for(auto it = motions.begin(); it != motions.end(); ++it){
		PluginSubject::MotionConstPtr motion;
		bool ok = (*it)->tryGet(motion, false);
        if(ok && motion != nullptr && motion->hasObjectOfType(type) == true){
            return true;
        }
    }

    return false;
}

void Session::getWrappers(core::ConstObjectsList & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

PluginSubject::SubjectID Session::nextMotionID() const
{
	return currentMotionID++;
}

FilteredSession::FilteredSession(const core::ObjectWrapperConstPtr & originalSession, const PluginSubject::SessionConstPtr & originalUnpackedSession,
	const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers)
    : originalSession(originalSession), originalUnpackedSession(originalUnpackedSession), wrappers(wrappers), motions(motions)
{

}

FilteredSession::~FilteredSession()
{

}

const std::string & FilteredSession::getName() const
{
    return originalUnpackedSession->getName();
}

const std::string & FilteredSession::getLocalName() const
{
    return originalUnpackedSession->getLocalName();
}

SubjectID FilteredSession::getID() const
{
    return originalUnpackedSession->getID();
}

SubjectID FilteredSession::getLocalID() const
{
    return originalUnpackedSession->getLocalID();
}

void FilteredSession::getMotions(core::ConstObjectsList & motions) const
{
    motions.insert(motions.end(), this->motions.begin(), this->motions.end());
}

const core::ObjectWrapperConstPtr & FilteredSession::getSubject() const
{
    return subject;
}

const PluginSubject::SubjectConstPtr & FilteredSession::getUnpackedSubject() const
{
	return unpackedSubject;
}

void FilteredSession::getWrappers(core::ConstObjectsList & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

const core::ObjectWrapperConstPtr & FilteredSession::getOriginalSession() const
{
    return originalSession;
}

const PluginSubject::SessionConstPtr & FilteredSession::getOriginalUnpackedSession() const
{
	return originalUnpackedSession;
}

void FilteredSession::setSubject(const core::ObjectWrapperConstPtr & subject, const PluginSubject::SubjectConstPtr & unpackedSubject)
{
	this->subject = subject;
	this->unpackedSubject = unpackedSubject;
}
