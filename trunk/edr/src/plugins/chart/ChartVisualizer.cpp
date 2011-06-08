#include "ChartPCH.h"
#include <osgui/EventCallback.h>
#include "ChartVisualizer.h"
#include <QtGui/QIcon>
#include <QtGui/QMenu>
#include <osg/LineWidth>
#include <osg/BlendFunc>
#include "ui_ChartVisualizerTitleBar.h"


ChartVisualizer::ChartVisualizer() : 
name("Chart"), prevActiveSerie(-1), prevTime(0)
{

}

ChartVisualizer::~ChartVisualizer()
{

}

const std::string& ChartVisualizer::getName() const
{
    return name;
}

core::IVisualizer* ChartVisualizer::createClone() const
{
    return new ChartVisualizer();
}

//void ChartVisualizer::getInputInfo( int inputNo, std::string& name, core::ObjectWrapper::Types& type )
void ChartVisualizer::getInputInfo(std::vector<core::IInputDescription::InputInfo>& info)
{
    core::IInputDescription::InputInfo input;

    input.type = typeid(core::ScalarChannel);
    input.name = input.type.name();
    input.modify = false;
    input.required = false;

    info.push_back(input);

    //info.name = std::string("serie ") + boost::lexical_cast<std::string>(inputNo);
    //info.type.push_back( typeid(core::EMGChannel) );
    //info.type.push_back( typeid(core::GRFChannel) );
}

void ChartVisualizer::update( double deltaTime )
{
    bool needsUpdate = false;
    if ( chart->getActiveSerieIndex() != prevActiveSerie ) {
        prevActiveSerie = chart->getActiveSerieIndex();
        chart->refreshAll();
        needsUpdate = true;   
    }
    if ( chart->getActiveSerie() && chart->getActiveSerie()->getTime() != prevTime ) {
        prevTime = chart->getActiveSerie()->getTime();
        chart->refreshCursor();
        needsUpdate = true;
    }
    
    if ( needsUpdate ) {
        //dynamic_cast<QWidget*>(viewer.get())->update();
        viewer->update();
    }
}

QWidget* ChartVisualizer::createWidget(std::vector<QObject*>& actions)
{
    //osgui::QOsgDefaultWidget* widget = new osgui::QOsgDefaultWidget();
    viewer = new osgui::QOsgDefaultWidget();
    //widget->setTimerActive(false);
    viewer->setTimerActive(false);
    //widget->setMinimumSize(300, 300);

    //viewer = widget;
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    //viewer->addEventHandler( new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()) );

    // pobranie cech kontekstu graficznego
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    osg::Camera* camera = viewer->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie callbacak dostosowuj¹cego rozmiar kamery do rozmiaru okna
    camera->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) { 
            dynamic_cast<osg::Camera&>(*node).setProjectionMatrixAsOrtho2D(0, event->getWindowWidth(), 0, event->getWindowHeight()); 
    } 
    ));

    chart = new Chart();
    chart->setAutoRefresh(false);
    chart->setLocation(0, 0, traits->width, traits->height);

    chart->setGridColor(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
    chart->setShowGridX(true);
    chart->setShowGridY(true);
    chart->setGridDensity(5);
    chart->setMargin(10);
    chart->setNormalized(false);

    chart->setZ(-0.5f);
    chart->setZRange(0.5f);


    //chart->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //chart->getOrCreateStateSet()->setAttribute(new osg::LineWidth(10), osg::StateAttribute::ON);
    chart->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    // dodanie callbacka dostosowuj¹cego rozmiar wykresu do rozmiaru okna
    chart->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) {
            dynamic_cast<Chart&>(*node).setLocation(0, 0, event->getWindowWidth(), event->getWindowHeight()); 
    }
    ));

    chart->setAutoRefresh(true);
    viewer->setSceneData(chart);


    // przewidujemy kolory na 10 wykresów...
    seriesColors.resize(10);
    seriesColors[0] = osg::Vec4(1, 0, 0, 1);
    seriesColors[1] = osg::Vec4(0, 1, 0, 1);
    seriesColors[2] = osg::Vec4(0, 0, 1, 1);
    seriesColors[3] = osg::Vec4(1, 1, 0, 1);
    seriesColors[4] = osg::Vec4(0, 1, 1, 1);
    seriesColors[5] = osg::Vec4(1, 0, 1, 1);
    seriesColors[6] = osg::Vec4(1, 0.5, 0.5, 1);
    seriesColors[7] = osg::Vec4(0.5, 1, 0.5, 1);
    seriesColors[8] = osg::Vec4(0.5, 0.5, 1, 1);
    seriesColors[9] = osg::Vec4(1, 1, 0.5, 1);


    // dodajemy akcje
    //actionNormalized = new QAction("normalize", widget);
    actionNormalized = new QAction("normalize", viewer);
    actionNormalized->setCheckable(true);
    actionNormalized->setChecked( chart->isNormalized() );
    connect(actionNormalized, SIGNAL(triggered(bool)), this, SLOT(setNormalized(bool)));
    actions.push_back(actionNormalized);

    /*QMenu* testMenu = new QMenu("testMenu", widget);
    QActionGroup* group = new QActionGroup(widget);*/

    /*QMenu* testMenu = new QMenu("testMenu", viewer);
    QActionGroup* group = new QActionGroup(viewer);

    QAction* act = testMenu->addAction("action");
    act->setCheckable(true);
    group->addAction(act);

    act = testMenu->addAction("action2");
    act->setCheckable(true);
    group->addAction(act);

    actions.push_back(testMenu);*/

    //return widget;
    return viewer;
}

