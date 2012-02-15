#include "NewChartPCH.h"
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include <QtGui/QCheckBox>
#include <QtGui/QPushButton>
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


NewChartVisualizer::NewChartVisualizer() : 
showLegend(true), 
    currentSerie(-1),
    plotPanner(nullptr),
    plotMagnifier(nullptr),
    statsTable(nullptr),
    eventsItem(nullptr),
    eventMode(false),
    eventsVisible(true),
    context(C3DEventsCollection::Context::Left),
    eventsMenu(nullptr), eventsActionGroup(nullptr),
    noneEvents(nullptr), leftEvents(nullptr),
    rightEvents(nullptr)
{

}


NewChartVisualizer::~NewChartVisualizer()
{
    //TODO
    //przejrzec niszczenie dynamicznie tworzonych zasobów
    if(eventsMenu != nullptr){
        delete eventsMenu;
    }

    if(eventsActionGroup != nullptr){
        delete eventsActionGroup;
    }
}


QWidget* NewChartVisualizer::createWidget( core::IActionsGroupManager * manager )
{
    QWidget* widget = new QWidget();
    widget->setObjectName(QString::fromUtf8("newChartVisualizerWidget"));
    QwtText txt(getName().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    qwtPlot->setObjectName(QString::fromUtf8("plot"));
    //aby legenda nie by³a usuwana podczas chowania i pokazywania trzeba ustawiæ parenta innego ni¿ QwtPlot
    legend = new NewChartLegend(widget);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
  
    qwtPlot->setAutoReplot(true);

    if (isShowLegend()) {    
        //legend->setItemMode(QwtLegend::CheckableItem);
        qwtPlot->insertLegend( legend, QwtPlot::BottomLegend );
    }

    qwtPlot->canvas()->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);

    activeSerieCombo = new QComboBox();

    //connect(activeSerieCombo, SIGNAL(destroyed ( QObject *)), this, SLOT(onComboDestroy(QObject*)));

    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    //QAction* eventMode = new QAction(QString(tr("Event mode")), nullptr);
    //QIcon icon0;
    //icon0.addFile(QString::fromUtf8(":/resources/icons/normalizacja1.png"), QSize(), QIcon::Normal, QIcon::Off);
    //icon0.addFile(QString::fromUtf8(":/resources/icons/normalizacja1a.png"), QSize(), QIcon::Normal, QIcon::On);
    //eventMode->setIcon(icon0);
    //eventMode->setCheckable(true);
    //eventMode->setChecked(false);
    ////actions.push_back(eventMode);
    //connect(eventMode, SIGNAL(triggered(bool)), this, SLOT(setEventMode(bool)));

    eventsMenu = new QMenu("Event Context");
    
    eventsActionGroup = new QActionGroup(widget);
    noneEvents = eventsMenu->addAction(tr("None events"));
    leftEvents = eventsMenu->addAction(tr("Left events"));
    rightEvents = eventsMenu->addAction(tr("Right events"));

    noneEvents->setCheckable(true);
    leftEvents->setCheckable(true);
    rightEvents->setCheckable(true);

    eventsActionGroup->addAction(noneEvents);
    eventsActionGroup->addAction(leftEvents);
    eventsActionGroup->addAction(rightEvents);

    connect(noneEvents, SIGNAL(triggered()), this, SLOT(onEventContext()));
    connect(leftEvents, SIGNAL(triggered()), this, SLOT(onEventContext()));
    connect(rightEvents, SIGNAL(triggered()), this, SLOT(onEventContext()));

    picker.reset(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/picker.png"), QSize(), QIcon::Normal, QIcon::Off);
    QAction * pickerAction = new QAction("Picker", this);
    pickerAction->setIcon(icon1);
    statesMap[pickerAction] = picker;
    connect(pickerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
    
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/value_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    QAction * valueMarkerAction = new QAction("Value Marker", this);
    valueMarkerAction->setIcon(icon2);
    statesMap[valueMarkerAction] =  NewChartStatePtr(new NewChartValueMarker(this));
    connect(valueMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/vertical_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    QAction * hMarkerAction = new QAction("Horizontal Marker", this);
    hMarkerAction->setIcon(icon3);
    statesMap[hMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Horizontal));
    connect(hMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/resources/icons/horizontal_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    QAction * vMarkerAction = new QAction("Vertical Marker", this);
    vMarkerAction->setIcon(icon4);
    statesMap[vMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Vertical));
    connect(vMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));

    QAction* showStats = new QAction("Statistics", widget);
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
        /*QwtScaleDraw *scaleDraw = qwtPlot->axisScaleDraw( i );
        if ( scaleDraw ) {
            scaleDraw->enableComponent( QwtAbstractScaleDraw::Backbone, false );
        }*/
    }
    qwtPlot->plotLayout()->setAlignCanvasToScales( true );

    /*zoomer.reset(new QwtPlotZoomer(qwtPlot->canvas()));
    zoomer->setStateMachine(new QwtPickerDragRectMachine);
    zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    zoomer->setRubberBand(QwtPicker::RectRubberBand); */

    plotPanner =  new QwtPlotPanner( qwtPlot->canvas() );
    plotMagnifier =  new QwtPlotMagnifier( qwtPlot->canvas() );
    

    //connect(qwtPlot, SIGNAL(legendClicked(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));
    bool c = connect(legend, SIGNAL(checked( QwtPlotItem *, bool, int)), this, SLOT(onSerieSelected(QwtPlotItem*, bool, int)));
    c = connect(legend, SIGNAL(checkboxChanged(const QwtPlotItem*, bool)), this, SLOT(onSerieVisible(const QwtPlotItem*, bool)));
    //qwtMarker.reset(new QwtPlotMarker());
    qwtMarker = new NewChartMarker();
    qwtMarker->setXValue(0);
    qwtMarker->setYValue(0);
    //qwtMarker->setSymbol( new QwtSymbol( QwtSymbol::Diamond,
    //    QColor( Qt::yellow ), QColor( Qt::green ), QSize( 7, 7 ) ) );
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
    //statsTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    qwtPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    

    /*qwtPlot->canvas()->setLineWidth( 1 );
    qwtPlot->canvas()->setFrameStyle( QFrame::Box | QFrame::Plain );
    qwtPlot->canvas()->setBorderRadius( 15 );*/

    

    /*QSplitter * splitter = new QSplitter();
    splitter->setOrientation(Qt::Vertical);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(qwtPlot);
    splitter->addWidget(statsTable);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(splitter);
    widget->setLayout(layout);*/
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(qwtPlot);
    layout->setMargin(0);
    //layout->setContentsMargins(2, 2, 2, 2);
    //layout->setContentsMargins(2,0,2,2);
    layout->addWidget(statsTable);
    statsTable->setVisible(false);
    widget->setLayout(layout);

    core::IActionsGroupManager::GroupID id = manager->createGroup("Operations");
    manager->addGroupAction(id, activeSerieCombo);
    manager->addGroupAction(id, pickerAction);
    manager->addGroupAction(id, showStats);

    id = manager->createGroup("Events");
    //manager->addGroupAction(id, eventMode);
    manager->addGroupAction(id, eventsMenu);

    id = manager->createGroup("Tags");
    manager->addGroupAction(id, valueMarkerAction);
    manager->addGroupAction(id, hMarkerAction);
    manager->addGroupAction(id, vMarkerAction);

    leftEvents->setChecked(true);

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

    if(series.size() == 1){
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(ret->getCurve()));
        if(legendLabel != nullptr && legendLabel->isItemActive() == false){
            legend->blockSignals(true);
            legendLabel->setItemActive(true);
            legendLabel->setItemVisibleEnabled(false);
            legend->blockSignals(false);
        }
    }

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
    (*it)->removeItemsFromPlot();
    series.erase(it);
    
    if(series.empty() == true){
        activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
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
}

