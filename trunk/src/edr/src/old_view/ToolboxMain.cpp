#include "CorePCH.h"
#include "ToolboxMain.h"
#include <utils/Push.h>
#include <core/TypeInfo.h>
#include <core/ObjectWrapperFactory.h>
#include "MainWindow.h"
//#include "ui_toolboxmaindeffile.h"
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
//#include <core/C3DParserEx.h>
#include "UserInterfaceService.h"
//#include "config/ConfigurationFileService.h"
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
//#include <core/C3DChannels.h>

//#include <core/Chart.h>
#include <osgWidget/ViewerEventHandlers>

#include <osgui/EventCallback.h>
#include <osg/BlendFunc>
#include <osg/LineWidth>

#include <boost/random.hpp>

#include <utils/Push.h>
#include "DataProcessor.h"

#include <core/EDRDockWidget.h>

//#include <plugins/c3d/C3DChannels.h>

#include "WorkflowService.h"
#include "WorkflowWidget.h"
#include "LocalDataSource.h"

using namespace core;

struct SortActionsByNames 
{
	inline bool operator()(const QAction* a1, const QAction* a2)
	{
		return a1->text() < a2->text();
	}
};

void ToolboxMain::init( core::PluginLoader* pluginLoader )
{
	MainWindow::init(pluginLoader);
	initializeUI();
	setupUi(this);
	connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));
	/*connect(&getUpdateTimer(), SIGNAL(timeout()), this, SLOT(updateServices()));
	connect(&getUpdateTimer(), SIGNAL(timeout()), this, SLOT(updateVisualizers()));

	getUpdateTimer().start(20);*/

	if ( getNumViews() ) {
		connect( &viewerFrameTimer, SIGNAL(timeout()), this, SLOT(update()) );
		viewerFrameTimer.start( 20 );
	}


	populateVisualizersMenu(menuCreateVisualizer);
}

void ToolboxMain::initializeUI()
{
	//ladowanie styli qt
	QString style;
	if(DataManager::getInstance()->getApplicationSkinsFilePathCount() > 0)
	{
		//style qt
		QFile file(QString::fromAscii(DataManager::getInstance()->getApplicationSkinsFilePath(0).c_str(), DataManager::getInstance()->getApplicationSkinsFilePath(0).size()));
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			style = file.readAll();
			file.close();
		}
	}
	setStyleSheet(style);

	setDockOptions( AllowNestedDocks | AllowTabbedDocks );
	setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
	setCentralWidget( nullptr );
	setDocumentMode(true);

	// pozosta³e widgety "p³ywaj¹ce"
	for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		IServicePtr service = ServiceManager::getInstance()->getService(i);

		// HACK
		std::vector<QObject*> mainWidgetActions;
		QWidget* viewWidget = service->getWidget(mainWidgetActions);

		std::vector<QObject*> controlWidgetActions;
		QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

		std::vector<QObject*> settingsWidgetActions;
		QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

		if ( viewWidget ) {

			addDockWidget(Qt::RightDockWidgetArea, embeddWidget(
				viewWidget, 
				mainWidgetActions,
				toQString(service->getName()), 
				style,
				"",
				Qt::RightDockWidgetArea));
		}
		if ( controlWidget ) {
			addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(
				controlWidget, 
				controlWidgetActions,
				toQString(service->getName() + " control"), 
				style,
				"Control",
				Qt::BottomDockWidgetArea));
		}
		if ( settingsWidget ) {
			addDockWidget(Qt::LeftDockWidgetArea, embeddWidget(
				settingsWidget, 
				settingsWidgetActions,
				toQString(service->getName() + " settings"), 
				style,
				"Settings",
				Qt::LeftDockWidgetArea));
		}
	}

	//#ifdef UTILS_DEBUG
	//	// testowe opcje
	//	removeOnClick = false;
	//	onTestItemClickedPtr.reset(new Window::ItemPressed(boost::bind(&MainWindow::onTestItemClicked, this, _1, _2 )));
	//	onTestRemoveToggledPtr.reset(new Window::ItemPressed(boost::bind(&MainWindow::onTestRemoveToggled, this, _1, _2 )));
	//	core::shared_ptr<IUserInterface> userInterfaceService = queryServices<IUserInterface>(ServiceManager::getInstance());
	//	userInterfaceService->getMainWindow()->addMenuItem("Callback test/Option", onTestItemClickedPtr);
	//	userInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested/Option", onTestItemClickedPtr);
	//	userInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested/Option2", onTestItemClickedPtr);
	//	userInterfaceService->getMainWindow()->addMenuItem("Callback test/Nested2/Option", onTestItemClickedPtr);
	//	userInterfaceService->getMainWindow()->addMenuItem("Callback test/Remove on click?", onTestRemoveToggledPtr, true, removeOnClick);
	//#endif // UTILS_DEBUG
}


