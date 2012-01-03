#include "hmmPCH.h"
#include <cmath>
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
#include <plugins/subject/ISubjectService.h>
#include "IllnessUnit.h"
#include "EMGFilter.h"
#include "EDRTitleBar.h"


using namespace core;

HMMVisualizerUsageContext::HMMVisualizerUsageContext(FlexiTabWidget * flexiTabWidget) : flexiTabWidget(flexiTabWidget), visualizerGroupID(-1)
{

}

void HMMVisualizerUsageContext::activateContext(QWidget * contextWidget)
{
    if(contextWidget == nullptr){
        return;
    }

    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    if(it->second.empty() == false){
        auto visWidget = qobject_cast<VisualizerWidget*>(contextWidget);
        auto vis = visWidget->getCurrentVisualizer();
        //tworzymy grupe dla wizualizatora
        visualizerGroupID = flexiTabWidget->addGroup(QString::fromUtf8("Visualizer - ") + QString::fromUtf8(vis->getName().c_str()), VisualizerManager::getInstance()->getIcon(visWidget->getCurrentVisualizer()->getID()));

        for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); sectionIT++){
            auto sectionID = flexiTabWidget->addSection(visualizerGroupID, sectionIT->second, sectionIT->first);
            sectionIT->second->setVisible(true);
            visualizerSectionsIDs.insert(sectionID);
        }
        
        flexiTabWidget->setCurrentGroup(visualizerGroupID);
    }
}

void HMMVisualizerUsageContext::deactivateContext(QWidget * contextWidget)
{
    auto visWidget = qobject_cast<VisualizerWidget*>(contextWidget);

    if(visualizerGroupID != -1 && getCurrentContextWidget() == contextWidget){
        flexiTabWidget->removeGroup(visualizerGroupID);
        visualizerGroupID = -1;
        visualizerSectionsIDs.swap(std::set<FlexiTabWidget::GUIID>());
    }
}

