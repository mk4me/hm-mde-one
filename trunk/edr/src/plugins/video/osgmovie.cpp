#include "VideoPCH.h"
/* OpenSceneGraph example, osgmovie.
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/
/*
#include <iostream>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgDB/ReadFile>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/StateSet>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/TextureRectangle>
#include <osg/TextureCubeMap>
#include <osg/TexMat>
#include <osg/CullFace>
#include <osg/ImageStream>
#include <osg/io_utils>
#include <osg/Timer>
#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>
#include <osgViewer/ViewerEventHandlers>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/WindowManager>
#include <osgText/Text>
#include <osg/GLExtensions>

#include <utils/ObserverPattern.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/PtrWrapper.h>

#include "Callback.h"
#include "HorizontalScrollBar.h"
#include "Potentiometer.h"
#include "Model.h"
#include "TimelineImageStreamWrapper.h"
#include "Controller.h"
#include "View.h"
#include "TimelineViewOSG.h"
#include "StreamsViewOSGWidget.h"

#include "tm.h"

#include "core/VM.h"
#include "osg/VideoImageStream.h"
#include "StaticKeyboardHandler.h"
#include <utils/Profiler.h>
#include "VideoWidget.h"


using namespace timeline;

USE_OSGPLUGIN(VideoManager);



//! Pobiera rozmiar okna. Jeœli viewer jest oknie jest to rozmiar okna,
//! w przeciwnym wypadku jest to rozmiar pierwszego ekranu
//! \param viewer
//! \param width
//! \param height
bool getViewSize( osgViewer::Viewer &viewer, unsigned int &width, unsigned int &height )
{
  if (viewer.getCamera()->getGraphicsContext() == NULL) {
    // skoro jeszcze nie ma kontekstu to ustawiamy go na ca³y ekran
    viewer.setUpViewOnSingleScreen();
  }
  // jest, pobieramy
  width = viewer.getCamera()->getGraphicsContext()->getTraits()->width;
  height = viewer.getCamera()->getGraphicsContext()->getTraits()->height;

  return true;
}

void configureView(bool vertically, bool horizontally,
                                std::vector<osg::Image*> &images, std::vector<osg::ImageStream*> &streams, 
                                osgViewer::Viewer &viewer, bool textureRect)
{
    unsigned int viewerWidth = viewer.getCamera()->getGraphicsContext()->getTraits()->width;
    unsigned int viewerHeight = viewer.getCamera()->getGraphicsContext()->getTraits()->height;

    // tworzymy korzeñ
    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->getOrCreateStateSet()->setMode(GL_LIGHTING,osg::StateAttribute::OFF);
    // tworzymy wêze³ filmów
    osg::ref_ptr<osg::Geode> imagesNode = new osg::Geode;

    // okreœlamy liczbê kolumn i wierszy
    unsigned int rows = 0;
    unsigned int columns = 0;
    if ( !vertically && !horizontally ) {
        columns = static_cast<unsigned int>(ceil(sqrt(static_cast<double>(images.size()))));
        if ( columns != 0 ) {
            if ( columns * (columns-1) >= images.size() ) {
                rows = columns-1;
            } else {
                rows = columns;
            }
        }
    } else if ( vertically ) {
        rows = images.size();
        columns = 1;
    } else if ( horizontally ) {
        columns = images.size();
        rows = 1;
    }

    // przygotowanie widoków strumieni
    StreamViewOSGWidget * streamView = new StreamViewOSGWidget(&viewer,
        textureRect, viewerWidth, viewerHeight, 0x1234, 0);
    streamView->setDimensions(rows, columns);

    // tworzymy geometriê
    for ( unsigned int y = 0; y < rows; ++y ) {
        for ( unsigned int x = 0; x < columns; ++x ) {
            // indeks obrazka
            size_t idx = x + y * columns;
            if ( idx < images.size() ) {
                osg::Image* image = images[idx];
                streamView->addStream(image, y, x);
            }
        }
    }

    // kontroler
    Controller * controller = new Controller();
    for (  std::vector<osg::ImageStream*>::iterator it = streams.begin(); it != streams.end(); ++it ) {
        controller->getModel()->addStream(  *it );
    }
    // widok
    TimelineViewOSG * view = new TimelineViewOSG(&viewer, viewerWidth, viewerHeight, 0x1234, 0);
    view->setController(controller);
    // model
    controller->getModel()->attach(view);

    osg::Camera * guiCamera = view->createParentOrthoCamera();
    guiCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    guiCamera->setRenderOrder(osg::Camera::POST_RENDER, 2);

    osg::Camera * streamViewCamera = streamView->createParentOrthoCamera();
    streamViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    streamViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);

    //root->addSlave(textCamera);
    root->addChild(imagesNode);
    root->addChild(guiCamera);
    root->addChild(streamViewCamera);


    // dodanie dwóch takich handlerów powoduje b³êdy!
    viewer.addEventHandler( new osgWidget::MouseHandler(streamView) );
    viewer.addEventHandler( new osgui::StaticKeyboardHandler(streamView) );
    viewer.addEventHandler( new osgViewer::StatsHandler );
    viewer.addEventHandler( new osgGA::StateSetManipulator( viewer.getCamera()->getOrCreateStateSet() ) );
    viewer.addEventHandler( new osgViewer::WindowSizeHandler );
    viewer.addEventHandler( new osgWidget::ResizeHandler(view, guiCamera) );
    viewer.addEventHandler( new osgWidget::ResizeHandler(streamView, streamViewCamera) );

    viewer.setSceneData(root.get());

    viewer.getCamera()->setClearColor(osg::Vec4(0,0,0,1));
}

//------------------------------------------------------------------------------

int main(int argc, char** argv)
{
  UTILS_PROFILER_THREAD_SCOPE;
  utils::Profiler::setEnabled(true);
  
 
  // parsowanie parametrów
  osg::ArgumentParser arguments(&argc,argv);
  arguments.getApplicationUsage()->setApplicationName(arguments.getApplicationName());
  arguments.getApplicationUsage()->setDescription(arguments.getApplicationName()+" example usage of Timeline Manager.");
  arguments.getApplicationUsage()->setCommandLineUsage(arguments.getApplicationName()+" [options] filename ...");
  arguments.getApplicationUsage()->addCommandLineOption("-h or --help","Display this information");
  arguments.getApplicationUsage()->addCommandLineOption("--vertical","Stacks streams vertically");
  arguments.getApplicationUsage()->addCommandLineOption("--horizontal","Stacks stream horizontally");
  arguments.getApplicationUsage()->addCommandLineOption("--texture2D","Use Texture2D rather than TextureRectangle");
  arguments.getApplicationUsage()->addCommandLineOption("--buffered","Buffer stream.");
  arguments.getApplicationUsage()->addCommandLineOption("--pixelFormat","Pixel format {rgb | yuv | bgra}.");

  bool vertically = arguments.read("--vertical");
  bool horizontally = arguments.read("--horizontal");
  bool textureRect = !arguments.read("--texture2D");
  bool buffered = arguments.read("--buffered");

  std::string name = arguments.getApplicationName();

  
  vm::VideoManager::getInstance()->setEnableBuffering( buffered );
  std::string format;
  if ( arguments.read("--pixelFormat", format) ) {
    if ( format == "rgb" ) {
      vm::VideoManager::getInstance()->setPrefferedFormat(vm::PixelFormatRGB24);
    } else if ( format == "yuv" ) {
      vm::VideoManager::getInstance()->setPrefferedFormat(vm::PixelFormatYV12);
    } else if ( format == "bgra" ) {
      vm::VideoManager::getInstance()->setPrefferedFormat(vm::PixelFormatBGRA);
    } else {
      arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
      return 1;
    }
  }

  // czy s¹ argumenty?
  if (arguments.argc()<=1) {
    arguments.getApplicationUsage()->write(std::cout,osg::ApplicationUsage::COMMAND_LINE_OPTION);
    return 1;
  }
  // czy wyœwietlamy pomoc?
  if (arguments.read("-h") || arguments.read("--help")) {
    arguments.getApplicationUsage()->write(std::cout);
    return 1;
  }

  // tworzymy viewera
  osgViewer::Viewer viewer(arguments);
  // ustalamy rozmiar ekranu
  unsigned int viewerWidth, viewerHeight;
  if (!getViewSize(viewer, viewerWidth, viewerHeight)) {
    return 1;
  }

  // teraz na pewno mamy kontekst, mo¿na sprawdziæ mo¿liwoœci
  osg::GraphicsContext* context = viewer.getCamera()->getGraphicsContext();
  context->realize();
  context->makeCurrent();
  if (textureRect) {
    if (!OSG_GL3_FEATURES && !osg::isGLExtensionSupported(context->getState()->getContextID(), "GL_ARB_texture_rectangle")) {
      OSG_WARN<<"TextureRect not available on current platform, using Texture2D instead."<<std::endl;
      textureRect = false;
    }
  }
  context->releaseContext();

  float screenAspectRatio = (float)viewerWidth/viewerHeight;

  // bufory
  std::vector<osg::ImageStream*> streams;
  std::vector<osg::Image*> images;
  // odczytujemy zadane obrazki
  for (int i=1;i<arguments.argc();++i) {
    if (arguments.isString(i)) {
      osg::Image* image = osgDB::readImageFile(arguments[i]);
      if ( image != NULL ) {
        images.push_back(image);
      }
      // czy to film?
      osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(image);
      if ( imageStream != NULL ) {
        streams.push_back(imageStream);
      }
    }
  }


  // czy s¹ obrazki?
  if (images.size() == 0) {
    arguments.getApplicationUsage()->write(std::cout);
    return 1;
  }
  // czy by³y b³êdy?
  if (arguments.errors()) {
    arguments.writeErrorMessages(std::cout);
    return 1;
  }

  configureView(vertically, horizontally, images, streams, viewer, textureRect );

  
  viewer.setThreadingModel(osgViewer::ViewerBase::DrawThreadPerContext);
  viewer.run();

  // zwolnienie
  vm::VideoManager::destroyInstance();

  return 0;
}*/