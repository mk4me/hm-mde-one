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

//! Wizualizator wykresów.
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
    //! Czy u¿ywaæ textureRect?
    bool useTextureRect;
    //!
    double prevStreamTime;
    //!
    int prevStreamWidth;

    //! Obiekt pilnuj¹cy wspó³czynników proporcji widgetów.
    osg::ref_ptr<osgui::AspectRatioKeeper> ratioKeeper;
    //! Obszar roboczy.
    osg::ref_ptr<osgWidget::Box> workspace;
    //! Widget w którym rysujemy.
    osg::ref_ptr<osgWidget::Widget> widget;
    //! Bie¿¹cy obrazek.
    osg::ref_ptr<vidlib::VideoImage> streamImage;


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
    virtual core::IVisualizer* create() const;
    //! \see IVisualizer::getSlotInfo
    virtual void getInputInfo(int source, core::IInputDescription::InputInfo& info);
    //! Nic nie robi.
    //! \see IVisualizer::update
    virtual void update(double deltaTime);
    //! \see IVisualizer::createWidget
    virtual QWidget* createWidget(std::vector<QObject*>& actions);
    //! \see IVisualizer::createIcon
    virtual QIcon* createIcon();
    //! \see IVisualizer::setUp
    virtual void setUp(core::IObjectSource* source);
};


#endif  // HEADER_GUARD_VIDEO__VIDEOVISUALIZER_H__