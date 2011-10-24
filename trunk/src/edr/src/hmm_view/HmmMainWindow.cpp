#include "HmmMainWindow.h"
#include <core/ServiceManager.h>
#include <core/IService.h>
#include <utils/Push.h>
#include <plugins/subject/Motion.h>
#include <plugins/subject/Session.h>
#include <QtGui/QTreeWidgetItem>
#include <QtGui/QGridLayout>
#include <boost/foreach.hpp>
#include "ui_LoadingDialog.h"
#include "LoadingDialog.h"
#include "VisualizerWidget.h"
#include "WorkflowWidget.h"
#include "EDRConsoleInnerWidget.h"
#include "EDRConsoleWidget.h"
#include "EDRDockWidgetSet.h"
#include "EDRDockWidgetManager.h"
#include "DataFilterWidget.h"

using namespace core;


HmmMainWindow::HmmMainWindow() :
	MainWindow(),
	currentVisualizer(nullptr),
	topMainWindow(nullptr),
	//treeWidget(nullptr),
    currentItem(nullptr)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
    itemClickAction.setMainWindow(this);
}


void HmmMainWindow::init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor )
{
	core::MainWindow::init(pluginLoader, managersAccessor);

	setupUi(this);

	topButton->setFixedWidth(0);
	bottomButton->setFixedWidth(0);

	connect(this->fontSlider, SIGNAL(valueChanged(int)), this, SLOT(setFont(int)));
	connect(this->layoutTopSlider, SIGNAL(valueChanged(int)), this, SLOT(setTop(int)));
	connect(this->layoutBottomSlider, SIGNAL(valueChanged(int)), this, SLOT(setBottom(int)));
	
	this->showFullScreen();

    treeWidget->setColumnCount(2);
    treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));    
	QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    

    QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    topMainWindow = new EDRDockWidgetManager(this);
    bottomMainWindow = new QMainWindow(this);
    bottomMainWindow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    createFilterTabs();


    //splitter->addWidget(topMainWindow->asQWidget());
    //splitter->addWidget(bottomMainWindow);

    QGridLayout* v = new QGridLayout(hlayout);
    //v->addWidget(splitter);
    v->addWidget(topMainWindow->asQWidget());
    topMainWindow->asQWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    //bottomMainWindow->setMaximumHeight(bottomMainWindow->sizeHint().height());
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    hlayout->setLayout(v);

    int i = hlayout->children().size();

	for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		IServicePtr service = ServiceManager::getInstance()->getService(i);

		const std::string& name = service->getName();
		// HACK! nie da sie na razie castowac na CommunicationService
		// trzeba by do HmmView dodac zaleznosc od tego pluginu (lub jego bibliotek)
		// Nie da sie jednak includowac z tego poziomu odpowiedniego naglowka 
		// (gdzies po drodze wymagane sa prywane naglowki z Communication)
		// Moze wlasciwe bedzie identyfikowanie po UniqueID.
		if (name == "Communication") {
			std::vector<QObject*> mainWidgetActions;
			QWidget* viewWidget = service->getWidget(mainWidgetActions);

			std::vector<QObject*> controlWidgetActions;
			QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

			std::vector<QObject*> settingsWidgetActions;
			QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

			QVBoxLayout *layout = new QVBoxLayout;
			layout->addWidget(settingsWidget);
			this->Badania_2->setLayout(layout);
        }else if (name == "newTimeline") {
            // przeniesione do showTimeline();
        }
	}

    // akcje - Workflow (VDF) i konsola

    QMainWindow * actionsMainWindow = new QMainWindow(nullptr);

	EDRWorkflowWidget* widget = new EDRWorkflowWidget();
	QVBoxLayout* layout = new QVBoxLayout();
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
    layout->addWidget(actionsMainWindow);

	Operations->setLayout(layout);
}

HmmMainWindow::~HmmMainWindow()
{

}

bool HmmMainWindow::isDataItem(QTreeWidgetItem * item)
{
    HmmTreeItem* hmmItem = dynamic_cast<HmmTreeItem*>(item);
    return hmmItem && hmmItem->getHelper()->isDataItem();
}

void HmmMainWindow::createNewVisualizer()
{
    if(emptyVisualizersPool.empty() == false);
}

void HmmMainWindow::onTreeContextMenu(const QPoint & pos)
{
    currentItem = treeWidget->itemAt(pos);

    if(currentItem == nullptr || isDataItem(currentItem) == false){
        return;
    }

    QMenu * menu = new QMenu();

    if(items2Descriptions.find(currentItem) == items2Descriptions.end()){
        QAction * addNew = new QAction(menu);
        addNew->setText(QString::fromUtf8("Add to new visualizer"));
        menu->addAction(addNew);
        connect(addNew, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));

        menu->addAction("Add to existing visualizer");
    }else{
        QAction * action = new QAction(menu);
        action->setText(QString::fromUtf8("Remove"));
        menu->addAction(action);
    }    

    menu->exec(pos);
}

