#include "hmmPCH.h"
#include "HmmMainWindow.h"
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
#include "IllnessUnit.h"

using namespace core;


HmmMainWindow::HmmMainWindow() :
	MainWindow(),
	currentVisualizer(nullptr),
	topMainWindow(nullptr),
	analisis(nullptr),
    currentItem(nullptr),
    tests(nullptr),
    operations(nullptr)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
    itemClickAction.setMainWindow(this);
}


void HmmMainWindow::init( core::PluginLoader* pluginLoader, core::IManagersAccessor * managersAccessor )
{
	core::MainWindow::init(pluginLoader, managersAccessor);
    
    QString locale = QLocale::system().name();
    QTranslator translator;
    translator.load(QString("lang_") + locale);
    if (translator.isEmpty()) {
        translator.load("lang_en_EN");
    }
    qApp->installTranslator(&translator);
    
	setupUi(this);

    this->analisis = new AnalisisWidget(nullptr);
    this->tests = new QWidget(nullptr);
    this->operations = new QWidget(nullptr);
    this->raports = new QWidget(nullptr);
	//topButton->setFixedWidth(0);
	//bottomButton->setFixedWidth(0);

    button2TabWindow[this->testsButton] = this->tests;
    button2TabWindow[this->operationsButton] = this->operations;
    button2TabWindow[this->raportsButton] = this->raports;
    button2TabWindow[this->analisisButton] = this->analisis;

    /*QHBoxLayout* grid = new QHBoxLayout;
    grid->setMargin(0);
    QMargins m(1, 1, 1, 1);
    grid->setContentsMargins(m);
    mainArea->setLayout(grid);*/
    for(auto it = button2TabWindow.begin(); it != button2TabWindow.end(); it++) {
        mainArea->layout()->addWidget(it->second);
        it->second->hide();
        bool c = connect(it->first, SIGNAL(clicked()), this, SLOT(onToolButton()));
        UTILS_ASSERT(c);
    }

    this->tests->show();

    

	//connect(this->fontSlider, SIGNAL(valueChanged(int)), this, SLOT(setFont(int)));
	//connect(this->layoutTopSlider, SIGNAL(valueChanged(int)), this, SLOT(setTop(int)));
	//connect(this->layoutBottomSlider, SIGNAL(valueChanged(int)), this, SLOT(setBottom(int)));
	
	this->showFullScreen();
    QTreeWidget* treeWidget = this->analisis->getTreeWidget();
    //treeWidget->setColumnCount(2);
    treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));    
	QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    

    QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    topMainWindow = new EDRDockWidgetManager(this);
    topMainWindow->setTabsPosition(QTabWidget::North);
    bottomMainWindow = new QMainWindow(this);
    bottomMainWindow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    createFilterTabs();


    //splitter->addWidget(topMainWindow->asQWidget());
    //splitter->addWidget(bottomMainWindow);

    QWidget* analisisArea = analisis->getArea();
    QGridLayout* v = new QGridLayout(analisisArea);
    //v->addWidget(splitter);
    v->addWidget(topMainWindow->asQWidget());
    topMainWindow->asQWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    analisisArea->setLayout(v);

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
			this->tests->setLayout(layout);
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

	operations->setLayout(layout);
}

HmmMainWindow::~HmmMainWindow()
{
    this->analisis->setParent(nullptr);
    delete this->analisis;
    this->tests->setParent(nullptr);
    delete tests;
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
    if(emptyVisualizersPool.empty() == false);
}

void HmmMainWindow::onTreeContextMenu(const QPoint & pos)
{
    QTreeWidget* treeWidget = analisis->getTreeWidget();
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
		//d->setWindowFlags(Qt::Tool);
		d->start(directory);
		delete d;
	}

	std::vector<SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance());
    currentSessions = sessions;
    QTreeWidgetItem* item = TreeBuilder::createTree("Sessions", sessions);
    analisis->getTreeWidget()->addTopLevelItem(item);
}

void HmmMainWindow::setFont( int size )
{
	/*const QFont& font = tabWidget->font();
	QFont newFont(font);
	newFont.setPointSize(size);
	tabWidget->setFont(newFont);*/
}

void HmmMainWindow::setTop( int size )
{
	//topButton->setFixedHeight(size);
	//topButton->setFixedWidth(0);
}

void HmmMainWindow::setBottom( int size )
{
	/*bottomButton->setFixedHeight(size);
	bottomButton->setFixedWidth(0);*/
}

