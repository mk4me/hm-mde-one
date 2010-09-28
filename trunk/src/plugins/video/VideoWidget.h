/********************************************************************
	created:  2010/09/28
	created:  28:9:2010   14:46
	filename: VideoWidget.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGET_H__
#define __HEADER_GUARD__VIDEOWIDGET_H__

#include <core/QOSGWidget.h>
#include "ui_video.h"


class VideoWidget : public QWidget, Ui::VideoWidget
{
private:
    QOSGViewer* viewer;

public:
    VideoWidget()
    {
        setupUi(this); 
        viewer = new QOSGViewer(this, "OsgViewer");
        GLWidget->addWidget( viewer );
    }

    virtual ~VideoWidget()
    {
    }
};

#endif  // __HEADER_GUARD__VIDEOWIDGET_H__