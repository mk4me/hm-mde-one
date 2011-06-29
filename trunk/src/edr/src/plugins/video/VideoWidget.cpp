#include "VideoPCH.h"
#include "VideoWidget.h"

#include <boost/foreach.hpp>
#include <osg/GraphicsContext>

#include <utils/ObserverPattern.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/PtrWrapper.h>

#include <vidlib/osg/VideoImageStreamSizeOptimizer.h>
#include "StreamOsgWidget.h"

#include <vidlib/osg/VideoImageStream.h>
#include <utils/Profiler.h>
#include "VideoWidget.h"
#include <core/MultiView.h>
#include <osgui/AspectRatioKeeper.h>
#include <core/MultiViewWidgetItem.h>
#include <core/OsgSceneDump.h>
#include <core/ILog.h>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <osgui/OsgWidgetUtils.h>


using namespace vidlib;
using namespace video;

#ifdef _DEBUG
#define WM_FLAGS 0//osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif


class OsgWidgetWindowItem : public vidlib::VideoImageStreamSizeOptimizer::Client
{
    //! Okno wzgl�dem kt�rego sprawdza� widoczno��.
    osg::observer_ptr<osgWidget::Window> window;
    //! Widget dla kt�rego dostosowywa� wsp�rz�dne tekstury.
    osg::observer_ptr<osgWidget::Widget> widget;

public:
    OsgWidgetWindowItem(osgWidget::Window* window, osgWidget::Widget* widget) :
      window(window), widget(widget)
      {}

public:

    //! 
    virtual bool isValid()
    {
        return window.valid() && widget.valid();
    }

    //! \return Czy 
    virtual osg::Vec2 getDesiredImageSize()
    {
        if ( window->isVisible() ) {
            return widget->getSize();
        } else {
            return osg::Vec2(0, 0);
        }
    }

    //! \param optimizer
    //! \param prevS
    //! \param prevT
    virtual void onImageResized(vidlib::VideoImageStreamSizeOptimizer* optimizer, int prevS, int prevT)
    {
        osg::Image* image = optimizer->getImage();
        if ( osg::StateSet* state = widget->getStateSet() ) {
            if ( osg::TextureRectangle* rect = dynamic_cast<osg::TextureRectangle*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE)) ) {
                // przeskalowanie wsp�rz�dnych tekstury
                for (unsigned i = 0; i < 4; ++i) {
                    osgWidget::TexCoord coord = widget->getTexCoord(static_cast<osgWidget::Widget::Corner>(i));
                    coord.x() *= image->s()/static_cast<double>(prevS);
                    coord.y() *= image->t()/static_cast<double>(prevT);
                    widget->setTexCoord( coord, static_cast<osgWidget::Widget::Corner>(i) );
                }
            }
        }
    }
};

