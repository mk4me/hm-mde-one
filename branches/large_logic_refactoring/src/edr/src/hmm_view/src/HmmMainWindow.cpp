#include "hmmPCH.h"

#include <QtGui/QMenu>
#include <QtGui/QSplashScreen>
#include <cmath>
#include <utils/Debug.h>
#include "HmmMainWindow.h"
#include "DataFilterWidget.h"
#include "TreeBuilder.h"
#include "Vector3DFilterCommand.h"
#include "AnalisisWidget.h"
#include <plugins/subject/ISubjectService.h>
#include <plugins/c3d/C3DChannels.h>
#include <plugins/newChart/INewChartVisualizer.h>
#include "IllnessUnit.h"
#include "EMGFilter.h"
#include <coreui/CoreTextEditWidget.h>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include "AboutDialog.h"
#include "ContextAction.h"
#include "ContextEventFilter.h"
#include <corelib/IServiceManager.h>
#include <corelib/ISourceManager.h>
#include <coreui/CoreDockWidget.h>
#include <corelib/IDataHierarchyManagerReader.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newVdf/NewVdfService.h>
#include "HmmVdf.h"

using namespace core;

using namespace PluginSubject;

void unpackSessions(const core::ObjectWrapperCollection & owc, std::vector<SessionConstPtr> & dest)
{
	for(auto it = owc.begin(); it != owc.end(); ++it){
		SessionConstPtr val;
		bool ok = (*it)->tryGet(val, false);
		if(ok && val != nullptr){
			dest.push_back(val);
		}
	}
}

void HmmMainWindow::update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication )
{
	if(modyfication == core::Visualizer::REMOVE_SERIE){
		auto it = seriesToChannels.find(serie);
		if(it != seriesToChannels.end()){
			std::string path = it->second;
			auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
			if(timeline != nullptr){
				timeline->removeChannel(it->second);
			}

			seriesToChannels.erase(it);

			//TODO
			//przejrzec caly ten mechanizm bo bez sensu teraz jest to robione
			//usunąć wpisy dla pozostałych serii bo kanal usuwamy tylko raz

			auto tIT = seriesToChannels.begin();
			while(tIT != seriesToChannels.end()){
				if(tIT->second == path){
					auto toErase = tIT;
					++tIT;
					seriesToChannels.erase(toErase);
				}else{
					++tIT;
				}
			}

			//teraz usuwam wpisy dla menu
			for(auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it)
			{
				if(it->second.path == path){
					items2Descriptions.erase(it);
					break;
				}
			}
		}
	}
}