void HmmMainWindow::visualizerFocusChanged(bool focus)
{
    VisualizerWidget * widget = qobject_cast<VisualizerWidget *>(sender());

    if(widget == nullptr){
        return;
    }

    if(widget != currentVisualizer){
        if(currentVisualizer != nullptr){
            currentVisualizer->setTitleBarVisible(!focus);
        }

        currentVisualizer = widget;
    }

    currentVisualizer->setTitleBarVisible(focus);
}

void HmmMainWindow::onOpen()
{
	utils::Push<bool> pushed(updateEnabled, false);
	Filesystem::Path initPath = getUserDataPath() / "trial";
	const QString directory = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
	if (!directory.isEmpty()) 
	{
		LoadingDialog* d = new LoadingDialog();
		d->setWindowFlags(Qt::Tool);
		d->start(directory);
		delete d;
	}

	std::vector<SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance());
    currentSessions = sessions;
    QTreeWidgetItem* item = SimpleFilterCommand::createTree("Sessions", sessions);
    treeWidget->addTopLevelItem(item);
}

void HmmMainWindow::setFont( int size )
{
	const QFont& font = this->tabWidget->font();
	QFont newFont(font);
	newFont.setPointSize(size);
	this->tabWidget->setFont(newFont);
}

void HmmMainWindow::setTop( int size )
{
	topButton->setFixedHeight(size);
	topButton->setFixedWidth(0);
}

void HmmMainWindow::setBottom( int size )
{
	bottomButton->setFixedHeight(size);
	bottomButton->setFixedWidth(0);
}

void HmmMainWindow::onTreeItemClicked( QTreeWidgetItem *item, int column )
{
    // sprawdzanie, czy pod item jest podpiety jakis obiekt
    HmmTreeItem* hmmItem = dynamic_cast<HmmTreeItem*>(item);
    
    if (hmmItem) {
        showTimeline();
        TreeItemHelper* helper = hmmItem->getHelper();
        std::stack<QString> pathStack;
        QTreeWidgetItem * pomItem = item;

        while(pomItem != nullptr){
            pathStack.push(pomItem->text(0));
            pomItem = pomItem->parent();
        }

        QString path;

        path += pathStack.top();
        pathStack.pop();

        while(pathStack.empty() == false){
            path += "/";
            path += pathStack.top();
            pathStack.pop();
        }

        VisualizerManager* visualizerManager = VisualizerManager::getInstance();
        VisualizerPtr visualizer = helper->createVisualizer();
        visualizer->getOrCreateWidget();

        VisualizerWidget* visualizerDockWidget = new VisualizerWidget(visualizer);
        visualizerDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
        //visu->setStyleSheet(styleSheet());
        visualizerDockWidget->setVisualizerIconVisible(false);
        visualizerDockWidget->setSplitHVisible(false);
        visualizerDockWidget->setSplitVVisible(false);
        visualizerDockWidget->setActiveVisualizerSwitch(false);
        visualizerDockWidget->setSourceVisible(false);

        visualizerDockWidget->setTitleBarVisible(false);

        std::vector<VisualizerTimeSeriePtr> series;
        helper->createSeries(visualizer, path, series);
        visualizer->getWidget()->setFocusProxy(visualizerDockWidget);

        connect(visualizerDockWidget, SIGNAL(focuseChanged(bool)), this, SLOT(visualizerFocusChanged(bool)));

        topMainWindow->autoAddDockWidget( visualizerDockWidget);
        connect(visualizerDockWidget, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));

        TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
        if(timeline != nullptr) {
            if (series.size() == 1 && series[0] != nullptr) {
                VisualizerChannelPtr channel(new VisualizerChannel(series[0], visualizerDockWidget));
                try{
                    timeline->addChannel(path.toStdString(), channel);
                }catch(...){
                    LOG_ERROR("Could not add channel to timeline!");
                }
            } else {
                VisualizerMultiChannel::SeriesWidgets seriesMap;
                for (int i = 0; i < series.size(); i++) {
                    auto timeSerie = series[i];
                    if(timeSerie != nullptr){
                       seriesMap[timeSerie] = visualizerDockWidget;
                    }
                }
                if (seriesMap.size() > 0) {
                    VisualizerMultiChannelPtr multi(new VisualizerMultiChannel(seriesMap));
                    try {
                        timeline->addChannel(path.toStdString(), multi);
                    } catch (...) {
                        LOG_ERROR("Could not add multichannel to timeline!");
                    }
                }
            }
        }
    }
}


