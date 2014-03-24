#include "CorePCH.h"
#include <QtGui/QApplication>
#include <osgQt/GraphicsWindowQt>
#include <osg/ArgumentParser>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include <QtCore/QLocale>
#include <QtGui/QDesktopServices>
#include <QtCore/QDir>
#include "Config.h"
#include <coreui/CoreMainWindow.h>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>
#include "ApplicationCommon.h"
#include "VisualizerManager.h"
#include "ServiceManager.h"
#include "MemoryDataManager.h"
#include "ParserManager.h"
#include "StreamDataManager.h"
#include "FileDataManager.h"
#include "DataHierarchyManager.h"
#include "HierarchyDataManager.h"
#include "SourceManager.h"
#include "LogInitializer.h"
#include "PluginLoader.h"
#include "ThreadPool.h"
#include "JobManager.h"
#include "LanguagesManager.h"
#include "LanguagesHelper.h"
#include "LanguagesLoader.h"
#include "LanguageChangeFilter.h"
#include <boost/algorithm/string.hpp>

#ifdef WIN32
#include <Windows.h>
//#define KEY_PATH1 TEXT("Software\\Wow6432Node\\PJWSTK\\EDR")
// Od Visty dodawane s? przedrostki typu Wow6432Node do sciezki w rejestrach
// adres podawany do oczytu klucza powinien by? automatycznie konwertowany.
#define KEY_PATH TEXT("Software\\PJWSTK\\MDE")
#endif

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

const double TimeDelta = 1000.0/25.0;

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
			CORE_LOG_ERROR("Error occurred in UI during user operation execution for QObject "
				<< receiver->objectName().toStdString()  << " while processing event type " << event->type() << ": " << e.what());
		} catch (...) {
			CORE_LOG_ERROR("Unknown error occurred in UI during user operation execution for QObject "
				<< receiver->objectName().toStdString() << " while processing event type " << event->type());
		}
		return false;
	}
};

}

using namespace coreUI;
using namespace core;

void Application::updateVisualizers()
{
	visualizerManager_->update(visualizerTimeDelta);
}

void Application::updateServices()
{
	serviceManager_->update(servicesTimeDelta);
}

Application::Application() : visualizerTimeDelta(TimeDelta), servicesTimeDelta(TimeDelta), mainWindow(nullptr), uiInit(false)
{
	
}

void Application::showSplashScreenMessage(const QString & message)
{
	mainWindow->showSplashScreenMessage(message);
	QApplication::processEvents();
}

