#include "VideoPCH.h"
#include "VideoVisualizer.h"
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <osgui/EventCallback.h>
#include <osgui/QOsgWidgets.h>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Box>
#include <osgui/AspectRatioKeeper.h>
#include <osgui/OsgWidgetUtils.h>


struct VideoVisualizer::Refresher
{
    VideoVisualizer* visualizer;
    void operator()(osg::Node* node, const osgGA::GUIEventAdapter* event) 
    {
        UTILS_ASSERT(visualizer);
        visualizer->refresh( static_cast<float>(event->getWindowWidth()), static_cast<float>(event->getWindowHeight()) );
    }
};

struct VideoVisualizer::WidgetUpdater : public osg::Drawable::UpdateCallback
{
    VideoVisualizer* visualizer;

    WidgetUpdater(VideoVisualizer* visualizer = nullptr) :
    visualizer(visualizer)
    {

    }

    virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable)
    {
        UTILS_ASSERT(visualizer);
        visualizer->updateWidget();
    }
};




VideoVisualizer::VideoVisualizer() :
name("Video"), prevStreamTime(-1), prevStreamWidth(-1), useTextureRect(true)
{

}

VideoVisualizer::~VideoVisualizer()
{
    viewer = nullptr;
}

const std::string& VideoVisualizer::getName() const
{
    return name;
}

core::IVisualizer* VideoVisualizer::create() const
{
    return new VideoVisualizer();
}

void VideoVisualizer::getSlotInfo( int source, std::string& name, core::ObjectWrapper::Types& types )
{
    if ( source == 0 ) {
        name = "video";
        types.push_back(typeid(VideoStream));
    }
}

void VideoVisualizer::refresh( float width, float height )
{
    // aktualizacja wspó³czynnika proporcji
    if ( streamImage ) {
        ratioKeeper->setAspectRatio( streamImage->getPixelAspectRatio() * streamImage->s() / streamImage->t() );
    }

    // aktualizacja rozmiaru t³a
    workspace->setOrigin(0, 0);
    workspace->resize(width, height);
    workspace->update();
    // aktualizacja tekstury t³a
    workspace->getBackground()->setTexCoordRegion(0, height, width, -height);

    // aktualizacja rozmiaru tekstury
    if ( streamImage ) {
        // odœwie¿enia danych zale¿nych od obrazka
        // resize obrazka
        streamImage->setMaxWidth(widget, widget->getWidth());
        workspace->resizeAdd();
    }
}

void VideoVisualizer::updateWidget()
{
    // korekcja wspó³rzêdnych tekstury
    if ( streamImage ) {
        if ( useTextureRect ) {
            // sprawdzamy, czy trzeba odœwie¿yæ wspó³rzêdne tekstury
            float s = static_cast<float>(streamImage->s());
            float t = static_cast<float>(streamImage->t());
            // 
            auto ll = widget->getTexCoord(osgWidget::Widget::LOWER_LEFT);
            auto ur = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
            float oldS = fabs( ll.x() - ur.x() );
            float oldT = fabs( ll.y() - ur.y() );

            // czy trzeba aktualizowaæ?
            if ( oldS != s || oldT != t ) {
                widget->setTexCoord(0, t, osgWidget::Widget::UPPER_LEFT);
                widget->setTexCoord(0, 0, osgWidget::Widget::LOWER_LEFT);
                widget->setTexCoord(s, t, osgWidget::Widget::UPPER_RIGHT);
                widget->setTexCoord(s, 0, osgWidget::Widget::LOWER_RIGHT);
                osgui::correctTexCoords(widget);
            }
        }
    }
}

void VideoVisualizer::update( double deltaTime )
{
    if ( streamImage ) {

        bool needsUpdate = false;
        double timestamp = streamImage->getTimestamp();
        int width = streamImage->s();
        
        if ( prevStreamWidth != width ) {
            prevStreamWidth = width;
            needsUpdate = true;
        }

        if ( timestamp != prevStreamTime ) {
            prevStreamTime = timestamp;
            needsUpdate = true;
        }

        if ( needsUpdate ) {
            viewer->update();
        }
    }
}

QWidget* VideoVisualizer::createWidget(std::vector<QObject*>& actions)
{
    using namespace osg;
    using namespace osgWidget;
    using namespace osgui;

    viewer = new QOsgDefaultWidget();
    viewer->setTimerActive(false);
    //widget->setMinimumSize(300, 300);

    viewer->addEventHandler( new osgViewer::StatsHandler() );
    //viewer->addEventHandler( new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()) );

    // pobranie cech kontekstu graficznego
    const GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    Camera* camera = viewer->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::PROTECTED | StateAttribute::OFF);
    camera->setProjectionMatrix(Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(Transform::ABSOLUTE_RF);
    camera->setViewMatrix(Matrix::identity());
    camera->setClearColor(Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie WMa
    WindowManager* manager = new WindowManager(viewer, float(traits->width), float(traits->height), 0xFF/*, WindowManager::WM_PICK_DEBUG*/);
    viewer->setSceneData(manager);
    viewer->addEventHandler( new osgWidget::MouseHandler(manager) );
    // viewer->addEventHandler( new osgWidget::KeyboardHandler(manager) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(manager, camera) );

    // dodanie obszaru roboczego
    workspace = new osgWidget::Box();
    workspace->getBackground()->setImage("data/resources/images/transparent_background.png", false, false);
    osg::Texture* texture = osgui::getTexture(workspace->getBackground());
    texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    manager->addChild(workspace);

    // dodanie widgetu
    widget = new Widget("video", 100, 100);
    widget->setSize(100, 100);
    widget->setUpdateCallback( new WidgetUpdater(this) );

    // ratio keeper
    ratioKeeper = new AspectRatioKeeper(widget, 1);
    workspace->addWidget(ratioKeeper);
    
    Refresher refresher = { this };
    workspace->addEventCallback(createEventCallback(osgGA::GUIEventAdapter::RESIZE, refresher));


    refresh(float(viewer->width()), float(viewer->height()));

    return viewer;
}

void VideoVisualizer::setUp( core::IObjectSource* source )
{
    streamImage = nullptr;
    

    // usuniêcie widgetu z rodzica
    //workspace->removeWidget(ratioKeeper);

    // pobranie obrazkaa
    if ( source->tryGetObject(stream, 0) ) {
        ratioKeeper->setTarget(widget);
        streamImage = stream->getImage(vidlib::PixelFormatRGB24);
        widget->setTexture( stream->getTexture(streamImage->getFormat(), useTextureRect), true, useTextureRect );
        osgui::correctTexCoords(widget, streamImage);
    } else {
        ratioKeeper->setTarget(nullptr);
    }

    refresh(viewer->width(), viewer->height());


 /*   osg::ref_ptr<const vidlib::VideoImageStream> stream = source->getObject(0);

    widgetKeeper->setAspectRatio(stream->getPixelAspectRatio() * stream->s() / stream->t());

    osgWidget::Widget* widget = widgetKeeper->getTarget();

    widget->setImage( const_cast<vidlib::VideoImageStream*>(&*stream), true, true );
    // czy trzeba zrobiæ flipa?
    osgui::correctTexCoords(widget, const_cast<vidlib::VideoImageStream*>(&*stream));
    */
}

QIcon* VideoVisualizer::createIcon()
{
    return new QIcon("data/resources/icons/video.png");
}
