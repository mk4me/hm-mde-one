#include "CorePCH.h"
#include <QtWidgets/QApplication>
#include <osgQt/GraphicsWindowQt>
#include <osg/ArgumentParser>
#include <QtCore/QTranslator>
#include <QtCore/QSettings>
#include <QtCore/QLocale>
#include <QtCore/QStandardPaths>
#include <QtCore/QDir>
#include <utils/PtrPolicyStd.h>
#include "Config.h"
#include <corelib/Exceptions.h>
#include <coreui/CoreMainWindow.h>
#include <QtWidgets/QSplashScreen>
#include <QtWidgets/QMessageBox>
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
#include "LanguagesManager.h"
#include "LanguagesHelper.h"
#include "LanguagesLoader.h"
#include "LanguageChangeFilter.h"
#include <boost/algorithm/string.hpp>
#include "ApplicationDescription.h"

#ifdef CORE_ENABLE_LEAK_DETECTION
#include <utils/LeakDetection.h>
#endif

#ifdef WIN32
#define NOMINMAX
#include <Windows.h>
//#define KEY_PATH1 TEXT("Software\\Wow6432Node\\PJWSTK\\EDR")
// Od Visty dodawane s? przedrostki typu Wow6432Node do sciezki w rejestrach
// adres podawany do oczytu klucza powinien by? automatycznie konwertowany.
//#define KEY_PATH TEXT("Software\\PJWSTK\\MEDUSA")
#endif

DEFINE_WRAPPER(int, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);
DEFINE_WRAPPER(double, utils::PtrPolicyStd, utils::ClonePolicyCopyConstructor);

const double TimeDelta = 1000.0 / 25.0;

namespace coreUI {
	class UIApplication : public QApplication
	{
	public:
		UIApplication(int & argc, char *argv[]) : QApplication(argc, argv) {}

