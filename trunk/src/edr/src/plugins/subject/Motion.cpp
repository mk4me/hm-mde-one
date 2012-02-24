#include "SubjectPCH.h"
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include "SubjectService.h"
#include <plugins/subject/Session.h>
#include <plugins/subject/Subject.h>
#include <plugins/subject/Motion.h>
#include <core/PluginCommon.h>
#include <plugins/kinematic/Wrappers.h>
#include <core/IDataManager.h>

using namespace PluginSubject;

Motion::Motion(core::IMemoryDataManager * memoryDataManager, PluginSubject::SubjectID motionID, const PluginSubject::SessionConstPtr & session,
    PluginSubject::SubjectID localMotionID, const std::vector<core::ObjectWrapperConstPtr> & wrappers) : memoryDataManager(memoryDataManager),
    motionID(motionID), session(session), localMotionID(localMotionID), wrappers(wrappers)
{

    class JointsInitializer : public core::IDataInitializer
    {
    public:
        JointsInitializer(const core::ObjectWrapperConstPtr & dataWrapper, const core::ObjectWrapperConstPtr & modelWrapper) : 
          dataWrapper(dataWrapper), modelWrapper(modelWrapper)
          {

          }

          virtual void initialize(core::ObjectWrapperPtr & object)
          {
              kinematic::SkeletalDataPtr data;
              kinematic::SkeletalModelPtr model;
              if(dataWrapper->tryGet(data) == true && modelWrapper->tryGet(model) == true && data != nullptr && model != nullptr){
                  kinematic::JointAnglesCollectionPtr joints(new kinematic::JointAnglesCollection());
                  joints->setSkeletal(model, data);
                  object->trySet(joints);
              }
          }

    private:
        core::ObjectWrapperConstPtr dataWrapper;
        core::ObjectWrapperConstPtr modelWrapper;
    };

    //generujemy nazwe

    std::stringstream ss;
    ss.fill('0');
    ss.width(4);
    ss << motionID;

    name = "Motion" + ss.str();

    localName = session->getLocalName() + "-M";

    std::stringstream ss1;
    ss1.fill('0');
    ss1.width(2);
    ss1 << localMotionID;

    localName += ss1.str();


    //sprawdzamy joint angles - jesli nie ma budujemy i dodajemy do DM
    core::ObjectWrapperConstPtr dataWrapper;
    core::ObjectWrapperConstPtr modelWrapper;
    for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
        if ((*it)->isSupported(typeid(kinematic::JointAnglesCollection))) {
            return;
        } else if ((*it)->isSupported(typeid(kinematic::SkeletalData))) {
            dataWrapper = *it;
            break;
        }
    }

    modelWrapper = session->getWrapperOfType(typeid(kinematic::SkeletalModel));

    if (dataWrapper && modelWrapper) {
        core::ObjectWrapperPtr jointsWrapper(core::addData(memoryDataManager, kinematic::JointAnglesCollectionPtr(), core::DataInitializerPtr(new JointsInitializer(dataWrapper, modelWrapper))));        
        jointsWrapper->setName(localName + " joints");
        jointsWrapper->setSource("SubjectPlugin->motion->" + localName);
        this->wrappers.push_back(jointsWrapper);
    }
}

Motion::~Motion()
{

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


const SessionConstPtr & Motion::getSession() const
{
    return session;
}

int Motion::size() const
{
    return wrappers.size();
}

const core::ObjectWrapperConstPtr & Motion::get(int i) const
{
    return wrappers[i];
}

bool Motion::isSupported( const core::TypeInfo& typeToCheck ) const
{
    for (auto it = types.begin(); it != types.end(); it++) {
        if (*it == typeToCheck) {
            return true;
        }
    }
    return false;
}

void Motion::getWrappers(std::vector<core::ObjectWrapperConstPtr> & wrappers) const
{
    wrappers.insert(wrappers.end(), this->wrappers.begin(), this->wrappers.end());
}

FilteredMotion::FilteredMotion(const MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers)
    : originalMotion(originalMotion), wrappers(wrappers)
{

}

FilteredMotion::~FilteredMotion()
{

}

const std::string & FilteredMotion::getName() const
{
    return originalMotion->getName();
}

const std::string & FilteredMotion::getLocalName() const
{
    return originalMotion->getLocalName();
}

SubjectID FilteredMotion::getID() const
{
    return originalMotion->getID();
}

SubjectID FilteredMotion::getLocalID() const
{
    return originalMotion->getLocalID();
}

const SessionConstPtr & FilteredMotion::getSession() const
{
    return session;
}

void FilteredMotion::setSession(const SessionConstPtr & session)
{
    this->session = session;
}

int FilteredMotion::size() const
{
    return wrappers.size();
}

const core::ObjectWrapperConstPtr & FilteredMotion::get(int i) const
{
    return wrappers[i];
}

const MotionConstPtr & FilteredMotion::getOriginalMotion() const
{
    return originalMotion;
}
