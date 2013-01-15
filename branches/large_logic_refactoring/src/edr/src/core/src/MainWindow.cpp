#include "CorePCH.h"
#include "MainWindow.h"
#include "ApplicationCommon.h"
#include "FileDataManager.h"
#include "ParserManager.h"
#include "DataHierarchyManager.h"

#include <iostream>

#include <QtGui/QSplashScreen>

#include "PluginLoader.h"
#include <core/IManagersAccessor.h>
#include <core/TypeInfo.h>
#include <core/ObjectWrapperFactory.h>
#include <core/StringTools.h>
#include "Visualizer.h"

#include <osgui/QOsgWidgets.h>
#include <osgui/EventCallback.h>

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/BlendFunc>
#include <osg/LineWidth>

#include <osgViewer/GraphicsWindow>
#include <osgText/Text>
#include <osgViewer/Scene>
#include <osgGA/TerrainManipulator>
#include <osgWidget/ViewerEventHandlers>

#include "EDRConsoleWidget.h"

#include "ServiceManager.h"
#include "SourceManager.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include "UserInterfaceService.h"
#include "VisualizerManager.h"
#include "EDRConfig.h"
#include "EDRTitleBar.h"

//#include <utils/Debug.h>

#include <boost/foreach.hpp>
#include <boost/random.hpp>
#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>

#include "Log.h"

#include "VisualizerWidget.h"

#include <utils/Push.h>
#include "DataProcessor.h"

#include "EDRDockWidget.h"
#include <QtCore/QFile>
#include "LocalDataSource.h"
#include "CustomApplication.h"

//DEFINE_DEFAULT_LOGGER("edr.core");

using namespace core;

MainWindow * MainWindow::instance = nullptr;

void MainWindow::setStyle( const core::Filesystem::Path& path )
{
    QFile file(QString::fromUtf8(path.string().c_str()));
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString style;
        style = file.readAll();
        file.close();
        setStyleSheet(style);
    } else {
        throw std::runtime_error(std::string("Problem loading file : ") + path.string() );
    }
}


bool MainWindow::trySetStyle( const core::Filesystem::Path& path )
{
    try {
        setStyle(path);
    } catch(...) {
        return false;
    }

    return true;
}


void MainWindow::setStyleByName( const std::string& styleName )
{
    int count = getApplicationSkinsFilePathCount();
    for (int i = 0; i < count; ++i) {
        const core::Filesystem::Path& path = getApplicationSkinsFilePath(i);
        if (path.stem() == styleName) {
            setStyle(path);
            return;
        }
    }

    throw std::runtime_error(std::string("Unable to set style : ") + styleName);
}

void MainWindow::saveScreen(const QPixmap & pixmap)
{
	static int i = 0;
	static auto filePath = core::getUserDataPath() / "screens";

	if(core::Filesystem::pathExists(filePath) == false){
		core::Filesystem::createDirectory(filePath);
	}

	pixmap.save(QString((filePath / "screen_%1.png").string().c_str()).arg(i++));
}


bool MainWindow::trySetStyleByName( const std::string& styleName )
{
    try {
        setStyleByName(styleName);
    } catch(...) {
        return false;
    }

    return true;
}

CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

Q_DECLARE_METATYPE ( Filesystem::Path );

MainWindow::MainWindow(): QMainWindow(nullptr), updateEnabled(true), pluginLoader(nullptr),
    splashScreen_(new QSplashScreen())
{
    //splashScreen_->setWindowFlags(splashScreen_->windowFlags() | Qt::WindowStaysOnTopHint);
}

QSplashScreen * MainWindow::splashScreen()
{
    return splashScreen_;
}

void MainWindow::showSplashScreenMessage(const QString & message, int alignment, const QColor & color)
{
    splashScreen_->showMessage(message, Qt::AlignLeft | Qt::AlignBottom, Qt::white);
    QCoreApplication::processEvents();
}

