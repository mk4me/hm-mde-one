#include "NewChartPCH.h"
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>
#include <QtGui/QSplitter>
#include <QtGui/QPainter>
#include <QtGui/QMenu>
#include "NewChartVisualizer.h"
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_layout.h>
#include <qwt/qwt_legend_label.h>
#include <qwt/qwt_legend.h>
#include "StatsTable.h"
#include "NewChartPicker.h"
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
    eventMode(true),
    eventsVisible(false),
    context(C3DEventsCollection::Context::Left)
{

}


NewChartVisualizer::~NewChartVisualizer()
{

}


QWidget* NewChartVisualizer::createWidget( std::vector<QObject*>& actions )
{
    QWidget* widget = new QWidget();
    QwtText txt(getName().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    //aby legenda nie by³a usuwana podczas chowania i pokazywania trzeba ustawiæ parenta innego ni¿ QwtPlot
    legend = new QwtLegend(widget);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
    qwtPlot->setAutoReplot(true);

    if (isShowLegend()) {    
        //legend->setItemMode(QwtLegend::CheckableItem);
        qwtPlot->insertLegend( legend, QwtPlot::RightLegend );
    }

    qwtPlot->canvas()->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);

    activeSerieCombo = new QComboBox();

    //connect(activeSerieCombo, SIGNAL(destroyed ( QObject *)), this, SLOT(onComboDestroy(QObject*)));

    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    QAction* eventMode = new QAction(QString(tr("Event mode")), nullptr);
    QIcon icon0;
    icon0.addFile(QString::fromUtf8(":/resources/icons/normalizacja1.png"), QSize(), QIcon::Normal, QIcon::Off);
    eventMode->setIcon(icon0);
    eventMode->setCheckable(true);
    //actions.push_back(eventMode);
    connect(eventMode, SIGNAL(triggered(bool)), this, SLOT(setEventMode(bool)));



    QMenu* menu = new QMenu("Event Context");

    QAction* leftEvents = menu->addAction("Left");
    QAction* rightEvents = menu->addAction("Right");
    actions.push_back(menu);


    connect(leftEvents, SIGNAL(triggered()), this, SLOT(onEventContext()));
    connect(rightEvents, SIGNAL(triggered()), this, SLOT(onEventContext()));

    actions.push_back(activeSerieCombo);
    NewChartPickerPtr picker(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));

    //akcje powinny byc zwracane zawsze w takiej samej kolejnosci - inaczej w UI beda za kazdym razem inaczej organizowane!!
    // dlatego od razu w takije kolejnosci jak sa tworzone leca do wektora

    QIcon icon1;
    icon1.addFile(QString::fromUtf8(":/resources/icons/picker.png"), QSize(), QIcon::Normal, QIcon::Off);
    QAction * action = new QAction("Picker", this);
    action->setIcon(icon1);
    actions.push_back(action);
    statesMap[action] = picker;
    
    QIcon icon2;
    icon2.addFile(QString::fromUtf8(":/resources/icons/value_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    action = new QAction("Value Marker", this);
    action->setIcon(icon2);
    actions.push_back(action);
    statesMap[action] =  NewChartStatePtr(new NewChartValueMarker(this));

    QIcon icon3;
    icon3.addFile(QString::fromUtf8(":/resources/icons/vertical_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    action = new QAction("Horizontal Marker", this);
    action->setIcon(icon3);
    actions.push_back(action);
    statesMap[action] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Horizontal));

    QIcon icon4;
    icon4.addFile(QString::fromUtf8(":/resources/icons/horizontal_tag.png"), QSize(), QIcon::Normal, QIcon::Off);
    action = new QAction("Vertical Marker", this);
    action->setIcon(icon4);
    actions.push_back(action);
    statesMap[action] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Vertical));
    
    for (auto it = actions.begin(); it != actions.end(); it++) {
        //actions.push_back(it->first);
        connect(*it, SIGNAL(triggered()), this, SLOT(onStateAction()));
    }

    //nie powinna zmieniaæ stanu wizualizatora wiêc nie ³¹czymy jej ze slotem
    actions.insert(actions.begin(), eventMode);

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
    connect(legend, SIGNAL(checked( QwtPlotItem *, bool, int)), this, SLOT(onSerieSelected(QwtPlotItem*, bool, int)));
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

    QAction* showStats = new QAction("Statistics", widget);
    showStats->setCheckable(true);
    showStats->setChecked(false);
    QIcon iconStats;
    iconStats.addFile(QString::fromUtf8(":/resources/icons/stat-a.png"), QSize(), QIcon::Mode::Normal, QIcon::State::Off);
    iconStats.addFile(QString::fromUtf8(":/resources/icons/stat-b.png"), QSize(), QIcon::Mode::Normal, QIcon::State::On);
    showStats->setIcon(iconStats);
    actions.push_back(showStats);
    connect(showStats, SIGNAL(triggered(bool)), this, SLOT(showStatistics(bool)));
    widget->setLayout(layout);
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
        QwtLegendLabel * legendLabel = qobject_cast<QwtLegendLabel *>(legend->legendWidget(ret->getCurve()));
        if(legendLabel != nullptr && legendLabel->isChecked() == false){
            legend->blockSignals(true);
            legendLabel->setChecked(true);
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
    if (plotScales.initialized) {
        plotScales.xMin = std::min(plotScales.xMin, scales.xMin);
        plotScales.xMax = std::max(plotScales.xMax, scales.xMax);
        plotScales.yMin = std::min(plotScales.yMin, scales.yMin);
        plotScales.yMax = std::max(plotScales.yMax, scales.yMax);
    } else {
        plotScales = scales;
    }

    qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.xMin, plotScales.xMax);
    qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.yMin, plotScales.yMax);
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
            if (serie->getActive()) {
                active = i;
            }
        }

        activeSerieCombo->setCurrentIndex(active);
    }

    activeSerieCombo->blockSignals(false);
}