void HMMVisualizerUsageContext::onRegisterContextWidget(QWidget * contextWidget)
{
    VisualizerWidget * visWidget = qobject_cast<VisualizerWidget*>(contextWidget);
    // przygotowanie do wypelnienia grupy
    VisualizerWidget::VisualizerTitleBarElements titleBarElements;

    visWidget->getVisualizerTitleBarElements(titleBarElements);

    if(titleBarElements.empty() == false){
        auto vis = visWidget->getCurrentVisualizer();

        //podziel elementy na 4 grupy - akcje, menusy, widget i inne nieobslugiwane elementy
        //przy okazji wyznaczamy ilosc elementow oraz ich sumaryczna szerokosc i najwieksza wysokosc do pozniejszego layoutowania
        //elementy sa indeksowane tak jak podeslal nam je klient, ale ich kolejnosc moze zostac zmieniona zeby lepiej je rozlozyc
        std::map<unsigned int, QAction*> actions;
        std::map<unsigned int, QMenu*> menus;
        //std::map<unsigned int, QWidget*> widgets;
        std::map<unsigned int, QObject*> others;

        // budujemy widgety ktore beda potem trafialy do toolbarow
        std::map<unsigned int, QWidget*> toolbarElements;

        int maxHeight = 0;
        int totalWidth = 0;
        int totalElements = titleBarElements.size();

        for(unsigned int i = 0; i < totalElements; i++){
            if(QAction * action = qobject_cast<QAction*>(titleBarElements[i].first)){
                actions[i] = action;
            }else if(QMenu * menu = qobject_cast<QMenu*>(titleBarElements[i].first)){
                menus[i] = menu;
            }else if(QWidget * widget = qobject_cast<QWidget*>(titleBarElements[i].first)){
                //widgets[i] = widget;
                toolbarElements[i] = widget;

                if(QComboBox * cbox = qobject_cast<QComboBox*>(widget)){
                    cbox->setSizeAdjustPolicy(QComboBox::AdjustToMinimumContentsLengthWithIcon);
                    cbox->setMinimumContentsLength(min(cbox->currentText().size(), 10));
                }

                //dodajemy od razu do elementow toolbara - indeksy zostaja zachowane dla pozniejszego rozmieszczania wg kolejnosci
                auto s = widget->sizeHint();
                int width = min(s.width(), 250);
                if(s.width() > 250){
                    widget->setMaximumWidth(250);
                }
                totalWidth += width;
                maxHeight = max(maxHeight, s.height());
            }else{
                others[i] = titleBarElements[i].first;
            }
        }

        totalElements -= others.size();

        if(totalElements == 0){
            return;
        }

        for(auto it = actions.begin(); it != actions.end(); it++){
            QToolButton * actionButton = new QToolButton();
            if(it->second->toolTip().isEmpty() == true){
                actionButton->setToolTip(it->second->text());
            }

            //actionButton->setIconSize(QSize(20,20));
            actionButton->setFixedSize(20,20);

            actionButton->setDefaultAction(it->second);
            //actionButton->setMaximumSize(QSize(20,20));
            actionButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            toolbarElements[it->first] = actionButton;

            auto s = actionButton->sizeHint();
            totalWidth += s.width();
            maxHeight = max(maxHeight, s.height());
        }

        for(auto it = menus.begin(); it != menus.end(); it++){
            QToolButton * menuButton = new QToolButton();

            if(it->second->toolTip().isEmpty() == true){
                menuButton->setToolTip(it->second->title());
            }

            menuButton->setMenu(it->second);
            //menuButton->setIconSize(QSize(20,20));
            //menuButton->setMaximumHeight(20);

            menuButton->setText(it->second->title());
            menuButton->setPopupMode(QToolButton::InstantPopup);

            menuButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

            toolbarElements[it->first] = menuButton;

            auto s = menuButton->sizeHint();
            totalWidth += s.width();
            maxHeight = max(maxHeight, s.height());
        }

        //rozkladamy to w niezaleznych QToolBarach. Maksymalnie 2 rzedy.
        //TODO
        //dodac maksymalna szerokosc + przerzucanie elementow do ukrytego panelu jesli za duzo ich jest

        int halfWidth = totalWidth;

        int halfElements = totalElements / 2;

        QVBoxLayout * layout = new QVBoxLayout();
        QToolBar * topToolbar = new QToolBar();
        topToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        topToolbar->setIconSize(QSize(20,20));

        layout->addWidget(topToolbar);

        QToolBar * bottomToolbar = nullptr;

        //budujemy 2 wiersze jesli conajmniej 5 elementow
        if(halfElements > 4){
            bottomToolbar = new QToolBar();
            bottomToolbar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            bottomToolbar->setIconSize(QSize(20,20));
            layout->addWidget(bottomToolbar);
            halfWidth /= 2;
        }

        layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Fixed, QSizePolicy::Expanding));

        int currentWidth = 0;
        auto it = toolbarElements.begin();

        while(it != toolbarElements.end()){
            auto s = it->second->sizeHint();
            int width = min(s.width(), 250);
            if(currentWidth + width > halfWidth){
                it++;
            }else{
                topToolbar->addWidget(it->second);
                currentWidth += width;
                it = toolbarElements.erase(it);
            }
        }

        if(bottomToolbar != nullptr){
            it = toolbarElements.begin();

            while(it != toolbarElements.end()){
                bottomToolbar->addWidget(it->second);
                it++;
            }
        }

        //wypelniamy grupe
        QWidget * widget = new QWidget();
        widget->setLayout(layout);
        layout->setContentsMargins(0,0,0,0);
        layout->setMargin(0);
        layout->setSpacing(1);

        visualizersData[contextWidget][QString::fromUtf8("Default operations")] = widget;
    }
}

