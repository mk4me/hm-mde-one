#include "NewChartPCH.h"
#include <limits>
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
#include <QtGui/QDoubleSpinBox>
#include <boost/foreach.hpp>
#include "NewChartVisualizer.h"
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_legend_label.h>
#include <qwt/qwt_legend.h>
#include "StatsTable.h"
#include "NewChartMarker.h"
#include "NewChartValueMarker.h"
#include "NewChartVerticals.h"
#include "NewChartHorizontals.h"
#include "NewChartHelpers.h"
#include "NewChartScaleDrawer.h"
#include "NewChartLegend.h"
#include <limits>
#include <boost/bind.hpp>


NewChartVisualizer::NewChartVisualizer() : 
    showLegend(true), 
    currentSerie(-1),
    plotPanner(nullptr),
    plotMagnifier(nullptr),
    statsTable(nullptr),
    context(C3DEventsCollection::Context::General),
    eventsContextWidget(nullptr),
    eventsMenu(nullptr),
    scaleToActive(false),
    percentDraw(nullptr),
    shiftSpinX(nullptr),
    shiftSpinY(nullptr),
    scaleSpinX(nullptr),
    scaleSpinY(nullptr),
    upperBoundCurve(nullptr),
    lowerBoundCurve(nullptr),
    averageCurve(nullptr),
    boundsAutoRefresh(true),
    movingAverageTimeWindow(0.125),
    pointsPerWindow(25)
{

}


NewChartVisualizer::~NewChartVisualizer()
{
    //TODO
    //przejrzec niszczenie dynamicznie tworzonych zasobów
    if(eventsContextWidget != nullptr){
        delete eventsContextWidget;
    }
	delete percentDraw;
    //delete qwtPlot;
}


