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

void Application::updateVisualizers()
{
	visualizerManager_->update(visualizerTimeDelta);
}

void Application::updateServices()
{
	serviceManager_->update(servicesTimeDelta);
}

Application::Application() : visualizerTimeDelta(0), servicesTimeDelta(0), mainWindow(nullptr)
{
	connect(&visualizerUpdateTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
	connect(&servicesUpdateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
}

void Application::showSplashScreenMessage(const QString & message)
{
	mainWindow->showSplashScreenMessage(message);
	QApplication::processEvents();
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

	//t³umaczenia aplikacji - musze to robiæ tutaj aby wszystkie nowo utworzone widgety ju¿ widzia³y t³umaczenia
	//alternatywnie mogê to robiæ póŸniej pod warunkiem ¿e wszystkie widgety sa œwiadome t³umaczeñ - obs³uguj¹ event
	// zmiany jêzyka!! Dla plików Ui ju¿ jest metoda retranslateUi
	{
		std::string locale = QLocale::system().name().toStdString();
		auto langPath = paths_->getResourcesPath() / "lang";
		// pobieram listê wszystkich dostarczonych standardowo t³umaczeñ
		auto files = core::Filesystem::listFiles(langPath.string(), true, ".qm");

		for(auto it = files.begin(); it != files.end(); ++it)
		{
			// sprawdzam czy t³umaczenie pasuje do aktualnego jêzyka
			if((*it).find(locale) != std::string::npos ){

				shared_ptr<QTranslator> translator(new QTranslator);

				if(translator->load((*it).c_str()) == true) {
					uiApplication_->installTranslator(translator.get());
					translators_.push_back(translator);
					//TODO
					//jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
				}
			}
		}		

		// teraz t³umaczenia z Qt

		shared_ptr<QTranslator> translator(new QTranslator);
		bool translationFound = translator->load(("qt_" + locale).c_str(), langPath.string().c_str());
		if(translationFound == false) {
			translationFound = translator->load(QString("qt_pl"), langPath.string().c_str());
			// TODO
			// jak tego siê nie uda za³adowaæ to mamy tylko angielski jêzyk - trzeba poinformowaæ
		}

		if(translationFound == true){
			uiApplication_->installTranslator(translator.get());
			translators_.push_back(translator);
		}
	}

	return 0;
}

void Application::initWithUI(CoreMainWindow * mainWindow)
{
	this->mainWindow = mainWindow;
	mainWindow->splashScreen();
	

	showSplashScreenMessage(QObject::tr("Initializing log"));	

	//Mam œcie¿kê do konfiguracji loggera, nie wiem czy to OSG, Log4cxx czy pusty albo coœ jeszcze innego - initializer powinien to zrobiæ za mnie
	//powinien te¿ obs³ugiwaæ widget!!
	{	
		logInitializer_.reset(new LogInitializer((paths_->getResourcesPath() / "settings" / "log.ini").string()));

		loggerPrototype_.reset(new Log());
		logger_ = loggerPrototype_->subLog("core");
	}

	showSplashScreenMessage(QObject::tr("Initializing directories"));
	

	//probujemy tmp katalog zapewniæ
	try{
		Filesystem::createDirectory(paths_->getTmpPath());
	}catch(...){
		CORE_LOG_ERROR("Could not create temporary directory: " << paths_->getTmpPath());
		throw;
	}

	showSplashScreenMessage(QObject::tr("Initializing translations"));	

	

	showSplashScreenMessage(QObject::tr("Initializing 3D context"));

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

	showSplashScreenMessage(QObject::tr("Initializing core managers"));

	dataHierarchyManager_.reset(new DataHierarchyManager());
	memoryDataManager_.reset(new MemoryDataManager());
	parserManager_.reset(new ParserManager());
	streamDataManager_.reset(new StreamDataManager());
	fileDataManager_.reset(new FileDataManager());
	serviceManager_.reset(new ServiceManager());
	sourceManager_.reset(new SourceManager());
	visualizerManager_.reset(new VisualizerManager());	

	showSplashScreenMessage(QObject::tr("Initializing plugins loader"));
	
	//inicjalizacja obiektu ³aduj¹cego pluginy
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

	//³adujemy pluginy
	pluginLoader_->load();

	for(int i = 0; i < pluginLoader_->getNumPlugins(); ++i){
		unpackPlugin(mainWindow, pluginLoader_->getPlugin(i));
	}

	showSplashScreenMessage(QObject::tr("Initializing services"));	

	// inicjalizacja us³ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->init(sourceManager_.get(),
			visualizerManager_.get(), memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	// inicjalizacja us³ug
	for (int i = 0; i < serviceManager_->getNumServices(); ++i) {
		serviceManager_->getService(i)->lateInit();
	}

	showSplashScreenMessage(QObject::tr("Initializing sources"));

	// inicjalizacja Ÿróde³	
	for (int i = 0; i < sourceManager_->getNumSources(); ++i) {
		sourceManager_->getSource(i)->init(memoryDataManager_.get(), streamDataManager_.get(), fileDataManager_.get());
	}

	showSplashScreenMessage(QObject::tr("Initializing console"));	

	logInitializer_->setConsoleWidget(mainWindow->getConsole());

	showSplashScreenMessage(QObject::tr("Initializing main view"));	

	mainWindow->show();

	mainWindow->splashScreen()->close();
}

int Application::run()
{
	servicesTimeDelta = visualizerTimeDelta = 1000.0/25.0;

	visualizerUpdateTimer.start(visualizerTimeDelta);
	servicesUpdateTimer.start(servicesTimeDelta);	
	return uiApplication_->exec();
}

Application::~Application()
{
	//zeruje ju¿ konsole - wiêcej z niej nie bêdê korzysta³
	CORE_LOG_INFO("Closing core application");

	CORE_LOG_INFO("Releasing sources");
	sourceManager_.reset();
	CORE_LOG_INFO("Releasing services");
	serviceManager_.reset();

	CORE_LOG_INFO("Releasing core managers");
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

	CORE_LOG_INFO("Cleaning UI context");
	uiApplication_.reset();

	CORE_LOG_INFO("Cleaning translations");
	std::vector<shared_ptr<QTranslator>>().swap(translators_);

	CORE_LOG_INFO("Cleaning tmp files");
	Filesystem::deleteDirectory(getPathInterface()->getTmpPath());

	CORE_LOG_INFO("Releasing logs - reseting to default loggers");
	logInitializer_.reset();

	CORE_LOG_INFO("Releasing log prototype");
	loggerPrototype_.reset();

	CORE_LOG_INFO("Releasing core log");
	logger_.reset();
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

	try{
		CORE_LOG_INFO("Finalizing sources");
		sourceManager_->finalizeSources();

		CORE_LOG_INFO("Finalizing services");
		serviceManager_->finalizeServices();

		CORE_LOG_INFO("Closing log widget console");
		logInitializer_->setConsoleWidget(nullptr);
		
		
	}catch(std::exception & e){
		CORE_LOG_ERROR("Error while closing UI during sources and services finalization: " << e.what());
	}catch(...){
		CORE_LOG_ERROR("UNKNOWN error while closing UI during sources and services finalization");
	}
}