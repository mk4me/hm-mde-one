#include "MdePCH.h"
#include "AnalisisWidget.h"
#include <corelib/IServiceManager.h>
#include <corelib/IIdentifiable.h>
#include <corelib/PluginCommon.h>
#include <corelib/BaseDataTypes.h>
#include <coreui/CoreVisualizerWidget.h>
#include <coreui/CoreDockWidgetSet.h>
#include <corelib/HierarchyHelper.h>
#include <coreui/HierarchyTreeModel.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>
#include <plugins/newVdf/NewVdfService.h>
#include <coreui/CoreTitleBar.h>
#include <QtGui/QMenu>
#include "ContextAutoPlacer.h"
#include <coreui/DataFilterWidget.h>
#include "ReportsThumbnailContext.h"
//#include "SummaryWindow.h"
//#include "AnalisisTreeWidget.h"

AnalisisWidget::AnalisisWidget( AnalisisModelPtr model, QWidget* parent, int margin /*= 2*/, Qt::WindowFlags flags /*= 0*/ ) : 
    QWidget(parent, flags),
    model(model),
    manager(nullptr),
    flexiTabWidget(nullptr),
    margin(margin),
    filterWidth(-1), 
    filterHeight(-1)
{
    setupUi(this);
    devideArea();
    showTimeline();
    treeView->setModel(model->getTreeModel());
    treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    contextMenu = new AnalysisTreeContextMenu(model, this);
    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)), contextMenu ,SLOT(contextualMenu(const QPoint &)));
    connect(contextMenu, SIGNAL(createVisualizer(core::IHierarchyDataItemConstPtr, core::HierarchyHelperPtr)), this, SLOT(createVisualizer(core::IHierarchyDataItemConstPtr, core::HierarchyHelperPtr)));

    
    auto vdfService = core::queryServices<vdf::NewVdfService>(plugin::getServiceManager());
    connect(vdfService.get(), SIGNAL(transferResults(core::IHierarchyItemPtr )), this, SLOT(addRoot(core::IHierarchyItemPtr)));
    connect(model.get(), SIGNAL(filterBundleAdded(core::IFilterBundlePtr)), this, SLOT(onFilterBundleAdded(core::IFilterBundlePtr)));
}

void AnalisisWidget::showTimeline()
{
    auto timeline = plugin::getServiceManager()->getService(core::UID::GenerateUniqueID("{0157346E-D1F3-4A4F-854F-37C87FA3E5F9}"));
    if (timeline) {
        QWidget* controlWidget = timeline->getWidget();
        dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
        bottomMainWindow->layout()->addWidget(controlWidget);
        dynamic_cast<QVBoxLayout*>(bottomMainWindow->layout())->addStretch();
    }
}

void AnalisisWidget::devideArea()
{
    topMainWindow = new coreUI::CoreDockWidgetManager();
    topMainWindow->setSetsClosable(true);
    topMainWindow->setTabPosition(QTabWidget::South);
    topMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bottomMainWindow = new QFrame();
    bottomMainWindow->setObjectName(QString("bottomMainWindow"));
    bottomMainWindow->setLayout(new QVBoxLayout);
    bottomMainWindow->layout()->setContentsMargins(0,0,0,0);
    bottomMainWindow->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QWidget* analisisArea = getArea();
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
    analisisArea->setLayout(v);
}

void AnalisisWidget::createVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper )
{
    createAndAddDockVisualizer(treeItem, helper, nullptr);
}

QDockWidget* AnalisisWidget::createDockVisualizer(const core::VisualizerPtr & visualizer)
{
    connect(visualizer.get(), SIGNAL(screenshotTaken(const QPixmap&)), this, SLOT(addToReports(const QPixmap&)));

    auto visWidget = new coreUI::CoreVisualizerWidget(visualizer);

    auto dockVisWidget = embeddWidget(visWidget, QString::fromStdString(visualizer->getName()),
        Qt::AllDockWidgetAreas,
        false);

    // TODO : rejestrowanien
    registerVisualizerContext(model->getContextEventFilter(), qobject_cast<coreUI::CoreTitleBar*>(dockVisWidget->titleBarWidget()), qobject_cast<coreUI::CoreVisualizerWidget*>(dockVisWidget->widget()), visualizer);
    dockVisWidget->setMinimumSize((std::max)(50, dockVisWidget->minimumWidth()), (std::max)(50, dockVisWidget->minimumHeight()));
    return dockVisWidget;
}

coreUI::CoreDockWidget* AnalisisWidget::embeddWidget(QWidget * widget, const QString & windowTitle, Qt::DockWidgetArea allowedAreas, bool permanent)
{
    coreUI::CoreDockWidget * embeddedDockWidget = new coreUI::CoreDockWidget(windowTitle);
    embeddedDockWidget->setWidget(widget);
    embeddedDockWidget->setAllowedAreas(allowedAreas);
    embeddedDockWidget->setPermanent(permanent);

    auto consoleTitleBar = coreUI::CoreTitleBar::supplyWithCoreTitleBar(embeddedDockWidget);
    return embeddedDockWidget;
}