HmmMainWindow::HmmMainWindow(const CloseUpOperations & closeUpOperations) :
    coreUI::CoreMainWindow(closeUpOperations),
    currentVisualizer(nullptr),
    topMainWindow(nullptr),
	bottomMainWindow(nullptr),
    analisis(nullptr),
    data(nullptr),
	vdf(nullptr),
    operations(nullptr),
	raports(nullptr),
	currentButton(nullptr),
    dataObserver(new DataObserver(this)),
    summaryWindow(new SummaryWindow(this)),
    summaryWindowController(new SummaryWindowController(summaryWindow, this))
{
    setupUi(this);
    connect(actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

    {
        processedBranch = new QTreeWidgetItem();
        processedBranch->setText(0, tr("Processing"));
        treeRefresher.setProcessed(processedBranch);
    }

	contextPlaceholder = new QTabWidget(this);
	contextPlaceholder->setTabsClosable(false);
	//contextPlaceholder->setDocumentMode(true);
	contextPlaceholder->setMovable(false);
	contextPlaceholder->setVisible(false);

	connect(contextPlaceholder, SIGNAL(currentChanged(int)), this, SLOT(onContextChange(int)));
    connect(this, SIGNAL(onSwitchToAnalysis()), this, SLOT(safeSwitchToAnalysis()), Qt::QueuedConnection);

    visualizerUsageContext.reset(new HMMVisualizerUsageContext(contextPlaceholder));
    vdfUsageContext.reset(new HMMVVdfUsageContext(contextPlaceholder));
    treeUsageContext.reset(new HMMTreeItemUsageContext(contextPlaceholder, this));
    raportsThumbnailsContext.reset(new RaportsThumbnailsContext(contextPlaceholder, this));
    raportsTabContext.reset(new RaportsTabContext(contextPlaceholder, this));
    tabPlaceholder->layout()->addWidget(contextPlaceholder);

	contextPlaceholder->findChild<QTabBar*>()->setDrawBase(false);

    dataContext.reset(new HMMDataContext());
    analisisContext.reset(new HMMAnalysisContext(&treeRefresher));
    reportsContext.reset(new HMMReportContext());
    vdfContext.reset(new HMMVdfContext());

    this->setWindowFlags(Qt::FramelessWindowHint);
    setMouseTracking(true);
    contextEventFilter = new ContextEventFilter(this);
}

void HmmMainWindow::onContextChange(int idx)
{
	if(contextPlaceholder->count() == 0){
		contextPlaceholder->setVisible(false);
	}else{
		contextPlaceholder->setVisible(true);
	}
}

void HmmMainWindow::initializeSplashScreen(QSplashScreen * splashScreen)
{
	splashScreen->setPixmap(QPixmap(":/resources/splashscreen/splash.png"));
}

void HmmMainWindow::showSplashScreenMessage(const QString & message)
{
	splashScreen()->showMessage(message, Qt::AlignBottom | Qt::AlignLeft, Qt::white);
}

void HmmMainWindow::activateContext(QWidget * widget)
{
    QWidget* toSet = nullptr;
    if(plainContextWidgets.find(widget) != plainContextWidgets.end()){
        toSet = widget;
    }else{
        auto it = derrivedContextWidgets.left.find(widget);

		if(it != derrivedContextWidgets.left.end()){
			toSet = it->second;
		}
    }

    setCurrentContext(toSet);

    // hack - nie da się zinstalować dwoch filtrow eventów dla jednego widgeta,
    // obecne rozwiazanie jest specyficzne dla kontekstów
    coreUI::CoreVisualizerWidget* vw = dynamic_cast<coreUI::CoreVisualizerWidget*>(toSet);
    if (vw) {
        summaryWindowController->onVisualizator(vw);
    }

	if(contextPlaceholder->count() > 0){
		contextPlaceholder->setVisible(true);
	}
}

void HmmMainWindow::deactivateContext(QWidget * widget)
{
    if(widget == nullptr || widget == getCurrentContextWidget()){
        auto context = getCurrentContext();
        if(context != nullptr){
            setCurrentContext(context);
        }else{
            setCurrentContext((QWidget*)nullptr);
        }
    }

	if(contextPlaceholder->count() == 0){
		contextPlaceholder->setVisible(false);
	}
}


void HmmMainWindow::customViewInit(QWidget * console)
{
	auto memoryManager = plugin::getDataManagerReader();
	memoryManager->addObserver(dataObserver);

    addContext(dataContext);
    addContext(analisisContext);
    addContext(reportsContext);
    addContext(vdfContext);
    addContext(visualizerUsageContext, analisisContext);

    trySetStyleByName("hmm");
    this->analisis = new AnalisisWidget(nullptr, this);
    QTreeWidget* treeWidget = this->analisis->getTreeWidget();
    summaryWindow->initialize();

    addContext(treeUsageContext, analisisContext);
    addWidgetToContext(treeUsageContext, treeWidget);
    plainContextWidgets.insert(treeWidget);
    contextEventFilter->registerPermamentContextWidget(treeWidget);


    addContext(raportsThumbnailsContext, analisisContext);
    addWidgetToContext(raportsThumbnailsContext, analisis->scrollArea_3);
    plainContextWidgets.insert(analisis->scrollArea_3);
    contextEventFilter->registerPermamentContextWidget(analisis->scrollArea_3);

    QTabWidget * dataTabWidget = coreUI::createNamedObject<QTabWidget>(QString::fromUtf8("dataTabWidget"));

    this->data = dataTabWidget;

    this->data->setContentsMargins(0,0,0,0);
    this->analisis->setContentsMargins(0,0,0,0);

	QTabWidget * vdfTabWidget = coreUI::createNamedObject<QTabWidget>(QString::fromUtf8("VDF Widget"));
	this->vdf = vdfTabWidget;

    this->operations = new QWidget();
    this->raports = new coreUI::CoreTextEditWidget();
    addContext(raportsTabContext, reportsContext);
    addWidgetToContext(raportsTabContext, this->raports);
    contextEventFilter->registerPermamentContextWidget(this->raports);
    plainContextWidgets.insert(this->raports);
    button2TabWindow[this->dataButton] = this->data;
    button2TabWindow[this->operationsButton] = this->operations;
    button2TabWindow[this->raportsButton] = this->raports;
    button2TabWindow[this->analisisButton] = this->analisis;
	button2TabWindow[this->processingButton] = this->vdf;
    currentButton = dataButton;

	contextStates[this->analisisButton] = ContextState(analisisContext, nullptr);

    analisisButton->setEnabled(false);
    raportsButton->setEnabled(false);

    for(auto it = button2TabWindow.begin(); it != button2TabWindow.end(); ++it) {
        mainArea->layout()->addWidget(it->second);
        it->second->hide();
        bool c = connect(it->first, SIGNAL(toggled(bool)), this, SLOT(onToolButton(bool)));
        UTILS_ASSERT(c);
    }

    treeWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(treeWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onTreeContextMenu(const QPoint&)));
    QObject::connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), summaryWindowController, SLOT(onTreeItemSelected(QTreeWidgetItem* , int)));

    topMainWindow = new coreUI::CoreDockWidgetManager();
	topMainWindow->setSetsClosable(true);
    topMainWindow->setTabPosition(QTabWidget::South);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomMainWindow = new QFrame();
	bottomMainWindow->setObjectName(QString("bottomMainWindow"));
	bottomMainWindow->setLayout(new QVBoxLayout);
	bottomMainWindow->layout()->setContentsMargins(0,0,0,0);
    bottomMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(topMainWindow, SIGNAL(currentSetChanged(int)), treeUsageContext.get(), SLOT(refresh()));
	connect(topMainWindow, SIGNAL(setCloseRequested(int)), this, SLOT(onTabCloseRequest(int)));
    //TODO
    //trzeba wyzanaczać max wysokość tak aby nie "wypychało" nam timeline!!
    // powinno być mniej więcej coś takiego - wysokość dla aktualnej rozdzielczości - wysokośc timeline - wysokość górnego paska - przestrzeń wolna pomiędzy tymi elementami w pionie
    // ~ currentRes.h - 270 w przybliżeniu -> trtzeba dokłądnie sprawdzić i policzyć
    //topMainWindow->setMaximumHeight(850);
    //topMainWindow->setMinimumHeight(850);

    createFilterTabs();

    QWidget* analisisArea = analisis->getArea();
    QLayout* v = analisisArea->layout() ? analisisArea->layout() : new QGridLayout(analisisArea);

    v->setMargin(0);
    v->setContentsMargins(QMargins(0, 0, 0, 0));
    v->addWidget(topMainWindow);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    v->addWidget(bottomMainWindow);
    //TODO
    //zapewniamy timelie minimalną, ale wystarczająca ilośc miejsca
    //trzeba to zgrać razem z max wysokością dla wizualizatorów
    bottomMainWindow->setMaximumHeight(120); // tymczasowo
    bottomMainWindow->setMinimumHeight(120); // tymczasowo
    //bottomMainWindow->layout()->setMargin(10);
    //bottomMainWindow->layout()->setContentsMargins(QMargins(0, 0, 0, 0));
    analisisArea->setLayout(v);


    // akcje - Workflow (VDF) i konsola <--- aktualnie śmietnik na inne serwisy i testy
    QMainWindow * actionsMainWindow = new QMainWindow(nullptr);
    QVBoxLayout* layout = new QVBoxLayout();

    for (int i = 0; i < plugin::getServiceManager()->getNumServices(); ++i) {
        plugin::IServicePtr service = plugin::getServiceManager()->getService(i);

		auto timeline = core::dynamic_pointer_cast<ITimelineService>(service);
		auto vdf = core::dynamic_pointer_cast<vdf::NewVdfService>(service);
        if(timeline != nullptr) {
            showTimeline();
        } else if (vdf) {
			QWidget* viewWidget = vdf->getWidget();
			QWidget* controlWidget = vdf->getControlWidget();
			QWidget* settingsWidget = vdf->getSettingsWidget();
			controlWidget->setMaximumWidth(350);
			viewWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			controlWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
			settingsWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
			QVBoxLayout* vlayout = new QVBoxLayout();
			vlayout->addWidget(viewWidget);
			//vlayout->addWidget(settingsWidget);
			QWidget* widget = new QWidget();
			widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			widget->setLayout(vlayout);
			QHBoxLayout* hlayout = new QHBoxLayout();
			hlayout->addWidget(widget);
			hlayout->addWidget(controlWidget);
			vdfTabWidget->setLayout(hlayout);

            addContext(vdfUsageContext, vdfContext);
            addWidgetToContext(vdfUsageContext, viewWidget);
            addWidgetToContext(vdfUsageContext, this->vdf);
            addWidgetToContext(vdfUsageContext, controlWidget);
            addWidgetToContext(vdfUsageContext, processingButton);
            contextEventFilter->registerPermamentContextWidget(this->processingButton);
            plainContextWidgets.insert(this->processingButton);
            contextEventFilter->registerClosableContextWidget(this->processingButton);

            contextEventFilter->registerPermamentContextWidget(viewWidget);
            plainContextWidgets.insert(viewWidget);

            contextEventFilter->registerClosableContextWidget(viewWidget);

            HmmVdf::registerNodes(this);

		} else {
            QWidget* viewWidget = service->getWidget();
            QWidget* controlWidget = service->getControlWidget();
            QWidget* settingsWidget = service->getSettingsWidget();

            if(settingsWidget){
				settingsWidget->setParent(operations);
                layout->addWidget(settingsWidget);
            }

            if(viewWidget){
				viewWidget->setParent(operations);
                layout->addWidget(viewWidget);
            }

            if(controlWidget){
				controlWidget->setParent(operations);
                layout->addWidget(controlWidget);
            }
        }
    }

	auto dockConsole = embeddWidget(console, tr("Console"), Qt::BottomDockWidgetArea, true);
	actionsMainWindow->addDockWidget(Qt::BottomDockWidgetArea, dockConsole);
	dockConsole->setFeatures(dockConsole->features() | QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	dockConsole->setObjectName(QString::fromUtf8("ConsoleWidget"));
    layout->addWidget(actionsMainWindow);

    operations->setLayout(layout);

    for (int i = 0; i < plugin::getSourceManager()->getNumSources(); ++i) {
        auto source = plugin::getSourceManager()->getSource(i);

        dataTabWidget->addTab(source->getWidget(), QString::fromStdString(source->getName()));
        //TODO
        //obsłużyc konteksy źródeł
    }

    //chowamy zakładki jeśli tylko jedno źródło
    if(dataTabWidget->count() == 1){
        QList<QTabBar*> tabBars = dataTabWidget->findChildren<QTabBar*>();
        for(auto it = tabBars.begin(); it != tabBars.end(); ++it){
            if((*it)->parent() == dataTabWidget){
                (*it)->setVisible(false);
                break;
            }
        }
    }

    //TODO
    //Tak dlugo jak nie mamy raportów chowamy je w wersji release
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
    treeRefresher.setPreventRefresh(true);
    this->data->show();
}

