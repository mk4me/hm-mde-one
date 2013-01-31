#include "ToolboxMain.h"
#include <utils/Push.h>
#include <corelib/BaseDataTypes.h>
#include <osgui/QOsgWidgets.h>
//#include "SceneGraphWidget.h"
#include <osg/Vec3d>
#include <osg/Quat>
#include <osg/PositionAttitudeTransform>

#include <osgViewer/GraphicsWindow>
#include <osgText/Text>
#include <osg/ShapeDrawable>
#include <osgViewer/Scene>
#include <iostream>
#include <osgGA/TerrainManipulator>

#include <iostream>

#include <utils/Debug.h>

#include <boost/tokenizer.hpp>
#include <boost/bind.hpp>
#include <functional>

#include <corelib/StringTools.h>
#include <boost/foreach.hpp>
#include <coreui/CoreVisualizerWidget.h>

#include <corelib/Visualizer.h>
#include <osgWidget/ViewerEventHandlers>

#include <osgui/EventCallback.h>
#include <osg/BlendFunc>
#include <osg/LineWidth>

#include <boost/random.hpp>

#include <utils/Push.h>

#include <coreui/CoreDockWidget.h>
#include <coreui/CoreTitleBar.h>

using namespace coreUI;
using namespace core;

struct SortActionsByNames 
{
	inline bool operator()(const QAction* a1, const QAction* a2)
	{
		return a1->text() < a2->text();
	}
};

ToolboxMain::ToolboxMain(const CloseUpOperations & closeUpOperations) : CoreMainWindow(closeUpOperations)
{

}

ToolboxMain::~ToolboxMain()
{

}

void ToolboxMain::customViewInit(QWidget * console)
{
	initializeUI();
	setupUi(this);
	connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));
	CoreDockWidget * consoleDockWidget = new CoreDockWidget(console->windowTitle().isEmpty() == true ? tr("Console") : console->windowTitle());
	consoleDockWidget->setWidget(console);

	auto consoleTitleBar = CoreTitleBar::supplyWithCoreTitleBar(consoleDockWidget);
	CoreTitleBar::supplyCoreTitleBarWithActions(consoleTitleBar, console);
	
	addDockWidget(Qt::BottomDockWidgetArea, consoleDockWidget);
	populateVisualizersMenu(menuCreateVisualizer);
}


void ToolboxMain::initializeUI()
{
	qApp->setApplicationName("EDR");
    trySetStyleByName("dark");
    QString style = this->styleSheet();
	setDockOptions( AllowNestedDocks | AllowTabbedDocks );
	setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
	setCentralWidget( nullptr );
	setDocumentMode(true);

	IServicePtr dataExplorer;

	// pozostałe widgety "pływające"
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
		//obsłużyc konteksy źródeł
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

void ToolboxMain::onExit()
{
	close();
}

void ToolboxMain::populateWindowMenu()
{
	// uwaga: nie musimy usuwać starych akcji, ponieważ QMenu pilnuje,
	// aby nie był dodane dwie jednakowe instancje
	populateWindowMenu(menuWindow);
}

void ToolboxMain::populateVisualizersMenu()
{
	// czyścimy menu
	menuCreateVisualizer->clear();
	populateVisualizersMenu(menuCreateVisualizer);
}

void ToolboxMain::actionCreateVisualizer()
{
	QAction* action = qobject_cast<QAction*>(sender());
	CoreVisualizerWidget* widget = new CoreVisualizerWidget(action->data().value<UniqueID>(), this, Qt::WindowTitleHint);
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
