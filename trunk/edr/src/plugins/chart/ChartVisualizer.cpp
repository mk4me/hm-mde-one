#include "ChartPCH.h"
#include <osgui/EventCallback.h>
#include "ChartVisualizer.h"
#include <QtGui/QIcon>


ChartVisualizer::ChartVisualizer() : 
name("Chart")
{

}

ChartVisualizer::~ChartVisualizer()
{

}

const std::string& ChartVisualizer::getName() const
{
    return name;
}

core::IVisualizer* ChartVisualizer::create() const
{
    return new ChartVisualizer();
}

void ChartVisualizer::getSlotInfo( int source, std::string& name, core::ObjectWrapper::Types& types )
{
    name = std::string("serie ") + boost::lexical_cast<std::string>(source);
    types.push_back( typeid(core::EMGChannel) );
    types.push_back( typeid(core::GRFChannel) );
}

void ChartVisualizer::update( double deltaTime )
{

}

QWidget* ChartVisualizer::createWidget()
{
    osgui::QOsgDefaultWidget* widget = new osgui::QOsgDefaultWidget();
    widget->setTimerActive(false);
    //widget->setMinimumSize(300, 300);

    viewer = widget;
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

    chart = new Chart(0, 0, traits->width, traits->height);
    chart->setLocation(0, 0, traits->width, traits->height);

    chart->setGridColor(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
    chart->setShowGridX(true);
    chart->setShowGridY(true);
    chart->setGridDensity(5);
    chart->setMargin(10);

    chart->setZ(-0.5f);
    chart->setZRange(0.5f);


    //chart->getOrCreateStateSet()->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    //chart->getOrCreateStateSet()->setAttribute(new osg::LineWidth(10), osg::StateAttribute::ON);
    //chart->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    // dodanie callbacka dostosowuj¹cego rozmiar wykresu do rozmiaru okna
    chart->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) {
            dynamic_cast<Chart&>(*node).setLocation(0, 0, event->getWindowWidth(), event->getWindowHeight()); 
    }
    ));


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

    return widget;
}

void ChartVisualizer::setUp( core::IObjectSource* source )
{
    using namespace core;

    core::ScalarChannelConstPtr channel;

    chart->removeAllChannels();
    for ( int i = 0; i < source->getNumObjects(); ++i ) {
        if ( source->tryGetObject(channel, i) ) {
            // okreœlenie koloru wykresu
            osg::Vec4 color;
            if ( i < static_cast<int>(seriesColors.size()) ) {
                // kolor 
                color = seriesColors[i];
            } else {
                // losowy kolor
                LOG_WARNING("No color defined for chart " << i << ", using random one.");
                color.r() = (rand()%1000)/999.0f;
                color.g() = (rand()%1000)/999.0f;
                color.b() = (rand()%1000)/999.0f;
                color.a() = 1.0f;
            }
            chart->addChannel(channel, color);
        }
    }

    chart->refresh();
}

QIcon* ChartVisualizer::createIcon()
{
    return new QIcon("data/resources/icons/charts.png");
}