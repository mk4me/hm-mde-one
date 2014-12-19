#include "CorePCH.h"
#include "Path.h"
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <chrono>
#include <thread>
#include <boost/lexical_cast.hpp>

using namespace core;

Path::Path(const Filesystem::Path & userDataPath,
	const Filesystem::Path & applicationDataPath,
	const Filesystem::Path & userApplicationDataPath,
	const Filesystem::Path & resourcesPath,
	const Filesystem::Path & tmpPath,
	const Filesystem::Path & pluginPath)
	: translationsPath_(resourcesPath / "lang"),
	  userDataPath_(userDataPath),
	  applicationDataPath_(applicationDataPath),
	  userApplicationDataPath_(userApplicationDataPath),
	  resourcesPath_(resourcesPath),
	  tmpPath_(tmpPath),
	  pluginPath_(pluginPath)
{
	translationsPath_.make_preferred();
	userDataPath_.make_preferred();
	applicationDataPath_.make_preferred();
	userApplicationDataPath_.make_preferred();
	resourcesPath_.make_preferred();
	tmpPath_.make_preferred();
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

const Filesystem::Path& Path::getTmpPath() const
{
	return tmpPath_;
}

const Filesystem::Path& Path::getPluginPath() const
{
	return pluginPath_;
}

const Filesystem::Path Path::generateTempFileName() const
{
	return TempFileName();
}

const Filesystem::Path& Path::getTranslationsPath() const
{
	return translationsPath_;
}

const Filesystem::Path Path::TempFileName()
{
	return Filesystem::Path(boost::lexical_cast<std::string>(std::chrono::system_clock::now().time_since_epoch().count()) + "_" + boost::lexical_cast<std::string>(std::this_thread::get_id()) + ".tmp");
}