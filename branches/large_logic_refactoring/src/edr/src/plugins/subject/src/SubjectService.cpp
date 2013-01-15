#include "SubjectPCH.h"
#include "SubjectService.h"

#include <limits>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <core/ObjectWrapper.h>
#include "Session.h"
#include "Subject.h"
#include "Motion.h"

OpenThreads::Mutex subjectCreationMutex;
OpenThreads::Mutex sessionCreationMutex;
OpenThreads::Mutex motionCreationMutex;

using namespace PluginSubject;

FilteredDataFacory::FilteredDataFacory()
{

}

FilteredDataFacory::~FilteredDataFacory()
{

}

core::ObjectWrapperPtr FilteredDataFacory::createFilteredMotion(const core::ObjectWrapperConstPtr & originalMotion, const core::ConstObjectsList & wrappers) const
{
	core::ObjectWrapperPtr ret;
	PluginSubject::MotionConstPtr motion;
	bool ok = originalMotion->tryGet(motion, false);
	if(ok && motion != nullptr){
		auto filteredMotion = core::dynamic_pointer_cast<const FilteredMotion>(motion);

		ret = core::ObjectWrapper::create<PluginSubject::IMotion>();
		ret->copyMeta(*originalMotion);
		ret->set(MotionPtr(new FilteredMotion(filteredMotion != nullptr ? filteredMotion->getOriginalMotion() : originalMotion, filteredMotion != nullptr ? filteredMotion->getOriginalUnpackedMotion() : motion, wrappers)));
	}
	return ret;
}

core::ObjectWrapperPtr FilteredDataFacory::createFilteredSession(const core::ObjectWrapperConstPtr & originalSession, const core::ConstObjectsList & motions, const core::ConstObjectsList & wrappers) const
{
	core::ObjectWrapperPtr ret;
	PluginSubject::SessionConstPtr session;
	bool ok = originalSession->tryGet(session, false);
	if(ok && session != nullptr){
		auto filteredSession = core::dynamic_pointer_cast<const FilteredSession>(session);

		ret = core::ObjectWrapper::create<PluginSubject::ISession>();
		ret->copyMeta(*originalSession);
		ret->set(SessionPtr(new FilteredSession(filteredSession != nullptr ? filteredSession->getOriginalSession() : originalSession,
			filteredSession != nullptr ? filteredSession->getOriginalUnpackedSession() : session, motions, wrappers)));

		//TODO
		//ustawic sesje motionów

	}
    
    return ret;
}

SubjectService::SubjectService() : filteredDataFactory(new FilteredDataFacory)
{

}

SubjectService::~SubjectService()
{

}

void SubjectService::init(core::ISourceManager * sourceManager,
	core::IDataSourceManager * dataSourceManager,
	core::IDataProcessorManager *dataProcessorManager,
	core::IDataSinkManager * dataSinkManager,
	core::IVisualizerManager * visualizerManager,
	core::IMemoryDataManager * memoryDataManager,
	core::IFileDataManager * fileDataManager)
{

}

QWidget* SubjectService::getWidget( core::IActionsGroupManager * actionsManager )
{
	return nullptr;
}

QWidget* SubjectService::getSettingsWidget( core::IActionsGroupManager * actionsManager )
{
	return nullptr;
}

const std::string& SubjectService::getName() const
{
	static std::string s = "SubjectService";
	return s;
}

QWidget* SubjectService::getControlWidget( core::IActionsGroupManager * actionsManager )
{
	return nullptr;
}

core::ObjectWrapperPtr SubjectService::createSubject()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(subjectCreationMutex);
	core::ObjectWrapperPtr ret = core::ObjectWrapper::create<PluginSubject::ISubject>();
	ret->set(SubjectPtr(new Subject()));
    return ret;
}

core::ObjectWrapperPtr SubjectService::createSession(const core::ObjectWrapperConstPtr & subject, const core::ConstObjectsList & wrappers)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(sessionCreationMutex);	

    if(subject == nullptr || subject->isNull() == true){
        throw std::runtime_error("Wrong subject for session");
    }

	PluginSubject::SubjectConstPtr unpackedSubject;
	bool ok = subject->tryGet(unpackedSubject, false);

	if(!ok || unpackedSubject == nullptr){
		throw std::runtime_error("Subject not passed in wrapper");
	}

	auto subImpl = core::dynamic_pointer_cast<const Subject>(unpackedSubject);

	if(subImpl == nullptr){
		throw std::runtime_error("Other subject implementation passed in wrapper");
	}

	core::ObjectWrapperPtr ret = core::ObjectWrapper::create<PluginSubject::ISession>();
	SessionPtr session(new Session(subject,unpackedSubject, subImpl->nextSessionID(), wrappers));
	ret->set(session);
	(*ret)["name"] = session->getName();

    return ret;
}

core::ObjectWrapperPtr SubjectService::createMotion(const core::ObjectWrapperConstPtr & session,
    const core::ConstObjectsList & wrappers)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(motionCreationMutex);

	if(session == nullptr || session->isNull() == true){
		throw std::runtime_error("Wrong session for motion");
	}

	PluginSubject::SessionConstPtr unpackedSession;
	bool ok = session->tryGet(unpackedSession, false);

	if(!ok || unpackedSession == nullptr){
		throw std::runtime_error("Session not passed in wrapper");
	}

	auto sessionImpl = core::dynamic_pointer_cast<const Session>(unpackedSession);

	if(sessionImpl == nullptr){
		throw std::runtime_error("Other session implementation passed in wrapper");
	}

	core::ObjectWrapperPtr ret = core::ObjectWrapper::create<PluginSubject::IMotion>();
	MotionPtr motion(new Motion(session,unpackedSession, sessionImpl->nextMotionID(), wrappers));
	ret->set(motion);
	(*ret)["name"] = motion->getName();

	return ret;
}

const FilteredDataFacoryPtr & SubjectService::getFilteredDataFacotry() const
{
    return filteredDataFactory;
}
