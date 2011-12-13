#include "NewChartPCH.h"
#include "NewChartSerie.h"
#include "NewChartVisualizer.h"

const int ACTIVE_WIDTH = 2;
const int NON_ACTIVE_WIDTH = 1;


void NewChartSerie::setData( const core::ObjectWrapperConstPtr & data )
{
    this->data = data;
    curve = new QwtPlotCurve(data->getName().c_str());
    reader = data->get();
    accessor.reset(new ScalarContiniousTimeAccessor(reader));
    ScalarChannelReaderInterfacePtr nonConstChannel(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
    stats.reset(new ScalarChannelStats(nonConstChannel));
    int count = reader->size();
    xvals = new double[count];
    yvals = new double[count];
    for (int i = 0; i < count; i++) {
        xvals[i] = reader->argument(i);
        yvals[i] = reader->value(i);
    }

    //ScalarChannelReaderInterfacePtr nonConstChannel2(core::const_pointer_cast<ScalarChannelReaderInterface>(reader));
    //
    //if(absChannel == nullptr) {
    //    absChannel.reset(new ScalarModifier(nonConstChannel2, ScalarChannelAbs()));
    //}
    //
    //boost::shared_ptr<ChannelNoCopyModifier<float, float>> absTest( new AbsMeanChannel<float, float>(nonConstChannel2));
    //
    //absTest->initialize();
    //
    //if (integratorChannel == nullptr) {
    //    integratorChannel.reset(new ScalarModifier(absTest, ScalarChannelIntegrator()));
    //}

    //stats.reset(new ScalarChannelStats(integratorChannel));
    //int count = integratorChannel->size();
    //xvals = new double[count];
    //yvals = new double[count];
    //for (int i = 0; i < count; i++) {
    //    xvals[i] = integratorChannel->argument(i);
    //    yvals[i] = integratorChannel->value(i);
    //}
    curve->setRawSamples(xvals, yvals, count);
    int r = rand() % 256;
    int g = rand() % 256;
    int b = rand() % 256;
    setColor(r, g, b);
    setWidth(active ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    curve->setRenderHint(QwtPlotItem::RenderAntialiased, true);
    //curve->setCurveAttribute( QwtPlotCurve::Fitted );
    visualizer->addPlotCurve(curve, getScales());
}

void NewChartSerie::setTime( float time )
{
    this->time = time;
}

void NewChartSerie::setActive( bool val )
{
    this->active = val;
    if (curve) {
        setWidth(val ? ACTIVE_WIDTH : NON_ACTIVE_WIDTH);
    }
}

NewChartSerie::~NewChartSerie()
{
    delete[] xvals;
    delete[] yvals;
}
