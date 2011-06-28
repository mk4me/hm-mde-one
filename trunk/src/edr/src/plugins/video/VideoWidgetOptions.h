/********************************************************************
	created:  2010/12/14
	created:  14:12:2010   13:52
	filename: VideoWidgetOptions.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__
#define __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__

#include <osgui/QOsgWidgets.h>
#include "ui_VideoWidgetOptions.h"

class VideoService;

class VideoWidgetOptions : public QWidget, public Ui::VideoWidgetOptions
{
    Q_OBJECT
private:
    VideoService* service;

public:
    VideoWidgetOptions(VideoService* service);

public slots:
    void outputFormatChanged(int);
    void textureRectangleChecked(int);
};



#endif  // __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__