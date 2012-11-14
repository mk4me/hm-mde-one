#include "VideoPCH.h"
#include "VideoWidgetOptions.h"
#include "VideoService.h"
#include <vidlib/PixelFormat.h>

Q_DECLARE_METATYPE(vidlib::PixelFormat);

VideoWidgetOptions::VideoWidgetOptions( VideoService* service ) : service(service)
{
    setupUi(this);
}

void VideoWidgetOptions::outputFormatChanged( int value )
{

}

void VideoWidgetOptions::textureRectangleChecked( int checked )
{

}
