#include "CorePCH.h"
#include "Application.h"
#include "Log.h"
#include "LogInitializer.h"
#include "PluginLoader.h"
#include <QtGui/QApplication>
#include <osgQt/GraphicsWindowQt>
#include <osg/ArgumentParser>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include <corelib/Config.h>
#include "MainWindow.h"
#include <QtGui/QSplashScreen>

#ifdef WIN32
#include <Windows.h>
//#define KEY_PATH1 TEXT("Software\\Wow6432Node\\PJWSTK\\EDR")
// Od Visty dodawane s� przedrostki typu Wow6432Node do sciezki w rejestrach
// adres podawany do oczytu klucza powinien by� automatycznie konwertowany.
#define KEY_PATH TEXT("Software\\PJWSTK\\EDR")
#endif

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

//WA�NE!!
//tak inicjalizujemy resourcy wkompilowane w biblioteki statyczne linkowane do aplikacji - w naszym przypadku to Core jest tak� bibliotek� i jego resourcy musza by� jawnie inicjalizowane
//Nazwa resourc�w musi by� unikalna poniewa� Qt "miesza" nazwy metod z nazwamy plik�w resourc�w kt�re chcemy inicjalizowa� tworz�c unikalne statyczne funkcje na potrzeby inicjalizacji
//link: http://developer.qt.nokia.com/doc/qt-4.8/resources.html - sam d� stronki
inline void initCoreResources() { Q_INIT_RESOURCE(CoreIcons); }

CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

namespace core {

class UIApplication : public QApplication
{
	Q_OBJECT;
public:
	UIApplication(int & argc, char *argv[], Application * coreApplication) : QApplication(argc, argv), coreApplication(coreApplication) {}

	virtual bool notify(QObject* receiver, QEvent* event)
	{
		try {
			return QApplication::notify(receiver, event);
		} catch (std::exception &e) {
			CORE_LOG_ERROR("Error occured: " << e.what());
		} catch (...) {
			CORE_LOG_ERROR("Unknown error occured");
		}
		return false;
	}

private:
	Application * coreApplication;
};

}

using namespace core;

Application::Application()
{

}

int Application::initUI(int & argc, char *argv[])
{
	//obs�uga argument�w i opisu uzycia aplikacji z konsoli
	osg::ArgumentParser arguments(&argc,argv);
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
	arguments.getApplicationUsage()->addCommandLineOption("--plugins <path>","Additional plugins directory");

	// czy wy�wietlamy pomoc?
	if (arguments.read("-h") || arguments.read("--help"))
	{
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}


	// inicjalizacja UI, wszystkich potrzebnych zasob�w
	{
		initCoreResources();

		uiApplication_.reset(new UIApplication(argc, argv));
		QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/plugins");

		//ustawienia aplikacji
		uiApplication_->setApplicationName("EDR");
		uiApplication_->setOrganizationName("PJWSTK");
		QSettings::setDefaultFormat(QSettings::IniFormat);
	}

	return 0;
}

