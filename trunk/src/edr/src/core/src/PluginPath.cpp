#include "CorePCH.h"
#include "PluginPath.h"
#include "ApplicationCommon.h"
#include "Path.h"

using namespace core;

PluginPath::PluginPath(const std::string & pluginName)
	: pluginPath(getPaths()->getPluginPath()/pluginName),
	pluginTmpPath(getPaths()->getTmpPath()/pluginName)
{
	Filesystem::createDirectory(pluginTmpPath);
}

PluginPath::~PluginPath()
{

}

const Filesystem::Path& PluginPath::getUserDataPath() const
{
	return getPaths()->getUserDataPath();
}

const Filesystem::Path& PluginPath::getApplicationDataPath() const
{
	return getPaths()->getApplicationDataPath();
}

const Filesystem::Path& PluginPath::getUserApplicationDataPath() const
{
	return getPaths()->getUserApplicationDataPath();
}

const Filesystem::Path& PluginPath::getResourcesPath() const
{
	return getPaths()->getResourcesPath();
}

const Filesystem::Path& PluginPath::getTmpPath() const
{
	return pluginTmpPath;
}

const Filesystem::Path& PluginPath::getPluginPath() const
{
	return pluginPath;
}

const Filesystem::Path PluginPath::generateTempFileName() const
{
	return Path::TempFileName();
}