void HmmMainWindow::showTimeline()
{
    static bool timelineVisible = false;
    if (timelineVisible == false) {
        for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		    IServicePtr service = ServiceManager::getInstance()->getService(i);

		    const std::string& name = service->getName();
		    if (name == "newTimeline") {
                std::vector<QObject*> mainWidgetActions;
                QWidget* viewWidget = service->getWidget(mainWidgetActions);

                std::vector<QObject*> controlWidgetActions;
                QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

                std::vector<QObject*> settingsWidgetActions;
                QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

                EDRDockWidget * widget = new EDRDockWidget();
                widget->getInnerWidget()->layout()->addWidget(controlWidget);
                widget->setAllowedAreas(Qt::BottomDockWidgetArea);
                widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                for(auto it = controlWidgetActions.begin(); it!= controlWidgetActions.end(); it++){
                    widget->getTitleBar()->addObject(*it, IEDRTitleBar::Left);
                }
			
                bottomMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);

                widget->getTitleBar()->setFloatButtonVisible(false);
                widget->getTitleBar()->setCloseButtonVisible(false);

                widget->setTitleBarVisible(false);
                timelineVisible = true;
            }
	    }
    }
}

void HmmMainWindow::clearTree()
{
    treeWidget->clear();
}

void HmmMainWindow::addItemToTree( QTreeWidgetItem* item )
{
    treeWidget->addTopLevelItem(item);
}

void HmmMainWindow::createFilterTabs()
{
    filterTabWidget->clear();
    createFilterTab1();
    createFilterTab2();
}

void HmmMainWindow::createFilterTab1()
{
    core::IMemoryDataManager * memoryDataManager = managersAccessor->getMemoryDataManager();

    QWidget* tab = new QWidget(filterTabWidget);
    QVBoxLayout* tabLayout = new QVBoxLayout();
    QMargins margins(0, 0, 0, 0);
    tabLayout->setContentsMargins(margins);
    tabLayout->setMargin(0);
    tab->setLayout(tabLayout);
    QWidget* h1 = new QWidget(tab);
    QWidget* h2 = new QWidget(tab);
    QHBoxLayout* htablayout1 = new QHBoxLayout();
    htablayout1->setContentsMargins(margins);
    htablayout1->setMargin(0);
    QHBoxLayout* htablayout2 = new QHBoxLayout();
    htablayout2->setContentsMargins(margins);
    htablayout2->setMargin(0);
    h1->setLayout(htablayout1);
    h2->setLayout(htablayout2);
    tabLayout->addWidget(h1);
    tabLayout->addWidget(h2);
    QIcon iconAnalog(core::getResourceString("icons/analogBig.png"));
    QIcon iconKinetic(core::getResourceString("icons/kineticBig.png"));
    QIcon iconKinematic(core::getResourceString("icons/kinematicBig.png"));
    QIcon iconVideo(core::getResourceString("icons/videoBig.png"));
    //DataFilterWidget* filter1 = new DataFilterWidget(icon.pixmap(64, 64), "Charts", "this filter gives only charts");
    //DataFilterWidget* filter2 = new DataFilterWidget(icon.pixmap(64, 64), "Emg", "this filter gives only emgs");

    DataFilterWidget* filter1 = new DataFilterWidget(iconAnalog.pixmap(48,48), this);
    DataFilterWidget* filter2 = new DataFilterWidget(iconKinetic.pixmap(48,48), this);
    DataFilterWidget* filter3 = new DataFilterWidget(iconKinematic.pixmap(48,48), this);
    DataFilterWidget* filter4 = new DataFilterWidget(iconVideo.pixmap(48,48), this);

    std::vector<TypeInfo> types;
    types.push_back(typeid(GRFCollection));
    types.push_back(typeid(GRFChannel));
    TypeFilterPtr typeFilter1(new TypeFilter(memoryDataManager, types));
    TypeFilterPtr typeFilter2(new TypeFilter(memoryDataManager, typeid(EMGChannel)));

    filter1->addFilter("Grf", "label", typeFilter1);
    filter1->addFilter("Emg", "emg label", typeFilter2);

    TypeFilterPtr typeFilter3(new TypeFilter(memoryDataManager, typeid(ForceCollection)));
    TypeFilterPtr typeFilter4(new TypeFilter(memoryDataManager, typeid(MomentCollection)));
    TypeFilterPtr typeFilter5(new TypeFilter(memoryDataManager, typeid(PowerCollection)));

    filter2->addFilter("Force", "count: ", typeFilter3);
    filter2->addFilter("Moment", "moments", typeFilter4);
    filter2->addFilter("Power", "powers", typeFilter5);

    TypeFilterPtr typeFilter6(new TypeFilter(memoryDataManager, typeid(MarkerCollection)));
    TypeFilterPtr typeFilter7(new TypeFilter(memoryDataManager, typeid(kinematic::JointAnglesCollection)));

    filter3->addFilter("Markers", "count: 1", typeFilter6);
    filter3->addFilter("Joints", "count: 1", typeFilter7);

    TypeFilterPtr typeFilter8(new TypeFilter(memoryDataManager, typeid(VideoChannel)));
    filter4->addFilter("Video", "count: 4", typeFilter8);

    connect(filter1, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter2, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter3, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter4, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));

    dataFilterWidgets.push_back(filter1);
    dataFilterWidgets.push_back(filter2);
    dataFilterWidgets.push_back(filter3);
    dataFilterWidgets.push_back(filter4);

    htablayout1->addWidget(filter1);
    htablayout1->addWidget(filter2);
    htablayout2->addWidget(filter3);
    htablayout2->addWidget(filter4);
    filterTabWidget->addTab(tab, "Filters");
}

