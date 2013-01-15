/********************************************************************
    created:  2013/01/14
    created:  14:1:2013   16:55
    filename: Application.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___APPLICATION_H__
#define HEADER_GUARD___APPLICATION_H__

#include <core/SmartPtr.h>

namespace core {

	class MemoryDataManager;
	class StreamDataManager;
	class FileDataManager;
	class Log;
	class Path;
	class DataHierarchyManager;
	class ParserManager;
	class VisualizerManager;
	class DataSourceManager;
	class DataSinkManager;
	class DataProcessorManager;
	class ServiceManager;
	class SourceManager;

	class Application
	{
	private:
		shared_ptr<MemoryDataManager> memoryDataManager_;
		shared_ptr<StreamDataManager> streamDataManager_;
		shared_ptr<FileDataManager> fileDataManager_;
		shared_ptr<Log> logger_;
		shared_ptr<Log> loggerPrototype_;
		shared_ptr<Path> paths_;
		shared_ptr<DataHierarchyManager> dataHierarchyManager_;
		shared_ptr<ParserManager> parserManager_;
		shared_ptr<VisualizerManager> visualizerManager_;
		shared_ptr<DataSourceManager> dataSourceManager_;
		shared_ptr<DataSinkManager> dataSinkManager_;
		shared_ptr<DataProcessorManager> dataProcessorManager_;
		shared_ptr<ServiceManager> serviceManager_;
		shared_ptr<SourceManager> sourceManager_;

	public:
		Application();
		~Application();

		MemoryDataManager* memoryDataManager();
		StreamDataManager* streamDataManager();
		FileDataManager* fileDataManager();
		Log* logger();
		Log* loggerPrototype();
		Path* paths();
		DataHierarchyManager* dataHierarchyManager();
		ParserManager* parserManager();
		VisualizerManager* visualizerManager();
		DataSourceManager* dataSourceManager();
		DataSinkManager* dataSinkManager();
		DataProcessorManager* dataProcessorManager();
		ServiceManager* serviceManager();
		SourceManager* sourceManager();
	
	};

}

#endif	//	HEADER_GUARD___APPLICATION_H__