void ToolboxMain::onOpen()
{
	utils::Push<bool> pushed(updateEnabled, false);
	Filesystem::Path initPath = getUserDataPath() / "trial";
	const QString directory = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
	if (!directory.isEmpty()) 
	{
		std::vector<Filesystem::Path> paths;
		Filesystem::Path dirPath(directory.toStdString());
		Filesystem::Iterator itEnd;

		for( Filesystem::Iterator it(dirPath); it != itEnd; it++){
			if(Filesystem::isRegularFile(*it) == true){
				//openFile((*it).path().string());
				paths.push_back(it->path());
			}
		}        

		DataManager::getInstance()->loadFiles(paths);
	}
}

void ToolboxMain::onExit()
{
	close();
}

void ToolboxMain::onMaterial()
{

}

void ToolboxMain::onBones()
{

}

void ToolboxMain::onWireframe()
{

}

void ToolboxMain::createWorkflow()
{
	EDRWorkflowWidget* widget = new EDRWorkflowWidget();
	widget->setAllowedAreas(Qt::AllDockWidgetAreas);

	QObject::connect( widget, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );

	addDockWidget(Qt::LeftDockWidgetArea, widget);
}

void ToolboxMain::onCustomAction()
{
	QObject* obj = QObject::sender();
	std::string path = toStdString(obj->objectName());
	this->triggerMenuItem(path, false);
}

void ToolboxMain::onCustomAction( bool triggered )
{
	QObject* obj = QObject::sender();
	std::string path = toStdString(obj->objectName());
	this->triggerMenuItem(path, triggered);
}

void ToolboxMain::onRemoveMenuItem( const std::string& path )
{
	// TODO: rekurencyjne usuwanie niepotrzebnych podmenu
	QAction* action = findChild<QAction*>(toQString(path));
	if ( action ) 
	{
		delete action;
	}
}



void ToolboxMain::onAddMenuItem( const std::string& path, bool checkable, bool initialState )
{
	//typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
	//tokenizer tokens(path, boost::char_separator<char>("/"));
	//tokenizer::iterator next;
	//tokenizer::iterator token = tokens.begin();

	//// rewizja
	////QWidget* currentMenu = QToolboxMain::menuBar();
	//std::string pathPart;

	//for (token = tokens.begin(); token != tokens.end(); token = next ) 
	//{        
	//	// ustawiamy nastêpny (przyda siê)
	//	next = token;
	//	++next;

	//	// aktualizacja bie¿¹cej œcie¿ki
	//	if ( !pathPart.empty() ) 
	//	{
	//		pathPart.append("/");
	//	}
	//	pathPart += *token;

	//	// wyszukanie dziecka
	//	QString itemName = toQString(pathPart);

	//	if ( next == tokens.end() ) 
	//	{
	//		// liœæ
	//		QAction* action = new QAction(this);
	//		action->setObjectName( toQString(pathPart) );
	//		action->setText(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
	//		currentMenu->addAction(action);
	//		if ( checkable ) 
	//		{
	//			action->setCheckable(true);
	//			action->setChecked(initialState);
	//			QObject::connect(action, SIGNAL(toggled(bool)), this , SLOT(onCustomAction(bool)));
	//		} 
	//		else 
	//		{
	//			QObject::connect(action, SIGNAL(triggered()), this , SLOT(onCustomAction()));
	//		}
	//	} 
	//	else 
	//	{
	//		if ( QMenu* menu = currentMenu->findChild<QMenu*>(itemName) )
	//		{
	//			// menu ju¿ istnieje
	//			currentMenu = menu;
	//		}
	//		else 
	//		{
	//			// ga³¹Ÿ
	//			menu = new QMenu(currentMenu);
	//			menu->setObjectName( itemName );
	//			menu->setTitle(QApplication::translate("ToolboxMain", token->c_str(), 0, QApplication::UnicodeUTF8));
	//			currentMenu->addAction( menu->menuAction() );
	//			currentMenu = menu;
	//		}
	//	}
	//}
}






void ToolboxMain::onDockWidgetVisiblityChanged( bool visible )
{

}
void ToolboxMain::openLayout( const QString& path )
{
	readSettings(QSettings(path, QSettings::IniFormat), false);
}

void ToolboxMain::onOpenLayout()
{
	// TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
	Filesystem::Path dir = getPathInterface()->getUserDataPath() / "layouts";
	utils::Push<bool> pushed(updateEnabled, false);
	const QString fileName = QFileDialog::getOpenFileName(this, 0, dir.string().c_str(), "*.layout");
	if ( !fileName.isEmpty() ) {
		openLayout(fileName);
	}
}

void ToolboxMain::onShowSavedLayouts()
{
	// usuniêcie starych akcji
	menuLoad_layout->clear();
	menuLoad_layout->addAction(actionLayoutOpen);

	// layouty wbudowane
	Filesystem::Path dir = getPathInterface()->getResourcesPath() / "layouts";
	addLayoutsToMenu(dir);
	// layouty zdefiniowane przez u¿ytkownika
	dir = getPathInterface()->getUserDataPath() / "layouts";
	addLayoutsToMenu(dir);
}

