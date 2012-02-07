#include "CorePCH.h"
#include <core/TypeInfo.h>
#include <core/ObjectWrapperFactory.h>
#include "MainWindow.h"
#include <osgui/QOsgWidgets.h>

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>

#include <osgViewer/GraphicsWindow>
#include <osgText/Text>
#include <osg/ShapeDrawable>
#include <osgViewer/Scene>
#include <iostream>
#include <osgGA/TerrainManipulator>

#include "EDRConsoleWidget.h"

#include "ServiceManager.h"
#include "DataProcessorManager.h"
#include "DataSourceManager.h"
#include "UserInterfaceService.h"
#include "EDRConfig.h"
#include "EDRTitleBar.h"

#include <iostream>

#include <utils/Debug.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>

#include "Log.h"
#include <core/StringTools.h>
#include <boost/foreach.hpp>
#include "VisualizerWidget.h"

#include <core/Visualizer.h>
#include <osgWidget/ViewerEventHandlers>

#include <osgui/EventCallback.h>
#include <osg/BlendFunc>
#include <osg/LineWidth>

#include <boost/random.hpp>

#include <utils/Push.h>
#include "DataProcessor.h"

#include <core/EDRDockWidget.h>

#include "WorkflowService.h"
#include "WorkflowWidget.h"
#include "LocalDataSource.h"

DEFINE_DEFAULT_LOGGER("edr.core");

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
    for (int i = 0; i < count; i++) {
        const core::Filesystem::Path& path = getApplicationSkinsFilePath(i);
        if (path.stem() == styleName) {
            setStyle(path);
            return;
        }
    }

    throw std::runtime_error(std::string("Unable to set style : ") + styleName);
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

MainWindow::MainWindow():
QMainWindow(nullptr), updateEnabled(true), pluginLoader(nullptr)//, widgetSceneGraph(nullptr)//, currentContextWidget(nullptr)
{

}


void MainWindow::init(PluginLoader* pluginLoader, IManagersAccessor * managersAccessor) 
{
    instance = this;
	this->pluginLoader = pluginLoader;
    this->managersAccessor = managersAccessor;
    //setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    DataManager* dataManager = DataManager::getInstance();
    ServiceManager* serviceManager = ServiceManager::getInstance();
    VisualizerManager* visualizerManager = VisualizerManager::getInstance();
	EDRConfig* directoriesInfo = EDRConfig::getInstance();
    DataProcessorManager* dataProcesorManager = DataProcessorManager::getInstance();

    registerCoreDataSources();

	Filesystem::Path pluginPath = getApplicationDataPath() / "plugins";
	
	if(Filesystem::pathExists(pluginPath) == true) {
        Filesystem::Iterator endIT;
		std::for_each(Filesystem::Iterator(pluginPath), endIT, [=](Filesystem::Path p) {
			if (Filesystem::isDirectory(p)) {
				pluginLoader->addPath(p.string());
			}
		});
	}
    pluginLoader->load();

    registerPluginsWrapperFactories();

    registerPluginsServices();
	registerPluginsParsers();
    registerPluginsVisualizers();
    registerPluginsDataProcessors();
    registerPluginsDataSources();

	findResources(directoriesInfo->getResourcesPath().string());

    // inicjalizacja us³ug
    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
        serviceManager->getService(i)->init(managersAccessor);
    }

    // inicjalizacja us³ug
    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
        serviceManager->getService(i)->lateInit();
    }

    //initializeConsole();          // Console Widget 
    //InitializeControlWidget();          // Control Widget + TimeLine
    //visualizerManager->setDebugWidget(widgetSceneGraph);


    readSettings(QSettings(), true);

    connect(&visualizerTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
    connect(&serviceTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
	visualizerTimer.start(20);
    serviceTimer.start(20);
}

MainWindow::~MainWindow()
{
//    VisualizerManager::getInstance()->setDebugWidget(nullptr);
}

void MainWindow::findResources(const std::string& resourcesPath)
{
    // TODO : potrzebna rewizja tego kodu...
    resourcesPaths.clear();
    //szukaj shaderow
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
        LOG_INFO("Finding resources exception: " << e.what());
    }
}

