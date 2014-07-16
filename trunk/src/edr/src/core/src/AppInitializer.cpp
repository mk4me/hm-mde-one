#include "CorePCH.h"
#include <core/AppInitializer.h>
#include <threading/SynchronizationPolicies.h>
#include "ApplicationCommon.h"
#include <corelib/PluginCommon.h>
#include <boost/bind.hpp>
#include <QtCore/QCoreApplication>
#include "PluginApplication.h"
#include <coreui/SingleInstanceWindow.h>
#include <QtWidgets/QMessageBox>

DEFINE_CORE_APPLICATION_ACCESSOR;
PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR;

using namespace core;
using namespace coreUI;

class AppInitializer::AppInitializerImpl
{
public:

	AppInitializerImpl(int & argc, char *argv[]) : coreApplication(new Application())
	{
		cleanUp_ = boost::bind(&Application::finalizeUI, coreApplication.get());
		__application = coreApplication.get();
		plugin::__coreApplication = &mainViewApplication;
		initUIContextRes = coreApplication->initUIContext(argc, argv, translations);
	}

	~AppInitializerImpl()
	{

	}

	//! Metoda uruchamiaj�ca aplikacje, pobiera jako parametr wzorca widok kt�ry b�dzie uruchomiony, widok powinien dziedziczy� po CoreMainWindow
	int start(coreUI::CoreMainWindow * mainWindow)
	{
		int result = 0;
		if(initUIContextRes == 1){
			result = 1;
		}else{			

			coreUI::SingleInstanceWindow * siw = dynamic_cast<coreUI::SingleInstanceWindow*>(mainWindow);
			bool allowed = true;
			if(siw != nullptr){
				if(siw->isSingle() == false){

					mainWindow->setCloseConfirmationRequired(false);
					mainWindow->setAttribute(Qt::WA_QuitOnClose);
					mainWindow->close();

					QMessageBox::warning(nullptr, mainWindow->windowTitle(), QObject::tr("Detected other instance of application already running. Only one instance of this application is allowed to be run. This application will be closed now."));
					allowed = false;
				}
			}

			if(allowed == true){

				try{

					//inicjalizujemy widok
					coreApplication->initWithUI(mainWindow, translations);

					if(translations.empty() == false){
						auto it = translations.begin();
						std::string files((*it).string());
						++it;

						for( ; it != translations.end(); ++it){
							files += "\n" + (*it).string();
						}

						CORE_LOG_INFO("Unused translation files:\n" << files);

						std::vector<Filesystem::Path>().swap(translations);
					}

					mainWindow->init(plugin::__coreApplication);
					//ustawiamy tutaj �eby nadpisa� ewentualne zmiany z widok�w
					qApp->setOrganizationName("PJWSTK");

					try{
						//faktycznie blokowane wywo�anie
						//uruchamia kontekst Qt
						result = coreApplication->run();
					}catch(std::exception & e){
						result = -2;
						CORE_LOG_ERROR("Error while UI run: " << e.what());
					}catch(...){
						result = -2;
						CORE_LOG_ERROR("Unknown error while UI run");
					}

				}catch(std::exception & e){
					result = -1;
					CORE_LOG_ERROR("Error while UI initialization: " << e.what());
				}catch(...){
					result = -1;
					CORE_LOG_ERROR("Unknown error while UI initialization");
				}
			}
		}

		return result;
	}

	coreUI::CoreMainWindow::CloseUpOperations cleanUp() const
	{
		return cleanUp_;
	}

private:
	std::vector<Filesystem::Path> translations;
	utils::shared_ptr<Application> coreApplication;
	MainViewApplication mainViewApplication;
	int initUIContextRes;
	coreUI::CoreMainWindow::CloseUpOperations cleanUp_;

};

AppInitializer::AppInitializer(int & argc, char *argv[]) : impl(new AppInitializerImpl(argc, argv))
{
	
}

AppInitializer::~AppInitializer()
{

}

int AppInitializer::start(coreUI::CoreMainWindow * mainWindow)
{
	return impl->start(mainWindow);
}

coreUI::CoreMainWindow::CloseUpOperations AppInitializer::cleanUp() const
{
	return impl->cleanUp();
}