VideoWidget::VideoWidget() :
format(PixelFormatBGRA)
{
    // inicjalizacja UI
    setupUi(this); 

    // dodanie grupy akcji (designer na to nie pozwala :()
    // TIP: w przyk�adach Qt nie ma jawnego zwalniania akcji
    QActionGroup* formatGroup = new QActionGroup(this);
    formatGroup->addAction( actionFormatYUV );
    formatGroup->addAction( actionFormatRGB );
    formatGroup->addAction( actionFormatBGRA );

    // tworzenie viewera
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // tworzymy multi widok
    multiView = new core::MultiView(viewer, traits->width, traits->height, 0xF0000000, WM_FLAGS);
    multiView->addChild(new osgWidget::Box("HACK"));

    // stworzenie kamery
    osg::Camera* multiViewCamera = multiView->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    viewer->addEventHandler( new core::OsgSceneDumpFileHandler( "dump_VideoService.txt") );
    viewer->addEventHandler( new osgWidget::MouseHandler(multiView) );
    viewer->addEventHandler( new osgWidget::KeyboardHandler(multiView) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    // ustawienie sceny
    viewer->setSceneData(multiViewCamera);
}

void VideoWidget::init( std::vector<std::string> &files )
{
    // odczytanie plik�w
    images.clear();
    BOOST_FOREACH(const std::string& file, files) {
        if ( osg::Image* image = osgDB::readImageFile(file) ) {
            images.push_back(image);
            if ( VideoImageStream* stream = dynamic_cast<VideoImageStream*>(image) ) {
                stream->setTargetFormat(format);
            }
        } else {
            LOG_ERROR("VideoService: "<<file<<" could not be read.");
        }
    }
   
    createScene();
}

void VideoWidget::init( std::vector<osg::ref_ptr<vidlib::VideoImageStream> > &streams )
{
    images.clear();
    BOOST_FOREACH(vidlib::VideoImageStream* stream, streams) {
        images.push_back( stream );
    }
    createScene();
}

void VideoWidget::setPixelFormat( PixelFormat format )
{
    // ustawienie formatu strumieni
    BOOST_FOREACH( osg::Image* image, images ) {
        if ( VideoImageStream* stream = dynamic_cast<VideoImageStream*>(image) ) {
            stream->setTargetFormat(format);
        }
    }
    this->format = format;
    createScene();
}

void VideoWidget::setUseTextureRect( bool useTextureRect )
{
    this->useTextureRect = useTextureRect;
    createScene();
}

void VideoWidget::createScene()
{
    unsigned sel = multiView->getSelectedIndex();
    bool doSelect = ( sel != multiView->getNumItems() );

    // na wszelki wypadek czy�cimy scen�
    clearScene();

    // stworzenie widget�w i dodanie ich do multi widoku
    float avgRatio = 0;
    BOOST_FOREACH(osg::Image* image, images) {
        // wyliczenie wsp�czynnika proporcji oraz aktualizacja zbiorczego wsp�czynnika
        float ratio = image->getPixelAspectRatio() * image->s() / image->t();
        avgRatio += ratio;

        VideoImageStreamSizeOptimizer* optimizer = new VideoImageStreamSizeOptimizer(new osg::Uniform(yuvImageSizeName.c_str(), osg::Vec2(0, 0)));
        
        // faktyczne dodanie miniaturki do grida
        osgWidget::Widget* streamWidget = createStreamWidget( image, optimizer );
        osgWidget::Box* thumbnail = new osgWidget::Box(streamWidget->getName());
        streamWidget->setCanFill(true);
        thumbnail->addWidget(streamWidget);
        thumbnail->setStrata( osgWidget::Window::STRATA_BACKGROUND );
        multiView->addChild(thumbnail);

        // faktyczne dodanie du�ego okna
        osgWidget::Widget* streamWidgetClone = createStreamWidget( /*osg::clone*/(image), optimizer);
        osgWidget::Box* preview = new osgWidget::Box(std::string(streamWidgetClone->getName()) + "Preview");        
        osgui::AspectRatioKeeper* keeper = new osgui::AspectRatioKeeper(streamWidgetClone, ratio);
        keeper->setColor(0, 0, 0, 0);
        preview->getBackground()->setColor(0,0,0,0);
        preview->addWidget(keeper);
        preview->setStrata( osgWidget::Window::STRATA_BACKGROUND );
        multiView->addChild(preview);

        // dodanie item�w do multiviewa
        core::MultiViewWidgetItem* thumbnailItem = new core::MultiViewWidgetItem(thumbnail, ratio);
        core::MultiViewWidgetItem* previewItem = new core::MultiViewWidgetItem(preview, ratio);
        multiView->addItem(thumbnailItem, previewItem);

        optimizer->setImage(image);
        optimizer->getClients().push_back(new OsgWidgetWindowItem(thumbnail, streamWidget));
        optimizer->getClients().push_back(new OsgWidgetWindowItem(preview, streamWidgetClone));
        optimizers.push_back(optimizer);
        multiView->addUpdateCallback(optimizer);
    }

    // obliczenie rozmiaru grida (jak najbardziej kwadratowy)
    avgRatio /= images.size();
    osg::Vec2s dimensions = osgui::Grid::getDimensionsAsSquare(images.size());
    unsigned rows = dimensions.y();
    unsigned columns = dimensions.x();

    if ( doSelect ) {
        multiView->setSelectedByIndex(sel);
    }
}

void VideoWidget::clearScene()
{
    // usuni�cie callback�w
    BOOST_FOREACH(VideoImageStreamSizeOptimizer* optimizer, optimizers) {
        multiView->removeUpdateCallback(optimizer);
    }
    optimizers.clear();

    // usuni�cie item�w
    multiView->removeAllItems();
    // usuni�cie wszystkich w�z��w (!)
    multiView->removeChildren(0, multiView->getNumChildren());
    // reinicjalizacja multi view (po usunieciu wszystkich w�z��w - wymagana)
    multiView->restoreRequiredChildren();

    // dodanie "hacka"
    multiView->addChild(new osgWidget::Box("HACK"));
}


osgWidget::Widget* VideoWidget::createStreamWidget( osg::Image* image )
{
    return createStreamWidget(image, new osg::Uniform(yuvSamplerName.c_str(), 0), new osg::Uniform(yuvImageSizeName.c_str(), osg::Vec2(0, 0)) );
}

osgWidget::Widget* VideoWidget::createStreamWidget( osg::Image* image, VideoImageStreamSizeOptimizer* optimizer )
{
    return createStreamWidget(image, new osg::Uniform(yuvSamplerName.c_str(), 0), optimizer->getImageSize() );
}

osgWidget::Widget* VideoWidget::createStreamWidget( osg::Image* image, osg::Uniform* sampler, osg::Uniform* imageSize )
{
    UTILS_ASSERT(image);
    // tworzymy kontrolk�
    StreamOsgWidget* widget = new StreamOsgWidget( core::Filesystem::Path(image->getFileName()).filename().string() );

    // ustawienie tekstury
    widget->setImage(image, true, useTextureRect);
    // czy trzeba zrobi� flipa?
    osgui::correctTexCoords(widget, image);
    // poprawki
    if ( !useTextureRect && osgui::getTexture(widget) ) {
        osgui::getTexture(widget)->setResizeNonPowerOfTwoHint(false);
    }

    // ustawienie formatu
    if (VideoImageStream* stream = dynamic_cast<VideoImageStream*>(image)) {
        if ( stream->getTargetFormat() == PixelFormatYV12 ) {
            // ustawienie shader�w
            widget->setYuvTexture2DShader(yuvTexture2DShader);
            widget->setYuvTextureRectShader(yuvTextureRectShader);
            widget->getOrCreateStateSet()->addUniform( sampler );
            widget->getOrCreateStateSet()->addUniform( imageSize );
        }
        widget->setPixelFormat(stream->getTargetFormat());   
    }

    widget->refreshShaders();
    return widget;
}



void VideoWidget::loadShaders(const std::string& yuvTextureRect, const std::string& yuvTexture2D)
{
    yuvTexture2DShader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, yuvTexture2D);
    if (!yuvTexture2DShader) {
        LOG_ERROR("Could not load shader: "<<yuvTextureRect);
    }
    yuvTextureRectShader = osg::Shader::readShaderFile(osg::Shader::FRAGMENT, yuvTextureRect);
    if (!yuvTextureRectShader) {
        LOG_ERROR("Could not load shader: "<<yuvTexture2D);
    }
}

