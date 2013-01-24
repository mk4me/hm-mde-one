#include "VideoPCH.h"
#include "VideoVisualizer.h"

#include <osgWidget/Widget>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgWidget/Box>

#include <osgViewer/Viewer>

#include <osgui/AspectRatioKeeper.h>
#include <osgui/OsgWidgetUtils.h>
#include <osgui/EventCallback.h>

#include <vidlib/osg/VideoImageStream.h>

#include <core/StringTools.h>
#include <core/PluginCommon.h>

#include <QtGui/QIcon>

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


VideoVisualizer::VideoSerie::VideoSerie(VideoVisualizer * visualizer)
	: visualizer(visualizer)
{

}

VideoVisualizer::VideoSerie::~VideoSerie()
{

}

void VideoVisualizer::VideoSerie::setName(const std::string & name)
{
	this->name = name;
}

const std::string & VideoVisualizer::VideoSerie::getName() const
{
	return name;
}

void VideoVisualizer::VideoSerie::setData(const core::ObjectWrapperConstPtr & data)
{
	this->data = data;
	visualizer->reset();
	bool success = false;
	if (data->isSupported(typeid(VideoStreamPtr))) {
		success = data->clone()->tryGet(visualizer->stream);
	} else if (data->isSupported(typeid(VideoChannel))) {
		VideoChannelPtr channel = data->clone()->get();
		if (channel) {
			visualizer->stream = channel->getVideoStream();
			success = visualizer->stream != nullptr;
		}
	}

	// pobranie obrazka
	if ( success  == true && visualizer->stream != nullptr ) {
		visualizer->ratioKeeper->setTarget(visualizer->widget);
		if(visualizer->getImage() == true){
			visualizer->refreshImage();
			//! Fix pierwszej ramki - wymuszam poprawny resize okienek OSG!!
			visualizer->viewer->getEventQueue()->windowResize(0, 0, visualizer->viewer->width(), visualizer->viewer->height());
			visualizer->viewer->frame();
		}
	}else{

		visualizer->refresh(visualizer->viewer->width(), visualizer->viewer->height());
	}
}

const core::ObjectWrapperConstPtr & VideoVisualizer::VideoSerie::getData() const
{
	return data;
}

double VideoVisualizer::VideoSerie::getLength() const
{
	return visualizer->stream->getDuration();
}

void VideoVisualizer::VideoSerie::setTime(double time)
{
	visualizer->currentStreamTime = time;
}

VideoVisualizer::VideoVisualizer() :
name("Video"), useTextureRect(true), prevStreamTime(-1), currentStreamTime(-1), prevStreamWidth(-1)
{

}

VideoVisualizer::~VideoVisualizer()
{
    viewer = nullptr;
}

const std::string& VideoVisualizer::getName() const
{
    return name;
}

plugin::IVisualizer* VideoVisualizer::create() const
{
    return new VideoVisualizer();
}

void VideoVisualizer::getInputInfo( std::vector<plugin::IInputDescription::InputInfo>& info )
{
    plugin::IInputDescription::InputInfo input;

    input.name = "videoStream";
    input.type = typeid(VideoStream);
    input.modify = false;
    input.required = false;

    info.push_back(input);

	input.name = "videoChannel";
	input.type = typeid(VideoChannel);
	info.push_back(input);

    input.name = "picture";
    input.type = typeid(vidlib::Picture);

    info.push_back(input);
}

void VideoVisualizer::refresh( float width, float height )
{
    // aktualizacja współczynnika proporcji
    if ( streamImage ) {
        ratioKeeper->setAspectRatio( streamImage->getPixelAspectRatio() * streamImage->s() / streamImage->t() );
    }

    // aktualizacja rozmiaru tła
    workspace->setOrigin(0, 0);
    workspace->resize(width , height);
    workspace->update();
    // aktualizacja tekstury tła
    //workspace->getBackground()->setTexCoordRegion(0, height, width, -height);
	workspace->getBackground()->setTexCoordRegion(0, 0, width, height);

    // aktualizacja rozmiaru tekstury
    if ( streamImage ) {
        // odświeżenia danych zależnych od obrazka
        // resize obrazka
        streamImage->setMaxWidth(widget, widget->getWidth());
		if(width > 0 && height > 0){
			workspace->resizeAdd();
		}
    }
}