void Application::initWithUI(MainWindow * mainWindow)
{
	mainWindow->splashScreen();
	mainWindow->showSplashScreenMessage(tr("Initializing application paths"));
	QCoreApplication::processEvents();
	//inicjalizacja �cie�ek aplikacji, katalog�w tymczasowych itp, u�ywane do �adowania t�umacze�
	{
		if(trySetPathsFromRegistry(paths_) == false){
			setDefaultPaths(paths_);
		}

		//sprawdzamy czy uda�o si� wygenerowac poprawne sciezki alikacji
		if(paths_ == nullptr){
			throw std::runtime_error("Could not initialize application paths");
		}
	}

	mainWindow->showSplashScreenMessage(tr("Initializing logger"));
	QCoreApplication::processEvents();

	//Mam �cie�k� do konfiguracji loggera, nie wiem czy to OSG, Log4cxx czy pusty albo co� jeszcze innego - initializer powinien to zrobi� za mnie
	//powinien te� obs�ugiwa� widget!!
	{	
		logInitializer_.reset(new LogInitializer((paths_->getResourcesPath() / "settings" / "log.ini").string()));

		loggerPrototype_.reset(new Log());
		logger_ = loggerPrototype_->subLog("core");
	}

	mainWindow->showSplashScreenMessage(tr("Initializing directories"));
	QCoreApplication::processEvents();

	//probujemy tmp katalog zapewni�
	try{
		Filesystem::createDirectory(paths_->getTmpPath());
	}catch(...){
		CORE_LOG_ERROR("Could not create temporary directory: " << paths_->getTmpPath());
		throw;
	}

	mainWindow->showSplashScreenMessage(tr("Initializing translations"));
	QCoreApplication::processEvents();

	//t�umaczenia aplikacji
	{
		QString locale = QLocale::system().name();

		auto langPath = QString::fromStdString((paths_->getResourcesPath() / "lang").string());

		QTranslator appTranslator;

		if(appTranslator.load("lang_" + locale, langPath) == false) {
			appTranslator.load(QString("lang_pl_PL"), langPath);
			//TODO
			//jak tego si� nie uda za�adowa� to mamy tylko angielski j�zyk - trzeba poinformowa�
		}

		uiApplication_->installTranslator(&appTranslator);

		QTranslator qtTranslator;

		if(qtTranslator.load("qt_" + QLocale::languageToString(QLocale::system().language()), langPath) == false) {
			qtTranslator.load(QString("qt_pl"), langPath);
			//TODO
			//jak tego si� nie uda za�adowa� to mamy tylko angielski j�zyk - trzeba poinformowa�
		}

		uiApplication_->installTranslator(&qtTranslator);
	}

	mainWindow->showSplashScreenMessage(tr("Initializing plugins loader"));
	QCoreApplication::processEvents();

	//inicjalizacja obiektu �aduj�cego pluginy
	pluginLoader_.reset(new PluginLoader(Filesystem::Path(QCoreApplication::applicationFilePath().toStdString()).parent_path()));

	//obsluga dodatkowej sciezki z pluginami zewnetrznymi
	{
		std::string additionalPluginDirPath;
		if (arguments.read("--plugins",additionalPluginDirPath))
		{
			Filesystem::Path path(additionalPluginDirPath);
			if(Filesystem::pathExists(path) == true) {
				Filesystem::Iterator endIT;
				std::for_each(Filesystem::Iterator(path), endIT, [=](const Filesystem::Path & p) {
					if (Filesystem::isDirectory(p)) {
						pluginLoader_->addPath(p);
						CORE_LOG_INFO("Plugin path added: " << p);
					}
				});
			}
			
		}
	}

	mainWindow->showSplashScreenMessage(tr("Loading plugins"));
	QCoreApplication::processEvents();

	//�adujemy pluginy
	pluginLoader_->load();

	mainWindow->showSplashScreenMessage(tr("Initializing 3D context"));
	QCoreApplication::processEvents();

	{
		//HACK - wymuszamy statyczne linkowanie z bibliotek� osgQt.
		// to gwarantuje poprawne zainicjowanie obiektu HeartBeat odpowiedzialnego
		// za obs�ug� scen OpenGL po stronie widget�w OSG.
		// Cho� niekoniecznie w tym w�tku z tego kozystamy ( nie musimy mie� tworzonych na starcie �adnych widget�w OSG)
		// jest to niezb�dne dla prawid�owej deinicjalizacji tego obiektu - czasu �ycia jego zasob�w.
		// W przeciwnym wypadku powstanie kilka instancji tego obiektu - po jednej dla ka�dego pluginu dostarczaj�cego widget�w OSG
		// Bardzo niebezpieczne!! Powodowa�o crash aplikacji przy inicjalizacji a potem przy zamykaniu
		boost::shared_ptr<QWidget> w(new osgQt::GLWidget());
	}

	mainWindow->showSplashScreenMessage(tr("Initializing core managers"));
	QCoreApplication::processEvents();

	dataHierarchyManager_.reset(new DataHierarchyManager());
	memoryDataManager_.reset(new MemoryDataManager());
	parserManager_.reset(new ParserManager());
	streamDataManager_.reset(new StreamDataManager());
	fileDataManager_.reset(new FileDataManager());
	serviceManager_.reset(new ServiceManager());
	sourceManager_.reset(new SourceManager());
	visualizerManager_.reset(new VisualizerManager());	

	mainWindow->showSplashScreenMessage(tr("Registering application core domain types"));
	QCoreApplication::processEvents();

	registerCoreDomainTypes();

	for(int i = 0; i < pluginLoader_->getNumPlugins(); ++i){
		unpackPlugin(splashScreen, pluginLoader_->getPlugin(i));
	}

	//findResources(core::getResourcesPath().string());

	mainWindow->showSplashScreenMessage(tr("Initializing services"));
	QCoreApplication::processEvents();

	// inicjalizacja us�ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->init(sourceManager_.get(),
			visualizerManager_.get(), memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	// inicjalizacja us�ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->lateInit();
	}

	mainWindow->showSplashScreenMessage(tr("Initializing sources"));
	QCoreApplication::processEvents();

	// inicjalizacja �r�de�	
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		sourceManager_->getSource(i)->init(memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	mainWindow->showSplashScreenMessage(tr("Initializing console"));
	QCoreApplication::processEvents();
	logInitializer_->setConsoleWidget(mainWindow->getConsole());

	mainWindow->showSplashScreenMessage(tr("Initializing main view"));
	QCoreApplication::processEvents();
	mainWindow->init();

	mainWindow->show();

	//widget.setWindowIcon(QPixmap(QString::fromUtf8(":/resources/icons/appIcon.png")));
	//TODO
	//co to ma robi�?!
	/*if (!filePath.empty())
	{
	widget.openFile(filePath);
	}*/
}

