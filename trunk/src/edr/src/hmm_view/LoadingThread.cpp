#include "hmmPCH.h"
#include "LoadingThread.h"
#include <boost/bind.hpp>
#include <core/PluginCommon.h>
#include <QtCore/QElapsedTimer>
#include "DataManager.h"

LoadingThread::LoadingThread( const QString& directoryName) :
	filesToLoad(0),
	directoryName(directoryName),
	QThread()
{

}

LoadingThread::~LoadingThread()
{

}

void LoadingThread::run()
{
	std::vector<core::Filesystem::Path> paths;
	core::Filesystem::Path dirPath(directoryName.toStdString());
	core::Filesystem::Iterator itEnd;


	filesToLoad = 0;
	for( core::Filesystem::Iterator it(dirPath); it != itEnd; it++){
		std::string ext = core::Filesystem::fileExtension(*it);
		if(core::Filesystem::isRegularFile(*it) == true) {// && ext == ".c3d") {
			paths.push_back(it->path());
			filesToLoad++;
		}
	}        

	emit sendMinMax(0, filesToLoad);
	emit sendValue(0);
	
	filesToLoad = 0;

	QElapsedTimer myTimer;
	myTimer.start();
    
    for(auto it = paths.begin(); it != paths.end(); it++){
        std::string info((*it).filename().string());
        try{
            DataManager::getInstance()->addData(*it);
        }catch(std::exception & e){
            info.insert(0, "Error while adding file ");
            LOG_DEBUG(info);
        }
        emit sendFile(QString(info.c_str()));
        emit sendValue(++filesToLoad);
    }

	LOG_ERROR(myTimer.elapsed() / 1000.0f);

	emit loadingEnded();
}