int Application::initUIContext(int & argc, char *argv[], std::vector<Filesystem::Path> & coreTranslations)
{
	//obs?uga argument?w i opisu uzycia aplikacji z konsoli
	osg::ArgumentParser arguments(&argc,argv);
	arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of MDE.");
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename");
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
	arguments.getApplicationUsage()->addCommandLineOption("--plugins <path>","Additional plugins directory");

	// czy wy?wietlamy pomoc?
	if (arguments.read("-h") || arguments.read("--help"))
	{
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}

	// pdczytujemy czy jest podana dodatkowa ?cie?ka dla plugin?w
	std::string path;
	arguments.read("--plugins",path);
	if(path.empty() == false){
		additionalPluginsPath = path;
	} 
	// inicjalizacja UI, wszystkich potrzebnych zasob?w
	{
		uiApplication_.reset(new coreUI::UIApplication(argc, argv));
		QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath()+"/plugins");

		//ustawienia aplikacji
		QSettings::setDefaultFormat(QSettings::IniFormat);
	}

	//inicjalizacja ?cie?ek aplikacji, katalog?w tymczasowych itp, u?ywane do ?adowania t?umacze?
	{
		if(trySetPathsFromRegistry(paths_) == false){
			setDefaultPaths(paths_);
		}

		//sprawdzamy czy uda?o si? wygenerowac poprawne sciezki alikacji
		if(paths_ == nullptr){
			throw std::runtime_error("Could not initialize application path interface");
		}
	}	

	//teraz inicjujemy logger zeby moc juz wszystko logowaæ
	//potem moge dolaczyc widget
	{	
		logInitializer_.reset(new LogInitializer((paths_->getApplicationDataPath() / "resources" / "settings" / "log.ini").string()));

		loggerPrototype_.reset(new Log());
		logger_ = loggerPrototype_->subLog("core");
	}

	//! DOPIERO OD TEGO MOMENTU MOGÊ LOGOWAC INFORMACJE!!

	//t?umaczenia aplikacji - musze to robi? tutaj aby wszystkie nowo utworzone widgety ju? widzia?y t?umaczenia
	//alternatywnie mog? to robi? p?niej pod warunkiem ?e wszystkie widgety sa ?wiadome t?umacze? - obs?uguj? event
	// zmiany j?zyka!! Dla plik?w Ui ju? jest metoda retranslateUi
	{
		//inicjalizujemy klase pomocnicza przy obsludze tlumaczen i jezykow
		LanguagesHelper::init();

		// teraz inicjujemy manager jezykow
		languagesManager_.reset(new LanguagesManager);

		//pliki tlumaczen dostarczone z instalacja
		coreTranslations = core::Filesystem::listFiles(paths_->getTranslationsPath(), true, ".qm");

		// ladujemy tlumaczenia dla core, qt i widoku
		LanguagesLoader::loadCoreTranslations(coreTranslations, languagesManager_.get());

		// dodaje na sztywno angielski zebym mial przynajmniej jeden jezyk
		// a qt w takiej wersji jest domyslnie i dla niej nie ma
		// translatora
		
		auto en = LanguagesHelper::languageFromISO639Code("en");

		languagesManager_->registerTranslator("qt",	en,
			LanguagesManager::TranslatorPtr());

		//ustawiamy jezyk
		//TODO
		// powinienem sprawdziæ w lokalnej bazie/rejestrze/jakims konfigu
		// czy user juz nie wybral jezyka, jesli tak to ten jezyk ustawic,
		// a jak nie to probowac go pobrac z systemu i jak ten sie nie
		// powiedzie to domyslnie angielski
		std::string locale = QLocale::system().name().toStdString();

		auto lang = LanguagesHelper::extractLanguageISO639Code(locale);

		if(lang.empty() == true){
			lang = en;
			
			QMessageBox::information(nullptr, QObject::tr("Translation problem"),
				QObject::tr("Could not deduce application language from locale %1. Application will be launched with English translation").arg(QLocale::system().name()));

		}else {

			auto registeredLangs = languagesManager_->registeredLanguages();
			if(registeredLangs.find(lang) == registeredLangs.end()){
				lang = en;

				QMessageBox::information(nullptr, QObject::tr("Translation problem"),
					QObject::tr("Could not load application with %1 language - no translations for this language. Application will be launched with English translation").arg(QString::fromStdString(lang)));
			}
		}

		languagesManager_->setLanguage(lang);
	}

	return 0;
}

