#include "NewChartPCH.h"
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


NewChartVisualizer::NewChartVisualizer() : 
    showLegend(true), 
    currentSerie(-1),
    plotPanner(nullptr),
    plotMagnifier(nullptr),
    statsTable(nullptr),
    eventsItem(nullptr),
    eventsVisible(true),
    context(C3DEventsCollection::Context::General),
    eventsContextWidget(nullptr),
    eventsMenu(nullptr),
    scaleToActive(false),
    percentDraw(nullptr),
    shiftSpinX(nullptr),
    shiftSpinY(nullptr),
    scaleSpinX(nullptr),
    scaleSpinY(nullptr)
{

}


NewChartVisualizer::~NewChartVisualizer()
{
    //TODO
    //przejrzec niszczenie dynamicznie tworzonych zasobów
    if(eventsContextWidget != nullptr){
        delete eventsContextWidget;
    }
}


QWidget* NewChartVisualizer::createWidget( core::IActionsGroupManager * manager )
{
    QWidget* widget = new QWidget();
    widget->setObjectName(QString::fromUtf8("newChartVisualizerWidget"));
    QwtText txt(getName().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    qwtPlot->setObjectName(QString::fromUtf8("plot"));
    percentDraw = new PercentScaleDraw(0.0, 5.0);
    qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, percentDraw);
    
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
    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    eventsContextWidget = new QWidget();
    QVBoxLayout* eventsLayout = new QVBoxLayout();
    eventsLayout->setMargin(0);
    eventsLayout->setContentsMargins(0, 0, 0, 0);

    //QLabel* eventsLabel = new QLabel("Events Context", eventsContextWidget);
    //eventsLabel->setTextFormat(Qt::RichText);
    //eventsLabel->setText("<img src=\":/resources/icons/normalizacja1.png\"> Events Context");
    //eventsLayout->addWidget(eventsLabel);
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

    QAction* scaleAction = new QAction("Scale to active", this);
    scaleAction->setCheckable(true);
    scaleAction->setChecked(scaleToActive);
    connect(scaleAction, SIGNAL(triggered(bool)), this, SLOT(scaleToActiveSerie(bool)));


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
    //statsTable->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    qwtPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(qwtPlot);
    layout->setMargin(0);
    layout->addWidget(statsTable);
    statsTable->setVisible(false);
    widget->setLayout(layout);

    auto shiftX = LabeledSpinbox::create(widget, "X:", 0.03, -1000.0, 1000.0);
    auto shiftY = LabeledSpinbox::create(widget, "Y:", 0.03, -1000.0, 1000.0);
    connect(shiftX.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onShiftX(double)));
    connect(shiftY.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onShiftY(double)));
    shiftSpinX = shiftX.get<2>();
    shiftSpinY = shiftY.get<2>();

    auto scaleX = LabeledSpinbox::create(widget, "SX:", 0.01, -5.0, 5.0);
    auto scaleY = LabeledSpinbox::create(widget, "SY:", 0.01, -5.0, 5.0);
    connect(scaleX.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onScaleX(double)));
    connect(scaleY.get<2>(), SIGNAL(valueChanged(double)), this, SLOT(onScaleY(double)));
    scaleSpinX = scaleX.get<2>();
    scaleSpinY = scaleY.get<2>();
 
    //QWidget* spinWidgetX = new QWidget(widget);
    //QWidget* spinWidgetY = new QWidget(widget);
    //spinWidgetX->setLayout(new QHBoxLayout());
    //spinWidgetY->setLayout(new QHBoxLayout());
    //spinWidgetX->layout()->addWidget(new QLabel("X:"));
    //spinWidgetY->layout()->addWidget(new QLabel("Y:"));
    //spinX = new QDoubleSpinBox(widget);
    //spinY = new QDoubleSpinBox(widget);
    //connect(spinX, SIGNAL(valueChanged(double)), this, SLOT(shiftX(double)));
    //connect(spinY, SIGNAL(valueChanged(double)), this, SLOT(shiftY(double)));
    //spinX->setMaximum(1000.0);
    //spinY->setMaximum(1000.0);
    //spinX->setMinimum(-1000.0);
    //spinY->setMinimum(-1000.0);
    //spinX->setSingleStep(0.03);
    //spinY->setSingleStep(0.03);
    //spinWidgetX->layout()->addWidget(spinX);
    //spinWidgetY->layout()->addWidget(spinY);
    //spinWidgetX->layout()->setMargin(0);
    //spinWidgetY->layout()->setMargin(0);
    //spinWidgetX->layout()->setContentsMargins(0, 0, 0, 0);

    core::IActionsGroupManager::GroupID id = manager->createGroup("Operations");
    manager->addGroupAction(id, activeSerieCombo);
    manager->addGroupAction(id, pickerAction);
    manager->addGroupAction(id, showStats);
    manager->addGroupAction(id, scaleAction);

    id = manager->createGroup("Events");
    manager->addGroupAction(id, eventsContextWidget);

    id = manager->createGroup("Tags");
    manager->addGroupAction(id, valueMarkerAction);
    manager->addGroupAction(id, hMarkerAction);
    manager->addGroupAction(id, vMarkerAction);

    id = manager->createGroup("Active Serie");
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

        refreshSpinBoxes();


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
    if (on == true) {
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
        qwtMarker->setXValue(x * serie->getXScale() + serie->getXOffset());
        qwtMarker->setYValue(y * serie->getYScale() + serie->getYOffset());
        qwtMarker->setLabel(QString("Time: %1, Value: %2").arg(x).arg(y));

        //static EventsHelper::SegmentConstPtr oldSegment = EventsHelper::SegmentConstPtr();
        if (eventsVisible && context != C3DEventsCollection::Context::General) {
            setScale(scaleToActive, true);
            auto helper = eventsHelpers.find(serie);
            if (helper != eventsHelpers.end()) {
                EventsHelperPtr h = helper->second;
                EventsHelper::SegmentConstPtr segment = h->getSegment(x, this->context);
                if (segment != oldSegment) {
                    recreateStats(segment ? segment->stats : ScalarChannelStatsConstPtr());
                    setScale(this->scaleToActive, segment ? true : false);
                    oldSegment = segment;
                }
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
    // jesli nie istnieje jeszcze wizualizacja eventow, to tworzymy obiekt eventow 
    // i dolaczamy go do wykresu.
    if (!eventsItem) {
        eventsItem = new EventsPlotItem(val);
        eventsItem->setVisible(context != C3DEventsCollection::Context::General);
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

void NewChartVisualizer::onEventContext(int index)
{
    C3DEventsCollection::Context c = static_cast<C3DEventsCollection::Context>(eventsMenu->itemData(index).toInt());
    bool eventMode = (c != C3DEventsCollection::Context::General);
    eventsItem->setVisible(eventMode);
    context = c;
    setScale(scaleToActive, eventMode);
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
    if (context == C3DEventsCollection::Context::General || !timeInsideEvent()) {
        setScale(this->scaleToActive, false);
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
        auto helper = eventsHelpers.find(serie);
        if (helper != eventsHelpers.end()) {
            EventsHelperPtr h = helper->second;
            EventsHelper::SegmentConstPtr segment = h->getSegment(x, this->context);
            if (segment) {
                if (scaleToActive) {
                    //qwtPlot->setAxisScale(QwtPlot::xBottom, segment->begin, segment->end);
                    percentDraw->setPercentMode(true);
                    percentDraw->setLeftRightValues(segment->begin, segment->end);
                    qwtPlot->setAxisScale(QwtPlot::yLeft, segment->stats->minValue(), segment->stats->maxValue());
                    qwtPlot->setAxisScaleDiv(QwtPlot::xBottom, percentDraw->getScaleDiv());
                } else {
                    float minY = segment->stats->minValue();
                    float maxY = segment->stats->maxValue();
                    for (auto it = series.begin(); it != series.end(); it++) {
                        if ((*it)->isVisible()) {
                            auto h = eventsHelpers.find(*it);
                            if (h != eventsHelpers.end()) {
                                EventsHelper::SegmentConstPtr s = h->second->getSegment(x, this->context);
                                if (s) {
                                    minY = (std::min)(minY, s->stats->minValue());
                                    maxY = (std::max)(maxY, s->stats->maxValue());
                                }
                            }
                        }
                    }
                    //qwtPlot->setAxisScale(QwtPlot::xBottom, segment->begin, segment->end);
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
}

void NewChartVisualizer::setGlobalScales(bool scaleToActive)
{
    UTILS_ASSERT(percentDraw);
    percentDraw->setPercentMode(false);
    if (scaleToActive) {
        Scales s = series[currentSerie]->getScales();
        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
        qwtPlot->setAxisScale(QwtPlot::yLeft, s.getYMin(), s.getYMax());
    } else {
        qwtPlot->setAxisScale(QwtPlot::xBottom, plotScales.getXMin(), plotScales.getXMax());
        qwtPlot->setAxisScale(QwtPlot::yLeft, plotScales.getYMin(), plotScales.getYMax());
    }
}

void NewChartVisualizer::onShiftX( double d )
{
    NewChartSerie* s = series[currentSerie];
    s->setXOffset(d);
    qwtPlot->replot();
}

void NewChartVisualizer::onShiftY( double d )
{
    NewChartSerie* s = series[currentSerie];
    s->setYOffset(d);
    qwtPlot->replot();
}

void NewChartVisualizer::onScaleX( double d )
{
    if (d != 0.0) {
        NewChartSerie* s = series[currentSerie];
        s->setXScale(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::onScaleY( double d )
{
    if (d != 0.0) {
        NewChartSerie* s = series[currentSerie];
        s->setYScale(d);
        qwtPlot->replot();
    }
}

void NewChartVisualizer::refreshSpinBoxes()
{
    shiftSpinX->blockSignals(true);
    shiftSpinX->setValue(series[currentSerie]->getXOffset());
    shiftSpinX->blockSignals(false);

    shiftSpinY->blockSignals(true);
    shiftSpinY->setValue(series[currentSerie]->getYOffset());
    shiftSpinY->blockSignals(false);

    scaleSpinX->blockSignals(true);
    scaleSpinX->setValue(series[currentSerie]->getXScale());
    scaleSpinX->blockSignals(false);
     
    scaleSpinY->blockSignals(true);
    scaleSpinY->setValue(series[currentSerie]->getYScale());
    scaleSpinY->blockSignals(false);
}




