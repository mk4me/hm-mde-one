#include "CommunicationPython.h"
#include <boost/python.hpp>
#include "corelib/ISourceManager.h"
#include "corelib/PluginCommon.h"
#include "corelib/IServiceManager.h"
#include "plugins/python/python/PythonPluginUtils.h"
#include "corelib/IDataManager.h"
#include <QtWidgets/QWidget>
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"
#include "plugins/hmdbCommunication/ContextConfigurationSettingsFile.h"

using namespace boost::python;





void python::CommunicationPython::login(const hmdbCommunication::IHMDBSourceViewManager::ContextConfiguration& conf)
{
	utils::shared_ptr<hmdbCommunication::IHMDBSource> icomm = core::querySource<hmdbCommunication::IHMDBSource>(getSourceManager());
	plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);

	if (commSource != nullptr) {
		auto commWidget = commSource->getWidget();
		auto list = commWidget->findChildren<SourceOptionsWidget*>();
		UTILS_ASSERT(!list.isEmpty());
		SourceOptionsWidget* sourceOptionsWidget = *list.begin();
		sourceOptionsWidget->setConnectionProfile(conf);
		sourceOptionsWidget->onLogin();
		QStringList errors;
		sourceOptionsWidget->verify(errors);
		if (!errors.isEmpty()) {
			throw std::runtime_error("There was problem during logging");
		}
	} else {
		throw std::runtime_error("Unable to find source");
	}
}

python::CommunicationPython::ContextConf python::CommunicationPython::loadConf(const QString& path)
{
	return hmdbCommunication::ContextConfigurationSettingsFile::read(path);
}