QWidget* NewChartVisualizer::createWidget( core::IActionsGroupManager * manager )
{
    QWidget* widget = new QWidget();
    widget->setObjectName(QString::fromUtf8("newChartVisualizerWidget"));
    QwtText txt(getName().c_str());
    qwtPlot = new NCPlot(txt, nullptr);
    qwtPlot->setObjectName(QString::fromUtf8("plot"));
    percentDraw = new PercentScaleDraw(0.0, 5.0);
    qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, percentDraw);
    
    //aby legenda nie by³a usuwana podczas chowania i pokazywania trzeba ustawiæ parenta innego ni¿ QwtPlot
    legend = new NewChartLegend(widget);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
  
    qwtPlot->setAutoReplot(true);

    if (isShowLegend()) {    
        qwtPlot->insertLegend( legend, QwtPlot::BottomLegend );
    }

    qwtPlot->canvas()->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);

    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(tr("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    eventsContextWidget = new QWidget();
    QVBoxLayout* eventsLayout = new QVBoxLayout();
    eventsLayout->setMargin(0);
    eventsLayout->setContentsMargins(0, 0, 0, 0);


    eventsMenu = new QComboBox(eventsContextWidget);
    eventsMenu->setToolTip(tr("Events context"));
    connect(eventsMenu, SIGNAL(activated(int)), this, SLOT(onEventContext(int)));
    eventsLayout->addWidget(eventsMenu);
    eventsContextWidget->setLayout(eventsLayout);

    QIcon eventsIcon(":/resources/icons/normalizacja1.png");
    eventsMenu->addItem(eventsIcon, tr("None events") , QVariant(C3DEventsCollection::Context::General));
    eventsMenu->addItem(eventsIcon, tr("Left events") , QVariant(C3DEventsCollection::Context::Left));
    eventsMenu->addItem(eventsIcon, tr("Right events"), QVariant(C3DEventsCollection::Context::Right));

    picker.reset(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/picker.png"), QSize(), QIcon::Normal, QIcon::Off);
    pickerAction = new QAction(tr("Picker"), this);
    pickerAction->setIcon(icon1);
    statesMap[pickerAction] = picker;
    connect(pickerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
    
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/value_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    valueMarkerAction = new QAction(tr("Value Marker"), this);
    valueMarkerAction->setIcon(icon2);
    statesMap[valueMarkerAction] =  NewChartStatePtr(new NewChartValueMarker(this));
    connect(valueMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/vertical_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    hMarkerAction = new QAction(tr("Horizontal Marker"), this);
    hMarkerAction->setIcon(icon3);
    statesMap[hMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Horizontal));
    connect(hMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/resources/icons/horizontal_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    vMarkerAction = new QAction(tr("Vertical Marker"), this);
    vMarkerAction->setIcon(icon4);
    statesMap[vMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Vertical));
    connect(vMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    scaleAction = new QAction(tr("Scale to active"), this);
    scaleAction->setCheckable(true);
    scaleAction->setChecked(scaleToActive);
    scaleAction->setIcon(QIcon(QString::fromUtf8(":/resources/icons/pomiary.png")));
    connect(scaleAction, SIGNAL(triggered(bool)), this, SLOT(scaleToActiveSerie(bool)));


    QAction* showStats = new QAction(tr("Statistics"), widget);
    showStats->setCheckable(true);
    showStats->setChecked(false);
    QIcon iconStats;
    iconStats.addFile(QString::fromUtf8(":/resources/icons/stat-a.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    iconStats.addFile(QString::fromUtf8(":/resources/icons/stat-b.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    showStats->setIcon(iconStats);
    connect(showStats, SIGNAL(triggered(bool)), this, SLOT(showStatistics(bool)));

    for ( int i = 0; i < QwtPlot::axisCnt; i++ )    {
        QwtScaleWidget *scaleWidget = qwtPlot->axisWidget( i );
        if ( scaleWidget ) {
            scaleWidget->setMargin( 0 );
        }
    }
    qwtPlot->plotLayout()->setAlignCanvasToScales( true );

    plotPanner =  new QwtPlotPanner( qwtPlot->canvas() );
    plotMagnifier =  new QwtPlotMagnifier( qwtPlot->canvas() );
    

    bool c = connect(legend, SIGNAL(checked( QwtPlotItem *, bool, int)), this, SLOT(onSerieSelected(QwtPlotItem*, bool, int)));
    c = connect(legend, SIGNAL(checkboxChanged(const QwtPlotItem*, bool)), this, SLOT(onSerieVisible(const QwtPlotItem*, bool)));
    qwtMarker = new NewChartMarker();
    qwtMarker->setXValue(0);
    qwtMarker->setYValue(0);
    qwtMarker->attach(qwtPlot);

    grid.reset(new QwtPlotGrid);
    grid->enableXMin(false);
    grid->enableYMin(false);

    grid->setMajPen(QPen(Qt::gray, 0, Qt::DashLine));
    //grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(qwtPlot);

    this->currentState = picker;
    this->currentState->stateBegin();

    qwtPlot->replot();

    statsTable = new StatsTable();
    statsTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    qwtPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(qwtPlot);
    layout->setMargin(0);
    layout->addWidget(statsTable);
    statsTable->setVisible(false);
    widget->setLayout(layout);

    auto shiftX = LabeledSpinbox::create(widget, tr("X:"), 0.03, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    auto shiftY = LabeledSpinbox::create(widget, tr("Y:"), 0.03, -std::numeric_limits<double>::max(), std::numeric_limits<double>::max());
    connect(shiftX.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onShiftX(double)));
    connect(shiftY.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onShiftY(double)));
    shiftSpinX = shiftX.get<2>();
    shiftSpinY = shiftY.get<2>();

    auto scaleX = LabeledSpinbox::create(widget, tr("SX:"), 0.01, -5.0, 5.0);
    auto scaleY = LabeledSpinbox::create(widget, tr("SY:"), 0.01, -5.0, 5.0);
    connect(scaleX.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onScaleX(double)));
    connect(scaleY.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onScaleY(double)));
    scaleSpinX = scaleX.get<2>();
    scaleSpinY = scaleY.get<2>();
    
    bandsAction = new QAction("Bands", this);
    QIcon iconBands;
    iconBands.addFile(QString::fromUtf8(":/resources/icons/charts.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    bandsAction->setIcon(iconBands);
    bandsAction->setCheckable(true);
    bandsAction->setChecked(false);
    connect(bandsAction, SIGNAL(triggered(bool)), this, SLOT(showBands(bool)));

    upperBoundCurve = new QwtPlotCurve(tr("Upper bound"));
    lowerBoundCurve = new QwtPlotCurve(tr("Lower bound"));
    averageCurve = new QwtPlotCurve(tr("Moving average"));

    upperBoundCurve->setPen( QPen( Qt::black, 2, Qt::DotLine ) ),
    upperBoundCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true);
    lowerBoundCurve->setPen( QPen( Qt::black, 2, Qt::DotLine ) ),
    lowerBoundCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );
    averageCurve->setPen( QPen( Qt::black, 4, Qt::DashLine ) ),
    averageCurve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

    upperBoundCurve->setVisible(false);
    lowerBoundCurve->setVisible(false);
    averageCurve->setVisible(false);
    
    core::IActionsGroupManager::GroupID id = manager->createGroup(tr("Operations"));
    manager->addGroupAction(id, activeSerieCombo);
    manager->addGroupAction(id, pickerAction);
    manager->addGroupAction(id, showStats);
    manager->addGroupAction(id, scaleAction);
    manager->addGroupAction(id, bandsAction);

    id = manager->createGroup(tr("Events"));
    manager->addGroupAction(id, eventsContextWidget);

    id = manager->createGroup(tr("Tags"));
    manager->addGroupAction(id, valueMarkerAction);
    manager->addGroupAction(id, hMarkerAction);
    manager->addGroupAction(id, vMarkerAction);

    id = manager->createGroup(tr("Active Data Series"));
    manager->addGroupAction(id, shiftX.get<0>());
    manager->addGroupAction(id, shiftY.get<0>());
    manager->addGroupAction(id, scaleX.get<0>());
    manager->addGroupAction(id, scaleY.get<0>());
    return widget;
}


void NewChartVisualizer::getInputInfo( std::vector<core::IInputDescription::InputInfo>& info )
{
    core::IInputDescription::InputInfo input;

    input.type = typeid(ScalarChannelReaderInterface);
    input.name = "Scalar";
    input.modify = false;
    input.required = false;

    info.push_back(input);
}

core::IVisualizer::SerieBase * NewChartVisualizer::createSerie( const core::ObjectWrapperConstPtr& data, const std::string& name )
{
    NewChartSerie * ret = new NewChartSerie(this);
    ret->setName(name);
    ret->setData(data);
    series.push_back(ret);

    plotChanged();

    statsTable->addEntry(QString("Whole chart"), QString(name.c_str()), ret->getStats());

    if(series.size() == 1){
        activeSerieCombo->blockSignals(true);
        activeSerieCombo->clear();
        activeSerieCombo->addItem(name.c_str());
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->blockSignals(false);
        setActiveSerie(0);
    }else{
        activeSerieCombo->addItem(name.c_str());
    }

    activeSerieCombo->setEnabled(true);
    NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(ret->getCurve()));
    legendLabel->setToolTip(data->getSource().c_str());
    if(series.size() == 1){
        
        if(legendLabel != nullptr && legendLabel->isItemActive() == false){
            legend->blockSignals(true);
            legendLabel->setItemActive(true);
            legendLabel->setItemVisibleEnabled(false);
            legend->blockSignals(false);
        }
    }

    adjustOffsetStep(shiftSpinX, QwtPlot::xBottom);
    adjustOffsetStep(shiftSpinY, QwtPlot::yLeft);

    return ret;
}

core::IVisualizer::SerieBase * NewChartVisualizer::createSerie( const core::IVisualizer::SerieBase * serie )
{
    return nullptr;
}

void NewChartVisualizer::addPlotCurve( QwtPlotCurve* curve, const Scales& scales)
{
    plotScales.merge(scales);

    qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
    qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
    curve->attach(qwtPlot);
}

//TODO
//kod debugowy - do usuniêcia na koniec
void NewChartVisualizer::onComboDestroy(QObject * obj)
{
    obj = nullptr;
}

void NewChartVisualizer::removeSerie( core::IVisualizer::SerieBase *serie )
{
    NewChartSerie* chSerie = dynamic_cast<NewChartSerie*>(serie);

    if(chSerie == nullptr){
        throw std::runtime_error("Serie type not generated by this Visualizer!");
    }

    auto it = std::find(series.begin(), series.end(), chSerie);
    if(it == series.end()){
        throw std::runtime_error("Given serie dos not belong to this visualizer!");
    }

    activeSerieCombo->blockSignals(true);
    updateFIFO.push_back(boost::bind( &NewChartSerie::removeItemsFromPlot, *it ));
    updateFIFO.push_back(boost::bind( &QwtPlot::replot, qwtPlot));
    series.erase(it);
    
    if(series.empty() == true){
        activeSerieCombo->addItem(tr("No active serie"));
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->setEnabled(false);
    }else{
        activeSerieCombo->clear();
        int active = 0;
        int count = series.size();
        for (int i = 0; i < count; i++) {
            NewChartSerie* serie = series[i];
            activeSerieCombo->addItem(serie->getName().c_str());
            if (serie->isActive()) {
                active = i;
            }
        }
    
        activeSerieCombo->setCurrentIndex(active);
    }
    
    activeSerieCombo->blockSignals(false);

    plotChanged();
}

void NewChartVisualizer::setActiveSerie( int idx )
{
    bool layersRefreshRequired = false;

    //ostatnia zmieniamy na nieaktywna
    if(currentSerie > -1){
        NewChartSerie* serie = series[currentSerie];
        serie->setActive(false);

        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
            //helper->getEventsItem()->detach();
             helper->getEventsItem()->setVisible(false);
        }

        //odznacz w legendzie
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(serie->curve));
        if(legendLabel != nullptr && legendLabel->isItemActive() == true){
            legendLabel->blockSignals(true);
            legendLabel->setItemActive(false);
            legendLabel->blockSignals(false);
        }

        currentSerie = -1;
        layersRefreshRequired = true;
        picker->setCurrentCurve(nullptr);
    }

    if(idx > -1){
        currentSerie = idx;
        NewChartSerie* serie = series[currentSerie];
        serie->setActive(true);
        picker->setCurrentCurve(serie->curve);

        refreshSpinBoxes();

        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
            helper->getEventsItem()->setVisible(context != C3DEventsCollection::Context::General);
            //helper->getEventsItem()->attach(qwtPlot);
        }

        //zaznacz w legendzie
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(serie->curve));
        if(legendLabel != nullptr && legendLabel->isItemActive() == false){
            legendLabel->blockSignals(true);
            legendLabel->setItemActive(true);
            legendLabel->blockSignals(false);
        }
        
        layersRefreshRequired = true;
    }

    if(layersRefreshRequired == true){
        refreshSerieLayers();
    }
}