		virtual bool notify(QObject* receiver, QEvent* event)
		{
			try {
				return QApplication::notify(receiver, event);
			}
			catch (std::exception &e) {
				CORE_LOG_ERROR("Error occurred in UI during user operation execution for QObject "
					<< receiver->objectName().toStdString() << " while processing event type " << event->type() << ": " << e.what());
			}
			catch (...) {
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

Application::Application() : vendor_("Polsko-Japo�ska Wy�sza Szko�a Technik Komputerowych, Oddzia� zamiejscowy w Bytomiu",
	"PJWSTK Bytom", "Uczelnia prywatna", "marek.kulbacki@gmail.com"),
	visualizerTimeDelta(TimeDelta), servicesTimeDelta(TimeDelta), mainWindow(nullptr), uiInit(false)
{
}

void Application::showSplashScreenMessage(const QString & message)
{
	mainWindow->showSplashScreenMessage(message);
	QApplication::processEvents();
}

int Application::initUIContext(int & argc, char *argv[], const std::string & appName,
	std::vector<Filesystem::Path> & coreTranslations)
{
	//obs?uga argument?w i opisu uzycia aplikacji z konsoli
	osg::ArgumentParser arguments(&argc, argv);
	//arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
	arguments.getApplicationUsage()->setApplicationName(appName);
	arguments.getApplicationUsage()->setDescription("example usage of " + appName);
	arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName() + " [options] filename");
	arguments.getApplicationUsage()->addCommandLineOption("-h or --help", "Display this information");
	arguments.getApplicationUsage()->addCommandLineOption("--plugins <path>", "Additional plugins directory");

	// czy wy?wietlamy pomoc?
	if (arguments.read("-h") || arguments.read("--help"))
	{
		arguments.getApplicationUsage()->write(std::cout);
		return 1;
	}

	// pdczytujemy czy jest podana dodatkowa ?cie?ka dla plugin?w
	std::string path;
	arguments.read("--plugins", path);
	if (path.empty() == false){
		additionalPluginsPath = path;
	}
	// inicjalizacja UI, wszystkich potrzebnych zasob?w
	{
		uiApplication_.reset(new coreUI::UIApplication(argc, argv));
		QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins");
		//TODO - przejrzec w Qt jak to edytowa�
		//QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath() + "/plugins/platforms");

		//ustawienia aplikacji
		QSettings::setDefaultFormat(QSettings::IniFormat);
	}

	//inicjalizacja ?cie?ek aplikacji, katalog?w tymczasowych itp, u?ywane do ?adowania t?umacze?
	{
		if (trySetPathsFromRegistry(paths_, appName) == false){
			setDefaultPaths(paths_, appName);
		}

		//sprawdzamy czy uda?o si? wygenerowac poprawne sciezki alikacji
		if (paths_ == nullptr){
			throw core::runtime_error("Could not initialize application path interface");
		}
	}

	//teraz inicjujemy logger zeby moc juz wszystko logowa�
	//potem moge dolaczyc widget
	{
		logInitializer_.reset(new LogInitializer((paths_->getApplicationDataPath() / "resources" / "settings" / "log.ini").string()));

		loggerPrototype_.reset(new Log());
		logger_ = loggerPrototype_->subLog("core");
	}

	//! DOPIERO OD TEGO MOMENTU MOG� LOGOWAC INFORMACJE!!
	ExceptionLogger::setLog(logger_);

	CORE_LOG_INFO("UserDataPath: " << paths_->getUserDataPath());
	CORE_LOG_INFO("ApplicationDataPath: " << paths_->getApplicationDataPath());
	CORE_LOG_INFO("UserApplicationDataPath: " << paths_->getUserApplicationDataPath());
	CORE_LOG_INFO("ResourcesPath: " << paths_->getResourcesPath());
	CORE_LOG_INFO("TmpPath: " << paths_->getTmpPath());
	CORE_LOG_INFO("PluginPath: " << paths_->getPluginPath());
	CORE_LOG_INFO("TranslationsPath: " << paths_->getTranslationsPath());

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

		const auto en = LanguagesHelper::languageFromISO639Code("en");

		languagesManager_->registerTranslator("qt", en,
			LanguagesManager::TranslatorPtr());

		//ustawiamy jezyk
		//TODO
		// powinienem sprawdzi� w lokalnej bazie/rejestrze/jakims konfigu
		// czy user juz nie wybral jezyka, jesli tak to ten jezyk ustawic,
		// a jak nie to probowac go pobrac z systemu i jak ten sie nie
		// powiedzie to domyslnie angielski
		const std::string locale = QLocale::system().name().toStdString();

		auto lang = LanguagesHelper::extractLanguageISO639Code(locale);

		CORE_LOG_INFO("System locale and language: " << locale << " -> " << lang);

		if (lang.empty() == true){
			QMessageBox::information(nullptr, QObject::tr("Translation problem"),
				QObject::tr("Could not deduce application language from locale %1. Application will be launched with English translation").arg(QString::fromStdString(locale)));

			lang = en;
		}
		else {
			auto registeredLangs = languagesManager_->registeredLanguages();
			if (registeredLangs.find(lang) == registeredLangs.end()){
				QMessageBox::information(nullptr, QObject::tr("Translation problem"),
					QObject::tr("Could not load application with %1 language - no translations for this language. Application will be launched with English translation").arg(QString::fromStdString(lang)));

				lang = en;
			}
		}

		CORE_LOG_DEBUG("Setting appliaction language to " << lang);
		languagesManager_->setLanguage(lang);
	}

	// Opis aplikacji
	{
		applicationDescription_.reset(new ApplicationDescription(&version_, &vendor_));
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
	}
	catch (...){
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
		utils::shared_ptr<QWidget> w(new osgQt::GLWidget());
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

	memoryDataManager_->addObserver(fileDataManager_);
	memoryDataManager_->addObserver(streamDataManager_);

	//Wielow�tkowo��
	{
		showSplashScreenMessage(QObject::tr("Initializing threading"));		

		core::ThreadPool::setLog(logger_->subLog("threadPool"));
		auto minThreads = std::thread::hardware_concurrency();
		if (minThreads < 2){
			//tyle by wypada�o �eby nowe maszyny dawa�y rad� - 2xCore x2 threads per core
			minThreads = 3;
		}
		else{
			//jeden dla gui natywnie
			--minThreads;
		}

		innerThreadPool.reset(new core::ThreadPool::InnerThreadPool(minThreads, minThreads * 10));
		threadPool_.reset(new core::ThreadPool(innerThreadPool.get()));

		core::JobManager::setLog(logger_->subLog("jobManager"));
		innerWorkManager_.reset(new core::JobManager::InnerWorkManager);
		innerJobManager_.reset(new core::JobManager::InnerJobManager(innerWorkManager_.get()));
		jobManager_.reset(new core::JobManager(innerJobManager_.get()));
			
		core::ThreadPool::Threads threads;
		threadPool_->get(std::thread::hardware_concurrency() - 1, threads,  true, "Core", "JobManager worker thread");

		for (auto & t : threads){			
			innerWorkManager_->addWorkerThread(std::move(t));
		}
	}

	showSplashScreenMessage(QObject::tr("Initializing plugins loader"));

	//inicjalizacja obiektu ?aduj?cego pluginy
	pluginLoader_.reset(new PluginLoader(Filesystem::Path(QCoreApplication::applicationFilePath().toStdString()).parent_path()));

#if defined(__WIN32__)
	if (additionalPluginsPath.empty() == true) {
		additionalPluginsPath = getPaths()->getPluginPath();
	}
#endif

	//obsluga dodatkowej sciezki z pluginami zewnetrznymi
	if (additionalPluginsPath.empty() == false && Filesystem::pathExists(additionalPluginsPath) == true)
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

		for (int i = 0; i < pluginLoader_->getNumPlugins(); ++i){
			auto plugin = pluginLoader_->getPlugin(i);
			auto pluginName = plugin->getPath().stem().string();

			// je�eli jestesmy w debug to pluginy maj� d na ko�cu a t�umaczenia nie!!
			// musz� si� pozby� d
#ifdef _DEBUG

			if (pluginName.back() == 'd'){
				pluginName.resize(pluginName.size() - 1);
			}

#endif

			LanguagesLoader::loadPluginDefaultTranslation(pluginName,
				plugin->defaultLanguageCode(), languagesManager_.get());

			LanguagesLoader::loadPluginTranslations(translations,
				pluginName, languagesManager_.get());

			unpackPlugin(mainWindow, plugin);
		}
	}

	showSplashScreenMessage(QObject::tr("Initializing services"));

	// inicjalizacja us?ug
	std::set<plugin::IServicePtr> skipServices;
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		try{
			serviceManager_->getService(i)->init(sourceManager_.get(),
				visualizerManager_.get(), memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
		}
		catch (std::exception & e){
			skipServices.insert(serviceManager_->getService(i));
			CORE_LOG_ERROR("Failed to initialize " << serviceManager_->getService(i)->name() << " service (ID: " << serviceManager_->getService(i)->ID() << ") -> " << e.what());
			try{
				serviceManager_->getService(i)->finalize();
			}
			catch (...){

			}
		}
		catch (...){
			skipServices.insert(serviceManager_->getService(i));
			CORE_LOG_ERROR("Failed to initialize " << serviceManager_->getService(i)->name() << " service (ID: " << serviceManager_->getService(i)->ID() << ")");
			try{
				serviceManager_->getService(i)->finalize();
			}
			catch (...){

			}
		}
	}

	// inicjalizacja us?ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {

		if (skipServices.find(serviceManager_->getService(i)) != skipServices.end()){
			continue;
		}

		try{
			serviceManager_->getService(i)->lateInit();
		}
		catch (std::exception & e){
			CORE_LOG_ERROR("Failed to do late initialization for " << serviceManager_->getService(i)->name() << " service (ID: " << serviceManager_->getService(i)->ID() << ") -> " << e.what());
			try{
				serviceManager_->getService(i)->finalize();
			}
			catch (...){

			}
		}
		catch (...){
			CORE_LOG_ERROR("Failed to do late initialization for " << serviceManager_->getService(i)->name() << " service (ID: " << serviceManager_->getService(i)->ID() << ")");
			try{
				serviceManager_->getService(i)->finalize();
			}
			catch (...){

			}
		}
	}

