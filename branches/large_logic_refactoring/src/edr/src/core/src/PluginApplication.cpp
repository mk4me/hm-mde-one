#include "CorePCH.h"
#include "PluginApplication.h"
#include "PluginPath.h"

using namespace core;

PluginApplication::PluginApplication(const std::string & pluginName)
	: path_(new PluginPath(pluginName)),
	log_(getPrototypeLogInterface()->subLog("plugin." + pluginName))
{

}

IPath * PluginApplication::pathInterface()
{
	return path_.get();
}

IDataManagerReader * PluginApplication::dataManagerReader()
{
	return getMemoryDataManager();
}

IPluginStorage * PluginApplication::pluginStorage()
{
	return nullptr;
}

ILog * PluginApplication::log()
{
	return log_.get();
}

IServiceManager * PluginApplication::serviceManager()
{
	return getServiceManager();
}

IStreamManagerReader * PluginApplication::streamDataManagerReader()
{
	return getStreamDataManager();
}

IFileManagerReader * PluginApplication::fileDataManagerReader()
{
	return getFileDataManager();
}

IDataHierarchyManagerReader * PluginApplication::dataHierarchyManagerReader()
{
	return getDataHierarchyManager();
}

IVisualizerManager * PluginApplication::visualizerManager()
{
	return getVisualizerManager();
}