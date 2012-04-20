#include "hmmPCH.h"

#include <cmath>
#include "HmmMainWindow.h"
#include "SourceManager.h"
#include "ui_LoadingDialog.h"
#include "LoadingDialog.h"
#include "VisualizerWidget.h"
#include "WorkflowWidget.h"
#include "EDRConsoleInnerWidget.h"
#include "EDRConsoleWidget.h"
#include "EDRDockWidgetSet.h"
#include "EDRDockWidgetManager.h"
#include "DataFilterWidget.h"
#include "TreeBuilder.h"
#include "Vector3DFilterCommand.h"
#include "AnalisisWidget.h"
#include <plugins/subject/ISubjectService.h>
#include <plugins/c3d/C3DChannels.h>
#include "IllnessUnit.h"
#include "EMGFilter.h"
#include "EDRTitleBar.h"
#include "TextEdit.h"
#include "PseudoLoginWidget.h"
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
//#include "Measurements.h"


using namespace core;

using namespace PluginSubject;


HmmMainWindow::HmmMainWindow() :
    MainWindow(),
    currentVisualizer(nullptr),
    topMainWindow(nullptr),
    analisis(nullptr),
    currentItem(nullptr),
    data(nullptr),
    operations(nullptr),
    dataObserver(new DataObserver(this)),
    visualizersActionsTab(new QWidget()),
    flexiTabWidget(new FlexiTabWidget())
{
    setupUi(this);
    splashScreen()->setPixmap(QPixmap(":/resources/splashscreen/spplash.png"));

    visualizerUsageContext.reset(new HMMVisualizerUsageContext(flexiTabWidget));
    treeUsageContext.reset(new HMMTreeItemUsageContext(flexiTabWidget, this));
    raportsThumbnailsContext.reset(new RaportsThumbnailsContext(flexiTabWidget, this));
    raportsTabContext.reset(new RaportsTabContext(flexiTabWidget, this));
    tabPlaceholder->layout()->addWidget(flexiTabWidget);

    this->setWindowFlags(Qt::FramelessWindowHint);
    itemClickAction.setMainWindow(this);
    setMouseTracking(true);
    IMemoryDataManager* manager = DataManager::getInstance();
    manager->attach(dataObserver.get());
}

void HmmMainWindow::onFocusChange(QWidget * oldWidget, QWidget * newWidget)
{
    //jesli faktycznie contextWidget to ustawiamy aktywny kontekst
    if(isContextWidget(newWidget)){
        setCurrentContext(newWidget);
    }else{
    
        QWidget * widget = getParentContextWidget(newWidget);
    
        if(widget != nullptr){
            setCurrentContext(widget);
            //if(widget->hasFocus() == false){
            //    widget->blockSignals(true);
            //    widget->setFocus();
            //    widget->blockSignals(false);
            //}
        }
    }
}

void HmmMainWindow::init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor )
{
    core::MainWindow::init(pluginLoader, managersAccessor);
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChange(QWidget*,QWidget*)));
    

    trySetStyleByName("hmm");

    this->analisis = new AnalisisWidget(nullptr, this);
    QTreeWidget* treeWidget = this->analisis->getTreeWidget();
    addContext(treeUsageContext);
    addWidgetToContext(treeUsageContext, treeWidget);

    
    addContext(raportsThumbnailsContext);
    //addWidgetToContext(raportsThumbnailsContext, analisis->getRaportsThumbnailList());
    addWidgetToContext(raportsThumbnailsContext, analisis->scrollArea_3);

    
	QTabWidget * dataTabWidget = createNamedObject<QTabWidget>(QString::fromUtf8("dataTabWidget"));
   
    this->data = dataTabWidget;

    this->data->setContentsMargins(0,0,0,0);
    this->analisis->setContentsMargins(0,0,0,0);

    this->operations = new QWidget();
    this->raports = new TextEdit();
    addContext(raportsTabContext);
    addWidgetToContext(raportsTabContext, this->raports);
    button2TabWindow[this->dataButton] = this->data;
    button2TabWindow[this->operationsButton] = this->operations;
    button2TabWindow[this->raportsButton] = this->raports;
    button2TabWindow[this->analisisButton] = this->analisis;
    currentButton = dataButton;

    analisisButton->setEnabled(false);
    raportsButton->setEnabled(false);

    for(auto it = button2TabWindow.begin(); it != button2TabWindow.end(); it++) {
        mainArea->layout()->addWidget(it->second);
        it->second->hide();
        bool c = connect(it->first, SIGNAL(toggled(bool)), this, SLOT(onToolButton(bool)));
        UTILS_ASSERT(c);
    }

    //treeWidget->setColumnCount(2);
    //treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));    
    //QObject::connect(treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    

   /* QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);*/

    topMainWindow = new EDRDockWidgetManager();
    topMainWindow->setTabsPosition(QTabWidget::South);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomMainWindow = new QMainWindow();
    bottomMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    bool c = connect(topMainWindow, SIGNAL(changed()), treeUsageContext.get(), SLOT(refresh()));
    //TODO
    //trzeba wyzanaczaæ max wysokoœæ tak aby nie "wypycha³o" nam timeline!!
    // powinno byæ mniej wiêcej coœ takiego - wysokoœæ dla aktualnej rozdzielczoœci - wysokoœc timeline - wysokoœæ górnego paska - przestrzeñ wolna pomiêdzy tymi elementami w pionie
    // ~ currentRes.h - 270 w przybli¿eniu -> trtzeba dok³¹dnie sprawdziæ i policzyæ
    //topMainWindow->setMaximumHeight(850);
    //topMainWindow->setMinimumHeight(850);

    createFilterTabs();

    QWidget* analisisArea = analisis->getArea();
    QLayout* v = analisisArea->layout() ? analisisArea->layout() : new QGridLayout(analisisArea);
    
    v->setMargin(0);
    v->setContentsMargins(QMargins(0, 0, 0, 0));
    v->addWidget(topMainWindow->asQWidget());
    topMainWindow->asQWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    //TODO
    //zapewniamy timelie minimaln¹, ale wystarczaj¹ca iloœc miejsca
    //trzeba to zgraæ razem z max wysokoœci¹ dla wizualizatorów
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    bottomMainWindow->setMinimumHeight(120); // tymczasowo
    bottomMainWindow->layout()->setMargin(0);
    bottomMainWindow->layout()->setContentsMargins(QMargins(0, 0, 0, 0));
    analisisArea->setLayout(v);

    int i = toolBar->children().size();

    // akcje - Workflow (VDF) i konsola <--- aktualnie œmietnik na inne serwisy i testy

    QMainWindow * actionsMainWindow = new QMainWindow(nullptr);
    QVBoxLayout* layout = new QVBoxLayout();

	IServicePtr dataExplorer;

	for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		IServicePtr service = ServiceManager::getInstance()->getService(i);

        const std::string& name = service->getName();
        // HACK! nie da sie na razie castowac na CommunicationService
        // trzeba by do HmmView dodac zaleznosc od tego pluginu (lub jego bibliotek)
        // Nie da sie jednak includowac z tego poziomu odpowiedniego naglowka 
        // (gdzies po drodze wymagane sa prywane naglowki z Communication)
        // Moze wlasciwe bedzie identyfikowanie po UniqueID.
        //if (name == "Communication") {
		//if (name == "DataExplorer") {
            /*ActionsGroupManager mainWidgetActions;
            QWidget* viewWidget = service->getWidget(&mainWidgetActions);

            ActionsGroupManager controlWidgetActions;
            QWidget* controlWidget = service->getControlWidget(&controlWidgetActions);

            ActionsGroupManager settingsWidgetActions;
            QWidget* settingsWidget = service->getSettingsWidget(&settingsWidgetActions);

            QVBoxLayout *layout = new QVBoxLayout();
            layout->setContentsMargins(0,0,0,0);
            layout->addWidget(settingsWidget);
            layout->addWidget(viewWidget);
            layout->addWidget(controlWidget);
            this->data->setLayout(layout);*/
        if (name == "newTimeline") {
            showTimeline();
        }else if(name != "DataExplorer") {
            ActionsGroupManager mainWidgetActions;
            QWidget* viewWidget = service->getWidget(&mainWidgetActions);

            ActionsGroupManager controlWidgetActions;
            QWidget* controlWidget = service->getControlWidget(&controlWidgetActions);

            ActionsGroupManager settingsWidgetActions;
            QWidget* settingsWidget = service->getSettingsWidget(&settingsWidgetActions);

            layout->addWidget(settingsWidget);
            layout->addWidget(viewWidget);
            layout->addWidget(controlWidget);
        }else{
			//mam dataExplorer - zapamiêtuje i potem go wrzuce do danych
			dataExplorer = service;
		}
    }

    // akcje - Workflow (VDF) i konsola
    EDRWorkflowWidget* widget = new EDRWorkflowWidget();
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);
    widget->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    initializeConsole();
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
    widgetConsole->setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
    layout->addWidget(actionsMainWindow);

    operations->setLayout(layout);

	//QTabWidget * dataTabWidget = core::createNamedObject<QTabWidget>(QString("dataTabWidget"));

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

	//layout = new QVBoxLayout();
	//layout->addWidget(dataTabWidget);
	//this->data->setLayout(layout);

	//chowamy zak³adki jesli tylko jedno Ÿród³o
	if(dataTabWidget->count() == 1){
		QList<QTabBar*> tabBars = dataTabWidget->findChildren<QTabBar*>();
		for(auto it = tabBars.begin(); it != tabBars.end(); ++it){
			if((*it)->parent() == dataTabWidget){
				(*it)->setVisible(false);
				break;
			}
		}
	}    

    //inicjalizacja title bara
    //toolsGroupID = flexiTabWidget->addGroup(QObject::tr("Tools"));
    visualizerGroupID = flexiTabWidget->addGroup(QObject::tr("Visualizer"), QIcon(), false);

    //TODO
    //Tak dlugo jak nie mamy raportow chowamy je w wersji release
    //Podobnie odnosnik do stronki
    #ifndef _DEBUG
        openButton->setVisible(false);
    #endif

    this->showFullScreen();

