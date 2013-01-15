/********************************************************************
	created:	2011/07/29
	created:	29:7:2011   10:41
	filename: 	AppInitializer.h
	author:		Wojciech Kniec

	purpose:
*********************************************************************/

#ifndef HEADER_GUARD_CORE__APPINITIALIZER_H__
#define HEADER_GUARD_CORE__APPINITIALIZER_H__
#include <QtGui/QApplication>
#include <core/src/PluginLoader.h>
#include "Log.h"
#include <corelib/Config.h>
#include "LogInitializer.h"
#include <core/ObjectWrapperCollection.h>
#include <QtCore/QSettings>
#include "ServiceManager.h"
#include "SourceManager.h"
#include "VisualizerManager.h"
#include "EDRConfig.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include <utils/Push.h>
#include <core/IManagersAccessor.h>
#include <osgQt/GraphicsWindowQt>
#include <osg/ArgumentParser>
#include <QtCore/QTranslator>
#include "CustomApplication.h"

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

PLUGIN_DEFINE_CORE_APPLICATION_ACCESSOR;

//WAŻNE!!
//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest taką biblioteką i jego resourcy musza być jawnie inicjalizowane
//Nazwa resourców musi być unikalna ponieważ Qt "miesza" nazwy metod z nazwamy plików resourców które chcemy inicjalizować tworząc unikalne statyczne funkcje na potrzeby inicjalizacji
//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dół stronki
inline void initCoreResources() { Q_INIT_RESOURCE(CoreIcons); }

namespace core {

class MyApplication : public QApplication
{
public:
    MyApplication(int & argc, char *argv[]) : QApplication(argc, argv) {}

    virtual bool notify(QObject* receiver, QEvent* event)
    {
        try {
            return QApplication::notify(receiver, event);
        } catch (std::exception &e) {
            LOG_ERROR("Error occured: " << e.what());
        } catch (...) {
            LOG_ERROR("Unknown error occured");
        }
        return false;
    }
};

class AppInitializer
{
public:

