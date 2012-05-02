#include "VideoPCH.h"
#include "VideoVisualizer.h"
#include <plugins/video/Wrappers.h>
#include <vidlib/osg/VideoImageStream.h>
#include <osgui/EventCallback.h>
#include <osgui/QOsgWidgets.h>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Box>
#include <osgui/AspectRatioKeeper.h>
#include <core/StringTools.h>

#ifdef _DEBUG

#include <core/MultiViewWidgetItem.h>

#endif

using namespace core;

struct VideoVisualizer::Refresher
{
    VideoVisualizer* visualizer;
    void operator()(osg::Node* node, const osgGA::GUIEventAdapter* event) 
    {
        UTILS_ASSERT(visualizer);
        visualizer->refresh( static_cast<float>(event->getWindowWidth()), static_cast<float>(event->getWindowHeight()) );
    }
};

struct VideoVisualizer::WidgetUpdater : public osg::Drawable::UpdateCallback
{
    VideoVisualizer* visualizer;

    WidgetUpdater(VideoVisualizer* visualizer = nullptr) :
    visualizer(visualizer)
    {

    }

    virtual void update(osg::NodeVisitor* nv, osg::Drawable* drawable)
    {
        UTILS_ASSERT(visualizer);
        visualizer->updateWidget();
    }
};




VideoVisualizer::VideoVisualizer() :
name("Video"), prevStreamTime(-1), currentStreamTime(-1), prevStreamWidth(-1), useTextureRect(true)
{

}

VideoVisualizer::~VideoVisualizer()
{
	/*streamImage = nullptr;
	stream = nullptr;*/
    viewer = nullptr;
}

const std::string& VideoVisualizer::getName() const
{
    return name;
}

core::IVisualizer* VideoVisualizer::createClone() const
{
    return new VideoVisualizer();
}

void VideoVisualizer::getInputInfo( std::vector<core::IInputDescription::InputInfo>& info )
{
    core::IInputDescription::InputInfo input;

    input.name = "videoStream";
    input.type = typeid(VideoStream);
	input.type = typeid(VideoChannel);
    input.modify = false;
    input.required = false;

    info.push_back(input);

    input.name = "picture";
    input.type = typeid(vidlib::Picture);

    info.push_back(input);    
}

void VideoVisualizer::refresh( float width, float height )
{
    // aktualizacja wsp�czynnika proporcji
    if ( streamImage ) {
        ratioKeeper->setAspectRatio( streamImage->getPixelAspectRatio() * streamImage->s() / streamImage->t() );
		//ratioKeeper->setAspectRatio( width / height );

    }

    // aktualizacja rozmiaru t�a
    workspace->setOrigin(0, 0);
    workspace->resize(width , height);
    workspace->update();
    // aktualizacja tekstury t�a
    //workspace->getBackground()->setTexCoordRegion(0, height, width, -height);
	workspace->getBackground()->setTexCoordRegion(0, 0, width, height);
	
    // aktualizacja rozmiaru tekstury
    if ( streamImage ) {
        // od�wie�enia danych zale�nych od obrazka
        // resize obrazka
        streamImage->setMaxWidth(widget, widget->getWidth());
		if(width > 0 && height > 0){
			workspace->resizeAdd();
		}
    }
}

void VideoVisualizer::updateWidget()
{
    // korekcja wsp�rz�dnych tekstury
    if ( streamImage ) {
        if ( useTextureRect ) {
            // sprawdzamy, czy trzeba od�wie�y� wsp�rz�dne tekstury
            float s = static_cast<float>(streamImage->s());
            float t = static_cast<float>(streamImage->t());
            // 
            auto ll = widget->getTexCoord(osgWidget::Widget::LOWER_LEFT);
            auto ur = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
            float oldS = fabs( ll.x() - ur.x() );
            float oldT = fabs( ll.y() - ur.y() );

			float h = widget->getHeight();
			float w = widget->getWidth();

			float wR = w / h;
			float tR = s / t;

			float mul = wR / tR;

			float nextS = s;
			float nextT = t;
			if (mul > 1.0f) {
				nextT = s * h / w;
			} else { 
				nextS = w * t / h;
			}
			
			float deltaS = (s - nextS) / 2.0f;
			float deltaT = (t - nextT) / 2.0f;

            // czy trzeba aktualizowa�?
            if ( oldS != s || oldT != t ) {
                //widget->setTexCoord(0 + deltaS, t - deltaT, osgWidget::Widget::UPPER_LEFT);
                //widget->setTexCoord(0 + deltaS, 0 + deltaT, osgWidget::Widget::LOWER_LEFT);
                //widget->setTexCoord(s - deltaS, t - deltaT, osgWidget::Widget::UPPER_RIGHT);
                //widget->setTexCoord(s - deltaS, 0 + deltaT, osgWidget::Widget::LOWER_RIGHT);
                widget->setTexCoord(0, t , osgWidget::Widget::UPPER_LEFT);
                widget->setTexCoord(0, 0 , osgWidget::Widget::LOWER_LEFT);
                widget->setTexCoord(s, t , osgWidget::Widget::UPPER_RIGHT);
                widget->setTexCoord(s, 0 , osgWidget::Widget::LOWER_RIGHT);
              osgui::correctTexCoords(widget);
            }
        }
    }
}

