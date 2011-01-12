#include "CorePCH.h"
#include <osgViewer/Renderer>
#include <core/QOsgWidgets.h>
#include <core/DisableableRenderer.h>

////////////////////////////////////////////////////////////////////////////////
namespace core {
////////////////////////////////////////////////////////////////////////////////



QOsgContextWidget::QOsgContextWidget(QWidget * parent /*= 0*/, const char * name /*= 0*/, const QGLWidget * shareWidget /*= 0*/, Qt::WindowFlags f /*= 0*/) :
osgQt::GraphWidget( QGLFormat::defaultFormat(), parent, shareWidget, f )
{
    this->setWindowTitle(name);
}

//! 
osg::ref_ptr<osg::Camera> QOsgContextWidget::createCamera()
{
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = createTraits();
    osg::ref_ptr<osg::Camera> camera = createCamera(traits);
    return camera;
}

//! Tworzy kamerê dla zadanego okna.
osg::ref_ptr<osg::Camera> QOsgContextWidget::createCamera( osg::GraphicsContext::Traits* traits )
{
    osg::GraphicsContext::Traits* _traits = traits;
    // kod zaczerpniety z osgQt/GraphicsWindowQt.cpp
    QGLFormat format( QGLFormat::defaultFormat() );
    format.setAlphaBufferSize( _traits->alpha );
    format.setRedBufferSize( _traits->red );
    format.setGreenBufferSize( _traits->green );
    format.setBlueBufferSize( _traits->blue );
    format.setDepthBufferSize( _traits->depth );
    format.setStencilBufferSize( _traits->stencil );
    format.setSampleBuffers( _traits->sampleBuffers );
    format.setSamples( _traits->samples );
    format.setAlpha( _traits->alpha>0 );
    format.setDepth( _traits->depth>0 );
    format.setStencil( _traits->stencil>0 );
    format.setDoubleBuffer( _traits->doubleBuffer );
    format.setSwapInterval( _traits->vsync ? 1 : 0 );
    // ustawienie formatu
    setFormat(format);

    std::string name = windowTitle().toStdString();
    // kod zaczerpniêty z osgViewerQtContext.cpp
    osg::ref_ptr<osg::Camera> camera = new osg::Camera();
    // to tutaj dzieje siê magia!
    camera->setGraphicsContext( new osgQt::GraphicsWindowQt(traits) );
    camera->setName(name);
    camera->setClearColor( osg::Vec4(0.2, 0.2, 0.6, 1.0) );
    camera->setViewport( new osg::Viewport(0, 0, traits->width, traits->height) );
    camera->setProjectionMatrixAsPerspective(
        45.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0f, 10000.0f );
    return camera;
}

//! Tworzy cechy kontekstu na podstawie bie¿¹cego okna.
osg::ref_ptr<osg::GraphicsContext::Traits> QOsgContextWidget::createTraits()
{
    osg::DisplaySettings* ds = osg::DisplaySettings::instance().get();
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;
    traits->windowName = windowTitle().toStdString();
    traits->windowDecoration = false;
    traits->x = x();
    traits->y = y();
    traits->width = width();
    traits->height = height();
    traits->doubleBuffer = true;
    traits->alpha = ds->getMinimumNumAlphaBits();
    traits->stencil = ds->getMinimumNumStencilBits();
    traits->sampleBuffers = ds->getMultiSamples();
    traits->samples = ds->getNumMultiSamples();
    traits->inheritedWindowData = new osgQt::GraphicsWindowQt::WindowData( this );
    return traits;
}

QOsgContextWidget::~QOsgContextWidget()
{

}

QOsgView::QOsgView( QWidget * parent /*= 0*/, const char * name /*= 0*/, const QGLWidget * shareWidget /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
QOsgContextWidget(parent, name, shareWidget, f)
{
    init();
}

void QOsgView::init()
{
    setCamera( createCamera() );
}

//! \param camera
osg::GraphicsOperation* QOsgView::createRenderer(osg::Camera* camera)
{
    DisableableRenderer* render = new DisableableRenderer(camera);
    camera->setStats(new osg::Stats("Camera"));
    return render;
}

bool QOsgView::isRenderingEnabled() const
{
    if ( const osg::Camera* camera = getCamera() ) {
        if ( const DisableableRenderer* renderer = dynamic_cast<const DisableableRenderer*>(camera->getRenderer()) ){
            return renderer->isRenderingEnabled();
        }
    }
    return true;
}

void QOsgView::setRenderingEnabled( bool renderingEnabled )
{
    if ( osg::Camera* camera = getCamera() ) {
        if ( DisableableRenderer* renderer = dynamic_cast<DisableableRenderer*>(camera->getRenderer()) ){
            renderer->setRenderingEnabled(renderingEnabled);
        }
    }
}

QOsgViewer::QOsgViewer( QWidget * parent /*= 0*/, const char * name /*= 0*/, const QGLWidget * shareWidget /*= 0*/, Qt::WindowFlags f /*= 0*/ ) :
QOsgContextWidget(parent, name, shareWidget, f), skipFramesIfInvisible(true)
{
    setThreadingModel(osgViewer::Viewer::SingleThreaded);
    init();
}

void QOsgViewer::init()
{
    setCamera( createCamera() );
    frameTimer.setInterval(defaultInterval);
    connect(&frameTimer, SIGNAL(timeout()), this, SLOT(updateGL()));
    frameTimer.start();
}

void QOsgViewer::paintGL()
{
    // rysujemy tylko gdy event pochodzi z timera
    if ( isTimerActive() ) {
        if ( !skipFramesIfInvisible || ( isVisible() && !isHidden() )  ) {
            frame();
        }  else {
            int debuggable = 0;
        }
    } else {
        int debuggable = 0;
    }
}

//! \param camera
osg::GraphicsOperation* QOsgViewer::createRenderer(osg::Camera* camera)
{
    DisableableRenderer* render = new DisableableRenderer(camera);
    camera->setStats(new osg::Stats("Camera"));
    return render;
}

bool QOsgViewer::isRenderingEnabled() const
{
    if ( const osg::Camera* camera = getCamera() ) {
        if ( const DisableableRenderer* renderer = dynamic_cast<const DisableableRenderer*>(camera->getRenderer()) ){
            return renderer->isRenderingEnabled();
        }
    }
    return false;
}

void QOsgViewer::setRenderingEnabled( bool renderingEnabled )
{
    if ( osg::Camera* camera = getCamera() ) {
        if ( DisableableRenderer* renderer = dynamic_cast<DisableableRenderer*>(camera->getRenderer()) ){
            renderer->setRenderingEnabled(renderingEnabled);
        }
    }
}

QOsgViewer::~QOsgViewer()
{
    setCamera(NULL);
}   
////////////////////////////////////////////////////////////////////////////////
} // namespace core
////////////////////////////////////////////////////////////////////////////////