//#ifndef EDR_PRESENTATION_MODE_
//
//    PseudoLoginWidget login;
//    int ret = login.exec();
//    if(ret == QDialog::Accepted){
//        this->data->show();
//    }else{
//        QApplication::postEvent(qApp, new QCloseEvent());
//        //QTimer::singleShot(500, qApp, SLOT(quit()));
//    }
//#else
//    this->data->show();
//#endif

    
    /*std::string path = core::getResourceString("schemas\\measurementconfs.xml");
    try {
    MeasurementsParser p;
    p.parse(path);
    this->measurements = p.getMeasurments();
    } catch (std::runtime_error& e) {
    LOG_ERROR(e.what());
    }*/
    this->data->show();
}

void HmmMainWindow::setCurrentVisualizerActions(VisualizerWidget * visWidget)
{
    if(currentVisualizer != visWidget){
        return;
    }

    setCurrentContext(visWidget);
}

HmmMainWindow::~HmmMainWindow()
{
    setCurrentContext(nullptr);
    this->analisis->setParent(nullptr);
    delete this->analisis;
    this->data->setParent(nullptr);
    delete data;
    this->operations->setParent(nullptr);
    delete operations;
}

bool HmmMainWindow::isDataItem(QTreeWidgetItem * item)
{
    TreeItemHelper* hmmItem = dynamic_cast<TreeItemHelper*>(item);
    return hmmItem && hmmItem->isDataItem();
}

void HmmMainWindow::createNewVisualizer()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    UTILS_ASSERT(action);
    if (action) {
        createNewVisualizer(action->getItemHelper(), action->getDockSet());
    }
}

void HmmMainWindow::createNewVisualizer( TreeItemHelper* helper, EDRDockWidgetSet* dockSet )
{
    showTimeline();
    VisualizerWidget* w = createDockVisualizer(helper);
    if (dockSet) {
        dockSet->addDockWidget(w);
    } else {
        topMainWindow->autoAddDockWidget( w );
    }
}

void HmmMainWindow::createVisualizerInNewSet()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    UTILS_ASSERT(action);
    if (action) {
        showTimeline();
        VisualizerWidget* w = createDockVisualizer(action->getItemHelper());
        EDRDockWidgetSet* set = new EDRDockWidgetSet(QString(w->getCurrentVisualizer()->getUIName()), topMainWindow);
        set->addDockWidget(w);
        topMainWindow->addDockWidgetSet(set);
    }
}

void HmmMainWindow::removeFromAll()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    removeFromVisualizers(action, false);
}

void HmmMainWindow::removeFromVisualizer()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    removeFromVisualizers(action, true);
}

//void HmmMainWindow::highlightVisualizer()
//{
//    ContextAction* action = qobject_cast<ContextAction*>(sender());
//    UTILS_ASSERT(action);
//    if (action) {
//        for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); it++) {
//            DataItemDescription desc = it->second;
//            if (desc.visualizer.lock() == action->getVisualizer()) {
//                desc.visualizerWidget->setStyleSheet("color: red;");
//            } else {
//                desc.visualizerWidget->setStyleSheet("");
//            }
//
//        }
//    }
//}

void HmmMainWindow::highlightVisualizer(const VisualizerPtr& visualizer )
{
    for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); it++) {
        DataItemDescription desc = it->second;
        if (desc.visualizer.lock() == visualizer) {
            // todo: optymalniej!
            EDRDockWidgetSet* set = topMainWindow->tryGetDockSet(desc.visualizerWidget);
            if (set) {
                topMainWindow->raiseSet(set);
            }
            desc.visualizerWidget->setStyleSheet(QString::fromUtf8(
                "VisualizerWidget > .QWidget {" \
                "border: 2px solid red;"\
                "}"));
        } else {
            desc.visualizerWidget->setStyleSheet(QString());
        }
    }
}


void HmmMainWindow::addToVisualizer()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    UTILS_ASSERT(action);
    if (action) {
        try {
            VisualizerPtr visualizer = action->getVisualizer();
            TreeItemHelper* helper = action->getItemHelper();
            static int counter = 0;
            QString path = QString("Custom_addition...%1").arg(counter++);
            std::vector<core::VisualizerTimeSeriePtr> series;
            helper->getSeries(visualizer, path, series);
			VisualizerManager::getInstance()->createChannel(series, visualizer.get(), path.toStdString());
            //addSeriesToTimeline(series, path, visualizer);

            VisualizerWidget* vw = nullptr;
            for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); it++) {
                DataItemDescription& d = it->second;
                if (d.visualizer.lock() == visualizer) {
                    vw = d.visualizerWidget;
                    break;
                }
            }
            UTILS_ASSERT(vw);
            DataItemDescription desc(visualizer, series, vw);
            items2Descriptions.insert(std::make_pair(helper, desc));

        } catch (std::exception& e) {
            QString message("Unable to add data to visualizer");
            message += "\n";
            message += tr("reason: ");
            message += tr(e.what());
            QMessageBox::warning(this, tr("Warning"), message);
        } catch (...) {
            QMessageBox::warning(this, tr("Warning"), tr("Unable to add data to visualizer"));
        }
    }
}


void HmmMainWindow::onTreeContextMenu(const QPoint & pos)
{
    QTreeWidget* treeWidget = analisis->getTreeWidget();
    currentItem = treeWidget->itemAt(pos);
    treeWidget->setCurrentItem(currentItem);
    if(currentItem == nullptr || isDataItem(currentItem) == false){
        return;
    }

    TreeItemHelper* helper = dynamic_cast<TreeItemHelper*>(currentItem);

    if (helper) {
        QMenu* menu = getContextMenu(treeWidget, helper);
        menu->exec(treeWidget->mapToGlobal(pos));
    }
}

