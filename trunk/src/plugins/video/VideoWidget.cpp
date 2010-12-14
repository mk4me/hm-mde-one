#include "VideoPCH.h"
#include "VideoWidget.h"

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
    streamHelper = new OsgWidgetStreamHelper( "data/resources/shaders/", textureRectangleCheck->isChecked() );

    // tworzenie viewera
    viewer = new QOSGViewer(this, "OsgViewer");
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // tworzymy multi widok
    multiView = new core::MultiView(viewer, traits->width, traits->height, 0xF0000000, WM_FLAGS);
    multiView->addChild(new osgWidget::Box("HACK"));

    // stworzenie kamery
    osg::Camera* multiViewCamera = multiView->createParentOrthoCamera();
    multiViewCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
    multiViewCamera->setRenderOrder(osg::Camera::POST_RENDER, 1);
    multiViewCamera->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

    viewer->addEventHandler( new osgWidget::MouseHandler(multiView) );
    viewer->addEventHandler( new osgUI::StaticKeyboardHandler(multiView) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(multiView, multiViewCamera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );

    //viewer->addEventHandler( new osgViewer::WindowSizeHandler );
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

    osg::ref_ptr<osg::Group> root = new osg::Group();
    root->addChild(multiViewCamera);
    viewer->setSceneData(root);
    
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
    images.clear();
    std::vector<osg::ImageStream*> streams;
    for (size_t i = 0; i < files.size(); ++i) {
        osg::Image* image = osgDB::readImageFile(files[i]);
        if ( image != NULL ) {
            images.push_back(image);

//             std::ostringstream out;
//             out << i << ": " << files[i];
//             this->displayTypeCombo->addItem( QString::fromStdString(out.str()) );

        }
        // czy to film?
        osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(image);
        if ( imageStream != NULL ) {
            streams.push_back(imageStream);
        }
    }

    

    //int rows, columns;
    //view->makeGrid(static_cast<int>(images.size()), rows, columns);
    //configureView(rows, columns, images);
    float avgRatio = 0;
    for (ImagesList::iterator it = images.begin(); it != images.end(); ++it) {
        osg::Image* image = *it;
        float ratio = image->getPixelAspectRatio() * image->s() / image->t();
        avgRatio += ratio;

        osgWidget::Box* box1 = new osgWidget::Box(image->getFileName());
        video::StreamOsgWidget* aaa1 = streamHelper->createWidget( image );
        //osgWidget::Widget *aaa1 = new osgWidget::Widget("aaa", 100, 100);
        //aaa1->setColor(  osgWidget::Color(1,1,1,1) * std::distance(images.begin(), it) / 4 );
        aaa1->setCanFill(true);
        box1->addWidget(aaa1);
        multiView->addChild(box1);
        osgWidget::Box* clon = new osgWidget::Box(image->getFileName());

        video::StreamOsgWidget* widget = streamHelper->createWidget( osg::clone(image) );
        
        osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, ratio);
        keeper->setColor(0,0,0,0);
        clon->getBackground()->setColor(0,0,0,0);



        clon->addWidget(keeper);
        multiView->addChild(clon);
        multiView->addItem(new core::MultiViewWidgetItem(box1, ratio), new core::MultiViewWidgetItem(clon, ratio));
    }


    // stworzenie widoku z girdem
    avgRatio /= images.size();
    unsigned rows;
    unsigned columns = static_cast<unsigned>(ceil(sqrt(static_cast<double>(images.size()))));
    if ( columns != 0 ) {
        if ( columns * (columns-1) >= images.size() ) {
            rows = columns-1;
        } else {
            rows = columns;
        }
    } else {
        rows = 0;
    }

    // TODO: kopia
    osgUI::Grid* gridThumbs = new osgUI::Grid("all", rows, columns);
    gridThumbs->getBackground()->setColor(0,0,0,0);
    osgUI::Grid* grid = new osgUI::Grid("all", rows, columns);
    grid->getBackground()->setColor(0,0,0,0);
    //osgUI::Grid* grid = osg::clone(gridThumbs, osg::CopyOp::DEEP_COPY_ALL);
    for (unsigned row = 0; row < rows; ++row) {
        for (unsigned col = 0; col < columns; ++col) {
            osg::Image* image = images[ row * columns + col ];
            video::StreamOsgWidget* thumb = streamHelper->createWidget( osg::clone(image) );
            thumb->setCanFill(true);
            gridThumbs->addWidget(thumb, row, col);
            video::StreamOsgWidget* widget = streamHelper->createWidget( osg::clone(image) );
            osgUI::AspectRatioKeeper* keeper = new osgUI::AspectRatioKeeper(widget, avgRatio);
            grid->addWidget(keeper, row, col);
        }
    }
    multiView->addChild(gridThumbs);
    multiView->addChild(grid);
    multiView->addItem(new core::MultiViewWidgetItem(gridThumbs, avgRatio), new core::MultiViewWidgetItem(grid, avgRatio));


}

void VideoWidget::outputFormatChanged( int index )
{
    //// TODO
    //// dodaæ sta³e albo rêcznie wype³niaæ
    //if ( index == 0 ) {
    //    // yuv
    //    view->setFormat(vm::PixelFormatYV12);
    //} else if ( index == 1 ) {
    //    view->setFormat(vm::PixelFormatRGB24);
    //} else {
    //    view->setFormat(vm::PixelFormatBGRA);
    //}
}

void VideoWidget::textureRectangleChecked( int checked )
{
    //view->clear();
    //view->setUseTextureRect( checked != 0 );
    //configureView(last.rows, last.columns, last.images);
}