void NewChartVisualizer::refreshSerieLayers()
{
    qwtPlot->setAutoReplot(false);
    qwtPlot->blockSignals(true);
    int size = series.size();
    for(int i = 0; i < size; ++i){
        series[i]->setZ(i == currentSerie ? 1.0 : (double)i / (double)size , true);
    }

    qwtPlot->updateLayout();
    qwtPlot->blockSignals(false);
    qwtPlot->setAutoReplot(true);
    qwtPlot->replot();
}

void NewChartVisualizer::setNormalized( bool normalized )
{

}

void NewChartVisualizer::onSerieSelected(QwtPlotItem* item, bool on, int idx)
{
    legend->blockSignals(true);
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
    if (!isCurveFromSerie(curve)) {
        QPen pen = curve->pen();
        QColor c = pen.color();
        c.setRed(255 - c.red());
        pen.setColor(c);
        curve->setPen(pen);
    } else {
        idx = -1; // idx nie chcemy wykorzystywac!
        if (on == true) {
        
            for (int i = 0; i < series.size(); i++) {
                NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(series[i]->curve));
                if (series[i]->curve == curve) {
                    // powinno wywolac sygnal, ktory ustawi aktywna serie
                    activeSerieCombo->setCurrentIndex(i);
                    legendLabel->setItemVisible(true);
                    series[i]->setVisible(true);
                    legendLabel->setItemVisibleEnabled(false);
                }else{                
                    if(legendLabel != nullptr && legendLabel->isItemActive() == true){
                        legendLabel->setItemActive(false);
                        legendLabel->setItemVisibleEnabled(true);
                    }
                }
            }
        }else{
            //ignorujemy to - zawsze musi byæ jedna seria aktywna
            NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(item));
            if(legendLabel != nullptr && legendLabel->isItemActive() == false){
                legendLabel->setItemActive(true);
                legendLabel->blockSignals(false);
            }
        }
    }
    legend->blockSignals(false);
}

