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

#undef  min
#undef  max

#include <utils/PtrPolicyOSG.h>
#include <utils/ObserverPattern.h>
#include "core/VM.h"
#include "osg/VideoImageStream.h"

#include "HorizontalScrollBar.h"
#include "WidgetFunctors.h"
#include "StreamsViewOSGWidget.h"
#include "StaticKeyboardHandler.h"
#include "ImageCanvas.h"

#include <core/OsgWidgetGridView.h>

#include <core/Enumerators.h>



#include <core/AspectRatioKeeper.h>
#include "StreamOsgWidget.h"

//
//
//using namespace osgUI;
//
//const std::string SHADERS_ROOT = "data/resources/shaders/";
//
//class FindCanvasesVisitor : public osg::NodeVisitor
//{
//public:
//    std::vector<osgUI::ImageCanvas*> canvases;
//
//    virtual void apply(osg::Node& node)
//    {
//        if ( osgUI::ImageCanvas* canvas = dynamic_cast<osgUI::ImageCanvas*>(&node) ) {
//            apply(*canvas);
//        }
//        traverse(node);
//    }
//
//    inline void apply(osgUI::ImageCanvas& canvas) 
//    {
//        canvases.push_back(&canvas);
//    }
//};
//
//
//struct ImageViewUpdater : public osg::NodeCallback
//{
//  osg::observer_ptr<StreamViewOSGWidget> manager;
//  osg::observer_ptr<osg::Image> image;
//  double lastWidth;
//  double lastHeight;
//  float scale;
//  osg::observer_ptr<osg::Uniform> textureSize;
//  int index;
//
//  ImageViewUpdater(StreamViewOSGWidget * manager, osg::Image * image, int index, osg::Uniform* textureSize = NULL)
//  {
//    this->manager = manager;
//    this->image = image;
//    this->textureSize = textureSize;
//    lastWidth = manager->getWidth();
//    lastHeight = manager->getHeight();
//    scale = 1;
//    this->index = index;
//  }
//
//  void operator()(osg::Node* node, osg::NodeVisitor* nv)
//  {
//    ImageCanvas * canvas = dynamic_cast<ImageCanvas*>(node);
//    if ( canvas || manager.valid() || image->valid() ) {
//      // czy zmiana rozmiaru?
//      if ( lastWidth !=  manager->getWidth() || lastHeight != manager->getHeight() ) {
//    
////           core::GridModel &model = manager->model;
////           model.setSize( manager->getWidth(), manager->getHeight() );
////           model.setNumItems(  manager->getRows() * manager->getColumns() );
////           model.setAspectRatio( canvas->getImage()->getPixelAspectRatio() * canvas->getImage()->s() / canvas->getImage()->t() );
////           
////           core::GridModel::Item item = model.getItem(index);
////           canvas->setOrigin(item.x, item.y);
////           canvas->resize( item.width, item.height );
//
////         // znormalizowane rozmiary okna
////         float normWidth = scale / manager->getColumns();
////         float normHeight = scale / manager->getRows();
////         // znormalizowane wspó³rzêdne œrodka
////         float normX = (canvas->getX() + canvas->getWidth() / 2.0f) / lastWidth;
////         float normY = (canvas->getY() + canvas->getHeight() / 2.0f) / lastHeight;
////         // zmieniamy rozmiar zachowuj¹c proporcje
////         canvas->resize( normWidth * manager->getWidth(), normHeight * manager->getHeight() );
////         // nadajemy nowe wsó³rzêdne (lewego dolnego rogu)
////         float x = normX * manager->getWidth() - canvas->getWidth() / 2.0f;
////         float y = normY * manager->getHeight() - canvas->getHeight() / 2.0f;
////         canvas->setOrigin( static_cast<int>(x + 0.5f), static_cast<int>(y + 0.5f) );
//        canvas->resizeAdd();
//        // zapamiêtujemy
//        lastWidth = manager->getWidth();
//        lastHeight = manager->getHeight();
//        notifyImageSize(canvas);
//      }
//    }
//    traverse(node, nv);
//  }
//
//  void notifyImageSize( ImageCanvas * canvas )
//  {
//    vmOSGPlugin::VideoImageStream * stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image.get());
//    if ( stream ) {
//      stream->setMaxWidth(static_cast<int>(canvas->getWidth()));
//      canvas->setTexCoord();
//      // czy jest zmienna okreœlaj¹ca rozmiar tekstury?
//      if ( textureSize.valid() ) {
//        textureSize->set( stream->s(), stream->t() );
//      }
//      //canvas->getRect()->getOrCreateStateSet()->setUniformList()
//    }
//  }
//
//  //! Scroll myszki.
//  //! \param event
//  bool onMouseScroll(osgWidget::Event& event)
//  {
//    ImageCanvas * canvas = dynamic_cast<ImageCanvas*>(event.getWindow());
//    if ( StaticKeyboardHandler::isCtrlPressed() ) {
//      if ( event.getWindowManager()->isMouseScrollingUp()) {
//        changeScale(canvas, +0.0334f);
//      } else {
//        changeScale(canvas, -0.0334f);
//      }
//    } else if ( StaticKeyboardHandler::isAltPressed() ) {
//      if ( event.getWindowManager()->isMouseScrollingUp()) {
//        changeAlpha(canvas, +0.0334f);
//      } else {
//        changeAlpha(canvas, -0.0334f);
//      }
//    }
//    return false;
//  }
//
//  //! Zmienia wartoœæ modyfikatora rozmiaru.
//  //! \param delta
//  void changeScale(ImageCanvas * canvas, float delta)
//  {
//    float newScale = std::max<float>(0.1f, scale + delta);
//    // bazowy rozmiar
//    float baseWidth = canvas->getWidth() / scale;
//    float baseHeight = canvas->getHeight() / scale;
//    // wspó³rzêdne œrodka
//    float x = canvas->getX() + canvas->getWidth() / 2.0f;
//    float y = canvas->getY() + canvas->getHeight() / 2.0f;
//    // resize
//    canvas->resize(baseWidth * newScale, baseHeight * newScale);
//    canvas->setX(x - canvas->getWidth() / 2.0f);
//    canvas->setY(y - canvas->getHeight() / 2.0f);
//    canvas->resizeAdd();
//    scale = newScale;
//    notifyImageSize(canvas);
//  }
//
//  //! Zmienia alfê obrazka.
//  //! \param delta
//  void changeAlpha(ImageCanvas * canvas, float delta)
//  {
//    osgWidget::Color clr = canvas->getRect()->getColor();
//    clr.a() = utils::clamp(clr.a() + delta, 0.0f, 1.0f);
//    canvas->getRect()->setColor(clr);
//  }
//};
//
//
//class VideoImageStreamGridItem : public core::WidgetMultiViewItem
//{
//private:
//    //! Rozmiar tekstury.
//    osg::observer_ptr<osg::Uniform> textureSize;
//
//public:
//    //! 
//    //! \param widget
//    VideoImageStreamGridItem(osgWidget::Widget* widget) : core::WidgetMultiViewItem(widget)
//    {
//    }
//
//public:
//    //! \return
//    osg::Uniform* getTextureSize()
//    { 
//        osg::ref_ptr<osg::Uniform> textureSize;
//        if ( this->textureSize.lock(textureSize) ) {
//            return textureSize.get();
//        } else {
//            return NULL;
//        }
//    }
//    //! \param textureSize
//    void setTextureSize(osg::Uniform* textureSize) 
//    { 
//        this->textureSize = textureSize; 
//    }
//
//    //! \param location
//    virtual void setLocation(osg::Vec4 location)
//    {
//        core::WidgetMultiViewItem::setLocation(location);
//        resizeStream();
//    }
//
//    //! 
//    void resizeStream()
//    {
//        osg::Texture* texture = NULL;
//        osg::Image* image = NULL;
//        vmOSGPlugin::VideoImageStream* stream = NULL;
//        osg::StateSet* state = NULL;
//
//        // pobranie danych
//        if (state = widget->getStateSet()) {
//            osg::Texture * texture = dynamic_cast<osg::Texture*>(state->getTextureAttribute(0, osg::StateAttribute::TEXTURE));
//            if ( texture ) {
//                image = texture->getImage(0);
//                stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
//            }
//        }
//
//        // aktualizacja rozmiaru i uvek
//        if ( stream ) {
//            // aby niepotrzebnie nie tworzyæ gigantycznych tekstur
//            stream->setMaxWidth( widget->getWidth() );
//
//            // aktualizacja uvek i rozmiaru
//            bool useTextureRect = (dynamic_cast<osg::TextureRectangle*>(texture) != NULL);
//            bool flip = (image->getOrigin() == osg::Image::TOP_LEFT);
//
//            // ustalenie wspó³rzêdnych
//            float left, right, top, bottom;
//            float width, height;
//            if ( useTextureRect ) {
//                width = image->s();
//                height = image->t();
//            } else {
//                width = 1.0;
//                height = 1.0;
//            }
//            bottom = (flip ? 1.0 : 0.0) * height;
//            top = (flip ? 0.0 : 1.0) * height;
//            left = 0;
//            right = width;
//
//            // ustawienie wspó³rzêdnych
//            widget->setTexCoord(left, top, osgWidget::Widget::UPPER_LEFT);
//            widget->setTexCoord(left, bottom, osgWidget::Widget::LOWER_LEFT);
//            widget->setTexCoord(right, top, osgWidget::Widget::UPPER_RIGHT);
//            widget->setTexCoord(right, bottom, osgWidget::Widget::LOWER_RIGHT);
//        }
//
//        // aktualizacja zmiennej trzymaj¹cej rozmiar tekstury
//        osg::ref_ptr<osg::Uniform> textureSize;
//        if ( this->textureSize.lock(textureSize) ) {
//            textureSize->set(image->s(), image->t());
//        }
//    }
//};
//
//bool callbackWindowResize(osgWidget::Event& ev)
//{
//    if(!ev.getWindow() || !ev.getWindowManager()->isMiddleMouseButtonDown()) return false;
//    ev.getWindow()->resizeAdd(ev.x, ev.y);
//    ev.getWindow()->update();
//    return true;
//}
//
//
////!
//StreamViewOSGWidget::StreamViewOSGWidget(osgViewer::View * view, bool useTextureRect,
//                                         float width, float height, unsigned int mask, unsigned int flags)
//: osgWidget::WindowManager(view, width, height, mask, flags),
//  useTextureRect(useTextureRect),
//  rows(1),
//  columns(1)
//{
//  yuvProgram = new osg::Program;
//
//  yuvTexture2DShader = new osg::Shader(osg::Shader::FRAGMENT);
//  yuvTextureRectShader = new osg::Shader(osg::Shader::FRAGMENT);
//
//  std::string path = SHADERS_ROOT + "texture2D_yuv_to_rgb.frag";
//  if (!yuvTexture2DShader->loadShaderSourceFromFile(path)) {
//      OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
//      yuvTexture2DShader = NULL;
//  }
//  path = SHADERS_ROOT + "textureRect_yuv_to_rgb.frag";
//  if (!yuvTextureRectShader->loadShaderSourceFromFile(path)) {
//      OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
//      yuvTextureRectShader = NULL;
//  }
//  setUseTextureRect(useTextureRect);
//
////   model.setNumItems(rows * columns);
////   model.setSize( getWidth(), getHeight() );
//
//    gridView = new core::OsgWidgetGrid("grid");
//    gridView->resize(150, 150);
//    gridView->getBackground()->setColor(0.0, 0.0, 0.0, 0);
//    gridView->getLabel()->setColor(0.7,0.7,0.7,0.5);
//    //gridView->getLabel()->setLabel()
//    gridView->setOrigin(50, 50);
//
//    
//    gridView->attachMoveCallback();
//    addChild(gridView);
////
////    osgUI::Grid* grid = new osgUI::Grid("GRID", 3, 2);
////    //osgWidget::Table* grid = new osgWidget::Table("GRID", 3, 2);
////    grid->getBackground()->setColor(1,1,1,1);
////
////    osgWidget::Widget* test = new osgWidget::Widget("0, 0", 100.0f, 25.0f);
////    test->setColor(1,1,0,1);
////    test->setPadding(10);
////    //test->setCanFill(true);
////    //test->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
////    //test->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
////
////    //osgWidget::Table* grid = new osgWidget::Table("GRID", 3, 2);
////    grid->setOrigin(0, 0);
////    grid->addWidget( new osgUI::AspectRatioKeeper(test, 2), 0, 0);
////    grid->addWidget( new osgWidget::Widget("0, 1", 100.0f, 25.0f), 0, 1);
////    grid->addWidget( new osgWidget::Widget("2, 0", 10.0f, 25.0f), 2, 0);
////    grid->addWidget( new osgWidget::Widget("2, 1", 10.0f, 25.0f), 2, 1);
////
//////     grid->getByName("0, 0")->setAlignHorizontal(osgWidget::Widget::HA_LEFT);
//////     grid->getByName("0, 0")->setAlignVertical(osgWidget::Widget::VA_BOTTOM);
////    //grid->getByName("0, 0")->setPadding(10.0f);
////    grid->getByName("0, 0")->setCanFill(true);
////    grid->getByName("0, 1")->setCanFill(true);
////    grid->getByName("2, 1")->setCanFill(true);
////    grid->getByName("2, 0")->setCanFill(true);
////
////
////    grid->addWidget( new osgWidget::Widget("1, 0", 100.0f, 0.0f), 1, 0);
////    grid->addWidget( new osgWidget::Widget("1, 1", 10.0f, 0.0f), 1, 1);
////    grid->getByName("1, 1")->setCanFill(true);
////    grid->getByName("1, 0")->setCanFill(true);
////    grid->getByName("1, 1")->setPadLeft(50.0f);
////    grid->getByName("1, 0")->setPadLeft(50.0f);
////    grid->setRowWeight(1, 0.0f);
////    grid->setRowWeight(2, 5.0f);
////    // Change the colors a bit to differentiate this row from the others.
////
////    grid->getByName("2, 0")->setColor(1.0f, 0.0f, 0.0f, 1.0f, osgWidget::Widget::LOWER_LEFT);
////    grid->getByName("2, 1")->setColor(1.0f, 0.0f, 0.0f, 0.5f);
////    grid->getByName("2, 1")->setWidth(666);
////    grid->attachMoveCallback();
////    //grid->setDimensions(3,1);
////    //grid->addWidget( new osgWidget::Widget("1, 1", 100.0f, 25.0f), 2, 0 );
////
////    grid->setEventMask(osgWidget::EVENT_MASK_MOUSE_DRAG);
////    grid->addCallback( new osgWidget::Callback(&callbackWindowResize, osgWidget::EVENT_MOUSE_DRAG));
////    grid->resizeAdd();
////    grid->getBackground()->setColor(0,0,0,0.5);
////
////    //grid->adjustDimensions(500, 500, 1);
////
////    addChild(grid);
//
//}
//
//
//
//void StreamViewOSGWidget::addStream( osg::Image * image, int row, int column )
//{
//    float width = getWidth() / columns;
//    float height = getHeight() / rows;
//
// 
//    // tworzymy kontrolkê
//    //ImageCanvas * canv = new ImageCanvas(image->getFileName());
//    //osgWidget::Widget* canv = new osgWidget::Widget(image->getFileName());
//    video::StreamOsgWidget* canv = new video::StreamOsgWidget(image->getFileName());
//    osg::Uniform* textureSize = NULL;
//
//
//    // czy wymuszamy shader?
//    vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
//    if (stream && stream->getTargetFormat() == vm::PixelFormatYV12 /*&&
//        static_cast<avinout::PixelFormat>(vmOSGPlugin::VideoImageStream::TargetFrame::pixelFormat) == avinout::PixelFormatYV12*/) {
//        canv->setImage(image, true, useTextureRect);
//        //osg::ref_ptr<osg::StateSet> state = canv->getRect()->getOrCreateStateSet();
//        osg::ref_ptr<osg::StateSet> state = canv->getOrCreateStateSet();
//        state->addUniform(new osg::Uniform("movie_texture",0));
//        state->addUniform(textureSize = new osg::Uniform("texture_size", 0, 0));
//        state->setAttribute(yuvProgram);
//
//        canv->setTextureSize(textureSize);
//    } else {
//        canv->setImage(image, true, useTextureRect);
//    }
//
//    // czy trzeba zrobiæ flipa?
//    if (image->getOrigin()==osg::Image::TOP_LEFT) {
//        osgWidget::TexCoord temp = canv->getTexCoord(osgWidget::Widget::UPPER_LEFT);
//        canv->setTexCoord(canv->getTexCoord(osgWidget::Widget::LOWER_LEFT), osgWidget::Widget::UPPER_LEFT);
//        canv->setTexCoord(temp, osgWidget::Widget::LOWER_LEFT);
//        temp = canv->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
//        canv->setTexCoord(canv->getTexCoord(osgWidget::Widget::LOWER_RIGHT), osgWidget::Widget::UPPER_RIGHT);
//        canv->setTexCoord(temp, osgWidget::Widget::LOWER_RIGHT);
//    }
//
//    // dodajemy callbacki
//    //canv->attachMoveCallback();
//    //canv->setShowLabel(false);
//    //canv->setShowBorder(false);
//    //canv->setEventMask(osgWidget::EVENT_MASK_MOUSE_CLICK);
////     ImageViewUpdater * updater = new ImageViewUpdater(this, image, row * rows + column, textureSize);
////     canv->setUpdateCallback(updater);
////     canv->getRect()->addCallback(new osgWidget::Callback(&ImageViewUpdater::onMouseScroll, updater, osgWidget::EVENT_MOUSE_SCROLL));
//// 
////     // rozmiar
////     canv->setKeepImageRatio(true);
////     canv->getBorder()->setShowLabel(false);
////     canv->getBorder()->setShowBorder(false);
////     canv->resize(width, height);
////     if (stream) {
////         updater->notifyImageSize( canv );
////         /*stream->setMaxWidth(static_cast<int>(width));
////         canv->setTexCoord();*/
////     }
//// 
////     // pozycja canvasa
////     float x  = width * column;
////     float y = getHeight() - height * (row+1);
////     x += (width - canv->getWidth()) / 2.0;
////     y += (height - canv->getHeight()) / 2.0;
////     canv->setOrigin( static_cast<int>(x + 0.5), static_cast<int>(y + 0.5));
////     //addChild(canv);
//// 
//    //osg::ImageStream
//
//    //canv->setCanFill(true);
////     dynamic_cast<osgUI::Grid*>(getByName("GRID"))->addWidget( new osgUI::AspectRatioKeeper(canv, stream->getPixelAspectRatio()), 0, 0);
////  
//    osgWidget::Box* box = new osgWidget::Box();
//    canv->setName("ITEM");
//    box->addWidget(canv);
//    addChild(box);
//
//
//    osgWidget::Widget* embedded = canv;
//    //embedded->setColor(0, 0, 0, 0);
//    //gridView->addWidget( embedded );
//    //addChild()
//    gridView->addItem( new core::WindowMultiViewItem(box, gridView) );
//    gridView->setOrigin(50, 50);
//    gridView->resize(150, 150);
//    //gridView->addCallback(new osgWidget::Callback(&callbackWindowResize, osgWidget::EVENT_MOUSE_DRAG));
//    
//
//
////     osgWidget::Widget * border[4];
//// 
////     border[LEFT] = new osgWidget::Widget("borderLeft", 1, 1);
////     border[LEFT]->setColor(1, 1, 1, 1);
////     border[UP] = osg::clone(border[LEFT]/*.get()*/, "borderTop", osg::CopyOp::DEEP_COPY_ALL);
////     border[RIGHT] = osg::clone(border[LEFT]/*.get()*/, "borderRight", osg::CopyOp::DEEP_COPY_ALL);
////     border[DOWN] = osg::clone(border[LEFT]/*.get()*/, "borderBottom", osg::CopyOp::DEEP_COPY_ALL);
//// 
//
////     if ( row == 0 && column == 0 ) {
//// 
////     } else {
////         return;
////     }
//    //core::OsgWidgetGrid * test = new core::OsgWidgetGrid("DUPA");
////     osgWidget::Widget* kurwa = new osgWidget::Widget("ddd", 50, 50);
////     kurwa->setColor(0, 0, 1, 1);
////     gridView->addWidget( kurwa );
////     gridView->addItem( new core::WidgetMultiViewItem(kurwa) );
////     gridView->addItem( new core::WidgetMultiViewItem( osg::clone(kurwa, osg::CopyOp::DEEP_COPY_ALL) ) );
////     gridView->addItem( new core::WidgetMultiViewItem( osg::clone(kurwa, osg::CopyOp::DEEP_COPY_ALL) ) );
////     gridView->addItem( new core::WidgetMultiViewItem( osg::clone(kurwa, osg::CopyOp::DEEP_COPY_ALL) ) );
//    gridView->resize(450, 250);
//    gridView->setOrigin(50, 50);
//    //gridView->attachMoveCallback();
//    //addChild(gridView);
//    gridView->getLabel()->setLabel("FUUUUUUUU");
//}
//
//void StreamViewOSGWidget::clear()
//{
////    return;
//    rows = 0;
//    columns = 0;
//    //this->removeChildren(0, this->getNumChildren());
//    //gridView->removeAllItems();
//    //gridView->removeChildren(0, gridView->getNumChildren());
//}
//
//void StreamViewOSGWidget::setFormat( vm::PixelFormat format )
//{
//    return;
//    FindCanvasesVisitor visitor;
//    visitor.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
//    this->accept(visitor);
//    
//    for (size_t i = 0; i < visitor.canvases.size(); ++i) {
//        // czy wymuszamy shader?
//        osgUI::ImageCanvas* canvas = visitor.canvases[i];
//        vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(canvas->getImage());
//        if ( !stream ) {
//            continue;
//        }
//        ImageViewUpdater* callback = dynamic_cast<ImageViewUpdater*>(canvas->getUpdateCallback());
//        if ( stream->getTargetFormat() != vm::PixelFormatYV12 ) {
//            if ( format == vm::PixelFormatYV12 ) {
//                osg::ref_ptr<osg::StateSet> state = canvas->getRect()->getOrCreateStateSet();
//                osg::Uniform* textureSize = NULL;
//                state->addUniform(new osg::Uniform("movie_texture",0));
//                state->addUniform(textureSize = new osg::Uniform("texture_size", 0, 0));
//                state->setAttribute(yuvProgram);
//                callback->textureSize = textureSize;
//            }
//        } else if ( format != vm::PixelFormatYV12 ) {
//            osg::ref_ptr<osg::StateSet> state = canvas->getRect()->getOrCreateStateSet();
//            state->removeAttribute(yuvProgram);
//            state->removeUniform("texture_size");
//        }
//        stream->setTargetFormat(format);
//        callback->notifyImageSize(canvas);
//        
//    }
//}
//
//void StreamViewOSGWidget::setUseTextureRect( bool useTextureRect )
//{
//    while ( yuvProgram->getNumShaders() ) {
//        yuvProgram->removeShader( yuvProgram->getShader(0) );
//    }
//    if ( useTextureRect ) {
//        if (yuvTextureRectShader) {
//            yuvProgram->addShader(yuvTextureRectShader);
//        }
//    } else {
//        if (yuvTexture2DShader) {
//            yuvProgram->addShader(yuvTexture2DShader);
//        }
//    }
//    this->useTextureRect = useTextureRect;
//}
//
//void StreamViewOSGWidget::makeGrid( int count, int& rows, int& columns )
//{
//    columns = static_cast<int>(ceil(sqrt(static_cast<double>(count))));
//    if ( columns != 0 ) {
//        if ( columns * (columns-1) >= count ) {
//            rows = columns-1;
//        } else {
//            rows = columns;
//        }
//    } else {
//        rows = 0;
//    }
//}

