/********************************************************************
	created:  2010/12/14
	created:  14:12:2010   13:52
	filename: VideoWidgetOptions.h
	author:	  Piotr Gwiazdowski
	
	purpose:  
*********************************************************************/
#ifndef __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__
#define __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__

#include <stdint.h>
#include <map>
#include <vector>
#include <osg/ImageStream>
#include <osgViewer/Viewer>
#include <core/MultiView.h>
#include <core/QOSGWidget.h>
#include "ui_VideoWidgetOptions.h"
#include "StreamsViewOSGWidget.h"


class VideoWidgetOptions : public QWidget, public Ui::VideoWidgetOptions
{
public:
    //!
    VideoWidgetOptions()
    {
        setupUi(this);
    }
};



#endif  // __HEADER_GUARD__VIDEOWIDGETOPTIONS_H__