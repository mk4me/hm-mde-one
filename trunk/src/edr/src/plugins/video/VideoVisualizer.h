/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   16:01
    filename: VideoVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__
#define HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__

#include <osgViewer/Viewer>
#include <osgWidget/Widget>
#include <osgui/AspectRatioKeeper.h>
#include <core/IVisualizer.h>
#include <plugins/video/Wrappers.h>
#include <osgui/QOsgWidgets.h>
#include <osgui/OsgWidgetUtils.h>

//! Wizualizator wykres�w.
class VideoVisualizer : public core::IVisualizer
{
    struct Refresher;
    struct WidgetUpdater;

    UNIQUE_ID("{82978BE1-6A5F-4CE8-9CCE-302F82256A20}", "Video Visualizer");
private:
    //! Nazwa wizualizatora.
    std::string name;
    //! Viewer osg.
    osg::ref_ptr<osgui::QOsgDefaultWidget> viewer;
    //!
    VideoStreamConstPtr stream;
    //! Czy u�ywa� textureRect?
    bool useTextureRect;
    //!
    double prevStreamTime;
    //!
    int prevStreamWidth;

    //! Obiekt pilnuj�cy wsp�czynnik�w proporcji widget�w.
    osg::ref_ptr<osgui::AspectRatioKeeper> ratioKeeper;
    //! Obszar roboczy.
    osg::ref_ptr<osgWidget::Box> workspace;
    //! Widget w kt�rym rysujemy.
    osg::ref_ptr<osgWidget::Widget> widget;
    //! Bie��cy obrazek.
    osg::ref_ptr<vidlib::VideoImage> streamImage;

public:

    class VideoSerie : public core::IVisualizer::SerieBase
    {
    public:
        VideoSerie(VideoVisualizer * visualizer)
            : visualizer(visualizer)
        {

        }

    protected:

        virtual void setSerieName(const std::string & name)
        {

        }

        virtual void setSerieData(const core::ObjectWrapperConstPtr & data)
        {
            visualizer->reset();

            bool success = data->tryGet(visualizer->stream);

            // pobranie obrazkaa
            if ( success  == true && visualizer->stream != nullptr ) {
                visualizer->ratioKeeper->setTarget(visualizer->widget);
                visualizer->streamImage = visualizer->stream->getImage(vidlib::PixelFormatRGB24);
                visualizer->widget->setTexture( visualizer->stream->getTexture(visualizer->streamImage->getFormat(), visualizer->useTextureRect), true, visualizer->useTextureRect );
                osgui::correctTexCoords(visualizer->widget, visualizer->streamImage);
            }

            visualizer->refresh(visualizer->viewer->width(), visualizer->viewer->height());
        }

    private:
        VideoVisualizer * visualizer;
    };

public:
    //!
    VideoVisualizer();
    //!
    virtual ~VideoVisualizer();

public:
    //! 
    //! \param width
    //! \param height
    void refresh(float width, float height);
    //!
    void updateWidget();

public:
    //! \see IVisualizer::getName
    virtual const std::string& getName() const;
    //! \see IVisualizer::create
    virtual core::IVisualizer* createClone() const;
    //! \see IVisualizer::getSlotInfo
    virtual void getInputInfo(std::vector<core::IInputDescription::InputInfo>& info);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    //! \see IVisualizer::createIcon
    virtual QIcon* createIcon();
    //! \see IVisualizer::setUp
    virtual void setUp(core::IObjectSource* source);

    virtual int getMaxDataSeries() const;

    //! \return Seria danych ktora mozna ustawiac - nazwa i dane, nie zarzadza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
    virtual core::IVisualizer::SerieBase* createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());

    //! \param serie Seria danych do usuni�cia, nie powinien usuwac tej serii! Zarzadzamy nia my!!
    virtual void removeSerie(core::IVisualizer::SerieBase* serie);

    virtual void reset();
};


#endif  // HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__