void NewChartVisualizer::setActiveSerie( int idx )
{
    bool layersRefreshRequired = false;

    //ostatnia zmieniamy na nieaktywna
    if(currentSerie > -1){
        series[currentSerie]->setActive(false);

        //odznacz w legendzie
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(series[currentSerie]->curve));
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
        series[currentSerie]->setActive(true);
        picker->setCurrentCurve(series[currentSerie]->curve);

        //zaznacz w legendzie
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(series[currentSerie]->curve));
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

    //legend->setVisible(false);
    //legend->setVisible(true);
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
    idx = -1; // idx nie chcemy wykorzystywac!
    legend->blockSignals(true);
    if(on == true){

        //onSerieSelected(item);
        
        QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
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

void NewChartVisualizer::update( double deltaTime )
{
    if (currentSerie >= 0 && currentSerie < series.size()) {
        
        qwtMarker->setVisible(true);
        NewChartSerie* serie = series[currentSerie];
        float x = serie->getTime();
        float y = serie->getCurrentValue();
        qwtMarker->setXValue(x);
        qwtMarker->setYValue(y);
        qwtMarker->setLabel(QString("Time: %1, Value: %2").arg(x).arg(y));

        //static EventsHelper::SegmentConstPtr oldSegment = EventsHelper::SegmentConstPtr();
        if (eventsVisible && eventMode) {
            auto helper = eventsHelpers.find(serie);
            if (helper != eventsHelpers.end()) {
                EventsHelperPtr h = helper->second;
                EventsHelper::SegmentConstPtr segment = h->getSegment(x, this->context);
                if (segment != oldSegment) {
                    if (segment) {
                        qwtPlot->setAxisScale(QwtPlot::xBottom, segment->begin, segment->end);
                        qwtPlot->setAxisScale(QwtPlot::yLeft, segment->stats->minValue(), segment->stats->maxValue());
                        recreateStats(segment->stats);
                    } else {
                        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
                        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
                        recreateStats();
                    }
                    oldSegment = segment;
                }
            }
        }
        /*if (eventsVisible && eventMode) {
            EventsCollectionConstPtr events = eventsItem->getEvents();
            C3DEventsCollection::EventConstPtr event = events->getEvent(x, context);
            C3DEventsCollection::EventConstPtr nextEvent;
            if (event) {
                nextEvent = events->getNextEvent(event, context); 
                if (nextEvent) {
                    nextEvent = events->getNextEvent(nextEvent, context);
                }
            }

            if (event && nextEvent) {
                qwtPlot->setAxisScale(QwtPlot::xBottom, event->getTime(), nextEvent->getTime());
            } else {
                qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.xMin, plotScales.xMax);
            }
            
        }*/
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
    // jesli nie istnieje jeszcze wizualizacja eventow, to tworzymy obiekt eventow 
    // i dolaczamy go do wykresu.
    if (!eventsItem) {
        eventsItem = new EventsPlotItem(val);
        eventsItem->attach(qwtPlot);
        eventsVisible = true;

    } else if (eventsVisible) {
        // jesli obiekt z eventami juz istnieje, to sprawdzamy, czy dotyczy tych samych eventow
        // jesli tak, to nie trzeba robic nic, bo wizualizujemy dobre eventy
        // jesli nie, to wylaczamy obiekt, poki co nie rysujemy roznych eventow na wykresach
        if (eventsItem->getEvents() != val) {
            eventsVisible = false;
            eventsItem->detach();
        }
    }

    EventsHelperPtr helper(new EventsHelper(val, serie->getReader()));
    eventsHelpers[serie] = helper;
    int no = 0;
    for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); segment++) {
        statsTable->addEntry(QString("Left"), QString("%1: Left step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
    }
    no = 0;
    for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); segment++) {
        statsTable->addEntry(QString("Right"), QString("%1: Right step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
    }
}

void NewChartVisualizer::onEventContext()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action == noneEvents) {
        setEventMode(false);
    }else{
        setEventMode(true);
        if (action == leftEvents) {
            context = C3DEventsCollection::Context::Left;
        } else if (action == rightEvents) {
            context = C3DEventsCollection::Context::Right;
        } else {
            UTILS_ASSERT(false);
        }
    }

    eventsMenu->setTitle(action->text());
}