void AnalisisWidget::registerVisualizerContext(ContextEventFilterPtr contextEventFilter, coreUI::CoreTitleBar * titleBar, coreUI::CoreVisualizerWidget* visualizerDockWidget, const core::VisualizerPtr & visualizer )
{
    contextEventFilter->registerClosableContextWidget(titleBar);
    titleBar->installEventFilter(contextEventFilter.get());
    contextEventFilter->registerClosableContextWidget(visualizerDockWidget);
    visualizerDockWidget->installEventFilter(contextEventFilter.get());

    auto visWidget = visualizer->getWidget();

    contextEventFilter->registerClosableContextWidget(visWidget);
    visWidget->installEventFilter(contextEventFilter.get());

    // TODO : zastanowic sie nas sensem tych operacji

    //plainContextWidgets.insert(visualizerDockWidget);
    //derrivedContextWidgets.insert(DerrivedContextWidgets::value_type(titleBar,visualizerDockWidget));
    //derrivedContextWidgets.insert(DerrivedContextWidgets::value_type(visWidget,visualizerDockWidget));
    //
    //visWidget->setFocusPolicy(Qt::ClickFocus);
    //visualizerDockWidget->setFocusProxy(visWidget);
    //
    connect(visualizerDockWidget, SIGNAL(destroyed(QObject *)), this, SLOT(visualizerDestroyed(QObject *)));

    //kontekst wizualizatora!!
    SimpleContextPtr visualizerUsageContext(new SimpleContext(flexiTabWidget, tr("Visualizer")));
    manager->addContext(visualizerUsageContext, parent);
    manager->addWidgetToContext(visualizerUsageContext, visualizerDockWidget);
    manager->addWidgetToContext(visualizerUsageContext, titleBar);
    manager->addWidgetToContext(visualizerUsageContext, visWidget);
}

void AnalisisWidget::visualizerDestroyed(QObject * visualizer)
{
    // TODO
    //auto w = qobject_cast<QWidget*>(visualizer);
    //visualizerUsageContext->setCurrentContextWidgetDestroyed(true);
    //removeContext(w);
    //visualizerUsageContext->setCurrentContextWidgetDestroyed(false);
    //contextEventFilter->unregisterClosableContextWidget(w);
    //auto it = derrivedContextWidgets.right.find(w);
    //while( it != derrivedContextWidgets.right.end() && it->first == w){
    //    contextEventFilter->unregisterClosableContextWidget(it->second);
    //    ++it;
    //}
    //derrivedContextWidgets.right.erase(w);
    //plainContextWidgets.erase(w);
}