void NewChartVisualizer::setActiveSerie( int idx )
{
    for (int i = series.size() - 1; i >= 0; --i) {
        series[i]->setActive(idx == i);
    }
    currentSerie = idx;
}

void NewChartVisualizer::setNormalized( bool normalized )
{

}

void NewChartVisualizer::onSerieSelected(QwtPlotItem* item, bool on, int idx)
{
    legend->blockSignals(true);
    if(on == true){
        
        QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
        for (int i = 0; i < series.size(); i++) {
            if (series[i]->curve == curve) {
                // powinno wywolac sygnal, ktory ustawi aktywna serie
                activeSerieCombo->setCurrentIndex(i);
            }else{
                QwtLegendLabel * legendLabel = qobject_cast<QwtLegendLabel *>(legend->legendWidget(series[i]->curve));
                if(legendLabel != nullptr && legendLabel->isChecked() == true){
                    legendLabel->setChecked(false);
                }
            }
        }
    }else{
        //ignorujemy to - zawsze musi byæ jedna seria aktywna
        QwtLegendLabel * legendLabel = qobject_cast<QwtLegendLabel *>(legend->legendWidget(item));
        if(legendLabel != nullptr && legendLabel->isChecked() == false){
            legendLabel->setChecked(true);
        }
    }
    legend->blockSignals(false);
}

void NewChartVisualizer::onSerieSelected( QwtPlotItem* item)
{
    QwtPlotCurve* curve = dynamic_cast<QwtPlotCurve*>(item);
    for (int i = series.size() - 1; i >= 0; --i) {
        if (series[i]->curve == curve) {
            // powinno wywolac sygnal, ktory ustawi aktywna serie
            activeSerieCombo->setCurrentIndex(i);
            return;
        }
    }
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
                        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.xMin, plotScales.xMax);
                        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.yMin, plotScales.yMax);
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
    static std::string name = "NewChartVisualizer";
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
    if (action->text() == "Left") {
        context = C3DEventsCollection::Context::Left;
    } else if (action->text() == "Right") {
        context = C3DEventsCollection::Context::Right;
    } else {
        UTILS_ASSERT(false);
    }
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
    if (!eventMode) {
        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.xMin, plotScales.xMax);
        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.yMin, plotScales.yMax);
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

  for (int i = 0; i < count; i++) {
      C3DEventsCollection::EventConstPtr event = events->getEvent(i);
      if (event->getContext() != C3DEventsCollection::Context::Left && event->getContext() != C3DEventsCollection::Context::Right) {
          continue;
      }
      int x = static_cast<int>(xMap.transform(event->getTime()));
      
      int half = (canvasRect.bottom() - canvasRect.top())/ 2 + canvasRect.top();
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
      } else if (!lastLeftEvent && left) {
          // pierwszy lewy event
          painter->setPen(QPen(Qt::white));
          painter->setFont(QFont("Tahoma", 72));
          painter->setPen(QPen(QColor(255, 0, 0, 35)));
          painter->drawText(x + 30, bottom - 82, "Left");
      } else if (!lastRightEvent && right) {
          // pierwszy prawy event
          painter->setPen(QPen(Qt::white));
          painter->setFont(QFont("Tahoma", 72));
          painter->setPen(QPen(QColor(0,255,  0, 35)));
          painter->drawText(x + 30, bottom - 82, "Right");
      }

      if (event->getContext() == C3DEventsCollection::Context::Left) {
          lastLeftEvent = event;
      } else {
          lastRightEvent = event;
      }
  }
  
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