//void HmmMainWindow::onTreeItemClicked( QTreeWidgetItem *item, int column )
//{
//    try {
//        // sprawdzanie, czy pod item jest podpiety jakis obiekt
//        TreeItemHelper* hmmItem = dynamic_cast<TreeItemHelper*>(item);
//        //treeUsageContext->setActiveTreeItem(hmmItem);
//        /*if (hmmItem) {
//            showTimeline();
//            VisualizerWidget* w = createDockVisualizer(hmmItem);
//            topMainWindow->autoAddDockWidget( w );
//        }
//
//        ChildrenVisualizers* cv = dynamic_cast<ChildrenVisualizers*>(item);
//        if (cv) {
//            EDRDockWidgetSet* set = new EDRDockWidgetSet(this);
//            int count = cv->childCount();
//            for (int i = 0; i < count; i++) {
//                TreeItemHelper* hmmItem = dynamic_cast<TreeItemHelper*>(cv->child(i));
//                VisualizerWidget* w = createDockVisualizer(hmmItem);
//                switch (cv->getPolicy()) {
//                case ChildrenVisualizers::Auto:
//                    set->addDockWidget(w);
//                    break;
//                case ChildrenVisualizers::Horizontal:
//                    set->addDockWidget(w, Qt::Horizontal);
//                    break;
//                case ChildrenVisualizers::Vertical:
//                    set->addDockWidget(w, Qt::Vertical);
//                    break;
//                }
//            }
//            int added = set->getNumWidgets();
//        
//            if (added > 0) {
//                showTimeline();
//                set->setMaxWidgetsNumber(added);
//                topMainWindow->addDockWidgetSet(set);
//            } else {
//                delete set;
//            }
//        }*/
//    } catch ( std::exception& e) {
//        LOG_ERROR("Click on tree failed, reason : " << e.what());
//    }
//}
//

void HmmMainWindow::showTimeline()
{
    static bool timelineVisible = false;
    if (timelineVisible == false) {
        for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
            IServicePtr service = ServiceManager::getInstance()->getService(i);

            const std::string& name = service->getName();
            if (name == "newTimeline") {
                ActionsGroupManager mainWidgetActions;
                QWidget* viewWidget = service->getWidget(&mainWidgetActions);

                ActionsGroupManager controlWidgetActions;
                QWidget* controlWidget = service->getControlWidget(&controlWidgetActions);

                ActionsGroupManager settingsWidgetActions;
                QWidget* settingsWidget = service->getSettingsWidget(&settingsWidgetActions);

                EDRDockWidget * widget = new EDRDockWidget();
                widget->setTitleBarWidget(new QWidget());
                widget->setFeatures(QDockWidget::NoDockWidgetFeatures);
                widget->setWidget(controlWidget);
                widget->setAllowedAreas(Qt::BottomDockWidgetArea);
                widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            
                bottomMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);
                timelineVisible = true;
            }
        }
    }
}

void HmmMainWindow::clearTree()
{
    analisis->getTreeWidget()->clear();
}

void HmmMainWindow::addItemToTree( QTreeWidgetItem* item )
{
    analisis->getTreeWidget()->addTopLevelItem(item);
}

void HmmMainWindow::createFilterTabs()
{
    analisis->getTreeWidget()->clear();
    createFilterTab1();
    createFilterTab2();
}

