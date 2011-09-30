#include "CorePCH.h"
#include <core/TypeInfo.h>
#include <core/ObjectWrapperFactory.h>
#include "MainWindow.h"
#include <osgui/QOsgWidgets.h>
#include "SceneGraphWidget.h"

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

#include <iostream>

#include <utils/Debug.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <boost/regex.hpp>

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

CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
CORE_DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);

Q_DECLARE_METATYPE ( Filesystem::Path );

MainWindow::MainWindow():
QMainWindow(nullptr), updateEnabled(true), pluginLoader(nullptr)
{

}


void MainWindow::init(PluginLoader* pluginLoader) 
{
	this->pluginLoader = pluginLoader;
    //setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);

    DataManager* dataManager = DataManager::getInstance();
    ServiceManager* serviceManager = ServiceManager::getInstance();
    VisualizerManager* visualizerManager = VisualizerManager::getInstance();
	EDRConfig* directoriesInfo = EDRConfig::getInstance();
    DataProcessorManager* dataProcesorManager = DataProcessorManager::getInstance();

    registerCoreServices();

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

	dataManager->findResources(directoriesInfo->getResourcesPath().string());
    dataManager->loadResources();

    // inicjalizacja us³ug
    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
        serviceManager->getService(i)->init();
    }

    // inicjalizacja us³ug
    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
        serviceManager->getService(i)->lateInit();
    }

    initializeConsole();          // Console Widget 
    InitializeControlWidget();          // Control Widget + TimeLine
    visualizerManager->setDebugWidget(widgetSceneGraph);


    readSettings(QSettings(), true);

    connect(&visualizerTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
	visualizerTimer.start(20);
}

MainWindow::~MainWindow()
{
    VisualizerManager::getInstance()->setDebugWidget(nullptr);
}