void VideoVisualizer::updateWidget()
{
    // korekcja współrzędnych tekstury
    if ( streamImage ) {
        if ( useTextureRect ) {
            // sprawdzamy, czy trzeba odświeżyć współrzędne tekstury
            float s = static_cast<float>(streamImage->s());
            float t = static_cast<float>(streamImage->t());
            //
            auto ll = widget->getTexCoord(osgWidget::Widget::LOWER_LEFT);
            auto ur = widget->getTexCoord(osgWidget::Widget::UPPER_RIGHT);
            float oldS = fabs( ll.x() - ur.x() );
            float oldT = fabs( ll.y() - ur.y() );

			//float h = widget->getHeight();
			//float w = widget->getWidth();

			//float wR = w / h;
			//float tR = s / t;

			//float mul = wR / tR;

			//float nextS = s;
			//float nextT = t;
			//if (mul > 1.0f) {
				//nextT = s * h / w;
			//} else {
				//nextS = w * t / h;
			//}

			//float deltaS = (s - nextS) / 2.0f;
			//float deltaT = (t - nextT) / 2.0f;

            // czy trzeba aktualizować?
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
	viewer->setMinimumSize(50,50);
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
    workspace->getBackground()->setImage(plugin::getResourcePath("images/transparent_background.png").string(), false, false);
    osg::Texture* texture = osgui::getTexture(workspace->getBackground());
	if (texture) {
		texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
		texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	}
    windowManager->addChild(workspace);

    // dodanie widgetu
    widget = new Widget("video");
    widget->setUpdateCallback( new WidgetUpdater(this) );

    // ratio keeper
    ratioKeeper = new AspectRatioKeeper(widget, 1);
    workspace->addWidget(ratioKeeper);
	widget->setMinimumSize(50, 50);

    Refresher refresher = { this };
    workspace->addEventCallback(createEventCallback(osgGA::GUIEventAdapter::RESIZE, refresher));


    refresh(float(viewer->width()), float(viewer->height()));

#ifdef _DEBUG

    //dodajemy tez event handler ze statystykami
    viewer->addEventHandler( new osgViewer::StatsHandler() );
    viewer->setTimerActive(true);

#endif

    viewer->setFocusPolicy(Qt::StrongFocus);

    return viewer;
}

QIcon* VideoVisualizer::createIcon()
{
    return new QIcon(QString::fromUtf8(":/resources/icons/video.png"));
}

int VideoVisualizer::getMaxDataSeries() const
{
    return 1;
}

plugin::IVisualizer::ITimeSerieFeatures* VideoVisualizer::createSerie(const ObjectWrapperConstPtr & data, const std::string & name)
{
    VideoSerie* ret = new VideoSerie(this);

    ret->setName(name);
    ret->setData(data);

    return ret;
}

plugin::IVisualizer::ITimeSerieFeatures* VideoVisualizer::createSerie(const plugin::IVisualizer::ISerie * serie)
{
    return nullptr;
}

void VideoVisualizer::removeSerie(plugin::IVisualizer::ISerie* serie)
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
	/*QPixmap pixmap = QPixmap::grabWindow(QApplication::desktop()->winId());
	QRect widgetRect = viewer->geometry();
	widgetRect.moveTopLeft(viewer->parentWidget()->mapToGlobal(widgetRect.topLeft()));
	return pixmap.copy(widgetRect);*/
	return QPixmap::fromImage(viewer->grabFrameBuffer(true));
}