void MainWindow::init(PluginLoader* pluginLoader, ExtendedCustomApplication * coreApplication)
{
    instance = this;
	this->pluginLoader = pluginLoader;
    this->coreApplication = coreApplication;

    splashScreen_->setVisible(true);
    QCoreApplication::processEvents();

    showSplashScreenMessage(tr("Registering application core data sources"));

    registerCoreDataSources();

	showSplashScreenMessage(tr("Registering application core domain types"));

	registerCoreDomainTypes();

    showSplashScreenMessage(tr("Registering plugins"));

	Filesystem::Path pluginPath = core::getPluginPath();

	if(Filesystem::pathExists(pluginPath) == true) {
        Filesystem::Iterator endIT;
		std::for_each(Filesystem::Iterator(pluginPath), endIT, [=](Filesystem::Path p) {
			if (Filesystem::isDirectory(p)) {
				pluginLoader->addPath(p.string());
			}
		});
	}
    pluginLoader->load();

	for(int i = 0; i < pluginLoader->getNumPlugins(); ++i)
	{
		unpackPlugin(pluginLoader->getPlugin(i));
	}

	findResources(core::getResourcesPath().string());

	showSplashScreenMessage(tr("Initializing services"));

	auto serviceManager = core::getServiceManager();

	// inicjalizacja usług
	//TODO
	for (int i = 0; i < serviceManager->getNumServices(); ++i) {
		serviceManager->getService(i)->init(coreApplication->sourceManager(),
			coreApplication->dataSourceManager(), coreApplication->dataProcessorManager(),
			nullptr, coreApplication->visualizerManager(),
			getMemoryDataManager(), nullptr);
	}

	// inicjalizacja usług
	for (int i = 0; i < serviceManager->getNumServices(); ++i) {
		serviceManager->getService(i)->lateInit();
	}

	showSplashScreenMessage(tr("Initializing sources"));

	auto sourceManager = coreApplication->sourceManager();

	// inicjalizacja źródeł
	//TODO
	for (int i = 0; i < sourceManager->getNumSources(); ++i) {
		sourceManager->getSource(i)->init(getMemoryDataManager(), nullptr, serviceManager);
	}

    readSettings(QSettings(), true);

    connect(&visualizerTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
    connect(&serviceTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
	visualizerTimer.start(20);
    serviceTimer.start(20);

    splashScreen_->setVisible(false);
    QCoreApplication::processEvents();
}

MainWindow::~MainWindow()
{
	visualizerTimer.stop();
	serviceTimer.stop();
	//żądanie odłączenia się serwisów od widgetów i elementów UI oraz innych serwisów czy zasobów aplikacji
	ServiceManager::getInstance()->finalizeServices();

//    VisualizerManager::getInstance()->setDebugWidget(nullptr);
	VisualizerManager::getInstance()->markAllChannelsAsRemoved();
	VisualizerManager::getInstance()->removeAllChannels();
}

void MainWindow::findResources(const std::string& resourcesPath)
{
    // TODO : potrzebna rewizja tego kodu...
    resourcesPaths.clear();
    //szukaj shaderów
    std::vector<std::string> ext;
    ext.push_back(".frag");
    ext.push_back(".vert");
    //ext.push_back(".avi");
    try {
        std::vector<std::string> temp = Filesystem::listFiles(resourcesPath, true, ext);
        resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
        //szukaj mesh
        temp = Filesystem::listFiles(resourcesPath, true, ".fmesh");
        resourcesPaths.insert(resourcesPaths.end(), temp.begin(), temp.end());
        //szukaj styli qt
        temp = Filesystem::listFiles(resourcesPath, true, ".qss");
        applicationSkinsPaths.insert(applicationSkinsPaths.end(), temp.begin(), temp.end());
    } catch(std::exception& e) {
        CORE_LOG_INFO("Finding resources exception: " << e.what());
    }
}

void MainWindow::openFile( const std::string& path )
{
	CORE_LOG_INFO("Opening file: " << path);
	std::vector<Filesystem::Path> paths;
	paths.push_back(path);
    auto fdmTrans = getFileDataManager()->transaction();
    for(auto it = paths.begin(); it != paths.end(); ++it){
        fdmTrans->tryAddFile(*it);
    }
}

void MainWindow::readSettings( const QSettings& settings, bool readGeometry )
{
    if ( settings.contains("WindowState") ) {
        restoreState(settings.value("WindowState").toByteArray());
    }
    if ( readGeometry && settings.contains("Geometry") ) {
        restoreGeometry(settings.value("Geometry").toByteArray());
    }
}

void MainWindow::writeSettings()
{
    QSettings settings;
    settings.setValue("Geometry", saveGeometry());
    settings.setValue("WindowState", saveState());
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    writeSettings();
    QMainWindow::closeEvent(event);
}

//void MainWindow::InitializeControlWidget()
//{
//    // inicjalizacja GridWidget
//    EDRDockWidget *gDock = new EDRDockWidget(tr("Service scene graph"), this, Qt::WindowTitleHint);
//    gDock->setObjectName(QString("GridWidget"));
//    gDock->setAllowedAreas(Qt::LeftDockWidgetArea);
//
//    widgetSceneGraph = new SceneGraphWidget();
//    addDockWidget(Qt::LeftDockWidgetArea, gDock);
//    //gDock->getInnerWidget()->layout()->addWidget((QWidget*)widgetSceneGraph);
//    gDock->setWidget((QWidget*)widgetSceneGraph);
//    gDock->setPermanent(true);
//}



void MainWindow::initializeConsole()
{
    widgetConsole = new ::EDRConsoleWidget(tr("Console"), this, Qt::WindowTitleHint);
    widgetConsole->setObjectName("Console");
    widgetConsole->setAllowedAreas(Qt::BottomDockWidgetArea);
    widgetConsole->setPermanent(true);
    //addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
}

void MainWindow::updateVisualizers()
{
    if ( updateEnabled ) {
        VisualizerManager::getInstance()->update();
    }
}

void MainWindow::updateServices()
{
    if ( updateEnabled ) {
        ServiceManager::getInstance()->update(0.02);
    }
}

void MainWindow::safeRegisterPlugin(const PluginPtr & plugin)
{

}

void MainWindow::safeRegisterService(const plugin::IServicePtr & service)
{
    try{

        coreApplication->serviceManager()->registerService(service);

    }catch(std::exception & e){
        CORE_LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
            " has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
            " has caused an UNKNOWN error during registration. Service NOT registered in application!" );
    }
}

void MainWindow::safeRegisterSource(const plugin::ISourcePtr & source)
{
	try{

		coreApplication->sourceManager()->registerSource(source);

	}catch(std::exception & e){
		CORE_LOG_WARNING("Source " << source->getName() << " with ID " << source->getID() <<
			" has caused an error during registration: " << e.what() << ". Source NOT registered in application!" );
	}
	catch(...){
		CORE_LOG_WARNING("Source " << source->getName() << " with ID " << source->getID() <<
			" has caused an UNKNOWN error during registration. Source NOT registered in application!" );
	}
}

void MainWindow::safeRegisterParser(const plugin::IParserPtr & parser)
{
    try{

        getParserManager()->registerParser(parser);

    }catch(std::exception & e){
        CORE_LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
            " has caused an UNKNOWN error during registration. Parser NOT registered in application!" );
    }
}

