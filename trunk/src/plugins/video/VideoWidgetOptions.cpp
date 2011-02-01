#include "VideoPCH.h"
#include "VideoWidgetOptions.h"
#include <plugins/video/VideoService.h>
#include <vidlib/PixelFormat.h>

Q_DECLARE_METATYPE(vidlib::PixelFormat);

VideoWidgetOptions::VideoWidgetOptions( VideoService* service ) : service(service)
{
    setupUi(this);
//     outputFormatCombo->setItemData(0, qVariantFromValue(vidlib::PixelFormatYV12) );
//     outputFormatCombo->setItemData(1, qVariantFromValue(vidlib::PixelFormatRGB24) );
//     outputFormatCombo->setItemData(2, qVariantFromValue(vidlib::PixelFormatBGRA) );
}

void VideoWidgetOptions::outputFormatChanged( int value )
{
    //service->setOutputFormat( outputFormatCombo->itemData(value).value<vidlib::PixelFormat>() );
}

void VideoWidgetOptions::textureRectangleChecked( int checked )
{
    //service->setUseTextureRectangle(checked != 0);
}