void VideoVisualizer::update( double deltaTime )
{
	bool needsUpdate = false;

	if ( currentStreamTime != prevStreamTime ) {
		prevStreamTime = currentStreamTime;
		needsUpdate = true;

		stream->setTime(currentStreamTime);
		getImage();
	}

    if ( streamImage ) {

		refreshImage();

        int width = streamImage->s();
        
        if ( prevStreamWidth != width ) {
            prevStreamWidth = width;
            needsUpdate = true;
        }    
    }

	if ( needsUpdate ) {
		viewer->update();
	}
}

bool VideoVisualizer::getImage()
{
	streamImage = stream->getImage(vidlib::PixelFormatRGB24);
	return streamImage != nullptr;
}

void VideoVisualizer::refreshImage()
{
	widget->setTexture( stream->getTexture(streamImage->getFormat(), useTextureRect), true, useTextureRect );
	osgui::correctTexCoords(widget, streamImage);
	refresh(viewer->width(), viewer->height());
}

QWidget* VideoVisualizer::createWidget(core::IActionsGroupManager * manager)
{
    using namespace osg;
    using namespace osgWidget;
    using namespace osgui;

    viewer = new QOsgDefaultWidget();
    viewer->setTimerActive(false);

    // pobranie cech kontekstu graficznego
    const GraphicsContext::Traits* traits = viewer->getCamera()->getGraphicsContext()->getTraits();

    // konfiguracja kamery
    Camera* camera = viewer->getCamera();
    camera->getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::PROTECTED | StateAttribute::OFF);
    camera->setProjectionMatrix(Matrix::ortho2D(0.0, double(traits->width), 0.0f, double(traits->height)));
    camera->setReferenceFrame(Transform::ABSOLUTE_RF);
    camera->setViewMatrix(Matrix::identity());
    camera->setClearColor(Vec4(0.73f, 0.73f, 0.73f, 1));

    // dodanie WMa
    WindowManager* windowManager = new WindowManager(viewer, float(traits->width), float(traits->height), 0xFF/*, WindowManager::WM_PICK_DEBUG*/);
    viewer->setSceneData(windowManager);
    viewer->addEventHandler( new osgWidget::MouseHandler(windowManager) );
    viewer->addEventHandler( new osgWidget::ResizeHandler(windowManager, camera) );

    // dodanie obszaru roboczego
    workspace = new osgWidget::Box();
    workspace->getBackground()->setImage(getResourceString("images/transparent_background.png"), false, false);
    osg::Texture* texture = osgui::getTexture(workspace->getBackground());
	if (texture) {
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	}
    windowManager->addChild(workspace);

    // dodanie widgetu
    widget = new Widget("video");
	widget->setMinimumSize(50, 50);
    //widget->setUpdateCallback( new WidgetUpdater(this) );

    // ratio keeper
    ratioKeeper = new AspectRatioKeeper(widget, 1);
    workspace->addWidget(ratioKeeper);
    
    Refresher refresher = { this };
    workspace->addEventCallback(createEventCallback(osgGA::GUIEventAdapter::RESIZE, refresher));


    refresh(float(viewer->width()), float(viewer->height()));

#ifdef _DEBUG

    //dodajemy tez event handler ze statystykami
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->setTimerActive(true);

#endif

    viewer->setFocusPolicy(Qt::StrongFocus);

	viewer->setMinimumSize(50, 50);

    return viewer;
}

void VideoVisualizer::setUp( core::IObjectSource* source )
{
    reset();
}

QIcon* VideoVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/resources/icons/video.png"));
}

int VideoVisualizer::getMaxDataSeries() const
{
    return 1;
}

core::IVisualizer::TimeSerieBase* VideoVisualizer::createSerie(const ObjectWrapperConstPtr & data, const std::string & name)
{
    VideoSerie* ret = new VideoSerie(this);

    ret->setName(name);
    ret->setData(data);

    return ret;
}

core::IVisualizer::TimeSerieBase* VideoVisualizer::createSerie(const core::IVisualizer::SerieBase * serie)
{
    return nullptr;
}

void VideoVisualizer::removeSerie(core::IVisualizer::SerieBase* serie)
{
    reset();
}

void VideoVisualizer::reset()
{
	streamImage = nullptr;
	stream = nullptr;
	ratioKeeper->setTarget(nullptr);

	prevStreamTime = currentStreamTime = -1;

	prevStreamWidth = -1;

	refresh(viewer->width(), viewer->height());
}

QPixmap VideoVisualizer::print() const
{
    QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
    QRect widgetRect = viewer->geometry();
    widgetRect.moveTopLeft(viewer->parentWidget()->mapToGlobal(widgetRect.topLeft()));
    return pixmap.copy(widgetRect);
}