void MainWindow::safeRegisterObjectWrapperPrototype(const ObjectWrapperPtr & prototype)
{
    try{

        getDataHierarchyManager()->registerObjectWrapperPrototype(prototype);

    }catch(std::exception & e){
        CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an error during registration: "
            << e.what() << ". Object type NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("Object wrapper prototype for type " << prototype->getTypeInfo().name() << " has caused an UNKNOWN error during registration. Object type NOT registered in application!" );
    }
}

void MainWindow::safeRegisterVisualizer(const plugin::IVisualizerPtr & visualizer)
{
    try{

        VisualizerManager::getInstance()->registerVisualizer(visualizer);

    }catch(std::exception & e){
        CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
            << " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
            << " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!" );
    }
}

void MainWindow::safeRegisterDataProcessor(const plugin::IDataProcessorPtr & dataProcessor)
{
    try{

        DataProcessorManager::getInstance()->registerDataProcessor(dataProcessor);

    }catch(std::exception & e){
        CORE_LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
            << " has caused an error during registration: " << e.what() << ". DataProcessor NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
            << " has caused an UNKNOWN error during registration. DataProcessor NOT registered in application!" );
    }
}

void MainWindow::safeRegisterDataSource(const plugin::IDataSourcePtr & dataSource)
{
    try{

        DataSourceManager::getInstance()->registerDataSource(dataSource);

    }catch(std::exception & e){
        CORE_LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
            << " has caused an error during registration: " << e.what() << ". DataSource NOT registered in application!" );
    }
    catch(...){
        CORE_LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
            << " has caused an UNKNOWN error during registration. DataSource NOT registered in application!" );
    }
}

