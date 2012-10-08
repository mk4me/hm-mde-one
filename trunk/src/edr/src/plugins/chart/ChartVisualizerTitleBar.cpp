#include "ChartPCH.h"
#include <osgui/EventCallback.h>
#include "ChartVisualizerTitleBar.h"

ChartWidget::ChartWidget()
{
    setupUi(this);

    // pobranie cech kontekstu graficznego
    const osg::GraphicsContext::Traits* traits = this->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    osg::Camera* camera = this->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::PROTECTED | osg::StateAttribute::OFF);
    camera->setProjectionMatrix(osg::Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
    camera->setViewMatrix(osg::Matrix::identity());
    camera->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie callbacak dostosowującego rozmiar kamery do rozmiaru okna
    camera->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) { 
            dynamic_cast<osg::Camera&>(*node).setProjectionMatrixAsOrtho2D(0, event->getWindowWidth(), 0, event->getWindowHeight()); 
    } 
    ));

    chart = new Chart();
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
    chart->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

    // dodanie callbacka dostosowującego rozmiar wykresu do rozmiaru okna
    chart->setEventCallback( osgui::createEventCallback( 
        osgGA::GUIEventAdapter::RESIZE,
        [](osg::Node* node, const osgGA::GUIEventAdapter* event) {
            dynamic_cast<Chart&>(*node).setLocation(0, 0, event->getWindowWidth(), event->getWindowHeight()); 
    }
    ));
}

ChartWidget::~ChartWidget()
{

}

void ChartWidget::setShowNormalized( bool normalized )
{
    if ( sender() != actionShowNormalized ) {
        actionShowNormalized->setChecked(normalized);
    } else {

    }
}

bool ChartWidget::isShowingNormalized() const
{
    return actionShowNormalized->isChecked();
}