void MainWindow::openFile( const std::string& path )
{
	LOG_INFO("Opening file: " << path);
	std::vector<Filesystem::Path> paths;
	paths.push_back(path);
    DataManager * dataManager = DataManager::getInstance();
    for(auto it = paths.begin(); it != paths.end(); it++){
        try{
            dataManager->addFile(*it);
        }catch(...){

        }
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
    //¿¹danie od³¹czenia siê serwisów od widgetów i elementów UI oraz innych serwisów czy zasobów aplikacji
    ServiceManager::getInstance()->finalizeServices();
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
    widgetConsole = new EDRConsoleWidget(tr("Console"), this, Qt::WindowTitleHint);    
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

void MainWindow::safeRegisterService(const IServicePtr & service)
{
    try{

        ServiceManager::getInstance()->registerService(service);

    }catch(std::runtime_error e){
        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
            " has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
            " has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
            " has caused an UNKNOWN error during registration. Service NOT registered in application!" );
    }
}

void MainWindow::safeRegisterParser(const IParserPtr & parser)
{
    try{

        DataManager::getInstance()->registerParser(parser);

    }catch(std::runtime_error e){ 
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() <<
            " has caused an UNKNOWN error during registration. Parser NOT registered in application!" );
    }
}

void MainWindow::safeRegisterObjectFactory(const IObjectWrapperFactoryPtr & factory)
{
    try{

        DataManager::getInstance()->registerObjectFactory(factory);

    }catch(std::runtime_error e){ 
        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an error during registration: "
            << e.what() << ". Object type NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an error during registration: "
            << e.what() << ". Object type NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an UNKNOWN error during registration. Object type NOT registered in application!" );
    }
}

void MainWindow::safeRegisterVisualizer(const IVisualizerPtr & visualizer)
{
    try{

        VisualizerManager::getInstance()->registerVisualizer(visualizer);

    }catch(std::runtime_error e){ 
        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
            << " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
            << " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
            << " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!" );
    }
}

void MainWindow::safeRegisterDataProcessor(const IDataProcessorPtr & dataProcessor)
{
    try{

        DataProcessorManager::getInstance()->registerDataProcessor(dataProcessor);

    }catch(std::runtime_error e){ 
        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
            << " has caused an error during registration: " << e.what() << ". DataProcessor NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
            << " has caused an error during registration: " << e.what() << ". DataProcessor NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
            << " has caused an UNKNOWN error during registration. DataProcessor NOT registered in application!" );
    }
}

void MainWindow::safeRegisterDataSource(const IDataSourcePtr & dataSource)
{
    try{

        DataSourceManager::getInstance()->registerDataSource(dataSource);

    }catch(std::runtime_error e){ 
        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
            << " has caused an error during registration: " << e.what() << ". DataSource NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
            << " has caused an error during registration: " << e.what() << ". DataSource NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
            << " has caused an UNKNOWN error during registration. DataSource NOT registered in application!" );
    }
}

void MainWindow::registerCoreDataSources()
{
    DataSourceManager::getInstance()->registerDataSource(IDataSourcePtr(new LocalDataSource()));
}

void MainWindow::registerPluginsServices()
{
    for ( int i = 0; i < pluginLoader->getNumPlugins(); ++i ) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for ( int j = 0; j < plugin->getNumServices(); ++j ) {
            safeRegisterService(plugin->getService(j));
        }
    }
}

void MainWindow::registerPluginsParsers()
{
    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for(int j = 0; j < plugin->getNumParsers(); ++j) {
            safeRegisterParser(plugin->getParser(j));
        }
    }

    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<int>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<double>()) );
}

void MainWindow::registerPluginsWrapperFactories()
{
    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for(int j = 0; j < plugin->getNumWrapperFactories(); ++j) {
            safeRegisterObjectFactory(plugin->getWrapperFactory(j));
        }
    }
}

void MainWindow::registerPluginsVisualizers()
{
    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for(int j = 0; j < plugin->getNumVisualizers(); ++j) {
            safeRegisterVisualizer(plugin->getVisualizer(j));
        }
    }
}

void MainWindow::registerPluginsDataProcessors()
{
    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for(int j = 0; j < plugin->getNumDataProcessors(); ++j) {
            safeRegisterDataProcessor(plugin->getDataProcessor(j));
        }
    }
}

void MainWindow::registerPluginsDataSources()
{
    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
        PluginPtr plugin = pluginLoader->getPlugin(i);
        for(int j = 0; j < plugin->getNumDataSources(); ++j) {
            safeRegisterDataSource(plugin->getDataSource(j));
        }
    }
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

    for(auto groupIT = widgetActions.begin(); groupIT != widgetActions.end(); groupIT++){
        std::map<int, QObject *> allObjects;
        (*groupIT).getAllObjects(allObjects);
        for(auto objectIT = allObjects.begin(); objectIT != allObjects.end(); objectIT++){
            titleBar->addObject(objectIT->second, IEDRTitleBar::Left);
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