void MainWindow::registerCoreDataSources()
{
    safeRegisterDataSource(plugin::IDataSourcePtr(new LocalDataSource()));
}

void MainWindow::registerCoreDomainTypes()
{
	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<int>());
	safeRegisterObjectWrapperPrototype(ObjectWrapper::create<double>());
}

void MainWindow::unpackPlugin(const core::PluginPtr & plugin)
{
	auto message = tr("Loading plugin %1 content: %2").arg(QString::fromStdString(plugin->getName()));
	
	showSplashScreenMessage(message.arg(tr("services")));
	
	for ( int j = 0; j < plugin->getNumServices(); ++j ) {
		safeRegisterService(plugin->getService(j));
	}

	showSplashScreenMessage(message.arg(tr("sources")));

	for ( int j = 0; j < plugin->getNumSources(); ++j ) {
		safeRegisterSource(plugin->getSource(j));
	}

	showSplashScreenMessage(message.arg(tr("parsers")));

	for(int j = 0; j < plugin->getNumParsers(); ++j) {
		safeRegisterParser(plugin->getParser(j));
	}

	showSplashScreenMessage(message.arg(tr("domain objects")));

	for(int j = 0; j < plugin->getNumObjectWrapperPrototypes(); ++j) {
		safeRegisterObjectWrapperPrototype(plugin->getObjectWrapperPrototype(j));
	}

	showSplashScreenMessage(message.arg(tr("visualizers")));

	for(int j = 0; j < plugin->getNumVisualizers(); ++j) {
		safeRegisterVisualizer(plugin->getVisualizer(j));
	}

	showSplashScreenMessage(message.arg(tr("data sources")));

	for(int j = 0; j < plugin->getNumDataSources(); ++j) {
		safeRegisterDataSource(plugin->getDataSource(j));
	}

	showSplashScreenMessage(message.arg(tr("data processors")));

	for(int j = 0; j < plugin->getNumDataProcessors(); ++j) {
		safeRegisterDataProcessor(plugin->getDataProcessor(j));
	}

	showSplashScreenMessage(message.arg(tr("data sinks")));

	//TODO
	//ładowanie sinków

}

QDockWidget* MainWindow::embeddWidget( QWidget* widget, const ActionsGroupManager & widgetActions, const QString& name, const QString& style, const QString& sufix,
    Qt::DockWidgetArea area /*= Qt::AllDockWidgetAreas*/)
{
    // dodajemy widget dokowalny
    EDRDockWidget* dock = new EDRDockWidget( name, this, Qt::WindowTitleHint);
    dock->setAllowedAreas(area);
    dock->setObjectName(name + widget->objectName() + "WIDGET" + sufix);
    dock->setStyleSheet(style);
    dock->setWidget(widget);
    dock->setPermanent(true);
    QObject::connect( dock, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );

    EDRTitleBar * titleBar = supplyWithEDRTitleBar(dock);

    for(auto groupIT = widgetActions.begin(); groupIT != widgetActions.end(); ++groupIT){
        std::map<int, QObject *> allObjects;
        (*groupIT).getAllObjects(allObjects);
        for(auto objectIT = allObjects.begin(); objectIT != allObjects.end(); ++objectIT){
            titleBar->addObject(objectIT->second, plugin::IEDRTitleBar::Left);
        }
    }

    //if(widgetActions.empty() == false){

    //    //EDRTitleBar * titleBar = new EDRTitleBar();
    //    //dock->setTitleBarWidget(titleBar);

    //    EDRTitleBar * titleBar = supplyWithEDRTitleBar(dock);

    //    for(auto it = widgetActions.begin(); it!= widgetActions.end(); it++){
    //        //dock->getTitleBar()->addObject(*it, IEDRTitleBar::Left);
    //        titleBar->addObject(*it, IEDRTitleBar::Left);
    //    }
    //}

    return dock;
}