void ChartVisualizer::setUp( core::IObjectSource* source )
{
    using namespace core;

    core::ScalarChannelConstPtr channel;

    series.clear();
    //series.resize(source->getNumSources());

    chart->removeAllChannels();
    //for ( int i = 0; i < source->getNumSources(); ++i ) {
    //    if ( source->tryGetObject(channel, i) ) {
    //        // okreœlenie koloru wykresu
    //        osg::Vec4 color;
    //        if ( i < static_cast<int>(seriesColors.size()) ) {
    //            // kolor 
    //            color = seriesColors[i];
    //        } else {
    //            // losowy kolor
    //            LOG_WARNING("No color defined for chart " << i << ", using random one.");
    //            color.r() = (rand()%1000)/999.0f;
    //            color.g() = (rand()%1000)/999.0f;
    //            color.b() = (rand()%1000)/999.0f;
    //            color.a() = 1.0f;
    //        }

    //        LineChartSeriePtr lineChart = new LineChartSerie();
    //        lineChart->setData(channel);
    //        lineChart->setColor(color);
    //        lineChart->setName(channel->getName());
    //        series[i] = lineChart;

    //        chart->addChannel(lineChart);
    //    }
    //}

    //chart->refreshAll();
}

QIcon* ChartVisualizer::createIcon()
{
    return new QIcon( core::getResourceString("icons/charts.png") );
}

int ChartVisualizer::getMaxDataSeries() const
{
    return -1;
}

core::IVisualizer::SerieBase* ChartVisualizer::createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name)
{
    //generuj kolor dla serii
    osg::Vec4 color;
    color.r() = (rand()%1000)/999.0f;
    color.g() = (rand()%1000)/999.0f;
    color.b() = (rand()%1000)/999.0f;
    color.a() = 1.0f;    

    core::LineChartSeriePtr lineChart = new core::LineChartSerie();
    core::ScalarChannelConstPtr d = data->get();
    lineChart->setData(d);
    lineChart->setColor(color);
    chart->addChannel(lineChart);

    ChartVisualizerSerie* serie = new ChartVisualizerSerie(lineChart);
    series.insert(serie);
    return serie;
}

void ChartVisualizer::removeSerie(core::IVisualizer::SerieBase* serie)
{
    ChartVisualizerSerie* chSerie = dynamic_cast<ChartVisualizerSerie*>(serie);

    if(chSerie == nullptr){
        throw std::runtime_error("Serie type not generated by this Visualizer!");
    }

    auto it = series.find(chSerie);
    if(it == series.end()){
        throw std::runtime_error("Given serie dos not belong to this visualizer!");
    }

    chart->removeChannel(chSerie->getSerie());

    series.erase(it);
}

void ChartVisualizer::setNormalized( bool normalized )
{
    if ( sender() != actionNormalized ) {
        actionNormalized->setChecked(normalized);
    } else {
        chart->setNormalized(normalized);
        //dynamic_cast<QWidget*>(viewer.get())->update();
        viewer->update();
    }
}

void ChartVisualizer::setActiveSerie( int idx )
{

}

//void ChartVisualizer::process( core::IObjectSource* source )
//{
//    for ( int i = 0; i < source->getNumObjects(); ++i ) {
//        if ( source->isChanged(i) ) {
//            needsRefresh = true;
//            break;
//        }
//    }
//}