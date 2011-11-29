#include "SubjectPCH.h"

#include <OpenThreads/ScopedLock>

#include "SubjectService.h"
#include <plugins/c3d/C3DChannels.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/Session.h>
#include <plugins/subject/Subject.h>
#include <plugins/subject/Motion.h>

FilteredDataFacory::FilteredDataFacory()
{

}

FilteredDataFacory::~FilteredDataFacory()
{

}

MotionPtr FilteredDataFacory::createFilteredMotion(const MotionConstPtr & originalMotion, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const
{
    auto filteredMotion = core::dynamic_pointer_cast<const FilteredMotion>(originalMotion);
    MotionConstPtr trueOriginalMotion(originalMotion);

    if(filteredMotion != nullptr){
        trueOriginalMotion = filteredMotion->getOriginalMotion();
    }

    return MotionPtr(new FilteredMotion(trueOriginalMotion, wrappers));
}

SessionPtr FilteredDataFacory::createFilteredSession(const SessionConstPtr & originalSession, const std::vector<MotionPtr> & motions, const std::vector<core::ObjectWrapperConstPtr> & wrappers) const
{
    auto filteredSession = core::dynamic_pointer_cast<const FilteredSession>(originalSession);
    SessionConstPtr trueOriginalSession(originalSession);

    if(filteredSession != nullptr){
        trueOriginalSession = filteredSession->getOriginalSession();
    }

    SessionPtr ret(new FilteredSession(originalSession, motions, wrappers));

    for(auto it = motions.begin(); it != motions.end(); it++){
        core::dynamic_pointer_cast<FilteredMotion>(*it)->setSession(ret);
    }
    
    return ret;
}

SubjectService::SubjectService() : filteredDataFactory(new FilteredDataFacory), currentSubjectID(0), currentSessionID(0), currentMotionID(0)
{

}

SubjectService::~SubjectService()
{

}

void SubjectService::init(core::IManagersAccessor * managersAccessor)
{
    memoryDataManager = managersAccessor->getMemoryDataManager();
}

QWidget* SubjectService::getWidget( std::vector<QObject*>& actions )
{
	return nullptr;
}

QWidget* SubjectService::getSettingsWidget( std::vector<QObject*>& actions )
{
	return nullptr;
}

const std::string& SubjectService::getName() const
{
	static std::string s = "SubjectService";
	return s;
}

QWidget* SubjectService::getControlWidget( std::vector<QObject*>& actions )
{
	return nullptr;
}

SubjectPtr SubjectService::createSubject()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(subjectCreationMutex);

    if(currentSubjectID == std::numeric_limits<SubjectID>::max()){
        throw std::runtime_error("Subjects overflow");
    }

    SubjectPtr ret(new Subject(memoryDataManager, ++currentSubjectID));

    localSessionIDs[ret] = 0;

    return ret;
}

SessionPtr SubjectService::createSession(const SubjectConstPtr & subject, unsigned int year,
    unsigned char month, unsigned char day, const std::vector<core::ObjectWrapperConstPtr> & wrappers)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(sessionCreationMutex);

    if(currentSessionID == std::numeric_limits<SubjectID>::max()){
        throw std::runtime_error("Sessions overflow");
    }

    if(subject == nullptr){
        throw std::runtime_error("Wrong subject for session");
    }

    if(month < 1 || month > 12 || day < 1 || day > 31){
        throw std::runtime_error("Wrong session date");
    }

    SessionPtr ret(new Session(memoryDataManager, ++currentSessionID, subject,
        ++localSessionIDs[subject], year, month, day, wrappers));

    localMotionIDs[ret] = 0;

    return ret;
}

MotionPtr SubjectService::createMotion(const SessionConstPtr & session,
    const std::vector<core::ObjectWrapperConstPtr> & wrappers)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(motionCreationMutex);

    if(session == nullptr){
        throw std::runtime_error("Wrong session for motion");
    }

    MotionPtr ret(new Motion(memoryDataManager, ++currentMotionID, session,
        ++localMotionIDs[session], wrappers));

    return ret;
}

const FilteredDataFacoryPtr & SubjectService::getFilteredDataFacotry() const
{
    return filteredDataFactory;
}