#include "CorePCH.h"
#include "CustomPath.h"

namespace core {

CustomPath::CustomPath(plugin::IPath * path, const std::string & folderName) : path(path), pluginPath(path->getPluginPath()/folderName),
	pluginTmpPath(path->getTmpPath()/folderName)
{

}

const Filesystem::Path& CustomPath::getUserDataPath() const
{
	return path->getUserDataPath();
}

const Filesystem::Path& CustomPath::getApplicationDataPath() const
{
	return path->getApplicationDataPath();
}

const Filesystem::Path& CustomPath::getResourcesPath() const
{
	return path->getResourcesPath();
}

const Filesystem::Path& CustomPath::getTmpPath() const
{
	return pluginTmpPath;
}

const Filesystem::Path& CustomPath::getPluginPath() const
{
	return pluginPath;
}

}