void NewChartVisualizer::onSerieSelected( QwtPlotItem* item)
{
    onSerieSelected(item, true, -1);
    //QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
    //for (int i = series.size() - 1; i >= 0; --i) {
    //    if (series[i]->curve == curve) {
    //        // powinno wywolac sygnal, ktory ustawi aktywna serie
    //        activeSerieCombo->setCurrentIndex(i);
    //        return;
    //    }
    //}
}

void NewChartVisualizer::onStateAction()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (currentState) {
        currentState->stateEnd();
    }
    currentState = statesMap[action];
    currentState->stateBegin();
}

bool NewChartVisualizer::eventFilter( QObject *object, QEvent *event )
{
    if (currentState && !currentState->stateEventFilter(object, event)) {
        return QObject::eventFilter(object, event);
    }

    return true;
}

QwtPlot* NewChartVisualizer::getPlot()
{
    UTILS_ASSERT(qwtPlot);
    return qwtPlot;
}

void NewChartVisualizer::setManipulation( bool val )
{
    UTILS_ASSERT(plotPanner && plotMagnifier);
    plotPanner->setEnabled(val);
    plotMagnifier->setEnabled(val);
}
const NewChartSerie* NewChartVisualizer::tryGetCurrentSerie() const
{
    if (currentSerie >= 0 && currentSerie < series.size()) {
        return series[currentSerie];
    }
    return nullptr;
}

NewChartSerie* NewChartVisualizer::tryGetCurrentSerie()
{
    if (currentSerie >= 0 && currentSerie < series.size()) {
        return series[currentSerie];
    }
    return nullptr;
}

void NewChartVisualizer::update( double deltaTime )
{
    for (auto it = updateFIFO.begin(); it != updateFIFO.end(); it++) {
        (*it)();
    }
    updateFIFO.clear();
    if (currentSerie >= 0 && currentSerie < series.size()) {
        
        qwtMarker->setVisible(true);
        NewChartSerie* serie = series[currentSerie];
        float x = serie->getTime();
        float y = serie->getCurrentValue();
        qwtMarker->setXValue(x * serie->getXScale() + serie->getXOffset());
        qwtMarker->setYValue(y * serie->getYScale() + serie->getYOffset());
        auto helper = serie->getEventsHelper();
        if (isEventMode() && helper) {
            EventsHelper::SegmentConstPtr segment = helper->getSegment(x, this->context);
            if (segment != oldSegment) {
                recreateStats(segment ? segment->stats : ScalarChannelStatsConstPtr());
                setScale(this->scaleToActive, segment ? true : false);
                oldSegment = segment;
            }

        }
    } else {
        qwtMarker->setVisible(false);
    }

    
}

void NewChartVisualizer::setShowLegend( bool val )
{
    if (qwtPlot) {
        // todo - sprawdzic wyciek...
        qwtPlot->insertLegend( val ? legend : nullptr, QwtPlot::RightLegend );
    }
    showLegend = val;
}

const std::string& NewChartVisualizer::getName() const
{
    static std::string name = "Visualizer 2D";
    return name;
}

core::IVisualizer* NewChartVisualizer::createClone() const
{
    return new NewChartVisualizer();
}

QIcon* NewChartVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/resources/icons/2D.png"));
}


void NewChartVisualizer::reset()
{

}

void NewChartVisualizer::setUp( core::IObjectSource* source )
{

}

int NewChartVisualizer::getMaxDataSeries( void ) const
{
    return -1;
}

void NewChartVisualizer::setEvents(NewChartSerie* serie, EventsCollectionConstPtr val )
{
    C3DEventsCollection::Context c = static_cast<C3DEventsCollection::Context>(eventsMenu->itemData(eventsMenu->currentIndex()).toInt());
    bool eventMode = (c != C3DEventsCollection::Context::General);
    EventsHelperPtr helper = serie->getEventsHelper();
    helper->getEventsItem()->attach(qwtPlot);
    helper->getEventsItem()->setVisible(serie == tryGetCurrentSerie() && eventMode);
    int no = 0;
    for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); segment++) {
        statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
    }
    no = 0;
    for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); segment++) {
        statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
    }
}

