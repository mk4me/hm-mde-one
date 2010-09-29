#include "VideoPCH.h"
#include "VideoWidget.h"


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


#ifdef _DEBUG
#define WM_FLAGS 0//osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif




VideoWidget::VideoWidget()
{
  setupUi(this); 
  viewer = new QOSGViewer(this, "OsgViewer");

  vm::VideoManager::getInstance()->setEnableBuffering( false );

  // bufory
  std::vector<osg::ImageStream*> streams;
  std::vector<osg::Image*> images;
  const char* files[] = { "s1.avi", "s1.avi", "s1.avi", "s1.avi" };
  for (size_t i = 0; i < utils::length(files); ++i) {
    osg::Image* image = osgDB::readImageFile(files[i]);
    if ( image != NULL ) {
      images.push_back(image);
    }
    // czy to film?
    osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(image);
    if ( imageStream != NULL ) {
      streams.push_back(imageStream);
    }
  }

  configureView(false, false, images, streams, *viewer, true );
  GLWidget->addWidget( viewer );
  
}


void VideoWidget::configureView(bool vertically, bool horizontally,
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
  StreamViewOSGWidget * streamView = new StreamViewOSGWidget(&viewer, rows, columns, 
    textureRect, viewerWidth, viewerHeight, 0x1234, WM_FLAGS);

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
  //// widok
  //TimelineViewOSG * view = new TimelineViewOSG(&viewer, viewerWidth, viewerHeight, 0x1234, WM_FLAGS);
  //view->setController(controller);
  //// model
  //controller->getModel()->attach(view);

  //osg::Camera * guiCamera = view->createParentOrthoCamera();
  //guiCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
  //guiCamera->setRenderOrder(osg::Camera::POST_RENDER, 2);

  osg::Camera * streamViewCamera = streamView->createParentOrthoCamera();
  streamViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
  streamViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);

  //root->addSlave(textCamera);
  root->addChild(imagesNode);
  //root->addChild(guiCamera);
  root->addChild(streamViewCamera);


  // dodanie dwóch takich handlerów powoduje b³êdy!
  viewer.addEventHandler( new osgWidget::MouseHandler(streamView) );
  viewer.addEventHandler( new osgUI::StaticKeyboardHandler(streamView) );
  viewer.addEventHandler( new osgViewer::StatsHandler );
  viewer.addEventHandler( new osgGA::StateSetManipulator( viewer.getCamera()->getOrCreateStateSet() ) );
  viewer.addEventHandler( new osgViewer::WindowSizeHandler );
  //viewer.addEventHandler( new osgWidget::ResizeHandler(view, guiCamera) );
  viewer.addEventHandler( new osgWidget::ResizeHandler(streamView, streamViewCamera) );

  viewer.setSceneData(root.get());
}