#include "CorePCH.h"
#include <core/AppInitializer.h>
#include <utils/SynchronizationPolicies.h>

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
		initUIContextRes = coreApplication->initUIContext(argc, argv);
	}

	~AppInitializerImpl()
	{

	}

	//! Metoda uruchamiaj¹ca aplikacje, pobiera jako parametr wzorca widok który bêdzie uruchomiony, widok powinien dziedziczyæ po CoreMainWindow
	int start(coreUI::CoreMainWindow * mainWindow)
	{
		int result = 0;
		if(initUIContextRes == 1){
			result = 1;
		}else{

			try{
				//inicjalizujemy widok
				coreApplication->initWithUI(mainWindow);

				mainWindow->init(plugin::__coreApplication);
				//ustawiamy tutaj ¿eby nadpisaæ ewentualne zmiany z widoków
				qApp->setOrganizationName("PJWSTK");
				try{
					//faktycznie blokowane wywo³anie
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

		return result;
	}

	coreUI::CoreMainWindow::CloseUpOperations cleanUp() const
	{
		return cleanUp_;
	}

private:

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