void Application::initWithUI(CoreMainWindow * mainWindow,
	std::vector<Filesystem::Path> & translations)
{
	this->mainWindow = mainWindow;
	mainWindow->splashScreen();

	showSplashScreenMessage(QObject::tr("Initializing directories"));

	//probujemy tmp katalog zapewni?
	try{
		Filesystem::createDirectory(paths_->getTmpPath());
	}catch(...){
		CORE_LOG_ERROR("Could not create temporary directory: " << paths_->getTmpPath());
		throw;
	}	

	showSplashScreenMessage(QObject::tr("Initializing 3D context"));

	{
		//HACK - wymuszamy statyczne linkowanie z bibliotek? osgQt.
		// to gwarantuje poprawne zainicjowanie obiektu HeartBeat odpowiedzialnego
		// za obs?ug? scen OpenGL po stronie widget?w OSG.
		// Cho? niekoniecznie w tym w?tku z tego kozystamy ( nie musimy mie? tworzonych na starcie ?adnych widget?w OSG)
		// jest to niezb?dne dla prawid?owej deinicjalizacji tego obiektu - czasu ?ycia jego zasob?w.
		// W przeciwnym wypadku powstanie kilka instancji tego obiektu - po jednej dla ka?dego pluginu dostarczaj?cego widget?w OSG
		// Bardzo niebezpieczne!! Powodowa?o crash aplikacji przy inicjalizacji a potem przy zamykaniu
		boost::shared_ptr<QWidget> w(new osgQt::GLWidget());
	}	

	showSplashScreenMessage(QObject::tr("Initializing core managers"));

	dataHierarchyManager_.reset(new DataHierarchyManager());
	memoryDataManager_.reset(new HierarchyDataManager());
	parserManager_.reset(new ParserManager());
	streamDataManager_.reset(new StreamDataManager());
	fileDataManager_.reset(new FileDataManager());
	serviceManager_.reset(new ServiceManager());
	sourceManager_.reset(new SourceManager());
	visualizerManager_.reset(new VisualizerManager());

	//Wielow¹tkowoœæ
	{	
		showSplashScreenMessage(QObject::tr("Initializing threading"));

		threadFactory_.reset(new utils::QtThreadFactory());

		const core::ThreadPool::size_type threadsCount = QThread::idealThreadCount();

		threadPool_.reset(new core::ThreadPool(threadFactory_, (threadsCount / 2) + 1 , threadsCount * 50));

		core::ThreadPool::Threads ts;
		threadPool_->getThreads("Core", ts, 1, true);
		ts.front()->setDestination("JobManager maintenance thread");

		jobManager_.reset(new core::JobManager(ts.front()));

		ts.clear();
	
		threadPool_->getThreads("Core", ts, threadsCount - 1, true);

		for(auto it = ts.begin(); it != ts.end(); ++it){
			(*it)->setDestination("JobManager worker");
			jobManager_->addWorkerThread(*it);
		}
	}

	showSplashScreenMessage(QObject::tr("Initializing plugins loader"));
	
	//inicjalizacja obiektu ?aduj?cego pluginy
	pluginLoader_.reset(new PluginLoader(Filesystem::Path(QCoreApplication::applicationFilePath().toStdString()).parent_path()));

#if defined(__WIN32__)
	if (additionalPluginsPath.empty() == true) {
		additionalPluginsPath = getPathInterface()->getPluginPath();
	}
#endif

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

	showSplashScreenMessage(QObject::tr("Loading plugins"));	

	{
		//tworze filtr zeby w unpackPlugin moc spokojnie dodawac nowe tlumaczenia
		LanguageChangeFilter filter;

		//ladujemy pluginy
		pluginLoader_->load();
		
		auto pluginTranslations = core::Filesystem::listFiles(paths_->getPluginPath(), true, ".qm");
		translations.insert(translations.end(), pluginTranslations.begin(), pluginTranslations.end());

		for(int i = 0; i < pluginLoader_->getNumPlugins(); ++i){
			auto plugin = pluginLoader_->getPlugin(i);
			auto pluginName = plugin->getPath().stem().string();

// je¿eli jestesmy w debug to pluginy maj¹ d na koñcu a t³umaczenia nie!!
// muszê siê pozbyæ d
#ifdef _DEBUG

			if(pluginName.back() == 'd'){
				pluginName.resize(pluginName.size() - 1);
			}

#endif

			LanguagesLoader::loadPluginDefaultTranslation(pluginName,
				plugin->getDefaultLanguageCode(), languagesManager_.get());

			LanguagesLoader::loadPluginTranslations(translations,
				pluginName, languagesManager_.get());			

			unpackPlugin(mainWindow, plugin);
		}
	}

	showSplashScreenMessage(QObject::tr("Initializing services"));	

	// inicjalizacja us?ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->init(sourceManager_.get(),
			visualizerManager_.get(), memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	// inicjalizacja us?ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->lateInit();
	}

	showSplashScreenMessage(QObject::tr("Initializing sources"));

	// inicjalizacja ?r?de?	
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		sourceManager_->getSource(i)->init(memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	// inicjalizacja ?r?de?	
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		sourceManager_->getSource(i)->lateInit();
	}

	showSplashScreenMessage(QObject::tr("Initializing console"));	

	logInitializer_->setConsoleWidget(mainWindow->getConsole());

	showSplashScreenMessage(QObject::tr("Initializing main view"));	

	mainWindow->show();

	mainWindow->splashScreen()->finish(mainWindow);

	uiInit = true;
}

int Application::run()
{
	if(uiInit == true){
		connect(&visualizerUpdateTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
		connect(&servicesUpdateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));

		visualizerUpdateTimer.start(visualizerTimeDelta);
		servicesUpdateTimer.start(servicesTimeDelta);
	}

	return uiApplication_->exec();
}