void HmmMainWindow::createFilterTab1()
{

    QPixmap iconAnalog(QString::fromUtf8(":/resources/icons/analogBig.png"));
    QPixmap iconKinetic(QString::fromUtf8(":/resources/icons/kineticBig.png"));
    QPixmap iconKinematic(QString::fromUtf8(":/resources/icons/kinematicBig.png"));
    QPixmap iconVideo(QString::fromUtf8(":/resources/icons/videoBig.png"));

    QPixmap iconEmgSmall(QString::fromUtf8(":/resources/icons/emg1Small.png"));
    QPixmap iconForceSmall(QString::fromUtf8(":/resources/icons/forcesSmall.png"));
    QPixmap iconPowerSmall(QString::fromUtf8(":/resources/icons/powerSmall.png"));
    QPixmap iconGRFSmall(QString::fromUtf8(":/resources/icons/grfSmall.png"));
    QPixmap iconMomentSmall(QString::fromUtf8(":/resources/icons/momentSmall.png"));
    QPixmap iconJointSmall(QString::fromUtf8(":/resources/icons/jointSmall.png"));
    QPixmap iconMarkerSmall(QString::fromUtf8(":/resources/icons/markerSmall.png"));
    QPixmap iconVideoSmall(QString::fromUtf8(":/resources/icons/videoSmall.png"));
    
    DataFilterWidget* filter1 = new DataFilterWidget(tr("ANALOG"), iconAnalog, this);
    DataFilterWidget* filter2 = new DataFilterWidget(tr("KINETIC"), iconKinetic, this);
    DataFilterWidget* filter3 = new DataFilterWidget(tr("KINEMATIC"), iconKinematic, this);
    DataFilterWidget* filter4 = new DataFilterWidget(tr("VIDEO"), iconVideo, this);
    
    //std::vector<TypeInfo> types;
    //types.push_back(typeid(GRFCollection));
    //types.push_back(typeid(GRFChannel));
    //DataFilterPtr typeFilter1(new TypeFilter(types));
    //DataFilterPtr typeFilter2(new TypeFilter(typeid(EMGChannel)));

    IFilterCommandPtr grfCommand(new BuilderFilterCommand(TreeBuilder::createGRFBranch, TreeBuilder::getRootGRFIcon(), TreeBuilder::getGRFIcon()));
    filter1->addFilter(tr("GRF"), grfCommand, &iconGRFSmall);

    QString emgFront = core::getResourceString("images/muscular_front/muscular_front.xml");
    QString emgBack = core::getResourceString("images/muscular_back/muscular_back.xml");

    typedef Vector3DFilterCommand2<EMGChannel, EMGCollection, EMGFilterHelper> EMGCommand;
    NamesDictionary emgNames;
    emgNames["noga1"  ] = std::make_pair("L1",  "Elektroda L1");
    emgNames["noga1R" ] = std::make_pair("L2",  "Elektroda L2");
    emgNames["noga2"  ] = std::make_pair("L3",  "Elektroda L3");
    emgNames["noga2R" ] = std::make_pair("L4",  "Elektroda L4");
    emgNames["noga3"  ] = std::make_pair("L5",  "Elektroda L5");
    emgNames["noga3R" ] = std::make_pair("L6",  "Elektroda L6");
    emgNames["noga4"  ] = std::make_pair("L7",  "Elektroda L7");
    emgNames["noga4R" ] = std::make_pair("R8",  "Elektroda R8");
    emgNames["noga5"  ] = std::make_pair("R9",  "Elektroda R9");
    emgNames["noga5R" ] = std::make_pair("R10", "Elektroda R10");
    emgNames["palec1" ] = std::make_pair("R11", "Elektroda R11");
    emgNames["palec1R"] = std::make_pair("R12", "Elektroda R12");
    emgNames["ramie1" ] = std::make_pair("R13", "Elektroda R13");
    emgNames["ramie1R"] = std::make_pair("R15", "Elektroda R15");
    emgNames["ramie2" ] = std::make_pair("R14", "Elektroda R14");
    emgNames["ramie2R"] = std::make_pair("R16", "Elektroda R16");

    emgNames["noga-tyl1"  ] = std::make_pair("L1",  "Elektroda L1");
    emgNames["noga-tyl1P" ] = std::make_pair("L2",  "Elektroda L2");
    emgNames["noga-tyl2"  ] = std::make_pair("L3",  "Elektroda L3");
    emgNames["noga-tyl2P" ] = std::make_pair("L4",  "Elektroda L4");
    emgNames["noga-tyl3"  ] = std::make_pair("L5",  "Elektroda L5");
    emgNames["noga-tyl3P" ] = std::make_pair("L6",  "Elektroda L6");
    emgNames["noga-tyl4"  ] = std::make_pair("L7",  "Elektroda L7");
    emgNames["noga-tyl4P" ] = std::make_pair("R8",  "Elektroda R8");
    emgNames["noga-tyl5"  ] = std::make_pair("R9",  "Elektroda R9");
    emgNames["noga-tyl5P" ] = std::make_pair("R10", "Elektroda R10");
    emgNames["noga-tyl6"  ] = std::make_pair("R11", "Elektroda R11");
    emgNames["noga-tyl6P" ] = std::make_pair("R12", "Elektroda R12");
    emgNames["ramie-tyl1" ] = std::make_pair("R13", "Elektroda R13");
    emgNames["ramie-tyl1P"] = std::make_pair("R15", "Elektroda R15");
    emgNames["ramie-tyl2" ] = std::make_pair("R14", "Elektroda R14");
    emgNames["ramie-tyl2P"] = std::make_pair("R16", "Elektroda R16");
    /*emgNames["ramie-tyl3" ] = std::make_pair("", "");
    emgNames["ramie-tyl3P"] = std::make_pair("", "");
    emgNames["ramie-tyl4" ] = std::make_pair("", "");
    emgNames["ramie-tyl4P"] = std::make_pair("", "");
    emgNames["ramie-tyl5" ] = std::make_pair("", "");
    emgNames["ramie-tyl5P"] = std::make_pair("", "");
    emgNames["ramie-tyl6" ] = std::make_pair("", "");
    emgNames["ramie-tyl6P"] = std::make_pair("", "");*/
    //emgNames["ramie3" ] = std::make_pair("", "Dioda ");
    //emgNames["ramie3R"] = std::make_pair("", "Dioda ");
    //IFilterCommandPtr emgCommand(new EMGCommand(emgNames, emgFront, emgBack));
    IFilterCommandPtr emgCommand(new BuilderConfiguredFilterCommand<EMGCollection>(TreeBuilder::createEMGBranch, 
        emgNames, emgFront, emgBack, TreeBuilder::getRootEMGIcon(), TreeBuilder::getEMGIcon()));
    filter1->addFilter(tr("EMG"), emgCommand, &iconEmgSmall);

    DataFilterPtr typeFilter3(new TypeFilter(typeid(ForceCollection)));
    DataFilterPtr typeFilter4(new TypeFilter(typeid(MomentCollection)));
    DataFilterPtr typeFilter5(new TypeFilter(typeid(PowerCollection)));

    typedef BuilderConfiguredFilterCommand<MomentCollection> MomentsCommand;
    typedef BuilderConfiguredFilterCommand<ForceCollection> ForcesCommand;
    typedef BuilderConfiguredFilterCommand<PowerCollection> PowerCommand;
    typedef BuilderConfiguredFilterCommand<MarkerCollection> MarkersCommand;
    
    QString pathFront = core::getResourceString("images/skeleton_front/skeleton_front.xml");
    QString pathBack = core::getResourceString("images/skeleton_back/skeleton_back.xml");

    NamesDictionary powersNames;
    powersNames["LAnkle"    ] = std::make_pair("LAnklePower",    "Left Ankle");
    powersNames["LElbow"    ] = std::make_pair("LElbowPower",    "Left Elbow");
    powersNames["LHip"      ] = std::make_pair("LHipPower",      "Left Hip");
    powersNames["LKnee"     ] = std::make_pair("LKneePower",     "Left Knee");
    powersNames["LNeck"     ] = std::make_pair("LNeckPower",     "Left Neck");
    powersNames["LShoulder" ] = std::make_pair("LShoulderPower", "Left Shoulder");
    powersNames["LWaist"    ] = std::make_pair("LWaistPower",    "Left Waist");
    powersNames["LWrist"    ] = std::make_pair("LWristPower",    "Left Wrist");
    powersNames["RAnkle"    ] = std::make_pair("RAnklePower",    "Right Ankle");
    powersNames["RElbow"    ] = std::make_pair("RElbowPower",    "Right Elbow");
    powersNames["RHip"      ] = std::make_pair("RHipPower",      "Right Hip");
    powersNames["RKnee"     ] = std::make_pair("RKneePower",     "Right Knee");
    powersNames["RNeck"     ] = std::make_pair("RNeckPower",     "Right Neck");
    powersNames["RShoulder" ] = std::make_pair("RShoulderPower", "Right Shoulder");
    powersNames["RWaist"    ] = std::make_pair("RWaistPower",    "Right Waist");
    powersNames["RWrist"    ] = std::make_pair("RWristPower",    "Right Wrist");
    //IFilterCommandPtr vFilter(new PowerCommand(powersNames, pathFront, pathBack));
    IFilterCommandPtr powerFilter(new PowerCommand(TreeBuilder::createPowersBranch, 
        powersNames, pathFront, pathBack, TreeBuilder::getRootPowersIcon(), TreeBuilder::getPowersIcon()));
    NamesDictionary momentsNames;
    momentsNames["LAnkle"           ] = std::make_pair("LAnkleMoment",          "Left Ankle");
    momentsNames["LElbow"           ] = std::make_pair("LElbowMoment",          "Left Elbow");
    momentsNames["LGroundReaction"  ] = std::make_pair("LGroundReactionMoment", "Left GRF");
    momentsNames["LHip"             ] = std::make_pair("LHipMoment",            "Left Hip");
    momentsNames["LKnee"            ] = std::make_pair("LKneeMoment",           "Left Knee");
    momentsNames["LNeck"            ] = std::make_pair("LNeckMoment",           "Left Neck");
    momentsNames["LShoulder"        ] = std::make_pair("LShoulderMoment",       "Left Shoulder");
    momentsNames["LWaist"           ] = std::make_pair("LWaistMoment",          "Left Waist");
    momentsNames["LWrist"           ] = std::make_pair("LWristMoment",          "Left Wrist");
    momentsNames["RAnkle"           ] = std::make_pair("RAnkleMoment",          "Right Ankle");
    momentsNames["RElbow"           ] = std::make_pair("RElbowMoment",          "Right Elbow");
    momentsNames["RGroundReaction"  ] = std::make_pair("RGroundReactionMoment", "Right GRF");
    momentsNames["RHip"             ] = std::make_pair("RHipMoment",            "Right Hip");
    momentsNames["RKnee"            ] = std::make_pair("RKneeMoment",           "Right Knee");
    momentsNames["RNeck"            ] = std::make_pair("RNeckMoment",           "Right Neck");
    momentsNames["RShoulder"        ] = std::make_pair("RShoulderMoment",       "Right Shoulder");
    momentsNames["RWaist"           ] = std::make_pair("RWaistMoment",          "Right Waist");
    momentsNames["RWrist"           ] = std::make_pair("RWristMoment",          "Right Wrist");
    
    //IFilterCommandPtr vFilter2(new MomentsCommand(momentsNames, pathFront, pathBack));
    IFilterCommandPtr momentFilter(new MomentsCommand(TreeBuilder::createMomentsBranch, 
        momentsNames, pathFront, pathBack, TreeBuilder::getRootMomentsIcon(), TreeBuilder::getMomentsIcon()));

    NamesDictionary forcesNames;
    forcesNames["LAnkle"          ] = std::make_pair("LAnkleForce",          "Left Ankle");
    forcesNames["LElbow"          ] = std::make_pair("LElbowForce",          "Left Elbow");
    forcesNames["LGroundReaction" ] = std::make_pair("LGroundReactionForce", "Left GRF");
    forcesNames["LHip"            ] = std::make_pair("LHipForce",            "Left Hip");
    forcesNames["LKnee"           ] = std::make_pair("LKneeForce",           "Left Knee");
    forcesNames["LNeck"           ] = std::make_pair("LNeckForce",           "Left Neck");
    forcesNames["LShoulder"       ] = std::make_pair("LShoulderForce",       "Left Shoulder");
    forcesNames["LWaist"          ] = std::make_pair("LWaistForce",          "Left Waist");
    forcesNames["LWrist"          ] = std::make_pair("LWristForce",          "Left Wrist");
    forcesNames["RAnkle"          ] = std::make_pair("RAnkleForce",          "Right Ankle");
    forcesNames["RElbow"          ] = std::make_pair("RElbowForce",          "Right Elbow");
    forcesNames["RGroundReaction" ] = std::make_pair("RGroundReactionForce", "Right GRF");
    forcesNames["RHip"            ] = std::make_pair("RHipForce",            "Right Hip");
    forcesNames["RKnee"           ] = std::make_pair("RKneeForce",           "Right Knee");
    forcesNames["RNeck"           ] = std::make_pair("RNeckForce",           "Right Neck");
    forcesNames["RShoulder"       ] = std::make_pair("RShoulderForce",       "Right Shoulder");
    forcesNames["RWaist"          ] = std::make_pair("RWaistForce",          "Right Waist");
    forcesNames["RWrist"          ] = std::make_pair("RWristForce",          "Right W rist");
    forcesNames["RNormalizedGR"   ] = std::make_pair("RNormalisedGRF",       "Right, normalised GRF");
    forcesNames["LNormalizedGR"   ] = std::make_pair("LNormalisedGRF",       "Left, normalised GRF");
    //IFilterCommandPtr vFilter3(new ForcesCommand(forcesNames, pathFront, pathBack));
    IFilterCommandPtr forceFilter(new ForcesCommand(TreeBuilder::createForcesBranch, 
        forcesNames, pathFront, pathBack, TreeBuilder::getRootForcesIcon(), TreeBuilder::getForcesIcon()));
    
    filter2->addFilter(tr("FORCES"), forceFilter, &iconForceSmall);
    filter2->addFilter(tr("MOMENTS"), momentFilter, &iconMomentSmall);
    filter2->addFilter(tr("POWERS"), powerFilter, &iconPowerSmall);

    //typedef Vector3DFilterCommand2<MarkerChannel, MarkerCollection, NewVector3ItemHelper, true> MarkersCommand;
    NamesDictionary markersNames;
    markersNames["RFHD"] = std::make_pair("RFHD", "Right front of head");
    markersNames["LFHD"] = std::make_pair("LFHD", "Left front of head");
    markersNames["CLAV"] = std::make_pair("CLAV", "Clavicle");
    markersNames["STRN"] = std::make_pair("STRN", "Sternum");
    markersNames["LSHO"] = std::make_pair("LSHO", "Left shoulder");
    markersNames["RSHO"] = std::make_pair("RSHO", "Right shoulder");
    markersNames["LUPA"] = std::make_pair("LUPA", "Left upper arm");
    markersNames["RUPA"] = std::make_pair("RUPA", "Right upper arm");
    markersNames["RELB"] = std::make_pair("RELB", "Right elbow");
    markersNames["LELB"] = std::make_pair("LELB", "Left elbow");
    markersNames["RFRM"] = std::make_pair("RFRM", "Right forearm");
    markersNames["LRFM"] = std::make_pair("LRFM", "Left forearm");
    markersNames["RWRB"] = std::make_pair("RWRB", "Right wrist marker B");
    markersNames["RWRA"] = std::make_pair("RWRA", "Right wrist marker A");
    markersNames["RFIN"] = std::make_pair("RFIN", "Right finger");
    markersNames["LWRA"] = std::make_pair("LWRA", "Left wrist marker A");
    markersNames["LWRB"] = std::make_pair("LWRB", "Left wrist marker B");
    markersNames["LFIN"] = std::make_pair("LFIN", "Left finger");
    markersNames["RASI"] = std::make_pair("RASI", "Right asis");
    markersNames["LASI"] = std::make_pair("LASI", "Left asis");
    markersNames["RTOE"] = std::make_pair("RTOE", "Right toe");
    markersNames["RANK"] = std::make_pair("RANK", "Right ankle");
    markersNames["LANK"] = std::make_pair("LANK", "Left ankle");
    markersNames["LTOE"] = std::make_pair("LTOE", "Left toe");
    markersNames["LTHI"] = std::make_pair("LTHI", "Left thigh");
    markersNames["RTHI"] = std::make_pair("RTHI", "Right thigh");
    markersNames["RKNE"] = std::make_pair("RKNE", "Right knee");
    markersNames["LKNE"] = std::make_pair("LKNE", "Left knee");
    markersNames["RTIB"] = std::make_pair("RTIB", "Right tibial wand marker");
    markersNames["LTIB"] = std::make_pair("LTIB", "Left tibial wand marker");
    markersNames["LPSI"] = std::make_pair("LPSI", "Left psis");
    markersNames["RPSI"] = std::make_pair("RPSI", "Right psis");
    markersNames["LBHD"] = std::make_pair("LBHD", "Left back of head");
    markersNames["RBHD"] = std::make_pair("RBHD", "Right back of head");
    markersNames["RBAK"] = std::make_pair("RBAK", "Right back");
    markersNames["C7"  ] = std::make_pair("C7"  , "Cervical Vertebra");
    markersNames["T10" ] = std::make_pair("T10" , "Thoracic Vertebra");
    QString markersFront = core::getResourceString("images/skeleton_front/skeleton_markers.xml");
    QString markersBack = core::getResourceString("images/skeleton_back/skeleton_markers.xml");
    //IFilterCommandPtr vFilter6(new MarkersCommand(markersNames, markersFront, markersBack));
    IFilterCommandPtr markersFilter(new MarkersCommand(TreeBuilder::createMarkersBranch, 
        markersNames, markersFront, markersBack, TreeBuilder::getRootMarkersIcon(), TreeBuilder::getMarkersIcon()));
    IFilterCommandPtr jointsFilter(new JointsCommand());

    filter3->addFilter(tr("MARKERS"), markersFilter, &iconMarkerSmall);
    filter3->addFilter(tr("JOINTS"), jointsFilter, &iconJointSmall);
    
    IFilterCommandPtr videoFilter(new BuilderFilterCommand(TreeBuilder::createVideoBranch, 
        TreeBuilder::getRootVideoIcon(), TreeBuilder::getVideoIcon()));
    filter4->addFilter(tr("VIDEOS"), videoFilter, &iconVideoSmall);
    
    connect(filter1, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter2, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter3, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connect(filter4, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));

    dataFilterWidgets.push_back(filter1);
    dataFilterWidgets.push_back(filter2);
    dataFilterWidgets.push_back(filter3);
    dataFilterWidgets.push_back(filter4);

    this->analisis->addDataFilterWidget(filter1);
    this->analisis->addDataFilterWidget(filter2);
    this->analisis->addDataFilterWidget(filter3);
    this->analisis->addDataFilterWidget(filter4);

    for (auto it = dataFilterWidgets.begin(); it != dataFilterWidgets.end(); it++) {
        for (int i = 0; i < (*it)->getNumEntries(); i++) {
            connect((*it)->getEntry(i), SIGNAL(onFilterClicked(FilterEntryWidget*)), this, SLOT(filterClicked(FilterEntryWidget*)));
        }
    }
}

