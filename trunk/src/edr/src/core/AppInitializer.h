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

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

CORE_DEFINE_INSTANCE_INFO;

namespace core {

class AppInitializer
{
public:

    //! Metoda uruchamiaj�ca aplikacje, pobiera jako parametr wzorca widok kt�ry b�dzie uruchomiony, widok powinien dziedziczy� po MainWindow
	template<class FrontpageWidget>
	static int start(int argc, char *argv[])
	{
        UTILS_STATIC_ASSERT((boost::is_base_of<MainWindow, FrontpageWidget>::value), "Klasa widoku musi dziedziczy� po klasie MainWindow");

        //WA�NE!!
        //tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak� bibliotek� i jego resourcy musza by� jawnie inicjalizowane
        //Nazwa resourc�w musi byc unikalna poniewa� Qt "miesza" nazwy metod z nazwamy plik�w resourc�w kt�re chcemy inicjalizowa� tworz�c unikalne statyczne funkcje na potrzeby inicjalizacji
        //link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam d� stronki
        Q_INIT_RESOURCE(CoreIcons);

        //! Wewn�trzna klasa tworz�ca wszystkie managery aplikacji i udostepniaj�ca je widokom oraz serwisom
        class AppManagers : public IManagersAccessor
        {
        public:

            //! Domy�lny konstruktor inicjuje wrazenie singletona dla kazdego managera
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
            //SEKCJA WSZYSTKICH MANAGEROW W APLIKACJI - TUTAJ POWINNY SI� ZNAJDOWA� ICH JEDYNE INSTANJCE!!

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
            //
            application.setApplicationName("EDR");
			application.setOrganizationName("PJWSTK");
            QString locale = QLocale::system().name();
            
            QTranslator appTranslator;
            appTranslator.load(QString("lang_") + locale);
            if (appTranslator.isEmpty()) {
                appTranslator.load("lang_pl_PL");
            }
            qApp->installTranslator(&appTranslator);

            QTranslator qtTranslator;
            qtTranslator.load(QString("qt_") + locale);
            if (qtTranslator.isEmpty()) {
                qtTranslator.load("qt_pl_PL");
            }
            qApp->installTranslator(&qtTranslator);

			QSettings::setDefaultFormat(QSettings::IniFormat);

			EDRConfig edrConfig;
			EDRConfig::setPaths(edrConfig);

            //Tworzymy tempa je�li brakuje
            edrConfig.ensureTempDirectory();

			core::Filesystem::Path p = edrConfig.getApplicationDataPath() / "resources" / "settings" / "log.ini";

			// UWAGA - obiekty udostepniane klientom poprzez interfejsy musz� mie� przywracane (zerowane) warto�ci na tym samym
			// poziomie na kt�rym zosta�y stworzone. Dlatego tutaj mamy Push dla logera i konfiguracji.
			// Dodatkowo trzeba pami�ta� o kolejno�ci niszczenia obiekt�w po zako�czeniu czasu ich �ycia - jest to odwrotna kolejno��
			// w stosunku do tej w jakiej zosta�y zdeklarowane w kodzie.

			// MUSI TAK BY ABY LOGGER WIDZIA� �CIE�KI
			utils::Push<IPath*> pushedDI(__instanceInfo.pathInterface, &edrConfig);

            try{

			    EDRLog logger(p.string());

			    utils::Push<ILog*> pushedIL(__instanceInfo.logInterface, &logger);

			    PluginLoader pluginLoader;
			    {
                    //Inicjalizacja manager�w
                    AppManagers appManagers;

				    // tworzenie manager�w
				    // tworzenie/niszczenie manager�w w ToolboxMain jest niebezpieczne
				    // z punktu widzenia destruktora - widgety mog� w jakim� stopniu
				    // zale�e� od manager�w, a w�wczas wska�niki wskazywa�yby �mieci
				    // podobnie �ywotno�� do�adowanych bibliotek musi przekracza� zako�czenie
				    // si� destruktora

				    {
                        // tworzymy widok
					    FrontpageWidget widget;
                        //inicjalizujemy widok
					    widget.init(&pluginLoader, &appManagers);
                        //inicjalizujemy konsol� logowania
					    logger.setConsoleWidget( widget.getConsole() );
                        //pokazujemy widok
					    widget.show();
					    if (!filePath.empty()) 
					    {
						    widget.openFile(filePath);
					    }

                        //uruchamiamy aplikacj� - kontekst UI z QT
					    result = application.exec();
                        //resetujemy konsol� po zako�czeniu aplikacji
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
