#include "SubjectPCH.h"
#include "SubjectService.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/tokenizer.hpp>
#include <plugins/c3d/C3DChannels.h>
#include <core/IDataManager.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/Session.h>

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

SubjectService::SubjectService()
{
	core::IDataManager* dataManager = core::getDataManager();
	boost::function<void (const core::Filesystem::Path&, bool)> f1 = boost::bind(&SubjectService::onFileLoaded, this, _1, _2);
	dataManager->addFileCallback(f1);
	boost::function<void (const std::vector<core::ObjectWrapperPtr>&, bool)> f2 = boost::bind(&SubjectService::onWrappersAdded, this, _1, _2);
	dataManager->addWrappersCallback(f2);
}

SubjectService::~SubjectService()
{
	
}

void SubjectService::onFileLoaded( const core::Filesystem::Path& path, bool loaded)
{
	if (loaded) {
		try {
			getFileDescriptor(path.filename().string(), lastFileDescriptor);
		} catch (...) {
			LOG_WARNING("File : " << path.filename().string() << " has wrong name.");
		}
	}
}

void SubjectService::onWrappersAdded( const std::vector<core::ObjectWrapperPtr>& wrappers, bool loaded)
{
	if (lastFileDescriptor.hasSessionDesc()) {
		std::string sessionName = lastFileDescriptor.getSessionDesc();
	
		SessionPtr currentSession;
		auto it = sessions.find(sessionName);
		if (it != sessions.end()){
			currentSession = it->second;
		} else 	{
			currentSession = SessionPtr(new Session);
			currentSession->setName(sessionName);
			sessions[sessionName] = currentSession;
			core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Session>();
			wrapper->set(currentSession);
			core::getDataManager()->addExternalData(wrapper);
		}

		core::IDataManager* manager = core::getDataManager();
		if (currentSession) {
			bool hasMotionDesc = lastFileDescriptor.hasMotionDesc();
			if (!hasMotionDesc) {
				for (std::vector<core::ObjectWrapperPtr>::const_iterator it = wrappers.cbegin(); it != wrappers.cend(); it++) {
					if ((*it)->isPtrSupported(typeid(kinematic::SkeletalModelPtr))) {
						manager->tryParseWrapper(*it);
						kinematic::SkeletalModelConstPtr model = (*it)->get();
						if (model) {
							currentSession->setSkeletalModel(model);
							BOOST_FOREACH(MotionPtr motion, currentSession->getMotions()) {
								motion->setSkeletalModel(model);
							}
						}
					} 
				}
			} else  {
				std::string motionName = lastFileDescriptor.getMotionDesc();
				MotionPtr currentMotion;
				auto it = motions.find(motionName);
				if (it == motions.end()) {
					currentMotion = MotionPtr(new Motion);
					currentMotion->setName(motionName);
					currentSession->addMotion(currentMotion);
					motions[motionName] = currentMotion;
				} else {
					currentMotion = it->second;
				}

				
				for (std::vector<core::ObjectWrapperPtr>::const_iterator it = wrappers.cbegin(); it != wrappers.cend(); it++) {
					if ((*it)->isPtrSupported(typeid(EMGCollectionPtr))) {
						manager->tryParseWrapper(*it);
						currentMotion->setEmg((*it)->get());
					} else if ((*it)->isPtrSupported(typeid(GRFCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setGrf((*it)->get());
					} else if ((*it)->isPtrSupported(typeid(MarkerCollectionPtr))) {
						manager->tryParseWrapper(*it);
						currentMotion->setMarkers((*it)->get());
					} else if((*it)->isPtrSupported(typeid(kinematic::JointAnglesCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setJoints((*it)->get());
					} else if((*it)->isPtrSupported(typeid(AngleCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setAngles((*it)->get());
					} else if((*it)->isPtrSupported(typeid(ForceCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setForces((*it)->get());
					} else if((*it)->isPtrSupported(typeid(MomentCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setMoments((*it)->get());
					} else if((*it)->isPtrSupported(typeid(PowerCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setPowers((*it)->get());
					} else if((*it)->isPtrSupported(typeid(EventsCollectionPtr)) ) {
						manager->tryParseWrapper(*it);
						currentMotion->setEvents((*it)->get());
					} else if ((*it)->isPtrSupported(typeid(VideoChannelPtr))) {
						manager->tryParseWrapper(*it);
						currentMotion->addVideo((*it)->get());
					} else if ((*it)->isPtrSupported(typeid(kinematic::SkeletalDataPtr))) {
						manager->tryParseWrapper(*it);
						currentMotion->setSkeletalData((*it)->get());
						kinematic::SkeletalModelConstPtr model = currentSession->getSkeletalModel();
						if (model) {
							currentMotion->setSkeletalModel(model);
						}
					}
				}
			}
		}
	}
}

void SubjectService::getFileDescriptor(const std::string& filename, fileDescriptor& descriptor)
{
	descriptor.clear();
	typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	boost::char_separator<char> sep("-.");
	tokenizer tokens(filename, sep);
	tokenizer::iterator it = tokens.begin();
	descriptor.year = boost::lexical_cast<int>(*it); it++;
	descriptor.month = boost::lexical_cast<int>(*it); it++;
	descriptor.day = boost::lexical_cast<int>(*it); it++;
	descriptor.body = *it; it++;
	descriptor.session = *it; it++;
	std::string t = *it; it++;
	if (it == tokens.end()) {
		descriptor.extension = t;
	} else {
		descriptor.motion = t;
		t = *it; it++;
		if (it != tokens.end()) {
			descriptor.additional = t;
			descriptor.extension = (*it);
		} else {
			descriptor.extension = t;
		}
	}
}
