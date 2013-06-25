#include "CorePCH.h"
#include "PluginPath.h"
#include "ApplicationCommon.h"

using namespace core;

PluginPath::PluginPath(const std::string & pluginName)
	: pluginPath(getPathInterface()->getPluginPath()/pluginName),
	pluginTmpPath(getPathInterface()->getTmpPath()/pluginName)
{
	Filesystem::createDirectory(pluginTmpPath);
}

const Filesystem::Path& PluginPath::getUserDataPath() const
{
	return getPathInterface()->getUserDataPath();
}

const Filesystem::Path& PluginPath::getApplicationDataPath() const
{
	return getPathInterface()->getApplicationDataPath();
}

const Filesystem::Path& PluginPath::getUserApplicationDataPath() const
{
	return getPathInterface()->getUserApplicationDataPath();
}

const Filesystem::Path& PluginPath::getResourcesPath() const
{
	return getPathInterface()->getResourcesPath();
}

const Filesystem::Path& PluginPath::getTmpPath() const
{
	return pluginTmpPath;
}

const Filesystem::Path& PluginPath::getPluginPath() const
{
	return pluginPath;
}
