#include "VideoPCH.h"
#include "VideoWidgetOptions.h"
#include <plugins/video/VideoService.h>
#include <plugins/video/core/PixelFormat.h>

Q_DECLARE_METATYPE(video::PixelFormat);

VideoWidgetOptions::VideoWidgetOptions( VideoService* service ) : service(service)
{
    setupUi(this);
//     outputFormatCombo->setItemData(0, qVariantFromValue(video::PixelFormatYV12) );
//     outputFormatCombo->setItemData(1, qVariantFromValue(video::PixelFormatRGB24) );
//     outputFormatCombo->setItemData(2, qVariantFromValue(video::PixelFormatBGRA) );
}

void VideoWidgetOptions::outputFormatChanged( int value )
{
    //service->setOutputFormat( outputFormatCombo->itemData(value).value<video::PixelFormat>() );
}

void VideoWidgetOptions::textureRectangleChecked( int checked )
{
    //service->setUseTextureRectangle(checked != 0);
}