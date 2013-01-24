/********************************************************************
    created:  2011/03/25
    created:  25:3:2011   16:01
    filename: VideoVisualizer.h
    author:	  Piotr Gwiazdowski
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__
#define HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__

#include <string>
#include <osgui/QOsgWidgets.h>

namespace osgui {

	class AspectRatioKeeper;
}

namespace osgWidget {

	class Box;
	class Widget;

}

namespace vidlib {

	class VideoImage;

}

#include <core/IVisualizer.h>
#include <plugins/video/Wrappers.h>

//! Wizualizator wykresów.
class VideoVisualizer : public plugin::IVisualizer
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
    VideoStreamPtr stream;
    //! Czy używać textureRect?
    bool useTextureRect;
    //!
    double prevStreamTime;

	double currentStreamTime;
    //!
    int prevStreamWidth;

    //! Obiekt pilnujący współczynników proporcji widgetów.
    osg::ref_ptr<osgui::AspectRatioKeeper> ratioKeeper;
    //! Obszar roboczy.
    osg::ref_ptr<osgWidget::Box> workspace;
    //! Widget w którym rysujemy.
    osg::ref_ptr<osgWidget::Widget> widget;
    //! Bieżący obrazek.
    osg::ref_ptr<vidlib::VideoImage> streamImage;

private:

	void refreshImage();

	bool getImage();

    class VideoSerie : public plugin::IVisualizer::ITimeSerieFeatures
    {
    public:
        VideoSerie(VideoVisualizer * visualizer);

		virtual ~VideoSerie();

        virtual void setName(const std::string & name);

        virtual const std::string & getName() const;

        virtual void setData(const core::ObjectWrapperConstPtr & data);

        virtual const core::ObjectWrapperConstPtr & getData() const;

        //! \return długość kanału w sekundach
        virtual double getLength() const;

        //! Czas zawiera się między 0 a getLength()
        //! \param time Aktualny, lokalny czas kanału w sekundach
        virtual void setTime(double time);

    private:
        VideoVisualizer * visualizer;
        std::string name;
        core::ObjectWrapperConstPtr data;
    };

    friend class VideoSerie;

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
    virtual plugin::IVisualizer* create() const;
    //! \see IVisualizer::getSlotInfo
    virtual void getInputInfo(std::vector<plugin::IInputDescription::InputInfo>& info);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget(core::IActionsGroupManager * manager);
    //! \see IVisualizer::createIcon
    virtual QIcon* createIcon();
    virtual QPixmap print() const;

    virtual int getMaxDataSeries() const;

    //! \return Seria danych która można ustawiac - nazwa i dane, nie zarządza ta seria danych - czasem jej zycia, my zwalniamy jej zasoby!!
    virtual plugin::IVisualizer::ITimeSerieFeatures* createSerie(const core::ObjectWrapperConstPtr & data, const std::string & name = std::string());

    virtual plugin::IVisualizer::ITimeSerieFeatures* createSerie(const plugin::IVisualizer::ISerie * serie);

    //! \param serie Seria danych do usunięcia, nie powinien usuwać tej serii! Zarządzamy nią my!!
    virtual void removeSerie(plugin::IVisualizer::ISerie* serie);

    virtual void reset();
};


#endif  // HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__
