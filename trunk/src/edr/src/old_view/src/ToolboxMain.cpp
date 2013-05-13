#include "ToolboxMain.h"
#include "ui_toolboxmaindeffile.h"
#include <utils/Push.h>
#include <corelib/BaseDataTypes.h>
#include <QtGui/QAction>
#include <iostream>
#include <corelib/PluginCommon.h>
#include <corelib/IServiceManager.h>
#include <corelib/IVisualizerManager.h>
#include <corelib/ISourceManager.h>

#include <utils/Debug.h>

#include <corelib/StringTools.h>
#include <boost/foreach.hpp>
#include <coreui/CoreVisualizerWidget.h>

#include <corelib/Visualizer.h>

#include <coreui/CoreDockWidget.h>
#include <coreui/CoreTitleBar.h>
#include <coreui/CoreSplitableDockWidgetT.h>
#include <timelinelib/IChannel.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newTimeline/ITimelineService.h>


using namespace coreUI;
using namespace core;
using namespace plugin;

Q_DECLARE_METATYPE(UniqueID);

struct SortActionsByNames 
{
	inline bool operator()(const QAction* a1, const QAction* a2)
	{
		return a1->text() < a2->text();
	}
};

typedef CoreSplitableDockWidgetT<WidgetClonePolicyCopyConstructor<CoreVisualizerWidget>> VisualizerWidget;

ToolboxMain::ToolboxMain(const CloseUpOperations & closeUpOperations) : CoreMainWindow(closeUpOperations), ui(new Ui::EDRMain), visualizersPlaceholder(new QMainWindow)
{
	ui->setupUi(this);
	qApp->setApplicationName("EDR");
	QString style = this->styleSheet();
	setDockOptions( AllowNestedDocks | AllowTabbedDocks );
	setTabPosition( Qt::RightDockWidgetArea, QTabWidget::North );
	setCentralWidget( visualizersPlaceholder );
	//setDocumentMode(true);	
	setWindowIcon(QPixmap(QString::fromUtf8(":/coreUI/icons/appIcon.png")));
}

ToolboxMain::~ToolboxMain()
{
	delete helper;
}

void ToolboxMain::initializeSplashScreen(QSplashScreen * splashScreen)
{

}

void ToolboxMain::customViewInit(QWidget * console)
{
	trySetStyleByName("dark");
	connect(ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));	
	auto dockConsole = embeddWidget(console, tr("Console"), Qt::BottomDockWidgetArea, true);
	addDockWidget(Qt::BottomDockWidgetArea, dockConsole);
	dockConsole->setFeatures(dockConsole->features() | QDockWidget::DockWidgetVerticalTitleBar);
	dockConsole->setObjectName(QString::fromUtf8("ConsoleWidget"));
	populateVisualizersMenu(ui->menuCreateVisualizer);
	initializeUI();
}

void ToolboxMain::initializeUI()
{
	// pozostałe widgety "pływające"
	for (int i = 0; i < getServiceManager()->getNumServices(); ++i) {
		auto service = getServiceManager()->getService(i);

		QWidget* viewWidget = service->getWidget();

		QWidget* controlWidget = service->getControlWidget();

		QWidget* settingsWidget = service->getSettingsWidget();

		if ( viewWidget ) {
			addDockWidget(Qt::RightDockWidgetArea, embeddWidget(viewWidget, QString::fromStdString(service->getName()), Qt::RightDockWidgetArea, true));
		}

		if ( controlWidget ) {
			addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(controlWidget, QString::fromStdString(service->getName()) + " " + tr("control"), Qt::BottomDockWidgetArea, true));
		}

		if ( settingsWidget ) {
			addDockWidget(Qt::LeftDockWidgetArea, embeddWidget(settingsWidget, QString::fromStdString(service->getName()) + " " + tr("settings"), Qt::LeftDockWidgetArea, true));
		}
	}

	for (int i = 0; i < getSourceManager()->getNumSources(); ++i) {
		auto source = getSourceManager()->getSource(i);

		QWidget* viewWidget = source->getWidget();

		QWidget* controlWidget = source->getControlWidget();

		QWidget* settingsWidget = source->getSettingsWidget();

		if ( viewWidget ) {
			addDockWidget(Qt::RightDockWidgetArea, embeddWidget(viewWidget, QString::fromStdString(source->getName()), Qt::RightDockWidgetArea, true));
		}

		if ( controlWidget ) {
			addDockWidget(Qt::BottomDockWidgetArea, embeddWidget(controlWidget, QString::fromStdString(source->getName()) + " " + tr("control"), Qt::BottomDockWidgetArea, true));
		}

		if ( settingsWidget ) {
			addDockWidget(Qt::LeftDockWidgetArea, embeddWidget(settingsWidget, QString::fromStdString(source->getName()) + " " + tr("settings"), Qt::LeftDockWidgetArea, true));
		}
	}

	auto timelineService = core::queryServices<ITimelineService>(plugin::getServiceManager());

	helper = new VisualizerTimelineHelper(timelineService.get());
}