void ToolboxMain::addLayoutsToMenu( const Filesystem::Path &dir )
{
	// rewizja
	/*if ( Filesystem::pathExists(dir) == true ) {     
		std::vector<std::string> files = Filesystem::listFiles(dir, false, ".layout");
		if ( files.empty() == false ) {
			menuLoad_layout->addSeparator();
			BOOST_FOREACH(const std::string& file, files) {
				QAction* action = new QAction(menuLoad_layout);
				action->setText(file.c_str());
				QVariant v;
				v.setValue(dir / file);
				action->setData(v);
				menuLoad_layout->addAction(action);
				QObject::connect(action, SIGNAL(triggered()), this, SLOT(onLayoutTriggered()));
			}
		}
	}*/
}

void ToolboxMain::populateWindowMenu()
{
	// uwaga: nie musimy usuwaæ starych akcji, poniewa¿ QMenu pilnuje,
	// aby nie by³ dodane dwie jednakowe instancje
	populateWindowMenu(menuWindow);
}

void ToolboxMain::populateVisualizersMenu()
{
	// czyœcimy menu
	menuCreateVisualizer->clear();
	populateVisualizersMenu(menuCreateVisualizer);
}
void ToolboxMain::actionCreateVisualizer()
{
	QAction* action = qobject_cast<QAction*>(sender());
	VisualizerWidget* widget = new VisualizerWidget(action->data().value<UniqueID>(), this, Qt::WindowTitleHint);
	widget->setAllowedAreas(Qt::RightDockWidgetArea);
	widget->setStyleSheet(styleSheet());
	addDockWidget(Qt::RightDockWidgetArea, widget);
}

void ToolboxMain::visualizerWidgetClosed( VisualizerWidget* widget )
{

}


void ToolboxMain::populateVisualizersMenu( QMenu* menu )
{
	std::vector<QAction*> sortedActions;
	// dodajemy wizualizatory
	BOOST_FOREACH(const IVisualizerConstPtr& vis, VisualizerManager::getInstance()->enumPrototypes()) {
		QAction* action = new QAction(toQString(vis->getName()), menu);
		action->setData( qVariantFromValue(vis->getID()) );
		action->setIcon( VisualizerManager::getInstance()->getIcon(vis->getID()) );
		action->connect( action, SIGNAL(triggered()), this, SLOT(actionCreateVisualizer()) );
		sortedActions.push_back(action);
	}
	std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
	BOOST_FOREACH(QAction* action, sortedActions) {
		menu->addAction(action);
	}
}

void ToolboxMain::populateWindowMenu( QMenu* menu )
{
	std::vector<QAction*> sortedActions;

	QList<QDockWidget*> dockwidgets = qFindChildren<QDockWidget*>(this);
	if ( dockwidgets.size() ) {
		// pobranie i posortowanie akcji wg nazw
		sortedActions.reserve(dockwidgets.size());
		BOOST_FOREACH(QDockWidget* dockWidget, dockwidgets) {
			sortedActions.push_back(dockWidget->toggleViewAction());
		}
		std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
		BOOST_FOREACH(QAction* action, sortedActions) {
			menu->addAction(action);
		}
		menu->addSeparator();
		sortedActions.resize(0);
	}

	QList<QToolBar*> toolbars = qFindChildren<QToolBar*>(this);
	if (toolbars.size()) {
		sortedActions.reserve(toolbars.size());
		BOOST_FOREACH(QToolBar* toolbar, toolbars) {
			sortedActions.push_back(toolbar->toggleViewAction());
		}
		std::sort(sortedActions.begin(), sortedActions.end(), SortActionsByNames());
		BOOST_FOREACH(QAction* action, sortedActions) {
			menu->addAction(action);
		}
	}
}

void ToolboxMain::refreshVisualizerWidgetsNames()
{
}


void ToolboxMain::onLayoutTriggered()
{
	// rewizja
	/*if ( QAction* action = qobject_cast<QAction*>(QObject::sender()) ) {
		openLayout( action->data().value<Filesystem::Path>().string().c_str() );
	}*/
}
void ToolboxMain::onSaveLayout()
{
	// TODO: czy na pewno ma wychodziæ gdy nie uda siê sprawdziæ, czy katalog istnieje?
	Filesystem::Path dir = getPathInterface()->getUserDataPath() / "layouts";

	//jesli nie istnieje spróbuj utworzyæ
	if(Filesystem::pathExists(dir) == false) {
		Filesystem::createDirectory(dir);
	}

	if(Filesystem::pathExists(dir) == true) {        
		const QString fileName = QFileDialog::getSaveFileName(this, 0, dir.string().c_str(), "*.layout");
		if ( !fileName.isEmpty() ) {
			QSettings settings(fileName, QSettings::IniFormat);
			settings.setValue("Geometry", saveGeometry());
			settings.setValue("WindowState", saveState());
		}
	} else {
		LOG_ERROR("Could not create directory: "<<toStdString(dir));
	}
}

#ifdef UTILS_DEBUG

void ToolboxMain::onTestItemClicked(const std::string& sender, bool state)
{
	if (removeOnClick) {
		removeMenuItem(sender);
	}
}

void ToolboxMain::onTestRemoveToggled(const std::string& sender, bool state )
{
	removeOnClick = state;
}



#endif // UTILS_DEBUG

