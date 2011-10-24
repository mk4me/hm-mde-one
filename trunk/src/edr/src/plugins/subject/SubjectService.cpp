#include "SubjectPCH.h"
#include "SubjectService.h"
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/tokenizer.hpp>
#include <plugins/c3d/C3DChannels.h>
#include <core/IDataManager.h>
#include <core/ObjectWrapper.h>
#include <plugins/subject/Session.h>

SubjectService::SubjectService()
{

}

SubjectService::~SubjectService()
{

}

void SubjectService::init(core::IManagersAccessor * managersAccessor)
{
    memoryDataManager = managersAccessor->getMemoryDataManager();
    managersAccessor->getFileDataManager()->attach(this);
}

void SubjectService::update(const core::IFileDataManager * fileDataManager)
{
    core::Files tmpFiles;
    fileDataManager->getManagedData(tmpFiles);

    std::vector<core::Filesystem::Path> difference(std::max(files.size(), tmpFiles.size()));

    auto lastIT = std::set_difference(tmpFiles.begin(), tmpFiles.end(), files.begin(), files.end(), difference.begin());

    //sprawdzamy czy coœ dosz³o
    if(lastIT == difference.begin()){

        //nic nie dzoszlo - usunieto jakies dane
        //TODO

    }else{

        for(auto fileIT = difference.begin(); fileIT != lastIT; fileIT++){
            
            try {
                fileDescriptor fDesc;
                getFileDescriptor((*fileIT).filename().string(), fDesc);

                if (fDesc.hasSessionDesc()) {

                    core::Objects wrappers;
                    fileDataManager->getObjectsForData(*fileIT, wrappers);

                    std::string sessionName = fDesc.getSessionDesc();

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
                        memoryDataManager->addData(wrapper);
                    }

                    if (currentSession) {
                        bool hasMotionDesc = fDesc.hasMotionDesc();
                        if (!hasMotionDesc) {
                            for (auto it = wrappers.cbegin(); it != wrappers.cend(); it++) {
                                currentSession->addWrapper(*it);
                            }
                        } else  {
                            std::string motionName = fDesc.getMotionDesc();
                            MotionPtr currentMotion;
                            auto it = motions.find(motionName);
                            if (it == motions.end()) {
                                currentMotion = MotionPtr(new Motion(memoryDataManager));
                                currentMotion->setName(motionName);
                                currentSession->addMotion(currentMotion);
                                motions[motionName] = currentMotion;
                            } else {
                                currentMotion = it->second;
                            }

                            UTILS_ASSERT(currentMotion);
                            for (auto it = wrappers.cbegin(); it != wrappers.cend(); it++) {
                                const std::string& ext = core::Filesystem::fileExtension(*fileIT);
                                if (currentMotion->isSupported((*it)->getTypeInfo())) {
                                    if (ext == ".c3d") {
                                        const_cast<core::IFileDataManager*>(fileDataManager)->initializeData(*fileIT);
                                    } else {
                                        (*it)->setName((*fileIT).filename().string());
                                    }
                                    currentMotion->addWrapper(*it);
                                }
                            }
                        }
                    }
                }

            } catch (...) {
                LOG_WARNING("File : " << *fileIT << " has wrong name. Could not create any subjects and motions.");
            }
        }
    }

    files = tmpFiles;
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

//void SubjectService::onFileLoaded( const core::Filesystem::Path& path, bool loaded)
//{
//	if (loaded) {
//		try {
//			getFileDescriptor(path.filename().string(), lastFileDescriptor);
//		} catch (...) {
//			LOG_WARNING("File : " << path.filename().string() << " has wrong name.");
//		}
//	}
//}
//
//void SubjectService::onWrappersAdded(const core::Filesystem::Path& path, const std::vector<core::ObjectWrapperPtr>& wrappers, bool loaded)
//{
//	onFileLoaded(path, loaded);
//	if (lastFileDescriptor.hasSessionDesc()) {
//		std::string sessionName = lastFileDescriptor.getSessionDesc();
//	
//		SessionPtr currentSession;
//		auto it = sessions.find(sessionName);
//		if (it != sessions.end()){
//			currentSession = it->second;
//		} else 	{
//			currentSession = SessionPtr(new Session);
//			currentSession->setName(sessionName);
//			sessions[sessionName] = currentSession;
//			core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<Session>();
//			wrapper->set(currentSession);
//			core::getDataManager()->addExternalData(wrapper);
//		}
//
//		core::IDataManager* manager = core::getDataManager();
//		if (currentSession) {
//			bool hasMotionDesc = lastFileDescriptor.hasMotionDesc();
//			if (!hasMotionDesc) {
//				for (std::vector<core::ObjectWrapperPtr>::const_iterator it = wrappers.cbegin(); it != wrappers.cend(); it++) {
//                    currentSession->addWrapper(*it);
//				}
//			} else  {
//				std::string motionName = lastFileDescriptor.getMotionDesc();
//				MotionPtr currentMotion;
//				auto it = motions.find(motionName);
//				if (it == motions.end()) {
//					currentMotion = MotionPtr(new Motion);
//					currentMotion->setName(motionName);
//					currentSession->addMotion(currentMotion);
//					motions[motionName] = currentMotion;
//				} else {
//					currentMotion = it->second;
//				}
//
//                UTILS_ASSERT(currentMotion);
//				for (std::vector<core::ObjectWrapperPtr>::const_iterator it = wrappers.cbegin(); it != wrappers.cend(); it++) {
//					const std::string& ext = core::Filesystem::fileExtension(path);
//                    if (currentMotion->isSupported((*it)->getTypeInfo())) {
//                        if (ext == ".c3d") {
//                            manager->tryParseWrapper(*it);
//                        } else {
//                            (*it)->setName(path.filename().string());
//                        }
//                        currentMotion->addWrapper(*it);
//                    }
//				}
//			}
//		}
//	}
//}

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
