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


using namespace timeline;


#ifdef _DEBUG
#define WM_FLAGS 0//osgWidget::WindowManager::WM_PICK_DEBUG
#else
#define WM_FLAGS 0
#endif



VideoWidget::VideoWidget()
:   viewType(ViewTypeGrid)
{
    last.rows = last.columns = 0;

    // inicjalizacja UI
    setupUi(this); 


    // TODO: do wyrzucenia!
    vm::VideoManager::getInstance()->setEnableBuffering( false );
    vm::VideoManager::getInstance()->setPrefferedFormat( vm::PixelFormatYV12);
    //connect(displayTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(displayTypeChanged()));

    // tworzenie viewera
    viewer = new QOSGViewer(this, "OsgViewer");
    const osg::GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    //viewer->getCamera()->setViewport(50, 50, traits->width / 2, traits->height / 2);

    // przygotowanie widoków strumieni
    view = new StreamViewOSGWidget(viewer, true, 
        traits->width,
        traits->height, 
        0x1234, WM_FLAGS);

    textureRectangleCheck->setCheckState( view->getUseTextureRect() ? Qt::Checked : Qt::Unchecked );

    // kamera
    osg::Camera * camera = view->createParentOrthoCamera();
    camera->setClearMask(GL_DEPTH_BUFFER_BIT);
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    viewer->setSceneData(camera);

    // podpiêcie handlerów
    viewer->addEventHandler( new osgWidget::MouseHandler(view) );
    viewer->addEventHandler( new osgUI::StaticKeyboardHandler(view) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(view, camera) );
    viewer->addEventHandler( new osgViewer::StatsHandler );
    viewer->addEventHandler( new osgGA::StateSetManipulator( viewer->getCamera()->getOrCreateStateSet() ) );
    //viewer->addEventHandler( new osgViewer::WindowSizeHandler );
    viewer->getCamera()->setClearColor(osg::Vec4(0.73f, 0.73f, 0.73f, 1));

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
    view->clear();

//   // okreœlamy liczbê kolumn i wierszy
//   unsigned int rows = 0;
//   unsigned int columns = 0;
//   if ( !vertically && !horizontally ) {
//     columns = static_cast<unsigned int>(ceil(sqrt(static_cast<double>(images.size()))));
//     if ( columns != 0 ) {
//       if ( columns * (columns-1) >= images.size() ) {
//         rows = columns-1;
//       } else {
//         rows = columns;
//       }
//     }
//   } else if ( vertically ) {
//     rows = images.size();
//     columns = 1;
//   } else if ( horizontally ) {
//     columns = images.size();
//     rows = 1;
//   }

  view->setDimensions(rows, columns);

  // tworzymy geometriê
  for ( int y = 0; y < rows; ++y ) {
    for ( int x = 0; x < columns; ++x ) {
      // indeks obrazka
      size_t idx = x + y * columns;
      if ( idx < images.size() ) {
        osg::Image* image = images[idx];
        view->addStream(image, y, x);
      }
    }
  }

  last.images = images;
  last.columns = columns;
  last.rows = rows;

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

            std::ostringstream out;
            out << i << ": " << files[i];
            this->displayTypeCombo->addItem( QString::fromStdString(out.str()) );

        }
        // czy to film?
        osg::ImageStream* imageStream = dynamic_cast<osg::ImageStream*>(image);
        if ( imageStream != NULL ) {
            streams.push_back(imageStream);
        }
    }

    int rows, columns;
    view->makeGrid(static_cast<int>(images.size()), rows, columns);
    configureView(rows, columns, images);
}


VideoWidget::ViewType VideoWidget::getViewType( int * selected /*= NULL*/ ) const
{
    if ( (viewType & ViewTypeSingle) == ViewTypeSingle ) {
        if ( selected ) {
            *selected = (((ViewTypeSingleIdxMask) & int(viewType)) >> ViewTypeSingleIdxShift);
        }
        return ViewTypeSingle;
    } else {
        return viewType;
    }
}

void VideoWidget::setViewType( ViewType viewType, int selected /*= 0*/ )
{
    this->viewType = viewType; 
    if ( viewType == ViewTypeSingle ) {
        int idx = (ViewTypeSingleIdxMask & (selected << ViewTypeSingleIdxShift));
        reinterpret_cast<int&>(viewType) |= idx;
    }
}

void VideoWidget::displayTypeChanged( int index )
{
    // TODO
    // dodaæ sta³e albo rêcznie wype³niaæ
    int count = static_cast<int>(images.size());
    if (index == 0) {
        // grid
        int rows, columns;
        view->makeGrid(count, rows, columns);
        configureView(rows, columns, images);
    } else if ( index == 1 ) {
        // horizontally
        configureView(1, count, images);
    } else if ( index == 2 ) {
        // vertically 
        configureView(count, 1, images);
    } else {
        // single
        index -= 3;
        ImagesList selected;
        if ( static_cast<size_t>(index) < images.size() ) {
            selected.push_back( images[index] );
        }
        configureView(1, 1, selected);
    }
}

void VideoWidget::outputFormatChanged( int index )
{
    // TODO
    // dodaæ sta³e albo rêcznie wype³niaæ
    if ( index == 0 ) {
        // yuv
        view->setFormat(vm::PixelFormatYV12);
    } else if ( index == 1 ) {
        view->setFormat(vm::PixelFormatRGB24);
    } else {
        view->setFormat(vm::PixelFormatBGRA);
    }
}

void VideoWidget::textureRectangleChecked( int checked )
{
    
    view->clear();
    view->setUseTextureRect( checked != 0 );
    configureView(last.rows, last.columns, last.images);
}