void HMMVisualizerUsageContext::onUnregisterContextWidget(QWidget * contextWidget)
{
    auto it = visualizersData.find(contextWidget);

    if(it == visualizersData.end()){
        return;
    }

    for(auto sectionIT = it->second.begin(); sectionIT != it->second.end(); sectionIT++){
        try{

            delete sectionIT->second;

        }catch(...)
        {

        }
    }

    visualizersData.erase(it);
}

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

    visualizerUsageContext.reset(new HMMVisualizerUsageContext(flexiTabWidget));

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

        if(widget != nullptr && widget != getCurrentContextWidget()){
            setCurrentContext(widget);
        }
    }
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
    
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)), this, SLOT(onFocusChange(QWidget*,QWidget*)));
	

    trySetStyleByName("hmm");

    this->analisis = new AnalisisWidget(nullptr);
    this->data = new QWidget(nullptr);

    this->data->setContentsMargins(0,0,0,0);

    this->operations = new QWidget(nullptr);
    this->raports = new QWidget(nullptr);

    button2TabWindow[this->dataButton] = this->data;
    button2TabWindow[this->operationsButton] = this->operations;
    button2TabWindow[this->raportsButton] = this->raports;
    button2TabWindow[this->analisisButton] = this->analisis;

    analisisButton->setEnabled(false);
    raportsButton->setEnabled(false);

    for(auto it = button2TabWindow.begin(); it != button2TabWindow.end(); it++) {
        mainArea->layout()->addWidget(it->second);
        it->second->hide();
        bool c = connect(it->first, SIGNAL(clicked()), this, SLOT(onToolButton()));
        UTILS_ASSERT(c);
    }

    this->data->show();
    	
	this->showFullScreen();
    QTreeWidget* treeWidget = this->analisis->getTreeWidget();
    //treeWidget->setColumnCount(2);
    //treeWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));    
	QObject::connect(treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*, int)), this, SLOT(onTreeItemClicked(QTreeWidgetItem*, int)));    

   /* QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);

    splitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);*/

    topMainWindow = new EDRDockWidgetManager(this);
    topMainWindow->setTabsPosition(QTabWidget::North);
    bottomMainWindow = new QMainWindow(this);
    bottomMainWindow->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    createFilterTabs();

    QWidget* analisisArea = analisis->getArea();
    QLayout* v = analisisArea->layout() ? analisisArea->layout() : new QGridLayout(analisisArea);
    
    v->setMargin(0);
    v->setContentsMargins(QMargins(0, 0, 0, 0));
    v->addWidget(topMainWindow->asQWidget());
    topMainWindow->asQWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    bottomMainWindow->layout()->setMargin(0);
    bottomMainWindow->layout()->setContentsMargins(QMargins(0, 0, 0, 0));
    analisisArea->setLayout(v);

    int i = toolBar->children().size();

    // akcje - Workflow (VDF) i konsola <--- aktualnie œmietnik na inne serwisy i testy

    QMainWindow * actionsMainWindow = new QMainWindow(nullptr);
    QVBoxLayout* layout = new QVBoxLayout();

	for (int i = 0; i < ServiceManager::getInstance()->getNumServices(); ++i) {
		IServicePtr service = ServiceManager::getInstance()->getService(i);

		const std::string& name = service->getName();
		// HACK! nie da sie na razie castowac na CommunicationService
		// trzeba by do HmmView dodac zaleznosc od tego pluginu (lub jego bibliotek)
		// Nie da sie jednak includowac z tego poziomu odpowiedniego naglowka 
		// (gdzies po drodze wymagane sa prywane naglowki z Communication)
		// Moze wlasciwe bedzie identyfikowanie po UniqueID.
		//if (name == "Communication") {
        if (name == "DataExplorer") {
			std::vector<QObject*> mainWidgetActions;
			QWidget* viewWidget = service->getWidget(mainWidgetActions);

			std::vector<QObject*> controlWidgetActions;
			QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

			std::vector<QObject*> settingsWidgetActions;
			QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

			QVBoxLayout *layout = new QVBoxLayout;
            layout->addWidget(settingsWidget);
            layout->addWidget(viewWidget);
            layout->addWidget(controlWidget);
			this->data->setLayout(layout);
        }else if (name == "newTimeline") {
            showTimeline();
        }else{
            std::vector<QObject*> mainWidgetActions;
            QWidget* viewWidget = service->getWidget(mainWidgetActions);

            std::vector<QObject*> controlWidgetActions;
            QWidget* controlWidget = service->getControlWidget(controlWidgetActions);

            std::vector<QObject*> settingsWidgetActions;
            QWidget* settingsWidget = service->getSettingsWidget(settingsWidgetActions);

            layout->addWidget(settingsWidget);
            layout->addWidget(viewWidget);
            layout->addWidget(controlWidget);
        }
	}

    // akcje - Workflow (VDF) i konsola
	EDRWorkflowWidget* widget = new EDRWorkflowWidget();
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widget);
    initializeConsole();
    actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, widgetConsole);
    layout->addWidget(actionsMainWindow);

	operations->setLayout(layout);

    toolsGroupID = flexiTabWidget->addGroup(QString::fromUtf8("Tools"));
    visualizerGroupID = flexiTabWidget->addGroup(QString::fromUtf8("Visualizer"), QIcon(), false);

    //TODO
    //Tak dlugo jak nie mamy raportow chowamy je w wersji release
    //Podobnie odnosnik do stronki
    #ifndef _DEBUG
        raportsButton->setVisible(false);
        openButton->setVisible(false);
    #endif
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
        showTimeline();
        VisualizerWidget* w = createDockVisualizer(action->getItemHelper());
        topMainWindow->autoAddDockWidget( w );
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
            addSeriesToTimeline(series, path, visualizer);
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

    if(currentItem == nullptr || isDataItem(currentItem) == false){
        return;
    }

    TreeItemHelper* helper = dynamic_cast<TreeItemHelper*>(currentItem);

    if (helper) {
        QMenu * menu = new QMenu(treeWidget);
        if(items2Descriptions.find(currentItem) == items2Descriptions.end()){
            QAction * addNew = new ContextAction(helper, menu);
            addNew->setText(tr("Add to new visualizer"));
            menu->addAction(addNew);
            connect(addNew, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));

            BOOST_FOREACH(EDRDockWidgetSet* set, topMainWindow->getDockSet()) {
                QMenu* group = new QMenu(set->windowTitle(), menu);
                menu->addMenu(group);
                BOOST_FOREACH(EDRDockWidget* dock, set->getDockWidgets()) {
                    VisualizerWidget* vw = dynamic_cast<VisualizerWidget*>(dock);
                    if (vw ) {
                        VisualizerPtr visualizer = vw->getCurrentVisualizer();
                        QAction* addAction = new ContextAction(helper, group, visualizer);
                        addAction->setText(vw->windowTitle());
                        connect(addAction, SIGNAL(triggered()), this, SLOT(addToVisualizer()));
                        group->addAction(addAction);
                    }
                }
            }
            //menu->addAction("Add to existing visualizer");
        }else{
            QAction * action = new QAction(menu);
            action->setText(QString::fromUtf8("Remove"));
            menu->addAction(action);
        }    

        menu->exec(treeWidget->mapToGlobal(pos));
    }
}