void NewChartVisualizer::onEventContext(int index)
{
    C3DEventsCollection::Context c = static_cast<C3DEventsCollection::Context>(eventsMenu->itemData(index).toInt());
    bool eventMode = (c != C3DEventsCollection::Context::General);
    NewChartSerie* serie = tryGetCurrentSerie();
    if (serie) {
        auto helper = serie->getEventsHelper();
        if (helper && helper->getEventsItem()) {
            helper->getEventsItem()->setVisible(eventMode);
            context = c;
            //setScale(scaleToActive, eventMode);
        }
    }
    setScale();
}

void NewChartVisualizer::rescale( float t1, float t2 )
{

}

void NewChartVisualizer::recreateStats( ScalarChannelStatsConstPtr stats /*= ScalarChannelStatsConstPtr()*/ )
{
    statsTable->clear();
    for (auto it = series.begin(); it != series.end(); it++) {
        statsTable->addEntry(tr("Whole chart"), (*it)->getName().c_str(), (*it)->getStats() );
    }
    if (stats) {
        QString group = context == C3DEventsCollection::Context::Left ? tr("Left") : tr("Right");
        QColor color  = context == C3DEventsCollection::Context::Left ?  QColor(255, 200, 200) : QColor(200, 255, 200);
        statsTable->addEntry(group, stats->getChannel()->getName().c_str(), stats, color);
    } else {
        for (auto it = series.begin(); it != series.end(); it++) {
            int no = 0;
            EventsHelperPtr helper = (*it)->getEventsHelper();
            for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); segment++) {
                statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg((*it)->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
            }
            no = 0;
            for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); segment++) {
                statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg((*it)->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
            }
        }
    }
}

void NewChartVisualizer::showStatistics( bool visible )
{
    if (visible) {
        statsTable->recalculateHeight();
    }
    statsTable->setVisible(visible);
}

void NewChartVisualizer::onSerieVisible(const QwtPlotItem* dataSerie, bool visible )
{
    legend->blockSignals(true);
    const QwtPlotCurve* curve = dynamic_cast<const QwtPlotCurve*>(dataSerie);
    if (!isCurveFromSerie(curve)) {
        QwtPlotCurve* c = const_cast<QwtPlotCurve*>(curve);
        c->setVisible(visible);
    } else {
        for (int i = 0; i < series.size(); i++) {
            if (series[i]->curve == curve) {
                if (!series[i]->isActive()) {
                    series[i]->setVisible(visible);
                    auto list = statsTable->getEntriesByChannel(series[i]->getStats()->getChannel());
                    BOOST_FOREACH(QTreeWidgetItem* item, list) {
                        item->setHidden(!visible);
                    }
                    for (auto it = statesMap.begin(); it != statesMap.end(); it++) {
                        NewChartLabelStatePtr labelState = core::dynamic_pointer_cast<NewChartLabelState>(it->second);
                        if (labelState) {
                            labelState->setVisible(series[i], visible);
                        }
                    }
                } else {
                    // aktywna seria nie moze byc niewidoczna, trzeba zapobiegac tej sytuacji wylaczajac checkBoxa
                    UTILS_ASSERT(false, "Active serie has to be visible");
                }

                break;
            }
        }
    }

    recreateScales();
    if (context == C3DEventsCollection::Context::General || !timeInsideEvent()) {
        setScale();
    }
    legend->blockSignals(false);

    plotChanged();
}

void NewChartVisualizer::recreateScales()
{
    plotScales.clear();
    for (auto it = series.begin(); it != series.end(); it++) {
        if ((*it)->isVisible()) {
            plotScales.merge((*it)->getScales());
        }
    }
    UTILS_ASSERT(plotScales.isInitialized());

}

bool NewChartVisualizer::timeInsideEvent()
{
    NewChartSerie* serie = series[currentSerie];

    auto helper = serie->getEventsHelper();
    if (helper) {
        if (helper->getSegment(serie->getTime(), this->context)) {
            return true;
        }
    }

    return false;
}

void NewChartVisualizer::scaleToActiveSerie( bool scaleToActive)
{
    this->scaleToActive = scaleToActive;
    setScale(scaleToActive, context != C3DEventsCollection::Context::General);
}

void NewChartVisualizer::setScale( bool scaleToActive, bool eventMode )
{
    UTILS_ASSERT(percentDraw);
   
    if (eventMode) {
        NewChartSerie* serie = series[currentSerie];
        float x = serie->getTime();
        EventsHelperPtr h = serie->getEventsHelper();
        if (h) {
            EventsHelper::SegmentConstPtr segment = h->getSegment(x, this->context);
            if (segment) {
                if (scaleToActive) {
                    percentDraw->setPercentMode(true);
                    percentDraw->setLeftRightValues(segment->begin, segment->end);
                    qwtPlot->setAxisScale(QwtPlot::yLeft, segment->stats->minValue(), segment->stats->maxValue());
                    qwtPlot->setAxisScaleDiv(QwtPlot::xBottom, percentDraw->getScaleDiv());
                } else {
                    float minY = segment->stats->minValue();
                    float maxY = segment->stats->maxValue();
                    for (auto it = series.begin(); it != series.end(); it++) {
                        if ((*it)->isVisible()) {
                            auto h = (*it)->getEventsHelper();
                            if (h) {
                                EventsHelper::SegmentConstPtr s = h->getSegment(x, this->context);
                                if (s) {
                                    minY = (std::min)(minY, s->stats->minValue());
                                    maxY = (std::max)(maxY, s->stats->maxValue());
                                }
                            }
                        }
                    }

                    percentDraw->setLeftRightValues(segment->begin, segment->end);
                    percentDraw->setPercentMode(true);
                    qwtPlot->setAxisScale(QwtPlot::yLeft, minY, maxY);
                    qwtPlot->setAxisScaleDiv(QwtPlot::xBottom, percentDraw->getScaleDiv());
                }
            } else {
                setGlobalScales(scaleToActive);
            }
        }
    } else {
        setGlobalScales(scaleToActive);

    }

    adjustOffsetStep(shiftSpinX, QwtPlot::xBottom);
    adjustOffsetStep(shiftSpinY, QwtPlot::yLeft);
}