Application::~Application()
{
	if(uiInit == true){
		//zeruje ju? konsole - wi?cej z niej nie b?d? korzysta?
		CORE_LOG_INFO("Closing core application");

		CORE_LOG_INFO("Releasing sources");
		sourceManager_.reset();
		CORE_LOG_INFO("Releasing services");
		serviceManager_.reset();
		CORE_LOG_INFO("Releasing visualizer manager");
		visualizerManager_.reset();
		CORE_LOG_INFO("Releasing stream data manager");
		streamDataManager_.reset();
		CORE_LOG_INFO("Releasing file data manager");
		fileDataManager_.reset();
		CORE_LOG_INFO("Releasing parsers manager");
		parserManager_.reset();
		CORE_LOG_INFO("Releasing memory data manager");
		memoryDataManager_.reset();
		CORE_LOG_INFO("Releasing hierarchy data manager");
		dataHierarchyManager_.reset();
		CORE_LOG_INFO("Releasing plugins");
		pluginLoader_.reset();

		CORE_LOG_INFO("Releasing job manager");
		jobManager_.reset();
		CORE_LOG_INFO("Releasing thread pool");
		threadPool_.reset();
		CORE_LOG_INFO("Releasing thread factory");
		threadFactory_.reset();

		CORE_LOG_INFO("Cleaning translations");
		languagesManager_.reset();

		CORE_LOG_INFO("Cleaning UI context");
		uiApplication_.reset();
		
		CORE_LOG_INFO("Cleaning tmp files");
		Filesystem::deleteDirectory(getPathInterface()->getTmpPath());
		
		CORE_LOG_INFO("Releasing core log");
		logger_.reset();
	}
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

core::ThreadPool* Application::threadPool()
{
	return threadPool_.get();
}

core::JobManager* Application::jobManager()
{
	return jobManager_.get();
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

	Filesystem::Path userApplicationDataPath;

	lpValueName = "UserApplicationDataPath";
	dwSize = sizeof(buffer);
	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, KEY_PATH, 0, KEY_READ, &hKey);
	if(lResult == ERROR_SUCCESS && RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		userApplicationDataPath = Filesystem::Path(buffer);
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

	path.reset(new Path(userDataPath, applicationDataPath, userApplicationDataPath, resourcesPath, userDataPath / "tmp", applicationDataPath / "plugins"));

	return true;
#else
	return false;
#endif
}

void Application::setDefaultPaths(utils::shared_ptr<Path> & path)
{
	//TODO
	//mie? na uwadze nazw? aplikacji i PJWSTK
	auto userPath = Filesystem::Path(QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation).toStdString()) / "PJWSTK" / "MDE";
	auto appDataPath = Filesystem::Path(QDesktopServices::storageLocation(QDesktopServices::DataLocation).toStdString());
	//TODO - czy pod linux taka konwencja jest ok? jak tam dzia³aj¹ takie wspólne foldery?
	auto userAppDataPath = appDataPath;
	auto resourcesPath = Filesystem::Path(QDir::currentPath().toStdString()) / "resources";
	
	path.reset(new Path(userPath, appDataPath, userAppDataPath, resourcesPath, userPath / "tmp", appDataPath / "plugins"));
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

void Application::unpackPlugin(CoreMainWindow * mainWindow, const core::PluginPtr & plugin)
{
	auto message = QObject::tr("Loading plugin %1 content: %2").arg(QString::fromStdString(plugin->getName()));

	showSplashScreenMessage(message.arg(QObject::tr("domain objects")));	

	for(int j = 0; j < plugin->getNumObjectWrapperPrototypes(); ++j) {
		safeRegisterObjectWrapperPrototype(plugin->getObjectWrapperPrototype(j));
	}

	showSplashScreenMessage(message.arg(QObject::tr("services")));	

	for ( int j = 0; j < plugin->getNumServices(); ++j ) {
		safeRegisterService(plugin->getService(j));
	}

	showSplashScreenMessage(message.arg(QObject::tr("sources")));	

	for ( int j = 0; j < plugin->getNumSources(); ++j ) {
		safeRegisterSource(plugin->getSource(j));
	}

	showSplashScreenMessage(message.arg(QObject::tr("parsers")));	

	for(int j = 0; j < plugin->getNumParsersPrototypes(); ++j) {
		safeRegisterParser(plugin->getParserPrototype(j));
	}
	
	showSplashScreenMessage(message.arg(QObject::tr("visualizers")));	

	for(int j = 0; j < plugin->getNumVisualizerPrototypes(); ++j) {
		safeRegisterVisualizer(plugin->getVisualizerPrototype(j));
	}
}

void Application::finalizeUI(){

	if(uiInit == true){

		try{
			CORE_LOG_INFO("Finalizing sources");
			sourceManager_->finalizeSources();

			CORE_LOG_INFO("Finalizing services");
			serviceManager_->finalizeServices();

			CORE_LOG_INFO("Closing log widget console");
			logInitializer_->setConsoleWidget(nullptr);

			CORE_LOG_INFO("Reseting log system");
			logInitializer_.reset();		
		
		}catch(std::exception & e){
			CORE_LOG_ERROR("Error while closing UI during sources and services finalization: " << e.what());
		}catch(...){
			CORE_LOG_ERROR("UNKNOWN error while closing UI during sources and services finalization");
		}
	}
}