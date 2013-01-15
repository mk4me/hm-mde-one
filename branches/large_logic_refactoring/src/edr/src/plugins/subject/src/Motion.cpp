#include "SubjectPCH.h"
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include "SubjectService.h"
#include "Session.h"
#include "Subject.h"
#include "Motion.h"
#include <core/PluginCommon.h>

using namespace PluginSubject;

PluginSubject::SubjectID Motion::globalID = 0;

PluginSubject::SubjectID Motion::nextGlobalID()
{
	if(globalID == std::numeric_limits<PluginSubject::SubjectID>::max()){
		throw std::runtime_error("Motion overflow");
	}

	return globalID++;
}

Motion::Motion(const core::ObjectWrapperConstPtr & session,
	const PluginSubject::SessionConstPtr & unpackedSession,
    PluginSubject::SubjectID localMotionID, const core::ConstObjectsList & wrappers)
	: motionID(nextGlobalID()), session(session), unpackedSession(unpackedSession),
	localMotionID(localMotionID), wrappers(wrappers)
{
    //generujemy nazwę
	generateName(name, localName, motionID, localMotionID, unpackedSession->getLocalName());
}

Motion::~Motion()
{

}

void Motion::generateName(std::string & name, std::string & localName, PluginSubject::SubjectID motionID, PluginSubject::SubjectID motionLocalID, const std::string & sessionLocalName)
{
	std::stringstream ss;
	ss.fill('0');
	ss.width(4);
	ss << motionID;

	name = "Motion" + ss.str();

	localName = sessionLocalName + "-M";

	std::stringstream ss1;
	ss1.fill('0');
	ss1.width(2);
	ss1 << motionLocalID;

	localName += ss1.str();
}

const std::string & Motion::getName() const
{
    return name;
}

const std::string & Motion::getLocalName() const
{
    return localName;
}

SubjectID Motion::getID() const
{
    return motionID;
}

SubjectID Motion::getLocalID() const
{
    return localMotionID;
}


const core::ObjectWrapperConstPtr & Motion::getSession() const
{
    return session;
}

const PluginSubject::SessionConstPtr & Motion::getUnpackedSession() const
{
	return unpackedSession;
}

int Motion::size() const
{
    return wrappers.size();
}

const core::ObjectWrapperConstPtr & Motion::get(int i) const
{
	auto it = wrappers.begin();
	std::advance(it, i);
    return *it;
}

bool Motion::isSupported( const core::TypeInfo& typeToCheck ) const
{
    for (auto it = types.begin(); it != types.end(); ++it) {
        if (*it == typeToCheck) {
            return true;
        }
    }
    return false;
}

void Motion::getWrappers(core::ConstObjectsList & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

FilteredMotion::FilteredMotion(const core::ObjectWrapperConstPtr & originalMotion,
	const PluginSubject::MotionConstPtr & originalUnpackedMotion, const core::ConstObjectsList & wrappers)
    : originalMotion(originalMotion), originalUnpackedMotion(originalUnpackedMotion), wrappers(wrappers)
{

}

FilteredMotion::~FilteredMotion()
{

}

const std::string & FilteredMotion::getName() const
{
    return originalUnpackedMotion->getName();
}

const std::string & FilteredMotion::getLocalName() const
{
    return originalUnpackedMotion->getLocalName();
}

SubjectID FilteredMotion::getID() const
{
    return originalUnpackedMotion->getID();
}

SubjectID FilteredMotion::getLocalID() const
{
    return originalUnpackedMotion->getLocalID();
}

const core::ObjectWrapperConstPtr & FilteredMotion::getSession() const
{
    return session;
}

const PluginSubject::SessionConstPtr & FilteredMotion::getUnpackedSession() const
{
	return unpackedSession;
}

void FilteredMotion::setSession(const core::ObjectWrapperConstPtr & session, const PluginSubject::SessionConstPtr & unpackedSession)
{
    this->session = session;
	this->unpackedSession = unpackedSession;
}

int FilteredMotion::size() const
{
    return wrappers.size();
}

const core::ObjectWrapperConstPtr & FilteredMotion::get(int i) const
{
	auto it = wrappers.begin();
	std::advance(it, i);
    return *it;
}

const core::ObjectWrapperConstPtr & FilteredMotion::getOriginalMotion() const
{
    return originalMotion;
}

const PluginSubject::MotionConstPtr & FilteredMotion::getOriginalUnpackedMotion() const
{
	return originalUnpackedMotion;
}