//#include "CorePCH.h"
//
//#include "ToolboxMain.h"
//#include "ui_toolboxmaindeffile.h"
//#include <osgui/QOsgWidgets.h>
//#include "SceneGraphWidget.h"
//
//#include <osg/Vec3d>
//#include <osg/Quat>
//#include <osg/PositionAttitudeTransform>
//
//#include <osgViewer/GraphicsWindow>
//#include <osgText/Text>
//#include <osg/ShapeDrawable>
//#include <osgViewer/Scene>
//#include <iostream>
//#include <osgGA/TerrainManipulator>
//
//#include "EDRConsoleWidget.h"
//
//#include "ServiceManager.h"
//#include "DataProcessorManager.h"
//#include "DataSourceManager.h"
////#include <core/C3DParserEx.h>
//#include "UserInterfaceService.h"
////#include "config/ConfigurationFileService.h"
//#include "EDRConfig.h"
//
//#include <iostream>
//
//#include <utils/Debug.h>
//
//#include <boost/tokenizer.hpp>
//#include <boost/bind.hpp>
//#include <functional>
//#include <boost/regex.hpp>
//
//#include "Log.h"
//#include <core/StringTools.h>
//#include <boost/foreach.hpp>
//#include "VisualizerWidget.h"
//
//#include <core/Visualizer.h>
////#include <core/C3DChannels.h>
//
////#include <core/Chart.h>
//#include <osgWidget/ViewerEventHandlers>
//
//#include <osgui/EventCallback.h>
//#include <osg/BlendFunc>
//#include <osg/LineWidth>
//
//#include <boost/random.hpp>
//
//#include <utils/Push.h>
//#include "DataProcessor.h"
//
//#include <core/EDRDockWidget.h>
//
////#include <plugins/c3d/C3DChannels.h>
//
//#include "WorkflowService.h"
//#include "WorkflowWidget.h"
//#include "LocalDataSource.h"
//
//DEFINE_DEFAULT_LOGGER("edr.core");
//
//struct SortActionsByNames 
//{
//    inline bool operator()(const QAction* a1, const QAction* a2)
//    {
//        return a1->text() < a2->text();
//    }
//};
//
//using namespace core;
//
//CORE_DEFINE_WRAPPER(int, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
//CORE_DEFINE_WRAPPER(double, utils::PtrPolicyBoost, utils::ClonePolicyCopyConstructor);
//
//Q_DECLARE_METATYPE ( Filesystem::Path );
//
//
//ToolboxMain::ToolboxMain(PluginLoader* pluginLoader) :
//QToolboxMain(nullptr), updateEnabled(true), pluginLoader(pluginLoader)
//{
//    setupUi(this);
//    setThreadingModel(osgViewer::CompositeViewer::SingleThreaded);
//
//    DataManager* dataManager = DataManager::getInstance();
//    ServiceManager* serviceManager = ServiceManager::getInstance();
//    VisualizerManager* visualizerManager = VisualizerManager::getInstance();
//	EDRConfig* directoriesInfo = EDRConfig::getInstance();
//    DataProcessorManager* dataProcesorManager = DataProcessorManager::getInstance();
//
//    registerCoreServices();
//
//    registerCoreDataSources();
//
//	Filesystem::Path pluginPath = getApplicationDataPath() / "plugins";
//	
//	if(Filesystem::pathExists(pluginPath) == true) {
//        Filesystem::Iterator endIT;
//		std::for_each(Filesystem::Iterator(pluginPath), endIT, [=](Filesystem::Path p) {
//			if (Filesystem::isDirectory(p)) {
//				pluginLoader->addPath(p.string());
//			}
//		});
//	}
//    pluginLoader->load();
//
//    registerPluginsWrapperFactories();
//
//    registerPluginsServices();
//	registerPluginsParsers();
//    registerPluginsVisualizers();
//    registerPluginsDataProcessors();
//    registerPluginsDataSources();
//
//    sceneRoot = new osg::Group();
//    sceneRoot->setName("root");
//
//	dataManager->findResources(directoriesInfo->getResourcesPath().string());
//    dataManager->loadResources();
//
//    // inicjalizacja us³ug
//    for (int i = 0; i < serviceManager->getNumServices(); ++i) {
//        serviceManager->getService(i)->init(serviceManager, dataManager);
//    }
//
//
//    initializeConsole();          // Console Widget 
//    InitializeControlWidget();          // Control Widget + TimeLine
//    visualizerManager->setDebugWidget(widgetSceneGraph);
//
//    initializeUI();
//
//
//    readSettings(QSettings(), true);
//
//
//    connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));
//    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateServices()));
//    connect(&updateTimer, SIGNAL(timeout()), this, SLOT(updateVisualizers()));
//    updateTimer.start(20);
//
//    if ( getNumViews() ) {
//        connect( &viewerFrameTimer, SIGNAL(timeout()), this, SLOT(update()) );
//        viewerFrameTimer.start( 20 );
//    }
//
//    populateVisualizersMenu(menuCreateVisualizer);
//
//    DataProcessor::test();
//}
//
//ToolboxMain::~ToolboxMain()
//{
//    VisualizerManager::getInstance()->setDebugWidget(nullptr);
//}
//
//void ToolboxMain::readSettings( const QSettings& settings, bool readGeometry )
//{
//    if ( settings.contains("WindowState") ) {
//        restoreState(settings.value("WindowState").toByteArray());
//    }
//    if ( readGeometry && settings.contains("Geometry") ) {
//        restoreGeometry(settings.value("Geometry").toByteArray());
//    }
//}
//
//void ToolboxMain::writeSettings()
//{
//    QSettings settings;
//    settings.setValue("Geometry", saveGeometry());
//    settings.setValue("WindowState", saveState());
//}
//
//void ToolboxMain::closeEvent(QCloseEvent* event)
//{
//    //¿¹danie od³¹czenia siê serwisów od widgetów i elementów UI oraz innych serwisów czy zasobów aplikacji
//    ServiceManager::getInstance()->finalizeServices();
//    writeSettings();
//    QToolboxMain::closeEvent(event);
//}
//
//void ToolboxMain::InitializeControlWidget()
//{
//    InitializeOGSWidget();  // MainWidget 
//
//    // inicjalizacja GridWidget
//    QDockWidget *gDock = new QDockWidget(tr("Service scene graph"), this, Qt::WindowTitleHint);
//    gDock->setObjectName(QString("GridWidget"));
//    gDock->setAllowedAreas(Qt::LeftDockWidgetArea);
//
//    widgetSceneGraph = new SceneGraphWidget();    
//    addDockWidget(Qt::LeftDockWidgetArea, gDock);
//    gDock->setWidget((QWidget*)widgetSceneGraph);
//}
//
//
//
//void ToolboxMain::initializeConsole()
//{
//    widgetConsole = new EDRConsoleWidget(tr("Console"), this, Qt::WindowTitleHint);    
//    widgetConsole->setObjectName("Console");
//    widgetConsole->setAllowedAreas(Qt::BottomDockWidgetArea);
//    addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
//}
//
//void ToolboxMain::InitializeOGSWidget()
//{
//    osg::ref_ptr<osg::Group> root = new osg::Group();
//    root->setName("root");
//    root->addChild(createGrid());
//}
//
////--------------------------------------------------------------------------------------------------
//osg::ref_ptr<osg::Node> ToolboxMain::createGrid()
//{
//    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
//    // create Geometry object to store all the vertices and lines primitive.
//    osg::ref_ptr<osg::Geometry>	linesGeom = new osg::Geometry();
//    float size = 0.5f; 
//    int sizeX = 21; 
//    int sizeY = 21; 
//    // this time we'll preallocate the vertex array to the size we
//    // need and then simple set them as array elements, 8 points
//    // makes 4 line segments.
//    osg::Vec3Array* vertices = new osg::Vec3Array((sizeX+sizeY)*2);
//    for (int i=0; i<sizeX; ++i)
//    {
//        float a_x = (float(i - sizeX/2) )*size; 
//        float a_y = -1.0f*(sizeX/2)*size; 
//        float b_x = (float(i - sizeX/2) )*size; 
//        float b_y = 1.0f*(sizeX/2)*size; 
//        (*vertices)[2*i].set( a_x, a_y, 0.0f);
//        (*vertices)[2*i+1].set( b_x, b_y, 0.0f);
//    }
//
//    for (int i=0; i<sizeY; ++i)
//    {
//        float a_x = -1.0f*(sizeY/2)*size; 
//        float a_y = (float(i - sizeY/2) )*size; 
//        float b_x = 1.0f*(sizeY/2)*size; 
//        float b_y = (float(i - sizeY/2) )*size; 
//        (*vertices)[sizeX*2+2*i].set( a_x, a_y, 0.0f);
//        (*vertices)[sizeX*2+2*i+1].set( b_x, b_y, 0.0f);
//    }
//    // pass the created vertex array to the points geometry object.
//    linesGeom->setVertexArray(vertices);
//    // set the colors as before, plus using the above
//    osg::Vec4Array* colors = new osg::Vec4Array;
//    colors->push_back( osg::Vec4(0.5f, 0.5f, 0.5f, 0.5f) );
//    linesGeom->setColorArray(colors);
//    linesGeom->setColorBinding( osg::Geometry::BIND_OVERALL );
//    // set the normal in the same way color.
//    osg::Vec3Array* normals = new osg::Vec3Array;
//    normals->push_back( osg::Vec3(0.0f, -1.0f, 0.0f) );
//    linesGeom->setNormalArray(normals);
//    linesGeom->setNormalBinding( osg::Geometry::BIND_OVERALL );
//    // This time we simply use primitive, and hardwire the number of coords to use 
//    // since we know up front,
//    linesGeom->addPrimitiveSet( new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, (sizeX+sizeY)*2) );
//    // add the points geometry to the geode.
//    geode->addDrawable(linesGeom);
//    //osgView_->setSceneData((osg::Node *)geode);
//    geode->setName("grid");
//    return geode; 
//}
//
//void ToolboxMain::updateServices()
//{
//	//if(DataManager::getInstance()->isLoadLocalTrialData())	{
//    if(ServiceManager::getInstance()->dataPassRequired() == true)	{
//		loadData();
//	}
//    widgetConsole->flushQueue();
//    if ( updateEnabled ) {
//        ServiceManager::getInstance()->updatePass();
//    }
//    widgetConsole->flushQueue();
//}
//
//void ToolboxMain::updateVisualizers()
//{
//    if ( updateEnabled ) {
//        VisualizerManager::getInstance()->update();
//    }
//}
//
//void ToolboxMain::safeRegisterPlugin(const PluginPtr & plugin)
//{
//
//}
//
//void ToolboxMain::safeRegisterService(const IServicePtr & service)
//{
//    try{
//
//        ServiceManager::getInstance()->registerService(service);
//
//    }catch(std::runtime_error e){
//        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
//            " has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
//            " has caused an error during registration: " << e.what() << ". Service NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("Service " << service->getName() << " " <<service->getDescription() << " with ID " << service->getID() <<
//            " has caused an UNKNOWN error during registration. Service NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::safeRegisterParser(const IParserPtr & parser)
//{
//    try{
//
//        DataManager::getInstance()->registerParser(parser);
//
//    }catch(std::runtime_error e){ 
//        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
//            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
//            " has caused an error during registration: " << e.what() << ". Parser NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("Parser " << parser->getDescription() << " with ID " << parser->getID() << " supporting extensions " << parser->getSupportedExtensions() <<
//            " has caused an UNKNOWN error during registration. Parser NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::safeRegisterObjectFactory(const IObjectWrapperFactoryPtr & factory)
//{
//    try{
//
//        DataManager::getInstance()->registerObjectFactory(factory);
//
//    }catch(std::runtime_error e){ 
//        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an error during registration: "
//            << e.what() << ". Object type NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an error during registration: "
//            << e.what() << ". Object type NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("Object factory for type " << factory->getType().name() << " has caused an UNKNOWN error during registration. Object type NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::safeRegisterVisualizer(const IVisualizerPtr & visualizer)
//{
//    try{
//
//        VisualizerManager::getInstance()->registerVisualizer(visualizer);
//
//    }catch(std::runtime_error e){ 
//        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
//            << " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
//            << " has caused an error during registration: " << e.what() << ". Visualizer NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("Visualizer " << visualizer->getName() << " with ID " << visualizer->getID()
//            << " has caused an UNKNOWN error during registration. Visualizer NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::safeRegisterDataProcessor(const IDataProcessorPtr & dataProcessor)
//{
//    try{
//
//        DataProcessorManager::getInstance()->registerDataProcessor(dataProcessor);
//
//    }catch(std::runtime_error e){ 
//        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
//            << " has caused an error during registration: " << e.what() << ". DataProcessor NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
//            << " has caused an error during registration: " << e.what() << ". DataProcessor NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("DataProcessor " << dataProcessor->getName() << " with ID " << dataProcessor->getID()
//            << " has caused an UNKNOWN error during registration. DataProcessor NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::safeRegisterDataSource(const IDataSourcePtr & dataSource)
//{
//    try{
//
//        DataSourceManager::getInstance()->registerDataSource(dataSource);
//
//    }catch(std::runtime_error e){ 
//        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
//            << " has caused an error during registration: " << e.what() << ". DataSource NOT registered in application!" );
//    }
//    catch(std::invalid_argument e){
//        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
//            << " has caused an error during registration: " << e.what() << ". DataSource NOT registered in application!" );
//    }
//    catch(...){
//        LOG_WARNING("DataSource " << dataSource->getName() << " with ID " << dataSource->getID()
//            << " has caused an UNKNOWN error during registration. DataSource NOT registered in application!" );
//    }
//}
//
//void ToolboxMain::registerCoreServices()
//{
//    // us³uga UI
//    UserInterfaceService* userInterfaceService = new UserInterfaceService();
//    userInterfaceService->setToolboxMain(this);
//    safeRegisterService(IServicePtr(userInterfaceService));
//}
//
//void ToolboxMain::registerCoreDataSources()
//{
//    DataSourceManager::getInstance()->registerDataSource(IDataSourcePtr(new LocalDataSource()));
//}
//
//void ToolboxMain::registerPluginsServices()
//{
//    for ( int i = 0; i < pluginLoader->getNumPlugins(); ++i ) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for ( int j = 0; j < plugin->getNumServices(); ++j ) {
//            safeRegisterService(plugin->getService(j));
//        }
//    }
//}
//
//void ToolboxMain::registerPluginsParsers()
//{
//    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for(int j = 0; j < plugin->getNumParsers(); ++j) {
//            safeRegisterParser(plugin->getParser(j));
//        }
//    }
//
//    //IParserPtr c3dParser = core::shared_ptr<C3DParser>(new C3DParser());
//
//    //safeRegisterParser(c3dParser);
//
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<int>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<double>()) );
//    /*safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__ScalarChannel>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__C3DAnalogChannel>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__EMGChannel>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__GRFChannel>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__MarkerChannel>()) );
//    safeRegisterObjectFactory( IObjectWrapperFactoryPtr(new ObjectWrapperFactory<__MarkerSet>()) );*/
//}
//
//void ToolboxMain::registerPluginsWrapperFactories()
//{
//    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for(int j = 0; j < plugin->getNumWrapperFactories(); ++j) {
//            safeRegisterObjectFactory(plugin->getWrapperFactory(j));
//        }
//    }
//}
//
//void ToolboxMain::registerPluginsVisualizers()
//{
//    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for(int j = 0; j < plugin->getNumVisualizers(); ++j) {
//            safeRegisterVisualizer(plugin->getVisualizer(j));
//        }
//    }
//}
//
//void ToolboxMain::registerPluginsDataProcessors()
//{
//    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for(int j = 0; j < plugin->getNumDataProcessors(); ++j) {
//            safeRegisterDataProcessor(plugin->getDataProcessor(j));
//        }
//    }
//}
//
//void ToolboxMain::registerPluginsDataSources()
//{
//    for (int i = 0; i < pluginLoader->getNumPlugins(); ++i) {
//        PluginPtr plugin = pluginLoader->getPlugin(i);
//        for(int j = 0; j < plugin->getNumDataSources(); ++j) {
//            safeRegisterDataSource(plugin->getDataSource(j));
//        }
//    }
//}
//
//void ToolboxMain::onOpen()
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
//void ToolboxMain::onExit()
//{
//    close();
//}
//
//void ToolboxMain::onMaterial()
//{
//    
//}
//
//void ToolboxMain::onBones()
//{
//
//}
//
//void ToolboxMain::onWireframe()
//{
//
//}
//
//void ToolboxMain::createWorkflow()
//{
//    EDRWorkflowWidget* widget = new EDRWorkflowWidget();
//    widget->setAllowedAreas(Qt::AllDockWidgetAreas);
//
//    QObject::connect( widget, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );
//
//    addDockWidget(Qt::LeftDockWidgetArea, widget);
//}
//
//
//QDockWidget* ToolboxMain::embeddWidget( QWidget* widget, std::vector<QObject*>& widgetActions, const QString& name, const QString& style, const QString& sufix,
//    Qt::DockWidgetArea area /*= Qt::AllDockWidgetAreas*/)
//{
//    // dodajemy widget dokowalny     
//    EDRDockWidget* dock = new EDRDockWidget( name, this, Qt::WindowTitleHint);        
//    dock->setAllowedAreas(area);
//    dock->setObjectName(name + widget->objectName() + "WIDGET" + sufix);
//    dock->setStyleSheet(style);
//    //dock->setWidget(widget);
//    dock->getInnerWidget()->layoutContent->addWidget(widget);
//    QObject::connect( dock, SIGNAL(visibilityChanged(bool)), this, SLOT(onDockWidgetVisiblityChanged(bool)) );
//
//    for(auto it = widgetActions.begin(); it!= widgetActions.end(); it++){
//        dock->getTitleBar()->addObject(*it, IEDRTitleBar::Left);
//    }
//
//    return dock;
//}
//
//void ToolboxMain::initializeUI()
//{
//	//ladowanie styli qt
//	QString style;
//	if(DataManager::getInstance()->getApplicationSkinsFilePathCount() > 0)
//	{
//		//style qt
//		QFile file(QString::fromAscii(DataManager::getInstance()->getApplicationSkinsFilePath(0).c_str(), DataManager::getInstance()->getApplicationSkinsFilePath(0).size()));
//		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
//		{
//			style = file.readAll();
//			file.close();
//		}
//	}
//	setStyleSheet(style);
//
//    setDockOptions( AllowNestedDocks | AllowTabbedDocks );
//    setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
//    setCentralWidget( nullptr );
//    setDocumentMode(true);
//
//    // pozosta³e widgety "p³ywaj¹ce"
//    for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
//        IServicePtr service = ServiceManager::getInstance()->getService(i);
//
//        // HACK
//        std::vector<QObject*> mainWidgetActions;
//        QWidget* viewWidget = service->getWidget(mainWidgetActions);
//
//        std::vector<QObject*> controlWidgetActions;
//        QWidget* controlWidget = service->getControlWidget(controlWidgetActions);
//
//        std::vector<QObject*> settingsWidgetActions;
//        QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);
//
//        if ( viewWidget ) {
//
//            addDockWidget(Qt::RightDockWidgetArea, embeddWidget(
//                viewWidget, 
//                mainWidgetActions,
//                toQString(service->getName()), 
//                style,
//                "",
//                Qt::RightDockWidgetArea));
//		}
//        if ( controlWidget ) {
//            addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(
//                controlWidget, 
//                controlWidgetActions,
//                toQString(service->getName() + " control"), 
//                style,
//                "Control",
//                Qt::BottomDockWidgetArea));
//        }
//        if ( settingsWidget ) {
//            addDockWidget(Qt::LeftDockWidgetArea, embeddWidget(
//                settingsWidget, 
//                settingsWidgetActions,
//                toQString(service->getName() + " settings"), 
//                style,
//                "Settings",
//                Qt::LeftDockWidgetArea));
//        }
//	}
//
//#ifdef UTILS_DEBUG
//    // testowe opcje
//    removeOnClick = false;
//    onTestItemClickedPtr.reset(new Window::ItemPressed(boost::bind(&ToolboxMain::onTestItemClicked, this, _1, _2 )));
//    onTestRemoveToggledPtr.reset(new Window::ItemPressed(boost::bind(&ToolboxMain::onTestRemoveToggled, this, _1, _2 )));
//    core::shared_ptr<IUserInterface> userInterfaceService = queryServices<IUserInterface>(ServiceManager::getInstance());
//    userInterfaceService->getToolboxMain()->addMenuItem("Callback test/Option", onTestItemClickedPtr);
//    userInterfaceService->getToolboxMain()->addMenuItem("Callback test/Nested/Option", onTestItemClickedPtr);
//    userInterfaceService->getToolboxMain()->addMenuItem("Callback test/Nested/Option2", onTestItemClickedPtr);
//    userInterfaceService->getToolboxMain()->addMenuItem("Callback test/Nested2/Option", onTestItemClickedPtr);
//    userInterfaceService->getToolboxMain()->addMenuItem("Callback test/Remove on click?", onTestRemoveToggledPtr, true, removeOnClick);
//#endif // UTILS_DEBUG
//}
//
//void ToolboxMain::onCustomAction()
//{
//    QObject* obj = QObject::sender();
//    std::string path = toStdString(obj->objectName());
//    this->triggerMenuItem(path, false);
//}
//
//void ToolboxMain::onCustomAction( bool triggered )
//{
//    QObject* obj = QObject::sender();
//    std::string path = toStdString(obj->objectName());
//    this->triggerMenuItem(path, triggered);
//}
//
//void ToolboxMain::onRemoveMenuItem( const std::string& path )
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
//void ToolboxMain::onAddMenuItem( const std::string& path, bool checkable, bool initialState )
//{
//    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
//    tokenizer tokens(path, boost::char_separator<char>("/"));
//    tokenizer::iterator next;
//    tokenizer::iterator token = tokens.begin();
//
//    QWidget* currentMenu = QToolboxMain::menuBar();
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
//void ToolboxMain::openFile( const std::string& path )
//{
//    LOG_INFO("Opening file: " << path);
//    std::vector<Filesystem::Path> paths;
//    paths.push_back(path);
//    DataManager::getInstance()->loadFiles(paths);
//}
//
//void ToolboxMain::loadData()
//{
//	ServiceManager::getInstance()->loadDataPass(DataManager::getInstance());
//}
//
//void ToolboxMain::paintEvent( QPaintEvent* event )
//{
//    if ( getNumViews() ) {
//        frame();
//    }
//}
//
//void ToolboxMain::onDockWidgetVisiblityChanged( bool visible )
//{
//
//}
//
//void ToolboxMain::onSaveLayout()
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
//
//void ToolboxMain::openLayout( const QString& path )
//{
//    readSettings(QSettings(path, QSettings::IniFormat), false);
//}
//
//void ToolboxMain::onOpenLayout()
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
//void ToolboxMain::onShowSavedLayouts()
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
//void ToolboxMain::onLayoutTriggered()
//{
//    if ( QAction* action = qobject_cast<QAction*>(QObject::sender()) ) {
//        openLayout( action->data().value<Filesystem::Path>().string().c_str() );
//    }
//}
//
//void ToolboxMain::addLayoutsToMenu( const Filesystem::Path &dir )
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
//
//void ToolboxMain::populateWindowMenu()
//{
//    // uwaga: nie musimy usuwaæ starych akcji, poniewa¿ QMenu pilnuje,
//    // aby nie by³ dodane dwie jednakowe instancje
//    populateWindowMenu(menuWindow);
//}
//
//void ToolboxMain::populateVisualizersMenu()
//{
//    // czyœcimy menu
//    menuCreateVisualizer->clear();
//    populateVisualizersMenu(menuCreateVisualizer);
//}
//
//void ToolboxMain::actionCreateVisualizer()
//{
//    QAction* action = qobject_cast<QAction*>(sender());
//    VisualizerWidget* widget = new VisualizerWidget(action->data().value<UniqueID>(), this, Qt::WindowTitleHint);
//    widget->setAllowedAreas(Qt::RightDockWidgetArea);
//    widget->setStyleSheet(styleSheet());
//    addDockWidget(Qt::RightDockWidgetArea, widget);
//}
//
//void ToolboxMain::visualizerWidgetClosed( VisualizerWidget* widget )
//{
//
//}
//
//
//void ToolboxMain::populateVisualizersMenu( QMenu* menu )
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
//void ToolboxMain::populateWindowMenu( QMenu* menu )
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
//void ToolboxMain::refreshVisualizerWidgetsNames()
//{
//}
//
//
//#ifdef UTILS_DEBUG
//
//void ToolboxMain::onTestItemClicked(const std::string& sender, bool state)
//{
//    if (removeOnClick) {
//        removeMenuItem(sender);
//    }
//}
//
//void ToolboxMain::onTestRemoveToggled(const std::string& sender, bool state )
//{
//    removeOnClick = state;
//}
//
//void ToolboxMain::init( PluginLoader* pluginLoader )
//{
//	throw std::exception("The method or operation is not implemented.");
//}
//
//
//#endif // UTILS_DEBUG
