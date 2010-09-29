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

struct ImageViewUpdater : public osg::NodeCallback
{
  osg::observer_ptr<StreamViewOSGWidget> manager;
  osg::observer_ptr<osg::Image> image;
  double lastWidth;
  double lastHeight;
  float scale;
  osg::ref_ptr<osg::Uniform> textureSize;

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
      if ( lastWidth !=  manager->getWidth() || lastHeight !=  manager->getHeight() ) {
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
      if ( textureSize ) {
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
StreamViewOSGWidget::StreamViewOSGWidget(osgViewer::View * view, int rows, int columns, bool useTextureRect,
                                         float width, float height, unsigned int mask, unsigned int flags)
: osgWidget::WindowManager(view, width, height, mask, flags), rows(rows), columns(columns),
  useTextureRect(useTextureRect)
{
  yuvShader = new osg::Program;

  // nazwa programu
  std::string shaderName = useTextureRect ? "textureRect_yuv_to_rgb.frag" : "texture2D_yuv_to_rgb.frag";

  // odczyt shadera
  osg::Shader* shader = new osg::Shader(osg::Shader::FRAGMENT);
  if (shader->loadShaderSourceFromFile(SHADERS_ROOT + shaderName)) {
    // uda³o siê!
    yuvShader->addShader(shader);
  } else {
    throw std::runtime_error("Error reading shader file: " + shaderName);
  }
}


void StreamViewOSGWidget::addStream( osg::Image * image, int row, int column )
{
#if 0
  {
    // tworzymy teksturkê
    osg::Texture* texture = new osg::Texture2D();
    bool flip = image->getOrigin()==osg::Image::TOP_LEFT;  texture->setImage(0, image);
    texture->setResizeNonPowerOfTwoHint(false);

    // tworzymy widget
    osgWidget::Widget * rect = new osgWidget::Widget("Image", 64, 64);
    rect->setTexture(texture, false, false);
    rect->setPadding(1);
    rect->setPadTop(0);
    double bottom = flip ? 1.0 : 0.0;
    double top = flip ? 0.0 : 1.0;
    rect->setTexCoord(0.0, top, osgWidget::Widget::UPPER_LEFT);
    rect->setTexCoord(0.0, bottom, osgWidget::Widget::LOWER_LEFT);
    rect->setTexCoord(1.0, top, osgWidget::Widget::UPPER_RIGHT);
    rect->setTexCoord(1.0, bottom, osgWidget::Widget::LOWER_RIGHT);
    // czy wymuszamy shader?
    if (dynamic_cast<vmOSGPlugin::VideoImageStream*>(image) &&
      vmOSGPlugin::VideoImageStream::TargetFrame::pixelFormat == avinout::PixelFormatYV12) {
      rect->getOrCreateStateSet()->addUniform(new osg::Uniform("movie_texture",0));
      rect->getOrCreateStateSet()->setAttribute(yuvShader);
    }


    // tworzymy labelkê
    osgWidget::Label * label = new osgWidget::Label("Label", image->getFileName());
    label->setColor(osgWidget::Color(0.25, 0.25, 0.25, 1));
    label->setFontColor(osgWidget::Color(1, 1, 1, 1));
    label->setAlignHorizontal(osgWidget::Widget::HA_CENTER);
    label->setAlignVertical(osgWidget::Widget::VA_TOP);
    label->setPadding(1);
    //label->setPadBottom(0);
    label->setHeight(10);

    // tworzymy boxa który to wszystko trzyma
    osgWidget::Box * box = new osgWidget::Box("Box", osgWidget::Box::VERTICAL);
    box->addWidget(rect);
    box->addWidget(label);
    box->attachMoveCallback();
    box->setUpdateCallback(new ImageViewUpdater(this, image));
    // rozmiar boxa
    resizeImageBox(box, image);
    box->resizeAdd();

    // pozycja boxa
    double dx = getWidth() / columns;
    double dy = getHeight() / rows;
    double x  = dx * column;
    double y = getHeight() - dy * (row+1);
    x += (dx - box->getWidth()) / 2.0;
    y += (dy - box->getHeight()) / 2.0;
    box->setOrigin( static_cast<int>(x + 0.5), static_cast<int>(y + 0.5));
    addChild(box);
  }
#else
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
      state->setAttribute(yuvShader);
    } else {
      canv->setImage(image, useTextureRect);
    }
    // dodajemy callbacki
    canv->attachMoveCallback();
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
#endif
}