void HmmMainWindow::createFilterTab2()
{
    core::IMemoryDataManager * memoryDataManager = managersAccessor->getMemoryDataManager();

    QPixmap iconKinetic(QString::fromUtf8(":/resources/icons/kineticBig.png"));
    QPixmap iconIllness(QString::fromUtf8(":/resources/icons/jed.chorobowe.png"));
    QPixmap iconEndo(QString::fromUtf8(":/resources/icons/po_endoplastyce.png"));
    QPixmap iconStroke(QString::fromUtf8(":/resources/icons/po_udarze.png"));
    QPixmap iconSpine(QString::fromUtf8(":/resources/icons/zwyrodnienia.png"));
   
    DataFilterWidget* filter1 = new DataFilterWidget(tr("ILLNESS"), iconIllness, this);
    DataFilterWidget* filter2 = new DataFilterWidget(tr("MULTI"), iconKinetic, this);
    DataFilterWidget* filter3 = new DataFilterWidget(tr("MULTI"), iconKinetic, this);
    DataFilterWidget* filter4 = new DataFilterWidget(tr("MULTI"), iconKinetic, this);

    IFilterCommandPtr endo(new Endo());
    IFilterCommandPtr stroke(new Stroke());
    IFilterCommandPtr spine(new Spine());

    filter1->addFilter(tr("Endo"), endo, &iconEndo);
    filter1->addFilter(tr("Stroke"), stroke, &iconStroke);
    filter1->addFilter(tr("Spine"), spine, &iconSpine);

    DataFilterPtr typeFilter1(new TypeFilter(typeid(GRFChannel)));
    DataFilterPtr typeFilter2(new TypeFilter(typeid(EMGChannel)));

    IFilterCommandPtr multi1(new MultiChartCommand<ForceCollection>());
    IFilterCommandPtr multi2(new MultiChartCommand<MomentCollection>());
    IFilterCommandPtr multi3(new MultiChartCommand<PowerCollection>());

    filter4->addFilter("Force", multi1);
    filter4->addFilter("Moment", multi2);
    filter4->addFilter("Power", multi3);

    bool connectionOK = connect(filter1, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connectionOK = connect(filter2, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connectionOK = connect(filter3, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));
    connectionOK = connect(filter4, SIGNAL(activated(bool)), this, SLOT(filterGroupActivated(bool)));

    dataFilterWidgets.push_back(filter1);
    dataFilterWidgets.push_back(filter2);
    dataFilterWidgets.push_back(filter3);
    dataFilterWidgets.push_back(filter4);

    this->analisis->addDataFilterWidget(filter1);
    this->analisis->addDataFilterWidget(filter2);
    this->analisis->addDataFilterWidget(filter3);
    this->analisis->addDataFilterWidget(filter4);
}

const std::vector<SessionConstPtr>& HmmMainWindow::getCurrentSessions()
{
    currentSessions = core::queryDataPtr(DataManager::getInstance());    
    return currentSessions;
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
            refreshTree();
        } 
    }
}

