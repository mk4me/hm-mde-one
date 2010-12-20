#include "VideoPCH.h"
#include "VideoWidget.h"

#include <boost/foreach.hpp>
#include <osg/GraphicsContext>

#include <utils/ObserverPattern.h>
#include <utils/PtrPolicyOSG.h>
#include <utils/PtrWrapper.h>

#include "Callback.h"
#include "HorizontalScrollBar.h"
#include "Potentiometer.h"
#include "StreamsViewOSGWidget.h"

#include "tm.h"

#include "core/VM.h"
#include "osg/VideoImageStream.h"
#include "StaticKeyboardHandler.h"
#include <utils/Profiler.h>
#include "VideoWidget.h"
#include "ImageCanvas.h"
#include <core/MultiView.h>
#include <core/AspectRatioKeeper.h>
#include <core/MultiViewWidgetItem.h>
#include <core/OsgSceneDump.h>
#include <core/Log.h>


using namespace timeline;


#ifdef _DEBUG
#define WM_FLAGS 0//osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif

VideoWidget::VideoWidget()
{
    // inicjalizacja UI
    setupUi(this); 

    // TODO: do wyrzucenia!
    vm::VideoManager::getInstance()->setEnableBuffering( false );
    vm::VideoManager::getInstance()->setPrefferedFormat( vm::PixelFormatYV12);

    // stworzenie helpera
    streamHelper = new OsgWidgetStreamHelper( "data/resources/shaders/", true );

    // tworzenie viewera
    viewer = new QOSGViewer(this, "OsgViewer");
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
    viewer->addEventHandler( new osgUI::StaticKeyboardHandler(multiView) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    // ustawienie sceny
    viewer->setSceneData(multiViewCamera);
    
    // dodanie do widgeta
    GLWidget->addWidget( viewer );

  // bufory

//   std::vector<std::string> files;
//   files.push_back("s1.avi");
//   files.push_back("s1.avi");
//   files.push_back("s1.avi");
//   files.push_back("s1.avi");
// 
//   init(files);
}


void VideoWidget::configureView(int rows, int columns, ImagesList& images)
{
    return;
//    view->clear();
//
////   // okreœlamy liczbê kolumn i wierszy
////   unsigned int rows = 0;
////   unsigned int columns = 0;
////   if ( !vertically && !horizontally ) {
////     columns = static_cast<unsigned int>(ceil(sqrt(static_cast<double>(images.size()))));
////     if ( columns != 0 ) {
////       if ( columns * (columns-1) >= images.size() ) {
////         rows = columns-1;
////       } else {
////         rows = columns;
////       }
////     }
////   } else if ( vertically ) {
////     rows = images.size();
////     columns = 1;
////   } else if ( horizontally ) {
////     columns = images.size();
////     rows = 1;
////   }
//
//  view->setDimensions(rows, columns);
//
//  // tworzymy geometriê
//  for ( int y = 0; y < rows; ++y ) {
//    for ( int x = 0; x < columns; ++x ) {
//      // indeks obrazka
//      size_t idx = x + y * columns;
//      if ( idx < images.size() ) {
//        osg::Image* image = images[idx];
//        view->addStream(image, y, x);
//      }
//    }
//  }
//
//  last.images = images;
//  last.columns = columns;
//  last.rows = rows;

//   // kontroler
//   Controller * controller = new Controller();
//   for (  std::vector<osg::ImageStream*>::iterator it = streams.begin(); it != streams.end(); ++it ) {
//     controller->getModel()->addStream(  *it );
//   }  
}

void VideoWidget::init( std::vector<std::string> &files )
{
    // odczytanie plików
    images.clear();
    BOOST_FOREACH(const std::string& file, files) {
        if ( osg::Image* image = osgDB::readImageFile(file) ) {
            images.push_back(image);
        } else {
            LOG_ERROR<<"VideoService: "<<file<<" could not be read.";
        }
    }
   
    createScene();
}

void VideoWidget::setPixelFormat( vm::PixelFormat format )
{
    // ustawienie formatu strumieni
    BOOST_FOREACH( osg::Image* image, images ) {
        if ( vmOSGPlugin::VideoImageStream* stream = dynamic_cast<vmOSGPlugin::VideoImageStream*>(image) ) {
            stream->setTargetFormat(format);
        }
    }
    createScene();
}

void VideoWidget::setUseTextureRect( bool useTextureRect )
{
    streamHelper->setUseTextureRect( useTextureRect );
    createScene();
}

void VideoWidget::createScene()
{
    unsigned sel = multiView->getSelectedIndex();
    bool doSelect = ( sel != multiView->getNumItems() );

    // na wszelki wypadek czyœcimy scenê
    clearScene();

    // stworzenie widgetów i dodanie ich do multi widoku
    float avgRatio = 0;
    BOOST_FOREACH(osg::Image* image, images) {
        // wyliczenie wspó³czynnika proporcji oraz aktualizacja zbiorczego wspó³czynnika
        float ratio = image->getPixelAspectRatio() * image->s() / image->t();
        avgRatio += ratio;

        // faktyczne dodanie miniaturki do grida
        osgWidget::Box* thumbnail = new osgWidget::Box(image->getFileName() + "thumbnail");
        video::StreamOsgWidget* streamWidget = streamHelper->createWidget( image );
        streamWidget->setCanFill(true);
        thumbnail->addWidget(streamWidget);
        multiView->addChild(thumbnail);

        // faktyczne dodanie du¿ego okna
        osgWidget::Box* preview = new osgWidget::Box(image->getFileName());
        video::StreamOsgWidget* streamWidgetClone = streamHelper->createWidget( osg::clone(image) );
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(streamWidgetClone, ratio);
        keeper->setColor(0, 0, 0, 0);
        preview->getBackground()->setColor(0,0,0,0);
        preview->addWidget(keeper);
        multiView->addChild(preview);

        // dodanie itemów do multiviewa
        multiView->addItem(new core::MultiViewWidgetItem(thumbnail, ratio), new core::MultiViewWidgetItem(preview, ratio));
    }

    // obliczenie rozmiaru grida (jak najbardziej kwadratowy)
    avgRatio /= images.size();
    osg::Vec2s dimensions = osgUI::Grid::getDimensionsAsSquare(images.size());
    unsigned rows = dimensions.y();
    unsigned columns = dimensions.x();

    // TODO: kopia
    osgUI::Grid* gridThumbs = new osgUI::Grid("allThumbnails", rows, columns);
    gridThumbs->getBackground()->setColor(0,0,0,0);
    osgUI::Grid* grid = new osgUI::Grid("all", rows, columns);
    grid->getBackground()->setColor(0,0,0,0);
    //osgUI::Grid* grid = osg::clone(gridThumbs, osg::CopyOp::DEEP_COPY_ALL);
    for (unsigned row = 0; row < rows; ++row) {
        for (unsigned col = 0; col < columns; ++col) {
            osg::Image* image = images[ row * columns + col ];
            video::StreamOsgWidget* thumbnail = streamHelper->createWidget( osg::clone(image) );
            thumbnail->setCanFill(true);
            gridThumbs->addWidget(thumbnail, row, col);
            video::StreamOsgWidget* preview = streamHelper->createWidget( osg::clone(image) );
            osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(preview, avgRatio);
            grid->addWidget(keeper, row, col);
        }
    }
    multiView->addChild(gridThumbs);
    multiView->addChild(grid);
    multiView->addItem(new core::MultiViewWidgetItem(gridThumbs, avgRatio), new core::MultiViewWidgetItem(grid, avgRatio));

    if ( doSelect ) {
        multiView->setSelectedByIndex(sel);
    }
}

void VideoWidget::clearScene()
{
    // usuniêcie itemów
    multiView->removeAllItems();
    // usuniêcie wszystkich wêz³ów (!)
    multiView->removeChildren(0, multiView->getNumChildren());
    // reinicjalizacja multi view (po usunieciu wszystkich wêz³ów - wymagana)
    multiView->restoreRequiredChildren();

    multiView->addChild(new osgWidget::Box("HACK"));
}