//void HmmMainWindow::onOpen()
//{
//	utils::Push<bool> pushed(updateEnabled, false);
//	Filesystem::Path initPath = getUserDataPath() / "trial";
//	const QString directory = QFileDialog::getExistingDirectory(this, 0, initPath.string().c_str());
//	if (!directory.isEmpty()) 
//	{
//		LoadingDialog* d = new LoadingDialog();
//		//d->setWindowFlags(Qt::Tool);
//		d->start(directory);
//		delete d;
//	}
//
//	std::vector<SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance());
//    currentSessions = sessions;
//    QTreeWidgetItem* item = TreeBuilder::createTree("Sessions", sessions);
//    analisis->getTreeWidget()->addTopLevelItem(item);
//}

void HmmMainWindow::onTreeItemClicked( QTreeWidgetItem *item, int column )
{
    try {
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
    } catch ( std::exception& e) {
        LOG_ERROR("Click on tree failed, reason : " << e.what());
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

                EDRDockWidget * widget = new EDRDockWidget(nullptr);
                widget->setTitleBarWidget(new QWidget());
                widget->titleBarWidget()->setEnabled(false);
                /*QLayout* layout = widget->getInnerWidget()->layout();
                layout->setMargin(0);
                layout->setContentsMargins(QMargins(0, 0, 0, 0));
                layout->addWidget(controlWidget);*/
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
    /*
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
    */

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
    filter1->addFilter(tr("GRF"), "label", grfCommand, &iconGRFSmall);

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
    filter1->addFilter(tr("EMG"), "emg label", emgCommand, &iconEmgSmall);

    DataFilterPtr typeFilter3(new TypeFilter(typeid(ForceCollection)));
    DataFilterPtr typeFilter4(new TypeFilter(typeid(MomentCollection)));
    DataFilterPtr typeFilter5(new TypeFilter(typeid(PowerCollection)));

    
    /*typedef Vector3DFilterCommand2<MomentChannel, MomentCollection, NewVector3ItemHelper> MomentsCommand;
    typedef Vector3DFilterCommand2<ForceChannel, ForceCollection, NewVector3ItemHelper> ForcesCommand;
    typedef Vector3DFilterCommand2<PowerChannel, PowerCollection, NewVector3ItemHelper> PowerCommand;*/

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
    
    filter2->addFilter(tr("FORCES"), "count: ", forceFilter, &iconForceSmall);
    filter2->addFilter(tr("MOMENTS"), "test", momentFilter, &iconMomentSmall);
    filter2->addFilter(tr("POWERS"), "powers", powerFilter, &iconPowerSmall);

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

    filter3->addFilter(tr("MARKERS"), "count: 1", markersFilter, &iconMarkerSmall);
    filter3->addFilter(tr("JOINTS"), "count: 1", jointsFilter, &iconJointSmall);
    
    /*DataFilterPtr typeFilter8(new TypeFilter(typeid(VideoChannel)));
    filter4->addFilter(tr("VIDEOS"), "count: 4", typeFilter8, &iconVideoSmall);*/
    IFilterCommandPtr videoFilter(new BuilderFilterCommand(TreeBuilder::createVideoBranch, 
        TreeBuilder::getRootVideoIcon(), TreeBuilder::getVideoIcon()));
    filter4->addFilter(tr("VIDEOS"), "count: 4", videoFilter, &iconVideoSmall);
    
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
    /*
    QPixmap iconKinetic(core::getResourceString("icons/kineticBig.png"));
    QPixmap iconIllness(core::getResourceString("icons/jed.chorobowe.png"));
    QPixmap iconEndo(core::getResourceString("icons/po_endoplastyce.png"));
    QPixmap iconStroke(core::getResourceString("icons/po_udarze.png"));
    QPixmap iconSpine(core::getResourceString("icons/zwyrodnienia.png"));
    */

    QPixmap iconKinetic(QString::fromUtf8(":/resources/icons/kineticBig.png"));
    QPixmap iconIllness(QString::fromUtf8(":/resources/icons/jed.chorobowe.png"));
    QPixmap iconEndo(QString::fromUtf8(":/resources/icons/po_endoplastyce.png"));
    QPixmap iconStroke(QString::fromUtf8(":/resources/icons/po_udarze.png"));
    QPixmap iconSpine(QString::fromUtf8(":/resources/icons/zwyrodnienia.png"));
   
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
    DataFilterPtr typeFilter1(new TypeFilter(typeid(GRFChannel)));
    DataFilterPtr typeFilter2(new TypeFilter(typeid(EMGChannel)));

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

    //QPixmap big(core::getResourceString(":/resources/icons/Big.png"));
    //this->analisis->setActivePixmapAndText(big, "ALL");
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
            /*this->clearTree();
            QTreeWidgetItem* item = TreeBuilder::createTree(tr("Active Data"), getCurrentSessions());
            this->addItemToTree(item);
            QPixmap big(core::getResourceString("icons/Big.png"));
            this->analisis->setActivePixmapAndText(big, "ALL");*/
            refreshTree();
        } else {
            //QPixmap pix(dataWidget->getPixmap());
            //QString name = dataWidget->getName();
            //this->analisis->setActivePixmapAndText(pix, name);
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


void HmmMainWindow::visualizerDestroyed(QObject * visualizer)
{
    removeContext(qobject_cast<QWidget*>(visualizer));
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
    visualizerDockWidget->setPermanent(false);
    visualizerDockWidget->setAllowedAreas(Qt::TopDockWidgetArea | Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea | Qt::BottomDockWidgetArea);
    visualizerDockWidget->setVisualizerIconVisible(false);
    visualizerDockWidget->setVisualizerSwitchEnable(false);
    visualizerDockWidget->setVisualizerSwitchVisible(false);
    visualizerDockWidget->setSourceVisible(false);

    EDRTitleBar * titleBar = supplyWithEDRTitleBar(visualizerDockWidget);

    std::vector<VisualizerTimeSeriePtr> series;
    hmmItem->getSeries(visualizer, path, series);
    visualizer->getWidget()->setFocusProxy(visualizerDockWidget);

    connect(visualizerDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(visualizerDestroyed(QObject *)));

    addSeriesToTimeline(series, path, visualizer);

    //kontekst wizualizatora!!
    addContext(visualizerUsageContext);
    addWidgetToContext(visualizerUsageContext, visualizerDockWidget);

    return visualizerDockWidget;
}

 void HmmMainWindow::addSeriesToTimeline( const std::vector<core::VisualizerTimeSeriePtr> &series, const QString &path, const VisualizerPtr &visualizer )
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
                 } catch (...) {
                     LOG_ERROR("Could not add multichannel to timeline!");
                 }
             }
         }
     }
 }

 void HmmMainWindow::refreshTree()
 {
     std::vector<SessionConstPtr> sessions = core::queryDataPtr(DataManager::getInstance());
     currentSessions = sessions;
     QTreeWidgetItem* item = TreeBuilder::createTree("Active Data", sessions);
     
     QTreeWidget* tree = analisis->getTreeWidget();
     tree->clear();
     tree->addTopLevelItem(item);
     item->setExpanded(true);

     QFont font = item->font(0);
     font.setPointSize(font.pointSize() + 4);
     item->setFont(0, font);

     for (int i = 0; i < item->childCount(); i++) {
         QTreeWidgetItem* child = item->child(i);
         child->setExpanded(true);

         QFont font = item->font(0);
         font.setPointSize(font.pointSize() + 3);
         item->setFont(0, font);
     }
 }


 void HmmMainWindow::DataObserver::update( const core::IMemoryDataManager * subject )
 {
     std::vector<MotionConstPtr> motions = core::queryDataPtr(DataManager::getInstance());
     int count = motions.size();
     if (motionsCount == 0 && count > 0) {
         hmm->analisisButton->setEnabled(true);
         hmm->raportsButton->setEnabled(true);
     }
     if (motionsCount != count) {
        hmm->refreshTree();
        motionsCount = count;
     }
 }
