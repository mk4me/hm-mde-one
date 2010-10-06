#include "VideoPCH.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <osg/ref_ptr>
#include <osg/Referenced>
#include <osg/ImageStream>
#include <osg/Timer>
#include <osg/Notify>
#include <OpenThreads/Thread>
#include <osgWidget/Box>
#include <osgWidget/Widget>
#include <osgWidget/Window>
#include <osgWidget/WindowManager>
#include <osgWidget/Label>

#include <utils/PtrPolicyOSG.h>
#include <utils/ObserverPattern.h>


#include "Model.h"
#include "Controller.h"
#include "View.h"

#include "core/VM.h"
#include "osg/VideoImageStream.h"

#include "Button.h"
#include "HorizontalScrollBar.h"
#include "WidgetFunctors.h"
#include "StreamsViewOSGWidget.h"
#include "StaticKeyboardHandler.h"
#include "ImageCanvas.h"


using namespace osgUI;

const std::string SHADERS_ROOT = "data/resources/shaders/";

class FindCanvasesVisitor : public osg::NodeVisitor
{
public:
    std::vector<osgUI::ImageCanvas*> canvases;

    virtual void apply(osg::Node& node)
    {
        if ( osgUI::ImageCanvas* canvas = dynamic_cast<osgUI::ImageCanvas*>(&node) ) {
            apply(*canvas);
        }
        traverse(node);
    }

    inline void apply(osgUI::ImageCanvas& canvas) 
    {
        canvases.push_back(&canvas);
    }
};


struct ImageViewUpdater : public osg::NodeCallback
{
  osg::observer_ptr<StreamViewOSGWidget> manager;
  osg::observer_ptr<osg::Image> image;
  double lastWidth;
  double lastHeight;
  float scale;
  osg::observer_ptr<osg::Uniform> textureSize;

  ImageViewUpdater(StreamViewOSGWidget * manager, osg::Image * image, osg::Uniform* textureSize = NULL)
  {
    this->manager = manager;
    this->image = image;
    this->textureSize = textureSize;
    lastWidth = manager->getWidth();
    lastHeight = manager->getHeight();
    scale = 1;
  }