void HmmMainWindow::setCurrentVisualizerActions(coreUI::CoreVisualizerWidget * visWidget)
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
    delete contextEventFilter;
}

bool HmmMainWindow::isDataItem(QTreeWidgetItem * item)
{
    HmmTreeItem* hmmItem = dynamic_cast<HmmTreeItem*>(item);
    return hmmItem && hmmItem->getHelper()->isDataItem();
}


void HmmMainWindow::createNewVisualizer()
{
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    try{
        createNewVisualizer(action->getTreeItem(), action->getDockSet());
    }catch(std::exception& e ){
        PLUGIN_LOG_ERROR("Error creating visualizer: " << e.what());
    } catch (...) {
        PLUGIN_LOG_ERROR("Error creating visualizer");
    }
}

//void HmmMainWindow::createSource()
//{
//    //static boost::uuids::random_generator random_uuid;
//    //static bool hack = false;
//    //ContextAction* action = qobject_cast<ContextAction*>(sender());
//    //try{
//    //    HmmTreeItem* treeItem = action->getTreeItem();
//    //    TreeWrappedItemHelperPtr wrapped = core::dynamic_pointer_cast<TreeWrappedItemHelper>(treeItem->getHelper());
//
//    //    core::IServiceManager* manager = plugin::getServiceManager();
//    //    vdf::NewVdfServicePtr vdfService = core::dynamic_pointer_cast<vdf::NewVdfService>(manager->getService(vdf::NewVdfService::getClassID()));
//    //    VectorChannelReaderInterfaceConstPtr item = wrapped->getWrapper()->get();
//
//    //    vdfService->registerDataSource(vdf::IDataSourcePtr(new vdf::DataSource(new XSource(item), core::UniqueID(random_uuid()), wrapped->getText().toStdString(), 
//    //        [&](const df::ISourceNode* prototype) -> XSource*
//    //        { 
//    //            auto source = new XSource();
//    //            auto proto = dynamic_cast<const XSource*>(prototype);
//    //            source->setChannel(proto->getChannel());
//    //            return source; 
//    //        })));
//
//    //    if (!hack) {
//    //        hack = true;
//    //        vdfService->registerDataSink(vdf::IDataSinkPtr(new vdf::DataSink(new XSink(this), core::UniqueID(random_uuid()), "Vector -> Tree", 
//    //            [&](const df::ISinkNode* prototype) -> XSink*
//    //        { 
//    //            auto sink = new XSink();
//    //            auto proto = dynamic_cast<const XSink*>(prototype);
//    //            sink->setHmm(proto->getHmm());
//    //            return sink; 
//    //        })));
//
//    //        vdfService->registerDataProcessor(vdf::IDataProcessorPtr(new vdf::DataProcessor(new XProcessor(), core::UniqueID(random_uuid()), "Vector Differ", 
//    //            [&](const df::IProcessingNode* prototype) -> XProcessor*
//    //        { 
//    //            auto p = new XProcessor();
//    //            //auto proto = dynamic_cast<const XProcessor*>(prototype);
//    //            //source->setChannel(proto->getChannel());
//    //            return p; 
//    //        })));
//    //    }
//    //}catch(std::exception& e ){
//    //    PLUGIN_LOG_ERROR("Error creating source: " << e.what());
//    //} catch (...) {
//    //    PLUGIN_LOG_ERROR("Error creating source");
//    //}
//}

void HmmMainWindow::createNewVisualizer( HmmTreeItem* item, coreUI::CoreDockWidgetSet* dockSet )
{    
    createAndAddDockVisualizer(item, dockSet);
}

void HmmMainWindow::createVisualizerInNewSet()
{
	static const QString setName = tr("Group %1");
    ContextAction* action = qobject_cast<ContextAction*>(sender());
    coreUI::CoreDockWidgetSet* set = new coreUI::CoreDockWidgetSet(topMainWindow);
    topMainWindow->addDockWidgetSet(set, setName.arg(topMainWindow->count()+1));

    createAndAddDockVisualizer(action->getTreeItem(), set);
}

