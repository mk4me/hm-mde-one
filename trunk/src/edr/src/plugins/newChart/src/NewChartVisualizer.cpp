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
#include "StatsTable.h"
#include "NewChartMarker.h"
#include "NewChartValueMarker.h"
#include "NewChartVerticals.h"
#include "NewChartHelpers.h"
#include "NewChartScaleDrawer.h"
#include "NewChartLegend.h"
#include "NewChartLegendItem.h"
#include <limits>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <coreui/CoreAction.h>
#include <coreui/CoreWidgetAction.h>


NewChartVisualizer::NewChartVisualizer() :
	upperBoundCurve(nullptr),
	lowerBoundCurve(nullptr),
	averageCurve(nullptr),
	boundsAutoRefresh(true),
	movingAverageTimeWindow(0.125),
	pointsPerWindow(25),
	qwtPlot(nullptr),
    showLegend(true), 
    currentSerie(-1),
    plotPanner(nullptr),
    plotMagnifier(nullptr),
    statsTable(nullptr),
	scaleToActive(false),
	percentDraw(nullptr),
    context(C3DEventsCollection::IEvent::General),
    eventsMenu(nullptr),
    shiftSpinX(nullptr),
    shiftSpinY(nullptr),
    scaleSpinX(nullptr),
    scaleSpinY(nullptr),
	legend(nullptr),
	boundsToRefresh(false),
	qwtMarker(nullptr),
	pickerAction(nullptr),
	valueMarkerAction(nullptr),
	vMarkerAction(nullptr),
	hMarkerAction(nullptr),
	scaleAction(nullptr),
	bandsAction(nullptr),
    currentSerieTime(-1.0f),
    currentSerieValue(-1.0f)
{

}


NewChartVisualizer::~NewChartVisualizer()
{
    delete upperBoundCurve;
    delete lowerBoundCurve;
    delete averageCurve;
}


