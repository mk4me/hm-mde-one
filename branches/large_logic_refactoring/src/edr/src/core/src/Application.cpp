#include "CorePCH.h"
#include <QtGui/QApplication>
#include <osgQt/GraphicsWindowQt>
#include <osg/ArgumentParser>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include "Config.h"
#include <coreui/CoreMainWindow.h>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>

#ifdef WIN32
#include <Windows.h>
//#define KEY_PATH1 TEXT("Software\\Wow6432Node\\PJWSTK\\EDR")
// Od Visty dodawane s¹ przedrostki typu Wow6432Node do sciezki w rejestrach
// adres podawany do oczytu klucza powinien byæ automatycznie konwertowany.
#define KEY_PATH TEXT("Software\\PJWSTK\\EDR")
#endif

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

namespace coreUI {

class UIApplication : public QApplication
{
public:
	UIApplication(int & argc, char *argv[]) : QApplication(argc, argv) {}

	virtual bool notify(QObject* receiver, QEvent* event)
	{
		try {
			return QApplication::notify(receiver, event);
		} catch (std::exception &e) {
			CORE_LOG_ERROR("Error occured in UI during user operation execution: " << e.what());
		} catch (...) {
			CORE_LOG_ERROR("Unknown error occured in UI during user operation execution");
		}
		return false;
	}
};

}

using namespace coreUI;
using namespace core;

Application::Application()
{

}

int Application::initUIContext(int & argc, char *argv[])
{
	//obs³uga argumentów i opisu uzycia aplikacji z konsoli
	osg::ArgumentParser arguments(&argc,argv);
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of EDR.");
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
	arguments.getApplicationUsage()->addCommandLineOption("--plugins <path>","Additional plugins directory");

	// czy wyœwietlamy pomoc?
	if (arguments.read("-h") || arguments.read("--help"))
	{
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}

	// pdczytujemy czy jest podana dodatkowa œcie¿ka dla pluginów
	std::string path;
	arguments.read("--plugins",path);
	if(path.empty() == false){
		additionalPluginsPath = path;
	}

	// inicjalizacja UI, wszystkich potrzebnych zasobów
	{
		uiApplication_.reset(new coreUI::UIApplication(argc, argv));
		QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/plugins");

		//ustawienia aplikacji
		QSettings::setDefaultFormat(QSettings::IniFormat);
	}

	return 0;
}