	showSplashScreenMessage(QObject::tr("Initializing sources"));

	// inicjalizacja ?r?de?
	std::set<plugin::ISourcePtr> skipSources;
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		try{
			sourceManager_->getSource(i)->init(memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
		}
		catch (std::exception & e){
			skipSources.insert(sourceManager_->getSource(i));
			CORE_LOG_ERROR("Failed to initialize " << sourceManager_->getSource(i)->name() << " source (ID: " << sourceManager_->getSource(i)->ID() << ") -> " << e.what());
			try{
				sourceManager_->getSource(i)->finalize();
			}
			catch (...){

			}
		}
		catch (...){
			skipSources.insert(sourceManager_->getSource(i));
			CORE_LOG_ERROR("Failed to initialize " << sourceManager_->getSource(i)->name() << " source (ID: " << sourceManager_->getSource(i)->ID() << ")");
			try{
				sourceManager_->getSource(i)->finalize();
			}
			catch (...){

			}
		}
	}

	// inicjalizacja ?r?de?
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {

		if (skipSources.find(sourceManager_->getSource(i)) != skipSources.end()){
			continue;
		}

		try{
			sourceManager_->getSource(i)->lateInit();
		}
		catch (std::exception & e){
			CORE_LOG_ERROR("Failed to do late initialization for " << sourceManager_->getSource(i)->name() << " source (ID: " << sourceManager_->getSource(i)->ID() << ") -> " << e.what());
			try{
				sourceManager_->getSource(i)->finalize();
			}
			catch (...){

			}
		}
		catch (...){
			CORE_LOG_ERROR("Failed to do late initialization for " << sourceManager_->getSource(i)->name() << " source (ID: " << sourceManager_->getSource(i)->ID() << ")");
			try{
				sourceManager_->getSource(i)->finalize();
			}
			catch (...){

			}
		}
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
	if (uiInit == true){
		connect(&visualizerUpdateTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
		connect(&servicesUpdateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));

		visualizerUpdateTimer.start(visualizerTimeDelta);
		servicesUpdateTimer.start(servicesTimeDelta);
	}

	return uiApplication_->exec();
}

Application::~Application()
{
	if (uiInit == true){
		CORE_LOG_INFO("Closing core application");

		CORE_LOG_INFO("Releasing sources");
		sourceManager_.reset();
		CORE_LOG_INFO("Releasing services");
		serviceManager_.reset();
		CORE_LOG_INFO("Releasing visualizer manager");
		visualizerManager_.reset();
		CORE_LOG_INFO("Releasing stream data manager");
		memoryDataManager_->removeObserver(streamDataManager_);
		streamDataManager_.reset();
		CORE_LOG_INFO("Releasing file data manager");
		memoryDataManager_->removeObserver(fileDataManager_);
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
		innerJobManager_.reset();
		innerWorkManager_.reset();		
		core::JobManager::setLog(LogPtr());
		CORE_LOG_INFO("Releasing thread pool");
		threadPool_.reset();
		innerThreadPool.reset();
		core::ThreadPool::setLog(LogPtr());
		CORE_LOG_INFO("Cleaning translations");
		languagesManager_.reset();

		CORE_LOG_INFO("Cleaning UI context");
		uiApplication_.reset();

		CORE_LOG_INFO("Cleaning tmp files");
		Filesystem::deleteDirectory(getPaths()->getTmpPath());

		CORE_LOG_INFO("Releasing application description");
		applicationDescription_.reset();

		CORE_LOG_INFO("Releasing core log and finally closing application");
		logger_.reset();
	}
}

ApplicationDescription * Application::description()
{
	return applicationDescription_.get();
}

LanguagesManager * Application::languageManager()
{
	return languagesManager_.get();
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

ThreadPool* Application::threadPool()
{
	return threadPool_.get();
}

JobManager* Application::jobManager()
{
	return jobManager_.get();
}

const Filesystem::Path resourcesPath()
{
	return Filesystem::Path(QCoreApplication::applicationFilePath().toStdString()).parent_path() / "resources";
}

bool Application::trySetPathsFromRegistry(utils::shared_ptr<Path> & path,
	const std::string & appName)
{
#ifdef WIN32
	//TODO
	//u�y� vendor info do generowania sciezki
#define KEY_PATH TEXT("Software\\PJWSTK\\")
#define PATH_BUFFER_SIZE 1024
	HKEY hKey;
	char buffer[PATH_BUFFER_SIZE];
	DWORD dwType, dwSize = PATH_BUFFER_SIZE;	

	std::string keyPath = KEY_PATH + appName;

	auto lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyPath.c_str(), 0, KEY_READ, &hKey);

	if (lResult != ERROR_SUCCESS){
		return false;
	}

	Filesystem::Path applicationDataPath;

	LPTSTR lpValueName = "ApplicationDataPath";
	dwSize = sizeof(buffer);
	if (RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		applicationDataPath = Filesystem::Path(buffer);
	}
	else {
		RegCloseKey(hKey);
		return false;
	}

	Filesystem::Path userApplicationDataPath;

	lpValueName = "UserApplicationDataPath";
	dwSize = sizeof(buffer);
	if (RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		userApplicationDataPath = Filesystem::Path(buffer);
	}
	else {
		RegCloseKey(hKey);
		return false;
	}

	Filesystem::Path userDataPath;

	lpValueName = "UserDataPath";
	dwSize = sizeof(buffer);
	if (RegQueryValueEx(hKey, lpValueName, 0, &dwType, (LPBYTE)buffer, &dwSize) == ERROR_SUCCESS) {
		userDataPath = Filesystem::Path(buffer);
	}
	else{
		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	path.reset(new Path(userDataPath, applicationDataPath, userApplicationDataPath, resourcesPath(), userDataPath / "tmp", applicationDataPath / "plugins"));

	return true;
#else
	return false;
#endif
}

void Application::setDefaultPaths(utils::shared_ptr<Path> & path, const std::string & appName)
{
	//TODO
	//ciagnac to info z vendor info?
	//mie? na uwadze nazw? aplikacji i PJWSTK
	auto userPath = Filesystem::Path(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0).toStdString()) / "PJWSTK" / appName;
#ifdef WIN32
	//HACK �eby dosta� si� do �cie�ek roaming dla usera, Qt w innych przypadkach podaje �ciezki w local
	QString settingsPath = QFileInfo(QSettings().fileName()).absolutePath();
	auto userAppDataPath = Filesystem::Path(settingsPath.toStdString()).parent_path() / "PJWSTK" / appName;

#else
	auto userAppDataPath = Filesystem::Path(QStandardPaths::standardLocations(QStandardPaths::DataLocation).at(0).toStdString()).parent_path() / "PJWSTK" / appName;
#endif // WIN32
	//TODO - czy pod linux taka konwencja jest ok? jak tam dzia�aj� takie wsp�lne foldery?
	auto appDataPath = Filesystem::Path(QStandardPaths::standardLocations(QStandardPaths::GenericConfigLocation).at(1).toStdString()) / "PJWSTK" / appName;

	path.reset(new Path(userPath, appDataPath, userAppDataPath, resourcesPath(), userPath / "tmp", appDataPath / "plugins"));
}

void Application::safeRegisterService(const plugin::IServicePtr & service)
{
	try{
		serviceManager_->registerService(service);
	}
	catch (std::exception & e){
		CORE_LOG_WARNING("Service " << service->name() << " " << service->description() << " with ID " << service->ID() <<
			" has caused an error during registration: " << e.what() << ". Service NOT registered in application!");
	}
	catch (...){
		CORE_LOG_WARNING("Service " << service->name() << " " << service->description() << " with ID " << service->ID() <<
			" has caused an UNKNOWN error during registration. Service NOT registered in application!");
	}
}

void Application::safeRegisterSource(const plugin::ISourcePtr & source)
{
	try{
		sourceManager_->registerSource(source);
	}
	catch (std::exception & e){
		CORE_LOG_WARNING("Source " << source->name() << " with ID " << source->ID() <<
			" has caused an error during registration: " << e.what() << ". Source NOT registered in application!");
	}
	catch (...){
		CORE_LOG_WARNING("Source " << source->name() << " with ID " << source->ID() <<
			" has caused an UNKNOWN error during registration. Source NOT registered in application!");
	}
}

void Application::safeRegisterParser(const plugin::IParserPtr & parser)
{
	try{
		parserManager_->registerParser(parser);
	}
	catch (std::exception & e){
		CORE_LOG_WARNING("Parser " << parser->description() << " with ID " << parser->ID() <<
			" has caused an error during registration: " << e.what() << ". Parser NOT registered in application!");
	}
	catch (...){
		CORE_LOG_WARNING("Parser " << parser->description() << " with ID " << parser->ID() <<
			" has caused an UNKNOWN error during registration. Parser NOT registered in application!");
	}
}

void Application::safeRegisterObjectWrapperPrototype(const utils::ObjectWrapperPtr & prototype)
{
	try{
		dataHierarchyManager_->registerObjectWrapperPrototype(prototype);
	}
	catch (std::exception & e){
		CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an error during registration: "
			<< e.what() << ". Object type NOT registered in application!");
	}
	catch (...){
		CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an UNKNOWN error during registration. Object type NOT registered in application!");
	}
}

void Application::safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer)
{
	try{
		visualizerManager_->registerVisualizerPrototype(visualizer);
	}
	catch (std::exception & e){
		CORE_LOG_WARNING("Visualizer " << visualizer->name() << " with ID " << visualizer->ID()
			<< " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!");
	}
	catch (...){
		CORE_LOG_WARNING("Visualizer " << visualizer->name() << " with ID " << visualizer->ID()
			<< " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!");
	}
}