OsgWidgetStreamHelper::OsgWidgetStreamHelper( const std::string& shaderDirectory, bool useTextureRect ) :
useTextureRect(useTextureRect)
{
    yuvProgram = new osg::Program;

    yuvTexture2DShader = new osg::Shader(osg::Shader::FRAGMENT);
    yuvTextureRectShader = new osg::Shader(osg::Shader::FRAGMENT);

    // TODO: tutaj powinien dzia³aæ DataManager raczej
    std::string path = shaderDirectory + "texture2D_yuv_to_rgb.frag";
    if (!yuvTexture2DShader->loadShaderSourceFromFile(path)) {
        OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
        yuvTexture2DShader = NULL;
    }
    path = shaderDirectory + "textureRect_yuv_to_rgb.frag";
    if (!yuvTextureRectShader->loadShaderSourceFromFile(path)) {
        OSG_FATAL<<"Could not load shader: "<<path<<std::endl;
        yuvTextureRectShader = NULL;
    }

    if ( useTextureRect ) {
        yuvProgram->addShader( yuvTextureRectShader );
    } else {
        yuvProgram->addShader( yuvTexture2DShader );
    }
}

const bool OsgWidgetStreamHelper::getUseTextureRect() const
{
    return useTextureRect;
}

void OsgWidgetStreamHelper::setUseTextureRect( bool useTextureRect )
{
    this->useTextureRect = useTextureRect;
}