int Application::run()
{
	return uiApplication_->exec();
}

Application::~Application()
{
	//TODO
	CORE_LOG_INFO("Closing core application");
	CORE_LOG_INFO("Releasing sources");
	CORE_LOG_INFO("Stopping services");
	CORE_LOG_INFO("Cleaning tmp files");
	CORE_LOG_INFO("Closing core managers");
	CORE_LOG_INFO("Releasing plugins");
	CORE_LOG_INFO("Closing logger");
}

MemoryDataManager* Application::memoryDataManager()
{
	return memoryDataManager_.get();
}

StreamDataManager* Application::streamDataManager()
{
	return streamDataManager_.get();
}

FileDataManager* Application::fileDataManager()
{
	return fileDataManager_.get();
}

ILog* Application::logger()
{
	return logger_.get();
}

ILog* Application::loggerPrototype()
{
	return loggerPrototype_.get();
}

Path* Application::paths()
{
	return paths_.get();
}

DataHierarchyManager* Application::dataHierarchyManager()
{
	return dataHierarchyManager_.get();
}

ParserManager* Application::parserManager()
{
	return parserManager_.get();
}

VisualizerManager* Application::visualizerManager()
{
	return visualizerManager_.get();
}

ServiceManager* Application::serviceManager()
{
	return serviceManager_.get();
}

SourceManager* Application::sourceManager()
{
	return sourceManager_.get();
}

bool Application::trySetPathsFromRegistry(shared_ptr<Path> & path)
{
#ifdef WIN32
#define PATH_BUFFER_SIZE 1024
	HKEY hKey;
	LONG lResult;
	char buffer[PATH_BUFFER_SIZE];
	DWORD dwType, dwSize = PATH_BUFFER_SIZE;

	Filesystem::Path resourcesPath;

	LPTSTR lpValueName = "ProgramFilesPath";
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		resourcesPath = Filesystem::Path(buffer) / "bin" / "resources";
		RegCloseKey(hKey);
	} else {
		return false;
	}

	Filesystem::Path applicationDataPath;

	lpValueName = "ApplicationDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		applicationDataPath = Filesystem::Path(buffer);
		RegCloseKey(hKey);
	} else {
		return false;
	}

	Filesystem::Path userDataPath;

	lpValueName = "UserDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		userDataPath = Filesystem::Path(buffer);
		RegCloseKey(hKey);
	}else{
		return false;
	}

	path.reset(new Path(userDataPath, applicationDataPath, resourcesPath, userDataPath / "tmp", applicationDataPath / "plugins"));

	return true;
