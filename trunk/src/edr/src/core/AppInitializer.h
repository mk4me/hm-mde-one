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
#include "SourceManager.h"
#include "DataManager.h"
#include "VisualizerManager.h"
#include "EDRConfig.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include <utils/Push.h>
#include <core/IManagersAccessor.h>
#include <osgQt/GraphicsWindowQt>

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

CORE_DEFINE_INSTANCE_INFO;

namespace core {

class MyApplication : public QApplication
{
public:
    MyApplication(int argc, char *argv[]) : QApplication(argc, argv) {}

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

    //! Metoda uruchamiaj¹ca aplikacje, pobiera jako parametr wzorca widok który bêdzie uruchomiony, widok powinien dziedziczyæ po MainWindow
	template<class FrontpageWidget>
	static int start(int argc, char *argv[])
	{
        UTILS_STATIC_ASSERT((boost::is_base_of<MainWindow, FrontpageWidget>::value), "Klasa widoku musi dziedziczyæ po klasie MainWindow");

        //WA¯NE!!
        //tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak¹ bibliotek¹ i jego resourcy musza byæ jawnie inicjalizowane
        //Nazwa resourców musi byc unikalna poniewa¿ Qt "miesza" nazwy metod z nazwamy plików resourców które chcemy inicjalizowaæ tworz¹c unikalne statyczne funkcje na potrzeby inicjalizacji
        //link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam dó³ stronki
        Q_INIT_RESOURCE(CoreIcons);

        //! Wewnêtrzna klasa tworz¹ca wszystkie managery aplikacji i udostepniaj¹ca je widokom oraz serwisom
        class AppManagers : public IManagersAccessor
        {
        public:

            //! Domyœlny konstruktor inicjuje wrazenie singletona dla kazdego managera
            //! Dzieki temu w kodzie po stronie EDR mamy wrazenie ze pracujemy z singletonami i mamy globalnie dostep do managerow!!
            AppManagers()
            {
                __instanceInfo.dataManagerReader = dataManager.manager = &dataManager;
                __instanceInfo.serviceManager = serviceManager.manager = &serviceManager;
				__instanceInfo.sourceManager = sourceManager.manager = &sourceManager;
                __instanceInfo.visualizerManager = visualizerManager.manager = &visualizerManager;
                __instanceInfo.dataProcessorManager = dataProcessorManager.manager = &dataProcessorManager;
                __instanceInfo.dataSourceManager = dataSourceManager.manager = &dataSourceManager;
            }

            //! Destruktor deinicjalizuje wrazenie singletona managerow
            ~AppManagers()
            {
                __instanceInfo.dataManagerReader = dataManager.manager = nullptr;
                __instanceInfo.serviceManager = serviceManager.manager = nullptr;
				__instanceInfo.sourceManager = sourceManager.manager = nullptr;
                __instanceInfo.visualizerManager = visualizerManager.manager = nullptr;
                __instanceInfo.dataProcessorManager = dataProcessorManager.manager = nullptr;
                __instanceInfo.dataSourceManager = dataSourceManager.manager = nullptr;
            }

            virtual IDataManagerReader * getDataManagerReader()
            {
                return &dataManager;
            }

            virtual const IDataManagerReader * getDataManagerReader() const
            {
                return &dataManager;
            }

            virtual IFileDataManager * getFileDataManager()
            {
                return &dataManager;
            }

            virtual const IFileDataManager * getFileDataManager() const
            {
                return &dataManager;
            }

            virtual IMemoryDataManager * getMemoryDataManager()
            {
                return &dataManager;
            }

            virtual const IMemoryDataManager * getMemoryDataManager() const
            {
                return &dataManager;
            }

            virtual DataManager * getDataManager()
            {
                return &dataManager;
            }

            virtual const DataManager * getDataManager() const
            {
                return &dataManager;
            }

            virtual ServiceManager * getServiceManager()
            {
                return &serviceManager;
            }

            virtual const ServiceManager * getServiceManager() const
            {
                return &serviceManager;
            }

			virtual ISourceManager * getSourceManager()
			{
				return &sourceManager;
			}

			virtual const ISourceManager * getSourceManager() const
			{
				return &sourceManager;
			}

            virtual VisualizerManager * getVisualizerManager()
            {
                return &visualizerManager;
            }

            virtual const VisualizerManager * getVisualizerManager() const
            {
                return &visualizerManager;
            }

            virtual DataProcessorManager * getDataProcessorManager()
            {
                return &dataProcessorManager;
            }

            virtual const DataProcessorManager * getDataProcessorManager() const
            {
                return &dataProcessorManager;
            }

            virtual DataSourceManager * getDataSourceManager()
            {
                return &dataSourceManager;
            }

            virtual const DataSourceManager * getDataSourceManager() const
            {
                return &dataSourceManager;
            }

        private:
            //SEKCJA WSZYSTKICH MANAGEROW W APLIKACJI - TUTAJ POWINNY SIÊ ZNAJDOWAÆ ICH JEDYNE INSTANJCE!!

            DataManager dataManager;
            VisualizerManager visualizerManager;
            DataProcessorManager dataProcessorManager;
            DataSourceManager dataSourceManager;
            ServiceManager serviceManager;
			SourceManager sourceManager;
        };

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
			MyApplication application(argc, argv);

			//tutaj mamy œciezki - potrzebujê je od razu
			EDRConfig edrConfig;
			EDRConfig::setPaths(edrConfig);

            //ustawienia aplikacji
            application.setApplicationName("EDR");
			application.setOrganizationName("PJWSTK");
			
			//t³umaczenia
			QString locale = QLocale::system().name();

			auto langPath = QString((edrConfig.getResourcesPath() / "lang").string().c_str());

			QTranslator appTranslator;
			
			if(appTranslator.load("lang_" + locale, langPath) == false) {
				appTranslator.load(QString("lang_pl_PL"), langPath);
				//TODO
				//jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
			}

			qApp->installTranslator(&appTranslator);

			QTranslator qtTranslator;

			if(qtTranslator.load("qt_" + QLocale::languageToString(QLocale::system().language()), langPath) == false) {
				qtTranslator.load(QString("qt_pl"), langPath);
				//TODO
				//jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
			}

			qApp->installTranslator(&qtTranslator);

			QSettings::setDefaultFormat(QSettings::IniFormat);

            //Tworzymy tempa jeœli brakuje
            edrConfig.ensureTempDirectory();

			core::Filesystem::Path p = edrConfig.getApplicationDataPath() / "resources" / "settings" / "log.ini";

			// UWAGA - obiekty udostepniane klientom poprzez interfejsy musz¹ mieæ przywracane (zerowane) wartoœci na tym samym
			// poziomie na którym zosta³y stworzone. Dlatego tutaj mamy Push dla logera i konfiguracji.
			// Dodatkowo trzeba pamiêtaæ o kolejnoœci niszczenia obiektów po zakoñczeniu czasu ich ¿ycia - jest to odwrotna kolejnoœæ
			// w stosunku do tej w jakiej zosta³y zdeklarowane w kodzie.

			// MUSI TAK BY ABY LOGGER WIDZIA£ ŒCIE¯KI
			utils::Push<IPath*> pushedDI(__instanceInfo.pathInterface, &edrConfig);

            try{

			    EDRLog logger(p.string());

			    utils::Push<ILog*> pushedIL(__instanceInfo.logInterface, &logger);

			    PluginLoader pluginLoader;
			    {
                    //Inicjalizacja managerów
                    AppManagers appManagers;

				    // tworzenie managerów
				    // tworzenie/niszczenie managerów w ToolboxMain jest niebezpieczne
				    // z punktu widzenia destruktora - widgety mog¹ w jakimœ stopniu
				    // zale¿eæ od managerów, a wówczas wskaŸniki wskazywa³yby œmieci
				    // podobnie ¿ywotnoœæ do³adowanych bibliotek musi przekraczaæ zakoñczenie
				    // siê destruktora

				    {
						{
							//HACK - wymuszamy statyczne linkowanie z bibliotek¹ osgQt.
							// to gwarantuje poprawne zainicjowanie obiektu HeartBeat odpowiedzialnego
							// za obs³uge scen OpenGL po stronie widgetów OSG.
							// Choæ niekoniecznie w tym w¹tku z tego kozystamy ( nie musimy mieæ tworzonych na starcie ¿adnych widgetów OSG)
							// jest to niezbêdne dla prawid³owej deinicjalizacji tego obiektu - czasu ¿ycia jego zasobów.
							// W przeciwnym wypadku powstanie kilka instancji tego obiektu - po jednej dla ka¿dego pluginu dostarczaj¹cego widgetów OSG
							// Bardzo niebezpieczne!! Powodowa³o crash aplikacji przy inicjalizacji a potem przy zamykaniu
							boost::shared_ptr<QWidget> w(new osgQt::GLWidget());
						}

                        // tworzymy widok
					    FrontpageWidget widget;
						widget.setWindowIcon(QPixmap(QString::fromUtf8(":/resources/icons/appIcon.png")));
                        //inicjalizujemy widok
					    widget.init(&pluginLoader, &appManagers);
                        //inicjalizujemy konsolê logowania
					    logger.setConsoleWidget( widget.getConsole() );
                        //pokazujemy widok
					    widget.show();
					    if (!filePath.empty()) 
					    {
						    widget.openFile(filePath);
					    }

                        //uruchamiamy aplikacjê - kontekst UI z QT
					    result = application.exec();
                        //resetujemy konsolê po zakoñczeniu aplikacji
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