void HmmMainWindow::createFilterTab2()
{
    core::IMemoryDataManager * memoryDataManager = managersAccessor->getMemoryDataManager();

    QWidget* tab = new QWidget(filterTabWidget);
    QVBoxLayout* tabLayout = new QVBoxLayout();
    tab->setLayout(tabLayout);
    QWidget* h1 = new QWidget(tab);
    QWidget* h2 = new QWidget(tab);
    QHBoxLayout* htablayout1 = new QHBoxLayout();
    QHBoxLayout* htablayout2 = new QHBoxLayout();
    h1->setLayout(htablayout1);
    h2->setLayout(htablayout2);

    QMargins margins(0, 0, 0, 0);
    tabLayout->setContentsMargins(margins);
    tabLayout->setMargin(0);
    htablayout1->setContentsMargins(margins);
    htablayout1->setMargin(0);
    htablayout2->setContentsMargins(margins);
    htablayout2->setMargin(0);

    tabLayout->addWidget(h1);
    tabLayout->addWidget(h2);
    QIcon iconKinetic(core::getResourceString("icons/kineticBig.png"));
   
    DataFilterWidget* filter1 = new DataFilterWidget(iconKinetic.pixmap(48,48), this);
    DataFilterWidget* filter2 = new DataFilterWidget(iconKinetic.pixmap(48,48), this);
    DataFilterWidget* filter3 = new DataFilterWidget(iconKinetic.pixmap(48,48), this);
    DataFilterWidget* filter4 = new DataFilterWidget(iconKinetic.pixmap(48,48), this);

    TypeFilterPtr typeFilter1(new TypeFilter(memoryDataManager, typeid(GRFChannel)));
    TypeFilterPtr typeFilter2(new TypeFilter(memoryDataManager, typeid(EMGChannel)));

    IFilterCommandPtr multi1(new MultiChartCommand<ForceCollection>());
    IFilterCommandPtr multi2(new MultiChartCommand<MomentCollection>());
    IFilterCommandPtr multi3(new MultiChartCommand<PowerCollection>());

    filter1->addFilter("Force", "Multi Chart", multi1);
    filter1->addFilter("Moment", "Multi Chart", multi2);
    filter1->addFilter("Power", "Multi Chart", multi3);

    filter2->addFilter("Force", "Multi Chart", multi1);
    filter2->addFilter("Moment", "Multi Chart", multi2);
    filter2->addFilter("Power", "Multi Chart", multi3);

    filter3->addFilter("Force", "Multi Chart", multi1);
    filter3->addFilter("Moment", "Multi Chart", multi2);
    filter3->addFilter("Power", "Multi Chart", multi3);

    filter4->addFilter("Force", "Multi Chart", multi1);
    filter4->addFilter("Moment", "Multi Chart", multi2);
    filter4->addFilter("Power", "Multi Chart", multi3);

    connect(filter1, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter2, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter3, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter4, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));

    dataFilterWidgets.push_back(filter1);
    dataFilterWidgets.push_back(filter2);
    dataFilterWidgets.push_back(filter3);
    dataFilterWidgets.push_back(filter4);

    htablayout1->addWidget(filter1);
    htablayout1->addWidget(filter2);
    htablayout2->addWidget(filter3);
    htablayout2->addWidget(filter4);
    filterTabWidget->addTab(tab, "Multi charts");
}

void HmmMainWindow::filterGroupActivated( bool active )
{
    DataFilterWidget* dataWidget = qobject_cast<DataFilterWidget*>(sender());

    if (dataWidget) {
        BOOST_FOREACH(DataFilterWidget* widget, dataFilterWidgets) {
            if (widget != dataWidget) {
                widget->blockSignals(true);
                widget->setActive(false);
                widget->blockSignals(false);
            }
        }

        if (!active) {
            treeWidget->clear();
            QTreeWidgetItem* item = SimpleFilterCommand::createTree("Sessions", getCurrentSessions());
            treeWidget->addTopLevelItem(item);
        }
    }
}