#else
	return false;
#endif
}

void Application::setDefaultPaths(shared_ptr<Path> & path)
{
	auto userPath = Filesystem::Path(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).toStdString()) / "PJWSTK" / "EDR";
	auto appDataPath = Filesystem::Path(QDesktopServices::storageLocation(QDesktopServices::DataLocation).toStdString());
	auto resourcesPath = Filesystem::Path(QDir::currentPath().toStdString()) / "resources";
	
	path.reset(new Path(userPath, appDataPath, resourcesPath, userPath / "tmp", appDataPath / "plugins"));
}

void Application::safeRegisterService(const plugin::IServicePtr & service)
{
	try{

		serviceManager_->registerService(service);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
			" has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
			" has caused an UNKNOWN error during registration. Service NOT registered in application!" );
	}
}

void Application::safeRegisterSource(const plugin::ISourcePtr & source)
{
	try{

		sourceManager_->registerSource(source);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Source " << source->getName() << " with ID " << source->getID() <<
			" has caused an error during registration: " << e.what() << ". Source NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Source " << source->getName() << " with ID " << source->getID() <<
			" has caused an UNKNOWN error during registration. Source NOT registered in application!" );
	}
}

void Application::safeRegisterParser(const plugin::IParserPtr & parser)
{
	try{

		parserManager_->registerParser(parser);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
			" has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
			" has caused an UNKNOWN error during registration. Parser NOT registered in application!" );
	}
}

void Application::safeRegisterObjectWrapperPrototype(const ObjectWrapperPtr & prototype)
{
	try{

		dataHierarchyManager_->registerObjectWrapperPrototype(prototype);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an error during registration: "
			<< e.what() << ". Object type NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an UNKNOWN error during registration. Object type NOT registered in application!" );
	}
}

void Application::safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer)
{
	try{

		visualizerManager_->registerVisualizer(visualizer);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
			<< " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
			<< " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!" );
	}
}

void Application::registerCoreDomainTypes()
{
	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<int>());
	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<double>());
}

void Application::showSplashScreenMessage(QSplashScreen * splashScreen, const QString & message)
{
	splashScreen->showMessage(message);
	QCoreApplication::processEvents();
}

void Application::unpackPlugin(QSplashScreen * splashScreen, const core::PluginPtr & plugin)
{
	auto message = QObject::tr("Loading plugin %1 content: %2").arg(QString::fromStdString(plugin->getName()));

	showSplashScreenMessage(splashScreen, message.arg(QObject::tr("services")));

	for ( int j = 0; j < plugin->getNumServices(); ++j ) {
		safeRegisterService(plugin->getService(j));
	}

	showSplashScreenMessage(splashScreen, message.arg(QObject::tr("sources")));

	for ( int j = 0; j < plugin->getNumSources(); ++j ) {
		safeRegisterSource(plugin->getSource(j));
	}

	showSplashScreenMessage(splashScreen, message.arg(QObject::tr("parsers")));

	for(int j = 0; j < plugin->getNumParsers(); ++j) {
		safeRegisterParser(plugin->getParser(j));
	}

	showSplashScreenMessage(splashScreen, message.arg(QObject::tr("domain objects")));

	for(int j = 0; j < plugin->getNumObjectWrapperPrototypes(); ++j) {
		safeRegisterObjectWrapperPrototype(plugin->getObjectWrapperPrototype(j));
	}

	showSplashScreenMessage(splashScreen, message.arg(QObject::tr("visualizers")));

	for(int j = 0; j < plugin->getNumVisualizers(); ++j) {
		safeRegisterVisualizer(plugin->getVisualizer(j));
	}
}

//��danie od��czenia si� serwis�w od widget�w i element�w UI oraz innych serwis�w czy zasob�w aplikacji
ServiceManager::getInstance()->finalizeServices();

//    VisualizerManager::getInstance()->setDebugWidget(nullptr);
VisualizerManager::getInstance()->markAllChannelsAsRemoved();
VisualizerManager::getInstance()->removeAllChannels();