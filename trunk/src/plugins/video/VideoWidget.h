/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   14:46
	filename: VideoWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGET_H__
#define __HEADER_GUARD__VIDEOWIDGET_H__

#include <vector>
#include <osg/ImageStream>
#include <osgViewer/Viewer>
#include <core/QOSGWidget.h>
#include "ui_video.h"


class VideoWidget : public QWidget, Ui::VideoWidget
{
private:
    QOSGViewer* viewer;

public:
    VideoWidget();

    virtual ~VideoWidget()
    {
    }

    static void configureView(bool vertically, bool horizontally,
      std::vector<osg::Image*> &images, std::vector<osg::ImageStream*> &streams, 
      osgViewer::Viewer &viewer, bool textureRect);
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__