  void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    ImageCanvas * canvas = dynamic_cast<ImageCanvas*>(node);
    if ( canvas || manager.valid() || image->valid() ) {
      // czy zmiana rozmiaru?
      if ( lastWidth !=  manager->getWidth() || lastHeight != manager->getHeight() ) {
        // znormalizowane rozmiary okna
        float normWidth = scale / manager->getColumns();
        float normHeight = scale / manager->getRows();
        // znormalizowane wspó³rzêdne œrodka
        float normX = (canvas->getX() + canvas->getWidth() / 2.0f) / lastWidth;
        float normY = (canvas->getY() + canvas->getHeight() / 2.0f) / lastHeight;
        // zmieniamy rozmiar zachowuj¹c proporcje
        canvas->resize( normWidth * manager->getWidth(), normHeight * manager->getHeight() );
        // nadajemy nowe wsó³rzêdne (lewego dolnego rogu)
        float x = normX * manager->getWidth() - canvas->getWidth() / 2.0f;
        float y = normY * manager->getHeight() - canvas->getHeight() / 2.0f;
        canvas->setOrigin( static_cast<int>(x + 0.5f), static_cast<int>(y + 0.5f) );
        canvas->resizeAdd();
        // zapamiêtujemy
        lastWidth = manager->getWidth();
        lastHeight = manager->getHeight();
        notifyImageSize(canvas);
      }
    }
    traverse(node, nv);
  }

  void notifyImageSize( ImageCanvas * canvas )
  {
    vmOSGPlugin::VideoImageStream * stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image.get());
    if ( stream ) {
      stream->setMaxWidth(static_cast<int>(canvas->getWidth()));
      canvas->setTexCoord();
      // czy jest zmienna okreœlaj¹ca rozmiar tekstury?
      if ( textureSize.valid() ) {
        textureSize->set( stream->s(), stream->t() );
      }
      //canvas->getRect()->getOrCreateStateSet()->setUniformList()
    }
  }

  //! Scroll myszki.
  //! \param event
  bool onMouseScroll(osgWidget::Event& event)
  {
    ImageCanvas * canvas = dynamic_cast<ImageCanvas*>(event.getWindow());
    if ( StaticKeyboardHandler::isCtrlPressed() ) {
      if ( event.getWindowManager()->isMouseScrollingUp()) {
        changeScale(canvas, +0.0334f);
      } else {
        changeScale(canvas, -0.0334f);
      }
    } else if ( StaticKeyboardHandler::isAltPressed() ) {
      if ( event.getWindowManager()->isMouseScrollingUp()) {
        changeAlpha(canvas, +0.0334f);
      } else {
        changeAlpha(canvas, -0.0334f);
      }
    }
    return false;
  }

  //! Zmienia wartoœæ modyfikatora rozmiaru.
  //! \param delta
  void changeScale(ImageCanvas * canvas, float delta)
  {
    float newScale = std::max<float>(0.1f, scale + delta);
    // bazowy rozmiar
    float baseWidth = canvas->getWidth() / scale;
    float baseHeight = canvas->getHeight() / scale;
    // wspó³rzêdne œrodka
    float x = canvas->getX() + canvas->getWidth() / 2.0f;
    float y = canvas->getY() + canvas->getHeight() / 2.0f;
    // resize
    canvas->resize(baseWidth * newScale, baseHeight * newScale);
    canvas->setX(x - canvas->getWidth() / 2.0f);
    canvas->setY(y - canvas->getHeight() / 2.0f);
    canvas->resizeAdd();
    scale = newScale;
    notifyImageSize(canvas);
  }

  //! Zmienia alfê obrazka.
  //! \param delta
  void changeAlpha(ImageCanvas * canvas, float delta)
  {
    osgWidget::Color clr = canvas->getRect()->getColor();
    clr.a() = utils::clamp(clr.a() + delta, 0.0f, 1.0f);
    canvas->getRect()->setColor(clr);
  }
};


//!
StreamViewOSGWidget::StreamViewOSGWidget(osgViewer::View * view, bool useTextureRect,
                                         float width, float height, unsigned int mask, unsigned int flags)
: osgWidget::WindowManager(view, width, height, mask, flags),
  useTextureRect(useTextureRect),
  rows(1),
  columns(1)
{
  yuvProgram = new osg::Program;

  yuvTexture2DShader = new osg::Shader(osg::Shader::FRAGMENT);
  yuvTextureRectShader = new osg::Shader(osg::Shader::FRAGMENT);

  std::string path = SHADERS_ROOT + "texture2D_yuv_to_rgb.frag";
  if (!yuvTexture2DShader->loadShaderSourceFromFile(path)) {
      OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
      yuvTexture2DShader = NULL;
  }
  path = SHADERS_ROOT + "textureRect_yuv_to_rgb.frag";
  if (!yuvTextureRectShader->loadShaderSourceFromFile(path)) {
      OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
      yuvTextureRectShader = NULL;
  }
  setUseTextureRect(useTextureRect);
}


