#include "CorePCH.h"
#include "ToolboxMain.h"
#include <utils/Push.h>
#include <core/TypeInfo.h>
#include <core/ObjectWrapperFactory.h>
#include "MainWindow.h"
#include <osgui/QOsgWidgets.h>
//#include "SceneGraphWidget.h"

#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>
#include "SourceManager.h"

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
#include "EDRTitleBar.h"

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

void ToolboxMain::init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor )
{
	MainWindow::init(pluginLoader, managersAccessor);
	initializeUI();
	setupUi(this);
	connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));
    initializeConsole();
    addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
	populateVisualizersMenu(menuCreateVisualizer);
}

void ToolboxMain::setCurrentVisualizerActions(VisualizerWidget * visWidget)
{
    auto vis = visWidget->getCurrentVisualizer();

    VisualizerWidget::VisualizerTitleBarElements titleBarElements;

    visWidget->getVisualizerTitleBarElements(titleBarElements);

    EDRTitleBar * titleBar = nullptr;

    if(visWidget->titleBarWidget() == nullptr){
        titleBar = supplyWithEDRTitleBar(visWidget);
    }

    if(titleBar == nullptr){
        LOG_WARNING("Could not load visualizer toolbar elements - TitleBar uninitialized");
    }else{

        titleBar->clear();

        for(auto it = titleBarElements.begin(); it != titleBarElements.end(); it++){
            titleBar->addObject((*it).first, (*it).second);
        }
    }
}

void ToolboxMain::initializeUI()
{
    trySetStyleByName("dark");
    QString style = this->styleSheet();
	setDockOptions( AllowNestedDocks | AllowTabbedDocks );
	setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
	setCentralWidget( nullptr );
	setDocumentMode(true);

	IServicePtr dataExplorer;

	// pozosta³e widgety "p³ywaj¹ce"
	for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		IServicePtr service = ServiceManager::getInstance()->getService(i);

		if(service->getName() != "DataExplorer"){
			// HACK
			ActionsGroupManager mainWidgetActions;
			QWidget* viewWidget = service->getWidget(&mainWidgetActions);

			ActionsGroupManager controlWidgetActions;
			QWidget* controlWidget = service->getControlWidget(&controlWidgetActions);

			ActionsGroupManager settingsWidgetActions;
			QWidget* settingsWidget = service->getSettingsWidget(&settingsWidgetActions);

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
		}else{
			dataExplorer = service;
		}
	}

	QTabWidget * dataTabWidget = createNamedObject<QTabWidget>(QString::fromUtf8("dataTabWidget"));

	for (int i = 0; i < SourceManager::getInstance()->getNumSources(); ++i) {
		auto source = SourceManager::getInstance()->getSource(i);

		ActionsGroupManager settingsWidgetActions;
		dataTabWidget->addTab(source->getWidget(&settingsWidgetActions), QString::fromStdString(source->getName()));
		//TODO
		//obs³u¿yc konteksy Ÿróde³
	}

	if(dataExplorer != nullptr){
		ActionsGroupManager settingsWidgetActions;
		dataTabWidget->addTab(dataExplorer->getWidget(&settingsWidgetActions), QString::fromStdString(dataExplorer->getName()));
	}

	addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(
		dataTabWidget, 
		ActionsGroupManager(),
		tr("Data Sources"), 
		style,
		"",
		Qt::RightDockWidgetArea));
}


void ToolboxMain::onOpen()
{
	utils::Push<bool> pushed(updateEnabled, false);
	Filesystem::Path initPath = getUserDataPath() / "trial";
	const QString directory = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
	if (!directory.isEmpty()) 
	{
		//std::vector<Filesystem::Path> paths;
		Filesystem::Path dirPath(directory.toStdString());
		Filesystem::Iterator itEnd;

		for( Filesystem::Iterator it(dirPath); it != itEnd; it++){
			if(Filesystem::isRegularFile(*it) == true){
				//paths.push_back(it->path());
                try{
                    DataManager::getInstance()->addFile(*it);
                }catch(...){

                }
			}
		}        
	}
}

void ToolboxMain::onExit()
{
	close();
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

void ToolboxMain::onLayoutTriggered()
{
	
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
