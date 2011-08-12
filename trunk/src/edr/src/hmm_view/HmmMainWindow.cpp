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

using namespace core;

#define FILL_TREE_WITH_SCALAR(data)												\
	do {																        \
		if (session->has##data()) {										        \
			QTreeWidgetItem* markersItem = new QTreeWidgetItem();			        \
			markersItem->setText(0, tr(#data));								        \
			item->addChild(markersItem);									        \
			BOOST_FOREACH(MotionPtr motion, session->getMotions()) {	        \
				auto channelCollection = motion->get##data();					\
				if (channelCollection) {								        \
					QTreeWidgetItem* motionItem = new QTreeWidgetItem();        \
					motionItem->setText(0, motion->getName().c_str());	        \
					markersItem->addChild(motionItem);						        \
					int count = channelCollection->getNumChannels();			\
					for (int i = 0; i < count; i++) {							\
						QTreeWidgetItem* channelItem = new QTreeWidgetItem();	\
						auto c = channelCollection->getChannel(i);				\
						channelItem->setText(0, c->getName().c_str());			\
						motionItem->addChild(channelItem);						\
						item2ScalarMap[channelItem] = 							\
								boost::dynamic_pointer_cast<const ScalarChannel>(c);  \
					}															\
				}														        \
			}															        \
		}																        \
	}																	        \
	while(false);


HmmMainWindow::HmmMainWindow() :
	MainWindow(),
	currentVisualizer(nullptr),
	pane(nullptr),
	treeWidget(nullptr)
{
	this->setWindowFlags(Qt::FramelessWindowHint);
}


void HmmMainWindow::init( core::PluginLoader* pluginLoader )
{
	core::MainWindow::init(pluginLoader);
	setupUi(this);

	topButton->setFixedWidth(0);
	bottomButton->setFixedWidth(0);
	QString s = tabWidget->styleSheet();
	tabWidget->setStyleSheet(
		"QTabBar::tab:selected { color: rgb(135, 173, 255); }" 
		"QTabBar::tab {  color: white; } ");    //height: 100px; width: 100px; 
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
	connect(&getUpdateTimer(), SIGNAL(timeout()), this, SLOT(updateVisualizers()));*/

	treeWidget = new QTreeWidget();
	QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    
	treeWidget->setMaximumWidth(250);
	treeWidget->setHeaderHidden(true);
	QHBoxLayout *hlayout = new QHBoxLayout;
	hlayout->addWidget(treeWidget);

	//pane = new QWidget(this);
    pane = new QMainWindow(nullptr);
	//pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//QGridLayout* gridLayout = new QGridLayout();
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
		}
	}

	EDRWorkflowWidget* widget = new EDRWorkflowWidget();
	QHBoxLayout* layout = new QHBoxLayout();
	layout->addWidget(widget);
	Operations->setLayout(layout);
	getUpdateTimer().start(20);
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

	//treeWidget = new QTreeWidget();
	//QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    
	//treeWidget->setMaximumWidth(250);
	//treeWidget->setHeaderHidden(true);
	//QHBoxLayout *layout = new QHBoxLayout;
	//layout->addWidget(treeWidget);

	//pane = new QWidget(this);
	//pane->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	//QGridLayout* gridLayout = new QGridLayout();
	//pane->setLayout(gridLayout);
	////VisualizerWidget* visualizerWidget = new VisualizerWidget();

	//layout->addWidget(pane);
	//Analizy->setLayout(layout);
	//layout->addStretch();

	for (int i = sessions.size() - 1; i >= 0; --i) {
		SessionPtr session = sessions[i];
		QTreeWidgetItem* item = new QTreeWidgetItem();
		QString tmp(session->getName().c_str());
		item->setText(0, tmp);
		treeWidget->addTopLevelItem(item);
		treeWidget->setColumnCount(2);
		treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
		FILL_TREE_WITH_SCALAR(Emg);
		FILL_TREE_WITH_SCALAR(Grf);
		
		if (session->hasMarkers()) {							
			QTreeWidgetItem* markersItem = new QTreeWidgetItem();		
			markersItem->setText(0, tr("Markers"));							
			item->addChild(markersItem);								
			BOOST_FOREACH(MotionPtr motion, session->getMotions()) {
				const auto channelCollection = motion->getMarkers();			
				if (channelCollection) {								
					QTreeWidgetItem* motionItem = new QTreeWidgetItem();    
					motionItem->setText(0, motion->getName().c_str());	    
					markersItem->addChild(motionItem);		
					item2Markers[motionItem] = channelCollection;
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
	onClicked<ScalarChannel, ScalarChannelConstPtr>(item, item2ScalarMap);
	onClicked<MarkerCollection>(item, item2Markers);
}





