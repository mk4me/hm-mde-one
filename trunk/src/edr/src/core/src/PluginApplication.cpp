#include "CorePCH.h"
#include "PluginApplication.h"
#include "ApplicationCommon.h"
#include "PluginPath.h"
#include "ParserManager.h"
#include "DataHierarchyManager.h"

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

ISourceManager * PluginApplication::sourceManager()
{
	return getSourceManager();
}

IServiceManager * PluginApplication::serviceManager()
{
	return getServiceManager();
}

IParserManagerReader * PluginApplication::parserManagerReader()
{
	return getParserManager();
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



IPath * MainViewApplication::pathInterface()
{
	return getPathInterface();
}

IDataManagerReader * MainViewApplication::dataManagerReader()
{
	return getMemoryDataManager();
}

IPluginStorage * MainViewApplication::pluginStorage()
{
	return nullptr;
}

ILog * MainViewApplication::log()
{
	return getLogInterface();
}

ISourceManager * MainViewApplication::sourceManager()
{
	return getSourceManager();
}

IServiceManager * MainViewApplication::serviceManager()
{
	return getServiceManager();
}

IParserManagerReader * MainViewApplication::parserManagerReader()
{
	return getParserManager();
}

IStreamManagerReader * MainViewApplication::streamDataManagerReader()
{
	return getStreamDataManager();
}

IFileManagerReader * MainViewApplication::fileDataManagerReader()
{
	return getFileDataManager();
}

IDataHierarchyManagerReader * MainViewApplication::dataHierarchyManagerReader()
{
	return getDataHierarchyManager();
}

IVisualizerManager * MainViewApplication::visualizerManager()
{
	return getVisualizerManager();
}