void NewChartVisualizer::setScale()
{
    NewChartSerie* serie = tryGetCurrentSerie();
    if (serie) {
       if (isEventMode()) {
           float x = serie->getTime();
           auto helper = serie->getEventsHelper();
           if (helper) {
               EventsHelper::SegmentConstPtr segment = helper->getSegment(x, this->context);
               setScale(this->scaleToActive, segment ? true : false);
           } else {
               setGlobalScales(scaleToActive);
           }
       } else {
           setGlobalScales(scaleToActive);
       }
    }    
}


void NewChartVisualizer::setGlobalScales(bool scaleToActive)
{
    UTILS_ASSERT(percentDraw);
    NewChartSerie* serie = tryGetCurrentSerie();
    if (serie) {
        percentDraw->setPercentMode(false);
        if (scaleToActive) {
            Scales s = serie->getScales();
            qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
            qwtPlot->setAxisScale(QwtPlot::yLeft, s.getYMin(), s.getYMax());
        } else {
            qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
            qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
        }
    }
}

void NewChartVisualizer::onShiftX( double d )
{
    NewChartSerie* s = tryGetCurrentSerie();
    if (s) {
        s->setXOffset(d);
        qwtPlot->replot();
        plotChanged();
    }
}

void NewChartVisualizer::onShiftY( double d )
{
    NewChartSerie* s = tryGetCurrentSerie();
    if (s) {
        s->setYOffset(d);
        qwtPlot->replot();
        plotChanged();
    }
}

void NewChartVisualizer::onScaleX( double d )
{ 
    NewChartSerie* s = tryGetCurrentSerie();
    if (s && d != 0.0) {
       
        s->setXScale(d);
        qwtPlot->replot();

        plotChanged();
    }
}

void NewChartVisualizer::onScaleY( double d )
{
    NewChartSerie* s = tryGetCurrentSerie();
    if (s && d != 0.0) {
        s->setYScale(d);
        qwtPlot->replot();

        plotChanged();
    }
}

void NewChartVisualizer::refreshSpinBoxes()
{
    NewChartSerie* serie = tryGetCurrentSerie();
    if (serie) {
        shiftSpinX->blockSignals(true);
        shiftSpinX->setValue(serie->getXOffset());
        shiftSpinX->blockSignals(false);

        shiftSpinY->blockSignals(true);
        shiftSpinY->setValue(serie->getYOffset());
        shiftSpinY->blockSignals(false);

        scaleSpinX->blockSignals(true);
        scaleSpinX->setValue(serie->getXScale());
        scaleSpinX->blockSignals(false);

        scaleSpinY->blockSignals(true);
        scaleSpinY->setValue(serie->getYScale());
        scaleSpinY->blockSignals(false);
    }
}

void NewChartVisualizer::adjustOffsetStep( QDoubleSpinBox* spinBox, QwtPlot::Axis axis)
{
    double upper = qwtPlot->axisScaleDiv(axis).upperBound();
    double lower = qwtPlot->axisScaleDiv(axis).lowerBound();
    if (lower < upper) {
        spinBox->setSingleStep((upper - lower) / 50.0);
    }

}

void NewChartVisualizer::plotChanged()
{
    if(boundsAutoRefresh == true){
        refreshBounds();
    }else{
        boundsToRefresh = true;
    }
}