void ToolboxMain::onExit()
{
	close();
}

void ToolboxMain::populateWindowMenu()
{
	// uwaga: nie musimy usuwać starych akcji, ponieważ QMenu pilnuje,
	// aby nie był dodane dwie jednakowe instancje
	populateWindowMenu(ui->menuWindow);
}

void ToolboxMain::populateVisualizersMenu()
{
	// czyścimy menu
	ui->menuCreateVisualizer->clear();
	populateVisualizersMenu(ui->menuCreateVisualizer);
}

void ToolboxMain::actionCreateVisualizer()
{
	QAction* action = qobject_cast<QAction*>(sender());
	auto visProto = getVisualizerManager()->getVisualizerPrototype(action->data().value<UniqueID>());
	VisualizerWidget * dockWidget = new VisualizerWidget();
	CoreVisualizerWidget* widget = new CoreVisualizerWidget(VisualizerPtr(visProto->create()), this, Qt::WindowTitleHint);
	widget->getVisualizer()->addDataSource(core::Visualizer::VisualizerDataSourcePtr(new core::VisualizerMemoryDataSource(plugin::getDataManagerReader())));

	widget->getVisualizer()->addObserver(helper);

	QList<QAction*> actions = widget->getVisualizer()->getOrCreateWidget()->actions();
	actions.append(widget->actions());
	actions.append(dockWidget->actions());

	dockWidget->setWidget(widget);

	auto visualizerTitleBar = CoreTitleBar::supplyWithCoreTitleBar(dockWidget);
	CoreTitleBar::supplyCoreTitleBarWithActions(visualizerTitleBar, actions);
	
	visualizersPlaceholder->addDockWidget(Qt::RightDockWidgetArea, dockWidget);
}

void ToolboxMain::populateVisualizersMenu( QMenu* menu )
{
	std::vector<QAction*> sortedActions;
	// dodajemy wizualizatory
	IVisualizerManager::VisualizerPrototypes visPrototypes;
	getVisualizerManager()->visualizerPrototypes(visPrototypes);
	BOOST_FOREACH(VisualizerConstPtr vis, visPrototypes) {
		QAction* action = new QAction(toQString(vis->getName()), menu);
		action->setData( qVariantFromValue(vis->getID()) );
		action->setIcon( vis->getIcon() );
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

CoreDockWidget * ToolboxMain::embeddWidget(QWidget * widget, const QString & windowTitle, Qt::DockWidgetArea allowedAreas, bool permanent)
{
	CoreDockWidget * embeddedDockWidget = new CoreDockWidget(windowTitle);
	embeddedDockWidget->setWidget(widget);
	embeddedDockWidget->setAllowedAreas(allowedAreas);
	embeddedDockWidget->setPermanent(permanent);

	auto consoleTitleBar = CoreTitleBar::supplyWithCoreTitleBar(embeddedDockWidget);
	CoreTitleBar::supplyCoreTitleBarWithActions(consoleTitleBar, widget);

	return embeddedDockWidget;
}