video::StreamOsgWidget* OsgWidgetStreamHelper::createWidget( osg::Image* image )
{
    // tworzymy kontrolkê
    video::StreamOsgWidget* widget = new video::StreamOsgWidget(image->getFileName());
    osg::Uniform* textureSize = NULL;

    // czy wymuszamy shader?
    vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image);
    if (stream && stream->getTargetFormat() == vm::PixelFormatYV12) {
        osg::StateSet* state = widget->getOrCreateStateSet();
        state->addUniform(new osg::Uniform("movie_texture",0));
        state->addUniform(textureSize = new osg::Uniform("texture_size", 0, 0));
        state->setAttribute(yuvProgram);
        widget->setImage(image, true, useTextureRect);
        widget->setTextureSize(textureSize);
    } else {
        widget->setImage(image, true, useTextureRect);
    }

    // czy trzeba zrobiæ flipa?
    if (image->getOrigin()==osg::Image::TOP_LEFT) {
        osgWidget::TexCoord temp = widget->getTexCoord(osgWidget::Widget::UPPER_LEFT);
        widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_LEFT), osgWidget::Widget::UPPER_LEFT);
        widget->setTexCoord(temp, osgWidget::Widget::LOWER_LEFT);
        temp = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
        widget->setTexCoord(widget->getTexCoord(osgWidget::Widget::LOWER_RIGHT), osgWidget::Widget::UPPER_RIGHT);
        widget->setTexCoord(temp, osgWidget::Widget::LOWER_RIGHT);
    }

    return widget;
}

void OsgWidgetStreamHelper::setFormat( vm::PixelFormat format )
{

}