void MainWindow::openFile( const std::string& path )
{
	LOG_INFO("Opening file: " << path);
	std::vector<Filesystem::Path> paths;
	paths.push_back(path);
	DataManager::getInstance()->loadFiles(paths);
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

void MainWindow::InitializeControlWidget()
{
    // inicjalizacja GridWidget
    EDRDockWidget *gDock = new EDRDockWidget(tr("Service scene graph"), this, Qt::WindowTitleHint);
    gDock->setObjectName(QString("GridWidget"));
    gDock->setAllowedAreas(Qt::LeftDockWidgetArea);

    widgetSceneGraph = new SceneGraphWidget();    
    addDockWidget(Qt::LeftDockWidgetArea, gDock);
    gDock->getInnerWidget()->layout()->addWidget((QWidget*)widgetSceneGraph);
    gDock->setPermanent(true);
}



void MainWindow::initializeConsole()
{
    widgetConsole = new EDRConsoleWidget(tr("Console"), this, Qt::WindowTitleHint);    
    widgetConsole->setObjectName("Console");
    widgetConsole->setAllowedAreas(Qt::BottomDockWidgetArea);
    addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
}

void MainWindow::updateVisualizers()
{
    if ( updateEnabled ) {
        VisualizerManager::getInstance()->update();
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
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
    }
    catch(std::invalid_argument e){
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
    }
    catch(...){
        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
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

void MainWindow::registerCoreServices()
{
    // us³uga UI
//    UserInterfaceService* userInterfaceService = new UserInterfaceService();
 //   userInterfaceService->setMainWindow(this);
 //   safeRegisterService(IServicePtr(userInterfaceService));
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

    //IParserPtr c3dParser = core::shared_ptr<C3DParser>(new C3DParser());

    //safeRegisterParser(c3dParser);

    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<int>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<double>()) );
    /*safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__ScalarChannel>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__C3DAnalogChannel>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__EMGChannel>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__GRFChannel>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__MarkerChannel>()) );
    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__MarkerSet>()) );*/
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

//void MainWindow::onOpen()
//{
//    utils::Push<bool> pushed(updateEnabled, false);
//    Filesystem::Path initPath = getUserDataPath() / "trial";
//    const QString directory = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
//    if (!directory.isEmpty()) 
//    {
//		std::vector<Filesystem::Path> paths;
//        Filesystem::Path dirPath(directory.toStdString());
//        Filesystem::Iterator itEnd;
//
//        for( Filesystem::Iterator it(dirPath); it != itEnd; it++){
//            if(Filesystem::isRegularFile(*it) == true){
//                //openFile((*it).path().string());
//				paths.push_back(it->path());
//            }
//        }        
//
//		DataManager::getInstance()->loadFiles(paths);
//    }
//}
//
//void MainWindow::onExit()
//{
//    close();
//}
//
//void MainWindow::onMaterial()
//{
//    
//}
//
//void MainWindow::onBones()
//{
//
//}
//
//void MainWindow::onWireframe()
//{
//
//}
//
//void MainWindow::createWorkflow()
//{
//    EDRWorkflowWidget* widget = new EDRWorkflowWidget();
//    widget->setAllowedAreas(Qt::AllDockWidgetAreas);
//
//    QObject::connect( widget, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );
//
//    addDockWidget(Qt::LeftDockWidgetArea, widget);
//}


QDockWidget* MainWindow::embeddWidget( QWidget* widget, std::vector<QObject*>& widgetActions, const QString& name, const QString& style, const QString& sufix,
    Qt::DockWidgetArea area /*= Qt::AllDockWidgetAreas*/)
{
    // dodajemy widget dokowalny     
    EDRDockWidget* dock = new EDRDockWidget( name, this, Qt::WindowTitleHint);        
    dock->setAllowedAreas(area);
    dock->setObjectName(name + widget->objectName() + "WIDGET" + sufix);
    dock->setStyleSheet(style);
    //dock->setWidget(widget);
    dock->getInnerWidget()->layoutContent->addWidget(widget);
    QObject::connect( dock, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );

    for(auto it = widgetActions.begin(); it!= widgetActions.end(); it++){
        dock->getTitleBar()->addObject(*it, IEDRTitleBar::Left);
    }

    return dock;
}


//void MainWindow::onCustomAction()
//{
//    QObject* obj = QObject::sender();
//    std::string path = toStdString(obj->objectName());
//    this->triggerMenuItem(path, false);
//}
//
//void MainWindow::onCustomAction( bool triggered )
//{
//    QObject* obj = QObject::sender();
//    std::string path = toStdString(obj->objectName());
//    this->triggerMenuItem(path, triggered);
//}
//
//void MainWindow::onRemoveMenuItem( const std::string& path )
//{
//    // TODO: rekurencyjne usuwanie niepotrzebnych podmenu
//    QAction* action = findChild<QAction*>(toQString(path));
//    if ( action ) 
//    {
//        delete action;
//    }
//}
//
//
//
//void MainWindow::onAddMenuItem( const std::string& path, bool checkable, bool initialState )
//{
//    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
//    tokenizer tokens(path, boost::char_separator<char>("/"));
//    tokenizer::iterator next;
//    tokenizer::iterator token = tokens.begin();
//
//    QWidget* currentMenu = QMainWindow::menuBar();
//    std::string pathPart;
//
//    for (token = tokens.begin(); token != tokens.end(); token = next ) 
//    {        
//        // ustawiamy nastêpny (przyda siê)
//        next = token;
//        ++next;
//
//        // aktualizacja bie¿¹cej œcie¿ki
//        if ( !pathPart.empty() ) 
//        {
//            pathPart.append("/");
//        }
//        pathPart += *token;
//
//        // wyszukanie dziecka
//        QString itemName = toQString(pathPart);
//
//        if ( next == tokens.end() ) 
//        {
//            // liœæ
//            QAction* action = new QAction(this);
//            action->setObjectName( toQString(pathPart) );
//            action->setText(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
//            currentMenu->addAction(action);
//            if ( checkable ) 
//            {
//                action->setCheckable(true);
//                action->setChecked(initialState);
//                QObject::connect(action, SIGNAL(toggled(bool)), this , SLOT(onCustomAction(bool)));
//            } 
//            else 
//            {
//                QObject::connect(action, SIGNAL(triggered()), this , SLOT(onCustomAction()));
//            }
//        } 
//        else 
//        {
//            if ( QMenu* menu = currentMenu->findChild<QMenu*>(itemName) )
//            {
//                // menu ju¿ istnieje
//                currentMenu = menu;
//            }
//            else 
//            {
//                // ga³¹Ÿ
//                menu = new QMenu(currentMenu);
//                menu->setObjectName( itemName );
//                menu->setTitle(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
//                currentMenu->addAction( menu->menuAction() );
//                currentMenu = menu;
//            }
//        }
//    }
//}
//
//
//void MainWindow::openFile( const std::string& path )
//{
//    LOG_INFO("Opening file: " << path);
//    std::vector<Filesystem::Path> paths;
//    paths.push_back(path);
//    DataManager::getInstance()->loadFiles(paths);
//}
//
//void MainWindow::loadData()
//{
//	ServiceManager::getInstance()->loadDataPass(DataManager::getInstance());
//}

//void MainWindow::onDockWidgetVisiblityChanged( bool visible )
//{
//
//}
//
//void MainWindow::onSaveLayout()
//{
//    // TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
//    Filesystem::Path dir = getPathInterface()->getUserDataPath() / "layouts";
//
//    //jesli nie istnieje spróbuj utworzyæ
//    if(Filesystem::pathExists(dir) == false) {
//        Filesystem::createDirectory(dir);
//    }
//
//    if(Filesystem::pathExists(dir) == true) {        
//        const QString fileName = QFileDialog::getSaveFileName(this, 0, dir.string().c_str(), "*.layout");
//        if ( !fileName.isEmpty() ) {
//            QSettings settings(fileName, QSettings::IniFormat);
//            settings.setValue("Geometry", saveGeometry());
//            settings.setValue("WindowState", saveState());
//        }
//    } else {
//        LOG_ERROR("Could not create directory: "<<toStdString(dir));
//    }
//}

//void MainWindow::openLayout( const QString& path )
//{
//    readSettings(QSettings(path, QSettings::IniFormat), false);
//}
//
//void MainWindow::onOpenLayout()
//{
//    // TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
//    Filesystem::Path dir = getPathInterface()->getUserDataPath() / "layouts";
//    utils::Push<bool> pushed(updateEnabled, false);
//    const QString fileName = QFileDialog::getOpenFileName(this, 0, dir.string().c_str(), "*.layout");
//    if ( !fileName.isEmpty() ) {
//        openLayout(fileName);
//    }
//}
//
//void MainWindow::onShowSavedLayouts()
//{
//    // usuniêcie starych akcji
//    menuLoad_layout->clear();
//    menuLoad_layout->addAction(actionLayoutOpen);
//
//    // layouty wbudowane
//    Filesystem::Path dir = getPathInterface()->getResourcesPath() / "layouts";
//    addLayoutsToMenu(dir);
//    // layouty zdefiniowane przez u¿ytkownika
//    dir = getPathInterface()->getUserDataPath() / "layouts";
//    addLayoutsToMenu(dir);
//}
//
//
//
//void MainWindow::onLayoutTriggered()
//{
//    if ( QAction* action = qobject_cast<QAction*>(QObject::sender()) ) {
//        openLayout( action->data().value<Filesystem::Path>().string().c_str() );
//    }
//}

//void MainWindow::addLayoutsToMenu( const Filesystem::Path &dir )
//{
//    if ( Filesystem::pathExists(dir) == true ) {     
//        std::vector<std::string> files = Filesystem::listFiles(dir, false, ".layout");
//        if ( files.empty() == false ) {
//            menuLoad_layout->addSeparator();
//            BOOST_FOREACH(const std::string& file, files) {
//                QAction* action = new QAction(menuLoad_layout);
//                action->setText(file.c_str());
//                QVariant v;
//                v.setValue(dir / file);
//                action->setData(v);
//                menuLoad_layout->addAction(action);
//                QObject::connect(action, SIGNAL(triggered()), this, SLOT(onLayoutTriggered()));
//            }
//        }
//    }
//}

//void MainWindow::populateWindowMenu()
//{
//    // uwaga: nie musimy usuwaæ starych akcji, poniewa¿ QMenu pilnuje,
//    // aby nie by³ dodane dwie jednakowe instancje
//    populateWindowMenu(menuWindow);
//}
//
//void MainWindow::populateVisualizersMenu()
//{
//    // czyœcimy menu
//    menuCreateVisualizer->clear();
//    populateVisualizersMenu(menuCreateVisualizer);
//}

//void MainWindow::actionCreateVisualizer()
//{
//    QAction* action = qobject_cast<QAction*>(sender());
//    VisualizerWidget* widget = new VisualizerWidget(action->data().value<UniqueID>(), this, Qt::WindowTitleHint);
//    widget->setAllowedAreas(Qt::RightDockWidgetArea);
//    widget->setStyleSheet(styleSheet());
//    addDockWidget(Qt::RightDockWidgetArea, widget);
//}
//
//void MainWindow::visualizerWidgetClosed( VisualizerWidget* widget )
//{
//
//}
//
//
//void MainWindow::populateVisualizersMenu( QMenu* menu )
//{
//    std::vector<QAction*> sortedActions;
//    // dodajemy wizualizatory
//    BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
//        QAction* action = new QAction(toQString(vis->getName()), menu);
//        action->setData( qVariantFromValue(vis->getID()) );
//        action->setIcon( VisualizerManager::getInstance()->getIcon(vis->getID()) );
//        action->connect( action, SIGNAL(triggered()), this, SLOT(actionCreateVisualizer()) );
//        sortedActions.push_back(action);
//    }
//    std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
//    BOOST_FOREACH(QAction* action, sortedActions) {
//        menu->addAction(action);
//    }
//}
//
//void MainWindow::populateWindowMenu( QMenu* menu )
//{
//    std::vector<QAction*> sortedActions;
//
//    QList<QDockWidget*> dockwidgets = qFindChildren<QDockWidget*>(this);
//    if ( dockwidgets.size() ) {
//        // pobranie i posortowanie akcji wg nazw
//        sortedActions.reserve(dockwidgets.size());
//        BOOST_FOREACH(QDockWidget* dockWidget, dockwidgets) {
//            sortedActions.push_back(dockWidget->toggleViewAction());
//        }
//        std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
//        BOOST_FOREACH(QAction* action, sortedActions) {
//            menu->addAction(action);
//        }
//        menu->addSeparator();
//        sortedActions.resize(0);
//    }
//
//    QList<QToolBar*> toolbars = qFindChildren<QToolBar*>(this);
//    if (toolbars.size()) {
//        sortedActions.reserve(toolbars.size());
//        BOOST_FOREACH(QToolBar* toolbar, toolbars) {
//            sortedActions.push_back(toolbar->toggleViewAction());
//        }
//        std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
//        BOOST_FOREACH(QAction* action, sortedActions) {
//            menu->addAction(action);
//        }
//    }
//}
//
//void MainWindow::refreshVisualizerWidgetsNames()
//{
//}


//#ifdef UTILS_DEBUG
//
//void MainWindow::onTestItemClicked(const std::string& sender, bool state)
//{
//    if (removeOnClick) {
//        removeMenuItem(sender);
//    }
//}
//
//void MainWindow::onTestRemoveToggled(const std::string& sender, bool state )
//{
//    removeOnClick = state;
//}
//
//
//
//#endif // UTILS_DEBUG