void Application::initWithUI(CoreMainWindow * mainWindow)
{
	mainWindow->splashScreen();
	mainWindow->showSplashScreenMessage(QObject::tr("Initializing application pathInterface"));
	QCoreApplication::processEvents();
	//inicjalizacja œcie¿ek aplikacji, katalogów tymczasowych itp, u¿ywane do ³adowania t³umaczeñ
	{
		if(trySetPathsFromRegistry(paths_) == false){
			setDefaultPaths(paths_);
		}

		//sprawdzamy czy uda³o siê wygenerowac poprawne sciezki alikacji
		if(paths_ == nullptr){
			throw std::runtime_error("Could not initialize application pathInterface");
		}
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing log"));
	QCoreApplication::processEvents();

	//Mam œcie¿kê do konfiguracji loggera, nie wiem czy to OSG, Log4cxx czy pusty albo coœ jeszcze innego - initializer powinien to zrobiæ za mnie
	//powinien te¿ obs³ugiwaæ widget!!
	{	
		logInitializer_.reset(new LogInitializer((paths_->getResourcesPath() / "settings" / "log.ini").string()));

		loggerPrototype_.reset(new Log());
		logger_ = loggerPrototype_->subLog("core");
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing directories"));
	QCoreApplication::processEvents();

	//probujemy tmp katalog zapewniæ
	try{
		Filesystem::createDirectory(paths_->getTmpPath());
	}catch(...){
		CORE_LOG_ERROR("Could not create temporary directory: " << paths_->getTmpPath());
		throw;
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing translations"));
	QCoreApplication::processEvents();

	//t³umaczenia aplikacji
	{
		QString locale = QLocale::system().name();

		auto langPath = QString::fromStdString((paths_->getResourcesPath() / "lang").string());

		QTranslator appTranslator;

		if(appTranslator.load("lang_" + locale, langPath) == false) {
			appTranslator.load(QString("lang_pl_PL"), langPath);
			//TODO
			//jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
		}

		uiApplication_->installTranslator(&appTranslator);

		QTranslator qtTranslator;

		if(qtTranslator.load("qt_" + QLocale::languageToString(QLocale::system().language()), langPath) == false) {
			qtTranslator.load(QString("qt_pl"), langPath);
			//TODO
			//jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
		}

		uiApplication_->installTranslator(&qtTranslator);
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing 3D context"));
	QCoreApplication::processEvents();

	{
		//HACK - wymuszamy statyczne linkowanie z bibliotek¹ osgQt.
		// to gwarantuje poprawne zainicjowanie obiektu HeartBeat odpowiedzialnego
		// za obs³ugê scen OpenGL po stronie widgetów OSG.
		// Choæ niekoniecznie w tym w¹tku z tego kozystamy ( nie musimy mieæ tworzonych na starcie ¿adnych widgetów OSG)
		// jest to niezbêdne dla prawid³owej deinicjalizacji tego obiektu - czasu ¿ycia jego zasobów.
		// W przeciwnym wypadku powstanie kilka instancji tego obiektu - po jednej dla ka¿dego pluginu dostarczaj¹cego widgetów OSG
		// Bardzo niebezpieczne!! Powodowa³o crash aplikacji przy inicjalizacji a potem przy zamykaniu
		boost::shared_ptr<QWidget> w(new osgQt::GLWidget());
	}	

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing core managers"));
	QCoreApplication::processEvents();

	dataHierarchyManager_.reset(new DataHierarchyManager());
	memoryDataManager_.reset(new MemoryDataManager());
	parserManager_.reset(new ParserManager());
	streamDataManager_.reset(new StreamDataManager());
	fileDataManager_.reset(new FileDataManager());
	serviceManager_.reset(new ServiceManager());
	sourceManager_.reset(new SourceManager());
	visualizerManager_.reset(new VisualizerManager());	

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing plugins loader"));
	QCoreApplication::processEvents();

	//inicjalizacja obiektu ³aduj¹cego pluginy
	pluginLoader_.reset(new PluginLoader(Filesystem::Path(QCoreApplication::applicationFilePath().toStdString()).parent_path()));

	//obsluga dodatkowej sciezki z pluginami zewnetrznymi
	if(additionalPluginsPath.empty() == false && Filesystem::pathExists(additionalPluginsPath) == true)
	{
		Filesystem::Iterator endIT;
		std::for_each(Filesystem::Iterator(additionalPluginsPath), endIT, [=](const Filesystem::Path & p) {
			if (Filesystem::isDirectory(p)) {
				pluginLoader_->addPath(p);
				CORE_LOG_INFO("Plugin path added: " << p);
			}
		});			
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Loading plugins"));
	QCoreApplication::processEvents();

	//³adujemy pluginy
	pluginLoader_->load();

	for(int i = 0; i < pluginLoader_->getNumPlugins(); ++i){
		unpackPlugin(mainWindow, pluginLoader_->getPlugin(i));
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing services"));
	QCoreApplication::processEvents();

	// inicjalizacja us³ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->init(sourceManager_.get(),
			visualizerManager_.get(), memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	// inicjalizacja us³ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->lateInit();
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing sources"));
	QCoreApplication::processEvents();

	// inicjalizacja Ÿróde³	
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		sourceManager_->getSource(i)->init(memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing console"));
	QCoreApplication::processEvents();

	logInitializer_->setConsoleWidget(mainWindow->getConsole());

	mainWindow->showSplashScreenMessage(QObject::tr("Initializing main view"));
	QCoreApplication::processEvents();
	//TODO
	//mainWindow->init();

	mainWindow->show();
}

int Application::run()
{
	return uiApplication_->exec();
}

Application::~Application()
{
	//zeruje ju¿ konsole - wiêcej z niej nie bêdê korzysta³
	logInitializer_->setConsoleWidget(nullptr);

	//TODO
	CORE_LOG_INFO("Closing core application");
	CORE_LOG_INFO("Releasing sources");
	CORE_LOG_INFO("Stopping services");
	CORE_LOG_INFO("Cleaning tmp files");
	CORE_LOG_INFO("Closing core managers");
	CORE_LOG_INFO("Releasing plugins");
	CORE_LOG_INFO("Closing log");
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
	//TODO
	//mieæ na uwadze nazwê aplikacji i PJWSTK
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

		visualizerManager_->registerVisualizerPrototype(visualizer);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
			<< " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
			<< " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!" );
	}
}

//void Application::registerCoreDomainTypes()
//{
//	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<int>());
//	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<double>());
//}

void Application::unpackPlugin(CoreMainWindow * mainWindow, const core::PluginPtr & plugin)
{
	auto message = QObject::tr("Loading plugin %1 content: %2").arg(QString::fromStdString(plugin->getName()));

	mainWindow->showSplashScreenMessage(message.arg(QObject::tr("domain objects")));
	QCoreApplication::processEvents();

	for(int j = 0; j < plugin->getNumObjectWrapperPrototypes(); ++j) {
		safeRegisterObjectWrapperPrototype(plugin->getObjectWrapperPrototype(j));
	}

	mainWindow->showSplashScreenMessage(message.arg(QObject::tr("services")));
	QCoreApplication::processEvents();

	for ( int j = 0; j < plugin->getNumServices(); ++j ) {
		safeRegisterService(plugin->getService(j));
	}

	mainWindow->showSplashScreenMessage(message.arg(QObject::tr("sources")));
	QCoreApplication::processEvents();

	for ( int j = 0; j < plugin->getNumSources(); ++j ) {
		safeRegisterSource(plugin->getSource(j));
	}

	mainWindow->showSplashScreenMessage(message.arg(QObject::tr("parsers")));
	QCoreApplication::processEvents();

	for(int j = 0; j < plugin->getNumParsersPrototypes(); ++j) {
		safeRegisterParser(plugin->getParserPrototype(j));
	}
	
	mainWindow->showSplashScreenMessage(message.arg(QObject::tr("visualizers")));
	QCoreApplication::processEvents();

	for(int j = 0; j < plugin->getNumVisualizerPrototypes(); ++j) {
		safeRegisterVisualizer(plugin->getVisualizerPrototype(j));
	}
}

void Application::finalizeUI(){

	try{
		sourceManager_->finalizeSources();
		serviceManager_->finalizeServices();
	}catch(std::exception & e){
		CORE_LOG_ERROR("Error while closing UI during sources and services finalization: " << e.what());
	}catch(...){
		CORE_LOG_ERROR("UNKNOWN error while closing UI during sources and services finalization");
	}
}