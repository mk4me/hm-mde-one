#include "SubjectPCH.h"
#include "SubjectService.h"

#include <limits>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <corelib/BaseDataTypes.h>
#include "Session.h"
#include "Subject.h"
#include "Motion.h"

OpenThreads::Mutex subjectCreationMutex;
OpenThreads::Mutex sessionCreationMutex;
OpenThreads::Mutex motionCreationMutex;

using namespace PluginSubject;

SubjectService::SubjectService()
{

}

SubjectService::~SubjectService()
{

}

void SubjectService::init(core::ISourceManager * sourceManager,
	core::IVisualizerManager * visualizerManager,
	core::IMemoryDataManager * memoryDataManager,
	core::IStreamDataManager * streamDataManager,
	core::IFileDataManager * fileDataManager)
{

}

QWidget* SubjectService::getWidget()
{
	return nullptr;
}


QWidgetList SubjectService::getPropertiesWidgets()
{
    return QWidgetList();
}


core::VariantPtr SubjectService::createSubject()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(subjectCreationMutex);
	core::VariantPtr ret = core::Variant::create<PluginSubject::ISubject>();
	ret->set(SubjectPtr(new Subject()));
    return ret;
}

core::VariantPtr SubjectService::createSession(const core::VariantConstPtr & subject)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(sessionCreationMutex);	

    if(subject == nullptr || subject->getRawPtr() == nullptr){
        throw std::runtime_error("Wrong subject for session");
    }

	PluginSubject::SubjectConstPtr unpackedSubject;
	bool ok = subject->tryGet(unpackedSubject, false);

	if(!ok || unpackedSubject == nullptr){
		throw std::runtime_error("Subject not passed in wrapper");
	}

	auto subImpl = utils::dynamic_pointer_cast<const Subject>(unpackedSubject);

	if(subImpl == nullptr){
		throw std::runtime_error("Other subject implementation passed in wrapper");
	}

	core::VariantPtr ret = core::Variant::create<PluginSubject::ISession>();
	SessionPtr session(new Session(subject,unpackedSubject, subImpl->nextSessionID()));
	ret->set(session);
	ret->setMetadata("name", session->getName());

    return ret;
}

core::VariantPtr SubjectService::createMotion(const core::VariantConstPtr & session)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(motionCreationMutex);

	if(session == nullptr || session->getRawPtr() == nullptr){
		throw std::runtime_error("Wrong session for motion");
	}

	PluginSubject::SessionConstPtr unpackedSession;
	bool ok = session->tryGet(unpackedSession, false);

	if(!ok || unpackedSession == nullptr){
		throw std::runtime_error("Session not passed in wrapper");
	}

	auto sessionImpl = utils::dynamic_pointer_cast<const Session>(unpackedSession);

	if(sessionImpl == nullptr){
		throw std::runtime_error("Other session implementation passed in wrapper");
	}

	core::VariantPtr ret = core::Variant::create<PluginSubject::IMotion>();
	MotionPtr motion(new Motion(session,unpackedSession, sessionImpl->nextMotionID()));
	ret->set(motion);
	ret->setMetadata("name", motion->getName());

	return ret;
}