void NewChartVisualizer::refreshBounds()
{
    boundsToRefresh = false;
    if(series.empty() == true){
        return;
    }

    float minT = std::numeric_limits<float>::min();
    float maxT = std::numeric_limits<float>::max();

    //wyznaczamy parametry dla wsteg i œrednich
    //minimalny czas dla wszystkich seri danych
    //maksymalny czas dla wszystkich serii danych
    //minimalna rozdzielczoœæ dla wszystkich kana³ów
    std::vector<ScalarChannelReaderInterfaceConstPtr> channels;
    for (auto it = series.begin(); it != series.end(); it++) {
        ScalarChannelReaderInterfaceConstPtr data((*it)->getData()->get());
        minT = std::max(minT, data->argument(0));
        maxT = std::min(maxT, data->argument(data->size() - 1));
        channels.push_back(data);
    }

    int totalWindows = (maxT - minT) / movingAverageTimeWindow;

    if(totalWindows == 0){
        return;
    }

    //obcinam max do ca³kowitej liczby okien
    //maxT = std::min(maxT, (float)totalWindows * (float)movingAverageTimeWindow);
    maxT = totalWindows * movingAverageTimeWindow;
    //wyliczam ilosc wszystkich punktow jakie mam
    int totalPoints = totalWindows * pointsPerWindow;
    //wyliczam pojedynczy krok czasu
    float timeStep = (maxT - minT) / (float)totalPoints;
    //dzielnik dla œredniej
    float size = channels.size();
    //akcesor do danych
    core::shared_ptr<ScalarContiniousTimeAccessor> accessor(new ScalarContiniousTimeAccessor(channels.front()));

    //wyliczamy próbki czasowe
    //wyliczam œredni¹ dla wszystkich próbek
    std::vector<float> timeValues;
    std::vector<float> avgValues;
    int i = 0;
    for(auto time = minT; i < totalPoints; ++i, time = minT + (float)i * timeStep){
        float value = 0;
        for (auto channelIT = channels.begin(); channelIT != channels.end(); ++channelIT) {
            accessor->setChannel(*channelIT);
            value += accessor->getValue(time);
        }
        
        timeValues.push_back(time);
        avgValues.push_back(value / size);
    }

    int outBegIdx = 0;
    int outNBElement = 0;

    //potem liczê œredni¹ krocz¹c¹ z danym oknem    
    //odœwie¿yæ œredni¹ krocz¹c¹
    //odœwie¿yæ wstêgi

    std::vector<float> maAvgValues(totalPoints);
    std::vector<float> upperBandValues(totalPoints);
    std::vector<float> lowerBandValues(totalPoints);

    bbands(0, totalPoints-1, avgValues, pointsPerWindow, 5, 5, outBegIdx,
        outNBElement, upperBandValues, maAvgValues, lowerBandValues);

    QPolygonF pointsAvg;
    QPolygonF pointsUp;
    QPolygonF pointsDown;
    for(int i = 0; i < totalPoints; ++i){
        auto time = timeValues[i];
        pointsAvg << QPointF( time, maAvgValues[i] );
        pointsUp << QPointF( time, upperBandValues[i] );
        pointsDown << QPointF( time, lowerBandValues[i] );
    }
    
    upperBoundCurve->setSamples( pointsUp );
    lowerBoundCurve->setSamples( pointsDown );
    averageCurve->setSamples( pointsAvg );
}

void NewChartVisualizer::showBands(bool show)
{
    showDataBounds(show);
    showMovingAverageCurve(show);
    qwtPlot->replot();
}

void NewChartVisualizer::showDataBounds(bool show)
{
    upperBoundCurve->setVisible(show);
    lowerBoundCurve->setVisible(show);
    if (show) {
        upperBoundCurve->attach(qwtPlot);
        lowerBoundCurve->attach(qwtPlot);
    } else {
        upperBoundCurve->detach();
        lowerBoundCurve->detach();
    }
}

void NewChartVisualizer::showMovingAverageCurve(bool show)
{
    averageCurve->setVisible(show);
    if (show) {
        averageCurve->attach(qwtPlot);
    } else {
        averageCurve->detach();
    }
}

void NewChartVisualizer::setAutoRefreshDataBounds(bool autorefresh)
{
    boundsAutoRefresh = autorefresh;

    if(boundsAutoRefresh == true && boundsToRefresh == true){
        refreshBounds();
    }
}

void NewChartVisualizer::setMovingAverageTimeWindow(double timeWindow)
{
    movingAverageTimeWindow = timeWindow;
    refreshBounds();
}

void NewChartVisualizer::simpleMovingAverage(int startIdx, int endIdx, const std::vector<float> & inReal,
    int optInTimePeriod, int & outBegIdx, int & outNBElement, std::vector<float> & outReal)
{
    double periodTotal, tempReal;
    int i, outIdx, trailingIdx, lookbackTotal;
    lookbackTotal = (optInTimePeriod-1);
    if( startIdx < lookbackTotal )
        startIdx = lookbackTotal;
    if( startIdx > endIdx )
    {
        outBegIdx = 0 ;
        outNBElement = 0 ;
        return;
    }
    periodTotal = 0;
    trailingIdx = startIdx-lookbackTotal;
    i=trailingIdx;
    if( optInTimePeriod > 1 )
    {
        while( i < startIdx )
            periodTotal += inReal[i++];
    }
    outIdx = 0;
    do
    {
        periodTotal += inReal[i++];
        tempReal = periodTotal;
        periodTotal -= inReal[trailingIdx++];
        outReal[outIdx++] = tempReal / optInTimePeriod;
    } while( i <= endIdx );
    outNBElement = outIdx;
    outBegIdx = startIdx;
}

