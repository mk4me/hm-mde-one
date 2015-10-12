#include "CorePCH.h"
#include "Path.h"

using namespace core;

Path::Path(const Filesystem::Path & userDataPath,
	const Filesystem::Path & applicationDataPath,
	const Filesystem::Path & userApplicationDataPath,
	const Filesystem::Path & resourcesPath,
	const Filesystem::Path & tempPath,
	const Filesystem::Path & pluginPath)
	: translationsPath_(resourcesPath / "lang"),
	  userDataPath_(userDataPath),
	  applicationDataPath_(applicationDataPath),
	  userApplicationDataPath_(userApplicationDataPath),
	  resourcesPath_(resourcesPath),
	  tempPath_(tempPath),
	  pluginPath_(pluginPath)
{
	translationsPath_.make_preferred();
	userDataPath_.make_preferred();
	applicationDataPath_.make_preferred();
	userApplicationDataPath_.make_preferred();
	resourcesPath_.make_preferred();
	tempPath_.make_preferred();
	pluginPath_.make_preferred();	
}

Path::~Path()
{

}

const Filesystem::Path& Path::getUserDataPath() const
{
	return userDataPath_;
}

const Filesystem::Path& Path::getApplicationDataPath() const
{
	return applicationDataPath_;
}


const Filesystem::Path& Path::getUserApplicationDataPath() const
{
	return userApplicationDataPath_;
}

const Filesystem::Path& Path::getResourcesPath() const
{
	return resourcesPath_;
}

const Filesystem::Path& Path::getTempPath() const
{
	return tempPath_;
}

const Filesystem::Path& Path::getPluginPath() const
{
	return pluginPath_;
}

const Filesystem::Path& Path::getTranslationsPath() const
{
	return translationsPath_;
}