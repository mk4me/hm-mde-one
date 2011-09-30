#include "LoadingThread.h"
#include <boost/bind.hpp>
#include <core/PluginCommon.h>
#include <QtCore/QElapsedTimer>

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
		std::string ext = core::Filesystem::fileExtension(*it);
		if(core::Filesystem::isRegularFile(*it) == true) {// && ext == ".c3d") {
			paths.push_back(it->path());
			filesToLoad++;
		}
	}        

	emit sendMinMax(0, filesToLoad);
	emit sendValue(0);
	
	filesToLoad = 0;
	core::IDataManager* dataManager = core::getDataManager();
	boost::function<void (const core::Filesystem::Path&, const std::vector<core::ObjectWrapperPtr>&, bool)> f = boost::bind(&LoadingThread::onFileLoaded, this, _1, _2, _3);
	dataManager->addWrappersCallback(f);

	QElapsedTimer myTimer;
	myTimer.start();
	dataManager->loadFiles(paths);
	LOG_ERROR(myTimer.elapsed() / 1000.0f);

	emit loadingEnded();
}

void LoadingThread::onFileLoaded( const core::Filesystem::Path& p, const std::vector<core::ObjectWrapperPtr>&, bool loaded )
{
	QString txt(p.filename().string().c_str());
	auto it = filesSended.find(txt);
	if (it == filesSended.end()) {
		emit sendFile(txt);
		emit sendValue(++filesToLoad);
		filesSended.insert(txt);
	}
	
}