QDockWidget* AnalisisWidget::createAndAddDockVisualizer( core::IHierarchyDataItemConstPtr treeItem, core::HierarchyHelperPtr helper, coreUI::CoreDockWidgetSet* dockSet)
{
    std::stack<QString> pathStack;
    core::IHierarchyItemConstPtr pomItem = utils::dynamic_pointer_cast<const core::IHierarchyItem>(treeItem);

    while(pomItem != nullptr){
        pathStack.push(pomItem->getName());
        pomItem = pomItem->getParent();
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
    auto visualizer = helper->createVisualizer(plugin::getVisualizerManager());
    auto visualizerDockWidget = createDockVisualizer(visualizer);

    if (dockSet) {
        dockSet->addDockWidget(visualizerDockWidget);
    } else {
        topMainWindow->autoAddDockWidget( visualizerDockWidget, tr("Group %1").arg(topMainWindow->count()+1) );
    }

    std::vector<core::Visualizer::VisualizerSerie*> series;
    helper->getSeries(visualizer, path, series);
    if (!series.empty()) {
        // TODO : zapis informacji

        //DataItemDescription desc(qobject_cast<coreUI::CoreVisualizerWidget*>(visualizerDockWidget->widget()), visualizerDockWidget);	 
        //desc.channel = utils::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
        //desc.path = path.toStdString();
        //items2Descriptions.insert(std::make_pair(helper, desc));

        auto timeline = core::queryServices<ITimelineService>(plugin::getServiceManager());
        //timeline->addChannel(desc.path, desc.channel);
        auto channels = utils::shared_ptr<VisualizerSerieTimelineMultiChannel>(new VisualizerSerieTimelineMultiChannel(VisualizerSerieTimelineMultiChannel::VisualizersSeries(series.begin(), series.end())));
        timeline->addChannel(path.toStdString(), channels);

        //for(auto it = series.begin(); it != series.end(); ++it){
        //    seriesToChannels[*it] = desc.path;
        //}
    } else {
        PLUGIN_LOG_WARNING("Problem adding series to visualizer");
    }

    // TODO : co robi observer wizualizatora
    //visualizer->addObserver(this);

    return visualizerDockWidget;
}

void AnalisisWidget::setContextItems( IAppUsageContextManager* manager, IAppUsageContextPtr parent, QTabWidget * flexiTabWidget )
{
    this->manager = manager;
    this->parent = parent;
    this->flexiTabWidget = flexiTabWidget;

    ReportsThumbnailContextPtr visualizerUsageContext(new ReportsThumbnailContext(flexiTabWidget, raportsArea));
    manager->addContext(visualizerUsageContext, parent);
    this->raportsArea->addAction(new QAction(tr("Create report"), this));
    model->getContextEventFilter()->registerPermamentContextWidget(this->raportsArea);
    this->raportsArea->installEventFilter(model->getContextEventFilter().get());
    manager->addWidgetToContext(visualizerUsageContext, this->raportsArea);
    connect(visualizerUsageContext.get(), SIGNAL(reportCreated(const QString&)), model.get(), SIGNAL(reportCreated(const QString&)));
}

void AnalisisWidget::addRoot( core::IHierarchyItemPtr root )
{
    model->getTreeModel()->addRootItem(root);
}

void AnalisisWidget::onFilterBundleAdded( core::IFilterBundlePtr bundle )
{
    coreUi::DataFilterWidget* dataWidget = new coreUi::DataFilterWidget("Filter 1", QPixmap());
    int count = bundle->genNumFilters();
    for (int i = 0; i < count; ++i) {
        dataWidget->addFilter(QString("Filter %1").arg(i + 1), bundle->getFilter(i));
    }
    addDataFilterWidget(dataWidget);
    connect(dataWidget, SIGNAL(onFilterClicked(core::IFilterCommandPtr)), this, SLOT(onFilterClicked(core::IFilterCommandPtr)));
}


void AnalisisWidget::addDataFilterWidget( coreUi::DataFilterWidget* filter )
{ 
    if (filterWidth < 0 && filterHeight < 0) {
        filterWidth = filter->width();
        filterHeight = filter->height();
    }

    UTILS_ASSERT(filterHeight == filter->height() && filterWidth == filter->width());


    int count = filterScroll->children().size();
    int x = count % 2;
    int y = count / 2;

    if (x == 0) {
        int w = 3 * margin + filterWidth * 2;
        int h = 2 * margin + (filterHeight + margin) * (y + 1);
        filterScroll->setMinimumSize(w, h);
        containerFrame->setMaximumWidth(w + 22);
        containerFrame->setMinimumWidth(w + 22);
        scrollArea->setMinimumHeight(3 * margin + filterHeight * 2);
    }

    filter->setParent(filterScroll);
    filter->setGeometry(margin + x * (filterWidth + margin),margin +  y * (margin + filterHeight), filterWidth, filterHeight);

    connect(filter, SIGNAL( clicked()), this, SLOT(switchToFirstTab()));
    connect(this->resetButton, SIGNAL(clicked()), filter, SLOT(resetFilters()));
    //connect(this->resetButton, SIGNAL(clicked()), hmm, SLOT(refreshTree()));
    for (int i = 0; i < filter->getNumEntries(); ++i) {
        connect(filter->getEntry(i), SIGNAL( onFilterClicked(core::IFilterCommandPtr)), this, SLOT(onFilterClicked(core::IFilterCommandPtr)));
    }
}

void AnalisisWidget::onFilterClicked( core::IFilterCommandPtr filter )
{
    model->applyFilter(filter);
}

void AnalisisWidget::addToReports( const QPixmap& pixmap )
{
    if (!(pixmap.width() && pixmap.height())) {
        return;
    }

    tabWidget->setCurrentWidget(raportsTab);
    const int maxH = 128;
    const int maxW = 128;
    QWidget* list = raportsArea;
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



AnalysisTreeContextMenu::AnalysisTreeContextMenu( AnalisisModelPtr model, AnalisisWidget* widget ) :
model(model),
    widget(widget)
{

}


void AnalysisTreeContextMenu::contextualMenu( const QPoint &clickedPoint )
{
    QMenu *menu = new QMenu;
    QTreeView* view = widget->getTreeView();
    UTILS_ASSERT(view);

    //QModelIndex index = view->indexAt(clickedPoint);
    QModelIndex index = view->currentIndex();
    core::IHierarchyItemConstPtr item = model->getTreeModel()->internalSmart(index);
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    if (dataItem) {
        auto helpers = dataItem->getHelpers();
        for (auto it = helpers.begin(); it != helpers.end(); ++it) {
            auto helper = *it;
            HelperAction* ha = new HelperAction(helper, QIcon(), helper->getText());
            menu->addAction(ha);
            connect(ha, SIGNAL(triggered()), this, SLOT(onCreateVisualizer()));
        }

    }
    menu->exec(QCursor::pos());
}

void AnalysisTreeContextMenu::onCreateVisualizer()
{
    HelperAction* ha = qobject_cast<HelperAction*>(sender());
    UTILS_ASSERT(ha);
    QModelIndex index = widget->getTreeView()->currentIndex();
    core::IHierarchyItemConstPtr item = model->getTreeModel()->internalSmart(index);
    core::IHierarchyDataItemConstPtr dataItem = utils::dynamic_pointer_cast<const core::IHierarchyDataItem>(item);
    UTILS_ASSERT(dataItem);
    emit createVisualizer(dataItem, ha->getHelper());
}