void HmmMainWindow::onTreeItemClicked( QTreeWidgetItem *item, int column )
{
    // sprawdzanie, czy pod item jest podpiety jakis obiekt
    TreeItemHelper* hmmItem = dynamic_cast<TreeItemHelper*>(item);
    
    if (hmmItem) {
        showTimeline();
        VisualizerWidget* w = createDockVisualizer(hmmItem);
        topMainWindow->autoAddDockWidget( w );
    }

    ChildrenVisualizers* cv = dynamic_cast<ChildrenVisualizers*>(item);
    if (cv) {
        EDRDockWidgetSet* set = new EDRDockWidgetSet(this);
        int count = cv->childCount();
        for (int i = 0; i < count; i++) {
            TreeItemHelper* hmmItem = dynamic_cast<TreeItemHelper*>(cv->child(i));
            VisualizerWidget* w = createDockVisualizer(hmmItem);
            switch (cv->getPolicy()) {
            case ChildrenVisualizers::Auto:
                set->addDockWidget(w);
                break;
            case ChildrenVisualizers::Horizontal:
                set->addDockWidget(w, Qt::Horizontal);
                break;
            case ChildrenVisualizers::Vertical:
                set->addDockWidget(w, Qt::Vertical);
                break;
            }
        }
        int added = set->getNumWidgets();
        
        if (added > 0) {
            showTimeline();
            set->setMaxWidgetsNumber(added);
            topMainWindow->addDockWidgetSet(set);
        } else {
            delete set;
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
    core::IMemoryDataManager * memoryDataManager = managersAccessor->getMemoryDataManager();
    QPixmap iconAnalog(core::getResourceString("icons/analogBig.png"));
    QPixmap iconKinetic(core::getResourceString("icons/kineticBig.png"));
    QPixmap iconKinematic(core::getResourceString("icons/kinematicBig.png"));
    QPixmap iconVideo(core::getResourceString("icons/videoBig.png"));

    QPixmap iconEmgSmall(core::getResourceString("icons/emg1Small.png"));
    QPixmap iconForceSmall(core::getResourceString("icons/forcesSmall.png"));
    QPixmap iconPowerSmall(core::getResourceString("icons/powerSmall.png"));
    QPixmap iconGRFSmall(core::getResourceString("icons/grfSmall.png"));
    QPixmap iconMomentSmall(core::getResourceString("icons/momentSmall.png"));
    QPixmap iconJointSmall(core::getResourceString("icons/jointSmall.png"));
    QPixmap iconMarkerSmall(core::getResourceString("icons/markerSmall.png"));
    QPixmap iconVideoSmall(core::getResourceString("icons/videoSmall.png"));
    
    DataFilterWidget* filter1 = new DataFilterWidget(tr("ANALOG"), iconAnalog, this);
    DataFilterWidget* filter2 = new DataFilterWidget(tr("KINETIC"), iconKinetic, this);
    DataFilterWidget* filter3 = new DataFilterWidget(tr("KINEMATIC"), iconKinematic, this);
    DataFilterWidget* filter4 = new DataFilterWidget(tr("VIDEO"), iconVideo, this);
    
    std::vector<TypeInfo> types;
    types.push_back(typeid(GRFCollection));
    types.push_back(typeid(GRFChannel));
    TypeFilterPtr typeFilter1(new TypeFilter(memoryDataManager, types));
    TypeFilterPtr typeFilter2(new TypeFilter(memoryDataManager, typeid(EMGChannel)));

    filter1->addFilter(tr("GRF"), "label", typeFilter1, &iconGRFSmall);

    QString emgFront = core::getResourceString("images/muscular_front/muscular_front.xml");
    QString emgBack = core::getResourceString("images/muscular_back/muscular_back.xml");

    typedef Vector3DFilterCommand2<EMGChannel, EMGCollectionPtr, ChartItemHelper> EMGCommand;
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
    IFilterCommandPtr emgCommand(new EMGCommand(memoryDataManager, emgNames, emgFront, emgBack));
    //filter1->addFilter("EMG", "emg label", typeFilter2, &iconEmgSmall);
    filter1->addFilter(tr("EMG"), "emg label", emgCommand, &iconEmgSmall);

    TypeFilterPtr typeFilter3(new TypeFilter(memoryDataManager, typeid(ForceCollection)));
    TypeFilterPtr typeFilter4(new TypeFilter(memoryDataManager, typeid(MomentCollection)));
    TypeFilterPtr typeFilter5(new TypeFilter(memoryDataManager, typeid(PowerCollection)));

    
    typedef Vector3DFilterCommand2<MomentChannel, MomentCollectionPtr, Vector3ItemHelper> MomentsCommand;
    typedef Vector3DFilterCommand2<ForceChannel, ForceCollectionPtr, Vector3ItemHelper> ForcesCommand;
    typedef Vector3DFilterCommand2<PowerChannel, PowerCollectionPtr, Vector3ItemHelper> PowerCommand;
    
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
    IFilterCommandPtr vFilter(new PowerCommand(memoryDataManager, powersNames, pathFront, pathBack));
    
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
    
    IFilterCommandPtr vFilter2(new MomentsCommand(memoryDataManager, momentsNames, pathFront, pathBack));

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
    IFilterCommandPtr vFilter3(new ForcesCommand(memoryDataManager, forcesNames, pathFront, pathBack));

    
    filter2->addFilter(tr("FORCES"), "count: ", vFilter3, &iconForceSmall);
    filter2->addFilter(tr("MOMENTS"), "test", vFilter2, &iconMomentSmall);
    filter2->addFilter(tr("POWERS"), "powers", vFilter, &iconPowerSmall);

    typedef Vector3DFilterCommand2<MarkerChannel, MarkerCollectionPtr, Vector3ItemHelper> MarkersCommand;
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
    IFilterCommandPtr vFilter6(new MarkersCommand(memoryDataManager, markersNames, markersFront, markersBack));
    TypeFilterPtr typeFilter7(new TypeFilter(memoryDataManager, typeid(kinematic::JointAnglesCollection)));

    filter3->addFilter(tr("MARKERS"), "count: 1", vFilter6, &iconMarkerSmall);
    filter3->addFilter(tr("JOINTS"), "count: 1", typeFilter7, &iconJointSmall);
    
    TypeFilterPtr typeFilter8(new TypeFilter(memoryDataManager, typeid(VideoChannel)));
    filter4->addFilter(tr("VIDEOS"), "count: 4", typeFilter8, &iconVideoSmall);
    
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
}




void HmmMainWindow::createFilterTab2()
{
    core::IMemoryDataManager * memoryDataManager = managersAccessor->getMemoryDataManager();
    QPixmap iconKinetic(core::getResourceString("icons/kineticBig.png"));
    QPixmap iconIllness(core::getResourceString("icons/jed.chorobowe.png"));
    QPixmap iconEndo(core::getResourceString("icons/po_endoplastyce.png"));
    QPixmap iconStroke(core::getResourceString("icons/po_udarze.png"));
    QPixmap iconSpine(core::getResourceString("icons/zwyrodnienia.png"));
   
    DataFilterWidget* filter1 = new DataFilterWidget("ILLNESS", iconIllness, this);
    DataFilterWidget* filter2 = new DataFilterWidget("MULTI", iconKinetic, this);
    DataFilterWidget* filter3 = new DataFilterWidget("MULTI", iconKinetic, this);
    DataFilterWidget* filter4 = new DataFilterWidget("MULTI", iconKinetic, this);

    IFilterCommandPtr endo(new Endo());
    IFilterCommandPtr stroke(new Stroke());
    IFilterCommandPtr spine(new Spine());

    filter1->addFilter("Endo", "...", endo, &iconEndo);
    filter1->addFilter("Stroke", "...", stroke, &iconStroke);
    filter1->addFilter("Spine", "...", spine, &iconSpine);

    
    /*filter2->addFilter("EMG", "...", stroke, &iconStroke);

    filter3->addFilter("EMG", "...", spine1, &iconSpine);
    filter3->addFilter("KINETIC", "...", spine2, &iconSpine);
    filter3->addFilter("ANGLES", "...", spine3, &iconSpine);*/
    TypeFilterPtr typeFilter1(new TypeFilter(memoryDataManager, typeid(GRFChannel)));
    TypeFilterPtr typeFilter2(new TypeFilter(memoryDataManager, typeid(EMGChannel)));

    IFilterCommandPtr multi1(new MultiChartCommand<ForceCollection>());
    IFilterCommandPtr multi2(new MultiChartCommand<MomentCollection>());
    IFilterCommandPtr multi3(new MultiChartCommand<PowerCollection>());

    /*filter1->addFilter("Force", "Multi Chart", multi1);
    filter1->addFilter("Moment", "Multi Chart", multi2);
    filter1->addFilter("Power", "Multi Chart", multi3);*/

    /*filter2->addFilter("Force", "Multi Chart", multi1);
    filter2->addFilter("Moment", "Multi Chart", multi2);
    filter2->addFilter("Power", "Multi Chart", multi3);*/

    /*filter3->addFilter("Force", "Multi Chart", multi1);
    filter3->addFilter("Moment", "Multi Chart", multi2);
    filter3->addFilter("Power", "Multi Chart", multi3);*/

    filter4->addFilter("Force", "Multi Chart", multi1);
    filter4->addFilter("Moment", "Multi Chart", multi2);
    filter4->addFilter("Power", "Multi Chart", multi3);

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

    QPixmap big(core::getResourceString("icons/Big.png"));
    this->analisis->setActivePixmapAndText(big, "ALL");
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
            this->clearTree();
            QTreeWidgetItem* item = TreeBuilder::createTree("Sessions", getCurrentSessions());
            this->addItemToTree(item);
            QPixmap big(core::getResourceString("icons/Big.png"));
            this->analisis->setActivePixmapAndText(big, "ALL");
            
        } else {
            QPixmap pix(dataWidget->getPixmap());
            QString name = dataWidget->getName();
            this->analisis->setActivePixmapAndText(pix, name);
        }
    }
}

void HmmMainWindow::onToolButton()
{
    for (auto it = button2TabWindow.begin(); it != button2TabWindow.end(); it++) {
        it->second->hide();
    }

    QToolButton* button = qobject_cast<QToolButton*>(sender());
    if (button) {
       button2TabWindow[button]->show();
    }
    
    mainArea->layout()->update();
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
    hmmItem->createSeries(visualizer, path, series);
    visualizer->getWidget()->setFocusProxy(visualizerDockWidget);

    connect(visualizerDockWidget, SIGNAL(focuseChanged(bool)), this, SLOT(visualizerFocusChanged(bool)));

    //topMainWindow->autoAddDockWidget( visualizerDockWidget);
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

    return visualizerDockWidget;
}