void NewChartVisualizer::bbands( int startIdx, int endIdx, const std::vector<float> & inReal,
    int optInTimePeriod, double optInNbDevUp, double optInNbDevDn, int & outBegIdx, int & outNBElement,
    std::vector<float> & outRealUpperBand,
    std::vector<float> & outRealMiddleBand,
    std::vector<float> & outRealLowerBand)
{
    int i;
    double tempReal, tempReal2;
    std::vector<float> & tempBuffer1 = outRealMiddleBand;
    std::vector<float> tempBuffer2 ;
    if( startIdx < 0 )
        return;
    if( (endIdx < 0) || (endIdx < startIdx))
        return;
    if( optInTimePeriod == std::numeric_limits<int>::min() )
        optInTimePeriod = 5;
    else if( (optInTimePeriod < 2) || (optInTimePeriod > 100000) )
        return;
    if( optInNbDevUp == (-4e+37) )
        optInNbDevUp = 2.000000e+0;
    else if( (optInNbDevUp < -3.000000e+37) || (optInNbDevUp > 3.000000e+37) )
        return;
    if( optInNbDevDn == (-4e+37) )
        optInNbDevDn = 2.000000e+0;
    else if( (optInNbDevDn < -3.000000e+37) || (optInNbDevDn > 3.000000e+37) )
        return ;
    //tempBuffer1 = outRealMiddleBand;
    tempBuffer2 = outRealLowerBand;
    
    simpleMovingAverage(startIdx, endIdx, inReal,
        optInTimePeriod, outBegIdx, outNBElement, tempBuffer1 );
    
    stddev_using_precalc_ma ( inReal, tempBuffer1,
        outBegIdx , outNBElement,
        optInTimePeriod, tempBuffer2 );
    
    if( optInNbDevUp == optInNbDevDn )
    {
        if( optInNbDevUp == 1.0 )
        {
            for( i=0; i < outNBElement ; i++ )
            {
                tempReal = tempBuffer2[i];
                tempReal2 = outRealMiddleBand[i];
                outRealUpperBand[i] = tempReal2 + tempReal;
                outRealLowerBand[i] = tempReal2 - tempReal;
            }
        }
        else
        {
            for( i=0; i < outNBElement ; i++ )
            {
                tempReal = tempBuffer2[i] * optInNbDevUp;
                tempReal2 = outRealMiddleBand[i];
                outRealUpperBand[i] = tempReal2 + tempReal;
                outRealLowerBand[i] = tempReal2 - tempReal;
            }
        }
    }
    else if( optInNbDevUp == 1.0 )
    {
        for( i=0; i < outNBElement ; i++ )
        {
            tempReal = tempBuffer2[i];
            tempReal2 = outRealMiddleBand[i];
            outRealUpperBand[i] = tempReal2 + tempReal;
            outRealLowerBand[i] = tempReal2 - (tempReal * optInNbDevDn);
        }
    }
    else if( optInNbDevDn == 1.0 )
    {
        for( i=0; i < outNBElement ; i++ )
        {
            tempReal = tempBuffer2[i];
            tempReal2 = outRealMiddleBand[i];
            outRealLowerBand[i] = tempReal2 - tempReal;
            outRealUpperBand[i] = tempReal2 + (tempReal * optInNbDevUp);
        }
    }
    else
    {
        for( i=0; i < outNBElement ; i++ )
        {
            tempReal = tempBuffer2[i];
            tempReal2 = outRealMiddleBand[i];
            outRealUpperBand[i] = tempReal2 + (tempReal * optInNbDevUp);
            outRealLowerBand[i] = tempReal2 - (tempReal * optInNbDevDn);
        }
    }
}

void NewChartVisualizer::stddev_using_precalc_ma( const std::vector<float> & inReal,
    const std::vector<float> & inMovAvg,
    int inMovAvgBegIdx,
    int inMovAvgNbElement,
    int timePeriod,
    std::vector<float> & output)
{
    double tempReal, periodTotal2, meanValue2;
    int outIdx;
    int startSum, endSum;
    startSum = 1+inMovAvgBegIdx-timePeriod;
    endSum = inMovAvgBegIdx;
    periodTotal2 = 0;
    for( outIdx = startSum; outIdx < endSum; outIdx++)
    {
        tempReal = inReal[outIdx];
        tempReal *= tempReal;
        periodTotal2 += tempReal;
    }
    for( outIdx=0; outIdx < inMovAvgNbElement; outIdx++, startSum++, endSum++ )
    {
        tempReal = inReal[endSum];
        tempReal *= tempReal;
        periodTotal2 += tempReal;
        meanValue2 = periodTotal2/timePeriod;
        tempReal = inReal[startSum];
        tempReal *= tempReal;
        periodTotal2 -= tempReal;
        tempReal = inMovAvg[outIdx];
        tempReal *= tempReal;
        meanValue2 -= tempReal;
        if( ! (meanValue2<0.00000001) )
            output[outIdx] = std::sqrt(meanValue2);
        else
            output[outIdx] = 0;
    }
}

bool NewChartVisualizer::isCurveFromSerie(const QwtPlotCurve* curve) const
{
    for (auto it = series.begin(); it != series.end(); it++) {
        if ((*it)->curve == curve) {
            return true;
        }
    }

    return false;
}

void NewChartVisualizer::setLabelsVisible( bool val )
{
    for (auto serie = series.begin(); serie != series.end(); serie++) {
        for (auto it = statesMap.begin(); it != statesMap.end(); it++) {
            NewChartLabelStatePtr labels = core::dynamic_pointer_cast<NewChartLabelState>(it->second);
            if (labels) {
                labels->setVisible(*serie, val & (*serie)->isVisible());
            }
        }
    }
}

boost::iterator_range<std::vector<NewChartSerie*>::const_iterator> NewChartVisualizer::getSeries() const
{
    return boost::make_iterator_range(series.cbegin(), series.cend());
}