QWidget* NewChartVisualizer::createWidget()
{
    QWidget* widget = new QWidget();
    widget->setObjectName(QString::fromUtf8("newChartVisualizerWidget"));
    QwtText txt(getName().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    qwtPlot->setObjectName(QString::fromUtf8("plot"));
    percentDraw = new PercentScaleDraw(0.0, 5.0);
    qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, percentDraw);
    
    //aby legenda nie była usuwana podczas chowania i pokazywania trzeba ustawić parenta innego niż QwtPlot
    legend = new NewChartLegend(widget);
    legend->setDefaultItemMode(QwtLegendData::Checkable);
  
    qwtPlot->setAutoReplot(true);

    if (isShowLegend()) {    
        qwtPlot->insertLegend( legend, QwtPlot::BottomLegend );
    }

    qobject_cast<QwtPlotCanvas *>(qwtPlot->canvas())->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);


    eventsMenu = new QComboBox();
    eventsMenu->setToolTip(tr("Events context"));

    QIcon eventsIcon(":/newChart/icons/normalizacja1.png");
    eventsMenu->addItem(eventsIcon, tr("None events") , QVariant(C3DEventsCollection::IEvent::General));
    eventsMenu->addItem(eventsIcon, tr("Left events") , QVariant(C3DEventsCollection::IEvent::Left));
    eventsMenu->addItem(eventsIcon, tr("Right events"), QVariant(C3DEventsCollection::IEvent::Right));
	connect(eventsMenu, SIGNAL(activated(int)), this, SLOT(onEventContext(int)));

	coreUI::CoreWidgetAction * eventsAction = new coreUI::CoreWidgetAction(widget, tr("Events"), coreUI::CoreTitleBar::Right);
	eventsAction->setDefaultWidget(eventsMenu);
	widget->addAction(eventsAction);


	picker.reset(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));

    coreUI::CoreAction * pickerAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/picker.png"), tr("Picker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[pickerAction] = picker;
    connect(pickerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(pickerAction);
    

    coreUI::CoreAction * valueMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/value_tag.png"), tr("Value Marker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[valueMarkerAction] =  NewChartStatePtr(new NewChartValueMarker(this));
    connect(valueMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(valueMarkerAction);


	coreUI::CoreAction * hMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/vertical_tag.png"), tr("Horizontal Marker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[hMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Horizontal));
    connect(hMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(hMarkerAction);


	coreUI::CoreAction * vMarkerAction = new coreUI::CoreAction(tr("Tags"), QIcon(":/newChart/icons/horizontal_tag.png"), tr("Vertical Marker"), widget, coreUI::CoreTitleBar::Left);
    statesMap[vMarkerAction] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Vertical));
    connect(vMarkerAction, SIGNAL(triggered()), this, SLOT(onStateAction()));
	widget->addAction(vMarkerAction);


	coreUI::CoreAction * scaleAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/pomiary.png"), tr("Scale to active"), widget, coreUI::CoreTitleBar::Left);
    scaleAction->setCheckable(true);
    scaleAction->setChecked(scaleToActive);
    connect(scaleAction, SIGNAL(triggered(bool)), this, SLOT(scaleToActiveSerie(bool)));
	widget->addAction(scaleAction);


	QIcon iconStats;
	iconStats.addFile(QString::fromUtf8(":/newChart/icons/stat-a.png"), QSize(), QIcon::Normal, QIcon::Off);
	iconStats.addFile(QString::fromUtf8(":/newChart/icons/stat-b.png"), QSize(), QIcon::Normal, QIcon::On);
	coreUI::CoreAction * showStats = new coreUI::CoreAction(tr("Operations"), iconStats, tr("Statistics"), widget, coreUI::CoreTitleBar::Left);
    showStats->setCheckable(true);
    showStats->setChecked(false);
    connect(showStats, SIGNAL(triggered(bool)), this, SLOT(showStatistics(bool)));
	widget->addAction(showStats);


    for ( int i = 0; i < QwtPlot::axisCnt; ++i )    {
        QwtScaleWidget *scaleWidget = qwtPlot->axisWidget( i );
        if ( scaleWidget ) {
            scaleWidget->setMargin( 0 );
        }
    }
    qwtPlot->plotLayout()->setAlignCanvasToScales( true );

    plotPanner =  new QwtPlotPanner( qwtPlot->canvas() );
    plotMagnifier =  new QwtPlotMagnifier( qwtPlot->canvas() );
    

    connect(legend, SIGNAL(checked( QwtPlotItem *, bool, int)), this, SLOT(onSerieSelected(QwtPlotItem*, bool, int)));
    connect(legend, SIGNAL(checkboxChanged(const QwtPlotItem*, bool)), this, SLOT(onSerieVisible(const QwtPlotItem*, bool)));
    qwtMarker = new NewChartMarker();
    qwtMarker->setXValue(0);
    qwtMarker->setYValue(0);
    qwtMarker->attach(qwtPlot);

    grid.reset(new QwtPlotGrid);
    grid->enableXMin(false);
    grid->enableYMin(false);

    grid->setMajorPen(QPen(Qt::gray, 0, Qt::DashLine));
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

    auto shiftX = LabeledSpinbox::create(tr("X:"), 0.03, -(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)());
    auto shiftY = LabeledSpinbox::create(tr("Y:"), 0.03, -(std::numeric_limits<double>::max)(), (std::numeric_limits<double>::max)());
    connect(shiftX.second, SIGNAL(valueChanged(double)), this, SLOT(onShiftX(double)));
    connect(shiftY.second, SIGNAL(valueChanged(double)), this, SLOT(onShiftY(double)));
    shiftSpinX = shiftX.second;
    shiftSpinY = shiftY.second;

	coreUI::CoreWidgetAction * shiftSpinXAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Left);
	shiftSpinXAction->setDefaultWidget(shiftX.first);
	widget->addAction(shiftSpinXAction);

	coreUI::CoreWidgetAction * shiftSpinYAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Left);
	shiftSpinYAction->setDefaultWidget(shiftY.first);
	widget->addAction(shiftSpinYAction);


    auto scaleX = LabeledSpinbox::create(tr("SX:"), 0.01, -5.0, 5.0);
    auto scaleY = LabeledSpinbox::create(tr("SY:"), 0.01, -5.0, 5.0);
    connect(scaleX.second, SIGNAL(valueChanged(double)), this, SLOT(onScaleX(double)));
    connect(scaleY.second, SIGNAL(valueChanged(double)), this, SLOT(onScaleY(double)));
    scaleSpinX = scaleX.second;
    scaleSpinY = scaleY.second;

	coreUI::CoreWidgetAction * scaleSpinXAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Right);
	scaleSpinXAction->setDefaultWidget(scaleX.first);
	widget->addAction(scaleSpinXAction);

	coreUI::CoreWidgetAction * scaleSpinYAction = new coreUI::CoreWidgetAction(widget, tr("Active Data Series"), coreUI::CoreTitleBar::Right);
	scaleSpinYAction->setDefaultWidget(scaleY.first);
	widget->addAction(scaleSpinYAction);
    
    
	coreUI::CoreAction * bandsAction = new coreUI::CoreAction(tr("Operations"), QIcon(":/newChart/icons/charts.png"), tr("Bands"), widget, coreUI::CoreTitleBar::Left);
    bandsAction->setCheckable(true);
    bandsAction->setChecked(false);
    connect(bandsAction, SIGNAL(triggered(bool)), this, SLOT(showBands(bool)));
	widget->addAction(bandsAction);


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
   
    return widget;
}

