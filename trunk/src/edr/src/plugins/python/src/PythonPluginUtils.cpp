#include "PythonPCH.h"
#include "corelib/PluginCommon.h"
#include "corelib/IServiceManager.h"
#include "corelib/ISourceManager.h"
#include "plugins/python/PythonPluginUtils.h"

core::IServiceManager* python::PythonPluginUtils::getServiceManager()
{
	return plugin::getServiceManager();
}

core::ISourceManager* python::PythonPluginUtils::getSourceManager()
{
	return plugin::getSourceManager();
}