void StreamViewOSGWidget::addStream( osg::Image * image, int row, int column )
{
    float width = getWidth() / columns;
    float height = getHeight() / rows;

    // tworzymy kontrolkê
    ImageCanvas * canv = new ImageCanvas(image->getFileName());
    osg::Uniform* textureSize = NULL;

    // czy wymuszamy shader?
    vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
    if (stream && stream->getTargetFormat() == vm::PixelFormatYV12 /*&&
        static_cast<avinout::PixelFormat>(vmOSGPlugin::VideoImageStream::TargetFrame::pixelFormat) == avinout::PixelFormatYV12*/) {
        canv->setImage(image, useTextureRect);
        osg::ref_ptr<osg::StateSet> state = canv->getRect()->getOrCreateStateSet();
        state->addUniform(new osg::Uniform("movie_texture",0));
        state->addUniform(textureSize = new osg::Uniform("texture_size", 0, 0));
        state->setAttribute(yuvProgram);
    } else {
        canv->setImage(image, useTextureRect);
    }

    // dodajemy callbacki
    canv->attachMoveCallback();
    //canv->setShowLabel(false);
    //canv->setShowBorder(false);
    //canv->setEventMask(osgWidget::EVENT_MASK_MOUSE_CLICK);
    ImageViewUpdater * updater = new ImageViewUpdater(this, image, textureSize);
    canv->setUpdateCallback(updater);
    canv->getRect()->addCallback(new osgWidget::Callback(&ImageViewUpdater::onMouseScroll, updater, osgWidget::EVENT_MOUSE_SCROLL));

    // rozmiar
    canv->setKeepImageRatio(true);
    canv->resize(width, height);
    if (stream) {
        updater->notifyImageSize( canv );
        /*stream->setMaxWidth(static_cast<int>(width));
        canv->setTexCoord();*/
    }

    // pozycja canvasa
    float x  = width * column;
    float y = getHeight() - height * (row+1);
    x += (width - canv->getWidth()) / 2.0;
    y += (height - canv->getHeight()) / 2.0;
    canv->setOrigin( static_cast<int>(x + 0.5), static_cast<int>(y + 0.5));
    addChild(canv);
}

void StreamViewOSGWidget::clear()
{
    rows = 0;
    columns = 0;
    this->removeChildren(0, this->getNumChildren());
}

void StreamViewOSGWidget::setFormat( vm::PixelFormat format )
{
    FindCanvasesVisitor visitor;
    visitor.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
    this->accept(visitor);
    
    for (size_t i = 0; i < visitor.canvases.size(); ++i) {
        // czy wymuszamy shader?
        osgUI::ImageCanvas* canvas = visitor.canvases[i];
        vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(canvas->getImage());
        if ( !stream ) {
            continue;
        }
        ImageViewUpdater* callback = dynamic_cast<ImageViewUpdater*>(canvas->getUpdateCallback());
        if ( stream->getTargetFormat() != vm::PixelFormatYV12 ) {
            if ( format == vm::PixelFormatYV12 ) {
                osg::ref_ptr<osg::StateSet> state = canvas->getRect()->getOrCreateStateSet();
                osg::Uniform* textureSize = NULL;
                state->addUniform(new osg::Uniform("movie_texture",0));
                state->addUniform(textureSize = new osg::Uniform("texture_size", 0, 0));
                state->setAttribute(yuvProgram);
                callback->textureSize = textureSize;
            }
        } else if ( format != vm::PixelFormatYV12 ) {
            osg::ref_ptr<osg::StateSet> state = canvas->getRect()->getOrCreateStateSet();
            state->removeAttribute(yuvProgram);
            state->removeUniform("texture_size");
        }
        stream->setTargetFormat(format);
        callback->notifyImageSize(canvas);
        
    }
}

void StreamViewOSGWidget::setUseTextureRect( bool useTextureRect )
{
    while ( yuvProgram->getNumShaders() ) {
        yuvProgram->removeShader( yuvProgram->getShader(0) );
    }
    if ( useTextureRect ) {
        if (yuvTextureRectShader) {
            yuvProgram->addShader(yuvTextureRectShader);
        }
    } else {
        if (yuvTexture2DShader) {
            yuvProgram->addShader(yuvTexture2DShader);
        }
    }
    this->useTextureRect = useTextureRect;
}

void StreamViewOSGWidget::makeGrid( int count, int& rows, int& columns )
{
    columns = static_cast<int>(ceil(sqrt(static_cast<double>(count))));
    if ( columns != 0 ) {
        if ( columns * (columns-1) >= count ) {
            rows = columns-1;
        } else {
            rows = columns;
        }
    } else {
        rows = 0;
    }
}

