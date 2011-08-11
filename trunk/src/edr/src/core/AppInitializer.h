/********************************************************************
	created:	2011/07/29
	created:	29:7:2011   10:41
	filename: 	AppInitializer.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_CORE__APPINITIALIZER_H__
#define HEADER_GUARD_CORE__APPINITIALIZER_H__
#include "CorePCH.h"
#include <QtGui/QApplication>
#include <core/PluginLoader.h>
#include "Log.h"
#include <core/Config.h>
#include "LogInitializer.h"
#include <core/ObjectWrapperCollection.h>

#include "ServiceManager.h"
#include "DataManager.h"
#include "VisualizerManager.h"
#include "EDRConfig.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include <utils/Push.h>

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif
//#include "MainWindow.h"

CORE_DEFINE_INSTANCE_INFO;

namespace core {


class AppInitializer
{
public:
	template<class FrontpageWidget>
	static int start(int argc, char *argv[])
	{
		osg::ArgumentParser arguments(&argc,argv);
		arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
		arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
		arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
		arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

		// czy wy�wietlamy pomoc?
		if (arguments.read("-h") || arguments.read("--help")) 
		{
			arguments.getApplicationUsage()->write(std::cout);
			return 1;
		}

		// nazwa pliku do za�adowania
		std::string filePath;
		for (int i=1; i<arguments.argc() && filePath.empty(); ++i) 
		{
			if (arguments.isString(i)) 
			{
				filePath = arguments[i];
			}
		}

		int result = 0;
		{
			// ustawienia aplikacji
			QApplication application(argc, argv);
			application.setApplicationName("EDR");
			application.setOrganizationName("PJWSTK");
			QSettings::setDefaultFormat(QSettings::IniFormat);

			EDRConfig edrConfig;
			EDRConfig::setPaths(edrConfig);
			core::Filesystem::Path p = edrConfig.getResourcesPath() / "settings" / "log.ini";

			// UWAGA - obiekty udostepniane klientom poprzez interfejsy musz� mie� przywracane (zerowane) warto�ci na tym samym
			// poziomie na kt�rym zosta�y stworzone. Dlatego tutaj mamy Push dla logera i konfiguracji.
			// Dodatkowo trzeba pami�ta� o kolejno�ci niszczenia obiekt�w po zako�czeniu obszaru ich �ycia - jest to odwrotna kolejno��
			// w stosunku do tej w jakiej zosta�y zdeklarowane w kodzie.

			// MUSI TAK BY ABY LOGGER �CIE�KI WIDZIA�
			utils::Push<IPath*> pushedDI(__instanceInfo.pathInterface, &edrConfig);

			EDRLog logger(p.string());

			utils::Push<ILog*> pushedIL(__instanceInfo.logInterface, &logger);

			PluginLoader pluginLoader;
			{
				DataManager dataManager;
				VisualizerManager visualizerManager;
				DataProcessorManager dataProcessorManager;
				DataSourceManager dataSourceManager;
				ServiceManager serviceManager;


				// tworzenie manager�w
				// tworzenie/niszczenie manager�w w ToolboxMain jest niebezpieczne
				// z punktu widzenia destruktora - widgety mog� w jakim� stopniu
				// zale�e� od manager�w, a w�wczas wska�niki wskazywa�yby �mieci
				// podobnie �ywotno�� do�adowanych bibliotek musi przekracza� zako�czenie
				// si� destruktora
				utils::Push<IDataManager*> pushedDM(__instanceInfo.dataManager, &dataManager);
				utils::Push<IVisualizerManager*> pushedVM(__instanceInfo.visualizerManager, &visualizerManager);
				utils::Push<IDataProcessorManager*> pushedDPM(__instanceInfo.dataProcessorManager, &dataProcessorManager);
				utils::Push<IDataSourceManager*> pushedDSM(__instanceInfo.dataSourceManager, &dataSourceManager);
				utils::Push<IServiceManager*> pushedSM(__instanceInfo.serviceManager, &serviceManager);

				{
					FrontpageWidget widget;
					widget.init(&pluginLoader);

					logger.setConsoleWidget( widget.getConsole() );
					widget.show();
					if (!filePath.empty()) 
					{
						widget.openFile(filePath);
					}

					result = application.exec();
					logger.setConsoleWidget(NULL);
				}
			}
		}
		return result;
	}
};
}
#endif
