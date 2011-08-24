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
	pane(nullptr),
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
	//QString s = tabWidget->styleSheet();
	/*tabWidget->setStyleSheet(
		"QTabBar::tab:selected { color: rgb(135, 173, 255); }" 
		"QTabBar::tab {  color: white; } "); */   //height: 100px; width: 100px; 
	/*Badania_2->setStyleSheet(
		"QTabBar::tab { background: gray; color: white; padding: 10px; } "
		"QTabBar::tab:selected { background: lightgray; } "
		"QTabWidget::pane { border: 0; } "
		"QWidget { background: lightgray; } ");*/


	connect(this->fontSlider, SIGNAL(valueChanged(int)), this, SLOT(setFont(int)));
	connect(this->layoutTopSlider, SIGNAL(valueChanged(int)), this, SLOT(setTop(int)));
	connect(this->layoutBottomSlider, SIGNAL(valueChanged(int)), this, SLOT(setBottom(int)));
	
	this->showFullScreen();
	/*connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(populateWindowMenu()));
	connect(&getUpdateTimer(), SIGNAL(timeout()), this, SLOT(updateServices()));
	connect(&getUpdateTimer(), SIGNAL(timeout()), this, SLOT(updateVisualizers()));
    getUpdateTimer().start(20);
    */

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

	//pane = new QWidget(this);
    pane = new QMainWindow(nullptr);
	//pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//QGridLayout* gridLayout = new QGridLayout();
	//pane->setLayout(gridLayout);
	hlayout->setSpacing(0);
	hlayout->setMargin(0);
	/*pane = new QFrame(this);
	pane->setFrameShape(QFrame::Box);
	pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);*/
	/*QGridLayout* gridLayout = new QGridLayout();
	gridLayout->setMargin(0);
	gridLayout->setSpacing(0);*/
	//pane->setLayout(gridLayout);
	//VisualizerWidget* visualizerWidget = new VisualizerWidget();

	hlayout->addWidget(pane);
    int i = hlayout->children().size();
	Analizy->setLayout(hlayout);

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
			//layout->addStretch();
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

            widget->getTitleBar()->setFloatButtonVisible(false);
            widget->getTitleBar()->setCloseButtonVisible(false);

            widget->setTitleBarVisible(false);

            pane->addDockWidget(Qt::BottomDockWidgetArea, widget);
        }
	}

	EDRWorkflowWidget* widget = new EDRWorkflowWidget();
	QVBoxLayout* layout = new QVBoxLayout();
	layout->addWidget(widget);
	layout->addWidget(widgetConsole);

	Operations->setLayout(layout);
	//getUpdateTimer().start(20);
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
				grfItem->setText(0, "GRF");
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
			tryAddVectorToTree(motion->getAngles(), "Angles", &icon3D, motionItem);
			tryAddVectorToTree(motion->getPowers(), "Powers", &icon3D, motionItem);

			const auto markerCollection = motion->getMarkers();			
			if (markerCollection) {
				QTreeWidgetItem* markersItem = new QTreeWidgetItem();
				markersItem->setIcon(0, icon3D);
				markersItem->setText(0, tr("Markers"));							
				motionItem->addChild(markersItem);		
				item2Markers[markersItem] = markerCollection;
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
	onClicked<ScalarChannel, ScalarChannelConstPtr>(item, item2ScalarMap);
	onClicked<MarkerCollection>(item, item2Markers);

	auto it = item2Vector.find(item);
	if (it != item2Vector.end()) {
		QDockWidget* dock = new QDockWidget();
		QLabel* label = new QLabel("<font color = \"red\" size = 5> Tutaj trzeba dodac wizualizator VectorChannel :) </font> <br /> pozdrawiam, WK");
		dock->setWidget(label);
		pane->addDockWidget(Qt::LeftDockWidgetArea, dock);
	}
}





