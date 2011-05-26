#include "CorePCH.h"
#include <QtGui/QApplication>
#include "ToolboxMain.h"
#include <core/PluginLoader.h>
#include <core/Log.h>
#include <core/Config.h>
//#include "VisualizerTitleBar.h"
#include "LogInitializer.h"
#include <core/ObjectWrapperCollection.h>

#include "ServiceManager.h"
#include "DataManager.h"
#include "VisualizerManager.h"
#include <utils/Push.h>

//FOR TESTS ONLY
#include "EDRDockWidget.h"
#include "TestEDRWidget.h"

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

using namespace core;

CORE_DEFINE_INSTANCE_INFO;



int main(int argc, char *argv[])
{
    

    osg::ArgumentParser arguments(&argc,argv);
    arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
    arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
    arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");

    // czy wyœwietlamy pomoc?
    if (arguments.read("-h") || arguments.read("--help")) 
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    // nazwa pliku do za³adowania
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

        LogInitializer logger("resources/settings/log.ini");

        PluginLoader pluginLoader;
        {
            DataManager dataManager;

            boost::filesystem::path userPath(toString(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation)));
            userPath /= "EDR";
            dataManager.setUserDataPath(userPath);

            boost::filesystem::path appDataPath(core::toStdString(QDesktopServices::storageLocation(QDesktopServices::DataLocation)));
            dataManager.setApplicationDataPath(appDataPath);

///            boost::filesystem::path resourcesPath = boost::filesystem::initial_path();
            boost::filesystem::path resourcesPath(core::toStdString(QDir::currentPath())); 
            resourcesPath /= "resources";
			dataManager.setResourcesPath(resourcesPath);
            ServiceManager serviceManager;
            VisualizerManager visualizerManager;

            // tworzenie managerów
            // tworzenie/niszczenie managerów w ToolboxMain jest niebezpieczne
            // z punktu widzenia destruktora - widgety mog¹ w jakimœ stopniu
            // zale¿eæ od managerów, a wówczas wskaŸniki wskazywa³yby œmieci
            // podobnie ¿ywotnoœæ do³adowanych bibliotek musi przekraczaæ zakoñczenie
            // siê destruktora
            utils::Push<IDataManager*> pushedDM(__instanceInfo.dataManager, &dataManager);
            utils::Push<IVisualizerManager*> pushedVM(__instanceInfo.visualizerManager, &visualizerManager);
            utils::Push<IServiceManager*> pushedSM(__instanceInfo.serviceManager, &serviceManager);

            
            {
                ToolboxMain window(&pluginLoader);

                //FOR TESTS ONLY
                EDRDockWidget * widget = new EDRDockWidget();
                //widget->setWidget(test);

                window.addDockWidget(Qt::DockWidgetArea::BottomDockWidgetArea, widget);

                TestEDRWidget * test = new TestEDRWidget(qobject_cast<EDRTitleBar*>(widget->titleBarWidget()));
                widget->getInnerWidget()->layoutContent->addWidget(test);

                logger.setConsoleWidget( window.getConsole() );
                window.show();
                if (!filePath.empty()) 
                {
                    window.openFile(filePath);
                }

                result = application.exec();
                logger.setConsoleWidget(NULL);
            }
        }
    }
    return result;
}

