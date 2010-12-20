/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   14:46
	filename: VideoWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGET_H__
#define __HEADER_GUARD__VIDEOWIDGET_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <osg/ImageStream>
#include <osgViewer/Viewer>
#include <core/MultiView.h>
#include <core/QOSGWidget.h>
#include "ui_video.h"
#include "StreamsViewOSGWidget.h"
#include <plugins/video/core/PixelFormat.h>

class VideoWidget : public QWidget, Ui::VideoWidget
{
    Q_OBJECT
public:
    //! Lista strumieni.
    typedef std::vector<osg::ref_ptr<osg::Image> > ImagesList;

private:
    //! Obrazki pogrupowane wg. po�o�enia.
    ImagesList images;
    //! Widget z widokiem OSG.
    QOSGViewer* viewer;
    //! Multi view.
    osg::ref_ptr<core::MultiView> multiView;
    //!
    osg::ref_ptr<OsgWidgetStreamHelper> streamHelper;

public:
    //! Konstruktor inicjalizujacu UI.
    VideoWidget();

    virtual ~VideoWidget()
    {
    }

    void init( std::vector<std::string> &files );

    
    void configureView(int rows, int columns, ImagesList & images);

public:

    void setPixelFormat(vm::PixelFormat format);
    void setUseTextureRect(bool useTextureRect);

    //! \return
    inline VideoWidget::ImagesList& getImages()
    { 
        return images;
    }

    //! \return
    QOSGViewer* getViewer() const
    { 
        return viewer;
    }

private:
    //! Tworzy scen�.
    void createScene();
    //! Czy�ci scen�.
    void clearScene();
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__