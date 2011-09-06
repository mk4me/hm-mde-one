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

using namespace core;


HmmMainWindow::HmmMainWindow() :
	MainWindow(),
	currentVisualizer(nullptr),
	topMainWindow(nullptr),
	treeWidget(nullptr)
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

    topMainWindow = new QMainWindow(nullptr);
    bottomMainWindow = new QMainWindow(nullptr);

    splitter->addWidget(topMainWindow);
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

            topMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);

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

    // prototyp textu dla Charta

    chartTextPrototype = new osgText::Text();

    chartTextPrototype->setColor(osg::Vec4(0, 0, 0, 1));
    chartTextPrototype->setFontResolution(50,50);
    chartTextPrototype->setFont(osgText::readFontFile("fonts/arial.ttf"));    
    chartTextPrototype->setCharacterSize(12);
    chartTextPrototype->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS);
    chartTextPrototype->setAxisAlignment(osgText::Text::USER_DEFINED_ROTATION );
    chartTextPrototype->setAutoRotateToScreen(false);
    chartTextPrototype->setAlignment(osgText::Text::CENTER_CENTER);
    chartTextPrototype->setText("");
    chartTextPrototype->setLayout(osgText::Text::LEFT_TO_RIGHT);
}

void HmmMainWindow::visualizerGainedFocus()
{
    VisualizerWidget * widget = qobject_cast<VisualizerWidget *>(sender());

    if(widget == nullptr){
        return;
    }

    if(currentVisualizer != widget){
        widget->setTitleBarVisible(true);
        if(currentVisualizer != nullptr){
            currentVisualizer->setTitleBarVisible(false);
        }
        currentVisualizer = widget;
    }
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
	}

	std::vector<SessionPtr> sessions = core::queryDataPtr(DataManager::getInstance());

	for (int i = sessions.size() - 1; i >= 0; --i) {
		SessionPtr session = sessions[i];
		//QTreeWidgetItem* item = new QTreeWidgetItem();
		//QString tmp(session->getName().c_str());
		//item->setText(0, tmp);
		//treeWidget->addTopLevelItem(item);
		treeWidget->setColumnCount(2);
		treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
		
		QIcon icon(core::getResourceString("icons/charts.png"));
		QIcon icon3D(core::getResourceString("icons/3d.png"));
		QIcon iconVideo(core::getResourceString("icons/video.png"));
										
		BOOST_FOREACH(MotionPtr motion, session->getMotions()) {	
			QTreeWidgetItem* motionItem = new QTreeWidgetItem();  
			motionItem->setText(0, motion->getName().c_str());	 
			treeWidget->addTopLevelItem(motionItem);
			//item->addChild(motionItem);
			auto channelCollection = motion->getEmg();

			if (channelCollection) {	
				QTreeWidgetItem* emgItem = new QTreeWidgetItem();
				motionItem->addChild(emgItem);
				emgItem->setText(0, "EMG");
				int count = channelCollection->getNumChannels();			
				for (int i = 0; i < count; i++) {							
					QTreeWidgetItem* channelItem = new QTreeWidgetItem();
					channelItem->setIcon(0, icon);							
					auto c = channelCollection->getChannel(i);				
					channelItem->setText(0, c->getName().c_str());			
					emgItem->addChild(channelItem);						
					item2ScalarMap[channelItem] = 							
						boost::dynamic_pointer_cast<const ScalarChannel>(c);
				}														
			}	

			auto grfCollection = motion->getGrf();
			if (grfCollection) {	
				QTreeWidgetItem* grfItem = new QTreeWidgetItem();
				item2GrfCollection[grfItem] = grfCollection;
				grfItem->setText(0, "GRF");
				grfItem->setIcon(0, icon3D);
				motionItem->addChild(grfItem);
				int count = grfCollection->getNumChannels();			
				for (int i = 0; i < count; i++) {							
					QTreeWidgetItem* channelItem = new QTreeWidgetItem();	
						
					channelItem->setIcon(0, icon);							
					auto c = grfCollection->getChannel(i);				
					channelItem->setText(0, c->getName().c_str());			
					grfItem->addChild(channelItem);						
					item2ScalarMap[channelItem] = 							
						boost::dynamic_pointer_cast<const ScalarChannel>(c);
				}														
			}

            auto eventsCollection = motion->getEvents();
            if(eventsCollection){
                QTreeWidgetItem* eventItem = new QTreeWidgetItem();
                eventItem->setText(0, "Events");
                motionItem->addChild(eventItem);
                item2Events[eventItem] = eventsCollection;
            }

			tryAddVectorToTree(motion->getForces(), "Forces", &icon3D, motionItem);
			tryAddVectorToTree(motion->getMoments(), "Moments", &icon3D, motionItem);
			// do rozwiniecia - potrzeba parsowac pliki vsk i interpretowac strukture kinamatyczna tak jak to robi Vicon
			//tryAddVectorToTree(motion->getAngles(), "Angles", &icon3D, motionItem);
			tryAddVectorToTree(motion->getPowers(), "Powers", &icon3D, motionItem);

			const auto markerCollection = motion->getMarkers();			
			if (markerCollection) {
				QTreeWidgetItem* markersItem = new QTreeWidgetItem();
				markersItem->setIcon(0, icon3D);
				markersItem->setText(0, tr("Markers"));							
				motionItem->addChild(markersItem);		
				item2Markers[markersItem] = markerCollection;
			}

			const auto skeletalCollection = motion->getJoints();
			if (skeletalCollection) {
				QTreeWidgetItem* skeletonItem = new QTreeWidgetItem();
				skeletonItem->setIcon(0, icon3D);
				skeletonItem->setText(0, tr("Joints"));
				motionItem->addChild(skeletonItem);
				item2Skeleton[skeletonItem] = skeletalCollection;
			}

			if (skeletalCollection && markerCollection && grfCollection) {
				QTreeWidgetItem* kinematicItem = new QTreeWidgetItem();
				kinematicItem->setIcon(0, icon3D);
				kinematicItem->setText(0, tr("Kinematic"));
				motionItem->addChild(kinematicItem);
				item2Kinematic[kinematicItem] = motion;
			}


			const std::vector<VideoChannelConstPtr>& videoCollection = motion->getVideos();
			int count = videoCollection.size();
			if (count) {
				QTreeWidgetItem* videoItem = new QTreeWidgetItem();
				videoItem->setText(0, "Video");
				motionItem->addChild(videoItem);			
				for (int i = 0; i < count; i++) {							
					QTreeWidgetItem* channelItem = new QTreeWidgetItem();
					channelItem->setIcon(0, iconVideo);							
					VideoChannelConstPtr c = videoCollection[i];				
					channelItem->setText(0, c->getName().c_str());			
					videoItem->addChild(channelItem);						
					item2Video[channelItem] = 							
						boost::dynamic_pointer_cast<const VideoChannel>(c);
				}		
			}
		}
		
		//FILL_TREE_WITH_SCALAR(Joints);
		//FILL_TREE_WITH_SCALAR(Video);
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
	onClickedScalar<ScalarChannel, ScalarChannelConstPtr>(item, item2ScalarMap);
	onClickedOther<MarkerCollection>(item, item2Markers);
	onClickedOther<VideoChannel>(item, item2Video);
	onClickedOther<GRFCollection>(item, item2GrfCollection);
	onClickedOther<kinematic::JointAnglesCollection>(item, item2Skeleton);

	auto it = item2Vector.find(item);
	if (it != item2Vector.end()) {
		QDockWidget* dock = new QDockWidget();
		QLabel* label = new QLabel("<font color = \"red\" size = 5> Tutaj trzeba dodac wizualizator VectorChannel :) </font> <br /> pozdrawiam, WK");
		dock->setWidget(label);
		topMainWindow->addDockWidget(Qt::LeftDockWidgetArea, dock);
	}

	auto it2 = item2Kinematic.find(item);
	if (it2 != item2Kinematic.end()) {
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
		MotionPtr motion = it2->second;
        if(visualizerManager->existVisualizerForType(typeid(MarkerCollection)) == true){
			VisualizerPtr vis = VisualizerManager::getInstance()->createVisualizer(typeid(MarkerCollection));
            ObjectWrapperPtr wrapper = ObjectWrapper::create<MarkerCollection>();
            //wrapper->set(motion->getMarkers());
			wrapper->set(boost::const_pointer_cast<MarkerCollection>(motion->getMarkers()));
            static std::string prefix = typeid(MarkerCollection).name();
            // hack + todo - rozwiazanie problemu z zarejesrowanymi nazwami w timeline
            prefix += "_";
            wrapper->setName(prefix + "nazwa testowa");
            wrapper->setSource(prefix + "Sciezka testowa");

			ObjectWrapperPtr wrapper2 = ObjectWrapper::create<GRFCollection>();
			wrapper2->set(boost::const_pointer_cast<GRFCollection>(motion->getGrf()));
			wrapper2->setName(prefix + "nazwa testowa2");
			wrapper2->setSource(prefix + "Sciezka testowa2");

			ObjectWrapperPtr wrapper3 = ObjectWrapper::create<kinematic::JointAnglesCollection>();
			wrapper3->set(boost::const_pointer_cast<kinematic::JointAnglesCollection>(motion->getJoints()));
			wrapper3->setName(prefix + "nazwa testowa3");
			wrapper3->setSource(prefix + "Sciezka testowa3");


            vis->getOrCreateWidget();
			   
			VisualizerWidget* visu = new VisualizerWidget(vis);
            visu->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
            //visu->setStyleSheet(styleSheet());
            visu->setVisualizerIconVisible(false);
            visu->setSplitHVisible(false);
            visu->setSplitVVisible(false);
            visu->setActiveVisualizerSwitch(false);
            visu->setSourceVisible(false);

            visu->setTitleBarVisible(false);

            auto serie1 = vis->createSerie(wrapper, path.toStdString());
			auto serie2 = vis->createSerie(wrapper2, path.toStdString());
			auto serie3 = vis->createSerie(wrapper3, path.toStdString());

            TimelinePtr timeline = core::queryServices<ITimelineService>(core::getServiceManager());
            if(timeline != nullptr) {
                //timeline->setChannelTooltip(path.toStdString(), "test Tooltip");
                timeline::IChannelConstPtr channel(core::dynamic_pointer_cast<const timeline::IChannel>(serie1));
                if(channel != nullptr){
                    channelToVisualizer[channel] = visu;
                    //timeline->setOnChannelDblClick(itemClickAction);
                }
				timeline::IChannelConstPtr channel2(core::dynamic_pointer_cast<const timeline::IChannel>(serie2));
				if(channel2 != nullptr){
					channelToVisualizer[channel2] = visu;
				}
				timeline::IChannelConstPtr channel3(core::dynamic_pointer_cast<const timeline::IChannel>(serie3));
				if(channel3 != nullptr){
					channelToVisualizer[channel3] = visu;
				}
            }

            vis->getWidget()->setFocusProxy(visu);

            connect(visu, SIGNAL(focuseGained()), this, SLOT(visualizerGainedFocus()));

		//pane->addDockWidget(Qt::RightDockWidgetArea, visualizerWidget);
			/*pane->setUpdatesEnabled(false);
			if (currentVisualizer) {
				pane->layout()->removeWidget(currentVisualizer);
				delete currentVisualizer;
			}*/
			//currentVisualizer = visu;
			//pane->layout()->addWidget(visu);            
            pane->addDockWidget(Qt::TopDockWidgetArea, visu, Qt::Horizontal);
			//pane->setUpdatesEnabled(true);
        }else{

            /*TimelinePtr timeline = core::queryServices<TimelineService>(core::getServiceManager());
            if(timeline != nullptr && dynamic_cast<timeline::IChannel*>(serie.get()) != nullptr) {
                timeline::IChannelPtr channel = core::dynamic_pointer_cast<timeline::IChannel>(serie);
                timeline->addChannel(name, channel);
                timelineDataSeries.insert(serie);
            }*/
        }
	}
}





