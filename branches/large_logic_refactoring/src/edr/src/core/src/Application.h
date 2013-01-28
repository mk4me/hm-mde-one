/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:55
    filename: Application.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATION_H__
#define HEADER_GUARD___APPLICATION_H__

#include <corelib/SmartPtr.h>
#include <corelib/BaseDataTypes.h>
#include <corelib/ILog.h>
#include <corelib/IVisualizer.h>
#include <corelib/ISource.h>
#include <corelib/IService.h>
#include <corelib/IParser.h>
#include "Plugin.h"

class QSplashScreen;

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
	class VisualizerManager;
	class ServiceManager;
	class SourceManager;
	class LogInitializer;
	class PluginLoader;

	class Application
	{
		friend class UIApplication;

	private:
		shared_ptr<Path> paths_;
		shared_ptr<LogInitializer> logInitializer_;
		LogPtr loggerPrototype_;
		LogPtr logger_;
		shared_ptr<PluginLoader> pluginLoader_;
		shared_ptr<DataHierarchyManager> dataHierarchyManager_;
		shared_ptr<MemoryDataManager> memoryDataManager_;
		shared_ptr<ParserManager> parserManager_;
		shared_ptr<StreamDataManager> streamDataManager_;
		shared_ptr<FileDataManager> fileDataManager_;
		shared_ptr<coreUI::UIApplication> uiApplication_;
		shared_ptr<ServiceManager> serviceManager_;
		shared_ptr<SourceManager> sourceManager_;
		shared_ptr<VisualizerManager> visualizerManager_;

		Filesystem::Path additionalPluginsPath;

	private:

		static bool trySetPathsFromRegistry(shared_ptr<Path> & path);
		static void setDefaultPaths(shared_ptr<Path> & path);
		static void showSplashScreenMessage(QSplashScreen * splashScreen, const QString & message);

		void safeRegisterService(const plugin::IServicePtr & service);
		void safeRegisterSource(const plugin::ISourcePtr & source);
		void safeRegisterParser(const plugin::IParserPtr & parser);
		void safeRegisterObjectWrapperPrototype(const ObjectWrapperPtr & prototype);
		void safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer);
		//void registerCoreDomainTypes();
		void unpackPlugin(coreUI::CoreMainWindow * splashScreen, const PluginPtr & plugin);

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

		void finalizeUI();
	};
}

#endif	//	HEADER_GUARD___APPLICATION_H__