void NewChartVisualizer::getSupportedTypes(core::TypeInfoList & supportedTypes) const
{
    supportedTypes.push_back(typeid(ScalarChannelReaderInterface));
}

plugin::IVisualizer::ISerie * NewChartVisualizer::createSerie(const utils::TypeInfo & requestedType, const core::ObjectWrapperConstPtr& data)
{
    NewChartSerie * ret = new NewChartSerie(this);
	auto name = "Serie " + boost::lexical_cast<std::string>(series.size());
    ret->setName( name);
    ret->setData(requestedType, data);
    series.push_back(ret);

    plotChanged();

    statsTable->addEntry(QString("Whole chart"), QString(name.c_str()), ret->getStats());

    NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(ret->getCurve()));
	if(legendLabel != nullptr){	
		std::string source;
		data->tryGetMeta("core/source", source);
		legendLabel->setToolTip(source.c_str());		
		if(series.size() == 1){    
			if(legendLabel->isItemActive() == false){
				legend->blockSignals(true);
				legendLabel->setItemActive(true);
				legendLabel->setItemVisibleEnabled(false);
				legend->blockSignals(false);
			}
		}
	}

    adjustOffsetStep(shiftSpinX, QwtPlot::xBottom);
    adjustOffsetStep(shiftSpinY, QwtPlot::yLeft);

    return ret;
}

plugin::IVisualizer::ISerie * NewChartVisualizer::createSerie( const plugin::IVisualizer::ISerie * serie )
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

void NewChartVisualizer::removeSerie( plugin::IVisualizer::ISerie *serie )
{
    NewChartSerie* chSerie = dynamic_cast<NewChartSerie*>(serie);

    if(chSerie == nullptr){
        throw std::runtime_error("Serie type not generated by this Visualizer!");
    }

    auto it = std::find(series.begin(), series.end(), chSerie);
    if(it == series.end()){
        throw std::runtime_error("Given serie dos not belong to this visualizer!");
    }

	(*it)->removeItemsFromPlot();
	qwtPlot->replot();

    //updateFIFO.push_back(boost::bind( &NewChartSerie::removeItemsFromPlot, *it ));
    //updateFIFO.push_back(boost::bind( &QwtPlot::replot, qwtPlot));
    series.erase(it);

    plotChanged();
}

void NewChartVisualizer::setActiveSerie(plugin::IVisualizer::ISerie * serie)
{
	if(serie == nullptr){
		setActiveSerie(-1);
	}else{
		for(int i = 0; i < series.size(); ++i){
			if(series[i] == serie){
				setActiveSerie(i);
			}
		}
	}
}

const plugin::IVisualizer::ISerie * NewChartVisualizer::getActiveSerie() const
{
	for(int i = 0; i < series.size(); ++i){
		if(series[i]->isActive() == true){
			return series[i];
		}
	}

	return nullptr;
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
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(serie->getCurve()));
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
            helper->getEventsItem()->setVisible(context != C3DEventsCollection::IEvent::General);
            //helper->getEventsItem()->attach(qwtPlot);
        }

        //zaznacz w legendzie
        NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(serie->getCurve()));
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
        idx = -1; // idx nie chcemy wykorzystywać!
        if (on == true) {
        
            for (unsigned int i = 0; i < series.size(); ++i) {
                NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget(series[i]->getCurve()));
                if (series[i]->curve == curve) {
                    // powinno wywołać sygnał, który ustawi aktywną serię
                    setActiveSerie(i);
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
            //ignorujemy to - zawsze musi być jedna seria aktywna			
            NewChartLegendItem * legendLabel = qobject_cast<NewChartLegendItem *>(legend->legendWidget((const QwtPlotCurve*)curve));
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
    //        // powinno wywołać sygnał, który ustawi aktywną serię
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
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        return series[currentSerie];
    }
    return nullptr;
}

NewChartSerie* NewChartVisualizer::tryGetCurrentSerie()
{
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        return series[currentSerie];
    }
    return nullptr;
}