void NewChartVisualizer::rescale( float t1, float t2 )
{

}

void NewChartVisualizer::recreateStats( ScalarChannelStatsConstPtr stats /*= ScalarChannelStatsConstPtr()*/ )
{
    statsTable->clear();
    for (auto it = series.begin(); it != series.end(); it++) {
        statsTable->addEntry("Whole chart", (*it)->getName().c_str(), (*it)->getStats() );
    }
    if (stats) {
        QString group = context == C3DEventsCollection::Context::Left ? "Left" : "Right";
        QColor color  = context == C3DEventsCollection::Context::Left ?  QColor(255, 200, 200) : QColor(200, 255, 200);
        statsTable->addEntry(group, stats->getChannel()->getName().c_str(), stats, color);
    } else {
        for (auto it = series.begin(); it != series.end(); it++) {
            int no = 0;
            for (auto segment = eventsHelpers[*it]->getLeftSegments().begin(); segment != eventsHelpers[*it]->getLeftSegments().end(); segment++) {
                statsTable->addEntry(QString("Left"), QString("%1: Left step %2").arg((*it)->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
            }
            no = 0;
            for (auto segment = eventsHelpers[*it]->getRightSegments().begin(); segment != eventsHelpers[*it]->getRightSegments().end(); segment++) {
                statsTable->addEntry(QString("Right"), QString("%1: Right step %2").arg((*it)->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
            }
        }
    }
}

void NewChartVisualizer::setEventMode( bool val )
{
    eventMode = val;
    eventsItem->setVisible(val);
    if (!eventMode) {
        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
    }
}

void NewChartVisualizer::showStatistics( bool visible )
{
    if (visible) {
        statsTable->recalculateHeight();
    }
    statsTable->setVisible(visible);
}


const QColor leftColor1(255, 0, 0, 15);
const QColor leftColor2(128, 0, 0, 15);
const QColor rightColor1(0, 255, 0, 15);
const QColor rightColor2(0, 128, 0, 15);

EventsPlotItem::EventsPlotItem( EventsCollectionConstPtr events ) : 
events(events) 
{
    UTILS_ASSERT(events);
}


void EventsPlotItem::draw( QPainter *painter, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect ) const
{
  int count = events->getNumEvents();
  C3DEventsCollection::EventConstPtr lastLeftEvent;
  C3DEventsCollection::EventConstPtr lastRightEvent;

  int half = (canvasRect.bottom() - canvasRect.top())/ 2 + canvasRect.top();
 
  for (int i = 0; i < count; i++) {
      C3DEventsCollection::EventConstPtr event = events->getEvent(i);
      if (event->getContext() != C3DEventsCollection::Context::Left && event->getContext() != C3DEventsCollection::Context::Right) {
          continue;
      }
      int x = static_cast<int>(xMap.transform(event->getTime()));
      
       bool left = event->getContext() == C3DEventsCollection::Context::Left;
      int top = left ? canvasRect.top() : half;
      int bottom = left ? half : canvasRect.bottom();
      
      painter->setFont(QFont("Tahoma", 8));
      painter->setPen(QPen(QColor(0, 0, 0, 70)));
      painter->drawLine(x, top, x, bottom);

      painter->save();
      painter->translate(x + 10, bottom - 30);
      painter->rotate(-90); // or 270
      painter->drawText(0, 0, QString(event->getLabel().c_str()));
      painter->restore();
      //painter->drawText(x + 2, bottom - 30, QString(event->getLabel().c_str()));
      

      if (lastLeftEvent && left) {
          painter->setBrush(QBrush(i % 2 ? leftColor1 : leftColor2));
          int lastX = static_cast<int>(xMap.transform(lastLeftEvent->getTime()));
          painter->drawRect(lastX, top, x - lastX, bottom - top);
      } else if (lastRightEvent && !left) {
          painter->setBrush(QBrush(i % 2 ? rightColor1 : rightColor2));
          int lastX = static_cast<int>(xMap.transform(lastRightEvent->getTime()));
          painter->drawRect(lastX, top, x - lastX, bottom - top);
      } //else if (!lastLeftEvent && left) {
      //    // pierwszy lewy event
      //    painter->setPen(QPen(Qt::white));
      //    painter->setFont(QFont("Tahoma", 72));
      //    painter->setPen(QPen(QColor(255, 0, 0, 35)));
      //    painter->drawText(x + 30, top + 82, "Left");
      //} else if (!lastRightEvent && right) {
      //    // pierwszy prawy event
      //    painter->setPen(QPen(Qt::white));
      //    painter->setFont(QFont("Tahoma", 72));
      //    painter->setPen(QPen(QColor(0,255,  0, 35)));
      //    painter->drawText(x + 30, top + 82, "Right");
      //}

      if (event->getContext() == C3DEventsCollection::Context::Left) {
          lastLeftEvent = event;
      } else {
          lastRightEvent = event;
      }
  }
      
  painter->setFont(QFont("Tahoma", 32));  
  painter->save();
  
  painter->translate(canvasRect.left() + 30, half - 10);
  painter->rotate(-90); 
  painter->setPen(QPen(QColor(255, 0, 0, 55)));
  painter->drawText(0, 0, "Left");
  painter->restore();

  painter->save();
  painter->translate(canvasRect.left() + 30, canvasRect.bottom() - 10);
  painter->rotate(-90); 
  painter->setPen(QPen(QColor(0, 255, 0, 55)));
  painter->drawText(0, 0, "Right");
  painter->restore();
}



void EventsHelper::createSegments(std::vector<SegmentPtr>& collection, C3DEventsCollection::Context context)
{
    SegmentPtr currentSegment;
    for( auto it = events->cbegin(); it != events->cend(); it++) {
        EventConstPtr event = *it;
        if (event->getContext() == context) {
            if (event->getLabel() == "Foot Strike") {
                if (currentSegment) {
                    UTILS_ASSERT(currentSegment->event1);
                    currentSegment->end = event->getTime();
                    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(scalar));
                    currentSegment->stats = ScalarChannelStatsPtr(new ScalarChannelStats(nonConstChannel, currentSegment->begin, currentSegment->end));
                    collection.push_back(currentSegment);
                }

                currentSegment.reset(new Segment());
                currentSegment->event1 = event;
                currentSegment->begin = event->getTime();
            } else if (currentSegment && event->getLabel() == "Foot Off") {
                currentSegment->event2 = event;
            }
        } 
    }
    
    
}

EventsHelper::EventsHelper( EventsCollectionConstPtr events, ScalarChannelReaderInterfaceConstPtr scalar ) :
    events(events),
    scalar(scalar)
{
    createSegments(leftSegments, C3DEventsCollection::Context::Left);
    createSegments(rightSegments, C3DEventsCollection::Context::Right);
}

QWidget * NewChartLegend::createWidget( const QwtLegendData & data ) const
{
    NewChartLegendItem* item = new NewChartLegendItem(data);
    bool c2 = connect( item, SIGNAL( buttonClicked( bool ) ), this,  SLOT( itemChecked( bool ) ) );
    bool c3 = connect( item, SIGNAL( checkClicked(bool)), this, SLOT(onCheck(bool)));
    return item;
}

void NewChartLegend::onCheck(bool checked)
{
    auto it = widget2PlotItem.find(qobject_cast<QWidget*>(sender()));
    if (it != widget2PlotItem.end()) {
        const QwtPlotItem* plotItem = it->second;
        emit checkboxChanged(plotItem, checked);
    } else {
        UTILS_ASSERT(false);
    }
}

void NewChartLegend::updateLegend( const QwtPlotItem *item, const QList<QwtLegendData> &list )
{
    QwtLegend::updateLegend(item, list);
    widget2PlotItem[this->legendWidget(item)] = item;
}


NewChartLegendItem::NewChartLegendItem( const QwtLegendData & data, QWidget* parent /*= nullptr*/ ) :
    QWidget(parent)
{
    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);
    check = new QCheckBox();
    check->setCheckable(true);
    check->setChecked(true);
    l->addWidget(check);

    button = new QPushButton();
    button->setIcon(QIcon(data.icon().toPixmap()));
    button->setStyleSheet(
        "QPushButton:!checked {                    "
        "    border-style: none;                   "
        "}                                         "
        " QPushButton:checked {                    "
        "     border-style: solid;                 "
        "     border-width: 1px;                   "
        "     border-radius: 4px;                  "
        "     border-color: rgb(91, 91, 91);       "
        "}                                         ");

    button->setText(data.title().text());
    button->setCheckable(true);
    l->addWidget(button);
    //l->addWidget(QwtLegend::createWidget(data));
    this->setLayout(l);

    bool c2 = connect(button, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
    bool c3 = connect(check, SIGNAL(clicked(bool)), this, SLOT(onClick(bool)));
}

bool NewChartLegendItem::isItemVisible()
{
    return check->isChecked();
}

void NewChartLegendItem::setItemVisible(bool active)
{
    check->setChecked(active);
}

bool NewChartLegendItem::isItemActive() const
{
    return button->isChecked();
}

void NewChartLegendItem::setItemActive( bool checked )
{
    button->setChecked(checked);
    check->setEnabled(!checked);
}

void NewChartLegendItem::onClick( bool val )
{
    QPushButton* b = qobject_cast<QPushButton*>(sender());
    if (b) {
        emit buttonClicked(val);
        return;
    } 

    QCheckBox* c = qobject_cast<QCheckBox*>(sender());
    if (c) {
        emit checkClicked(val);
        return;
    }

    UTILS_ASSERT(false);
}

void NewChartLegendItem::setItemVisibleEnabled( bool enabled )
{
    check->setEnabled(enabled);
}

void NewChartLegendItem::setItemActiveEnabled( bool enabled )
{
    button->setEnabled(enabled);
}

void NewChartVisualizer::onSerieVisible(const QwtPlotItem* dataSerie, bool visible )
{
    legend->blockSignals(true);
    const QwtPlotCurve* curve = dynamic_cast<const QwtPlotCurve*>(dataSerie);
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
    recreateScales();
    if (!eventMode || !timeInsideEvent()) {
        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
    }
    legend->blockSignals(false);
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
    auto helper = eventsHelpers.find(serie);
    if (helper != eventsHelpers.end()) {
        EventsHelperPtr h = helper->second;
        if (h->getSegment(serie->getTime(), this->context)) {
            return true;
        }
    }

    return false;
}

