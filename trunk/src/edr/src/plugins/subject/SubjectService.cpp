#include "SubjectPCH.h"
#include "SubjectService.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/tokenizer.hpp>
#include <plugins/c3d/C3DChannels.h>
#include <core/IDataManager.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/Session.h>
#include <plugins/allc3d/AllC3DChannels.h>

SubjectService::SubjectService()
{
	core::IDataManager* dataManager = core::getDataManager();
	boost::function<void (const core::Filesystem::Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> f2 = boost::bind(&SubjectService::onWrappersAdded, this, _1, _2, _3);
	dataManager->addWrappersCallback(f2);
}

SubjectService::~SubjectService()
{

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

void SubjectService::onWrappersAdded(const core::Filesystem::Path& path, const std::vector<core::ObjectWrapperPtr>& wrappers, bool loaded)
{
	onFileLoaded(path, loaded);
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
                    currentSession->addWrapper(*it);
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

                UTILS_ASSERT(currentMotion);
				for (std::vector<core::ObjectWrapperPtr>::const_iterator it = wrappers.cbegin(); it != wrappers.cend(); it++) {
					const std::string& ext = core::Filesystem::fileExtension(path);
                    if (currentMotion->isSupported((*it)->getTypeInfo())) {
                        if (ext == ".c3d") {
                            manager->tryParseWrapper(*it);
                        } else {
                            (*it)->setName(path.filename().string());
                        }
                        currentMotion->addWrapper(*it);
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
