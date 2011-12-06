#include "NewChartPCH.h"
#include <QtGui/QAction>
#include <QtGui/QHBoxLayout>
#include "NewChartVisualizer.h"
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_scale_widget.h>
#include <qwt/qwt_plot_layout.h>
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
    statsTable(nullptr)
{

}

QWidget* NewChartVisualizer::createWidget( std::vector<QObject*>& actions )
{
    QwtText txt(getName().c_str());
    qwtPlot = new QwtPlot(txt, nullptr);
    qwtPlot->setAutoReplot(true);
    if (isShowLegend()) {
        QwtLegend* legend = new QwtLegend();
        legend->setItemMode(QwtLegend::ClickableItem);
        qwtPlot->insertLegend( legend, QwtPlot::RightLegend );
    }
    qwtPlot->canvas()->setFocusIndicator(QwtPlotCanvas::ItemFocusIndicator);
    qwtPlot->canvas()->installEventFilter(this);

    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    actions.push_back(activeSerieCombo);
    NewChartPickerPtr picker(new NewChartPicker(this));
    connect(picker.get(), SIGNAL(serieSelected(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));
    statesMap[new QAction("Picker", this)] = picker;
    statesMap[new QAction("Value Marker", this)] =  NewChartStatePtr(new NewChartValueMarker(this));
    statesMap[new QAction("Horizontal Marker", this)] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Horizontal));
    statesMap[new QAction("Vertical Marker", this)] =  NewChartStatePtr(new NewChartVerticals(this, NewChartLabel::Vertical));
    
    for (auto it = statesMap.begin(); it != statesMap.end(); it++) {
        actions.push_back(it->first);
        connect(it->first, SIGNAL(triggered()), this, SLOT(onStateAction()));
    }
    
    

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
    

    connect(qwtPlot, SIGNAL(legendClicked(QwtPlotItem*)), this, SLOT(onSerieSelected(QwtPlotItem*)));
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
    statsTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    qwtPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(qwtPlot);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    
    layout->addWidget(statsTable);
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

    statsTable->addEntry(QString(name.c_str()), ret->getStats());

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

    return ret;
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




