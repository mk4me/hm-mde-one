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

using namespace core;


HmmMainWindow::HmmMainWindow() :
	MainWindow(),
	currentVisualizer(nullptr),
	topMainWindow(nullptr),
	treeWidget(nullptr),
    currentItem(nullptr)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
    itemClickAction.setMainWindow(this);
}


void HmmMainWindow::init( core::PluginLoader* pluginLoader )
{
	core::MainWindow::init(pluginLoader);
	setupUi(this);

	topButton->setFixedWidth(0);
	bottomButton->setFixedWidth(0);

	connect(this->fontSlider, SIGNAL(valueChanged(int)), this, SLOT(setFont(int)));
	connect(this->layoutTopSlider, SIGNAL(valueChanged(int)), this, SLOT(setTop(int)));
	connect(this->layoutBottomSlider, SIGNAL(valueChanged(int)), this, SLOT(setBottom(int)));
	
	this->showFullScreen();

	treeWidget = new QTreeWidget();
    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));    
	QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    
	treeWidget->setMaximumWidth(250);
	treeWidget->setHeaderHidden(true);

	// TODO : przeniesc do designera
	QString style = 
		"QScrollBar:horizontal {											   "
		"border: 1px ridge grey;											   "
		"background: #FFFFFF;												   "
		"height: 15px;														   "
		"margin: 0px 0px 0px 0px;											   "
		"}																	   "
		"																	   "
		"QScrollBar::handle:horizontal,  QScrollBar::handle:pressed {			"	
		"background: rgb(135, 173, 255);									   "
		"min-width: 2px;													   "
		"max-width: 5px;													   "
		"height:5px;				    									   "
		"border: 0px solid grey;											   "
		"}																	   "
		"																	   "
		"QScrollBar::handle:horizontal {									   "
		"background: rgb(175, 213, 255);									   "
		"selection-background-color: rgb(135, 173, 255);						"
		"min-width: 2px;													   "
		"max-width: 5px;													   "
		"height:5px;				    									   "
		"border: 0px solid grey;											   "
		"}																	   "
		"																	   "
		"QScrollBar::add-line:horizontal {									   "
		"border: 0px solid grey;											   "
		"background: #FFFFFF;												   "
		"width: 0px;														   "
		"subcontrol-position: right;										   "
		"subcontrol-origin: margin;											   "
		"}																	   "
		"																	   "
		"QScrollBar::sub-line:horizontal {									   "
		"border: 0px solid grey;											   "
		"background: #FFFFFF;												   "
		"width: 0px;														   "
		"subcontrol-position: left;											   "
		"subcontrol-origin: margin;											   "
		"}																	   "
		"																	   "
		"QScrollBar:vertical {												   "
		"border: 1px ridge grey;											   "
		"background: #FFFFFF;												   "
		"width: 15px;														   "
		"margin: 2px 0 2px 0;												   "
		"}																	   "
		"																	   "
		"QScrollBar::handle:vertical,  QScrollBar::handle:pressed {			   "
		"background: rgb(135, 173, 255);									   "
		"border: 0px outset grey;											   "
		"min-height: 20px;													   "
		"}																	   "
		"																	   "
		"QScrollBar::handle:vertical {										   "
		"background: rgb(175, 213, 255);									   "
		"selection-background-color: rgb(135, 173, 255);					   "
		"border: 0px outset grey;											   "
		"min-height: 20px;													   "
		"}																	   ";

	treeWidget->setStyleSheet(style);
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(treeWidget);

    QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    topMainWindow = new EDRDockWidgetManager(this);
    bottomMainWindow = new QMainWindow(nullptr);

    splitter->addWidget(topMainWindow->asQWidget());
    splitter->addWidget(bottomMainWindow);

	hlayout->setSpacing(0);
	hlayout->setMargin(0);

	hlayout->addWidget(splitter);
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
            std::vector<QObject*> mainWidgetActions;
            QWidget* viewWidget = service->getWidget(mainWidgetActions);

            std::vector<QObject*> controlWidgetActions;
            QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

            std::vector<QObject*> settingsWidgetActions;
            QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

            EDRDockWidget * widget = new EDRDockWidget();
            widget->getInnerWidget()->layout()->addWidget(controlWidget);
            widget->setAllowedAreas(Qt::BottomDockWidgetArea);
            for(auto it = controlWidgetActions.begin(); it!= controlWidgetActions.end(); it++){
                widget->getTitleBar()->addObject(*it, IEDRTitleBar::Left);
            }
			
            bottomMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);

            widget->getTitleBar()->setFloatButtonVisible(false);
            widget->getTitleBar()->setCloseButtonVisible(false);

            widget->setTitleBarVisible(false);
        }
	}

    Analizy->setLayout(hlayout);

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
    auto it = item2Helper.find(item);
    if (it != item2Helper.end()) {
        return it->second->isDataItem();
    } else {
        return false;
    }
    //return dataItems.find(item) != dataItems.end();
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


    
	std::vector<SessionPtr> sessions = core::queryDataPtr(DataManager::getInstance());

	for (int i = sessions.size() - 1; i >= 0; --i) {
		SessionPtr session = sessions[i];
		treeWidget->setColumnCount(2);
		treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
		
		QIcon icon(core::getResourceString("icons/charts.png"));
		QIcon icon3D(core::getResourceString("icons/3d.png"));
		QIcon iconVideo(core::getResourceString("icons/video.png"));
										
		BOOST_FOREACH(MotionPtr motion, session->getMotions()) {	
			QTreeWidgetItem* motionItem = new QTreeWidgetItem();  
			motionItem->setText(0, motion->getName().c_str());	 
			treeWidget->addTopLevelItem(motionItem);
           
            bool hasEmg = motion->hasObjectOfType(typeid(EMGChannel));
            bool hasGrf = motion->hasObjectOfType(typeid(GRFChannel));
            if (hasEmg || hasGrf) {
                QTreeWidgetItem* analogItem = new QTreeWidgetItem();
                analogItem->setText(0, tr("Analog data"));
                motionItem->addChild(analogItem);
                if (hasEmg) {	
                	QTreeWidgetItem* emgItem = new QTreeWidgetItem();
                	analogItem->addChild(emgItem);
                	emgItem->setText(0, "EMG");
                    std::vector<ObjectWrapperPtr> emgs = motion->getWrappers(typeid(EMGChannel));
                	int count = emgs.size();			
                	for (int i = 0; i < count; i++) {							
                		QTreeWidgetItem* channelItem = new QTreeWidgetItem();
                		channelItem->setIcon(0, icon);							
                		EMGChannelPtr c = emgs[i]->get();	
                        if (c) {
                		    channelItem->setText(0, c->getName().c_str());			
                		    emgItem->addChild(channelItem);			
                            item2Helper[channelItem] = TreeItemHelperPtr(new ChartItemHelper(emgs[i]));
                        }
                	}														
                }	

                if (hasGrf) {	
                	QTreeWidgetItem* grfItem = new QTreeWidgetItem();
                    std::vector<ObjectWrapperPtr> grfCollections = motion->getWrappers(typeid(GRFCollection));
                    if (grfCollections.size() == 1) {
                        item2Helper[grfItem] = TreeItemHelperPtr(new TreeItemHelper(grfCollections[0]));
                    }
                	grfItem->setText(0, "GRF");
                	grfItem->setIcon(0, icon3D);
                	analogItem->addChild(grfItem);
                    std::vector<ObjectWrapperPtr> grfs = motion->getWrappers(typeid(GRFChannel));
                	int count = grfs.size();			
                	for (int i = 0; i < count; i++) {
                        GRFChannelPtr c = grfs[i]->get();
                        if (c) {
                            QTreeWidgetItem* channelItem = new QTreeWidgetItem();	
                            channelItem->setIcon(0, icon);						
                            channelItem->setText(0, c->getName().c_str());			
                            grfItem->addChild(channelItem);			
                            item2Helper[channelItem] = TreeItemHelperPtr(new Vector3ItemHelper(grfs[i]));
                        }
                	}														
                }
            }
                
            if (motion->hasObjectOfType(typeid(ForceCollection)) || motion->hasObjectOfType(typeid(MomentCollection)) || motion->hasObjectOfType(typeid(PowerCollection))) {
                QTreeWidgetItem* kineticItem = new QTreeWidgetItem();
                kineticItem->setText(0, tr("Kinetic data"));
                motionItem->addChild(kineticItem);
                std::vector<ObjectWrapperPtr> forces = motion->getWrappers(typeid(ForceCollection));
                std::vector<ObjectWrapperPtr> moments = motion->getWrappers(typeid(MomentCollection));
                std::vector<ObjectWrapperPtr> powers = motion->getWrappers(typeid(PowerCollection));
                if (forces.size() > 0) {
                    ForceCollectionPtr f = forces[0]->get();
                    tryAddVectorToTree<ForceChannel>(motion, f, "Forces", &icon3D, kineticItem);
                }
                if (moments.size() > 0) {
                    MomentCollectionPtr m = moments[0]->get();
                    tryAddVectorToTree<MomentChannel>(motion, m, "Moments", &icon3D, kineticItem);
                }
                // do rozwiniecia - potrzeba parsowac pliki vsk i interpretowac strukture kinamatyczna tak jak to robi Vicon
                //tryAddVectorToTree(motion->getAngles(), "Angles", &icon3D, kineticItem);
                if (powers.size() > 0) {
                    PowerCollectionPtr p = powers[0]->get();
                    tryAddVectorToTree<PowerChannel>(motion, p, "Powers", &icon3D, kineticItem);
                }
            }

            bool hasMarkers = motion->hasObjectOfType(typeid(MarkerCollection));	
            bool hasJoints = motion->hasObjectOfType(typeid(kinematic::JointAnglesCollection));
                
            if (hasMarkers || hasJoints) {
                QTreeWidgetItem* kinematicItem = new QTreeWidgetItem();
                kinematicItem->setText(0, tr("Kinematic data"));
                motionItem->addChild(kinematicItem);
                if (hasMarkers) {
                	QTreeWidgetItem* markersItem = new QTreeWidgetItem();
                	markersItem->setIcon(0, icon3D);
                	markersItem->setText(0, tr("Markers"));							
                	kinematicItem->addChild(markersItem);	
                    core::ObjectWrapperPtr markerCollection = motion->getWrapperOfType(typeid(MarkerCollection));
                    item2Helper[markersItem] = TreeItemHelperPtr(new TreeItemHelper(markerCollection));
                    
                    if (!markerCollection->isNull()) {
                        MarkerCollectionPtr m = markerCollection->get(); 
                        tryAddVectorToTree<MarkerChannel>(motion, m, "Marker collection", &icon3D, markersItem, false);
                    }
                }
                
                if (hasJoints) {
                	QTreeWidgetItem* skeletonItem = new QTreeWidgetItem();
                	skeletonItem->setIcon(0, icon3D);
                	skeletonItem->setText(0, tr("Joints"));
                	kinematicItem->addChild(skeletonItem);
                    item2Helper[skeletonItem] = TreeItemHelperPtr(new JointsItemHelper(motion));
                }
                				
                if (hasJoints || hasMarkers || hasGrf) {
                	item2Helper[kinematicItem] = TreeItemHelperPtr(new Multiserie3D(motion));
                }
            }
                
            if (motion->hasObjectOfType(typeid(VideoChannel))) {
                QTreeWidgetItem* videoItem = new QTreeWidgetItem();
                videoItem->setText(0, "Videos");
                motionItem->addChild(videoItem);
                std::vector<ObjectWrapperPtr> videos = motion->getWrappers(typeid(VideoChannel));
                int count = videos.size();			
                for (int i = 0; i < count; i++) {							
                    QTreeWidgetItem* channelItem = new QTreeWidgetItem();	

                    channelItem->setIcon(0, icon3D);						
                    channelItem->setText(0, videos[i]->getName().c_str());			
                    videoItem->addChild(channelItem);						
                    item2Helper[channelItem] = TreeItemHelperPtr(new TreeItemHelper(videos[i]));\
                }		
            }

            QTreeWidgetItem* testItem = new QTreeWidgetItem();
            testItem->setText(0, "TEST");
            motionItem->addChild(testItem);
            if (motion->hasObjectOfType(typeid(ForceCollection))) {
                ObjectWrapperPtr forcesWrapper = motion->getWrapperOfType(typeid(ForceCollection));
                ForceCollectionPtr forces = forcesWrapper->get();
                std::vector<ObjectWrapperPtr> fxWrappers;
                std::vector<ObjectWrapperPtr> fyWrappers;
                std::vector<ObjectWrapperPtr> fzWrappers;
                for( int i = 0; i < forces->getNumChannels(); i++) {
                    ForceChannelPtr f = forces->getChannel(i);
                    ScalarChannelReaderInterfacePtr x(new VectorToScalarAdaptor(f, 0));
                    ScalarChannelReaderInterfacePtr y(new VectorToScalarAdaptor(f, 1));
                    ScalarChannelReaderInterfacePtr z(new VectorToScalarAdaptor(f, 2));

                    core::ObjectWrapperPtr wrapperX = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                    core::ObjectWrapperPtr wrapperY = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                    core::ObjectWrapperPtr wrapperZ = core::ObjectWrapper::create<ScalarChannelReaderInterface>();

                    wrapperX->set(x);
                    wrapperY->set(y);
                    wrapperZ->set(z);

                    static int number = 0;
                    // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
                    std::string suffix = boost::lexical_cast<std::string>(number++);
                    wrapperX->setName("FX_" + suffix);
                    wrapperX->setSource("FX_" + suffix);
                    wrapperY->setName("FY_" + suffix);
                    wrapperY->setSource("FY_" + suffix);
                    wrapperZ->setName("FZ_" + suffix);
                    wrapperZ->setSource("FZ_" + suffix);

                    fxWrappers.push_back(wrapperX);
                    fyWrappers.push_back(wrapperY);
                    fzWrappers.push_back(wrapperZ);
                }
                QTreeWidgetItem* fX = new QTreeWidgetItem();
                fX->setText(0, "Motion Forces - X");
                item2Helper[fX] = TreeItemHelperPtr(new MultiserieHelper(fxWrappers));
                testItem->addChild(fX);

                QTreeWidgetItem* fY = new QTreeWidgetItem();
                fY->setText(0, "Motion Forces - Y");
                item2Helper[fY] = TreeItemHelperPtr(new MultiserieHelper(fyWrappers));
                testItem->addChild(fY);

                QTreeWidgetItem* fZ = new QTreeWidgetItem();
                fZ->setText(0, "Motion Forces - Z");
                item2Helper[fZ] = TreeItemHelperPtr(new MultiserieHelper(fzWrappers));
                testItem->addChild(fZ);
            }


		}
		
	}
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
    auto it = item2Helper.find(item);
    if (it != item2Helper.end()) {
        TreeItemHelperPtr helper = it->second;
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

        TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
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
        

        /*for (int i = 0; i < series.size(); i++) {
            auto timeSerie = series[i];

            if(timeSerie != nullptr){

                TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
                if(timeline != nullptr) {
                    VisualizerChannelPtr channel(new VisualizerChannel(timeSerie, visualizerDockWidget));
                    try{
                        timeline->addChannel(path.toStdString(), channel);
                    }catch(...){
                        LOG_ERROR("Could not add multichannel to timeline!");
                    }
                }
            }
        }*/
    }
}






