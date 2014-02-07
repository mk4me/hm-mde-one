/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:55
    filename: Application.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATION_H__
#define HEADER_GUARD___APPLICATION_H__

#include <utils/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/ILog.h>
#include <corelib/IVisualizer.h>
#include <corelib/ISource.h>
#include <corelib/IService.h>
#include <corelib/IParser.h>
#include "Plugin.h"
#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <threading/QtThreadFactory.h>

class QSplashScreen;
class QTranslator;

namespace coreUI {

	class UIApplication;
	class CoreMainWindow;
	
}

namespace core {

	class MemoryDataManager;
	class StreamDataManager;
	class FileDataManager;
	class Log;
	class Path;
	class DataHierarchyManager;
	class ParserManager;
	class LogInitializer;
	class PluginLoader;
	class VisualizerManager;
	class SourceManager;
	class ServiceManager;
	class ThreadPool;
	class JobManager;

	class Application : private QObject
	{
		Q_OBJECT;


		friend class UIApplication;

	private:
		utils::shared_ptr<Path> paths_;
		utils::shared_ptr<LogInitializer> logInitializer_;
		LogPtr loggerPrototype_;
		LogPtr logger_;
		utils::shared_ptr<PluginLoader> pluginLoader_;
		utils::shared_ptr<DataHierarchyManager> dataHierarchyManager_;
		utils::shared_ptr<MemoryDataManager> memoryDataManager_;
		utils::shared_ptr<ParserManager> parserManager_;
		utils::shared_ptr<StreamDataManager> streamDataManager_;
		utils::shared_ptr<FileDataManager> fileDataManager_;
		utils::shared_ptr<coreUI::UIApplication> uiApplication_;
		std::vector<utils::shared_ptr<QTranslator>> translators_;
		utils::shared_ptr<ServiceManager> serviceManager_;
		utils::shared_ptr<SourceManager> sourceManager_;
		utils::shared_ptr<VisualizerManager> visualizerManager_;
		shared_ptr<utils::IThreadFactory> threadFactory_;
		shared_ptr<core::ThreadPool> threadPool_;
		shared_ptr<JobManager> jobManager_;

		Filesystem::Path additionalPluginsPath;

		QTimer visualizerUpdateTimer;
		QTimer servicesUpdateTimer;

		double visualizerTimeDelta;
		double servicesTimeDelta;

		coreUI::CoreMainWindow * mainWindow;

		bool uiInit;

	private:

		static bool trySetPathsFromRegistry(utils::shared_ptr<Path> & path);
		static void setDefaultPaths(utils::shared_ptr<Path> & path);
		void showSplashScreenMessage(const QString & message);

		void safeRegisterService(const plugin::IServicePtr & service);
		void safeRegisterSource(const plugin::ISourcePtr & source);
		void safeRegisterParser(const plugin::IParserPtr & parser);
		void safeRegisterObjectWrapperPrototype(const ObjectWrapperPtr & prototype);
		void safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer);
		//void registerCoreDomainTypes();
		void unpackPlugin(coreUI::CoreMainWindow * splashScreen, const PluginPtr & plugin);

	private slots:

		void updateServices();

		void updateVisualizers();

	public:
		Application();
		~Application();

		int initUIContext(int & argc, char *argv[]);

		void initWithUI(coreUI::CoreMainWindow * mainWindow);

		int run();

		MemoryDataManager* memoryDataManager();
		StreamDataManager* streamDataManager();
		FileDataManager* fileDataManager();
		ILog* logger();
		ILog* loggerPrototype();
		Path* paths();
		DataHierarchyManager* dataHierarchyManager();
		ParserManager* parserManager();
		VisualizerManager* visualizerManager();
		ServiceManager* serviceManager();
		SourceManager* sourceManager();
		ThreadPool* threadPool();
		JobManager* jobManager();

		void finalizeUI();
	};


}

#endif	//	HEADER_GUARD___APPLICATION_H__
