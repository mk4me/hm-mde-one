#include "NewChartVisualizer.h"

const int ACTIVE_WIDTH = 3;
const int NON_ACTIVE_WIDTH = 1;

QWidget* NewChartVisualizer::createWidget( std::vector<QObject*>& actions )
{
    activeSerieCombo = new QComboBox();
    activeSerieCombo->addItem(QString::fromUtf8("No active serie"));
    activeSerieCombo->setEnabled(false);
    connect(activeSerieCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(setActiveSerie(int)));

    actions.push_back(activeSerieCombo);

    QwtText txt(getName().c_str());
    qwtPlot.reset(new QwtPlot(txt, nullptr));
    qwtPlot->setAutoReplot(true);
   
    zoomer.reset(new QwtPlotZoomer(qwtPlot->canvas()));
    zoomer->setStateMachine(new QwtPickerDragRectMachine);
    zoomer->setTrackerMode(QwtPicker::ActiveOnly);
    zoomer->setRubberBand(QwtPicker::RectRubberBand);

    //qwtMarker.reset(new QwtPlotMarker());
    qwtMarker = new QwtPlotMarker();
    qwtMarker->setLabel(QString(""));
    qwtMarker->setLabelAlignment(Qt::AlignLeft | Qt::AlignBottom);
    qwtMarker->setLabelOrientation(Qt::Horizontal);
    qwtMarker->setLineStyle(QwtPlotMarker::VLine);
    qwtMarker->setLinePen(QPen(Qt::black, 0, Qt::DashDotLine));
    qwtMarker->setXValue(0);
    qwtMarker->attach(qwtPlot.get());

    grid.reset(new QwtPlotGrid);
    grid->enableXMin(false);
    grid->enableYMin(false);

    grid->setMajPen(QPen(Qt::gray, 0, Qt::DashLine));
    //grid->setMinPen(QPen(Qt::gray, 0 , Qt::DotLine));
    grid->attach(qwtPlot.get());

    qwtPlot->replot();
    return qwtPlot.get();
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

    if(series.size() == 0){
        activeSerieCombo->blockSignals(true);
        activeSerieCombo->clear();
        activeSerieCombo->addItem(name.c_str());
        activeSerieCombo->setCurrentIndex(0);
        activeSerieCombo->blockSignals(false);
    }else{
        activeSerieCombo->addItem(name.c_str());
    }

    series.push_back(ret);

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
    curve->attach(qwtPlot.get());
}

void NewChartVisualizer::removeSerie( core::IVisualizer::SerieBase *serie )
{
    //throw std::runtime_error("Feature not implemented!");
}

void NewChartVisualizer::setActiveSerie( int idx )
{
    for (int i = series.size() - 1; i >= 0; --i) {
        series[i]->setActive(idx == i);
    }
}

void NewChartVisualizer::setNormalized( bool normalized )
{

}


void NewChartVisualizer::NewChartSerie::setData( const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
    curve = new QwtPlotCurve(data->getName().c_str());
    ScalarChannelReaderInterfaceConstPtr reader = data->get();
    
    pointHelper = new PointData(reader, 500);
       
    curve->setData(pointHelper);
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    setColor(r, g, b);
    setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    visualizer->addPlotCurve(curve, getScales());
}

void NewChartVisualizer::NewChartSerie::setTime( float time )
{
    visualizer->markerX = time;
    visualizer->markerLabel = QString("y(%1) = %2").arg(time).arg(pointHelper->y(time));
}

void NewChartVisualizer::NewChartSerie::setActive( bool val )
{
    this->active = val;
    if (curve) {
        setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    }
}