void Application::unpackPlugin(CoreMainWindow * mainWindow, const core::PluginPtr & plugin)
{
	//auto message = QObject::tr("Loading plugin %1 content: %2").arg(QString::fromStdString(plugin->name()));

	//showSplashScreenMessage(message.arg(QObject::tr("domain objects")));

	for (int j = 0; j < plugin->getNumObjectWrapperPrototypes(); ++j) {
		safeRegisterObjectWrapperPrototype(plugin->getObjectWrapperPrototype(j));
	}

	//showSplashScreenMessage(message.arg(QObject::tr("services")));

	for (int j = 0; j < plugin->getNumServices(); ++j) {
		safeRegisterService(plugin->getService(j));
	}

	//showSplashScreenMessage(message.arg(QObject::tr("sources")));

	for (int j = 0; j < plugin->getNumSources(); ++j) {
		safeRegisterSource(plugin->getSource(j));
	}

	//showSplashScreenMessage(message.arg(QObject::tr("parsers")));

	for (int j = 0; j < plugin->getNumParsersPrototypes(); ++j) {
		safeRegisterParser(plugin->getParserPrototype(j));
	}

	//showSplashScreenMessage(message.arg(QObject::tr("visualizers")));

	for (int j = 0; j < plugin->getNumVisualizerPrototypes(); ++j) {
		safeRegisterVisualizer(plugin->getVisualizerPrototype(j));
	}
}

void Application::finalizeUI(){
	if (uiInit == true){
		try{

			visualizerUpdateTimer.stop();
			servicesUpdateTimer.stop();

			CORE_LOG_INFO("Finalizing sources");
			sourceManager_->finalizeSources();

			CORE_LOG_INFO("Finalizing services");
			serviceManager_->finalizeServices();

			CORE_LOG_INFO("Closing log widget console");
			logInitializer_->setConsoleWidget(nullptr);

			CORE_LOG_INFO("Reseting log system");
			logInitializer_.reset();
		}
		catch (std::exception & e){
			CORE_LOG_ERROR("Error while closing UI during sources, services and log finalization: " << e.what());
		}
		catch (...){
			CORE_LOG_ERROR("UNKNOWN error while closing UI during sources and services finalization");
		}
	}
}