void HmmMainWindow::onTabCloseRequest(int tab)
{
	auto set = topMainWindow->set(tab);
	topMainWindow->removeSet(tab);
	delete set;
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

void HmmMainWindow::highlightVisualizer(const VisualizerPtr& visualizer )
{
    for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it) {
        DataItemDescription desc = it->second;
        if (desc.visualizerWidget->getVisualizer() == visualizer) {
            // todo: optymalniej!
            coreUI::CoreDockWidgetSet* set = topMainWindow->tryGetDockSet(desc.visualizerDockWidget);
            if (set) {
                topMainWindow->setCurrentSet(set);
            }
            desc.visualizerWidget->setStyleSheet(QString::fromUtf8(
                "coreUI--CoreVisualizerWidget {" \
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
    try {
        VisualizerPtr visualizer = action->getVisualizer();
        INewChartVisualizer* newChart = dynamic_cast<INewChartVisualizer*>(visualizer->visualizer());
        if (newChart) {
            newChart->setTitle(tr("Multichart"));
        }

        HmmTreeItem* item = action->getTreeItem();
        auto helper = item->getHelper();
        static int counter = 0;
        QString path = QString("Custom_addition...%1").arg(counter++);

        std::vector<core::Visualizer::VisualizerSerie*> series;
        helper->getSeries(visualizer, path, series);
            
		//TODO - obsługa timeline
		auto channel = core::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
		auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
		timeline->addChannel(path.toStdString(), channel);

		coreUI::CoreVisualizerWidget* vw = nullptr;
		QDockWidget* vd = nullptr;
		for (auto it = items2Descriptions.begin(); it != items2Descriptions.end(); ++it) {
			DataItemDescription& d = it->second;
			if (d.visualizerWidget->getVisualizer() == visualizer) {
				vw = d.visualizerWidget;
				vd = d.visualizerDockWidget;
				break;
			}
		}
		UTILS_ASSERT(vw);
		DataItemDescription desc(vw, vd);
		desc.channel = channel;
		desc.path = path.toStdString();

		for(auto it = series.begin(); it != series.end(); ++it){
			seriesToChannels[*it] = desc.path;
		}

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


void HmmMainWindow::onTreeContextMenu(const QPoint & pos)
{
    QTreeWidget* treeWidget = analisis->getTreeWidget();
    auto currentItem = treeWidget->itemAt(pos);
    treeWidget->setCurrentItem(currentItem);
    if(currentItem == nullptr || isDataItem(currentItem) == false){
        return;
    }

    HmmTreeItem* item = dynamic_cast<HmmTreeItem*>(currentItem);

    if (item) {
        QMenu* menu = getContextMenu(treeWidget, item);
        menu->exec(treeWidget->mapToGlobal(pos));
    }
}

void HmmMainWindow::showTimeline()
{
    static bool timelineVisible = false;
    if (timelineVisible == false) {
        for (int i = 0; i < plugin::getServiceManager()->getNumServices(); ++i) {
            plugin::IServicePtr service = plugin::getServiceManager()->getService(i);

            auto timeline = core::dynamic_pointer_cast<ITimelineService>(service);
            if(timeline != nullptr) {

                QWidget* viewWidget = service->getWidget();

                QWidget* controlWidget = service->getControlWidget();
                
                QWidget* settingsWidget = service->getSettingsWidget();
				dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
				bottomMainWindow->layout()->addWidget(controlWidget);
				dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
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

    IFilterCommandPtr grfCommand(new BuilderFilterCommand(TreeBuilder::createGRFBranch, TreeBuilder::getRootGRFIcon(), TreeBuilder::getGRFIcon()));
    filter1->addFilter(tr("GRF"), grfCommand, &iconGRFSmall);

    QString emgFront = QString::fromStdString(plugin::getResourcePath("images/muscular_front/muscular_front.xml").string());
    QString emgBack = QString::fromStdString(plugin::getResourcePath("images/muscular_back/muscular_back.xml").string());

    NamesDictionary emgNames;
    emgNames["krawiecki_l"                ] = std::make_pair("krawiecki_l"                , "krawiecki_l"                );
    emgNames["krawiecki_p"                ] = std::make_pair("krawiecki_p"                , "krawiecki_p"                );
    emgNames["naramienny_l"               ] = std::make_pair("naramienny_l"               , "naramienny_l"               );
    emgNames["naramienny_p"               ] = std::make_pair("naramienny_p"               , "naramienny_p"               );
    emgNames["odwodziciel_krotki_kciuka_l"] = std::make_pair("odwodziciel_krotki_kciuka_l", "odwodziciel_krotki_kciuka_l");
    emgNames["odwodziciel_krotki_kciuka_p"] = std::make_pair("odwodziciel_krotki_kciuka_p", "odwodziciel_krotki_kciuka_p");
    emgNames["piersiowy_wiekszy_l"        ] = std::make_pair("piersiowy_wiekszy_l"        , "piersiowy_wiekszy_l"        );
    emgNames["piersiowy_wiekszy_p"        ] = std::make_pair("piersiowy_wiekszy_p"        , "piersiowy_wiekszy_p"        );
    emgNames["piszczelowy_przedni_l"      ] = std::make_pair("piszczelowy_przedni_l"      , "piszczelowy_przedni_l"      );
    emgNames["piszczelowy_przedni_p"      ] = std::make_pair("piszczelowy_przedni_p"      , "piszczelowy_przedni_p"      );
    emgNames["prosty_uda_l"               ] = std::make_pair("prosty_uda_l"               , "prosty_uda_l"               );
    emgNames["prosty_uda_p"               ] = std::make_pair("prosty_uda_p"               , "prosty_uda_p"               );
    emgNames["ramie3"                     ] = std::make_pair("ramie3"                     , "ramie3"                     );
    emgNames["ramie3R"                    ] = std::make_pair("ramie3R"                    , "ramie3R"                    );
    emgNames["strzalkowy_p"               ] = std::make_pair("strzalkowy_p"               , "strzalkowy_p"               );
    emgNames["strzalkowy_l"               ] = std::make_pair("strzalkowy_l"               , "strzalkowy_l"               );
    emgNames["przy_kstrzalkowej_piszczelowy_p"] = std::make_pair("przy_kstrzalkowej_piszczelowy_p", "przy_kstrzalkowej_piszczelowy_p");
    emgNames["przy_kstrzalkowej_piszczelowy_l"] = std::make_pair("przy_kstrzalkowej_piszczelowy_l", "przy_kstrzalkowej_piszczelowy_l");

    emgNames["brzuchaty_lydki_gb_l"       ] = std::make_pair("brzuchaty_lydki_gb_l"       , "brzuchaty_lydki_gb_l"       );
    emgNames["brzuchaty_lydki_gb_p"       ] = std::make_pair("brzuchaty_lydki_gb_p"       , "brzuchaty_lydki_gb_p"       );
    emgNames["brzuchaty_lydki_gprzysr_l"  ] = std::make_pair("brzuchaty_lydki_gprzysr_l"  , "brzuchaty_lydki_gprzysr_l"  );
    emgNames["brzuchaty_lydki_gprzysr_p"  ] = std::make_pair("brzuchaty_lydki_gprzysr_p"  , "brzuchaty_lydki_gprzysr_p"  );
    emgNames["czworoboczny_grzbietu_l"    ] = std::make_pair("czworoboczny_grzbietu_l"    , "czworoboczny_grzbietu_l"    );
    emgNames["czworoboczny_grzbietu_p"    ] = std::make_pair("czworoboczny_grzbietu_p"    , "czworoboczny_grzbietu_p"    );
    emgNames["dwuglowy_uda_l"             ] = std::make_pair("dwuglowy_uda_l"             , "dwuglowy_uda_l"             );
    emgNames["dwuglowy_uda_p"             ] = std::make_pair("dwuglowy_uda_p"             , "dwuglowy_uda_p"             );
    emgNames["polsciegnisty_l"            ] = std::make_pair("polsciegnisty_l"            , "polsciegnisty_l"            );
    emgNames["polsciegnisty_p"            ] = std::make_pair("polsciegnisty_p"            , "polsciegnisty_p"            );
    emgNames["posladkowy_wielki_l"        ] = std::make_pair("posladkowy_wielki_l"        , "posladkowy_wielki_l"        );
    emgNames["posladkowy_wielki_p"        ] = std::make_pair("posladkowy_wielki_p"        , "posladkowy_wielki_p"        );
    emgNames["prostownik_palcow_l"        ] = std::make_pair("prostownik_palcow_l"        , "prostownik_palcow_l"        );
    emgNames["prostownik_palcow_p"        ] = std::make_pair("prostownik_palcow_p"        , "prostownik_palcow_p"        );
    emgNames["ramie-tyl2"                 ] = std::make_pair("ramie-tyl2"                 , "ramie-tyl2"                 );
    emgNames["ramie-tyl2P"                ] = std::make_pair("ramie-tyl2P"                , "ramie-tyl2P"                );
    emgNames["ramie-tyl3"                 ] = std::make_pair("ramie-tyl3"                 , "ramie-tyl3"                 );
    emgNames["ramie-tyl3P"                ] = std::make_pair("ramie-tyl3P"                , "ramie-tyl3P"                );
    emgNames["most_obojcz_sutkowy_p"      ] = std::make_pair("rmost_obojcz_sutkowy_p"     , "most_obojcz_sutkowy_p"      );
    emgNames["most_obojcz_sutkowy_l"      ] = std::make_pair("rmost_obojcz_sutkowy_l"     , "most_obojcz_sutkowy_l"      );
    //emgNames["ramie-tyl6"                 ] = std::make_pair("ramie-tyl6"                 , "ramie-tyl6"                 );
    //emgNames["ramie-tyl6P"                ] = std::make_pair("ramie-tyl6P"                , "ramie-tyl6P"                );
    emgNames["trojglowy_ramienia_gdluga_l"] = std::make_pair("trojglowy_ramienia_gdluga_l", "trojglowy_ramienia_gdluga_l");
    emgNames["trojglowy_ramienia_gdluga_p"] = std::make_pair("trojglowy_ramienia_gdluga_p", "trojglowy_ramienia_gdluga_p");

    IFilterCommandPtr emgCommand(new EMGCommand(TreeBuilder::createEMGBranch,
        emgNames, emgFront, emgBack, TreeBuilder::getRootEMGIcon(), TreeBuilder::getEMGIcon()));
    filter1->addFilter(tr("EMG"), emgCommand, &iconEmgSmall);

    SubjectHierarchyFilterPtr typeFilter3(new SubjectHierarchyTypeFilter(typeid(ForceCollection)));
    SubjectHierarchyFilterPtr typeFilter4(new SubjectHierarchyTypeFilter(typeid(MomentCollection)));
    SubjectHierarchyFilterPtr typeFilter5(new SubjectHierarchyTypeFilter(typeid(PowerCollection)));

    typedef BuilderConfiguredFilterCommand<MomentCollection> MomentsCommand;
    typedef BuilderConfiguredFilterCommand<ForceCollection> ForcesCommand;
    typedef BuilderConfiguredFilterCommand<PowerCollection> PowerCommand;
    typedef BuilderConfiguredFilterCommand<MarkerCollection> MarkersCommand;

    QString pathFront = QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_front.xml").string());
    QString pathBack = QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_back.xml").string());

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
    IFilterCommandPtr forceFilter(new ForcesCommand(TreeBuilder::createForcesBranch,
        forcesNames, pathFront, pathBack, TreeBuilder::getRootForcesIcon(), TreeBuilder::getForcesIcon()));

    filter2->addFilter(tr("FORCES"), forceFilter, &iconForceSmall);
    filter2->addFilter(tr("MOMENTS"), momentFilter, &iconMomentSmall);
    filter2->addFilter(tr("POWERS"), powerFilter, &iconPowerSmall);

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
    QString markersFront = QString::fromStdString(plugin::getResourcePath("images/skeleton_front/skeleton_markers.xml").string());
    QString markersBack = QString::fromStdString(plugin::getResourcePath("images/skeleton_back/skeleton_markers.xml").string());

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

    for (auto it = dataFilterWidgets.begin(); it != dataFilterWidgets.end(); ++it) {
        for (int i = 0; i < (*it)->getNumEntries(); ++i) {
            connect((*it)->getEntry(i), SIGNAL(onFilterClicked(FilterEntryWidget*)), this, SLOT(filterClicked(FilterEntryWidget*)));
        }
    }
}

void HmmMainWindow::createFilterTab2()
{
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

    SubjectHierarchyFilterPtr typeFilter1(new SubjectHierarchyTypeFilter(typeid(GRFChannel)));
    SubjectHierarchyFilterPtr typeFilter2(new SubjectHierarchyTypeFilter(typeid(EMGChannel)));

    IFilterCommandPtr multi1(new MultiChartCommand<ForceCollection>());
    IFilterCommandPtr multi2(new MultiChartCommand<MomentCollection>());
    IFilterCommandPtr multi3(new MultiChartCommand<PowerCollection>());

    filter4->addFilter("Force", multi1);
    filter4->addFilter("Moment", multi2);
    filter4->addFilter("Power", multi3);

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

const core::ConstObjectsList& HmmMainWindow::getCurrentSessions()
{
	currentSessions.clear();
	plugin::getDataManagerReader()->getObjects(currentSessions, typeid(PluginSubject::ISession), false);
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

    //zawsze którys button z podstawowych musi być kliknięty/checked
    if(currentButton == button){
        if(checked == false){
            button->blockSignals(true);
            button->setChecked(true);
            button->blockSignals(false);
        }
    }else{

        //zapamiętać aktualny kontekst
        contextStates[currentButton] = ContextState(getCurrentContext(), getCurrentContextWidget());

        currentButton->blockSignals(true);
        currentButton->setChecked(false);
        currentButton->blockSignals(false);
        button2TabWindow[currentButton]->hide();

        button2TabWindow[button]->show();

        //przywracamy poprzedni kontekst
        auto it = contextStates.find(button);
        if(it != contextStates.end()){
            if(it->second.first == nullptr){
                setCurrentContext((QWidget*)nullptr);
            }else{
                try{
                    setCurrentContext(it->second.first, it->second.second);
                }catch(...){
                    setCurrentContext(it->second.first, nullptr);
                    it->second.second = nullptr;
                }
            }
        }else{
            button2TabWindow[button]->setFocus(Qt::OtherFocusReason);
        }

        currentButton = button;
    }

    mainArea->layout()->update();
}


void HmmMainWindow::visualizerDestroyed(QObject * visualizer)
{
    auto w = qobject_cast<QWidget*>(visualizer);
    visualizerUsageContext->setCurrentContextWidgetDestroyed(true);
    removeContext(w);
    visualizerUsageContext->setCurrentContextWidgetDestroyed(false);
    contextEventFilter->unregisterClosableContextWidget(w);
    auto it = derrivedContextWidgets.right.find(w);
	while( it != derrivedContextWidgets.right.end() && it->first == w){
		contextEventFilter->unregisterClosableContextWidget(it->second);
		++it;
	}
    derrivedContextWidgets.right.erase(w);
    plainContextWidgets.erase(w);
}

QDockWidget* HmmMainWindow::createDockVisualizer(const core::VisualizerPtr & visualizer)
{
    // todo : zastanowic się nad bezpieczenstwem tej operacji
    connect(visualizer.get(), SIGNAL(screenshotTaken(const QPixmap&)), this, SLOT(addToRaports(const QPixmap&)));

	auto visWidget = new coreUI::CoreVisualizerWidget(visualizer);

	auto dockVisWidget = embeddWidget(visWidget, QString::fromStdString(visualizer->getName()),
		Qt::AllDockWidgetAreas,
		false);

    registerVisualizerContext(qobject_cast<coreUI::CoreTitleBar*>(dockVisWidget->titleBarWidget()), qobject_cast<coreUI::CoreVisualizerWidget*>(dockVisWidget->widget()), visualizer);
    dockVisWidget->setMinimumSize((std::max)(50, dockVisWidget->minimumWidth()), (std::max)(50, dockVisWidget->minimumHeight()));
    return dockVisWidget;
}

QDockWidget* HmmMainWindow::createAndAddDockVisualizer( HmmTreeItem* hmmItem, coreUI::CoreDockWidgetSet* dockSet)
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
     TreeItemHelperPtr helper = hmmItem->getHelper();
     auto visualizer = helper->createVisualizer();
     auto visualizerDockWidget = createDockVisualizer(visualizer);

     if (dockSet) {
         dockSet->addDockWidget(visualizerDockWidget);
     } else {
         topMainWindow->autoAddDockWidget( visualizerDockWidget, tr("Group %1").arg(topMainWindow->count()+1) );
     }

     std::vector<core::Visualizer::VisualizerSerie*> series;
     helper->getSeries(visualizer, path, series);
     if (!series.empty()) {
         DataItemDescription desc(qobject_cast<coreUI::CoreVisualizerWidget*>(visualizerDockWidget->widget()), visualizerDockWidget);	 
		 desc.channel = core::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
		 desc.path = path.toStdString();
         items2Descriptions.insert(std::make_pair(helper, desc));

		 auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
		 timeline->addChannel(desc.path, desc.channel);

		 for(auto it = series.begin(); it != series.end(); ++it){
			 seriesToChannels[*it] = desc.path;
		 }
     } else {
         PLUGIN_LOG_WARNING("Problem adding series to visualizer");
     }

	 visualizer->addObserver(this);

     return visualizerDockWidget;
 }

 void HmmMainWindow::refreshTree()
 {
     core::ConstObjectsList sessions;
	 plugin::getDataManagerReader()->getObjects(sessions, typeid(PluginSubject::ISession), false);
     currentSessions = sessions;
     treeRefresher.refresh(analisis->getTreeWidget());
 }

 QMenu* HmmMainWindow::getContextMenu( QWidget* parent, HmmTreeItem* item )
 {
     QMenu * menu = new QMenu(parent);
     QAction * addNew = new ContextAction(item, menu);

     addNew->setText(tr("Create new visualizer"));
     menu->addAction(addNew);
     menu->addSeparator();
     connect(addNew, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));
     connect(addNew, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

     if (core::dynamic_pointer_cast<NewChartItemHelper>(item->getHelper())) {
         QMenu* multiMenu = new QMenu(tr("Multi chart"), menu);
         QAction * multi = new ContextAction(item, menu);
         multi->setText(tr("All from session"));
         multiMenu->addAction(multi);
         connect(multi, SIGNAL(triggered()), this, SLOT(allFromSession()));
         connect(multi, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));
         menu->addMenu(multiMenu);
     }

     if (core::dynamic_pointer_cast<NewVector3ItemHelper>(item->getHelper())) {
         QMenu* multiMenu = new QMenu(tr("Multi chart"), menu);
         QAction * multiX = new ContextAction(item, menu);
         multiX->setText(tr("All X from session"));
         multiMenu->addAction(multiX);
         connect(multiX, SIGNAL(triggered()), this, SLOT(allXFromSession()));
         connect(multiX, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * multiY = new ContextAction(item, menu);
         multiY->setText(tr("All Y from session"));
         multiMenu->addAction(multiY);
         connect(multiY, SIGNAL(triggered()), this, SLOT(allYFromSession()));
         connect(multiY, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * multiZ = new ContextAction(item, menu);
         multiZ->setText(tr("All Z from session"));
         multiMenu->addAction(multiZ);
         connect(multiZ, SIGNAL(triggered()), this, SLOT(allZFromSession()));
         connect(multiZ, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         multiMenu->addSeparator();

         QAction * normL = new ContextAction(item, menu);
         normL->setText(tr("Left Step normalization"));
         multiMenu->addAction(normL);
         connect(normL, SIGNAL(triggered()), this, SLOT(normalizedLeftChart()));
         connect(normL, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normR = new ContextAction(item, menu);
         normR->setText(tr("Right Step normalization"));
         multiMenu->addAction(normR);
         connect(normR, SIGNAL(triggered()), this, SLOT(normalizedRightChart()));
         connect(normR, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normAllL = new ContextAction(item, menu);
         normAllL->setText(tr("Left Step normalization - all from session"));
         multiMenu->addAction(normAllL);
         connect(normAllL, SIGNAL(triggered()), this, SLOT(allLeftNormalized()));
         connect(normAllL, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));

         QAction * normAllR = new ContextAction(item, menu);
         normAllR->setText(tr("Right Step normalization - all from session"));
         multiMenu->addAction(normAllR);
         connect(normAllR, SIGNAL(triggered()), this, SLOT(allRightNormalized()));
         connect(normAllR, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));


         menu->addMenu(multiMenu);
     }

     QMenu* addTo = new QMenu(tr("Add to:"), menu);
	 addTo->setEnabled(false);
     connect(addTo, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
     connect(addTo, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
     BOOST_FOREACH(coreUI::CoreDockWidgetSet* set, topMainWindow->getDockSet()) {
         //QMenu* group = new QMenu(set->windowTitle(), menu);
		 QMenu* group = new QMenu(topMainWindow->setText(topMainWindow->indexOf(set)), menu);

         BOOST_FOREACH(QDockWidget* dock, set->getDockWidgets()) {
             coreUI::CoreVisualizerWidget* vw = dynamic_cast<coreUI::CoreVisualizerWidget*>(dock->widget());
             if (vw ) {
                 core::VisualizerPtr visualizer = vw->getVisualizer();
                 auto hierarchyManager = plugin::getDataHierachyManagerReader();
                 bool compatibile = false;
				 core::TypeInfoSet supportedTypes;
				 visualizer->getSupportedTypes(supportedTypes);

                 std::vector<TypeInfo> types = item->getHelper()->getTypeInfos();
					for (unsigned int h = 0; h < types.size(); ++h) {
						if(supportedTypes.find(types[h]) != supportedTypes.end()){
							compatibile = true;
						}else {
							for(auto it = supportedTypes.begin(); it != supportedTypes.end(); ++it){
								if(hierarchyManager->isTypeCompatible(*it, types[h]) == true){
									compatibile = true;
									break;
								}
							}
						}

						if(compatibile == true){
							break;
						}
					}

                 if (compatibile) {
                     int maxSeries = visualizer->getMaxSeries();
                     if (maxSeries == -1 || maxSeries > static_cast<int>(visualizer->getNumSeries())) {
                         QAction* addAction = new ContextAction(item, group, visualizer);
                         addAction->setText(QString::fromStdString(visualizer->getName()));
                         connect(addAction, SIGNAL(triggered()), this, SLOT(addToVisualizer()));
                         connect(addAction, SIGNAL(triggered()), this->treeUsageContext.get(), SLOT(refresh()));
                         group->addAction(addAction);
                     }
                 }
             }
         }

         if (group->actions().size()) {
             addTo->addMenu(group);
         } else  {
             delete group;
         }
     }

	 menu->addMenu(addTo);

     if (addTo->actions().size()) {
         addTo->setEnabled(true);
     }

	 QMenu* removeFrom = new QMenu(tr("Remove from:"), menu);
	 removeFrom->setEnabled(false);
	 menu->addMenu(removeFrom);

	 QMenu* createIn = new QMenu(tr("Create in:"), menu);
	 createIn->setEnabled(false);
	 menu->addMenu(createIn);

     TreeItemHelperPtr helper = item->getHelper();
     if(items2Descriptions.find(helper) != items2Descriptions.end()) {
         removeFrom->setEnabled(true);
         connect(removeFrom, SIGNAL(aboutToHide()), this, SLOT(menuHighlightVisualizer()));
         connect(removeFrom, SIGNAL(hovered(QAction*)), this, SLOT(menuHighlightVisualizer(QAction*)));
         auto range = items2Descriptions.equal_range(helper);
         for (auto it = range.first; it != range.second; it++) {
             DataItemDescription desc = it->second;
             QAction * action = new ContextAction(item, menu, desc.visualizerWidget->getVisualizer());
             action->setText(QString::fromStdString(desc.visualizerWidget->getVisualizer()->getName()));
             connect(action, SIGNAL(triggered()), this, SLOT(removeFromVisualizer()));
             connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
             removeFrom->addAction(action);
         }

         QAction* all = new ContextAction(item, menu);
         all->setText("All visualizers");
         connect(all, SIGNAL(triggered()), this, SLOT(removeFromAll()));
         connect(all, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
         removeFrom->addSeparator();
         removeFrom->addAction(all);
     }

     
     BOOST_FOREACH(coreUI::CoreDockWidgetSet* set, topMainWindow->getDockSet()) {
         if (set->isAdditionPossible()) {
             QAction* action = new ContextAction(item, menu, VisualizerPtr(), set);
             action->setText(topMainWindow->setText(topMainWindow->indexOf(set)));
             createIn->addAction(action);
             connect(action, SIGNAL(triggered()), this, SLOT(createNewVisualizer()));
             connect(action, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
			 //TODO
			 //connect(action, SIGNAL(hovered(QAction*)), this, SLOT(menuShowGroup(QAction*)));
         }
     }

     QAction* newGroup = new ContextAction(item, menu);
     newGroup->setText(tr("New group"));
     createIn->addAction(newGroup);
     connect(newGroup, SIGNAL(triggered()), this, SLOT(createVisualizerInNewSet()));
     connect(newGroup, SIGNAL(triggered()), treeUsageContext.get(), SLOT(refresh()));
     if (createIn->actions().size()) {
         createIn->setEnabled(true);
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
         typedef std::multimap<TreeItemHelperPtr, DataItemDescription> mmap;
         std::list<mmap::iterator> toErase;
         auto it = items2Descriptions.find(action->getTreeItem()->getHelper());
         while(it != items2Descriptions.end() ) {
             // na razie kopia, w przeciwnym razie jest problem z usuwaniem.
             DataItemDescription desc = it->second;
             // jeśli w akcji nie przechowujemy informacji o konkretnym wizualizatorze
             // to znaczy, ze chcemy usunąć dane z wszystkich wizualizatorw
             if (action->getVisualizer() == nullptr || desc.visualizerWidget->getVisualizer() == action->getVisualizer()) {

				 //teraz usuwamy serie
				 auto series = desc.channel->getVisualizersSeries();

				 for(auto it = series.begin(); it != series.end(); ++it){
					 auto vis = (*it)->visualizer();
					 vis->destroySerie((*it));					 
				 }

                 if (desc.visualizerWidget->getVisualizer()->getNumSeries() == 0) {
                     desc.visualizerDockWidget->close();
                 }

                 if (once) {
                    break;
                 }

				 it = items2Descriptions.find(action->getTreeItem()->getHelper());
             }else{
				 ++it;
			 }
         }
     }
 }

 void HmmMainWindow::filterClicked( FilterEntryWidget* entry )
 {
     for (auto it = dataFilterWidgets.begin(); it != dataFilterWidgets.end(); ++it) {
         (*it)->uncheckEntries(entry);
     }
 }

 void HmmMainWindow::allFromSession()
 {
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewChartItemHelperPtr helper = core::dynamic_pointer_cast<NewChartItemHelper>(a->getTreeItem()->getHelper());
     UTILS_ASSERT(helper);

     if (helper && helper->getMotion()) {
         NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
         SessionConstPtr s = helper->getMotion()->getUnpackedSession();
         core::ConstObjectsList motions;
         s->getMotions(motions);

         for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
			 PluginSubject::MotionConstPtr m = (*itMotion)->get();
             core::ConstObjectsList wrappers;
             //m->getObjects(wrappers, typeid(ScalarChannel), false);
			 m->getObjects(wrappers, typeid(ScalarChannelReaderInterface), false);
             EventsCollectionConstPtr events;
             if (m->hasObject(typeid(C3DEventsCollection), false)) {
				 core::ConstObjectsList e;
                 m->getObjects(e, typeid(C3DEventsCollection), false);
                 events = e.front()->get();
             }
			 std::string name;

			 if(helper->getWrapper()->tryGetMeta("core/name", name) == true){

				 for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
					std::string localName;
					if ((*it)->tryGetMeta("core/name", localName) && localName == name) {
						 toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(*it, events, m));
					 }
				 }
			 }
         }
         NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
         HmmTreeItem item(multi);
         createNewVisualizer(&item);         
     }
 }


 void HmmMainWindow::allLeftNormalized()
 {
     auto context = c3dlib::C3DParser::IEvent::Left;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     if (helper) {
         createNormalizedFromAll(helper, context);
     }
 }

 void HmmMainWindow::allRightNormalized()
 {
     auto context = c3dlib::C3DParser::IEvent::Right;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     if (helper) {
         createNormalizedFromAll(helper, context);

     }
 }

 void HmmMainWindow::normalizedLeftChart()
 {
     auto context = c3dlib::C3DParser::IEvent::Left;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     if (helper) {
         createNormalized(helper, context);

     }
 }

 void HmmMainWindow::normalizedRightChart()
 {
     auto context = c3dlib::C3DParser::IEvent::Right;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     if (helper) {
         createNormalized(helper, context);
     }
 }

 void HmmMainWindow::allXFromSession()
 {
     int channelNo = 0;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allYFromSession()
 {
     int channelNo = 1;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allZFromSession()
 {
     int channelNo = 2;
     ContextAction* a = qobject_cast<ContextAction*>(sender());
     NewVector3ItemHelperPtr helper = core::dynamic_pointer_cast<NewVector3ItemHelper>(a->getTreeItem()->getHelper());
     allTFromSession(helper, channelNo);
 }

 void HmmMainWindow::allTFromSession( NewVector3ItemHelperPtr helper, int channelNo )
 {
     UTILS_ASSERT(helper);

     if (helper) {
         NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
         SessionConstPtr s = helper->getMotion()->getUnpackedSession();
         core::ConstObjectsList motions;
         s->getMotions(motions);

         for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
			 PluginSubject::MotionConstPtr m = (*itMotion)->get();
             core::ConstObjectsList wrappers;
             m->getObjects(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

             EventsCollectionConstPtr events;
             if (m->hasObject(typeid(C3DEventsCollection), false)) {
				 core::ConstObjectsList e;
                 m->getObjects(e, typeid(C3DEventsCollection), false);
                 events = e.front()->get();
             }

             for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
                 VectorChannelCollectionConstPtr collection = (*it)->get();
                 int count = collection->getNumChannels();
                 for (int i = 0; i < count; ++i) {
                     VectorChannelConstPtr channel = collection->getChannel(i);
                     VectorChannelConstPtr helperChannel = helper->getWrapper()->get();
                     if (channel->getName() == helperChannel->getName()) {
                        ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                        core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                        wrapper->set(reader);
                        int no = toVisualize.size();
                        std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
						(*wrapper)["core/name"] = prefix + boost::lexical_cast<std::string>(no);
						std::string src;
						(*it)->tryGetMeta("core/source", src);
                        (*wrapper)["core/source"] = src + boost::lexical_cast<std::string>(no);
                        toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
                     }

                 }

             }
         }
         NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
         HmmTreeItem item(multi);
         createNewVisualizer(&item);
     }
 }

 void HmmMainWindow::createNormalized( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context )
 {
     NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
     MotionConstPtr motion = helper->getMotion();
     EventsCollectionConstPtr events;
     std::vector<FloatPairPtr> segments;
     if (motion->hasObject(typeid(C3DEventsCollection), false)) {
		 core::ConstObjectsList wrappers;
         motion->getObjects(wrappers, typeid(C3DEventsCollection), false);
         events = wrappers.front()->get();
         segments = getTimeSegments(events, context);
     }
     std::map<ObjectWrapperConstPtr, QColor> colorMap;
     VectorChannelConstPtr channel = helper->getWrapper()->get();
     for (int j = 0; j != segments.size(); ++j) {
         FloatPairPtr segment = segments[j];
         for (int channelNo = 0; channelNo <= 2; ++channelNo) {
             ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
             ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
             core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
             wrapper->set(normalized);
             int no = toVisualize.size();
             std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
             colorMap[wrapper] = channelNo == 0 ? QColor(255, 0, 0) : (channelNo == 1 ? QColor(0, 255, 0) : QColor(0, 0, 255));
			 (*wrapper)["core/name"] = prefix + ":" + boost::lexical_cast<std::string>(j);
			 std::string src;
			 helper->getWrapper()->tryGetMeta("core/sources", src);
             (*wrapper)["core/sources"] = src + boost::lexical_cast<std::string>(no);
             toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, motion));
         }
     }
     NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
     multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));
     HmmTreeItem item(multi);
     createNewVisualizer(&item);
     //delete multi;
 }

 void HmmMainWindow::createNormalizedFromAll( NewVector3ItemHelperPtr helper, c3dlib::C3DParser::IEvent::Context context )
 {
     NewMultiserieHelper::ChartWithDescriptionCollection toVisualize;
     SessionConstPtr s = helper->getMotion()->getUnpackedSession();
     core::ConstObjectsList motions;
     s->getMotions(motions);

     std::map<ObjectWrapperConstPtr, QColor> colorMap;
     for (auto itMotion = motions.begin(); itMotion != motions.end(); ++itMotion) {
		 PluginSubject::MotionConstPtr m = (*itMotion)->get();
         core::ConstObjectsList wrappers;
         m->getObjects(wrappers, typeid(utils::DataChannelCollection<VectorChannel>), false);

         EventsCollectionConstPtr events;
         std::vector<FloatPairPtr> segments;
         if (m->hasObject(typeid(C3DEventsCollection), false)) {
			 core::ConstObjectsList e;
             m->getObjects(e, typeid(C3DEventsCollection), false);
             events = e.front()->get();
             segments = getTimeSegments(events, context);
         }

         for (auto it = wrappers.begin(); it != wrappers.end(); ++it) {
             VectorChannelCollectionConstPtr collection = (*it)->get();
             int count = collection->getNumChannels();
             for (int i = 0; i < count; ++i) {
                 VectorChannelConstPtr channel = collection->getChannel(i);
                 VectorChannelConstPtr helperChannel = helper->getWrapper()->get();
                 if (channel->getName() == helperChannel->getName()) {

                     int r = rand() % 200;
                     int g = rand() % 200;
                     int b = rand() % 200;
                     QColor colorX(r + 55,g , b);
                     QColor colorY(r, g + 55, b);
                     QColor colorZ(r, g, b + 55);
                     for (int j = 0; j != segments.size(); ++j) {
                         FloatPairPtr segment = segments[j];

                         for (int channelNo = 0; channelNo <= 2; ++channelNo) {
                             ScalarChannelReaderInterfacePtr reader(new VectorToScalarAdaptor(channel, channelNo));
                             ScalarChannelReaderInterfacePtr normalized(new ScalarWithTimeSegment(reader, segment->first, segment->second));
                             core::ObjectWrapperPtr wrapper = core::ObjectWrapper::create<ScalarChannelReaderInterface>();
                             wrapper->set(normalized);
                             colorMap[wrapper] = channelNo == 0 ? colorX : (channelNo == 1 ? colorY : colorZ);
                             int no = toVisualize.size();
                             std::string prefix = channelNo == 0 ? "X_" : (channelNo == 1 ? "Y_" : "Z_");
							 (*wrapper)["core/name"] = prefix + boost::lexical_cast<std::string>(i) + ":" + boost::lexical_cast<std::string>(j);
							 std::string src;
							 (*it)->tryGetMeta("core/source", src);
                             (*wrapper)["core/source"] = src + boost::lexical_cast<std::string>(no);
                             toVisualize.push_back(NewMultiserieHelper::ChartWithDescription(wrapper, events, m));
                         }
                     }
                 }

             }

         }
     }
     if (toVisualize.empty() == false) {
         NewMultiserieHelperPtr multi(new NewMultiserieHelper(toVisualize));
         multi->setColorStrategy(IMultiserieColorStrategyPtr(new ColorMapMultiserieStrategy(colorMap)));
         HmmTreeItem item(multi);
         createNewVisualizer(&item);
     } else {
         // ?
     }

 }

 void HmmMainWindow::registerVisualizerContext( coreUI::CoreTitleBar * titleBar, coreUI::CoreVisualizerWidget* visualizerDockWidget, const core::VisualizerPtr & visualizer )
 {
     contextEventFilter->registerClosableContextWidget(titleBar);
     titleBar->installEventFilter(contextEventFilter);
     contextEventFilter->registerClosableContextWidget(visualizerDockWidget);
     visualizerDockWidget->installEventFilter(contextEventFilter);

     auto visWidget = visualizer->getWidget();

     contextEventFilter->registerClosableContextWidget(visWidget);
     visWidget->installEventFilter(contextEventFilter);

     plainContextWidgets.insert(visualizerDockWidget);
     derrivedContextWidgets.insert(DerrivedContextWidgets::value_type(titleBar,visualizerDockWidget));
     derrivedContextWidgets.insert(DerrivedContextWidgets::value_type(visWidget,visualizerDockWidget));

     visWidget->setFocusPolicy(Qt::ClickFocus);
     visualizerDockWidget->setFocusProxy(visWidget);

     connect(visualizerDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(visualizerDestroyed(QObject *)));

     //kontekst wizualizatora!!
     addWidgetToContext(visualizerUsageContext, visualizerDockWidget);
 }



 void HmmMainWindow::DataObserver::observe(const core::IDataManagerReader::ChangeList & changes)
 {
	 auto dhm = plugin::getDataHierachyManagerReader();
	 for(auto it = changes.begin(); it != changes.end(); ++it){
		 if((*it).type == typeid(PluginSubject::ISession) || dhm->isTypeCompatible(typeid(PluginSubject::ISession), (*it).type)){
			QMetaObject::invokeMethod(hmm, "onRefreshFiltersTree");
			break;
		 }
	 }
 }

 void HmmMainWindow::onRefreshFiltersTree()
 {
	 if(plugin::getDataManagerReader()->hasObject(typeid(PluginSubject::ISession), false)){
		 analisisButton->setEnabled(true);
	 }else{
		 analisisButton->setEnabled(false);
	 }

	 refreshTree();
 }

 void HmmMainWindow::onAbout()
 {
     AboutDialog about;
     about.exec();

 }

 HmmMainWindow::DataItemDescription::DataItemDescription
     ( coreUI::CoreVisualizerWidget* widget, QDockWidget * dockWidget) :
        visualizerWidget(widget),
        visualizerDockWidget(dockWidget)
 {
     //// konwersja na weak ptr.
     //for (auto it = series.begin(); it != series.end(); ++it) {
     //    this->series.push_back(*it);
     //}
 }

coreUI::CoreDockWidget * HmmMainWindow::embeddWidget(QWidget * widget, const QString & windowTitle, Qt::DockWidgetArea allowedAreas, bool permanent)
{
	coreUI::CoreDockWidget * embeddedDockWidget = new coreUI::CoreDockWidget(windowTitle);
	embeddedDockWidget->setWidget(widget);
	embeddedDockWidget->setAllowedAreas(allowedAreas);
	embeddedDockWidget->setPermanent(permanent);

	auto consoleTitleBar = coreUI::CoreTitleBar::supplyWithCoreTitleBar(embeddedDockWidget);
	//coreUI::CoreTitleBar::supplyCoreTitleBarWithActions(consoleTitleBar, widget);

	return embeddedDockWidget;
}

void HmmMainWindow::switchToAnalysis()
{
    emit onSwitchToAnalysis();
}

void HmmMainWindow::safeSwitchToAnalysis()
{
    analisisButton->click();
    analisis->getTreeWidget()->setFocus();
    refreshTree();
}

//
//void HmmMainWindow::processedBranchIncreaseWhenEqual( int sinkRunNo )
//{
//    ScopedLock lock(processedMutex);
//    static int count = 0;
//    if (sinkRunNo == count) {
//        QString s;
//        s.sprintf("Run %04d", ++count);
//        QTreeWidgetItem* itm = new QTreeWidgetItem();
//        itm->setText(0, s);
//        processedBranch->addChild(itm);
//    }
//}

int HmmMainWindow::getProcessedBranchCount()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(processedMutex);
    return processedBranch->childCount();
}

void HmmMainWindow::addItemToProcessedBranch(QTreeWidgetItem* item, int sinkRunNo)
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(processedMutex);
    static int count = 0;
    if (sinkRunNo == processedBranch->childCount()) {
        QString s;
        s.sprintf("Run %04d", ++count);
        QTreeWidgetItem* itm = new QTreeWidgetItem();
        itm->setText(0, s);
        processedBranch->addChild(itm);
    }
    
    int childCount = processedBranch->childCount();
    UTILS_ASSERT(childCount);
    auto child = processedBranch->child(childCount - 1);
    child->addChild(item);
    refreshTree();
}