void NewChartVisualizer::update( double deltaTime )
{
   /* for (auto it = updateFIFO.begin(); it != updateFIFO.end(); ++it) {
        (*it)();
    }
    updateFIFO.clear();*/
    if (currentSerie >= 0 && currentSerie < static_cast<int>(series.size())) {
        
        qwtMarker->setVisible(true);
        NewChartSerie* serie = series[currentSerie];
        if (currentSerieTime != serie->getTime() || currentSerieValue != serie->getCurrentValue()) {
            currentSerieTime = serie->getTime();
            currentSerieValue = serie->getCurrentValue();
            qwtMarker->setXValue(currentSerieTime * serie->getXScale() + serie->getXOffset());
            qwtMarker->setYValue(currentSerieValue * serie->getYScale() + serie->getYOffset());
        }
        auto helper = serie->getEventsHelper();
        if (isEventMode() && helper) {
            EventsHelper::SegmentConstPtr segment = helper->getSegment(currentSerieTime, this->context);
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

plugin::IVisualizer* NewChartVisualizer::create() const
{
    return new NewChartVisualizer();
}

QIcon* NewChartVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/newChart/icons/2D.png"));
}

int NewChartVisualizer::getMaxDataSeries( void ) const
{
    return -1;
}

void NewChartVisualizer::setEvents(NewChartSerie* serie, EventsCollectionConstPtr val )
{
    C3DEventsCollection::Context c = static_cast<C3DEventsCollection::Context>(eventsMenu->itemData(eventsMenu->currentIndex()).toInt());
    bool eventMode = (c != C3DEventsCollection::IEvent::General);
    EventsHelperPtr helper = serie->getEventsHelper();
    helper->getEventsItem()->attach(qwtPlot);
    helper->getEventsItem()->setVisible(serie == tryGetCurrentSerie() && eventMode);
    int no = 0;
    for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); ++segment) {
        statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
    }
    no = 0;
    for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); ++segment) {
        statsTable->addEntry(tr("Right"), tr("%1: Right step %2").arg(serie->getName().c_str()).arg(++no),(*segment)->stats, QColor(200, 255, 200));
    }
}