    //! Metoda uruchamiająca aplikacje, pobiera jako parametr wzorca widok który będzie uruchomiony, widok powinien dziedziczyć po MainWindow
	template<class FrontpageWidget>
	static int start(int & argc, char *argv[])
	{
		initCoreResources();

		//using namespace core;
	    // rev - statyczna
        //UTILS_STATIC_ASSERT((boost::is_base_of<MainWindow, FrontpageWidget>::value), "Klasa widoku musi dziedziczyć po klasie MainWindow");

        DataManager dataManager;
        VisualizerManager visualizerManager;
        DataProcessorManager dataProcessorManager;
        DataSourceManager dataSourceManager;
        ServiceManager serviceManager;
		SourceManager sourceManager;        

		osg::ArgumentParser arguments(&argc,argv);
		arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
		arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
		arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
		arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
		arguments.getApplicationUsage()->addCommandLineOption("--plugins <path>","Additional plugins directory");
		 
		// czy wyświetlamy pomoc?
		if (arguments.read("-h") || arguments.read("--help"))
		{
			arguments.getApplicationUsage()->write(std::cout);
			return 1;
		}


		int result = 0;
		{
			// ustawienia aplikacji
            MyApplication application(argc, argv);
            QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/plugins");
            
			//tutaj mamy ściezki - potrzebuję je od razu
			EDRConfig edrConfig;
			EDRConfig::setPaths(edrConfig);

            //ustawienia aplikacji
            application.setApplicationName("EDR");
			application.setOrganizationName("PJWSTK");

			//tłumaczenia
			QString locale = QLocale::system().name();

			auto langPath = QString((edrConfig.getResourcesPath() / "lang").string().c_str());

			QTranslator appTranslator;

			if(appTranslator.load("lang_" + locale, langPath) == false) {
				appTranslator.load(QString("lang_pl_PL"), langPath);
				//TODO
				//jak tego się nie uda załadować to mamy tylko angielski język - trzeba poinformować
			}

			qApp->installTranslator(&appTranslator);

			QTranslator qtTranslator;

			if(qtTranslator.load("qt_" + QLocale::languageToString(QLocale::system().language()), langPath) == false) {
				qtTranslator.load(QString("qt_pl"), langPath);
				//TODO
				//jak tego się nie uda załadować to mamy tylko angielski język - trzeba poinformować
			}

			qApp->installTranslator(&qtTranslator);

			QSettings::setDefaultFormat(QSettings::IniFormat);

            //Tworzymy tempa jeśli brakuje
            edrConfig.ensureTempDirectory();

			core::Filesystem::Path p = edrConfig.getApplicationDataPath() / "resources" / "settings" / "log.ini";

			// UWAGA - obiekty udostępniane klientom poprzez interfejsy muszą mieć przywracane (zerowane) wartości na tym samym
			// poziomie na którym zostały stworzone. Dlatego tutaj mamy Push dla logera i konfiguracji.
			// Dodatkowo trzeba pamiętać o kolejności niszczenia obiektów po zakończeniu czasu ich życia - jest to odwrotna kolejność
			// w stosunku do tej w jakiej zostały zdeklarowane w kodzie.

			// MUSI TAK BY ABY LOGGER WIDZIAŁ ŚCIEŻKI
			//utils::Push<IPath*> pushedDI(__instanceInfo.pathInterface, &edrConfig);

            try{

			    EDRLog logger(p.string());

			    //utils::Push<ILog*> pushedIL(__instanceInfo.logInterface, &logger);

			    PluginLoader pluginLoader;
		            std::string additionalPluginDirPath;
                            if (arguments.read("--plugins",additionalPluginDirPath))
			    {
				pluginLoader.addPath(additionalPluginDirPath);
				LOG_INFO("Plugin path added: " << additionalPluginDirPath);
			    }
			    {
                    ExtendedCustomApplication coreApplication(&edrConfig, &dataManager, nullptr, &logger, &serviceManager,
						&sourceManager, &dataSourceManager, &dataProcessorManager, nullptr);

				    // tworzenie managerów
				    // tworzenie/niszczenie managerów w ToolboxMain jest niebezpieczne
				    // z punktu widzenia destruktora - widgety mogą w jakimś stopniu
				    // zależeć od managerów, a wówczas wskaźniki wskazywałyby śmieci
				    // podobnie żywotność doładowanych bibliotek musi przekraczać zakończenie
				    // się destruktora

				    {
						{
							//HACK - wymuszamy statyczne linkowanie z biblioteką osgQt.
							// to gwarantuje poprawne zainicjowanie obiektu HeartBeat odpowiedzialnego
							// za obsługę scen OpenGL po stronie widgetów OSG.
							// Choć niekoniecznie w tym wątku z tego kozystamy ( nie musimy mieć tworzonych na starcie żadnych widgetów OSG)
							// jest to niezbędne dla prawidłowej deinicjalizacji tego obiektu - czasu życia jego zasobów.
							// W przeciwnym wypadku powstanie kilka instancji tego obiektu - po jednej dla każdego pluginu dostarczającego widgetów OSG
							// Bardzo niebezpieczne!! Powodowało crash aplikacji przy inicjalizacji a potem przy zamykaniu
							boost::shared_ptr<QWidget> w(new osgQt::GLWidget());
						}

                        // tworzymy widok
					    FrontpageWidget widget;
						widget.setWindowIcon(QPixmap(QString::fromUtf8(":/resources/icons/appIcon.png")));
                        //inicjalizujemy widok
					    widget.init(&pluginLoader, &coreApplication);
                        //inicjalizujemy konsolę logowania
					    logger.setConsoleWidget( widget.getConsole() );
                        //pokazujemy widok
					    widget.show();
					    if (!filePath.empty())
					    {
						    widget.openFile(filePath);
					    }

                        //uruchamiamy aplikację - kontekst UI z QT
					    result = application.exec();
                        //resetujemy konsolę po zakończeniu aplikacji
					    logger.setConsoleWidget(NULL);
				    }
			    }
            }catch(...){
                edrConfig.clearTempDirectory();
                throw;
            }

            try{
                edrConfig.clearTempDirectory();
            }catch(std::exception & e){
                std::cout << e.what() << std::endl;
            }
		}
		return result;
	}
};
}
#endif