void VideoWidget::loadShaders()
{
    loadShaders( 
        yuvTextureRectShader ? yuvTextureRectShader->getFileName() : "",
        yuvTexture2DShader ? yuvTexture2DShader->getFileName() : "" 
    );
}

VideoWidget::~VideoWidget()
{

}

void VideoWidget::onContextMenuRequested( QPoint position )
{
    QPoint globalPos = mapToGlobal(position);

    QMenu *menu=new QMenu;
    menu->addSeparator();
    menu->addAction("Format")->setEnabled(false);
    menu->addAction(actionFormatYUV);
    menu->addAction(actionFormatRGB);
    menu->addAction(actionFormatBGRA);
    menu->addSeparator();
    menu->addAction(actionTextureRectangle);

    menu->exec(globalPos);
}

void VideoWidget::onFormatYUVToggled(bool toggled)
{
    if ( toggled ) {
        setPixelFormat(vidlib::PixelFormatYV12);
    }
}

void VideoWidget::onFormatRGBToggled(bool toggled)
{
    if ( toggled ) {
        setPixelFormat(vidlib::PixelFormatRGB24);
    }
}

void VideoWidget::onFormatBGRAToggled(bool toggled)
{
    if ( toggled ) {
        setPixelFormat(vidlib::PixelFormatBGRA);
    }
}

void VideoWidget::onTextureRectangleToggled( bool toggled )
{
    setUseTextureRect( toggled );
}