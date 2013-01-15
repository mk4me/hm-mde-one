#include "CorePCH.h"
#include "Application.h"

namespace core{

Application::Application()
{

}

Application::~Application()
{

}

MemoryDataManager* Application::memoryDataManager()
{
	return memoryDataManager_.get();
}

StreamDataManager* Application::streamDataManager()
{
	return streamDataManager_.get();
}

FileDataManager* Application::fileDataManager()
{
	return fileDataManager_.get();
}

Log* Application::logger()
{
	return logger_.get();
}

Log* Application::loggerPrototype()
{
	return loggerPrototype_.get();
}

Path* Application::paths()
{
	return paths_.get();
}

DataHierarchyManager* Application::dataHierarchyManager()
{
	return dataHierarchyManager_.get();
}

ParserManager* Application::parserManager()
{
	return parserManager_.get();
}

VisualizerManager* Application::visualizerManager()
{
	return visualizerManager_.get();
}

DataSourceManager* Application::dataSourceManager()
{
	return dataSourceManager_.get();
}

DataSinkManager* Application::dataSinkManager()
{
	return dataSinkManager_.get();
}

DataProcessorManager* Application::dataProcessorManager()
{
	return dataProcessorManager_.get();
}

ServiceManager* Application::serviceManager()
{
	return serviceManager_.get();
}

SourceManager* Application::sourceManager()
{
	return sourceManager_.get();
}

}