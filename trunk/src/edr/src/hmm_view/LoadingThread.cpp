#include "LoadingThread.h"
#include <boost/bind.hpp>
#include <core/PluginCommon.h>

LoadingThread::LoadingThread( const QString& directoryName ) :
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
		if(core::Filesystem::isRegularFile(*it) == true){
			//openFile((*it).path().string());
			paths.push_back(it->path());
			filesToLoad++;
		}
	}        

	emit sendMinMax(0, filesToLoad);
	emit sendValue(0);
	
	//dialog->progressBar->setMinimum(0);
	//dialog->progressBar->setMaximum(filesToLoad);
	//dialog->progressBar->setValue(0);
	filesToLoad = 0;
	core::IDataManager* dataManager = core::getDataManager();
	boost::function<void (const core::Filesystem::Path&, bool)> f = boost::bind(&LoadingThread::onFileLoaded, this, _1, _2);
	dataManager->addFileCallback(f);

	dataManager->loadFiles(paths);
	emit loadingEnded();
}

void LoadingThread::onFileLoaded( const core::Filesystem::Path& p, bool loaded )
{
	QString txt(p.filename().string().c_str());
	emit sendFile(txt);
	emit sendValue(++filesToLoad);
}