void HmmMainWindow::onToolButton(bool checked)
{
    QToolButton* button = qobject_cast<QToolButton*>(sender());

    //zawsze ktorys button z podstawowych musi byc klikniety/checked
    if(currentButton == button){
        if(checked == false){
            button->blockSignals(true);
            button->setChecked(true);
            button->blockSignals(false);
        }
    }else{
        currentButton->blockSignals(true);
        currentButton->setChecked(false);
        currentButton->blockSignals(false);
        button2TabWindow[currentButton]->hide();
        button2TabWindow[button]->show();
        currentButton = button;
    }
    
    mainArea->layout()->update();
}


void HmmMainWindow::visualizerDestroyed(QObject * visualizer)
{
    visualizerUsageContext->setCurrentContextWidgetDestroyed(true);
    removeContext(qobject_cast<QWidget*>(visualizer));
    visualizerUsageContext->setCurrentContextWidgetDestroyed(false);
}

 VisualizerWidget* HmmMainWindow::createDockVisualizer( TreeItemHelper* hmmItem )
{
    std::stack<QString> pathStack;
    QTreeWidgetItem * pomItem = hmmItem;

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

    static int incr = 0;
    QString suffix = QString("/id_%1").arg(incr++);
    path += suffix;

    VisualizerManager* visualizerManager = VisualizerManager::getInstance();
    VisualizerPtr visualizer = hmmItem->createVisualizer();
    visualizer->getOrCreateWidget();
    // todo : zastanowic sie nad bezpieczenstwem tej operacji
    connect(visualizer.get(), SIGNAL(printTriggered(const QPixmap&)), this, SLOT(addToRaports(const QPixmap&)));
    VisualizerWidget* visualizerDockWidget = new VisualizerWidget(visualizer);
    visualizerDockWidget->setPermanent(false);
    visualizerDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    visualizerDockWidget->setVisualizerIconVisible(false);
    visualizerDockWidget->setVisualizerSwitchEnable(false);
    visualizerDockWidget->setVisualizerSwitchVisible(false);
    visualizerDockWidget->setSourceVisible(false);

    EDRTitleBar * titleBar = supplyWithEDRTitleBar(visualizerDockWidget);

    std::vector<VisualizerTimeSeriePtr> series;
    hmmItem->getSeries(visualizer, path, series);
    visualizer->getWidget()->setFocusPolicy(Qt::ClickFocus);
    //visualizer->getWidget()->setFocusProxy(visualizerDockWidget);
    visualizerDockWidget->setFocusProxy(visualizer->getWidget());

    DataItemDescription desc(visualizer, series, visualizerDockWidget);
    items2Descriptions.insert(std::make_pair(hmmItem, desc));

    connect(visualizerDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(visualizerDestroyed(QObject *)));
	VisualizerManager::getInstance()->createChannel(series, visualizer.get(), path.toStdString());
    //addSeriesToTimeline(series, path, visualizer);

    //kontekst wizualizatora!!
    addContext(visualizerUsageContext);
    addWidgetToContext(visualizerUsageContext, visualizerDockWidget);

    return visualizerDockWidget;
}

 /*void HmmMainWindow::addSeriesToTimeline( const std::vector<core::VisualizerTimeSeriePtr> &series, const QString &path, const VisualizerPtr &visualizer )
 {
 TimelinePtr timeline = core::queryServices<ITimelineService>(ServiceManager::getInstance());
 if(timeline != nullptr) {
 if (series.size() == 1 && series[0] != nullptr) {
 VisualizerChannelPtr channel(new VisualizerChannel(path.toStdString(), visualizer.get(), series[0]));
 try{
 timeline->addChannel(path.toStdString(), channel);
 }catch(...){
 LOG_ERROR("Could not add channel to timeline!");
 }
 } else {
 VisualizerMultiChannel::SeriesWidgets visSeries;
 for (int i = 0; i < series.size(); i++) {
 auto timeSerie = series[i];
 if(timeSerie != nullptr){
 visSeries.push_back(timeSerie);
 }
 }
 if (visSeries.size() > 0) {
 VisualizerMultiChannelPtr multi(new VisualizerMultiChannel(path.toStdString(), visualizer.get(), visSeries));
 try {
 timeline->addChannel(path.toStdString(), multi);
 } catch ( const std::exception & e){
 LOG_ERROR("Could not add multichannel to timeline! Reason: " << e.what());
 } catch (...) {
 LOG_ERROR("Could not add multichannel to timeline! UNKNOWN REASON");
 }
 }
 }
 }
 }*/

 void HmmMainWindow::refreshTree()
 {
     std::vector<SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance());
     currentSessions = sessions;
     QTreeWidgetItem* item = TreeBuilder::createTree(tr("Active Data"), sessions);
     
     QTreeWidget* tree = analisis->getTreeWidget();
     tree->clear();
     tree->addTopLevelItem(item);
     item->setExpanded(true);

     QFont font = item->font(0);
     //static int fontSize = font.pointSize();
     //font.setPointSize(fontSize + 4);
     font.setPointSize(12);
     item->setFont(0, font);

     for (int i = 0; i < item->childCount(); i++) {
         QTreeWidgetItem* child = item->child(i);
         child->setExpanded(true);

         QFont font = item->font(0);
         font.setPointSize(14);
         item->setFont(0, font);
     }
 }

 QMenu* HmmMainWindow::getContextMenu( QWidget* parent, TreeItemHelper* helper )
 {
     dropUnusedElements(items2Descriptions);
     //helper->getTypeInfos()
     QMenu * menu = new QMenu(parent);
     QAction * addNew = new ContextAction(helper, menu);
     addNew->setText(tr("Create new visualizer"));
     menu->addAction(addNew);
     menu->addSeparator();
     connect(addNew, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));
     connect(addNew, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

     if (dynamic_cast<NewChartItemHelper*>(helper)) {
         QMenu* multiMenu = new QMenu(tr("Multi chart"), menu);
         QAction * multi = new ContextAction(helper, menu);
         multi->setText(tr("All from session"));
         multiMenu->addAction(multi);
         connect(multi, SIGNAL(triggered()), this, SLOT(allFromSession()));
         connect(multi, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));
         menu->addMenu(multiMenu);
     }

     if (dynamic_cast<NewVector3ItemHelper*>(helper)) {
         QMenu* multiMenu = new QMenu(tr("Multi chart"), menu);
         QAction * multiX = new ContextAction(helper, menu);
         multiX->setText(tr("All X from session"));
         multiMenu->addAction(multiX);
         connect(multiX, SIGNAL(triggered()), this, SLOT(allXFromSession()));
         connect(multiX, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * multiY = new ContextAction(helper, menu);
         multiY->setText(tr("All Y from session"));
         multiMenu->addAction(multiY);
         connect(multiY, SIGNAL(triggered()), this, SLOT(allYFromSession()));
         connect(multiY, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * multiZ = new ContextAction(helper, menu);
         multiZ->setText(tr("All Z from session"));
         multiMenu->addAction(multiZ);
         connect(multiZ, SIGNAL(triggered()), this, SLOT(allZFromSession()));
         connect(multiZ, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         multiMenu->addSeparator();

         QAction * normL = new ContextAction(helper, menu);
         normL->setText(tr("Left Step normalization"));
         multiMenu->addAction(normL);
         connect(normL, SIGNAL(triggered()), this, SLOT(normalizedLeftChart()));
         connect(normL, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normR = new ContextAction(helper, menu);
         normR->setText(tr("Right Step normalization"));
         multiMenu->addAction(normR);
         connect(normR, SIGNAL(triggered()), this, SLOT(normalizedRightChart()));
         connect(normR, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normAllL = new ContextAction(helper, menu);
         normAllL->setText(tr("Left Step normalization - all from session"));
         multiMenu->addAction(normAllL);
         connect(normAllL, SIGNAL(triggered()), this, SLOT(allLeftNormalized()));
         connect(normAllL, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normAllR = new ContextAction(helper, menu);
         normAllR->setText(tr("Right Step normalization - all from session"));
         multiMenu->addAction(normAllR);
         connect(normAllR, SIGNAL(triggered()), this, SLOT(allRightNormalized()));
         connect(normAllR, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));


         menu->addMenu(multiMenu);
     }

     QMenu* addTo = new QMenu(tr("Add to:"), menu);
     connect(addTo, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
     connect(addTo, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
     BOOST_FOREACH(EDRDockWidgetSet* set, topMainWindow->getDockSet()) {
         QMenu* group = new QMenu(set->windowTitle(), menu);

         BOOST_FOREACH(EDRDockWidget* dock, set->getDockWidgets()) {
             VisualizerWidget* vw = dynamic_cast<VisualizerWidget*>(dock);
             if (vw ) {
                 VisualizerPtr visualizer = vw->getCurrentVisualizer();
                 DataManager* dataManager = DataManager::getInstance();
                 bool compatibile = false;
                 for (int idx = 0; idx < visualizer->getNumInputs(); idx++) {
                     std::vector<TypeInfo> types = helper->getTypeInfos();
                     for (int h = 0; h < types.size(); h++) {
                         if (dataManager->isTypeCompatible(visualizer->getInputType(idx), types[h])) {
                             compatibile = true;
                             break;
                         }
                     }

                 }

                 if (compatibile) {
                     QAction* addAction = new ContextAction(helper, group, visualizer);
                     addAction->setText(vw->windowTitle());
                     connect(addAction, SIGNAL(triggered()), this, SLOT(addToVisualizer()));
                     connect(addAction, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));
                     //connect(addAction, SIGNAL(hovered()), this, SLOT(highlightVisualizer()));
                     group->addAction(addAction);
                 }
             }
         }

         if (group->actions().size()) {
             addTo->addMenu(group);
         } else  {
             delete group;
         }
     }
     if (addTo->actions().size()) {
         menu->addMenu(addTo);
     }


     if(items2Descriptions.find(helper) != items2Descriptions.end()) {
         QMenu* removeFrom = new QMenu(tr("Remove from:"), menu);
         connect(removeFrom, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
         connect(removeFrom, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
         menu->addMenu(removeFrom);

         auto range = items2Descriptions.equal_range(helper);
         for (auto it = range.first; it != range.second; it++) {
             DataItemDescription desc = it->second;
             QAction * action = new ContextAction(helper, menu, desc.visualizer.lock());
             action->setText(desc.visualizer.lock()->getUIName());
             connect(action, SIGNAL(triggered()), this, SLOT(removeFromVisualizer()));
             connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
             removeFrom->addAction(action);
         }

         QAction* all = new ContextAction(helper, menu);
         all->setText("All visualizers");
         connect(all, SIGNAL(triggered()), this, SLOT(removeFromAll()));
         connect(all, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
         removeFrom->addSeparator();
         removeFrom->addAction(all);
     }    

     QMenu* createIn = new QMenu(tr("Create in:"), menu);
     BOOST_FOREACH(EDRDockWidgetSet* set, topMainWindow->getDockSet()) {
         if (set->isAdditionPossible()) {
             QAction* action = new ContextAction(helper, menu, VisualizerPtr(), set);
             action->setText(set->windowTitle());
             createIn->addAction(action);
             connect(action, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));
             connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
         }
     }
     QAction* newGroup = new ContextAction(helper, menu);
     newGroup->setText(tr("New group"));
     createIn->addAction(newGroup);
     connect(newGroup, SIGNAL(triggered()), this, SLOT(createVisualizerInNewSet()));
     connect(newGroup, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
     if (createIn->actions().size()) {
         menu->addMenu(createIn);
     }

     return menu;
 }

 void HmmMainWindow::menuHighlightVisualizer( QAction* action )
 {
     ContextAction* context = qobject_cast<ContextAction*>(action);
     if (context) {
         highlightVisualizer(context->getVisualizer());

     } else {
         highlightVisualizer(VisualizerPtr());
     }
 }

 void HmmMainWindow::dropUnusedElements( std::multimap<TreeItemHelper*, DataItemDescription>& multimap )
 {
     typedef std::multimap<TreeItemHelper*, DataItemDescription> mmap;
     for (auto it = multimap.begin(); it != multimap.end(); ) {
         DataItemDescription& desc = it->second;
         bool emptySerie = false;
         for (auto s = desc.series.begin(); s != desc.series.end(); s++) {
             if ((*s).use_count() == 0) {
                 emptySerie = true;
                 break;
             }
         }
         if (emptySerie || desc.visualizer.use_count() == 0) {
             auto toErase = it;
             it++;
             multimap.erase(toErase);
         } else {
             it++;
         }

     }
 }

 void HmmMainWindow::addToRaports( const QPixmap& pixmap )
 { 
     analisis->tabWidget->setCurrentWidget(analisis->raportsTab);
     if (!(pixmap.width() && pixmap.height())) {
         return;
     }
     const int maxH = 128;
     const int maxW = 128;
     QWidget* list = analisis->getRaportsThumbnailList();
     QLabel* thumb = new QLabel();

     if (pixmap.width() > pixmap.height()) {
         int newHeight = static_cast<int>(1.0 * maxH * pixmap.height() / pixmap.width());
         thumb->setFixedSize(maxW, newHeight);
     } else {
         int newWidth = static_cast<int>(1.0 * maxW * pixmap.width() / pixmap.height());
         thumb->setFixedSize(newWidth, maxH);
     }

     thumb->setScaledContents(true);
     thumb->setPixmap(pixmap);

     QGridLayout* grid = qobject_cast<QGridLayout*>(list->layout());
     if (grid) {
         int size = list->children().size() - 1;
         int x = size % 2;
         int y = size / 2;
         grid->addWidget(thumb, y, x);
     } else {
        list->layout()->addWidget(thumb);
     }
 }

 void HmmMainWindow::createRaport( const QString& html )
 {
     raportsButton->setEnabled(true);
     raportsButton->click();
     raports->setFocus();
     raports->setHtml(html);
 }

 void HmmMainWindow::removeFromVisualizers( ContextAction* action, bool once)
 {
     UTILS_ASSERT(action);
     if (action) {
         typedef std::multimap<TreeItemHelper*, DataItemDescription> mmap;
         std::list<mmap::iterator> toErase;
         auto range = items2Descriptions.equal_range(action->getItemHelper());
         for (auto it = range.first; it != range.second; ) {
             // na razie kopia, w przeciwnym razie jest problem z usuwaniem.
             DataItemDescription desc = it->second;
             // jesli w akcji nie przechowujemy informacji o konkretnym wizualizatorze
             // to znaczy, ze chcemy usunac dane z wszystkich wizualizatorw
             if (action->getVisualizer() == nullptr || desc.visualizer.lock() == action->getVisualizer()) {
                 auto toErase = it; it++;
                 items2Descriptions.erase(toErase);
                 for (int i = 0; i < desc.series.size(); i++) {
                     desc.visualizer.lock()->removeSerie(desc.series[i].lock());
                 }
                 if (desc.visualizer.lock()->getDataSeries().size() == 0) {
                     desc.visualizerWidget->close();
                     delete desc.visualizerWidget;
                 }

                 
                 if (once) {
                    break;
                 }
             } else {
                 it++;
             }

         }
     }
 }

 void HmmMainWindow::filterClicked( FilterEntryWidget* entry )
 {
     for (auto it = dataFilterWidgets.begin(); it != dataFilterWidgets.end(); it++) {
         (*it)->uncheckEntries(entry);
     }
 }

 void HmmMainWindow::allFromSession()
 {
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewChartItemHelper* helper = dynamic_cast<NewChartItemHelper*>(a->getItemHelper());
     UTILS_ASSERT(helper);
    
     if (helper) {
         NewMultiserieHelper::ChartWithEventsCollection toVisualize;
         SessionConstPtr s = helper->getMotion()->getSession();
         Motions motions;
         s->getMotions(motions);

         for (auto itMotion = motions.begin(); itMotion != motions.end(); itMotion++) {
             std::vector<core::ObjectWrapperConstPtr> wrappers;
             (*itMotion)->getWrappers(wrappers, typeid(ScalarChannel), false);
             EventsCollectionConstPtr events;
             if ((*itMotion)->hasObjectOfType(typeid(C3DEventsCollection))) {
                 auto w = (*itMotion)->getWrapperOfType(typeid(C3DEventsCollection));
                 events = w->get();
             }

             for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
                 if ((*it)->getName() == helper->getWrapper()->getName()) {
                     toVisualize.push_back(std::make_pair(*it, events));
                 }
             }
         }
         NewMultiserieHelper* multi = new NewMultiserieHelper(toVisualize);
         createNewVisualizer(multi);
         delete multi;
     }
 }


 void HmmMainWindow::allLeftNormalized()
 {
     auto context = c3dlib::C3DParser::IEvent::Left;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     if (helper) {
         createNormalizedFromAll(helper, context);

     }
 }

 void HmmMainWindow::allRightNormalized()
 {
     auto context = c3dlib::C3DParser::IEvent::Right;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     if (helper) {
         createNormalizedFromAll(helper, context);

     }
 }

 void HmmMainWindow::normalizedLeftChart()
 {
     auto context = c3dlib::C3DParser::IEvent::Left;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     if (helper) {
         createNormalized(helper, context);

     }
 }

 void HmmMainWindow::normalizedRightChart()
 {
     auto context = c3dlib::C3DParser::IEvent::Right;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     if (helper) {
         createNormalized(helper, context);
     }
 }

 void HmmMainWindow::allXFromSession()
 {
     int channelNo = 0;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allYFromSession()
 {
     int channelNo = 1;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allZFromSession()
 {
     int channelNo = 2;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelper* helper = dynamic_cast<NewVector3ItemHelper*>(a->getItemHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allTFromSession( NewVector3ItemHelper* helper, int channelNo )
 {
     UTILS_ASSERT(helper);

     if (helper) {
         NewMultiserieHelper::ChartWithEventsCollection toVisualize;
         SessionConstPtr s = helper->getMotion()->getSession();
         Motions motions;
         s->getMotions(motions);

         for (auto it = motions.begin(); it != motions.end(); it++) {
             std::vector<core::ObjectWrapperConstPtr> wrappers;
             (*it)->getWrappers(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

             EventsCollectionConstPtr events;
             if ((*it)->hasObjectOfType(typeid(C3DEventsCollection))) {
                 auto w = (*it)->getWrapperOfType(typeid(C3DEventsCollection));
                 events = w->get();
             }

             for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
                 VectorChannelCollectionConstPtr collection = (*it)->get();
                 int count = collection->getNumChannels();
                 for (int i = 0; i < count; i++) {
                     VectorChannelConstPtr channel = collection->getChannel(i);
                     VectorChannelConstPtr helperChannel = helper->getWrapper()->get();
                     if (channel->getName() == helperChannel->getName()) {
                        ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                        core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                        wrapper->set(reader);
                        int no = toVisualize.size();
                        std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
                        wrapper->setName  (prefix + boost::lexical_cast<std::string>(no));
                        wrapper->setSource((*it)->getSource() + boost::lexical_cast<std::string>(no));
                        toVisualize.push_back(std::make_pair(wrapper, events));
                     }
                     
                 }
                
             }
         }
         NewMultiserieHelper* multi = new NewMultiserieHelper(toVisualize);
         createNewVisualizer(multi);
         delete multi;
     }
 }

 void HmmMainWindow::createNormalized( NewVector3ItemHelper* helper, c3dlib::C3DParser::IEvent::Context context )
 {
     std::vector<core::ObjectWrapperConstPtr> toVisualize;
     MotionConstPtr motion = helper->getMotion();
     EventsCollectionConstPtr events;
     std::vector<FloatPairPtr> segments;
     if (motion->hasObjectOfType(typeid(C3DEventsCollection))) {
         auto w = motion->getWrapperOfType(typeid(C3DEventsCollection));
         events = w->get();
         segments = getTimeSegments(events, context);
     }
     std::map<ObjectWrapperConstPtr, QColor> colorMap;
     VectorChannelConstPtr channel = helper->getWrapper()->get();
     for (int j = 0; j != segments.size(); j++) {
         FloatPairPtr segment = segments[j];
         for (int channelNo = 0; channelNo <= 2; channelNo++) {
             ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
             ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
             core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
             wrapper->set(normalized);
             int no = toVisualize.size();
             std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
             colorMap[wrapper] = channelNo == 0 ? QColor(255, 0, 0) : (channelNo == 1 ? QColor(0, 255, 0) : QColor(0, 0, 255));
             wrapper->setName  (prefix + ":" + boost::lexical_cast<std::string>(j));
             wrapper->setSource(helper->getWrapper()->getSource() + boost::lexical_cast<std::string>(no));
             toVisualize.push_back(wrapper);
         }
     }
     NewMultiserieHelper* multi = new NewMultiserieHelper(toVisualize);
     multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));
     createNewVisualizer(multi);
     delete multi;
 }

 void HmmMainWindow::createNormalizedFromAll( NewVector3ItemHelper* helper, c3dlib::C3DParser::IEvent::Context context )
 {
     std::vector<core::ObjectWrapperConstPtr> toVisualize;
     SessionConstPtr s = helper->getMotion()->getSession();
     Motions motions;
     s->getMotions(motions);

     std::map<ObjectWrapperConstPtr, QColor> colorMap;
     for (auto it = motions.begin(); it != motions.end(); it++) {
         std::vector<core::ObjectWrapperConstPtr> wrappers;
         (*it)->getWrappers(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

         EventsCollectionConstPtr events;
         std::vector<FloatPairPtr> segments;
         if ((*it)->hasObjectOfType(typeid(C3DEventsCollection))) {
             auto w = (*it)->getWrapperOfType(typeid(C3DEventsCollection));
             events = w->get();
             segments = getTimeSegments(events, context);
         }

         for (auto it = wrappers.begin(); it != wrappers.end(); it++) {
             VectorChannelCollectionConstPtr collection = (*it)->get();
             int count = collection->getNumChannels();
             for (int i = 0; i < count; i++) {
                 VectorChannelConstPtr channel = collection->getChannel(i);
                 VectorChannelConstPtr helperChannel = helper->getWrapper()->get();
                 if (channel->getName() == helperChannel->getName()) {
                     
                     int r = rand() % 200;
                     int g = rand() % 200;
                     int b = rand() % 200;
                     QColor colorX(r + 55,g , b);
                     QColor colorY(r, g + 55, b);
                     QColor colorZ(r, g, b + 55);
                     for (int j = 0; j != segments.size(); j++) {
                         FloatPairPtr segment = segments[j];

                         for (int channelNo = 0; channelNo <= 2; channelNo++) {
                             ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                             ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
                             core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                             wrapper->set(normalized);
                             colorMap[wrapper] = channelNo == 0 ? colorX : (channelNo == 1 ? colorY : colorZ);
                             int no = toVisualize.size();
                             std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
                             wrapper->setName  (prefix + boost::lexical_cast<std::string>(i) + ":" + boost::lexical_cast<std::string>(j));
                             wrapper->setSource((*it)->getSource() + boost::lexical_cast<std::string>(no));
                             toVisualize.push_back(wrapper);
                         }
                     }
                 }

             }

         }
     }
     NewMultiserieHelper* multi = new NewMultiserieHelper(toVisualize);
     multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));
     createNewVisualizer(multi);
     delete multi;
 }



 void HmmMainWindow::DataObserver::update( const core::IMemoryDataManager * subject )
 {
     std::vector<MotionConstPtr> motions = core::queryDataPtr(DataManager::getInstance());
     int count = motions.size();

     if(count > 0){
         //if (motionsCount == 0) {
             hmm->analisisButton->setEnabled(true);			 
         //}
     }else{
         //if (motionsCount != 0) {
             hmm->analisisButton->setEnabled(false);
         //}
     }

	 hmm->refreshTree();

     /*if (motionsCount != count) {
         hmm->refreshTree();
         motionsCount = count;
     }*/
 }