void NewChartVisualizer::onEventContext(int index)
{
    C3DEventsCollection::Context c = static_cast<C3DEventsCollection::Context>(eventsMenu->itemData(index).toInt());
    bool eventMode = (c != C3DEventsCollection::IEvent::General);
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

void NewChartVisualizer::recreateStats( ScalarChannelStatsConstPtr stats /*= ScalarChannelStatsConstPtr()*/ )
{
    statsTable->clear();
    for (auto it = series.begin(); it != series.end(); ++it) {
        statsTable->addEntry(tr("Whole chart"), (*it)->getName().c_str(), (*it)->getStats() );
    }
    if (stats) {
        QString group = context == C3DEventsCollection::IEvent::Left ? tr("Left") : tr("Right");
        QColor color  = context == C3DEventsCollection::IEvent::Left ?  QColor(255, 200, 200) : QColor(200, 255, 200);
        statsTable->addEntry(group, stats->getChannel()->getName().c_str(), stats, color);
    } else {
        for (auto it = series.begin(); it != series.end(); ++it) {
            int no = 0;
            EventsHelperPtr helper = (*it)->getEventsHelper();
            for (auto segment = helper->getLeftSegments().begin(); segment != helper->getLeftSegments().end(); ++segment) {
                statsTable->addEntry(tr("Left"), tr("%1: Left step %2").arg((*it)->getName().c_str()).arg(++no),(*segment)->stats, QColor(255, 200, 200));
            }
            no = 0;
            for (auto segment = helper->getRightSegments().begin(); segment != helper->getRightSegments().end(); ++segment) {
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
        for (unsigned int i = 0; i < series.size(); ++i) {
            if (series[i]->curve == curve) {
                if (!series[i]->isActive()) {
                    series[i]->setVisible(visible);
                    auto list = statsTable->getEntriesByChannel(series[i]->getStats()->getChannel());
                    BOOST_FOREACH(QTreeWidgetItem* item, list) {
                        item->setHidden(!visible);
                    }
                    for (auto it = statesMap.begin(); it != statesMap.end(); ++it) {
                        NewChartLabelStatePtr labelState = utils::dynamic_pointer_cast<NewChartLabelState>(it->second);
                        if (labelState) {
                            labelState->setVisible(series[i], visible);
                        }
                    }
                } else {
                    // aktywna seria nie może być niewidoczna, trzeba zapobiegac tej sytuacji wyłączając checkBoxa
                    UTILS_ASSERT(false, "Active serie has to be visible");
                }

                break;
            }
        }
    }

    recreateScales();
    if (context == C3DEventsCollection::IEvent::General || !timeInsideEvent()) {
        setScale();
    }
    legend->blockSignals(false);

    plotChanged();
}

void NewChartVisualizer::recreateScales()
{
    plotScales.clear();
    for (auto it = series.begin(); it != series.end(); ++it) {
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
    setScale(scaleToActive, context != C3DEventsCollection::IEvent::General);
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
                    for (auto it = series.begin(); it != series.end(); ++it) {
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

    float minT = (std::numeric_limits<float>::min)();
    float maxT = (std::numeric_limits<float>::max)();

    //wyznaczamy parametry dla wsteg i średnich
    //minimalny czas dla wszystkich seri danych
    //maksymalny czas dla wszystkich serii danych
    //minimalna rozdzielczość dla wszystkich kanałów
    std::vector<ScalarChannelReaderInterfaceConstPtr> channels;
    for (auto it = series.begin(); it != series.end(); ++it) {
        ScalarChannelReaderInterfaceConstPtr data;
        (*it)->getData()->tryGet(data);
        minT = (std::max)(minT, data->argument(0));
        maxT = (std::min)(maxT, data->argument(data->size() - 1));
        channels.push_back(data);
    }

    int totalWindows = (maxT - minT) / movingAverageTimeWindow;

    if(totalWindows == 0){
        return;
    }

    //obcinam max do całkowitej liczby okien
    //maxT = std::min(maxT, (float)totalWindows * (float)movingAverageTimeWindow);
    maxT = totalWindows * movingAverageTimeWindow;
    //wyliczam ilość wszystkich punktów jakie mam
    int totalPoints = totalWindows * pointsPerWindow;
    //wyliczam pojedynczy krok czasu
    float timeStep = (maxT - minT) / (float)totalPoints;
    //dzielnik dla średniej
    float size = channels.size();
    //akcesor do danych
    utils::shared_ptr<ScalarContiniousTimeAccessor> accessor(new ScalarContiniousTimeAccessor(channels.front()));

    //wyliczamy próbki czasowe
    //wyliczam średnią dla wszystkich próbek
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

    //potem liczę średnią kroczącą z danym oknem    
    //odświeżyć średnią kroczącą
    //odświeżyć wstęgi

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
    if( optInTimePeriod == (std::numeric_limits<int>::min)() )
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
            for( i=0; i < outNBElement ; ++i )
            {
                tempReal = tempBuffer2[i];
                tempReal2 = outRealMiddleBand[i];
                outRealUpperBand[i] = tempReal2 + tempReal;
                outRealLowerBand[i] = tempReal2 - tempReal;
            }
        }
        else
        {
            for( i=0; i < outNBElement ; ++i )
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
        for( i=0; i < outNBElement ; ++i )
        {
            tempReal = tempBuffer2[i];
            tempReal2 = outRealMiddleBand[i];
            outRealUpperBand[i] = tempReal2 + tempReal;
            outRealLowerBand[i] = tempReal2 - (tempReal * optInNbDevDn);
        }
    }
    else if( optInNbDevDn == 1.0 )
    {
        for( i=0; i < outNBElement ; ++i )
        {
            tempReal = tempBuffer2[i];
            tempReal2 = outRealMiddleBand[i];
            outRealLowerBand[i] = tempReal2 - tempReal;
            outRealUpperBand[i] = tempReal2 + (tempReal * optInNbDevUp);
        }
    }
    else
    {
        for( i=0; i < outNBElement ; ++i )
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
    for( outIdx = startSum; outIdx < endSum; ++outIdx)
    {
        tempReal = inReal[outIdx];
        tempReal *= tempReal;
        periodTotal2 += tempReal;
    }
    for( outIdx=0; outIdx < inMovAvgNbElement; ++outIdx, ++startSum, ++endSum )
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
    for (auto it = series.begin(); it != series.end(); ++it) {
        if ((*it)->curve == curve) {
            return true;
        }
    }

    return false;
}

void NewChartVisualizer::setLabelsVisible( bool val )
{
    for (auto serie = series.begin(); serie != series.end(); ++serie) {
        for (auto it = statesMap.begin(); it != statesMap.end(); ++it) {
            NewChartLabelStatePtr labels = utils::dynamic_pointer_cast<NewChartLabelState>(it->second);
            if (labels) {
                labels->setVisible(*serie, val && (*serie)->isVisible());
            }
        }
    }
}

boost::iterator_range<std::vector<NewChartSerie*>::const_iterator> NewChartVisualizer::getSeries() const
{
    return boost::make_iterator_range(series.cbegin(), series.cend());
}

QPixmap NewChartVisualizer::takeScreenshot() const 
{
    return QPixmap::grabWidget(qwtPlot);
}



