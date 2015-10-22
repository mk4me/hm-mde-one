#include "CommunicationPython.h"
#include <boost/python.hpp>
#include "corelib/ISourceManager.h"
#include "corelib/PluginCommon.h"
#include "corelib/IServiceManager.h"
#include "plugins/python/PythonPluginUtils.h"
#include "corelib/IDataManager.h"
#include <QtWidgets/QWidget>
#include "plugins/hmdbCommunication/IHMDBSource.h"
#include "plugins/hmdbCommunication/SourceOptionsWidget.h"

using namespace boost::python;


struct CommunicationPython : private python::PythonPluginUtils
    {
		void login(std::string user, std::string password)
		{
			utils::shared_ptr<hmdbCommunication::IHMDBSource> icomm = core::querySource<hmdbCommunication::IHMDBSource>(getSourceManager());
			plugin::ISourcePtr commSource = utils::dynamic_pointer_cast<plugin::ISource>(icomm);

			if (commSource != nullptr) {
				auto commWidget = commSource->getWidget();
				auto list = commWidget->findChildren<SourceOptionsWidget*>();
				UTILS_ASSERT(!list.isEmpty());
				SourceOptionsWidget* sourceOptionsWidget = *list.begin();

				auto config = sourceOptionsWidget->getConnectionProfile();
				config.motionServicesConfiguration.userConfiguration.user = QString::fromStdString(user);
				config.motionServicesConfiguration.userConfiguration.password = QString::fromStdString(password);
				sourceOptionsWidget->setConnectionProfile(config);
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
    };

MDE_PYTHON_MODULE(plugin_communicationPython)
{
	class_<CommunicationPython>("CommunicationPython")
		.def("login", &CommunicationPython::login);
}
