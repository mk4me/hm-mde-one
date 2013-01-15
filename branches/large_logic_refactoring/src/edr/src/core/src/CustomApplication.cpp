#include "CustomApplication.h"

using namespace core;

CustomApplication::CustomApplication(plugin::IPath * pathInterface,	ILog * log,
	plugin::IDataManagerReader * dataManagerReader,
	plugin::IStreamDataManagerReader * streamDataManagerReader,
	plugin::IFileDataManagerReader * fileDataManagerReader,
	plugin::IDataHierarchyManagerReader * dataHierarchyManagerReader,
	plugin::IExtensionManagerReader * dataExtensionManagerReader,
	plugin::IPluginStorage * pluginStorage,
	plugin::IServiceManager * serviceManager)
	: pathInterface_(pathInterface), log_(log), dataManagerReader_(dataManagerReader),
	streamDataManagerReader_(streamDataManagerReader), fileDataManagerReader_(fileDataManagerReader),
	dataHierarchyManagerReader_(dataHierarchyManagerReader), dataExtensionManagerReader_(dataExtensionManagerReader),
	pluginStorage_(pluginStorage), serviceManager_(serviceManager)
{

}

plugin::IPath * CustomApplication::pathInterface()
{
	return pathInterface_;
}

plugin::IDataManagerReader * CustomApplication::dataManagerReader()
{
	return dataManagerReader_;
}

plugin::IPluginStorage * CustomApplication::pluginStorage()
{
	return pluginStorage_;
}

ILog * CustomApplication::log()
{
	return log_;
}

plugin::IServiceManager * CustomApplication::serviceManager()
{
	return serviceManager_;
}

ExtendedCustomApplication::ExtendedCustomApplication(plugin::IPath * pathInterface, MemoryDataManager * memoryDataManager,
	StreamDataManager * streamDataManager, FileDataManager * fileDataManager,
	plugin::IPluginStorage * pluginStorage, core::ILog * log, VisualizerManager * visualizerManager,
	ServiceManager * serviceManager, SourceManager * sourceManager,
	DataSourceManager * dataSourceManager, DataProcessorManager * dataProcessorManager,
	DataSinkManager * dataSinkManager)
	: pathInterface_(pathInterface), memoryDataManager_(memoryDataManager), streamDataManager_(streamDataManager),
	fileDataManager_(fileDataManager), pluginStorage_(pluginStorage), log_(log), visualizerManager_(visualizerManager),
	serviceManager_(serviceManager), sourceManager_(sourceManager),	dataSourceManager_(dataSourceManager),
	dataProcessorManager_(dataProcessorManager), dataSinkManager_(dataSinkManager)
{

}

plugin::IPath * ExtendedCustomApplication::pathInterface()
{
	return pathInterface_;
}

plugin::IDataManagerReader * ExtendedCustomApplication::dataManagerReader()
{
	return memoryDataManager_;
}

MemoryDataManager * ExtendedCustomApplication::memoryDataManager()
{
	return memoryDataManager_;
}

StreamDataManager * ExtendedCustomApplication::streamDataManager()
{
	return streamDataManager_;
}

FileDataManager * ExtendedCustomApplication::fileDataManager()
{
	return fileDataManager_;
}

plugin::IPluginStorage * ExtendedCustomApplication::pluginStorage()
{
	return pluginStorage_;
}

ILog * ExtendedCustomApplication::log()
{
	return log_;
}

ServiceManager * ExtendedCustomApplication::serviceManager()
{
	return serviceManager_;
}

SourceManager * ExtendedCustomApplication::sourceManager()
{
	return sourceManager_;
}

DataSourceManager * ExtendedCustomApplication::dataSourceManager()
{
	return dataSourceManager_;
}

DataProcessorManager * ExtendedCustomApplication::dataProcessorManager()
{
	return dataProcessorManager_;
}

DataSinkManager * ExtendedCustomApplication::dataSinkManager()
{
	return dataSinkManager_;
}

VisualizerManager * ExtendedCustomApplication::visualizerManager()
{
	return visualizerManager_;
}